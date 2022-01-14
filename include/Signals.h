#ifndef ES_SIGNALS_H
#define ES_SIGNALS_H

#include <TimeLib.h>
#include "const_defines.h"
#include "config.h"


enum class SignalLogType {SIGNAL, REPEAT, REACTION, DECLINED};


struct SignalTime {
public:
    SignalTime(){
        trigger_time = 0;
        expiry_time = 0;
        expiry_seconds = 0;
        repeat = 0;
        is_repeat = false;
        is_active = false;
        strcpy(questionnaire_name, "");
    };
    time_t trigger_time;
    time_t expiry_time;
    uint16_t expiry_seconds;
    uint8_t repeat;
    bool is_repeat;
    bool is_active;
    char questionnaire_name[NAMELENGTH];
};


class Signals {
public:
    Signals();
    static const char* signal_log_header;
    void reset();
    void generate(time_t* current_time = nullptr);
    void log_signal(SignalTime* signal, SignalLogType log_type);
    bool check_times(time_t current_time);
    void get_next_active_signal(char* name);
    void remove_next_active_signal(char* name, bool accepted);
    bool get_next_trigger(time_t &trigger, time_t current_time);

    //debug
    void print_signals_to_serial();
    char* study_name;

private:
    void check_expiry(time_t time);
    void add_signal_time(SignalTime* signal);
    time_t get_next_event_time(SignalTime* signal);
    SignalTime* times[MAXALARMS];
    void reorder();
};


#endif // ES_SIGNALS_H