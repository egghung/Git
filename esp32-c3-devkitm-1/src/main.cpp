#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <lvgl.h>

// ==== 你的接腳設定（軟體 SPI）====
#define TFT_CS    5
#define TFT_DC    4
#define TFT_RST   3
#define TFT_SCLK  6
#define TFT_MOSI  7
#define TFT_BL    10  // 背光，可改其他可用 GPIO

// 128x160 ST7735
#define TFT_W 128
#define TFT_H 160

// 建立 ST7735 物件（軟體 SPI 建構子）
Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// ====== LVGL 與 GFX 的 glue ======
// 建立一個小的雙 buffer，省記憶體：16 行的掃描列
static lv_color_t disp_draw_buf1[TFT_W * 16];
static lv_color_t disp_draw_buf2[TFT_W * 16];
static lv_disp_draw_buf_t draw_buf;

// flush callback：把 LVGL 的 buffer 畫到 GFX (ST7735)
static void my_disp_flush(lv_disp_drv_t *disp,
                          const lv_area_t *area,
                          lv_color_t *color_p)
{
  // 限界檢查
  int32_t x1 = area->x1;
  int32_t y1 = area->y1;
  int32_t x2 = area->x2;
  int32_t y2 = area->y2;

  if (x2 < 0 || y2 < 0 || x1 >= TFT_W || y1 >= TFT_H) {
    lv_disp_flush_ready(disp);
    return;
  }

  // 設好寫入視窗
  tft.startWrite();
  tft.setAddrWindow(x1, y1, (x2 - x1 + 1), (y2 - y1 + 1));

  // 直接把 LVGL 的 16-bit RGB565 資料丟給 TFT
  // Adafruit ST7735 支援 writePixels(uint16_t*, uint32_t)
  uint32_t len = (x2 - x1 + 1) * (y2 - y1 + 1);
  tft.writePixels((uint16_t*)color_p, len);

  tft.endWrite();

  // 通知 LVGL 已完成
  lv_disp_flush_ready(disp);
}

// 簡單的 LVGL 介面：一個 Label + 每秒更新一次
static lv_obj_t *label;
static uint32_t sec_counter = 0;

static void tick_task_cb(lv_timer_t *t) {
  // 每秒更新 Label
  static uint32_t last_ms = 0;
  uint32_t now = millis();
  if (now - last_ms >= 1000) {
    last_ms = now;
    sec_counter++;
    lv_label_set_text_fmt(label, "Hello LVGL\nsec=%lu", (unsigned long)sec_counter);
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("LVGL + Adafruit_ST7735 (SW SPI) demo start");

  // 背光
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH); // 你的板子為 HIGH=亮

  // 初始化螢幕
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0);            // 0: 直立，需求可改
  tft.fillScreen(ST77XX_BLACK);

  // 初始化 LVGL
  lv_init();

  // 初始化 LVGL draw buffer
  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf1, disp_draw_buf2, TFT_W * 16);

  // 註冊 display 驅動
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = TFT_W;
  disp_drv.ver_res = TFT_H;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // 建立一個 Label
  label = lv_label_create(lv_scr_act());
  lv_obj_center(label);
  lv_label_set_text(label, "Hello LVGL");

  // 每 5ms 跑一次 LVGL 任務（在 loop 裡會 call），並開一個 100ms 的 timer 處理 UI 更新
  lv_timer_create(tick_task_cb, 100, NULL);

  Serial.println("Init done.");
}

void loop() {
  // LVGL 任務處理（約 5~10ms 跑一次）
  lv_timer_handler();
  delay(5);
}
