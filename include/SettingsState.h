#ifndef ES_SETTINGS_STATE_H
#define ES_SETTINGS_STATE_H

#include "ExperienceSampler.h"
#include "config.h"

#ifdef LILYGO_WATCH_2020_V1
enum class SettingsItems {SET_DATE = 0, SET_TIME, DOWNLOAD, BACK};
#else
enum class SettingsItems {SET_DATE = 0, SET_TIME, BACK};
#endif


class SettingsItem;


class SettingsState : public State {
public:
    SettingsState(ExperienceSampler* parent, char* state_info = (char*) "");
    ~SettingsState();

    static void settings_state_cb(lv_obj_t* obj, lv_event_t event);
    void settings_state_cb_local(lv_obj_t* obj, lv_event_t event);
    void refresh();
    ExperienceSampler* get_parent(){return parent;};
protected:
    static SettingsState* active_settings_state;
    SettingsItem* child_item;
    lv_obj_t* main_screen;
    lv_obj_t* top_bar;
    lv_obj_t* time_label;
    lv_obj_t* settings_list;
    #ifdef LILYGO_WATCH_2020_V1
    lv_obj_t* settings_buttons[4];
    #else
    lv_obj_t* settings_buttons[3];
    #endif
    void make_item_setting();
    void build();
};


class SettingsItem {
public:
    SettingsItem(SettingsState* parent);
    virtual ~SettingsItem();
    virtual bool make_setting(){return false;};
protected:
    virtual void build(){};
    SettingsState* parent;
    lv_obj_t* item_screen;
    TTGOClass* watch;
};


class SettingsItemDate : public SettingsItem {
public:
    SettingsItemDate(SettingsState* parent);
    ~SettingsItemDate();
    bool make_setting();
private:
    void build();
    lv_obj_t* calendar;
};


class SettingsItemTime : public SettingsItem {
public:
    SettingsItemTime(SettingsState* parent);
    ~SettingsItemTime();
    bool make_setting();
private:
    void build();
    lv_obj_t* hour_roller;
    lv_obj_t* minute_roller;
};

#if defined LILYGO_WATCH_2020_V1 || defined V2_USE_SPIFFS
class SettingsItemDownload : public SettingsItem {
public:
    SettingsItemDownload(SettingsState* parent);
    ~SettingsItemDownload();
    bool make_setting();
    static void settings_download_cb(lv_obj_t* obj, lv_event_t event);
    void settings_download_local(lv_obj_t* obj, lv_event_t event);
private:
    static SettingsItemDownload* active_settings_item_download;
    void build();
    lv_obj_t* back_button;
    lv_obj_t* download_data_log_button;
    lv_obj_t* download_signal_log_button;
    void download_data_log();
    void download_signal_log();
};
#endif


#endif // ES_SETTINGS_STATE_H