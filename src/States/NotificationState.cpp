#include "NotificationState.h"
#include "Styles.h"
#include "Localization.h"

NotificationState* NotificationState::active_notification_state = nullptr;


NotificationState::NotificationState(ExperienceSampler* parent, char* state_info)
: State(parent, state_info) {
    type = StateType::notification;
    strcpy(questionnaire_name, state_info);
    active_notification_state = this;
    build();
    parent->trigger_alarm();
}


NotificationState::~NotificationState(){
    lv_obj_del_async(notification_screen);
    active_notification_state = nullptr;
}


void NotificationState::build(){
    notification_screen = lv_cont_create(lv_scr_act(), nullptr);
    lv_obj_add_style(notification_screen, LV_OBJ_PART_MAIN, &container_style);
    lv_obj_set_size(notification_screen, SCREEN_W, SCREEN_H);
    lv_obj_align(notification_screen, nullptr, LV_ALIGN_CENTER, 0, 0);

    char label_text[128];
    sprintf(label_text, get_localized_string(LocalStrings::NOTIFICATION), questionnaire_name);
    
    lv_obj_t* lab = lv_label_create(notification_screen, nullptr);
    lv_obj_add_style(lab, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text(lab, label_text);
    lv_label_set_long_mode(lab, LV_LABEL_LONG_BREAK);
    lv_label_set_align(lab, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_width(lab, SCREEN_W - 20);
    lv_obj_align(lab, notification_screen, LV_ALIGN_CENTER, 0, 0);

    accept_button = lv_btn_create(notification_screen, nullptr);
    lv_obj_add_style(accept_button, LV_BTN_PART_MAIN, &button_style);
    lv_obj_set_event_cb(accept_button, NotificationState::notification_state_cb);
    lv_obj_align(accept_button, notification_screen, LV_ALIGN_IN_BOTTOM_MID, 0, -15);
    lv_btn_set_fit(accept_button, LV_FIT_TIGHT);

    lab = lv_label_create(accept_button, nullptr);
    lv_obj_add_style(lab, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text(lab, get_localized_string(LocalStrings::NOTIFICATION_START));

    cancel_button = lv_btn_create(notification_screen, nullptr);
    lv_obj_set_style_local_value_str(cancel_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_CLOSE);
    lv_obj_set_style_local_value_color(cancel_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, KL_COLOR_RED);
    lv_obj_set_style_local_bg_color(cancel_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_bg_color(cancel_button, LV_BTN_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_obj_set_style_local_outline_opa(cancel_button, LV_BTN_PART_MAIN, LV_STATE_PRESSED, LV_OPA_TRANSP);
    lv_obj_set_style_local_border_color(cancel_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, KL_COLOR_BLUE);
    lv_obj_set_size(cancel_button, 40, 40);
    lv_obj_set_event_cb(cancel_button, NotificationState::notification_state_cb);
    lv_obj_align(cancel_button, notification_screen, LV_ALIGN_IN_TOP_RIGHT, -15, 15);
}


void NotificationState::handle_button_input(lv_obj_t* btn){
    bool accepted = false;
    if(btn == accept_button){
        parent->request_next_state(StateType::questionnaire, questionnaire_name);
        accepted = true;
    }else if(btn == cancel_button){
        parent->request_next_state(StateType::menu);
    }
    parent->activated_signal(questionnaire_name, accepted);
}


void NotificationState::notification_state_cb(lv_obj_t* obj, lv_event_t event){
    if(event == LV_EVENT_CLICKED){
        active_notification_state->handle_button_input(obj);
    }
}