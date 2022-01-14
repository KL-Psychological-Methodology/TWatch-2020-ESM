#ifndef ES_ITEM_RESULT_H
#define ES_ITEM_RESULT_H

#include "ItemDefinitions.h"


struct ItemResult {
    ItemResult() : item_name(nullptr), prompt(nullptr), left_label(nullptr), right_label(nullptr), resolution(nullptr), response(nullptr), print_result(true), duration(0) {};
    char* item_name;
    ItemType type;
    char* prompt;
    char* left_label;
    char* right_label;
    uint8_t* resolution;
    int16_t* response;
    bool print_result;
    unsigned long duration;
};

//id;quest_name;item_name;response_time;latitude;longitude;type;prompt;left_label;right_label;resolution;response

#endif // ES_ITEM_RESULT_H