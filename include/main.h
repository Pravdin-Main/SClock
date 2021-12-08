#ifndef main_h
#define main_h

//-------------------------- Libraries --------------------

#include <Arduino.h>
// #include <Wire.h>
#include "functions.h"
#include <GyverTimer.h>
//---------------------------------------------------------

// ------------------------- НАСТРОЙКИ --------------------
// #define RESET_CLOCK 0       // сброс часов на время загрузки прошивки (для модуля с несъёмной батарейкой). Не забудь поставить 0 и прошить ещё раз!
#if (ALARM == 1)
  #define ALARMCHECK 10000    // частота проверки времени будильника
  GTimer_ms checkAlarm(ALARMCHECK);             //Check alarm's time 10 sec
  bool alarmIs_ON = false;
#endif

#if (SENSORS == 1)
  #define SENS_TIME 10000     // время обновления показаний сенсоров на экране, миллисекунд
  GTimer_ms sensorsTimer(SENS_TIME);            //Check sensor timer 30 sec
  GTimer_ms drawSensorsTimer(SENS_TIME);        //Drawing sensors timer 30 sec
  
  #if(GRAPH == 1)
  GTimer_ms plotTimer(240000);                  //Drawing plot any 4 min
  #endif
#endif

GTimer_ms clockTimer(500);                    //Get real time every 0.5 sec
GTimer_ms powerControlTimer(1*60*60*1000);    //Check power voltage every 1 hour

#endif
