#include "Styles.h"

LV_FONT_DECLARE(open_sans_16);
LV_FONT_DECLARE(open_sans_32);


lv_style_t label_style;
lv_style_t container_style;
lv_style_t button_style;
lv_style_t label_white_style;
lv_style_t label_big_style;
lv_style_t settings_button_style;
lv_style_t list_button_style;
lv_style_t roller_style_bg;
lv_style_t roller_style_select;
lv_style_t calendar_style_dates;
lv_style_t slider_style_bg;
lv_style_t slider_style_knob;
lv_style_t spinbox_style;
lv_style_t spinbox_style_selected;
lv_style_t likert_style_bg;
lv_style_t likert_style_btn;


void init_styles(){
    lv_style_t* s;

    s = &container_style;
    lv_style_init(s);
    lv_style_set_radius(s, LV_OBJ_PART_MAIN, 0);
    lv_style_set_bg_color(s, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    lv_style_set_bg_opa(s, LV_OBJ_PART_MAIN, LV_OPA_COVER);
    lv_style_set_border_width(s, LV_OBJ_PART_MAIN, 0);

    s = &label_style;
    lv_style_init(s);
    lv_style_set_text_color(s, LV_STATE_DEFAULT, KL_COLOR_BLUE);
    lv_style_set_text_font(s, LV_STATE_DEFAULT, &open_sans_16);

    s = &button_style;
    lv_style_init(s);
    lv_style_set_bg_color(s, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_bg_color(s, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_style_set_border_color(s, LV_STATE_DEFAULT, KL_COLOR_BLUE);
    lv_style_set_border_color(s, LV_STATE_PRESSED, KL_COLOR_RED);
    lv_style_set_border_color(s, LV_STATE_DISABLED, LV_COLOR_GRAY);

    s = &label_white_style;
    lv_style_init(s);
    lv_style_set_text_color(s, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(s, LV_STATE_DEFAULT, &open_sans_16);

    s = &label_big_style;
    lv_style_init(s);
    lv_style_set_text_color(s, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(s, LV_STATE_DEFAULT, &open_sans_32);

    s = &settings_button_style;
    lv_style_init(s);
    lv_style_set_pad_all(s, LV_STATE_DEFAULT, 0);
    lv_style_set_margin_all(s, LV_STATE_DEFAULT, 0);
    lv_style_set_border_side(s, LV_STATE_DEFAULT, LV_BORDER_SIDE_NONE);
    lv_style_set_border_side(s, LV_STATE_PRESSED, LV_BORDER_SIDE_NONE);
    lv_style_set_outline_opa(s, LV_STATE_PRESSED, LV_OPA_TRANSP);
    lv_style_set_outline_opa(s, LV_STATE_FOCUSED, LV_OPA_TRANSP);
    lv_style_set_value_str(s, LV_STATE_DEFAULT, LV_SYMBOL_SETTINGS);
    lv_style_set_value_color(s, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_style_set_value_color(s, LV_STATE_PRESSED, KL_COLOR_RED);
    lv_style_set_value_align(s, LV_STATE_DEFAULT, LV_ALIGN_IN_RIGHT_MID);
    lv_style_set_bg_opa(s, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_bg_opa(s, LV_STATE_PRESSED, LV_OPA_TRANSP);

    s = &list_button_style;
    lv_style_init(s);
    lv_style_set_border_color(s, LV_STATE_DEFAULT, KL_COLOR_BLUE);
    lv_style_set_outline_color(s, LV_STATE_PRESSED, KL_COLOR_RED);
    lv_style_set_outline_opa(s, LV_STATE_FOCUSED, LV_OPA_TRANSP);
    lv_style_set_outline_opa(s, LV_STATE_PRESSED, LV_OPA_COVER);

    s = &roller_style_bg;
    lv_style_init(s);
    lv_style_set_border_color(s, LV_STATE_DEFAULT, KL_COLOR_BLUE);
    lv_style_set_border_color(s, LV_STATE_PRESSED, KL_COLOR_BLUE);
    lv_style_set_border_color(s, LV_STATE_FOCUSED, KL_COLOR_BLUE);
    lv_style_set_bg_color(s, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_color(s, LV_STATE_DEFAULT, KL_COLOR_BLUE);

    s = &roller_style_select;
    lv_style_init(s);
    lv_style_set_bg_color(s, LV_STATE_DEFAULT, KL_COLOR_BLUE);
    lv_style_set_text_color(s, LV_STATE_DEFAULT, LV_COLOR_WHITE);

    s = &calendar_style_dates;
    lv_style_init(s);
    lv_style_set_text_font(s, LV_STATE_DEFAULT, &open_sans_16);
    lv_style_set_bg_color(s, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_color(s, LV_STATE_DEFAULT, KL_COLOR_BLUE);
    lv_style_set_text_color(s, LV_STATE_PRESSED, KL_COLOR_BLUE);
    lv_style_set_text_color(s, LV_STATE_FOCUSED, LV_COLOR_WHITE);
    lv_style_set_bg_color(s, LV_STATE_FOCUSED, KL_COLOR_BLUE);
    lv_style_set_bg_color(s, LV_STATE_PRESSED, KL_COLOR_RED);

    s = &slider_style_bg;
    lv_style_init(s);
    lv_style_set_bg_color(s, LV_STATE_DEFAULT, KL_COLOR_BLUE);
    lv_style_set_bg_opa(s, LV_STATE_DEFAULT, LV_OPA_50);
    lv_style_set_outline_opa(s, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_outline_opa(s, LV_STATE_PRESSED, LV_OPA_COVER);
    lv_style_set_outline_opa(s, LV_STATE_FOCUSED, LV_OPA_COVER);
    lv_style_set_outline_width(s, LV_STATE_DEFAULT, 2);
    lv_style_set_outline_color(s, LV_STATE_DEFAULT, KL_COLOR_BLUE);
    lv_style_set_outline_color(s, LV_STATE_PRESSED, KL_COLOR_BLUE);
    lv_style_set_outline_color(s, LV_STATE_FOCUSED, KL_COLOR_BLUE);
    
    s = &slider_style_knob;
    lv_style_init(s);
    lv_style_set_bg_color(s, LV_STATE_DEFAULT, KL_COLOR_RED);

    s = &spinbox_style;
    lv_style_init(s);
    lv_style_set_bg_color(s, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_border_color(s, LV_STATE_DEFAULT, KL_COLOR_BLUE);
    lv_style_set_border_color(s, LV_STATE_PRESSED, KL_COLOR_BLUE);
    lv_style_set_border_color(s, LV_STATE_FOCUSED, KL_COLOR_BLUE);
    lv_style_set_outline_color(s, LV_STATE_DEFAULT, KL_COLOR_BLUE);
    lv_style_set_outline_color(s, LV_STATE_PRESSED, KL_COLOR_BLUE);
    lv_style_set_outline_color(s, LV_STATE_FOCUSED, KL_COLOR_BLUE);
    lv_style_set_text_font(s, LV_STATE_DEFAULT, &open_sans_32);
    lv_style_set_text_color(s, LV_STATE_DEFAULT, KL_COLOR_BLUE);

    s = &spinbox_style_selected;
    lv_style_init(s);
    lv_style_set_bg_color(s, LV_STATE_DEFAULT, KL_COLOR_BLUE);
    lv_style_set_text_color(s, LV_STATE_DEFAULT, LV_COLOR_WHITE);

    s = &likert_style_bg;
    lv_style_init(s);
    lv_style_set_border_opa(s, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_border_opa(s, LV_STATE_PRESSED, LV_OPA_TRANSP);
    lv_style_set_border_opa(s, LV_STATE_FOCUSED, LV_OPA_TRANSP);

    s = &likert_style_btn;
    lv_style_init(s);
    lv_style_set_border_color(s, LV_STATE_DEFAULT, KL_COLOR_BLUE);
    lv_style_set_border_opa(s, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(s, LV_STATE_CHECKED, KL_COLOR_RED);
    lv_style_set_radius(s, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
}