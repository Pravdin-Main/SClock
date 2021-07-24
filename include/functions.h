#ifndef functions_h
#define functions_h

#include <Arduino.h>
#include "sound.h"

#define DEBUG 0             // вывод на дисплей лог инициализации датчиков при запуске. Для дисплея 1602 не работает! Но дублируется через порт!
void debug_start();
#define RESET_CLOCK 0       // сброс часов на время загрузки прошивки (для модуля с несъёмной батарейкой). Не забудь поставить 0 и прошить ещё раз!

// Общие настройки
#define DISP_MODE 1         // в правом верхнем углу отображать: 0 - год, 1 - день недели, 2 - секунды
#define DISPLAY_TYPE 1      // тип дисплея: 1 - 2004 (большой), 0 - 1602 (маленький)
#define DISPLAY_ADDR 0x27   // адрес платы дисплея: 0x27 или 0x3f. Если дисплей не работает - смени адрес! На самом дисплее адрес не указан

#define WEEK_LANG 0         // язык дня недели: 0 - английский, 1 - русский (транслит)
#define PRESSURE 0          // 0 - график давления, 1 - график прогноза дождя (вместо давления). Не забудь поправить пределы гроафика
#define CO2_SENSOR 1        // включить или выключить поддержку/вывод с датчика СО2 (1 вкл, 0 выкл)
#define LED_MODE 0          // тип RGB светодиода: 0 - главный катод, 1 - главный анод
#define BLUE_YELLOW 0       // жёлтый цвет вместо синего (1 да, 0 нет) но из за особенностей подключения жёлтый не такой яркий

// Пины
// #define PHOTO_PIN 0         // пин фоторезистора
#define PHOTO A3            // Фоторезистор
#define CLK 12              // пин CLK энкодера
#define DT 11               // пин DT  энкодера
#define SW 9                // пин SW энкодера
#define MHZ_RX 2            // Датчик СО2 RX
#define MHZ_TX 3            // Датчик СО2 TX
#define BTN_PIN 4           // Сенсорная кнопка
#define LED_COM 7           // Катод (анод) RGB светодиода
#define LED_R 8             // Красноя нога светодиода
#define LED_G 6             // Зеленая нога светодиода
#define LED_B 5             // Синяя нога светодиода
#define BACKLIGHT 10        // Подсветка дисплея


// Управление яркостью
#define BRIGHT_CONTROL 1      // 0/1 - запретить/разрешить управление яркостью (при отключении яркость всегда будет макс.)
#define BRIGHT_THRESHOLD 150  // величина сигнала, ниже которой яркость переключится на минимум (0-1023)
#define LED_BRIGHT_MAX 255    // макс яркость светодиода СО2 (0 - 255)
#define LED_BRIGHT_MIN 10     // мин яркость светодиода СО2 (0 - 255)
#define LCD_BRIGHT_MAX 255    // макс яркость подсветки дисплея (0 - 255)
#define LCD_BRIGHT_MIN 10     // мин яркость подсветки дисплея (0 - 255)

// #if (LED_MODE == 0)
//   uint8_t LED_ON = (LED_BRIGHT_MAX);
//   uint8_t LED_OFF = (LED_BRIGHT_MIN);
// #else
//   uint8_t LED_ON = (255 - LED_BRIGHT_MAX);
//   uint8_t LED_OFF = (255 - LED_BRIGHT_MIN);
// #endif

// пределы отображения для графиков
#define TEMP_MIN 15         // Минимальны уровень отображения графика температуры
#define TEMP_MAX 35         // Максимальный уровень отображения графика температуры
#define HUM_MIN 0           // Минимальный уровень отображения графика влажности
#define HUM_MAX 100         // Максимальный уровень отображения графика влажности
#define PRESS_MIN -100      // Минимальный уровень отображения графика давления
#define PRESS_MAX 100       // Максимальный уровень отображения графика давления
#define CO2_MIN 300         // Минимальный уровень отображения графика СО2
#define CO2_MAX 2000        // Максимальный уровень отображения графика СО2

#include <LiquidCrystal_I2C.h>
// #if (DISPLAY_TYPE == 1)
// LiquidCrystal_I2C lcd(DISPLAY_ADDR, 20, 4);
// #else
// LiquidCrystal_I2C lcd(DISPLAY_ADDR, 16, 2); 
// #endif
void display_init();

#include <Adafruit_Sensor.h>
//#include <Adafruit_BME280.h>
#include <RTClib.h>

#include <GyverTimer.h>
// GTimer_ms hourPlotTimer((long)4 * 60 * 1000);         // 4 минуты
// GTimer_ms dayPlotTimer((long)1.6 * 60 * 60 * 1000);   // 1.6 часа
// GTimer_ms predictTimer((long)10 * 60 * 1000);         // 10 минут
// GTimer_ms drawDown_param(1000);
// GTimer_ms drawUp_param(200);
// GTimer_ms backToMain(10000);

