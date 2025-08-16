#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <lvgl.h>

// --- TFT 硬體腳位 ---
#define TFT_CS   5
#define TFT_RST  3   // 沒接就 -1
#define TFT_DC   4
#define TFT_SCLK 6
#define TFT_MOSI 7

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// ----------- LVGL 繪製回呼（v8.x API）-------------
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  tft.startWrite();
  for (int y = area->y1; y <= area->y2; y++) {
    tft.setAddrWindow(area->x1, y, area->x2 - area->x1 + 1, 1);
    for (int x = area->x1; x <= area->x2; x++) {
      // 注意：Adafruit_ST7735 用 RGB565，LVGL 是 lv_color_t
      uint16_t c = color_p->full;  // v8.x 寫法
      tft.pushColor(c);
      color_p++;
    }
  }
  tft.endWrite();
  lv_disp_flush_ready(disp);
}

// ------------ 初始化 --------------
void setup() {
  Serial.begin(115200);
  delay(200);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);

  // LVGL
  lv_init();

  static lv_disp_draw_buf_t draw_buf;
  static lv_color_t buf[128 * 20]; // 緩衝區：橫 128，20 行，依面板調整
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, 128 * 20);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = 128;   // 螢幕寬度
  disp_drv.ver_res = 160;   // 螢幕高度
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // -------- LVGL 畫面測試 ----------
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "LVGL Hello!\n你好！");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  Serial.println("LVGL init done");
}

void loop() {
  lv_timer_handler();
  delay(5);
}
