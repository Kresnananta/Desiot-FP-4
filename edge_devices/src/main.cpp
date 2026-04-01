#include <Arduino.h>

void setup() {
  // Inisialisasi komunikasi Serial
  Serial.begin(115200);
}

void loop() {
  // Buat data dummy suhu dan gas
  float suhu = random(250, 400) / 10.0; 
  float gas_co = random(0, 50) / 10.0;

  // Format menjadi JSON String
  Serial.print("{\"suhu\": ");
  Serial.print(suhu);
  Serial.print(", \"gas_co\": ");
  Serial.print(gas_co);
  Serial.println("}");

  delay(2000); 
}