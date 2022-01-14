#include "QuestionnaireState.h"

#include "config.h"
#include "SafeSD.h"
#include <ArduinoJson.h>
#include "ItemResult.h"


ItemQueue::ItemQueue(){
    for(uint8_t i = 0; i < MAXITEMNUM; i++){
        queue[i] = nullptr;
    }
    length = 0;
    index = 0;
}


void ItemQueue::addItem(ItemDefinition &item_def){
    if(length == MAXITEMNUM){
        return;
    }
    switch(item_def.type){
        case ItemType::VAS:
            queue[length++] = new EsmItemVAS(item_def);
            break;
        case ItemType::TEXT:
            queue[length++] = new EsmItemText(item_def);
            break;
        case ItemType::NUMERIC:
            queue[length++] = new EsmItemNumeric(item_def);
            break;
        case ItemType::LIKERT:
            queue[length++] = new EsmItemLikert(item_def);
            break;
        case ItemType::OPTIONS:
            queue[length++] = new EsmItemOptions(item_def);
            break;
        default:
            break;
    }
}


void ItemQueue::make_empty(){
    for(uint8_t i = 0; i < MAXITEMNUM && queue[i] != nullptr; i++){
        delete queue[i];
        queue[i] = nullptr;
    }
    length = 0;
    index = 0;
}


bool ItemQueue::is_empty(){
    return length == 0;
}


EsmBaseItem* ItemQueue::current_item(){
    return queue[index];
}


void ItemQueue::next(){
    if(index < length){
        index++;
    }
}


void ItemQueue::first(){
    index = 0;
}


bool ItemQueue::is_last(){
    return index == length - 1;
}


void ItemQueue::randomize(uint8_t from, uint8_t to){
    if(from >= length || to >= length || from >= to){
        return;
    }
    EsmBaseItem* pool[MAXITEMNUM];
    uint8_t pool_size = 0;
    for(uint8_t i = from; i <= to; i++){
        pool[pool_size++] = queue[i];
    }
    for(uint8_t i = from; i <= to; i++){
        uint8_t rand_index = random(pool_size);
        queue[i] = pool[rand_index];
        pool_size--;
        if(rand_index != pool_size){
            pool[rand_index] = pool[pool_size];
        }
    }
}



QuestionnaireState::QuestionnaireState(
    ExperienceSampler* parent,
    char* state_info
) : State(parent, state_info) {
    type = StateType::questionnaire;
    strcpy(questionnaire_name, state_info);

    File file = open_file_safely(CONFIGFILE, (const char*) FILE_READ);

    StaticJsonDocument<128> filter;
    filter["name"] = true;
    filter["randomize"] = true;
    filter["randomize"][0] = true;
    filter["randomize"][0][0] = true;
    #ifdef LILYGO_WATCH_2020_V2
        use_gps = false;
        filter["gps"] = true;
    #endif

    const size_t groups_capacity = JSON_ARRAY_SIZE(MAXRANDOMGROUPS) + JSON_ARRAY_SIZE(MAXRANDOMGROUPS * 2) + JSON_OBJECT_SIZE(MAXRANDOMGROUPS * 3 + 2) + sizeof(double) * MAXRANDOMGROUPS * 2 + NAMELENGTH + 100;
    StaticJsonDocument<groups_capacity> groups_doc;
    bool found_questionnaire = false;
    file.find("\"data\":");
    file.find("[");
    RandomGroupDefinition group_def;
    do{
        deserializeJson(groups_doc, file, DeserializationOption::Filter(filter));
        if(strcmp(questionnaire_name, groups_doc["name"].as<const char*>()) == 0){
            found_questionnaire = true;
            JsonArray groups_array = groups_doc["randomize"].as<JsonArray>();
            if(groups_array.isNull()){
                break;
            }
            for(JsonArray::iterator it = groups_array.begin(); it != groups_array.end(); ++it){
                if(!(*it).is<JsonArray>()){
                    continue;
                }
                if((*it).size() != 2){
                    continue;
                }
                if(!(*it)[0].is<int>() || !(*it)[1].is<int>()){
                    continue;
                }
                group_def.groups[group_def.length][0] = (*it)[0].as<int>() - 1;
                group_def.groups[group_def.length][1] = (*it)[1].as<int>() - 1;
                group_def.length += 1;
            }
        }
    }while(!found_questionnaire && file.findUntil(",","]"));

    file.seek(0);

    file.find("\"data\":");

    bool found = false;
    while(!found && file.find("\"name\":")){
        if(file.findUntil(state_info, ",")){
            found = true;
            file.find("\"items\":");
            file.find("[");
        }
    }

    if(!found){
        parent->request_next_state(StateType::menu);
        file.close();
        return;
    }

    StaticJsonDocument<1024> doc;

    do{
        deserializeJson(doc, file);

        bool valid_type = true;

        ItemDefinition item_def;
        item_def.item_name = doc["item"].as<const char*>();
        item_def.prompt = doc["prompt"].as<const char*>();
        item_def.parent_questionnaire = this;
        item_def.mandatory = doc["mandatory"].as<bool>();

        const char* type = doc["type"].as<const char*>();
        
        if(strcmp(type, "vas") == 0){
            item_def.type = ItemType::VAS;
            item_def.info_vas.left_label = doc["l_lab"].as<const char*>();
            item_def.info_vas.right_label = doc["r_lab"].as<const char*>();
        }else if(strcmp(type, "text") == 0){
            item_def.type = ItemType::TEXT;
        }else if(strcmp(type, "numeric") == 0){
            item_def.type = ItemType::NUMERIC;
            item_def.info_numeric.num_digits = doc["digits"].as<int>();
            item_def.info_numeric.separator_position = doc["dot_pos"].as<int>();
            item_def.info_numeric.min_value = doc["min_val"].as<int>();
            item_def.info_numeric.max_value = doc["max_val"].as<int>();
        }else if(strcmp(type, "likert") == 0){
            item_def.type = ItemType::LIKERT;
            item_def.info_likert.left_label = doc["l_lab"].as<const char*>();
            item_def.info_likert.right_label = doc["r_lab"].as<const char*>();
            item_def.info_likert.resolution = doc["res"].as<int>();
        }else if(strcmp(type, "option") == 0){
            item_def.type = ItemType::OPTIONS;
            item_def.info_options.options = doc["options"].as<const char*>();
        }else{
            valid_type = false;
        }

        if(valid_type){
            item_queue.addItem(item_def);
        }

    }while(file.findUntil(",","]"));

    #ifdef LILYGO_WATCH_2020_V2
        file.seek(0);
        file.find("\"data\":");
        file.find("[");

        StaticJsonDocument<128> gps_doc;
        do{
            deserializeJson(gps_doc, file, DeserializationOption::Filter(filter));
            if(strcmp(gps_doc["name"].as<const char*>(), state_info) == 0){
                use_gps = gps_doc["gps"].as<bool>();
            }
        }while(file.findUntil(",","]"));

        if(use_gps){
            parent->activate_gps();
        }
    #endif
    
    file.close();

    if(group_def.length > 0){
        for(uint8_t i = 0; i < group_def.length; i++){
            item_queue.randomize(group_def.groups[i][0], group_def.groups[i][1]);
        }
    }

    item_queue.current_item()->activate();

}


