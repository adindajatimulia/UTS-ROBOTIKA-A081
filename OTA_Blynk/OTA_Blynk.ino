
#define BLYNK_TEMPLATE_ID "TMPL682tShCYu"
#define BLYNK_TEMPLATE_NAME "iMCLab Blynk"
#define BLYNK_AUTH_TOKEN "U3wgvSDWUDkRTaLdoEiSTd2wFIvN1J5q"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ArduinoOTA.h>

// Wi-Fi
char ssid[] = "BAGUSMOTOR";
char pass[] = "kamispahing";

// Pin ESP32
const int ledPin = 2;
const int motorIn1 = 27;
const int motorIn2 = 26;
const int motorEnable = 12;

// Versi program
const char* versiProgram = "Versi 3 - Blynk OTA Blink";

int kecepatan = 0;
bool motorAktif = false;
bool statusLed = false;

BlynkTimer timer;

// Deklarasi fungsi
void aturMotor();
void blinkLED();

// Switch Blynk: V0
BLYNK_WRITE(V0) {
  motorAktif = param.asInt();
  aturMotor();
}

// Slider Blynk: V1
BLYNK_WRITE(V1) {
  kecepatan = param.asInt();
  aturMotor();
}

// Saat Blynk tersambung
BLYNK_CONNECTED() {
  Serial.println("Blynk terhubung!");
  Blynk.syncVirtual(V0, V1);
}

// Kontrol motor
void aturMotor() {
  if (motorAktif && kecepatan > 0) {
    digitalWrite(motorIn1, HIGH);
    digitalWrite(motorIn2, LOW);
    ledcWrite(motorEnable, kecepatan);
  } else {
    ledcWrite(motorEnable, 0);
    digitalWrite(motorIn1, LOW);
    digitalWrite(motorIn2, LOW);
  }
}

// LED berkedip setiap 1 detik
void blinkLED() {
  statusLed = !statusLed;
  digitalWrite(ledPin, statusLed);

  Blynk.virtualWrite(V2, statusLed ? 1 : 0);
}

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);

  digitalWrite(ledPin, LOW);

  // PWM: 30 kHz, resolusi 8-bit
  if (!ledcAttach(motorEnable, 30000, 8)) {
    Serial.println("Gagal mengatur PWM motor!");
  }

  aturMotor();

  // Hubungkan Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  Serial.print("Menghubungkan Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("IP ESP32: ");
  Serial.println(WiFi.localIP());
  Serial.println(versiProgram);

  // OTA
  ArduinoOTA.setHostname("OTA Blynk");
  ArduinoOTA.setPassword("123");

  ArduinoOTA.onStart([]() {
    Serial.println("OTA: mulai upload");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA: upload selesai!");
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA error: %u\n", error);
  });

  ArduinoOTA.begin();
  Serial.println("OTA siap!");

  // Blink LED setiap 1000 ms
  timer.setInterval(1000L, blinkLED);

  // Hubungkan Blynk
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect(10000);
}

void loop() {
  Blynk.run();
  ArduinoOTA.handle();
  timer.run();
}
