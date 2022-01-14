#ifndef ES_SAFE_SD_H
#define ES_SAFE_SD_H

#include "config.h"

#ifdef LILYGO_WATCH_2020_V1
    #include "FS.h"
    #include "SPIFFS.h"

    using fs::File;

#endif

enum class SDFailState {NO_SD_INIT, NO_SD_FOUND, NO_CONFIG_FILE, NO_SPIFFS};

File open_file_safely(const char* path, const char* mode);
lv_obj_t* display_SD_message(SDFailState fail_state);

#endif // ES_SAFE_SD_H