#include <GyverEncoder.h>
// Encoder enc(CLK, DT, SW, TYPE2);

#include <cactus_io_BME280_I2C.h>
// BME280_I2C bme(0x76);

#include <GyverButton.h>
// GButton button(BTN_PIN, LOW_PULL, NORM_OPEN);

// // символы
// // график
// uint8_t row8[8] = {0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
// uint8_t row7[8] = {0b00000,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
// uint8_t row6[8] = {0b00000,  0b00000,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
// uint8_t row5[8] = {0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
// uint8_t row4[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111,  0b11111};
// uint8_t row3[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};
// uint8_t row2[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111};
// uint8_t row1[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111};

// // цифры
// uint8_t LT[8] = {0b00111,  0b01111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
// uint8_t UB[8] = {0b11111,  0b11111,  0b11111,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000};
// uint8_t RT[8] = {0b11100,  0b11110,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
// uint8_t LL[8] = {0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b01111,  0b00111};
// uint8_t LB[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};
// uint8_t LR[8] = {0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11110,  0b11100};
// uint8_t UMB[8] = {0b11111,  0b11111,  0b11111,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111};
// uint8_t LMB[8] = {0b11111,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};


// uint8_t mode = 0;
// bool dotFlag;

#if (CO2_SENSOR == 1)
#include <MHZ19_uart.h>
// MHZ19_uart mhz19;
#endif

#include <RTClib.h>
// RTC_DS3231 rtc;
// DateTime now;

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

void drawDig(uint8_t dig, uint8_t x, uint8_t y);
void drawdots(uint8_t x, uint8_t y, bool state);
void drawClock(uint8_t hours, uint8_t minutes, uint8_t x, uint8_t y, bool dotState);
void drawData();
void drawPlot(uint8_t pos, uint8_t row, uint8_t width, uint8_t height, int min_val, int max_val, int *plot_array, uint8_t label);
void loadClock();
void loadPlot();
void setLED(uint8_t color);
void checkBrightness();
void modesTick();
void redrawPlot();
void readSensors();
void drawSensors();
void plotSensorsTick();
void clockTick();
void inition ();

#include "alarm.h"
// alarm alarm1;
// alarm alarm2;
// alarm alarm3;

void alarmTuning();
uint8_t alarmControl();
void alarmStart(uint8_t set);
void drawAlarmClock(uint8_t hours, uint8_t minutes, uint8_t x, uint8_t y, bool draw);
void alarmStop();

struct alarmTuner {
    int8_t hour;
    int8_t minute;
    int8_t sound;
    bool status;
};

// static int8_t hrs;
// static int8_t mins;
// static int8_t secs;

// bool firstStartFlag = false;
// uint8_t set_alarm;
// static uint8_t set_param;
// bool draw_param;

// переменные для вывода
// float dispTemp;
// uint8_t dispHum;
// int dispPres;
// int dispCO2;
// int dispRain;

// массивы графиков
// int tempHour[15], tempDay[15];
// int humHour[15], humDay[15];
// int pressHour[15], pressDay[15];
// int co2Hour[15], co2Day[15];
// int delta;
// uint32_t pressure_array[6];
// uint32_t sumX, sumY, sumX2, sumXY;
// float a, b;
// uint8_t time_array[6];

// #if (WEEK_LANG == 0)
// static const char *dayNames[]  = {
//   "Sund",
//   "Mond",
//   "Tues",
//   "Wedn",
//   "Thur",
//   "Frid",
//   "Satu",
// };
// #else
// static const char *dayNames[]  = {
//   "BOCK",
//   "POND",
//   "BTOP",
//   "CPED",
//   "4ETB",
//   "5YAT",
//   "CYBB",
// };
// #endif

bool Enc_IsClick();
bool Enc_IsDouble();
bool Enc_IsHolded();
void Enc_Tick();
void Enc_Reset();
uint8_t Mode(uint8_t x);

void drawFlags();

#define EEPROM_KEY_ADDR 0
#define EEPROM_KEY 150

#define ALARM1_HOUR_ADDR 1
#define ALARM1_MIN_ADDR 2
#define ALARM1_SOUND_ADDR 3
#define ALARM1_STATUS_ADDR 4

#define ALARM2_HOUR_ADDR 5
#define ALARM2_MIN_ADDR 6
#define ALARM2_SOUND_ADDR 7
#define ALARM2_STATUS_ADDR 8

#define ALARM3_HOUR_ADDR 9
#define ALARM3_MIN_ADDR 10
#define ALARM3_SOUND_ADDR 11
#define ALARM3_STATUS_ADDR 12

void EEPROM_init();
// void EEPROM_put(uint8_t x);
// void EEPROM_get(uint8_t x);

#endif