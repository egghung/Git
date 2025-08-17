#ifndef LV_CONF_H
#define LV_CONF_H

/* === 基本設定 === */
#define LV_COLOR_DEPTH 16
#define LV_COLOR_SCREEN_TRANSP 0

/* 使用自訂 tick（用 Arduino 的 millis）*/
#define LV_TICK_CUSTOM 1
#define LV_TICK_CUSTOM_INCLUDE "Arduino.h"
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())

/* 確保 v8 API 開啟 */
#define LV_USE_API_EXTENSION_V6 1
#define LV_USE_API_EXTENSION_V7 1

/* 預設用小 buffer，省記憶體 */
#define LV_MEM_SIZE (32U * 1024U)

/* 選用的 widgets (最小化，只開 Label) */
#define LV_USE_LABEL 1

#endif /* LV_CONF_H */

#define LV_FONT_MONTSERRAT_28 1 //字體大小
