#ifndef ES_ITEM_DEFINITIONS_H
#define ES_ITEM_DEFINITIONS_H

#include "Arduino.h"
#include "const_defines.h"


class QuestionnaireState;


enum class ItemType {VAS, LIKERT, TEXT, NUMERIC, OPTIONS};


struct ItemDefinitionVAS {
    const char* left_label;
    const char* right_label;
};


struct ItemDefinitionLikert {
    const char* left_label;
    const char* right_label;
    uint8_t resolution;
};


struct ItemDefinitionNumeric {
    uint8_t num_digits;
    uint8_t separator_position;
    int16_t min_value;
    int16_t max_value;
};


struct ItemDefinitionOptions {
    const char* options;
};


struct ItemDefinition {
    ItemType type;
    QuestionnaireState* parent_questionnaire;
    const char* prompt;
    const char* item_name;
    bool mandatory;
    union{
        ItemDefinitionLikert info_likert;
        ItemDefinitionVAS info_vas;
        ItemDefinitionNumeric info_numeric;
        ItemDefinitionOptions info_options;
    };
};

#endif // ES_ITEM_DEFINITIONS_H