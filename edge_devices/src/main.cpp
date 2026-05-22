#include <Arduino.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <MQUnifiedsensor.h>

// Definisikan Pin sesuai wiring kita tadi
#define DHTPIN 32        // Pin D2 untuk DHT22
#define DHTTYPE DHT22   // Tipe sensor DHT22
#define MQ6_PIN 36      // Pin VP (GPIO 36 / ADC1_CH0) untuk gas MQ-6

// --- Konfigurasi MQUnifiedsensor untuk ESP32 ---
#define MQ_BOARD "ESP-32"
#define MQ_TYPE "MQ-6"
#define VOLTAGE_RESOLUTION 3.3
#define ADC_BIT_RESOLUTION 12 
#define RATIO_CLEAN_AIR 10.0

// Inisialisasi sensor DHT
DHT dht(DHTPIN, DHTTYPE);
MQUnifiedsensor MQ6(MQ_BOARD, VOLTAGE_RESOLUTION, ADC_BIT_RESOLUTION, MQ6_PIN, MQ_TYPE);

void setup() {
  Serial.begin(115200);

  // --- Setup MQ-6 ---
  MQ6.setRegressionMethod(1); 
  MQ6.setA(1007.2); 
  MQ6.setB(-2.18);
  MQ6.init();

  // --- Proses Kalibrasi Awal ---
  Serial.println("{\"info\": \"Mengkalibrasi MQ-6. Tolong jangan beri gas dulu!\"}");
  float calcR0 = 0;
  for(int i = 1; i <= 10; i++) {
    MQ6.update(); 
    calcR0 += MQ6.calibrate(RATIO_CLEAN_AIR);
    delay(500);
  }
  
  float r0_average = calcR0 / 10;
  
  // --- BYPASS ERROR MATEMATIKA ---
  if(isinf(r0_average) || r0_average == 0) {
    Serial.println("{\"warning\": \"Tegangan sensor sangat rendah, menggunakan kalibrasi pabrik.\"}");
    MQ6.setR0(10.0); // Paksa gunakan nilai default
  } else {
    MQ6.setR0(r0_average);
    Serial.println("{\"info\": \"Kalibrasi Selesai! Mulai membaca data...\"}");
  }
  
  dht.begin();
  delay(2000); 
}

void loop() {
  // 1. Baca data dari DHT22
  float suhu = dht.readTemperature();

  // 2. Validasi DHT22
  if (isnan(suhu)) {
    Serial.println("{\"error\": \"Gagal membaca sensor DHT22! Cek kabel.\"}");
    delay(2000);
    return; 
  }

  // 3. Baca MQ-6 dalam satuan PPM
  MQ6.update(); 
  float gas_ppm = MQ6.readSensor(); 

  // 4. Buat objek JSON
  JsonDocument doc;
  doc["suhu"] = suhu;
  doc["gas_co"] = gas_ppm;

  // 5. Cetak JSON ke Serial
  serializeJson(doc, Serial);
  Serial.println(); 

  // Tunggu 5 detik
  delay(5000); 
}