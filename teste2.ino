/*
  ============================================================
  Osci_ESP32 - Osciloscópio Web (Modo 8-bit Turbo a 600 kHz)
  Decimação Dinâmica de Taxa DMA + Trigger no firmware (Etapa 1)
  ------------------------------------------------------------
  - Detector de trigger roda na adcTask em 12 bits.
  - Só registra bordas na janela VÁLIDA do buffer: precisa ter
    PRE amostras antes e POST amostras depois da borda.
  - Cabeçalho de 5 bytes:
      [0] = escala (1..5)
      [1] = bateria (0..255)
      [2] = posição da borda (byte baixo)
      [3] = bit7 = flag "borda encontrada", bits0-3 = nibble alto
      [4] = fração sub-amostra (0..255)
  - Comandos: start / stop / cap:N / rate:HZ / dac:N
              trig:LEVEL,HYST,ENABLED,PRE,POST
  - Frequências DAC suportadas: 122, 400, 1000, 2000, 5000, 10000 Hz
  ============================================================
*/

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <driver/i2s.h>
#include <driver/adc.h>
#include <DacESP32.h>
#include "index2.h"

const char* AP_SSID     = "Osci_ESP32";
const char* AP_PASSWORD = NULL;

#define ADC_CHANNEL           ADC1_CHANNEL_0    // GPIO36
#define MAX_SAMPLE_RATE       600000
#define MIN_SAMPLE_RATE       1000
#define DMA_BUF_COUNT         4
#define DMA_BUF_LEN           256
#define MAX_DATA_SAMPLES      4096
#define HEADER_SIZE           5
#define MAX_ADC_BUFFER_SIZE   (MAX_DATA_SAMPLES + HEADER_SIZE)
#define WS_PORT               81

#define BATTERY_ADC_CHANNEL   ADC1_CHANNEL_6
#define BATTERY_READ_INTERVAL 60000

const uint32_t DAC_DEFAULT_FREQ = 5000;

const uint8_t SCALE_PINS[5] = { 13, 14, 27, 32, 33 };

DacESP32 dac1(DAC_CHAN_0);
WebServer server(80);
WebSocketsServer webSocket(WS_PORT);

bool streaming = true;

volatile uint16_t targetDataSamples = 3512;
volatile uint32_t targetSampleRate  = MAX_SAMPLE_RATE;
volatile uint8_t  batteryRaw8       = 0;

// ------- Config de trigger (atualizada pelo cliente) -------
volatile uint8_t  triggerLevel   = 128;
volatile uint8_t  triggerHyst    = 8;
volatile bool     triggerEnabled = false;
volatile uint16_t trigPre        = 0;   // amostras antes da borda
volatile uint16_t trigPost       = 0;   // amostras depois da borda

volatile uint32_t dacFreqRequested = DAC_DEFAULT_FREQ;
uint32_t dacFreqActive = DAC_DEFAULT_FREQ;

enum BufferState : uint8_t { BUFFER_EMPTY, BUFFER_FILLING, BUFFER_READY, BUFFER_SENDING };

uint8_t  adcBuffers[2][MAX_ADC_BUFFER_SIZE];
uint16_t bufferDataLen[2] = {0, 0};
BufferState bufferState[2] = { BUFFER_EMPTY, BUFFER_EMPTY };
uint8_t fillingBuffer = 0;

SemaphoreHandle_t bufferMutex;
SemaphoreHandle_t bufferChanged;

uint8_t readScalePosition() {
  for (uint8_t position = 0; position < 5; ++position) {
    if (digitalRead(SCALE_PINS[position]) == LOW) return position + 1;
  }
  return 1;
}

uint8_t readBatteryRaw8() {
  int sum = 0;
  for (int i = 0; i < 8; i++) sum += adc1_get_raw(BATTERY_ADC_CHANNEL);
  int raw12 = sum / 8;
  const int FULL_SCALE_FOR_2V1 = 2606;
  if (raw12 > FULL_SCALE_FOR_2V1) raw12 = FULL_SCALE_FOR_2V1;
  return (uint8_t)((raw12 * 255) / FULL_SCALE_FOR_2V1);
}

