#ifndef ES_NOTIFICATION_STATE_H
#define ES_NOTIFICATION_STATE_H

#include "Arduino.h"
#include "config.h"
#include "ExperienceSampler.h"


class NotificationState : public State {
public:
    NotificationState(ExperienceSampler* parent, char* state_info = (char*) "");
    ~NotificationState();

    static void notification_state_cb(lv_obj_t* obj, lv_event_t event);
    void handle_button_input(lv_obj_t* btn);

private:
    static NotificationState* active_notification_state;
    lv_obj_t* notification_screen;
    lv_obj_t* accept_button;
    lv_obj_t* cancel_button;

    char questionnaire_name[NAMELENGTH];

    void build();
};


#endif // ES_NOTIFICATION_STATE_H