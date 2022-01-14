#ifndef ES_CONST_DEFINES_H
#define ES_CONST_DEFINES_H

#include "config.h"

// Timing definitions

#define FRAMETIME 30 // 25 fps
#define DELTATIME 60 // 1 minute
#define SLEEPTIME 30 // half a minute

// Hardware definitions

#define SCREEN_H 240
#define SCREEN_W 240
#if defined LILYGO_WATCH_2020_V1 || defined V2_USE_SPIFFS
#define CPU_ACTIVE_SPEED 240
#else
#define CPU_ACTIVE_SPEED 80
#endif
#define CPU_SLEEP_SPEED 10


// Maxima definitions

#define NAMELENGTH 21
#define LOGSTRINGLENGTH 1024
#define LABELLENGTH 25
#define PROMPTLENGTH 251
#define MAXITEMNUM 20
#define MENULISTLENGTH 10
#define MAXALARMS 30
#define MAXLIKERT 15
#define OPTIONSLENGTH 128
#define MAXRANDOMGROUPS 5


// File path definitions

#define CONFIGFILE "/config.json"
#define LOGFILE "/log.txt"
#define DATAFILE "/data.csv"
#define SIGNALLOGFILE "/%s_signallog.csv"
#define LOCALEFILE "/locale.json"

// Conversions

#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds


// Alarm definitions

#define DRV_EFFECT 15
#define VIBRATION_DURATION 750


#endif // ES_CONST_DEFINES_H