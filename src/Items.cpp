#include "Items.h"

#include "QuestionnaireState.h"
#include "Localization.h"
#include "Styles.h"

const char* EsmBaseItem::header = "id;quest_name;item_name;response_date;response_time;item_duration;type;prompt;left_label;right_label;resolution;response";


EsmBaseItem* EsmBaseItem::active_item = nullptr;


EsmBaseItem::EsmBaseItem(ItemDefinition &item_def){
    item_screen = nullptr;
    value = 0;
    unset = true;
    mandatory = item_def.mandatory;

    
    parent_questionnaire = item_def.parent_questionnaire;
    strcpy(item_name, item_def.item_name);
    strcpy(prompt, item_def.prompt);
}


EsmBaseItem::~EsmBaseItem(){
    if(item_screen != nullptr){
        lv_obj_del_async(item_screen);
        item_screen = nullptr;
    }
}


void EsmBaseItem::activate(){
    active_item = this;
    build();
    start_time = millis();
}


void EsmBaseItem::deactivate(){
    active_item = nullptr;
    if(item_screen != nullptr){
        lv_obj_del_async(item_screen);
        item_screen = nullptr;
    }
}


void EsmBaseItem::mark_complete(){
    complete_time = millis();
}


void EsmBaseItem::item_event_cb(lv_obj_t* obj, lv_event_t event){
    if(obj == active_item->continue_button && event == LV_EVENT_CLICKED){
        active_item->mark_complete();        
        active_item->parent_questionnaire->next_item();
    }else{
        active_item->update(obj, event);
    }
}


void EsmBaseItem::get_result(ItemResult& result) {
    result.prompt = prompt;
    result.item_name = item_name;
    result.duration = complete_time - start_time;
    if(!unset){
        result.response = &value;
    }
}


EsmItemVAS::EsmItemVAS(ItemDefinition &item_def) 
: EsmBaseItem(item_def) {
    strcpy(left_label, item_def.info_vas.left_label);
    strcpy(right_label, item_def.info_vas.right_label);
}


