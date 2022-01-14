#ifndef ES_STRING_LIST_H
#define ES_STRING_LIST_H

#include "Arduino.h"
#include "const_defines.h"

struct StringList{
public:
    StringList(){length = 0;};
    char list[10][NAMELENGTH];
    uint8_t length;
};

#endif // ES_STRING_LIST_H