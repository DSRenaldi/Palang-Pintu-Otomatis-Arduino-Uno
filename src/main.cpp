#include <Servo.h>
#include <Arduino.h>
#include <LiquidCrystal.h>

Servo palang;
LiquidCrystal lcd(7, 8, 5, 6, 10, 11);

// Pin
const int trigPin = 2;
const int echoPin = 3;
const int ledHijau = A1;
const int ledMerah = A2;
const int servoPin = 9;
const int batasJarakCm = 300;

void bukaPalang();
void tutupPalang();
float bacaJarakCm();
void setLed(bool hijauMenyala, bool merahMenyala);
void updateLampu();

enum StatusPintu {
  TERTUTUP,
  MEMBUKA,
  TERBUKA,
  MENUTUP
};

int lcdMode = -1; // 0=open, 1=closing, 2=closed

void tampilLCD(int mode) {
  if (lcdMode == mode) return;
  lcdMode = mode;
  lcd.clear();

  if (mode == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Pintu Terbuka");
  } else if (mode == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Pintu Menutup");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Silahkan");
    lcd.setCursor(0, 1);
    lcd.print("Mendekat");
  }
}

// Tracking untuk delay 5 detik
unsigned long waktuSensorKosong = 0;
unsigned long waktuMenutup = 0;
StatusPintu statusPintu = TERTUTUP;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledHijau, OUTPUT);
  pinMode(ledMerah, OUTPUT);

  digitalWrite(trigPin, LOW);

  lcd.begin(16, 2);
  palang.attach(servoPin);

  // kondisi awal tertutup
  tutupPalang();
  tampilLCD(2);
}

void loop() {
  float jarak = bacaJarakCm();
  bool adaObjek = jarak > 0 && jarak <= batasJarakCm;

  // HC-SR04: objek dianggap terdeteksi jika berada dalam batas jarak.
  if (adaObjek) {
    if (statusPintu == TERTUTUP || statusPintu == MENUTUP) {
      statusPintu = MEMBUKA;
      bukaPalang();
      statusPintu = TERBUKA;
    }
    waktuSensorKosong = 0;  // reset timer saat objek terdeteksi
    tampilLCD(0);
  } else {
    if (waktuSensorKosong == 0) {
      waktuSensorKosong = millis();
    }

    if (statusPintu == MENUTUP) {
      if (millis() - waktuMenutup >= 500) {
        statusPintu = TERTUTUP;
        tampilLCD(2);
      }
    } else if (statusPintu == TERBUKA) {
      if (millis() - waktuSensorKosong >= 5000) {
        statusPintu = MENUTUP;
        tampilLCD(1);
        tutupPalang();
        waktuMenutup = millis();
        waktuSensorKosong = 0;
      } else {
        tampilLCD(0);
      }
    } else {
      tampilLCD(2);
    }
  }

  updateLampu();

  delay(200);
}

float bacaJarakCm() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long durasi = pulseIn(echoPin, HIGH, 30000);
  if (durasi == 0) {
    return -1;
  }

  return durasi * 0.0343 / 2.0;
}

void setLed(bool hijauMenyala, bool merahMenyala) {
  // LED biasa pada diagram saat ini aktif saat pin HIGH.
  digitalWrite(ledHijau, hijauMenyala ? HIGH : LOW);
  digitalWrite(ledMerah, merahMenyala ? HIGH : LOW);
}

void updateLampu() {
  if (statusPintu == MEMBUKA || statusPintu == TERBUKA) {
    // Pintu terbuka atau sedang membuka -> lampu hijau.
    setLed(true, false);
  } else {
    // Pintu tertutup, sedang menutup, atau kondisi default -> lampu merah.
    setLed(false, true);
  }
}

// =====================
// FUNGSI BUKA
// =====================
void bukaPalang() {
  palang.write(90); // buka
}

// =====================
// FUNGSI TUTUP
// =====================
void tutupPalang() {
  palang.write(0); // tutup
}
