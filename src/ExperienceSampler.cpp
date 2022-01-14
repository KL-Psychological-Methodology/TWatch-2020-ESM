#include "ExperienceSampler.h"
#include "ArduinoJson.h"

#include "QuestionnaireState.h"
#include "QuestionnaireEndState.h"
#include "MenuState.h"
#include "NotificationState.h"
#include "SettingsState.h"
#include "SafeSD.h"
#include "Styles.h"
#include "WiFi.h"
#include "esp32-hal-bt.h"
#include "esp_task_wdt.h"


bool ExperienceSampler::irq = false;

time_t alarm_generator_trigger = 0;


ExperienceSampler::ExperienceSampler() {
    setCpuFrequencyMhz(CPU_ACTIVE_SPEED);
    esp_task_wdt_init(20000, false);

    current_state = nullptr;
    gps_active = false;

    watch = TTGOClass::getWatch();

    watch->begin();
    watch->lvgl_begin();
    watch->openBL();
    init_styles();

    WiFi.mode(WIFI_OFF);
    btStop();

    power = watch->power;
    #ifdef LILYGO_WATCH_2020_V2
        drv = watch->drv;
    #endif

    bool message_created = false;
    lv_obj_t* current_screen = lv_scr_act();
    lv_obj_t* message_screen;
    #if defined LILYGO_WATCH_2020_V1 || defined V2_USE_SPIFFS
        while(true){
            if(SPIFFS.begin(false)){
                break;
            }
            if(!message_created){
                message_created = true;
                message_screen = display_SD_message(SDFailState::NO_SD_INIT);
                lv_task_handler();
            }
            delay(1000);
        }
    #else
        while(true){
            if(watch->sdcard_begin()){
                break;
            }
            if(!message_created){
                message_created = true;
                message_screen = display_SD_message(SDFailState::NO_SD_INIT);
                lv_task_handler();
            }
            delay(1000);
        }
    #endif
    if(message_created){
        lv_scr_load(current_screen);
        lv_obj_del(message_screen);
    }

    // esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);

    pinMode(AXP202_INT, INPUT_PULLUP);
    attachInterrupt(AXP202_INT, []{
        ExperienceSampler::irq = true;
    }, FALLING);

    power->adc1Enable(
        AXP202_VBUS_VOL_ADC1 |
        AXP202_VBUS_CUR_ADC1 |
        AXP202_BATT_CUR_ADC1 |
        AXP202_BATT_VOL_ADC1,
        true
    );
    power->enableIRQ(
        AXP202_PEK_SHORTPRESS_IRQ |
        AXP202_VBUS_REMOVED_IRQ |
        AXP202_VBUS_CONNECT_IRQ |
        AXP202_CHARGING_IRQ,
        true
    );
    power->clearIRQ();

    #ifdef LILYGO_WATCH_2020_V2
        watch->enableDrv2650();
        drv->selectLibrary(1);
        drv->setMode(DRV2605_MODE_INTTRIG);
        // drv->setWaveform(0, DRV_EFFECT);
        // drv->setWaveform(1, 0);
        drv->setWaveform(0, 93);
        drv->setWaveform(1, 15);
        drv->setWaveform(2, 93);
        drv->setWaveform(3, 15);
        drv->setWaveform(4, 0);
    #else
        watch->motor_begin();
    #endif

    File file = open_file_safely(CONFIGFILE, (const char*) FILE_READ);
    
    if(!file){
        return;
    }

    StaticJsonDocument<128> filter;
    filter["study_name"] = true;
    filter["quick_return"] = true;

    StaticJsonDocument<256> doc;
    deserializeJson(doc, file, DeserializationOption::Filter(filter));

    strcpy(study_name, doc["study_name"].as<const char*>());
    quick_return = doc["quick_return"].as<bool>();
    signals.study_name = study_name;


    // debug
    // watch->rtc->setDateTime(2021, 8, 17, 8, 0, 30);
    // --
    
    setTime(get_time_t());

    time_t current_time = now();

    next_state_type = StateType::menu;
    strcpy(next_state_info, "");

    update_alarm_generation_trigger();
    signals.generate(&current_time);

    sleep_timer = current_time + SLEEPTIME;
    delta_t = current_time + DELTATIME;

    enter_next_state();

}