uint8_t safeReadBatteryRaw8() {
  i2s_adc_disable(I2S_NUM_0);
  uint8_t v = readBatteryRaw8();
  i2s_adc_enable(I2S_NUM_0);
  return v;
}

void setupI2S_ADC() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
    .sample_rate = MAX_SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = DMA_BUF_COUNT,
    .dma_buf_len = DMA_BUF_LEN,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_adc_mode(ADC_UNIT_1, ADC_CHANNEL);
  adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11);
  i2s_adc_enable(I2S_NUM_0);
}

void adcTask(void*) {
  int16_t dmaSamples[DMA_BUF_LEN];
  size_t bytesRead = 0;
  size_t samplesInBuffer = HEADER_SIZE;
  uint32_t activeSampleRate = MAX_SAMPLE_RATE;

  bool     trigArmed    = false;
  uint8_t  trigLowCount = 0;
  uint16_t trigPrev12   = 0;
  bool     trigHasPrev  = false;
  int32_t  edgePosData  = -1;
  uint8_t  edgeFrac8    = 0;

  xSemaphoreTake(bufferMutex, portMAX_DELAY);
  bufferState[fillingBuffer] = BUFFER_FILLING;
  xSemaphoreGive(bufferMutex);

  for (;;) {
    if (i2s_read(I2S_NUM_0, dmaSamples, sizeof(dmaSamples), &bytesRead, portMAX_DELAY) != ESP_OK) {
      continue;
    }

    uint32_t requestedRate = targetSampleRate;
    if (requestedRate != activeSampleRate) {
      i2s_set_sample_rates(I2S_NUM_0, requestedRate);
      activeSampleRate = requestedRate;
    }

    // Snapshot de config para este bloco DMA
    const uint8_t  tLevel   = triggerLevel;
    const uint8_t  tHyst    = triggerHyst;
    const bool     tEnabled = triggerEnabled;
    const uint16_t tPre     = trigPre;
    const uint16_t tPost    = trigPost;
    const uint16_t trig12   = ((uint16_t)tLevel) << 4;
    const uint16_t arm12    = (tLevel > tHyst)
                              ? (((uint16_t)(tLevel - tHyst)) << 4)
                              : 0;
    uint8_t minLow = (uint8_t)(tHyst + 1);
    if (minLow < 2) minLow = 2;
    if (minLow > 8) minLow = 8;

    uint16_t currentTarget = targetDataSamples;
    if (currentTarget < 64) currentTarget = 64;
    if (currentTarget > MAX_DATA_SAMPLES) currentTarget = MAX_DATA_SAMPLES;
    const size_t fillTarget = HEADER_SIZE + currentTarget;

    // Limite superior válido da posição da borda (índice do sample ANTES da borda)
    const int32_t maxEdgePos = (int32_t)currentTarget - (int32_t)tPost;

    const size_t sampleCount = bytesRead / sizeof(int16_t);
    for (size_t index = 0; index < sampleCount; ++index) {
      const uint16_t v12 = dmaSamples[index] & 0x0FFF;

      // ---- Detector de trigger (12 bits) ----
      if (tEnabled) {
        if (v12 <= arm12) {
          trigLowCount++;
          if (trigLowCount >= minLow) trigArmed = true;
        } else {
          trigLowCount = 0;
        }

        if (trigArmed && v12 >= trig12 && trigHasPrev && trigPrev12 < trig12) {
          const int32_t dataIdx = (int32_t)samplesInBuffer - (int32_t)HEADER_SIZE;
          const int32_t edgePos = dataIdx - 1;    // índice do sample ANTES da borda

          // Só registra se a borda está na janela válida:
          //   edgePos >= tPre            (tem pré-trigger suficiente)
          //   edgePos <= maxEdgePos      (tem pós-trigger suficiente)
          if (edgePos >= (int32_t)tPre && edgePos <= maxEdgePos) {
            edgePosData = edgePos;
            const uint16_t denom = v12 - trigPrev12;
            if (denom > 0) {
              uint32_t num  = (uint32_t)(trig12 - trigPrev12) * 256u;
              uint32_t frac = num / denom;
              if (frac > 255) frac = 255;
              edgeFrac8 = (uint8_t)frac;
            } else {
              edgeFrac8 = 0;
            }
          }
          trigArmed    = false;
          trigLowCount = 0;
        }
        trigPrev12  = v12;
        trigHasPrev = true;
      }

      adcBuffers[fillingBuffer][samplesInBuffer++] = (uint8_t)(v12 >> 4);

      if (samplesInBuffer < fillTarget) continue;

      // ---- Buffer completo ----
      const uint8_t completedBuffer = fillingBuffer;
      const uint8_t nextBuffer      = completedBuffer ^ 1U;

      adcBuffers[completedBuffer][0] = readScalePosition();
      adcBuffers[completedBuffer][1] = batteryRaw8;

      if (edgePosData >= 0) {
        const uint16_t p = (uint16_t)edgePosData;
        adcBuffers[completedBuffer][2] = (uint8_t)(p & 0xFF);
        adcBuffers[completedBuffer][3] = (uint8_t)(((p >> 8) & 0x0F) | 0x80);
        adcBuffers[completedBuffer][4] = edgeFrac8;
      } else {
        adcBuffers[completedBuffer][2] = 0;
        adcBuffers[completedBuffer][3] = 0;
        adcBuffers[completedBuffer][4] = 0;
      }

      bufferDataLen[completedBuffer] = samplesInBuffer - HEADER_SIZE;

      samplesInBuffer = HEADER_SIZE;
      trigArmed    = false;
      trigLowCount = 0;
      trigHasPrev  = false;
      edgePosData  = -1;
      edgeFrac8    = 0;

      xSemaphoreTake(bufferMutex, portMAX_DELAY);
      bufferState[completedBuffer] = BUFFER_READY;
      xSemaphoreGive(bufferMutex);
      xSemaphoreGive(bufferChanged);

      for (;;) {
        xSemaphoreTake(bufferMutex, portMAX_DELAY);
        if (bufferState[nextBuffer] == BUFFER_EMPTY) {
          bufferState[nextBuffer] = BUFFER_FILLING;
          fillingBuffer = nextBuffer;
          xSemaphoreGive(bufferMutex);
          break;
        }
        xSemaphoreGive(bufferMutex);
        xSemaphoreTake(bufferChanged, portMAX_DELAY);
      }
    }
  }
}

