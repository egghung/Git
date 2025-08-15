#include <lvgl.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// 螢幕腳位
#define TFT_CS     5
#define TFT_RST    3
#define TFT_DC     4
#define TFT_SCLK   6
#define TFT_MOSI   7

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

#define TFT_WIDTH  128
#define TFT_HEIGHT 160

/* --- LVGL 驅動緩衝區 --- */
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[TFT_WIDTH * 20]; // 20 行 buffer（你也可以 10 行，愈大愈滑順）

/* --- 把 LVGL 畫面傳給 Adafruit_ST7735 --- */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, (area->x2-area->x1+1), (area->y2-area->y1+1));
  for(int y = area->y1; y <= area->y2; y++) {
    for(int x = area->x1; x <= area->x2; x++) {
      tft.pushColor(color_p->full);
      color_p++;
    }
  }
  tft.endWrite();
  lv_disp_flush_ready(disp);
}

void setup()
{
  Serial.begin(115200);
  delay(200);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);

  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, TFT_WIDTH * 20);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = TFT_WIDTH;
  disp_drv.ver_res = TFT_HEIGHT;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // LVGL 測試元件
  lv_obj_t *btn = lv_btn_create(lv_scr_act());
  lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
  lv_obj_t *label = lv_label_create(btn);
  lv_label_set_text(label, "Hello LVGL!");
}

void loop()
{
  lv_timer_handler(); // LVGL 事件循環
  delay(5);
}