void ExperienceSampler::request_next_state(StateType next, char* state_info) {
    next_state_type = next;
    strcpy(next_state_info, state_info);
}


void ExperienceSampler::enter_next_state(){
    if(current_state != nullptr){
        delete current_state;
        current_state = nullptr;
    }
    switch(next_state_type){
        case StateType::questionnaire:
            current_state = new QuestionnaireState(this, next_state_info);
            break;
        case StateType::notification:
            current_state = new NotificationState(this, next_state_info);
            break;
        case StateType::settings:
            // Serial.println("Switching to settings state");
            current_state = new SettingsState(this, next_state_info);
            break;
        case StateType::questionnaire_end:
            current_state = new QuestionnaireEndState(this, next_state_info);
            break;
        default:
            current_state = new MenuState(this, next_state_info);
            break;
    }
    next_state_type = StateType::none;
    strcpy(next_state_info, "");
}


void ExperienceSampler::update(){
    time_t current_time = now();

    if(current_time >= delta_t){
        delta_t = current_time + DELTATIME;
        time_check(current_time);
        if(current_time >= alarm_generator_trigger){
            update_alarm_generation_trigger();
            signals.generate(&current_time);
        }

        StateType current_state_type = current_state->get_type();
        if(current_state_type == StateType::menu){
            ((MenuState*)current_state)->refresh();
        }else if(current_state_type == StateType::settings){
            ((SettingsState*)current_state)->refresh();
        }
    }

    int16_t x = 0, y = 0;
    if(watch->getTouch(x, y)){
        reset_sleep();
    }

    if(next_state_type != StateType::none){
        enter_next_state();
    }
    if(current_time >= sleep_timer && current_state->get_type() != StateType::questionnaire){
        enter_sleep();
        return;
    }
    if(irq){
        irq = false;
        power->readIRQ();
        if(power->isPEKShortPressIRQ()){
            enter_sleep();
            return;
        }else if(current_state->get_type() == StateType::menu){
            ((MenuState*)current_state)->refresh();
        }
        power->clearIRQ();
    }
    lv_task_handler();
    
    #ifdef LILYGO_WATCH_2020_V2
        if(gps_active){
            watch->gpsHandler();
        }
    #endif

    delay(FRAMETIME);
}


void ExperienceSampler::reset_sleep(){
    sleep_timer = now() + SLEEPTIME;
}


