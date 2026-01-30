/*
===============================================================================
DRISHTI v5.13 PRODUCTION FIRMWARE - GITHUB READY
104-Step Ultra-Fine Siren | 7-Wire | XSHUT Protected
Distance Recognition Intelligent Sensor-based Haptic Travel Instrument
Kartikey Kashyap | Class 8A | Vishwa Bharti School | Jan 2026
GitHub: https://github.com/esp32-lover123/Smart-Blind-cane/tree/main
===============================================================================
*/

#include <Wire.h>
#include <VL53L0X.h>

#define BUZZER_PIN 9
#define XSHUT_PIN 3

VL53L0X sensor;

// 104-STEP ULTRA-FINE AUDIO SWEEP (1000→3000Hz)
const int ULTRA_FINE_SWEEP[104] PROGMEM = {
  1000,1010,1020,1030,1040,1050,1060,1070,1080,1090,
  1100,1110,1120,1130,1140,1150,1160,1170,1180,1190,
  1200,1210,1220,1230,1240,1250,1260,1270,1280,1290,
  1300,1310,1320,1330,1340,1350,1360,1370,1380,1390,
  1400,1410,1420,1430,1440,1450,1460,1470,1480,1490,
  1500,1510,1520,1530,1540,1550,1560,1570,1580,1590,
  1600,1610,1620,1630,1640,1650,1660,1670,1680,1690,
  1700,1710,1720,1730,1740,1750,1760,1770,1780,1790,
  1800,1810,1820,1830,1840,1850,1860,1870,1880,1890,
  1900,1910,1920,1930,1940,1950,1960,1970,1980,1990,
  2000,2010,2020,2030,2040,2050,2060,2070,2080,2090,
  2100,2110,2120,2130,2140,2150,2160,2170,2180,2190
};

const uint32_t READ_INTERVAL = 70;  // 14Hz
const uint32_t TONE_INTERVAL = 10;  // 1.04s cycle
const uint32_t RESET_THRESHOLD = 20;

void setup() {
  Serial.begin(9600); delay(2000);
  
  pinMode(XSHUT_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  sensorPowerCycle();
  Wire.begin();
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  Wire.setClock(100000);
  
  Serial.println(F("\n=== DRISHTI v5.13 PRODUCTION STARTUP ==="));
  Serial.println(F("104-Step Siren | GitHub: kartikeykashyap.github.io/drishti-smart-cane"));
  
  if (initSensor()) {
    Serial.println(F("✅ PRODUCTION READY | ≤30cm = 104-STEP SIREN"));
    startupMelody();
  } else {
    Serial.println(F("❌ SENSOR FAILED - CHECK XSHUT→D3"));
    while(1) errorPattern();
  }
}

void loop() {
  static uint32_t lastRead = 0, lastTone = 0;
  static uint8_t toneIndex = 0;
  static int failureCount = 0;
  
  uint32_t now = millis();
  
  if (now - lastRead >= READ_INTERVAL) {
    uint16_t distance = readDistance();
    bool collision = (distance <= 300 && distance != 999);
    
    if (collision) {
      if (now - lastTone >= TONE_INTERVAL) {
        tone(BUZZER_PIN, pgm_read_word(&ULTRA_FINE_SWEEP[toneIndex]));
        toneIndex = (toneIndex + 1) % 104;
        lastTone = now;
      }
    } else {
      noTone(BUZZER_PIN);
      digitalWrite(BUZZER_PIN, LOW);
      pinMode(BUZZER_PIN, OUTPUT);
      toneIndex = 0;
    }
    
    if (distance == 999) {
      failureCount++;
      if (failureCount >= RESET_THRESHOLD) {
        Serial.println(F("🔄 XSHUT HARDWARE RESET"));
        sensorRestart();
        failureCount = 0;
      }
      Serial.println(F("📏 SENSOR ERROR | Auto-recovery active"));
    } else {
      failureCount = 0;
      Serial.print(F("📏 "));
      Serial.print(distance / 10);
      Serial.print(F("cm | "));
      Serial.println(collision ? F("🚨 104-STEP SIREN") : F("✅ SILENT"));
    }
    lastRead = now;
  }
}

// PRODUCTION SENSOR MANAGEMENT
bool initSensor() {
  Wire.end(); delay(50); Wire.begin(); delay(50);
  if (!sensor.init()) return false;
  sensor.setTimeout(100);
  sensor.setMeasurementTimingBudget(20000);
  return true;
}

uint16_t readDistance() {
  uint16_t dist = sensor.readRangeSingleMillimeters();
  return (sensor.timeoutOccurred() || dist < 30 || dist > 1800) ? 999 : dist;
}

void sensorPowerCycle() {
  digitalWrite(XSHUT_PIN, LOW); delay(10);
  digitalWrite(XSHUT_PIN, HIGH); delay(100);
}

void sensorRestart() {
  sensorPowerCycle();
  initSensor();
}

// PRODUCTION AUDIO
void startupMelody() {
  const int melody[] PROGMEM = {523, 659, 784, 1047};
  for (int i = 0; i < 4; i++) {
    tone(BUZZER_PIN, pgm_read_word(&melody[i]), 100);
    delay(150);
    noTone(BUZZER_PIN); digitalWrite(BUZZER_PIN, LOW);
  }
  Serial.println(F("🎵 PRODUCTION READY"));
}

void errorPattern() {
  while(1) {
    for (int i = 0; i < 3; i++) {
      tone(BUZZER_PIN, 200, 200); delay(250);
      noTone(BUZZER_PIN); digitalWrite(BUZZER_PIN, LOW); delay(100);
    }
    delay(1000);
  }
}
