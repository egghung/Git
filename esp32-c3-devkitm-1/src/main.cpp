#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// ======== 使用軟體 SPI 模式，腳位與你提供的一致 ========
#define TFT_CS    5  // 你的 CS
#define TFT_DC    4  // 你的 DC
#define TFT_SDA   7  // 你的 SDA (MOSI)
#define TFT_SCL   6  // 你的 SCK
#define TFT_RST   3  // 你的 RES
// =======================================================

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_SDA, TFT_SCL, TFT_RST);

void setup() {
  Serial.begin(115200);
  Serial.println("Adafruit ST7735 Software SPI Test!");

  tft.initR(INITR_BLACKTAB);
  Serial.println("Initialization done.");

  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(10, 10);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.println("Hello, C3!");

  Serial.println("Text should be on screen.");
}

void loop() {
  // 這裡不需要做任何事
}