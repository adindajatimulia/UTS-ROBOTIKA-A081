
#define BLYNK_TEMPLATE_ID "TMPL682tShCYu"
#define BLYNK_TEMPLATE_NAME "iMCLab Blynk"
#define BLYNK_AUTH_TOKEN "U3wgvSDWUDkRTaLdoEiSTd2wFIvN1J5q"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "BAGUSMOTOR";
char pass[] = "kamispahing";

// Pin ESP32
const int ledPin = 2;
const int motor1Pin1 = 27;
const int motor1Pin2 = 26;
const int enable1Pin = 12;

// PWM motor
const int freq = 30000;
const int resolution = 8;

int dutyCycle = 120;
bool motorOn = false;

// Terapkan kondisi motor
void applyMotor()
{
  if (motorOn && dutyCycle > 0) {
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    ledcWrite(enable1Pin, dutyCycle);
    digitalWrite(ledPin, HIGH);
  } else {
    ledcWrite(enable1Pin, 0);
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(ledPin, LOW);
  }
}

// Slider V1: atur kecepatan 0-255
BLYNK_WRITE(V1)
{
  dutyCycle = constrain(param.asInt(), 0, 255);
  applyMotor();

  Serial.print("PWM: ");
  Serial.println(dutyCycle);
}

// Tombol V0: ON/OFF
BLYNK_WRITE(V0)
{
  motorOn = (param.asInt() == 1);
  applyMotor();

  Serial.println(motorOn ? "Motor ON" : "Motor OFF");
}

// Sinkronisasi saat ESP32 terhubung
BLYNK_CONNECTED()
{
  Blynk.virtualWrite(V0, motorOn ? 1 : 0);
  Blynk.virtualWrite(V1, dutyCycle);
}

void setup()
{
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);

  ledcAttach(enable1Pin, freq, resolution);
  applyMotor();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop()
{
  Blynk.run();
}