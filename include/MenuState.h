#ifndef ES_MENU_STATE_H
#define ES_MENU_STATE_H

#include "ExperienceSampler.h"
#include "const_defines.h"
#include "StringList.h"


class MenuState : public State {
public:
    MenuState(ExperienceSampler* parent, char* state_info = (char*) "");
    ~MenuState();

    void refresh(bool alternate = false);
    static void menu_state_cb(lv_obj_t* obj, lv_event_t event);
    void menu_state_cb_local(lv_obj_t* obj, lv_event_t event);
protected:
    lv_obj_t* menu_screen;
    lv_obj_t* clock_label;
    lv_obj_t* power_label;
    lv_obj_t* charge_symbol;
    lv_obj_t* battery_symbol;
    lv_obj_t* settings_button;
    lv_obj_t* date_label;
    static MenuState* active_menu_state;

    StringList get_event_list();
    void build();
    void build_alternate();
};


#endif // ES_MENU_STATE_H