void ExperienceSampler::enter_sleep(){
    power->clearIRQ();

    request_next_state(StateType::menu);
    time_check(now());
    enter_next_state();
    if(current_state->get_type() == StateType::notification){
        return;
    }
    
    watch->closeBL();

    lv_task_handler();
    watch->stopLvglTick();
    watch->displaySleep();
    
    #ifdef LILYGO_WATCH_2020_V2
        power->setPowerOutPut(AXP202_LDO2, false);
        power->setPowerOutPut(AXP202_LDO3, false);
        power->setPowerOutPut(AXP202_LDO4, false);
    #else
        power->setPowerOutPut(AXP202_LDO2, false);
        power->setPowerOutPut(AXP202_LDO3, false);
    #endif

    bool should_wake_up = false;

    while(!should_wake_up){
        time_t next_trigger = alarm_generator_trigger;
        time_t next_signal;
        time_t current_time = now();
        bool next_wakeup_timer_is_signal = false;

        if(signals.get_next_trigger(next_signal, current_time) && next_signal < alarm_generator_trigger){
            next_trigger = next_signal;
            next_wakeup_timer_is_signal = true;
        }
        time_t time_to_signal = next_trigger - current_time;

        // Serial.print("Will wake up for notification in ");
        // Serial.print(time_to_signal);
        // Serial.print(" seconds (");
        // Serial.print(time_to_signal/60);
        // Serial.println(" minutes)");
        // Serial.flush();

        esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * (time_to_signal));
        esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
        
        esp_light_sleep_start();
        esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
        
        esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

        should_wake_up = true;
        if(wakeup_reason == ESP_SLEEP_WAKEUP_EXT1){
            irq = false;
            power->readIRQ();
            if(!power->isPEKShortPressIRQ()){
                should_wake_up = false;
            }
            power->clearIRQ();
        }else if(wakeup_reason == ESP_SLEEP_WAKEUP_TIMER){
            if(!next_wakeup_timer_is_signal){
                update_alarm_generation_trigger();
                signals.generate();
                should_wake_up = false;
            }
        }
    }

    #ifdef LILYGO_WATCH_2020_V2
        power->setPowerOutPut(AXP202_LDO2, true);
        power->setPowerOutPut(AXP202_LDO3, true);
        power->setPowerOutPut(AXP202_LDO4, true);
    #else
        power->setPowerOutPut(AXP202_LDO2, true);
        power->setPowerOutPut(AXP202_LDO3, true);
    #endif

    setTime(get_time_t());
    time_t current_time = now();
    time_check(current_time + 60);
    current_time = now();
    sleep_timer = current_time + SLEEPTIME;
    delta_t = current_time + DELTATIME;
    watch->displayWakeup();
    #ifdef LILYGO_WATCH_2020_V2
        watch->touchWakup();
    #endif

    enter_next_state();

    lv_tick_inc(LV_DISP_DEF_REFR_PERIOD);
    watch->startLvglTick();
    lv_task_handler();
    watch->openBL();
    // signals.print_signals_to_serial();
}


time_t ExperienceSampler::get_time_t(){
    RTC_Date rtc_time = watch->rtc->getDateTime();
    TimeElements time;
    
    time.Year = rtc_time.year - 1970;
    time.Month = rtc_time.month;
    time.Day = rtc_time.day;
    time.Hour = rtc_time.hour;
    time.Minute = rtc_time.minute;
    time.Second = rtc_time.second;

    time_t t = makeTime(time);
    return t;
}


char* ExperienceSampler::get_study_name(){
    return study_name;
}


void ExperienceSampler::update_alarm_generation_trigger(){
    time_t current_time = now();
    TimeElements tm;

    breakTime(current_time, tm);
    tm.Day += 1;
    tm.Hour = 0;
    tm.Minute = 1;

    alarm_generator_trigger = makeTime(tm);
}


void ExperienceSampler::activated_signal(char* questionnarie_name, bool accepted){
    signals.remove_next_active_signal(questionnarie_name, accepted);
}


void ExperienceSampler::time_check(time_t current_time){
    StateType current_state_type = current_state != nullptr ? current_state->get_type() : StateType::menu;
    if(signals.check_times(current_time)){
        reset_sleep();
        if(current_state_type == StateType::menu){
            char signal_name[NAMELENGTH];
            signals.get_next_active_signal(signal_name);
            if(strcmp(signal_name, "") != 0){
                request_next_state(StateType::notification, signal_name);
            }
        }
    }
}


void ExperienceSampler::trigger_alarm(){
    #ifdef LILYGO_WATCH_2020_V2
        TTGOClass::getWatch()->drv->go();
    #else
        TTGOClass::getWatch()->motor->onec(VIBRATION_DURATION);
    #endif
}


void ExperienceSampler::reset_times(){
    time_t current_time = now();
    delta_t = current_time + 5;
    sleep_timer = current_time + SLEEPTIME;
    signals.reset();
    update_alarm_generation_trigger();
    signals.generate();
}

#ifdef LILYGO_WATCH_2020_V2
void ExperienceSampler::activate_gps(){
    watch->trunOnGPS();
    watch->gps_begin();
    gps_active = true;
}


void ExperienceSampler::deactivate_gps(){
    watch->turnOffGPS();
    gps_active = false;
}
#endif


State::State(ExperienceSampler* parent, char* state_info){
    this->parent = parent;
}


State::~State(){}