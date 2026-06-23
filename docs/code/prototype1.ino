/*
 * ============================================================
 *  GAS LEAKAGE DETECTOR — Local Prototype
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

#define MQ2_AO_PIN    34
#define BUZZER_PIN    18
#define LED_GREEN     17
#define LED_RED       16

#define THRESHOLD     3000
#define WARMUP_MS     5000
#define BAUD_RATE     115200

void setup() {
  Serial.begin(BAUD_RATE);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_GREEN,  OUTPUT);
  pinMode(LED_RED,    OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_GREEN,  LOW);
  digitalWrite(LED_RED,    LOW);

  // Warmup Sensor
  Serial.println("Warming up sensor...");
  delay(WARMUP_MS);

  digitalWrite(LED_GREEN, HIGH);  // Green LED on - Standby
  Serial.println("Ready.");
  Serial.println("------------------------------");
}

void loop() {
  int   raw      = analogRead(MQ2_AO_PIN);
  float voltage  = raw * (3.3f / 4095.0f);
  bool  detected = (raw > THRESHOLD);

  // Serial Monitor output
  Serial.print("RAW: ");
  Serial.print(raw);
  Serial.print("  |  ");
  Serial.print(voltage, 2);
  Serial.print("V  |  >> ");
  Serial.println(detected ? "*** GAS DETECTED ***" : "Normal");

  // Gas Detection Alarm (Buzzer+Red LED)
  if (detected) {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED,   HIGH);
    tone(BUZZER_PIN, 2000);
    delay(300);
    noTone(BUZZER_PIN);
    delay(300);
  } else {
    digitalWrite(LED_GREEN,  HIGH);
    digitalWrite(LED_RED,    LOW);
    noTone(BUZZER_PIN);
    delay(500);
  }
}