void EsmItemVAS::build(){
    item_screen = lv_cont_create(lv_scr_act(), nullptr);
    lv_obj_add_style(item_screen, LV_OBJ_PART_MAIN, &container_style);
    lv_obj_set_size(item_screen, SCREEN_W, SCREEN_H);
    
    lv_obj_t* prompt_lab = lv_label_create(item_screen, nullptr);
    lv_obj_add_style(prompt_lab, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text(prompt_lab, prompt);
    lv_label_set_align(prompt_lab, LV_LABEL_ALIGN_CENTER);
    lv_label_set_long_mode(prompt_lab, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(prompt_lab, SCREEN_W - 10);
    lv_obj_align(prompt_lab, item_screen, LV_ALIGN_CENTER, 0, - SCREEN_H/4);

    vas = lv_slider_create(item_screen, nullptr);
    lv_obj_add_style(vas, LV_SLIDER_PART_BG, &slider_style_bg);
    lv_obj_add_style(vas, LV_SLIDER_PART_KNOB, &slider_style_knob);
    lv_obj_set_width(vas, 200);
    lv_slider_set_range(vas, 0, 100);
    lv_obj_align(vas, item_screen, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(vas, EsmBaseItem::item_event_cb);
    lv_obj_set_style_local_bg_opa(vas, LV_SLIDER_PART_INDIC, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_bg_opa(vas, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, LV_OPA_TRANSP);

    lv_obj_t* l_lab = lv_label_create(item_screen, nullptr);
    lv_obj_add_style(l_lab, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text(l_lab, left_label);
    lv_obj_align(l_lab, vas, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

    lv_obj_t* r_lab = lv_label_create(item_screen, l_lab);
    lv_label_set_text(r_lab, right_label);
    lv_obj_align(r_lab, vas, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 10);

    continue_button = lv_btn_create(item_screen, nullptr);
    lv_obj_add_style(continue_button, LV_BTN_PART_MAIN, &button_style);
    lv_obj_set_event_cb(continue_button, EsmBaseItem::item_event_cb);
    lv_obj_align(continue_button, item_screen, LV_ALIGN_IN_BOTTOM_MID, 0, -15);
    if(mandatory){
        lv_btn_set_state(continue_button, LV_BTN_STATE_DISABLED);
        lv_obj_set_click(continue_button, false);
    }

    lv_obj_t* button_label = lv_label_create(continue_button, nullptr);
    lv_obj_add_style(button_label, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text(button_label, get_localized_string(LocalStrings::QUESTIONNAIRE_CONTINUE));
}


void EsmItemVAS::update(lv_obj_t* obj, lv_event_t event){
    if(event == LV_EVENT_VALUE_CHANGED){
        lv_obj_set_style_local_bg_opa(vas, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, LV_OPA_COVER);
        value = lv_slider_get_value(vas);
        unset = false;
        lv_obj_set_click(continue_button, true);
        lv_btn_set_state(continue_button, LV_BTN_STATE_RELEASED);
    }
}


void EsmItemVAS::get_result(ItemResult& result){
    EsmBaseItem::get_result(result);
    result.left_label = left_label;
    result.right_label = right_label;
    result.type = ItemType::VAS;
}


EsmItemText::EsmItemText(ItemDefinition &item_def) : EsmBaseItem(item_def) {}


void EsmItemText::get_result(ItemResult& result){
    result.print_result = false;
    result.type = ItemType::TEXT;
}


void EsmItemText::build(){
    item_screen = lv_cont_create(lv_scr_act(), nullptr);
    lv_obj_add_style(item_screen, LV_OBJ_PART_MAIN, &container_style);
    lv_obj_set_size(item_screen, SCREEN_W, SCREEN_H);
    lv_obj_set_pos(item_screen, 0, 0);

    lv_obj_t* prompt_lab = lv_label_create(item_screen, nullptr);
    lv_obj_add_style(prompt_lab, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text(prompt_lab, prompt);
    lv_label_set_align(prompt_lab, LV_LABEL_ALIGN_CENTER);
    lv_label_set_long_mode(prompt_lab, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(prompt_lab, SCREEN_W - 10);
    lv_obj_align(prompt_lab, item_screen, LV_ALIGN_CENTER, 0, - SCREEN_H/6);

    continue_button = lv_btn_create(item_screen, nullptr);
    lv_obj_add_style(continue_button, LV_BTN_PART_MAIN, &button_style);
    lv_obj_set_event_cb(continue_button, EsmBaseItem::item_event_cb);
    lv_obj_align(continue_button, item_screen, LV_ALIGN_IN_BOTTOM_MID, 0, -15);

    lv_obj_t* button_label = lv_label_create(continue_button, nullptr);
    lv_obj_add_style(button_label, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text(button_label, get_localized_string(LocalStrings::QUESTIONNAIRE_CONTINUE));
}


EsmItemNumeric::EsmItemNumeric(ItemDefinition &item_def) : EsmBaseItem(item_def) {
    num_digits = item_def.info_numeric.num_digits;
    separator_position = item_def.info_numeric.separator_position;
    min_value = item_def.info_numeric.min_value;
    max_value = item_def.info_numeric.max_value;
    unset = false;
}


void EsmItemNumeric::build() {
    item_screen = lv_cont_create(lv_scr_act(), nullptr);
    lv_obj_add_style(item_screen, LV_OBJ_PART_MAIN, &container_style);
    lv_obj_set_size(item_screen, SCREEN_W, SCREEN_H);
    lv_obj_set_pos(item_screen, 0, 0);

    lv_obj_t* prompt_lab = lv_label_create(item_screen, nullptr);
    lv_obj_add_style(prompt_lab, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text(prompt_lab, prompt);
    lv_label_set_align(prompt_lab, LV_LABEL_ALIGN_CENTER);
    lv_label_set_long_mode(prompt_lab, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(prompt_lab, SCREEN_W - 10);
    lv_obj_align(prompt_lab, item_screen, LV_ALIGN_IN_TOP_MID, 0, 15);

    spinbox = lv_spinbox_create(item_screen, nullptr);
    lv_obj_add_style(spinbox, LV_SPINBOX_PART_BG, &spinbox_style);
    lv_obj_add_style(spinbox, LV_SPINBOX_PART_CURSOR, &spinbox_style_selected);
    lv_spinbox_set_range(spinbox, min_value, max_value);
    lv_spinbox_set_digit_format(spinbox, num_digits, separator_position);
    lv_textarea_set_text_align(spinbox, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_width(spinbox, 100);
    lv_obj_set_height(spinbox, 50);
    lv_obj_align(spinbox, item_screen, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(spinbox, EsmBaseItem::item_event_cb);

    lv_coord_t h = lv_obj_get_height(spinbox);
    increment_button = lv_btn_create(item_screen, nullptr);
    lv_obj_add_style(increment_button, LV_BTN_PART_MAIN, &button_style);
    lv_obj_set_size(increment_button, h, h);
    lv_obj_align(increment_button, spinbox, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_theme_apply(increment_button, LV_THEME_SPINBOX_BTN);
    lv_obj_set_style_local_value_str(increment_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_PLUS);
    lv_obj_set_event_cb(increment_button, EsmBaseItem::item_event_cb);

    decrement_button = lv_btn_create(item_screen, increment_button);
    lv_obj_align(decrement_button, spinbox, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_set_event_cb(decrement_button, EsmBaseItem::item_event_cb);
    lv_obj_set_style_local_value_str(decrement_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_MINUS);


    continue_button = lv_btn_create(item_screen, nullptr);
    lv_obj_add_style(continue_button, LV_BTN_PART_MAIN, &button_style);
    lv_obj_set_event_cb(continue_button, EsmBaseItem::item_event_cb);
    lv_obj_align(continue_button, item_screen, LV_ALIGN_IN_BOTTOM_MID, 0, -15);
    if(mandatory){
        lv_btn_set_state(continue_button, LV_BTN_STATE_DISABLED);
        lv_obj_set_click(continue_button, false);
    }

    lv_obj_t* button_label = lv_label_create(continue_button, nullptr);
    lv_obj_add_style(button_label, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text(button_label, get_localized_string(LocalStrings::QUESTIONNAIRE_CONTINUE));
}


void EsmItemNumeric::update(lv_obj_t* obj, lv_event_t event) {
    if(event == LV_EVENT_PRESSED && obj == spinbox){
        value = lv_spinbox_get_value(spinbox);
        if(unset){
            unset = false;
            lv_obj_set_click(continue_button, true);
            lv_btn_set_state(continue_button, LV_BTN_STATE_RELEASED);
        }
    }
    if(event == LV_EVENT_SHORT_CLICKED || event == LV_EVENT_LONG_PRESSED_REPEAT){
        if(obj == increment_button){
            lv_spinbox_increment(spinbox);
        }else if(obj == decrement_button){
            lv_spinbox_decrement(spinbox);
        }
        if(unset){
            unset = false;
            lv_obj_set_click(continue_button, true);
            lv_btn_set_state(continue_button, LV_BTN_STATE_RELEASED);
        }
        value = lv_spinbox_get_value(spinbox);
    }
}


void EsmItemNumeric::get_result(ItemResult& result){
    EsmBaseItem::get_result(result);
    result.resolution = &num_digits;
    result.type = ItemType::NUMERIC;
}


EsmItemLikert::EsmItemLikert(ItemDefinition &item_def) : EsmBaseItem(item_def) {
    strcpy(left_label, item_def.info_likert.left_label);
    strcpy(right_label, item_def.info_likert.right_label);
    resolution = item_def.info_likert.resolution;
}


void EsmItemLikert::build() {
    item_screen = lv_cont_create(lv_scr_act(), nullptr);
    lv_obj_add_style(item_screen, LV_OBJ_PART_MAIN, &container_style);
    lv_obj_set_size(item_screen, SCREEN_W, SCREEN_H);
    lv_obj_set_pos(item_screen, 0, 0);

    lv_obj_t* prompt_lab = lv_label_create(item_screen, nullptr);
    lv_obj_add_style(prompt_lab, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text(prompt_lab, prompt);
    lv_label_set_align(prompt_lab, LV_LABEL_ALIGN_CENTER);
    lv_label_set_long_mode(prompt_lab, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(prompt_lab, SCREEN_W - 10);
    lv_obj_align(prompt_lab, item_screen, LV_ALIGN_CENTER, 0, - SCREEN_H/4);

    uint8_t i = 0;
    for(;i < resolution && i < MAXLIKERT; i++){
        map[i] = " ";
    }
    map[i] = "";

    matrix = lv_btnmatrix_create(item_screen, nullptr);
    lv_obj_add_style(matrix, LV_BTNMATRIX_PART_BG, &likert_style_bg);
    lv_obj_add_style(matrix, LV_BTNMATRIX_PART_BTN, &likert_style_btn);
    lv_obj_set_size(matrix, SCREEN_W - 20, 40);//(SCREEN_W - 20) / (resolution-1));
    lv_btnmatrix_set_map(matrix, map);
    lv_obj_align(matrix, item_screen, LV_ALIGN_CENTER, 0, 0);
    lv_btnmatrix_set_btn_ctrl_all(matrix, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_btnmatrix_set_one_check(matrix, true);
    lv_obj_set_event_cb(matrix, EsmBaseItem::item_event_cb);

    lv_obj_t* l_lab = lv_label_create(item_screen, nullptr);
    lv_obj_add_style(l_lab, LV_LABEL_ALIGN_CENTER, &label_style);
    lv_label_set_text(l_lab, left_label);
    lv_obj_align(l_lab, matrix, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

    lv_obj_t* r_lab = lv_label_create(item_screen, l_lab);
    lv_label_set_text(r_lab, right_label);
    lv_obj_align(r_lab, matrix, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 10);

    continue_button = lv_btn_create(item_screen, nullptr);
    lv_obj_add_style(continue_button, LV_BTN_PART_MAIN, &button_style);
    lv_obj_set_event_cb(continue_button, EsmBaseItem::item_event_cb);
    lv_obj_align(continue_button, item_screen, LV_ALIGN_IN_BOTTOM_MID, 0, -15);
    if(mandatory){
        lv_btn_set_state(continue_button, LV_BTN_STATE_DISABLED);
        lv_obj_set_click(continue_button, false);
    }

    lv_obj_t* button_label = lv_label_create(continue_button, nullptr);
    lv_obj_add_style(button_label, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text(button_label, get_localized_string(LocalStrings::QUESTIONNAIRE_CONTINUE));
}


void EsmItemLikert::update(lv_obj_t* obj, lv_event_t event){
    if(event == LV_EVENT_VALUE_CHANGED){
        value = lv_btnmatrix_get_active_btn(matrix) + 1;
        unset = false;
        lv_obj_set_click(continue_button, true);
        lv_btn_set_state(continue_button, LV_BTN_STATE_RELEASED);
    }
}


void EsmItemLikert::get_result(ItemResult& result){
    EsmBaseItem::get_result(result);
    result.left_label = left_label;
    result.right_label = right_label;
    result.resolution = &resolution;
    result.type = ItemType::LIKERT;
}


EsmItemOptions::EsmItemOptions(ItemDefinition &item_def) : EsmBaseItem(item_def) {    
    strcpy(options, item_def.info_options.options);
    value = 1;
}


void EsmItemOptions::build() {
    item_screen = lv_cont_create(lv_scr_act(), nullptr);
    lv_obj_add_style(item_screen, LV_OBJ_PART_MAIN, &container_style);
    lv_obj_set_size(item_screen, SCREEN_W, SCREEN_H);
    lv_obj_set_pos(item_screen, 0, 0);

    lv_obj_t* prompt_lab = lv_label_create(item_screen, nullptr);
    lv_obj_add_style(prompt_lab, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text(prompt_lab, prompt);
    lv_label_set_align(prompt_lab, LV_LABEL_ALIGN_CENTER);
    lv_label_set_long_mode(prompt_lab, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(prompt_lab, SCREEN_W - 10);
    lv_obj_align(prompt_lab, item_screen, LV_ALIGN_IN_TOP_MID, 0, 15);

    roller = lv_roller_create(item_screen, nullptr);
    lv_obj_add_style(roller, LV_ROLLER_PART_BG, &roller_style_bg);
    lv_obj_add_style(roller, LV_ROLLER_PART_SELECTED, &roller_style_select);
    lv_roller_set_options(roller, options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_auto_fit(roller, false);
    lv_obj_set_width(roller, SCREEN_W - 20);
    resolution = lv_roller_get_option_cnt(roller);
    lv_roller_set_visible_row_count(roller, resolution == 2 ? 2 : 3);
    lv_obj_align(roller, item_screen, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(roller, EsmBaseItem::item_event_cb);


    continue_button = lv_btn_create(item_screen, nullptr);
    lv_obj_add_style(continue_button, LV_BTN_PART_MAIN, &button_style);
    lv_obj_set_event_cb(continue_button, EsmBaseItem::item_event_cb);
    lv_obj_align(continue_button, item_screen, LV_ALIGN_IN_BOTTOM_MID, 0, -15);
    if(mandatory){
        lv_btn_set_state(continue_button, LV_BTN_STATE_DISABLED);
        lv_obj_set_click(continue_button, false);
    }

    lv_obj_t* button_label = lv_label_create(continue_button, nullptr);
    lv_obj_add_style(button_label, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text(button_label, get_localized_string(LocalStrings::QUESTIONNAIRE_CONTINUE));
}


void EsmItemOptions::update(lv_obj_t* obj, lv_event_t event) {
    if(event == LV_EVENT_PRESSED){
        if(unset){
            unset = false;
            lv_obj_set_click(continue_button, true);
            lv_btn_set_state(continue_button, LV_BTN_STATE_RELEASED);
            value = lv_roller_get_selected(roller) + 1;
        }
    }else if(event == LV_EVENT_VALUE_CHANGED){
        value = lv_roller_get_selected(roller) + 1;
    }
}


void EsmItemOptions::get_result(ItemResult& result) {
    EsmBaseItem::get_result(result);
    result.resolution = &resolution;
    result.type = ItemType::OPTIONS;
}