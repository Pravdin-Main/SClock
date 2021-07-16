#ifndef main_h
#define main_h

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <GyverButton.h>
#include "functions.h"
// #include "debugger.h"

// ------------------------- НАСТРОЙКИ --------------------
// #define RESET_CLOCK 0       // сброс часов на время загрузки прошивки (для модуля с несъёмной батарейкой). Не забудь поставить 0 и прошить ещё раз!
#define SENS_TIME 30000     // время обновления показаний сенсоров на экране, миллисекунд
#define ALARMCHECK 10000    // частота проверки времени будильника

#define DEBUG 0             // вывод на дисплей лог инициализации датчиков при запуске. Для дисплея 1602 не работает! Но дублируется через порт!

// адрес BME280 жёстко задан в файле библиотеки Adafruit_BME280.h
// стоковый адрес был 0x77, у китайского модуля адрес 0x76.
// Так что если юзаете НЕ библиотеку из архива - не забудьте поменять

/*int8_t hrs, mins, secs;
byte mode = 0;*/
/*
  0 часы и данные
  1 график температуры за час
  2 график температуры за сутки
  3 график влажности за час
  4 график влажности за сутки
  5 график давления за час
  6 график давления за сутки
  7 график углекислого за час
  8 график углекислого за сутки
*/

/*// переменные для вывода
float dispTemp;
byte dispHum;
int dispPres;
int dispCO2;
int dispRain;

// массивы графиков
int tempHour[15], tempDay[15];
int humHour[15], humDay[15];
int pressHour[15], pressDay[15];
int co2Hour[15], co2Day[15];
int delta;
uint32_t pressure_array[6];
uint32_t sumX, sumY, sumX2, sumXY;
float a, b;
byte time_array[6];*/



/*#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;*/

#include <GyverTimer.h>
GTimer_ms sensorsTimer(SENS_TIME);      //Check sensor timer
GTimer_ms drawSensorsTimer(SENS_TIME);  //Drawing sensors timer
GTimer_ms clockTimer(500);              //Get real time every 0.5 sec
GTimer_ms plotTimer(240000);            //Drawing plot any 4 min
GTimer_ms brightTimer(2000);            //Check brightness 2 sec
GTimer_ms checkAlarm(ALARMCHECK);       //Check alarm's time 

bool alarmIs_ON = false;

#endif
