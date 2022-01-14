#include "Log.h"
#include "const_defines.h"
#include "SafeSD.h"


void logMsg(char* message){
    RTC_Date current_time = TTGOClass::getWatch()->rtc->getDateTime();

    File file = open_file_safely(LOGFILE, FILE_APPEND);

    if(!file){
        return;
    }

    file.print(current_time.year);
    file.print("/");
    file.print(current_time.month);
    file.print("/");
    file.print(current_time.day);
    file.print(" ");
    file.print(current_time.hour);
    file.print(":");
    file.print(current_time.second);
    file.print(" -> ");
    file.println(message);

    file.close();
}


void logMsgf(char* message, char* arg){
    char logstring[1024];
    sprintf(logstring, message, arg);
    logMsg(logstring);
}


void logMsgTime(RTC_Date time){
    char logstring[20];
    sprintf(
        logstring,
        "%d/%02d/%02d %02d:%02d",
        time.year,
        time.month,
        time.day,
        time.hour,
        time.minute
    );
    logMsg(logstring);
}