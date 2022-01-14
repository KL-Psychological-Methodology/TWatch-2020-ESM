#ifndef ES_STYLES_H
#define ES_STYLES_H

#include "config.h"

#define KL_COLOR_RED LV_COLOR_MAKE(0xFF, 0x32, 0x00)
#define KL_COLOR_BLUE LV_COLOR_MAKE(0x21, 0x33, 0x42)

extern lv_style_t label_style;
extern lv_style_t label_white_style;
extern lv_style_t label_big_style;
extern lv_style_t container_style;
extern lv_style_t button_style;
extern lv_style_t settings_button_style;
extern lv_style_t list_button_style;
extern lv_style_t roller_style_bg;
extern lv_style_t roller_style_select;
extern lv_style_t calendar_style_dates;
extern lv_style_t slider_style_bg;
extern lv_style_t slider_style_knob;
extern lv_style_t spinbox_style;
extern lv_style_t spinbox_style_selected;
extern lv_style_t likert_style_bg;
extern lv_style_t likert_style_btn;

void init_styles();


#endif // ES_STYLES_H