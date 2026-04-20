#include <Servo.h>
#include <Arduino.h>
#include <LiquidCrystal.h>

Servo palang;
LiquidCrystal lcd(7, 8, 5, 6, 10, 11);

// Pin
const int sensorPin = 4;
const int ledHijau = A1;
const int ledMerah = A2;
const int servoPin = 9;

void bukaPalang();
void tutupPalang();

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
bool terbuka = false;  // status palang: false = tertutup, true = terbuka
bool sedangMenutup = false;

void setup() {
  pinMode(sensorPin, INPUT_PULLUP); // gunakan pull-up internal untuk sensor IR
  pinMode(ledHijau, OUTPUT);
  pinMode(ledMerah, OUTPUT);

  lcd.begin(16, 2);
  palang.attach(servoPin);

  // kondisi awal tertutup
  tutupPalang();
  tampilLCD(2);
}

void loop() {
  int sensor = digitalRead(sensorPin);

  // Sensor IR: LOW = ada objek, HIGH = tidak ada objek
  if (sensor == LOW) {
    if (!terbuka) {
      bukaPalang();
      terbuka = true;
    }
    sedangMenutup = false;
    waktuSensorKosong = 0;  // reset timer saat objek terdeteksi
    tampilLCD(0);
  } else {
    if (waktuSensorKosong == 0) {
      waktuSensorKosong = millis();
    }

    if (sedangMenutup) {
      if (millis() - waktuMenutup >= 500) {
        sedangMenutup = false;
        tampilLCD(2);
      }
    } else if (terbuka) {
      if (millis() - waktuSensorKosong >= 5000) {
        tampilLCD(1);
        tutupPalang();
        terbuka = false;
        sedangMenutup = true;
        waktuMenutup = millis();
        waktuSensorKosong = 0;
      } else {
        tampilLCD(0);
      }
    } else {
      tampilLCD(2);
    }
  }

  delay(200);
}

// =====================
// FUNGSI BUKA
// =====================
void bukaPalang() {
  palang.write(90); // buka

  digitalWrite(ledHijau, LOW);   // nyalakan LED hijau
  digitalWrite(ledMerah, HIGH);  // matikan LED merah
}

// =====================
// FUNGSI TUTUP
// =====================
void tutupPalang() {
  palang.write(0); // tutup

  digitalWrite(ledHijau, HIGH);  // matikan LED hijau
  digitalWrite(ledMerah, LOW);   // nyalakan LED merah
}