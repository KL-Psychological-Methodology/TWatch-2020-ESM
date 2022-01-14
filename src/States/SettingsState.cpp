#include "SettingsState.h"
#include "TimeLib.h"
#include "const_defines.h"
#include "Localization.h"
#include "Styles.h"
#include "SafeSD.h"

SettingsState* SettingsState::active_settings_state = nullptr;


SettingsState::SettingsState(ExperienceSampler* parent, char* state_info) :
State(parent, state_info) {   
    type = StateType::settings;
    active_settings_state = this;
    child_item = nullptr;
    main_screen = lv_scr_act();
    build();
}


SettingsState::~SettingsState(){
    active_settings_state = nullptr;
    lv_obj_del_async(settings_list);
    if(child_item != nullptr){
        delete child_item;
    }
}


void SettingsState::build(){
    top_bar = lv_cont_create(main_screen, nullptr);
    lv_obj_add_style(top_bar, LV_OBJ_PART_MAIN, &container_style);
    lv_obj_set_style_local_bg_color(top_bar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, KL_COLOR_BLUE);
    lv_obj_set_size(top_bar, SCREEN_W, 30);
    lv_obj_align(top_bar, main_screen, LV_ALIGN_IN_TOP_MID, 0, 0);

    time_label = lv_label_create(main_screen, nullptr);
    lv_obj_add_style(time_label, LV_LABEL_PART_MAIN, &label_white_style);
    lv_label_set_align(time_label, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(time_label, top_bar, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* bottom_bar = lv_cont_create(main_screen, nullptr);
    lv_obj_add_style(bottom_bar, LV_OBJ_PART_MAIN, &container_style);
    lv_obj_set_style_local_bg_color(bottom_bar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, KL_COLOR_BLUE);
    lv_obj_set_style_local_bg_opa(bottom_bar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_20);
    lv_obj_set_size(bottom_bar, SCREEN_W, 30);
    lv_obj_align(bottom_bar, main_screen, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

    uint8_t mac[6];
    esp_efuse_mac_get_default(mac);
    lv_obj_t* mac_label = lv_label_create(bottom_bar, nullptr);
    lv_obj_add_style(mac_label, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text_fmt(mac_label, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    lv_obj_align(mac_label, bottom_bar, LV_ALIGN_IN_LEFT_MID, 15, 0);

    settings_list = lv_list_create(main_screen, nullptr);
    lv_obj_add_style(settings_list, LV_LIST_PART_BG, &container_style);
    lv_obj_set_size(settings_list, SCREEN_W, SCREEN_H - 60);
    lv_obj_align(settings_list, main_screen, LV_ALIGN_CENTER, 0, 0);

    LocalStrings string_values[] = {
        LocalStrings::SETTINGS_SET_DATE,
        LocalStrings::SETTINGS_SET_TIME,
    #ifdef LILYGO_WATCH_2020_V1
        LocalStrings::SETTINGS_DOWNLOAD_DATA,
    #endif
        LocalStrings::SETTINGS_BACK
    };
    SettingsItems item_order[] = {
        SettingsItems::SET_DATE,
        SettingsItems::SET_TIME,
    #ifdef LILYGO_WATCH_2020_V1
        SettingsItems::DOWNLOAD,
    #endif
        SettingsItems::BACK
    };

    #ifdef LILYGO_WATCH_2020_V1
    uint8_t list_length = 4;
    #else
    uint8_t list_length = 3;
    #endif

    lv_obj_t* list_btn;
    lv_obj_t* list_lab;

    for(uint8_t i = 0; i < list_length; i++){
        list_btn = lv_list_add_btn(settings_list, nullptr, get_localized_string(string_values[i]));
        lv_obj_add_style(list_btn, LV_BTN_PART_MAIN, &list_button_style);
        lv_obj_set_event_cb(list_btn, settings_state_cb);
        list_lab = lv_list_get_btn_label(list_btn);
        lv_obj_add_style(list_lab, LV_LABEL_PART_MAIN, &label_style);
        lv_obj_set_height(list_btn, lv_obj_get_height(list_lab));
        settings_buttons[static_cast<int>(item_order[i])] = list_btn;
    }

    refresh();
}


void SettingsState::refresh(){
    time_t current_time = now();
    TimeElements tm;
    breakTime(current_time, tm);

    lv_label_set_text_fmt(time_label, "%d.%d.%d %d:%02d", tm.Day, tm.Month, tm.Year + 1970, tm.Hour, tm.Minute);
    lv_obj_align(time_label, top_bar, LV_ALIGN_CENTER, 0, 0);
}


void SettingsState::settings_state_cb(lv_obj_t* obj, lv_event_t event){
    active_settings_state->settings_state_cb_local(obj, event);
}


void SettingsState::settings_state_cb_local(lv_obj_t* obj, lv_event_t event){
    lv_obj_type_t types;
    lv_obj_get_type(obj, &types);
    if(event == LV_EVENT_CLICKED){
        if(obj == settings_buttons[static_cast<int>(SettingsItems::SET_DATE)]){
            if(child_item != nullptr){
                delete child_item;
            }
            child_item = new SettingsItemDate(this);
        }else if(obj == settings_buttons[static_cast<int>(SettingsItems::SET_TIME)]){
            if(child_item != nullptr){
                delete child_item;
            }
            child_item = new SettingsItemTime(this);
        }else if(obj == settings_buttons[static_cast<int>(SettingsItems::BACK)]){
            parent->request_next_state(StateType::menu, (char*) "");
        }
        #ifdef LILYGO_WATCH_2020_V1
        else if(obj == settings_buttons[static_cast<int>(SettingsItems::DOWNLOAD)]){
            child_item = new SettingsItemDownload(this);
        }
        #endif
        else if(strcmp(types.type[0], "lv_btn") == 0){
            make_item_setting();
        }
    }else if(event == LV_EVENT_VALUE_CHANGED && strcmp(types.type[0], "lv_calendar") == 0){
        make_item_setting();
    }
}


void SettingsState::make_item_setting(){
    lv_scr_load(main_screen);
    if(child_item->make_setting()){
        parent->reset_times();
        refresh();
    }
}


SettingsItem::SettingsItem(SettingsState* parent){
    this->parent = parent;
    watch = TTGOClass::getWatch();
    item_screen = nullptr;
}


SettingsItem::~SettingsItem(){
    if(item_screen != nullptr){
        lv_obj_del_async(item_screen);
        item_screen = nullptr;
    }
}


SettingsItemDate::SettingsItemDate(SettingsState* parent) : SettingsItem(parent) {
    build();
    lv_scr_load(item_screen);
}


SettingsItemDate::~SettingsItemDate() {}


void SettingsItemDate::build(){
    item_screen = lv_cont_create(nullptr, nullptr);
    lv_obj_set_size(item_screen, SCREEN_W, SCREEN_H);
    lv_obj_add_style(item_screen, LV_OBJ_PART_MAIN, &container_style);
    
    calendar = lv_calendar_create(item_screen, nullptr);
    lv_obj_add_style(calendar, LV_CALENDAR_PART_BG, &container_style);
    lv_obj_add_style(calendar, LV_CALENDAR_PART_HEADER, &label_style);
    lv_obj_add_style(calendar, LV_CALENDAR_PART_DAY_NAMES, &label_style);
    lv_obj_add_style(calendar, LV_CALENDAR_PART_DATE, &calendar_style_dates);
    lv_obj_set_size(calendar, SCREEN_W, SCREEN_H);
    lv_obj_set_pos(calendar, 0, 0);

    lv_calendar_date_t today;
    RTC_Date rtc_today = watch->rtc->getDateTime();
    today.year = rtc_today.year;
    today.month = rtc_today.month;
    today.day = rtc_today.day;

    lv_calendar_set_today_date(calendar, &today);
    lv_calendar_set_showed_date(calendar, &today);
    lv_obj_set_event_cb(calendar, SettingsState::settings_state_cb);
}


bool SettingsItemDate::make_setting(){
    RTC_Date now = watch->rtc->getDateTime();
    lv_calendar_date_t* today = lv_calendar_get_pressed_date(calendar);
    if(!today){
        return false;
    }

    now.year = today->year;
    now.month = today->month;
    now.day = today->day;

    watch->rtc->setDateTime(now);
    setTime(now.hour, now.minute, now.second, now.day, now.month, now.year);
    return true;
}


SettingsItemTime::SettingsItemTime(SettingsState* parent) : SettingsItem(parent) {
    build();
    lv_scr_load(item_screen);
}


SettingsItemTime::~SettingsItemTime() {};


void SettingsItemTime::build() {
    item_screen = lv_cont_create(nullptr, nullptr);
    lv_obj_add_style(item_screen, LV_OBJ_PART_MAIN, &container_style);
    lv_obj_set_size(item_screen, SCREEN_W, SCREEN_H);

    lv_obj_t* lab = lv_label_create(item_screen, nullptr);
    lv_obj_add_style(lab, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text(lab, ":");
    lv_obj_align(lab, item_screen, LV_ALIGN_CENTER, 0, -SCREEN_H/6);

    hour_roller = lv_roller_create(item_screen, nullptr);
    lv_obj_add_style(hour_roller, LV_ROLLER_PART_BG, &roller_style_bg);
    lv_obj_add_style(hour_roller, LV_ROLLER_PART_SELECTED, &roller_style_select);
    lv_roller_set_options(
        hour_roller, 
        "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23", 
        LV_ROLLER_MODE_INIFINITE
    );
    lv_obj_align(hour_roller, lab, LV_ALIGN_OUT_LEFT_MID, -5, 0);

    minute_roller = lv_roller_create(item_screen, hour_roller);
    lv_roller_set_options(
        minute_roller,
        "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59",
        LV_ROLLER_MODE_INIFINITE
    );
    lv_obj_align(minute_roller, lab, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    RTC_Date now = watch->rtc->getDateTime();
    lv_roller_set_selected(hour_roller, now.hour, LV_ANIM_OFF);
    lv_roller_set_selected(minute_roller, now.minute, LV_ANIM_OFF);

    lv_obj_t* btn = lv_btn_create(item_screen, nullptr);
    lv_obj_add_style(btn, LV_BTN_PART_MAIN, &button_style);
    lv_obj_set_event_cb(btn, SettingsState::settings_state_cb);

    lab = lv_label_create(btn, nullptr);
    lv_obj_add_style(lab, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text(lab, get_localized_string(LocalStrings::SETTINGS_SET_TIME));
    
    lv_obj_align(btn, item_screen, LV_ALIGN_IN_BOTTOM_MID, 0, -20);
}


bool SettingsItemTime::make_setting(){
    RTC_Date now = watch->rtc->getDateTime();

    now.hour = lv_roller_get_selected(hour_roller);
    now.minute = lv_roller_get_selected(minute_roller);

    watch->rtc->setDateTime(now);
    setTime(now.hour, now.minute, now.second, now.day, now.month, now.year);
    return true;
}


#if defined LILYGO_WATCH_2020_V1 || defined V2_USE_SPIFFS

SettingsItemDownload* SettingsItemDownload::active_settings_item_download = nullptr;

SettingsItemDownload::SettingsItemDownload(SettingsState* parent) : SettingsItem(parent) {
    build();
    lv_scr_load(item_screen);
    active_settings_item_download = this;
}


SettingsItemDownload::~SettingsItemDownload() {
    active_settings_item_download = nullptr;
}


bool SettingsItemDownload::make_setting() {
    return false;
}


void SettingsItemDownload::build() {
    item_screen = lv_cont_create(nullptr, nullptr);
    lv_obj_add_style(item_screen, LV_OBJ_PART_MAIN, &container_style);
    lv_obj_set_size(item_screen, SCREEN_W, SCREEN_H);

    download_data_log_button = lv_btn_create(item_screen, nullptr);
    lv_obj_add_style(download_data_log_button, LV_BTN_PART_MAIN, &button_style);
    lv_obj_set_event_cb(download_data_log_button, SettingsItemDownload::settings_download_cb);

    lv_obj_t* lab = lv_label_create(download_data_log_button, nullptr);
    lv_obj_add_style(lab, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text(lab, "Download Data");
    
    lv_obj_align(download_data_log_button, item_screen, LV_ALIGN_CENTER, 0, - SCREEN_H/4);

    download_signal_log_button = lv_btn_create(item_screen, download_data_log_button);

    lab = lv_label_create(download_signal_log_button, nullptr);
    lv_obj_add_style(lab, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text(lab, "Download Signal Log");

    lv_obj_align(download_signal_log_button, item_screen, LV_ALIGN_CENTER, 0, 0);

    back_button = lv_btn_create(item_screen, download_data_log_button);

    lab = lv_label_create(back_button, nullptr);
    lv_obj_add_style(lab, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text(lab, "Back");

    lv_obj_align(back_button, item_screen, LV_ALIGN_CENTER, 0, SCREEN_H/4);
}


void SettingsItemDownload::settings_download_cb(lv_obj_t* obj, lv_event_t event) {
    active_settings_item_download->settings_download_local(obj, event);
}


void SettingsItemDownload::settings_download_local(lv_obj_t* obj, lv_event_t event) {
    if(event == LV_EVENT_CLICKED){
        if(obj == download_data_log_button){
            download_data_log();
        }else if(obj == download_signal_log_button){
            download_signal_log();
        }else if(obj == back_button){
            SettingsState::settings_state_cb(obj, event);
        }
    }
}


void SettingsItemDownload::download_data_log() {
    char filename[NAMELENGTH+8];
    
    char* name = parent->get_parent()->get_study_name();

    if(strcmp(name, "") == 0){
        strcpy(filename, DATAFILE);
    }else{
        sprintf(filename, "/%s.csv", name);
    }

    File file = SPIFFS.open(filename, FILE_READ);

    Serial.begin(115200);
    if(!Serial){
        if(file){
            file.close();
        }
        return;
    }

    if(!file){
        Serial.printf("No file %s\n\n", filename);
        Serial.flush();
        Serial.end();
        return;
    }

    Serial.printf("----- %s -----\n\n\n", filename);

    while(file.available()){
        Serial.write((char) file.read());
    }

    Serial.println("\n\n----------");
    Serial.flush();
    Serial.end();
    file.close();
}


void SettingsItemDownload::download_signal_log() {
    File file = SPIFFS.open(SIGNALLOGFILE, FILE_READ);

    Serial.begin(115200);
    if(!Serial){
        if(file){
            file.close();
        }
        return;
    }

    if(!file){
        Serial.printf("No file %s\n\n\n", SIGNALLOGFILE);
        Serial.flush();
        Serial.end();
        return;
    }
    Serial.printf("----- %s -----\n\n", SIGNALLOGFILE);

    while(file.available()){
        Serial.write((char) file.read());
    }

    Serial.println("\n\n----------");
    Serial.flush();
    Serial.end();
    file.close();
}


#endif