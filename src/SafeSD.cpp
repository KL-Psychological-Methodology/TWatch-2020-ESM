#include "SafeSD.h"
#include "const_defines.h"


File open_file_safely(const char* path, const char* mode){
    #if defined LILYGO_WATCH_2020_V1 || defined V2_USE_SPIFFS
        if(SPIFFS.exists(CONFIGFILE)){
            return SPIFFS.open(path, mode);
        }
    #else
        if(SD.exists(CONFIGFILE)){
            return SD.open(path, mode);
        }
    #endif

    bool path_is_config = strcmp(path, CONFIGFILE) == 0;

    lv_obj_t* current_screen = lv_scr_act();
    
    #if defined LILYGO_WATCH_2020_V1 || defined V2_USE_SPIFFS
        lv_obj_t* message_screen = display_SD_message(path_is_config ? SDFailState::NO_CONFIG_FILE : SDFailState::NO_SPIFFS);
    #else
        lv_obj_t* message_screen = display_SD_message(path_is_config ? SDFailState::NO_CONFIG_FILE : SDFailState::NO_SD_FOUND);
    #endif

    lv_task_handler();

    while(true){
        #if defined LILYGO_WATCH_2020_V1 || defined V2_USE_SPIFFS
        if(SPIFFS.begin(false) && SPIFFS.exists(CONFIGFILE)){
            break;
        }
        #else
        if(TTGOClass::getWatch()->sdcard_begin() && SD.exists(CONFIGFILE)){
            break;
        }
        #endif
        delay(1000);
    }
    lv_scr_load(current_screen);
    lv_obj_del(message_screen);
    #if defined LILYGO_WATCH_2020_V1 || defined V2_USE_SPIFFS
        return SPIFFS.open(path, mode);
    #else  
        return SD.open(path, mode);
    #endif
}


lv_obj_t* display_SD_message(SDFailState fail_state){
    lv_obj_t* message_screen = lv_cont_create(nullptr, nullptr);
    lv_obj_set_size(message_screen, SCREEN_W, SCREEN_H);
    lv_obj_set_pos(message_screen, 0, 0);

    lv_obj_t* lab = lv_label_create(message_screen, nullptr);
    switch (fail_state){
    case SDFailState::NO_CONFIG_FILE:
        lv_label_set_text(lab, "No configuration file found on SD card.");
        break;
    case SDFailState::NO_SD_FOUND:
        lv_label_set_text(lab, "No SD card found.");
        break;
    case SDFailState::NO_SD_INIT:
        lv_label_set_text(lab, "SD card failed to initialize");
        break;
    case SDFailState::NO_SPIFFS:
        lv_label_set_text(lab, "Could not mount SPIFFS");
        break;
    }
    lv_label_set_long_mode(lab, LV_LABEL_LONG_BREAK);
    lv_label_set_align(lab, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_width(lab, SCREEN_W - 40);
    lv_obj_align(lab, message_screen, LV_ALIGN_CENTER, 0, 0);

    lv_scr_load(message_screen);

    return message_screen;
}