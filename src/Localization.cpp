#include "Localization.h"
#include "DefaultLocale.h"
#include "config.h"
#include "const_defines.h"
#include "SafeSD.h"
#include "ArduinoJson.h"


bool get_string_from_file(const char* key, char* s){
    #if defined LILYGO_WATCH_2020_V1 || defined V2_USE_SPIFFS
        if(!SPIFFS.exists(LOCALEFILE)){
            return false;
        }
    #else
        if(!SD.exists(LOCALEFILE)){
            return false;
        }
    #endif
    
    File file = open_file_safely(LOCALEFILE, FILE_READ);
    if(!file){
        return false;
    }

    StaticJsonDocument<128> filter;
    filter[key] = true;

    StaticJsonDocument<256> doc;

    deserializeJson(doc, file, DeserializationOption::Filter(filter));

    if(doc[key].as<const char*>() != nullptr){
        strcpy(s, doc[key].as<const char*>());
        return true;
    }
    return false;
}


char* get_localized_string(LocalStrings loc_str){
    static char localized_string[64];
    switch (loc_str)
    {
    case LocalStrings::QUESTIONNAIRE_CONTINUE:
        if(!get_string_from_file("questionnaire_continue", localized_string)){
            strcpy(localized_string, DEFAULT_QUESTIONNAIRE_CONTINUE);
        }
        break;
    case LocalStrings::SETTINGS_SET_DATE:
        if(!get_string_from_file("settings_set_date", localized_string)){
            strcpy(localized_string, DEFAULT_SETTINGS_SET_DATE);
        }
        break;
    case LocalStrings::SETTINGS_SET_TIME:
        if(!get_string_from_file("settings_set_time", localized_string)){
            strcpy(localized_string, DEFAULT_SETTINGS_SET_TIME);
        }
        break;
    case LocalStrings::SETTINGS_DOWNLOAD_DATA:
        if(!get_string_from_file("settings_download_data", localized_string)){
            strcpy(localized_string, DEFAULT_SETTINGS_DOWNLOAD_DATA);
        }
        break;
    case LocalStrings::SETTINGS_BACK:
        if(!get_string_from_file("settings_back", localized_string)){
            strcpy(localized_string, DEFAULT_SETTINGS_BACK);
        }
        break;
    case LocalStrings::QUESTIONNAIRE_COMPLETED:
        if(!get_string_from_file("questionnaire_completed", localized_string)){
            strcpy(localized_string, DEFAULT_QUESTIONNAIRE_COMPLETED);
        }
        break;
    case LocalStrings::NOTIFICATION:
        if(!get_string_from_file("notification", localized_string)){
            strcpy(localized_string, DEFAULT_NOTIFICATION);
        }
        break;
    case LocalStrings::NOTIFICATION_START:
        if(!get_string_from_file("notification_start", localized_string)){
            strcpy(localized_string, DEFAULT_NOTIFICATION_START);
        }
        break;
    case LocalStrings::DOWNLOAD_DATA_LOG:
        if(!get_string_from_file("download_data_log", localized_string)){
            strcpy(localized_string, DEFAULT_DOWNLOAD_DATA_LOG);
        }
        break;
    case LocalStrings::DOWNLOAD_SIGNAL_LOG:
        if(!get_string_from_file("download_signal_log", localized_string)){
            strcpy(localized_string, DEFAULT_DOWNLOAD_SIGNAL_LOG);
        }
        break;
    }

    return localized_string;
}