QuestionnaireState::~QuestionnaireState(){
    item_queue.make_empty();
    #ifdef LILYGO_WATCH_2020_V2
        if(use_gps){
            parent->deactivate_gps();
        }
    #endif
}


char* QuestionnaireState::get_questionnaire_name(){
    return questionnaire_name;
}


void QuestionnaireState::next_item(){
    if(item_queue.is_last()){
        // item_queue.current_item()->deactivate();
        make_data_log();
        if(parent->do_quick_return()){
            parent->request_next_state(StateType::menu);
        }else{
            parent->request_next_state(StateType::questionnaire_end);
        }
        return;
    }else{
        item_queue.current_item()->deactivate();
        item_queue.next();
        item_queue.current_item()->activate();
    }
}


void QuestionnaireState::make_data_log(){
    if(item_queue.is_empty()){
        return;
    }

    char filename[NAMELENGTH+8];
    
    char* name = parent->get_study_name();

    if(strcmp(name, "") == 0){
        strcpy(filename, DATAFILE);
    }else{
        sprintf(filename, "/%s.csv", name);
    }

    bool addheader = false;
    File file;
    
    #if defined LILYGO_WATCH_2020_V1 || defined V2_USE_SPIFFS
        if(!SPIFFS.exists(filename)){
            file = SPIFFS.open(filename, FILE_APPEND);
            addheader = true;
        }else{
            file = open_file_safely(filename, (const char*) FILE_APPEND);
        }
    #else
        if(!SD.exists(filename)){
            file = SD.open(filename, FILE_APPEND);
            addheader = true;
        }else{
            file = open_file_safely(filename, (const char*) FILE_APPEND);
        }
    #endif


    if(!file){
        return;
    }

    if(addheader){
        file.println(EsmBaseItem::header);
    }

    // id;quest_name;item_name;response_date;response_time;latitude;longitude;type;prompt;left_label;right_label;resolution;response

    RTC_Date current_time = TTGOClass::getWatch()->rtc->getDateTime();
    uint8_t mac[6];
    esp_efuse_mac_get_default(mac);
    char mac_string[18];
    sprintf(mac_string, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    char lat[32];
    char lon[32];
    strcpy(lat, "");
    strcpy(lon, "");
    #ifdef LILYGO_WATCH_2020_V2
        if(use_gps){
            TinyGPSPlus* gps = TTGOClass::getWatch()->gps;
            if(gps->location.isValid()){
                sprintf(lat, "%f", gps->location.lat());
                sprintf(lon, "%f", gps->location.lng());
            }
        }
    #endif


    bool finished = false;
    for(item_queue.first(); !finished; item_queue.next()){
        ItemResult result;
        item_queue.current_item()->get_result(result);

        if(!result.print_result) continue;

        file.print(mac_string);
        file.print(";");
        file.print(questionnaire_name);
        file.print(";");
        file.print(result.item_name);
        file.print(";");
        file.printf("%d-%02d-%02d;%02d:%02d;", current_time.year, current_time.month, current_time.day, current_time.hour, current_time.minute);
        file.print(result.duration);
        file.print(";");
        // file.print(lat);
        // file.print(";");
        // file.print(lon);
        // file.print(";");

        switch (result.type)
        {
        case ItemType::VAS:
            file.print("vas;");
            break;
        case ItemType::LIKERT:
            file.print("likert;");
            break;
        case ItemType::NUMERIC:
            file.print("numeric;");
            break;
        case ItemType::OPTIONS:
            file.print("options");
            break;
        case ItemType::TEXT:
            break;
        }

        file.print(result.prompt);
        file.print(";");

        if(result.left_label != nullptr){
            file.print(result.left_label);
        }
        file.print(";");
        if(result.right_label != nullptr){
            file.print(result.right_label);
        }
        file.print(";");
        if(result.resolution != nullptr){
            file.print(*result.resolution);
        }
        file.print(";");
        if(result.response != nullptr){
            file.print(*result.response);
        }
        file.println(";");
        
        finished = item_queue.is_last();
    }

    file.close();
}