
/*
  Kit iMCLab - Kontrol Motor DC dan LED
  Board: ESP32
  Baud rate: 115200

  Perintah dari Python:
  1       = Motor ON
  0       = Motor OFF
  +       = PWM naik 10
  -       = PWM turun 10
  PWM:120 = Atur PWM
*/

const int ledPin = 2;
const int motor1Pin1 = 27;
const int motor1Pin2 = 26;
const int enable1Pin = 12;

const int freq = 30000;
const int resolution = 8;

int dutyCycle = 120;
String inputString = "";

void applyMotor() {
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);

  ledcWrite(enable1Pin, dutyCycle);
  digitalWrite(ledPin, dutyCycle > 0 ? HIGH : LOW);
}

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);

  bool pwmOK = ledcAttach(enable1Pin, freq, resolution);

  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(ledPin, LOW);
  ledcWrite(enable1Pin, 0);

  Serial.println("ESP32 siap menerima perintah!");

  if (!pwmOK) {
    Serial.println("ERROR: PWM gagal diaktifkan!");
  }
}

void prosesPerintah(String perintah) {
  perintah.trim();

  if (perintah == "1") {
    if (dutyCycle == 0) {
      dutyCycle = 120;
    }

    applyMotor();
    Serial.println("Motor ON");
  }
  else if (perintah == "0") {
    dutyCycle = 0;
    applyMotor();
    Serial.println("Motor OFF");
  }
  else if (perintah == "+") {
    dutyCycle = min(255, dutyCycle + 10);
    applyMotor();
    Serial.printf("PWM naik: %d\n", dutyCycle);
  }
  else if (perintah == "-") {
    dutyCycle = max(0, dutyCycle - 10);
    applyMotor();
    Serial.printf("PWM turun: %d\n", dutyCycle);
  }
  else if (perintah.startsWith("PWM:")) {
    String nilai = perintah.substring(4);
    nilai.trim();

    if (nilai.length() == 0) {
      Serial.println("ERROR: Nilai PWM kosong");
      return;
    }

    for (int i = 0; i < nilai.length(); i++) {
      if (!isDigit(nilai[i])) {
        Serial.println("ERROR: PWM harus berupa angka");
        return;
      }
    }

    int pwm = nilai.toInt();

    if (pwm >= 0 && pwm <= 255) {
      dutyCycle = pwm;
      applyMotor();
      Serial.printf("PWM diatur: %d\n", dutyCycle);
    } else {
      Serial.println("ERROR: PWM harus 0-255");
    }
  }
  else {
    Serial.println("Perintah tidak dikenal");
  }
}

void loop() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      if (inputString.length() > 0) {
        prosesPerintah(inputString);
        inputString = "";
      }
    }
    else if (inputString.length() < 40) {
      inputString += c;
    }
  }
}
