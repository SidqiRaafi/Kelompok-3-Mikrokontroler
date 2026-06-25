/*
 * ============================================================
 *  GAS LEAKAGE DETECTOR — FreeRTOS + MQTT
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
#include <WiFi.h>
#include <PubSubClient.h>

// WiFi & MQTT config 
#define WIFI_SSID     "Wokwi-GUEST"
#define WIFI_PASS     ""
#define MQTT_BROKER   "broker.emqx.io"
#define MQTT_PORT     1883
#define MQTT_TOPIC    "gasleak/data"
#define MQTT_CLIENT   "esp32-gasleak-01"

// Pin & sensor config 
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

// Handles
QueueHandle_t gasQueue;
WiFiClient    wifiClient;
PubSubClient  mqttClient(wifiClient);

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <WiFi.h>
#include <WiFiClientSecure.h> // WAJIB untuk HiveMQ Cloud
#include <PubSubClient.h>

// WiFi & MQTT config
#define WIFI_SSID     "Wokwi-GUEST"
#define WIFI_PASS     ""
#define MQTT_BROKER   "a2b717741b664ec883fd2bf66d9eb680.s1.eu.hivemq.cloud"
#define MQTT_PORT     8883
#define MQTT_USER     "gass12"
#define MQTT_PASS     "GasBocor123"
#define MQTT_TOPIC    "gasleak/data"
#define MQTT_CLIENT   "esp32-gasleak-01"

// Pin & sensor config 
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

// Handles
QueueHandle_t gasQueue;
WiFiClientSecure wifiClient;
PubSubClient  mqttClient(wifiClient);

// WiFi connection
void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASS, 6);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    vTaskDelay(pdMS_TO_TICKS(500));
  }
  Serial.println("\nWiFi connected: " + WiFi.localIP().toString());
  
  // Mengabaikan pengecekan sertifikat SSL
  wifiClient.setInsecure();
}

// MQTT connection 
void connectMQTT() {
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  while (!mqttClient.connected()) {
    Serial.print("Connecting to HiveMQ Cloud...");
    
    // Gunakan Client ID yang diacak agar koneksi stabil
    String clientId = String(MQTT_CLIENT) + "-" + String(random(0xffff), HEX);
    
    // Menghubungkan menggunakan Username dan Password
    if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
      Serial.println("connected.");
    } else {
      Serial.print("failed, rc=");
      Serial.println(mqttClient.state());
      vTaskDelay(pdMS_TO_TICKS(2000));
    }
  }
}

// Task 1: GasTask (Priority 3 — highest)
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

    xQueueOverwrite(gasQueue, &data);
  }
}

// Task 2: StatusTask (Priority 1)
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

// Task 3: MQTTTask (Priority 2)
void mqttTask(void *pvParameters) {
  GasData data;
  
  while (true) {
    // Reconnect Network & MQTT secara aman di dalam FreeRTOS Loop
    if (WiFi.status() != WL_CONNECTED) {
      connectWiFi();
    }
    if (!mqttClient.connected()) {
      connectMQTT();
    }
    
    mqttClient.loop();

    if (mqttClient.connected()) {
      if (xQueuePeek(gasQueue, &data, pdMS_TO_TICKS(1000)) == pdTRUE) {
        // Build JSON payload
        char payload[128];
        snprintf(payload, sizeof(payload),
          "{\"raw\":%d,\"voltage\":%.2f,\"status\":\"%s\",\"task\":\"GasTask\"}",
          data.raw,
          data.voltage,
          data.detected ? "DANGER" : "SAFE"
        );

        mqttClient.publish(MQTT_TOPIC, payload);
        Serial.print("[MQTTTask] Published: ");
        Serial.println(payload);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(2000));  // publish setiap 2 sec
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

  Serial.println("Warming up sensor...");
  delay(WARMUP_MS);
  digitalWrite(LED_GREEN, HIGH);
  Serial.println("Ready.");
  Serial.println("------------------------------");

  gasQueue = xQueueCreate(1, sizeof(GasData));

  // Memperbesar Stack Size untuk MQTTTask karena WiFiClientSecure memakan banyak memori
  xTaskCreate(gasTask,    "GasTask",    2048, NULL, 3, NULL);
  xTaskCreate(statusTask, "StatusTask", 2048, NULL, 1, NULL);
  xTaskCreate(mqttTask,   "MQTTTask",   8192, NULL, 2, NULL); 
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}