#ifndef ES_ITEMS_H
#define ES_ITEMS_H

#include "Arduino.h"
#include "lvgl/lvgl.h"
#include "config.h"
#include "ItemDefinitions.h"
#include "ItemResult.h"
#include "const_defines.h"

class QuestionnaireState;


class EsmBaseItem {
public:
    EsmBaseItem(ItemDefinition &item_def);
    virtual ~EsmBaseItem();
    virtual void update(lv_obj_t* obj, lv_event_t event) {};
    virtual void get_result(ItemResult& result);
    void activate();
    void deactivate();
    void mark_complete();

    static const char* header;
    static void item_event_cb(lv_obj_t* obj, lv_event_t event);
    lv_obj_t* continue_button;
protected:
    static EsmBaseItem* active_item;
    QuestionnaireState* parent_questionnaire;
    lv_obj_t* item_screen;
    char prompt[PROMPTLENGTH];
    char item_name[NAMELENGTH];

    int16_t value;
    bool unset;
    bool mandatory;
    unsigned long start_time;
    unsigned long complete_time;

    virtual void build(){};
};


class EsmItemVAS : public EsmBaseItem {
public:
    EsmItemVAS(ItemDefinition &item_def);
    ~EsmItemVAS(){};
    void update(lv_obj_t* obj, lv_event_t event);
    void get_result(ItemResult& result);

private:
    lv_obj_t* vas;
    char left_label[LABELLENGTH];
    char right_label[LABELLENGTH];

    void build();
};


class EsmItemText : public EsmBaseItem {
public:
    EsmItemText(ItemDefinition &item_def);
    ~EsmItemText(){};
    void update(lv_obj_t* obj, lv_event_t event){};
    void get_result(ItemResult& result);
private:
    void build();
};


class EsmItemNumeric : public EsmBaseItem {
public:
    EsmItemNumeric(ItemDefinition &item_def);
    ~EsmItemNumeric(){};
    void update(lv_obj_t* obj, lv_event_t event);
    void get_result(ItemResult& result);
private:
    lv_obj_t* spinbox;
    lv_obj_t* increment_button;
    lv_obj_t* decrement_button;
    uint8_t num_digits;
    uint8_t separator_position;
    int16_t min_value;
    int16_t max_value;

    void build();
};


class EsmItemLikert : public EsmBaseItem {
public:
    EsmItemLikert(ItemDefinition &item_def);
    ~EsmItemLikert(){};
    void update(lv_obj_t* obj, lv_event_t event);
    void get_result(ItemResult& result);
private:
    lv_obj_t* matrix;
    char left_label[LABELLENGTH];
    char right_label[LABELLENGTH];
    uint8_t resolution;
    const char* map[MAXLIKERT+1];

    void build();
};


class EsmItemOptions : public EsmBaseItem {
public:
    EsmItemOptions(ItemDefinition &item_def);
    ~EsmItemOptions(){};
    void update(lv_obj_t* obj, lv_event_t event);
    void get_result(ItemResult& result);
private:
    lv_obj_t* roller;
    uint8_t resolution;
    char options[OPTIONSLENGTH];
    void build();
};

#endif // ES_ITMES_H