/*
 * ============================================================
 *  GAS LEAKAGE DETECTOR — FreeRTOS
 *  ESP32 + MQ2 + Buzzer + 2 LEDs
 * ============================================================
 *  Wiring:
 *  ESP32 3.3V  → MQ2 VCC
 *  ESP32 GND   → MQ2 GND, Buzzer(-), 2 LED
 *  ESP32 D34   → MQ2 AOUT (analog)
 *  ESP32 D18   → Buzzer (+)
 *  ESP32 D17   → Green LED (+ 220Ω)
 *  ESP32 D16   → Red LED   (+ 220Ω)
 * ============================================================
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define MQ2_AO_PIN    34
#define BUZZER_PIN    18
#define LED_GREEN     17
#define LED_RED       16

#define THRESHOLD     3000
#define WARMUP_MS     5000
#define BAUD_RATE     115200

// Shared data struct
struct GasData {
  int   raw;
  float voltage;
  bool  detected;
};

// Queue handle 
QueueHandle_t gasQueue;

// Task 1: GasTask (Priority 3 — highest)
// baca sensor MQ-2, kontrol LED + Buzzer, kirim data ke queue
void gasTask(void *pvParameters) {
  while (true) {
    GasData data;
    data.raw      = analogRead(MQ2_AO_PIN);
    data.voltage  = data.raw * (3.3f / 4095.0f);
    data.detected = (data.raw > THRESHOLD);
    if (data.detected) {
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_RED,   HIGH);
      tone(BUZZER_PIN, 2000);
      vTaskDelay(pdMS_TO_TICKS(300));
      noTone(BUZZER_PIN);
      vTaskDelay(pdMS_TO_TICKS(300));
    } else {
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_RED,   LOW);
      noTone(BUZZER_PIN);
      vTaskDelay(pdMS_TO_TICKS(100));
    }
    // Kirim ke queue
    xQueueOverwrite(gasQueue, &data);
  }
}

// Task 2: StatusTask (Priority 1 — lowest) 
// Baca queue, Print ke Serial Monitor
void statusTask(void *pvParameters) {
  GasData data;
  while (true) {
    if (xQueuePeek(gasQueue, &data, pdMS_TO_TICKS(500)) == pdTRUE) {
      Serial.print("[StatusTask] RAW: ");
      Serial.print(data.raw);
      Serial.print("  |  ");
      Serial.print(data.voltage, 2);
      Serial.print("V  |  >> ");
      Serial.println(data.detected ? "*** GAS DETECTED ***" : "Normal");
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

// Setup 
void setup() {
  Serial.begin(BAUD_RATE);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_GREEN,  OUTPUT);
  pinMode(LED_RED,    OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_GREEN,  LOW);
  digitalWrite(LED_RED,    LOW);

  // Warmup sensor
  Serial.println("Warming up sensor...");
  delay(WARMUP_MS);
  digitalWrite(LED_GREEN, HIGH);
  Serial.println("Ready.");
  Serial.println("------------------------------");

  // Buat queue
  gasQueue = xQueueCreate(1, sizeof(GasData));

  // Buat Task
  xTaskCreate(gasTask,    "GasTask",    2048, NULL, 3, NULL);
  xTaskCreate(statusTask, "StatusTask", 2048, NULL, 1, NULL);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
