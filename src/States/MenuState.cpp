#include "MenuState.h"
#include <ArduinoJson.h>
#include "SafeSD.h"
#include "Styles.h"

MenuState* MenuState::active_menu_state = nullptr;


MenuState::MenuState(ExperienceSampler* parent, char* state_info) : State(parent, state_info) {
    type = StateType::menu;
    build();
    active_menu_state = this;
}


MenuState::~MenuState(){
    lv_obj_del_async(menu_screen);
    active_menu_state = nullptr;
}


StringList MenuState::get_event_list(){
    StringList event_list;

    File file = open_file_safely(CONFIGFILE, (const char*) FILE_READ);

    StaticJsonDocument<128> filter;
    filter["name"] = true;
    filter["active"] = true;
    filter["event"]  = true;

    file.find("\"data\":");
    file.find("[");

    StaticJsonDocument<256> doc;

    do{
        deserializeJson(doc, file, DeserializationOption::Filter(filter));

        if(doc["active"].as<bool>() && doc["event"].as<bool>()){
            // todo: StringList overflow safeguard
            strcpy(event_list.list[event_list.length++], doc["name"].as<const char*>());
        }
    }while(file.findUntil(",","]"));

    file.close();

    return event_list;
}


void MenuState::menu_state_cb(lv_obj_t* obj, lv_event_t event){
    active_menu_state->menu_state_cb_local(obj, event);
}


void MenuState::menu_state_cb_local(lv_obj_t* obj, lv_event_t event){
    if(event == LV_EVENT_CLICKED){
        if(obj == settings_button){
            parent->request_next_state(StateType::settings, (char*) "");
        }else{
            parent->request_next_state(StateType::questionnaire, (char*) lv_list_get_btn_text(obj));
        }
    }
}


