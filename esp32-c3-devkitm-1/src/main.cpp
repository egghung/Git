#include <lvgl.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <WiFi.h>
#include <time.h>
// 名稱要跟 .c 檔裡的變數對應
// 螢幕腳位
#define TFT_CS     5
#define TFT_RST    3
#define TFT_DC     4
#define TFT_SCLK   6
#define TFT_MOSI   7

// 宣告 Orara 圖片
// 兩張圖的變數名稱要跟你 .c 內一致
LV_IMG_DECLARE(OraraOpen);
LV_IMG_DECLARE(OraraClose);

// 全域圖片物件，讓計時器 callback 取得到
lv_obj_t* img_orara = nullptr;

 Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
 
#define TFT_WIDTH  128
#define TFT_HEIGHT 160

// WiFi 設定
const char* ssid     = "Aquaticpeng";
const char* password = "11191119";

// LVGL
static lv_disp_draw_buf_t draw_buf;
/* 建議先 80 行（約 20KB ×2 = 40KB），若覺得吃緊改 60 行 */
static lv_color_t buf1[TFT_WIDTH * 80];
static lv_color_t buf2[TFT_WIDTH * 80];


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

static void blink_cb(lv_timer_t* t) {
  static bool closed = false;
  static bool double_blink = false;

  if (closed) {
    // 張眼
    lv_img_set_src(img_orara, &OraraOpen);
    closed = false;

    if (double_blink) {
      // 如果觸發雙眨眼 → 很快再閉一次
      double_blink = false;
      lv_timer_set_period(t, 150);  // 150ms 後再閉一次
    } else {
      // 平常睜開 2.5–4 秒
      uint32_t wait_open = 2500 + (lv_tick_get() % 1500);
      lv_timer_set_period(t, wait_open);

      // 10% 機率觸發雙眨眼
      if ((lv_tick_get() % 40) == 0) {
        double_blink = true;
      }
    }
  } else {
    // 閉眼
    lv_img_set_src(img_orara, &OraraClose);
    closed = true;
    lv_timer_set_period(t, 120);  // 閉眼 120ms
  }
}




void setup()
{
  Serial.begin(115200);
  delay(200);

  // 初始化螢幕
  tft.initR(INITR_BLACKTAB);   // 可依你的面板換 GREENTAB/REDTAB
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);

  tft.setSPISpeed(40000000);  // 40 MHz（不穩就改 27000000 或 24000000）

  
  // LVGL 初始化
  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf1, buf2, TFT_WIDTH * 80);

  

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = TFT_WIDTH;
  disp_drv.ver_res = TFT_HEIGHT;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);
  //lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0); //黑底背景

  // --- LVGL 測試元件 ---
  label_time = lv_label_create(lv_scr_act());
  lv_obj_align(label_time, LV_ALIGN_BOTTOM_MID, 0, -2);
  lv_label_set_text(label_time, "Connecting to WiFi...");

  // 建立圖片物件並顯示
   img_orara = lv_img_create(lv_scr_act());
  lv_img_set_src(img_orara, &OraraOpen);
  lv_obj_align(img_orara, LV_ALIGN_TOP_MID, 0, 0); // 依需求改位置
 
  // 啟動眨眼計時器：先等待 3 秒再第一次眨眼
  lv_timer_create(blink_cb, 3000, nullptr);

  //設定字體
  lv_obj_set_style_text_font(label_time, &lv_font_montserrat_28, 0);  //字體大小
  lv_obj_set_style_text_color(label_time, lv_color_hex(0x800020), 0); //酒紅色字體


  // WiFi 連線
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lv_label_set_text(label_time, "Link ...");
    lv_timer_handler();  // 保證畫面會更新
  }
  Serial.println(" Connected!");
  lv_label_set_text(label_time, "OK!");

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
    // 高頻跑 UI：每 ~5ms 一次（人眼看起來就順）
  lv_timer_handler();

  // 每秒更新一次時間
  static uint32_t last = 0;
  uint32_t now = millis();
  if (now - last >= 1000) {
    last = now;
    update_time_label();
  }

  delay(2);  // ← 不要再 delay(1000)，改短一點讓 UI 能流暢刷新
}