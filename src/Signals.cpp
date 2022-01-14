#include "Signals.h"

#include "Arduino.h"
#include "ArduinoJson.h"
#include "config.h"
#include "SafeSD.h"


const char* Signals::signal_log_header = "id;signal_name;event;timestamp;date;time";


Signals::Signals() {
    for(uint8_t i = 0; i < MAXALARMS; i++) {
        times[i] = nullptr;
    }
}


void Signals::reorder() {

    // Serial.println("Reordering");

    uint8_t last_index;
    for(last_index = MAXALARMS - 1; last_index > 0 && times[last_index] == nullptr; last_index--);

    if(last_index == 0) {
        return;
    }
    
    bool contiguous = false;

    while(!contiguous){
        contiguous = true;

        for(uint8_t i = 0; i < last_index && i < MAXALARMS - 1; i++) {
            if(times[i] == nullptr && times[i+1] != nullptr) {
                times[i] = times[i+1];
                times[i+1] = nullptr;
                contiguous = false;
            }
        }
    }
}


void Signals::add_signal_time(SignalTime* signal) {
    uint8_t i;
    for(i = 0; i < MAXALARMS && times[i] != nullptr; i++);

    if(i < MAXALARMS) {
        times[i] = signal;
    }else{
        delete signal;
    }
}


void Signals::reset() {
    for(uint8_t i = 0; i < MAXALARMS; i++) {
        if(times[i] != nullptr) {
            delete times[i];
            times[i] = nullptr;
        }
    }
}


void Signals::generate(time_t* current_time) {

    uint8_t mac[6];
    esp_efuse_mac_get_default(mac);
    TimeElements tm;
    breakTime(now(), tm);

    long seed = 0;
    seed += mac[0] << 8*2;
    seed += mac[1] << 8*2;
    seed += mac[2] << 8*2;
    seed += mac[3] << 8;
    seed += mac[4] << 8;
    seed += mac[5] << 8;

    seed += tm.Year;
    seed += tm.Month;
    seed += tm.Day;

    randomSeed(seed);

    File file = open_file_safely(CONFIGFILE, (char*) FILE_READ);

    file.find("\"data\":");
    file.find("[");

    StaticJsonDocument<128> filter;
    filter["name"] = true;
    filter["active"] = true;

    StaticJsonDocument<264> doc;

    do{
        size_t previous_position = file.position();
        

        deserializeJson(doc, file, DeserializationOption::Filter(filter));
        if(!doc["active"].as<bool>()){
            continue;
        }

        const char* questionnaire_name = doc["name"].as<const char*>();

        file.seek(previous_position);
        file.find("\"alarms\":");
        file.find("[");

        StaticJsonDocument<264> signal_doc;
        do {
            deserializeJson(signal_doc, file);

            TimeElements start_time;
            breakTime(now(), start_time);
            start_time.Hour = signal_doc["H"].as<int>();
            start_time.Minute = signal_doc["M"].as<int>();

            time_t trigger_time = makeTime(start_time);
            uint16_t duration = signal_doc["dur"].as<int>();
            if(duration > 0) {
                trigger_time += 60 * (random(duration + 1));
            }

            if(current_time != nullptr && *current_time > trigger_time) {
                continue;
            }

            uint16_t expiry = signal_doc["exp"].as<int>();
            if(!expiry) expiry = 15;

            uint16_t expiry_seconds = expiry * 60;
            time_t expiry_time = trigger_time + expiry_seconds;

            uint8_t repeat = signal_doc["repeat"].as<int>();

            SignalTime* signal = new SignalTime();

            signal->trigger_time = trigger_time;
            signal->expiry_time = expiry_time;
            signal->expiry_seconds = expiry_seconds;
            signal->repeat = repeat;
            strcpy(signal->questionnaire_name, questionnaire_name);

            add_signal_time(signal);
        } while(file.findUntil(",","]"));

        file.seek(previous_position);
        deserializeJson(doc, file, DeserializationOption::Filter(filter));


    } while(file.findUntil(",","]"));

    file.close();
}



