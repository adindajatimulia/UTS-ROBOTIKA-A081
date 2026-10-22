
/*
  Kontrol Motor DC + LED menggunakan PWM via Serial
  Board     : ESP32
  Core      : Arduino-ESP32 3.3.12

  Perintah Serial Monitor:
  1       = Motor ON
  0       = Motor OFF
  +       = Kecepatan naik 10
  -       = Kecepatan turun 10
  PWM:xxx = Atur PWM 0-255
*/

const int ledPin = 2;

const int motor1Pin1 = 27;
const int motor1Pin2 = 26;
const int enable1Pin = 12;

// Pengaturan PWM
const int freq = 30000;
const int resolution = 8;

int dutyCycle = 120;  // Kecepatan awal relatif pelan
const int DUTY_STEP = 10;
const int DUTY_MIN = 0;
const int DUTY_MAX = 255;

String inputString = "";

void applyMotor() {
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);

  // ESP32 Core 3.x menggunakan PIN, bukan channel
  ledcWrite(enable1Pin, dutyCycle);

  digitalWrite(ledPin, dutyCycle > 0 ? HIGH : LOW);
}

void MotorOn() {
  if (dutyCycle == 0) {
    dutyCycle = 120;
  }

  applyMotor();

  Serial.print("Motor ON, PWM = ");
  Serial.println(dutyCycle);
}

void MotorOff() {
  dutyCycle = 0;
  applyMotor();

  Serial.println("Motor OFF");
}

void naikkanKecepatan() {
  dutyCycle += DUTY_STEP;

  if (dutyCycle > DUTY_MAX) {
    dutyCycle = DUTY_MAX;
  }

  applyMotor();

  Serial.print("PWM naik: ");
  Serial.println(dutyCycle);
}

void turunkanKecepatan() {
  dutyCycle -= DUTY_STEP;

  if (dutyCycle < DUTY_MIN) {
    dutyCycle = DUTY_MIN;
  }

  applyMotor();

  Serial.print("PWM turun: ");
  Serial.println(dutyCycle);
}

void setDuty(int val) {
  dutyCycle = constrain(val, DUTY_MIN, DUTY_MAX);
  applyMotor();

  Serial.print("PWM diatur ke: ");
  Serial.println(dutyCycle);
}

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);

  // Mengganti ledcSetup() dan ledcAttachPin()
  bool pwmOK = ledcAttach(enable1Pin, freq, resolution);

  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  ledcWrite(enable1Pin, 0);
  digitalWrite(ledPin, LOW);

  Serial.println("ESP32 Motor Controller siap.");

  if (!pwmOK) {
    Serial.println("ERROR: Konfigurasi PWM gagal!");
  } else {
    Serial.println("Perintah: 1=ON, 0=OFF, +=naik, -=turun");
    Serial.println("Ketik PWM:120 untuk mengatur kecepatan.");
    Serial.println("Ketik 1 untuk menyalakan motor.");
  }
}

void loop() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      inputString.trim();

      if (inputString.length() > 0) {
        if (inputString.startsWith("PWM:")) {
          int val = inputString.substring(4).toInt();
          setDuty(val);
        } else if (inputString == "1") {
          MotorOn();
        } else if (inputString == "0") {
          MotorOff();
        } else if (inputString == "+") {
          naikkanKecepatan();
        } else if (inputString == "-") {
          turunkanKecepatan();
        } else {
          Serial.print("Perintah tidak dikenal: ");
          Serial.println(inputString);
        }
      }

      inputString = "";
    } else {
      inputString += c;
    }
  }
}
