#include <Arduino.h>
#include <SPI.h>

#define TEST_MOSI 7
#define TEST_SCLK 6
#define TEST_CS   5

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== SPI Pin Test Start ===");

  // 初始化 SPI
  SPI.begin(TEST_SCLK, -1, TEST_MOSI, TEST_CS);
  pinMode(TEST_CS, OUTPUT);
  digitalWrite(TEST_CS, HIGH);

  // 發送測試資料
  Serial.println("Sending SPI test pulses...");
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(TEST_CS, LOW);
  for (int i = 0; i < 100; i++) {
    SPI.transfer(0xAA);  // 10101010
    delay(10);
  }
  digitalWrite(TEST_CS, HIGH);
  SPI.endTransaction();

  Serial.println("Test complete. Check MOSI/SCLK lines.");
}

void loop() {
  Serial.println("Running...");
  delay(1000);
}