bool claimReadyBuffer(uint8_t& bufferIndex) {
  if (xSemaphoreTake(bufferMutex, 0) != pdTRUE) return false;
  for (uint8_t index = 0; index < 2; ++index) {
    if (bufferState[index] == BUFFER_READY) {
      bufferState[index] = BUFFER_SENDING;
      bufferIndex = index;
      xSemaphoreGive(bufferMutex);
      return true;
    }
  }
  xSemaphoreGive(bufferMutex);
  return false;
}

void releaseBuffer(uint8_t bufferIndex) {
  xSemaphoreTake(bufferMutex, portMAX_DELAY);
  bufferState[bufferIndex] = BUFFER_EMPTY;
  xSemaphoreGive(bufferMutex);
  xSemaphoreGive(bufferChanged);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Desconectado\n", num);
      break;
    case WStype_CONNECTED:
      Serial.printf("[%u] Conectado de %s\n", num, webSocket.remoteIP(num).toString().c_str());
      break;
    case WStype_TEXT: {
      String cmd = String((char*)payload, length);
      if (cmd == "start") {
        streaming = true;
      } else if (cmd == "stop") {
        streaming = false;
      } else if (cmd.startsWith("cap:")) {
        int val = cmd.substring(4).toInt();
        if (val < 64) val = 64;
        if (val > MAX_DATA_SAMPLES) val = MAX_DATA_SAMPLES;
        targetDataSamples = (uint16_t)val;
      } else if (cmd.startsWith("rate:")) {
        uint32_t val = (uint32_t)cmd.substring(5).toInt();
        if (val < MIN_SAMPLE_RATE) val = MIN_SAMPLE_RATE;
        if (val > MAX_SAMPLE_RATE) val = MAX_SAMPLE_RATE;
        targetSampleRate = val;
      } else if (cmd.startsWith("dac:")) {
        uint32_t val = (uint32_t)cmd.substring(4).toInt();
        // Frequências suportadas: 122, 400, 1000, 2000, 5000, 10000 Hz
        if (val == 122 || val == 400 || val == 1000 || val == 2000 ||
            val == 5000 || val == 10000) {
          dacFreqRequested = val;
        }
      } else if (cmd.startsWith("trig:")) {
        // Formato: "trig:LEVEL,HYST,ENABLED,PRE,POST"
        int p1 = cmd.indexOf(',', 5);
        int p2 = (p1 > 0) ? cmd.indexOf(',', p1 + 1) : -1;
        int p3 = (p2 > 0) ? cmd.indexOf(',', p2 + 1) : -1;
        int p4 = (p3 > 0) ? cmd.indexOf(',', p3 + 1) : -1;

        if (p1 > 0 && p2 > 0 && p3 > 0 && p4 > 0) {
          int lv = cmd.substring(5, p1).toInt();
          int hy = cmd.substring(p1 + 1, p2).toInt();
          int en = cmd.substring(p2 + 1, p3).toInt();
          int pr = cmd.substring(p3 + 1, p4).toInt();
          int po = cmd.substring(p4 + 1).toInt();
          if (lv < 0) lv = 0; if (lv > 255) lv = 255;
          if (hy < 1) hy = 1; if (hy > 30) hy = 30;
          if (pr < 0) pr = 0;
          if (po < 0) po = 0;
          triggerLevel   = (uint8_t)lv;
          triggerHyst    = (uint8_t)hy;
          triggerEnabled = (en != 0);
          trigPre        = (uint16_t)pr;
          trigPost       = (uint16_t)po;
        } else if (p1 > 0) {
          // Retrocompatível: "trig:LEVEL,HYST,ENABLED"
          int lv = cmd.substring(5, p1).toInt();
          int hy = (p2 > 0) ? cmd.substring(p1 + 1, p2).toInt() : (int)triggerHyst;
          int en = (p2 > 0) ? cmd.substring(p2 + 1).toInt() : 1;
          if (lv < 0) lv = 0; if (lv > 255) lv = 255;
          if (hy < 1) hy = 1; if (hy > 30) hy = 30;
          triggerLevel   = (uint8_t)lv;
          triggerHyst    = (uint8_t)hy;
          triggerEnabled = (en != 0);
        }
      }
      break;
    }
    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n=== Osci_ESP32 Web (8-bit Turbo + Trigger no firmware) ===");

  for (uint8_t pin : SCALE_PINS) pinMode(pin, INPUT_PULLUP);

  adc1_config_channel_atten(BATTERY_ADC_CHANNEL, ADC_ATTEN_DB_11);

  dacFreqActive = dacFreqRequested;
  dac1.outputCW(dacFreqActive);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);

  server.on("/", []() { server.send_P(200, "text/html", index_html); });
  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  batteryRaw8 = readBatteryRaw8();

  setupI2S_ADC();
  bufferMutex   = xSemaphoreCreateMutex();
  bufferChanged = xSemaphoreCreateBinary();
  xTaskCreatePinnedToCore(adcTask, "ADC-DMA", 4096, nullptr, 3, nullptr, 1);

  Serial.println("Servidor pronto em http://192.168.4.1");
}

void loop() {
  server.handleClient();
  webSocket.loop();

  if (dacFreqRequested != dacFreqActive) {
    dacFreqActive = dacFreqRequested;
    dac1.outputCW(dacFreqActive);
    Serial.printf("DAC ajustado para %u Hz\n", (unsigned)dacFreqActive);
  }

  static uint32_t lastBattRead = 0;
  if (millis() - lastBattRead >= BATTERY_READ_INTERVAL) {
    lastBattRead = millis();
    batteryRaw8 = safeReadBatteryRaw8();
  }

  if (!streaming) { delay(1); return; }

  uint8_t readyBuffer = 0;
  if (webSocket.connectedClients() > 0 && claimReadyBuffer(readyBuffer)) {
    size_t sendLen = HEADER_SIZE + bufferDataLen[readyBuffer];
    webSocket.broadcastBIN(adcBuffers[readyBuffer], sendLen);
    releaseBuffer(readyBuffer);
  }
}