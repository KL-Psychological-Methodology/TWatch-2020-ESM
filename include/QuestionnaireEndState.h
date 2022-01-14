#ifndef ES_QUESTIONNAIRE_END_STATE_H
#define ES_QUESTIONNAIRE_END_STATE_H

#include "ExperienceSampler.h"
#include "config.h"


class QuestionnaireEndState : public State {
public:
    QuestionnaireEndState(ExperienceSampler* parent, char* state_info = (char*) "");
    ~QuestionnaireEndState();

    static void questionnaire_end_state_cb(lv_obj_t* obj, lv_event_t event);
    void advance_to_menu();

private:
    static QuestionnaireEndState* active_questionnaire_end_state;
    lv_obj_t* questionnaire_end_screen;
    void build_screen();
};


#endif // ES_QUESTIONNAIRE_END_STATE_H