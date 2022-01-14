#ifndef ES_LOG_H
#define ES_LOG_H

#include "config.h"

void logMsg(char* message);
void logMsgf(char* message, char* arg);
void logMsgTime(RTC_Date time);

#endif // ES_LOG_H