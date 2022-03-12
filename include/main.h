#ifndef main_h
#define main_h

//-------------------------- Libraries --------------------

#include <Arduino.h>
#include "functions.h"
#include <GyverTimer.h>
//---------------------------------------------------------

// ------------------------- НАСТРОЙКИ --------------------
#if (ALARM >= 1)
  #define ALARMCHECK 10000    // частота проверки времени будильника
  GTimer_ms checkAlarm(ALARMCHECK);             //Check alarm's time 10 sec
  #if (ALARM == 1)
    bool alarmIs_ON = false;
  #else
    uint8_t alarmIs_ON = false;
  #endif
#endif

#if (SENSORS == 1)
  #define SENS_TIME 10000     // время обновления показаний сенсоров на экране, миллисекунд
  GTimer_ms sensorsTimer(SENS_TIME);            //Check sensor timer 10 sec
  GTimer_ms drawSensorsTimer(SENS_TIME);        //Drawing sensors timer 10 sec
  
  #if(GRAPH == 1)
  GTimer_ms plotTimer(240000);                  //Drawing plot any 4 min
  #endif
#endif

static uint8_t PowerControlCheck;

GTimer_ms clockTimer(500);                    //Get real time every 0.5 sec

#endif
