#include "QuestionnaireEndState.h"
#include "Styles.h"
#include "const_defines.h"
#include "Localization.h"


QuestionnaireEndState* QuestionnaireEndState::active_questionnaire_end_state = nullptr;

QuestionnaireEndState::QuestionnaireEndState(ExperienceSampler* parent, char* state_info) :
State(parent, state_info){
    type = StateType::questionnaire_end;
    active_questionnaire_end_state = this;
    build_screen();
}


QuestionnaireEndState::~QuestionnaireEndState() {
    active_questionnaire_end_state = nullptr;
    lv_obj_del_async(questionnaire_end_screen);
}


void QuestionnaireEndState::questionnaire_end_state_cb(lv_obj_t* obj, lv_event_t event) {
    if(event == LV_EVENT_PRESSED){
        active_questionnaire_end_state->advance_to_menu();
    }
}


void QuestionnaireEndState::advance_to_menu() {
    parent->request_next_state(StateType::menu);
}


void QuestionnaireEndState::build_screen() {
    questionnaire_end_screen = lv_cont_create(lv_scr_act(), nullptr);
    lv_obj_add_style(questionnaire_end_screen, LV_OBJ_PART_MAIN, &container_style);
    lv_obj_set_size(questionnaire_end_screen, SCREEN_W, SCREEN_H);
    lv_obj_set_pos(questionnaire_end_screen, 0, 0);

    lv_obj_t* lab = lv_label_create(questionnaire_end_screen, nullptr);
    lv_obj_add_style(lab, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text(lab, get_localized_string(LocalStrings::QUESTIONNAIRE_COMPLETED));
    lv_label_set_align(lab, LV_LABEL_ALIGN_CENTER);
    lv_label_set_long_mode(lab, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(lab, SCREEN_W - 20);
    lv_obj_align(lab, questionnaire_end_screen, LV_ALIGN_CENTER, 0, - SCREEN_H/6);
    
    lv_obj_t* btn = lv_btn_create(questionnaire_end_screen, nullptr);
    lv_obj_add_style(btn, LV_BTN_PART_MAIN, &button_style);
    lv_obj_set_event_cb(btn, QuestionnaireEndState::questionnaire_end_state_cb);
    
    lab = lv_label_create(btn, nullptr);
    lv_obj_add_style(lab, LV_LABEL_PART_MAIN, &label_style);
    lv_label_set_text(lab, get_localized_string(LocalStrings::QUESTIONNAIRE_CONTINUE));

    lv_obj_align(btn, questionnaire_end_screen, LV_ALIGN_IN_BOTTOM_MID, 0, - 15);
}