#ifndef ES_EXPERIENCE_SAMPLER_H
#define ES_EXPERIENCE_SAMPLER_H

#include "Arduino.h"
#include "config.h"
#include "const_defines.h"
#include "TimeLib.h"
#include "Signals.h"

enum class StateType {none, questionnaire, menu, notification, settings, questionnaire_end};

extern time_t alarm_generator_trigger;

class State;


class ExperienceSampler {
public:
    ExperienceSampler();
    void update();
    void request_next_state(StateType next, char* state_info = (char*) "");
    void enter_next_state();
    static bool irq;
    char* get_study_name();
    void reset_sleep();
    void reset_times();
    void trigger_alarm();
    void activated_signal(char* questionnarie_name, bool accepted);
    #ifdef LILYGO_WATCH_2020_V2
        void activate_gps();
        void deactivate_gps();
    #endif
    bool do_quick_return(){return quick_return;};

private:
    TTGOClass* watch;
    AXP20X_Class* power;
    #ifdef LILYGO_WATCH_2020_V2
        Adafruit_DRV2605* drv;
    #endif

    State* current_state;
    StateType next_state_type;
    char next_state_info[NAMELENGTH];
    char study_name[NAMELENGTH];

    void enter_sleep();
    void soft_reset();
    time_t get_time_t();
    void update_alarm_generation_trigger();
    void time_check(time_t current_time);
    void setup_wakeup_timer();
    bool gps_active;
    bool quick_return;

    time_t delta_t;
    time_t sleep_timer;
    Signals signals;
};


class State {
public:
    State(ExperienceSampler* parent, char* state_info = (char*) "");
    virtual ~State();
    StateType get_type(){return type;};

protected:
    ExperienceSampler* parent;
    StateType type;
};


#endif // ES_EXPERIENCE_SAMPLER_H