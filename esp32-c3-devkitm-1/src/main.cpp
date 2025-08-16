#include <lvgl.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <WiFi.h>
#include <time.h>

// 螢幕腳位
#define TFT_CS     5
#define TFT_RST    3
#define TFT_DC     4
#define TFT_SCLK   6
#define TFT_MOSI   7

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

#define TFT_WIDTH  128
#define TFT_HEIGHT 160

// WiFi 設定
const char* ssid     = "Aquaticpeng";
const char* password = "11191119";

// LVGL
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[TFT_WIDTH * 20];

lv_obj_t* label_time;  // 時鐘 Label 指標

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

  // 初始化螢幕
  tft.initR(INITR_BLACKTAB);   // 可依你的面板換 GREENTAB/REDTAB
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);

  // LVGL 初始化
  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, TFT_WIDTH * 20);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = TFT_WIDTH;
  disp_drv.ver_res = TFT_HEIGHT;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // --- LVGL 測試元件 ---
  label_time = lv_label_create(lv_scr_act());
  lv_obj_align(label_time, LV_ALIGN_CENTER, 0, 0);
  lv_label_set_text(label_time, "Connecting to WiFi...");

  // WiFi 連線
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lv_label_set_text(label_time, "Link WiFi...");
    lv_timer_handler();  // 保證畫面會更新
  }
  Serial.println(" Connected!");
  lv_label_set_text(label_time, "Connected!");

  // NTP 設定
  configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov"); // UTC+8
}

void update_time_label() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    static char timeStr[16];
    sprintf(timeStr, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    lv_label_set_text(label_time, timeStr);
  } else {
    lv_label_set_text(label_time, "NTP同步中");
  }
}

void loop()
{
  lv_timer_handler();
  update_time_label();
  delay(1000);
}