void Signals::log_signal(SignalTime* signal, SignalLogType log_type) {
    File file;
    bool file_exists = true;
    char filename[NAMELENGTH+15];
    sprintf(filename, SIGNALLOGFILE, study_name);

    #if defined LILYGO_WATCH_2020_V1 || defined V2_USE_SPIFFS
        file_exists = SPIFFS.exists(filename);
        if(file_exists){
            file = open_file_safely(filename, (char*) FILE_APPEND);
        }else{
            file = SPIFFS.open(filename, FILE_APPEND);
        }
    #else
        file_exists = SD.exists(filename);
        if(file_exists){
            file = open_file_safely(filename, (char*) FILE_APPEND);
        }else{
            file = SD.open(filename, FILE_APPEND);
        }
    #endif

    if(!file){
        return;
    }

    if(!file_exists){
        file.println(signal_log_header);
    }

    TimeElements tm;
    time_t current_time = now();

    uint8_t mac[6];
    esp_efuse_mac_get_default(mac);
    char mac_string[18];
    sprintf(mac_string, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    file.printf(
        "%s;%s;",
        mac_string,
        signal->questionnaire_name
    );

    switch (log_type)
    {
    case SignalLogType::SIGNAL:
        file.print("signal;");
        break;
    case SignalLogType::REPEAT:
        file.print("repeat;");
        break;
    case SignalLogType::REACTION:
        file.print("reaction;");
        break;
    case SignalLogType::DECLINED:
        file.print("declined;");
        break;
    }
    file.print(current_time);
    breakTime(current_time, tm);

    file.printf(
        ";%d-%02d-%0d;%02d:%02d:%02d\n",
        tm.Year + 1970,
        tm.Month,
        tm.Day,
        tm.Hour,
        tm.Minute,
        tm.Second
    );

    file.close();
}


bool Signals::check_times(time_t current_time) {
    check_expiry(current_time);

    bool has_active = false;
    for(uint8_t i = 0; times[i] != nullptr && i < MAXALARMS; i++) {
        if(current_time >= times[i]->trigger_time) {
            if(!times[i]->is_active){
                times[i]->is_active = true;
                log_signal(times[i], times[i]->is_repeat ? SignalLogType::REPEAT : SignalLogType::SIGNAL);
            }
            has_active = true;
        }
    }
    return has_active;
}


void Signals::check_expiry(time_t current_time) {
    bool did_expire = false;
    for(uint8_t i = 0; times[i] != nullptr && i < MAXALARMS; i++) {
        if(current_time >= times[i]->expiry_time) {
            if(times[i]->repeat > 0) {
                times[i]->trigger_time += times[i]->expiry_seconds;
                times[i]->expiry_time = times[i]->trigger_time + times[i]->expiry_seconds;
                times[i]->repeat -= 1;
                times[i]->is_repeat = true;
                times[i]->is_active = false;
            }else{
                delete times[i];
                did_expire = true;
                times[i] = nullptr;
            }
        }
    }
    if(did_expire) reorder();
}


void Signals::get_next_active_signal(char* name) {
    strcpy(name, "");

    for(uint8_t i = 0; times[i] != nullptr && i < MAXALARMS; i++){
        if(!times[i]->is_active){
            continue;
        }
        strcpy(name, times[i]->questionnaire_name);
        return;
    }
}


void Signals::remove_next_active_signal(char* name, bool accepted) {
    bool did_delete = false;

    // Serial.printf("Removing next signal for %s\n", name);

    for(uint8_t i = 0; times[i] != nullptr && i < MAXALARMS; i++){
        if(!times[i]->is_active){
            // Serial.printf("Signal %d is not active\n", i);
            continue;
        }
        if(strcmp(times[i]->questionnaire_name, name) == 0){
            log_signal(times[i], accepted ? SignalLogType::REACTION : SignalLogType::DECLINED);
            delete times[i];
            times[i] = nullptr;
            did_delete = true;
            break;
        }
    }
    if(did_delete){
        reorder();
    }
}


time_t Signals::get_next_event_time(SignalTime* signal) {
    if(signal->is_active && signal->repeat > 0){
        return signal->expiry_time;
    }
    return signal->trigger_time;
}


bool Signals::get_next_trigger(time_t &trigger, time_t current_time) {
    if(times[0] == nullptr){
        return false;
    }
    bool found_trigger = false;
    trigger = current_time + 24*60*60;
    for(uint8_t i = 0; times[i] != nullptr && i < MAXALARMS; i++){
        time_t signal_next_trigger = get_next_event_time(times[i]);
        if(signal_next_trigger > current_time && signal_next_trigger < trigger){
            trigger = signal_next_trigger;
            found_trigger = true;
        }
    }
    return found_trigger;
}


void Signals::print_signals_to_serial() {
    Serial.println("---- Signals ---- \n\n");
    for(uint8_t i = 0; i < MAXALARMS; i++){
        if(times[i] == nullptr) {
            Serial.printf("%d: X\n", i);
        }else{
            Serial.printf(
                "%d: %s, %s\n",
                i,
                times[i]->questionnaire_name,
                times[i]->is_active ? "active" : "inactive"
            );
        }
    }
}