void MenuState::build(){
    StringList event_list = get_event_list();
    if(event_list.length == 0){
        build_alternate();
        return;
    }

    date_label = nullptr;

    menu_screen = lv_cont_create(lv_scr_act(), nullptr);
    lv_obj_add_style(menu_screen, LV_OBJ_PART_MAIN, &container_style);
    lv_obj_set_size(menu_screen, SCREEN_W, SCREEN_H);
    lv_obj_set_pos(menu_screen, 0, 0);

    lv_obj_t* top_bar = lv_cont_create(menu_screen, nullptr);
    lv_obj_add_style(top_bar, LV_OBJ_PART_MAIN, &container_style);
    lv_obj_set_style_local_bg_color(top_bar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, KL_COLOR_BLUE);
    lv_obj_set_size(top_bar, SCREEN_W, 30);
    lv_obj_align(top_bar, menu_screen, LV_ALIGN_IN_TOP_MID, 0, 0);

    clock_label = lv_label_create(top_bar, nullptr);
    lv_obj_add_style(clock_label, LV_OBJ_PART_MAIN, &label_white_style);
    lv_obj_align(clock_label, top_bar, LV_ALIGN_IN_RIGHT_MID, -15, 0);

    battery_symbol = lv_label_create(top_bar, nullptr);
    lv_obj_add_style(battery_symbol, LV_OBJ_PART_MAIN, &label_white_style);
    lv_obj_align(battery_symbol, top_bar, LV_ALIGN_IN_LEFT_MID, 15, 0);

    power_label = lv_label_create(top_bar, nullptr);
    lv_obj_add_style(power_label, LV_OBJ_PART_MAIN, &label_white_style);
    

    charge_symbol = lv_label_create(top_bar, nullptr);
    lv_obj_add_style(charge_symbol, LV_OBJ_PART_MAIN, &label_white_style);
    lv_obj_set_style_local_text_color(charge_symbol, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, KL_COLOR_RED);

    settings_button = lv_btn_create(top_bar, nullptr);
    lv_coord_t h = lv_obj_get_height(top_bar);
    lv_obj_add_style(settings_button, LV_BTN_PART_MAIN, &settings_button_style);
    lv_obj_set_size(settings_button, h, h);
    lv_obj_set_event_cb(settings_button, MenuState::menu_state_cb);
    lv_obj_align(settings_button, clock_label, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    
    lv_obj_t* list = lv_list_create(menu_screen, nullptr);
    lv_obj_add_style(list, LV_LIST_PART_BG, &container_style);
    lv_obj_set_size(list, SCREEN_W, SCREEN_H - 30);
    lv_obj_align(list, menu_screen, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

    lv_obj_t* list_btn;
    lv_obj_t* list_lab;

    for(uint8_t i = 0; i < event_list.length; i++){
        list_btn = lv_list_add_btn(list, nullptr, event_list.list[i]);
        lv_obj_add_style(list_btn, LV_BTN_PART_MAIN, &list_button_style);
        lv_obj_set_event_cb(list_btn, MenuState::menu_state_cb);
        list_lab = lv_list_get_btn_label(list_btn);
        lv_obj_add_style(list_lab, LV_OBJ_PART_MAIN, &label_style);
        lv_coord_t h = lv_obj_get_height(list_lab);
        lv_obj_set_height(list_btn, h);
    }
    refresh();
}


void MenuState::build_alternate() {
    menu_screen = lv_cont_create(lv_scr_act(), nullptr);
    lv_obj_add_style(menu_screen, LV_OBJ_PART_MAIN, &container_style);
    lv_obj_set_style_local_bg_color(menu_screen, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, KL_COLOR_BLUE);
    lv_obj_set_size(menu_screen, SCREEN_W, SCREEN_H);
    lv_obj_set_pos(menu_screen, 0, 0);

    clock_label = lv_label_create(menu_screen, nullptr);
    lv_obj_add_style(clock_label, LV_OBJ_PART_MAIN, &label_big_style);
    lv_obj_set_width(clock_label, SCREEN_W);
    lv_obj_align(clock_label, menu_screen, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_align(clock_label, LV_LABEL_ALIGN_CENTER);

    date_label = lv_label_create(menu_screen, nullptr);
    lv_obj_add_style(date_label, LV_OBJ_PART_MAIN, &label_white_style);
    lv_obj_align(date_label, menu_screen, LV_ALIGN_IN_BOTTOM_MID, 0, -16);
    lv_obj_set_width(date_label, SCREEN_W);
    lv_label_set_align(date_label, LV_LABEL_ALIGN_CENTER);

    battery_symbol = lv_label_create(menu_screen, nullptr);
    lv_obj_add_style(battery_symbol, LV_OBJ_PART_MAIN, &label_white_style);
    lv_obj_align(battery_symbol, menu_screen, LV_ALIGN_IN_TOP_LEFT, 16, 16);

    power_label = lv_label_create(menu_screen, nullptr);
    lv_obj_add_style(power_label, LV_OBJ_PART_MAIN, &label_white_style);
    
    charge_symbol = lv_label_create(menu_screen, nullptr);
    lv_obj_add_style(charge_symbol, LV_OBJ_PART_MAIN, &label_white_style);
    lv_obj_set_style_local_text_color(charge_symbol, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, KL_COLOR_RED);

    settings_button = lv_btn_create(menu_screen, nullptr);
    lv_obj_add_style(settings_button, LV_BTN_PART_MAIN, &settings_button_style);
    lv_obj_set_size(settings_button, 30, 30);
    lv_obj_set_event_cb(settings_button, MenuState::menu_state_cb);
    lv_obj_align(settings_button, menu_screen, LV_ALIGN_IN_TOP_RIGHT, -16, 8);

    refresh(true);
}


void MenuState::refresh(bool alternate){
    TTGOClass* watch = TTGOClass::getWatch();
    AXP20X_Class* power = watch->power;
    
    RTC_Date time = watch->rtc->getDateTime();

    lv_label_set_text_fmt(clock_label, "%2d:%02d", time.hour, time.minute);
    if(date_label != nullptr){
        lv_label_set_text_fmt(date_label, "%2d.%2d.%4d", time.day, time.month, time.year);
    }

    uint8_t percent = power->getBattPercentage();

    lv_obj_set_style_local_text_color(battery_symbol, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    if(percent >= 90){
        lv_label_set_text(battery_symbol, LV_SYMBOL_BATTERY_FULL);
    }else if(percent >= 60){
        lv_label_set_text(battery_symbol, LV_SYMBOL_BATTERY_3);
    }else if(percent >=40){
        lv_label_set_text(battery_symbol, LV_SYMBOL_BATTERY_2);
    }else if(percent >= 10){
        lv_label_set_text(battery_symbol, LV_SYMBOL_BATTERY_1);
        lv_obj_set_style_local_text_color(battery_symbol, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, KL_COLOR_RED);
    }else{
        lv_label_set_text(battery_symbol, LV_SYMBOL_BATTERY_EMPTY);
        lv_obj_set_style_local_text_color(battery_symbol, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, KL_COLOR_RED);

    }

    lv_label_set_text_fmt(power_label, "%d %%", percent);

    lv_label_set_text(charge_symbol, power->isChargeing() ? LV_SYMBOL_CHARGE : "");

    lv_obj_align(power_label, battery_symbol, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_align(charge_symbol, power_label, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    if(alternate) {
        lv_obj_align(clock_label, menu_screen, LV_ALIGN_CENTER, 0, 0);
        lv_obj_align(date_label, menu_screen, LV_ALIGN_IN_BOTTOM_MID, 0, -16);
    }
}