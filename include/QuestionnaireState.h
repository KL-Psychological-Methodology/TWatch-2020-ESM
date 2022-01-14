#ifndef ES_QUESTIONNAIRE_STATE_H
#define ES_QUESTIONNAIRE_STATE_H

#include "ExperienceSampler.h"
#include "Items.h"
#include "ItemDefinitions.h"

class QuestionnaireState;


class ItemQueue{
public:
    ItemQueue();

    void addItem(ItemDefinition &item_def);
    void make_empty();
    bool is_empty();
    EsmBaseItem* current_item();
    void first();
    void next();
    bool is_last();
    uint8_t get_length(){return length;};
    void randomize(uint8_t from, uint8_t to);

private:
    EsmBaseItem* queue[MAXITEMNUM];
    uint8_t length;
    uint8_t index;
};


struct RandomGroupDefinition{
    RandomGroupDefinition():length(0){};
    uint8_t groups[MAXRANDOMGROUPS][2];
    uint8_t length;
};


class QuestionnaireState: public State {
public:
    QuestionnaireState(ExperienceSampler* parent, char* state_info = (char*) "");
    ~QuestionnaireState();

    void next_item();
    char* get_questionnaire_name();

private:
    ItemQueue item_queue;
    char questionnaire_name[NAMELENGTH];
    bool use_gps;

    void make_data_log();
};


#endif // ES_QUESTIONNAIRE_STATE_H