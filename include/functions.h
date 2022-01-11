#ifndef functions_h
#define functions_h

#include <Arduino.h>

//-------------------------- Modules ----------------------

#define CLOCK 1       //1 - clock type 1; 2 - clock type 2; 3 - clock type 3; 4 - clock type 4
#define ALARM 1       //0 - alarm module OFF; 1 - alarm module ON
#define OPTION 1      //0 - options OFF; 1 - options ON
#define SENSORS 1     //0 - sensors OFF; 1 - sensors ON
#define SENS_CO2 1    //0 - CO2 sensor OFF; 1 - CO2 sensor ON
#define SENS_TEMP 1   //0 - temperature sensor OFF; 1 - remperature sensor ON
#define SENS_HUM 1    //0 - hummadity sensor OFF; 1 - hummadity sensor ON
#define SENS_PRESS 1  //0 - pressure sensor OFF; 1 - pressure sensor ON
#define GRAPH 0       //0 - graphics displaying OFF; 1 - graphics displaying ON

//---------------------------------------------------------

#define EEPROM_KEY_ADDR 0
#define EEPROM_KEY 190
#define DEBUG 0             // вывод на дисплей лог инициализации датчиков при запуске. Для дисплея 1602 не работает! Но дублируется через порт!
#define RESET_CLOCK 0       // сброс часов на время загрузки прошивки (для модуля с несъёмной батарейкой). Не забудь поставить 0 и прошить ещё раз!

// Общие настройки
#define DISP_MODE 1         // в правом верхнем углу отображать: 0 - год, 1 - день недели, 2 - секунды
#define DISPLAY_TYPE 1      // тип дисплея: 1 - 2004 (большой), 0 - 1602 (маленький)
#define DISPLAY_ADDR 0x27   // адрес платы дисплея: 0x27 или 0x3f. Если дисплей не работает - смени адрес! На самом дисплее адрес не указан
#define CURSOR_L 62           // Cursor's symbol for left side
#define CURSOR_R 60           // Cursor's symbol for right side
#define DELAY 1500          // Delay of displaying "done" message

#define WEEK_LANG 0         // язык дня недели: 0 - английский, 1 - русский (транслит)
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
#define BRIGHT_THRESHOLD 150  // величина сигнала, ниже которой яркость переключится на ночной режим (0-1023)
#define LED_BRIGHT_MAX 255    // макс яркость светодиода СО2 (0 - 255)
#define LED_BRIGHT_MIN 10     // мин яркость светодиода СО2 (0 - 255)
#define LCD_BRIGHT_MAX 255    // макс яркость подсветки дисплея (0 - 255)
#define LCD_BRIGHT_MIN 10     // мин яркость подсветки дисплея (0 - 255)
#define LCD_BRIGHT_STEP 10    // шаг изменения подсветки
#define OPTION_DT 255         // Значения по умолчанию
#define DIS_BRS_DAY_DT 80     // Яркость дисплея днем по умолчанию
#define DIS_BRS_NIGHT_DT 20   // Яркость дисплея ночью по умолчанию
#define RGB_BRS_DAY_DT 80     // Яркость светодиода днем по умолчанию
#define RGB_BRS_NIGHT_DT 20   // Яркость светодиода ночью по умолчанию
#define VOLUME_DT 50          // Уровень громкости динамика
#define ZOOM_LED 3000         // Время максимальной яркости стветодиода при нажатии на кнопку
#define CO2_GREEN_THRESHOLD 800   // Нижний порог допустимого уровня содержания СО2
#define CO2_YELLOW_THRESHOLD 1200 // Верхний порог допустимого уровня содержания СО2

#include <LiquidCrystal_I2C.h>
void display_init();

#if (SENSORS == 1)
  #include <Adafruit_Sensor.h>
  #include <Adafruit_BME280.h>
#endif

#include <RTClib.h>
#include <GyverTimer.h>
#include <GyverEncoder.h>
#include <cactus_io_BME280_I2C.h>
#include <GyverButton.h>

#if (SENS_CO2 == 1)
  #include <MHZ19_uart.h>
#endif

void drawDig(uint8_t dig, uint8_t x, uint8_t y);
void drawDots(uint8_t x, uint8_t y, bool state);
void drawClock(uint8_t hours, uint8_t minutes, uint8_t x, uint8_t y);
void drawData();
void loadClock();

void setLED(uint8_t color, uint8_t lbd, uint8_t lbn);
void brightnessRef(uint8_t dbd, uint8_t dbn, uint8_t lbd, uint8_t lbn);
void brightnessControl();

void clockTick();
void get_time();

void draw_main_disp();

void checkInput();
bool Enc_IsClick();
bool Enc_IsDouble();
bool Enc_IsHolded();
void Enc_Reset();

uint8_t Mode(uint8_t x);
void drawFlags();
void EEPROM_init();
void power_control();
void reload_ch_flg();
void space_prt(uint8_t q);

#if (WEEK_LANG == 0)
static const char *dayNames[]  = {
  "Sund",
  "Mond",
  "Tues",
  "Wedn",
  "Thur",
  "Frid",
  "Satu",
};
static const char *optNames[] = {
  "Night&Bright",
  "Alarms OFF",
  "Alarms RST",
  "Disp brs day",
  "Disp brs night",
  "LED brs day",
  "LED brs night",
  "Disp blink",
  "Volume",
  "Debug start",
  "Up"
};
#else
static const char *dayNames[]  = {
  "BOCK",
  "POND",
  "BTOP",
  "CPED",
  "4ETB",
  "5YAT",
  "CYBB",
};
#endif



#if (ALARM == 1)
  #include "sound.h"
  #include "alarm.h"

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
  void alarm_OFF();
  void alarm_RST();
#else
  #include <EEPROM.h>
#endif

//--------------- Options display -----------------
#if (OPTION == 1)
  #define OPTION_STATUS_ADDR 13
  #define DIS_BRS_DAY_ADDR 14
  #define DIS_BRS_NIGHT_ADDR 15
  #define RGB_BRS_DAY_ADDR 16
  #define RGB_BRS_NIGHT_ADDR 17
  #define VOLUME_ADDR 18

  typedef struct option{
    uint8_t disp_pos;
    uint8_t param;
    uint8_t d_param;
    uint8_t id;
    String name;
  };

  typedef struct print{
    uint8_t s1_c1; uint8_t s1_c2;
    uint8_t s2_c1; uint8_t s2_c2;
    uint8_t s3_c1; uint8_t s3_c2;
    uint8_t s4_c1; uint8_t s4_c2;
  };

  void options();
  void cursor();
  bool cursor_get_pos();
  void opt_save();
  void opt_ref();
  void opt_change(bool dir);
  void opt_eeprom_save();
  void opt_eeprom_dwl();
  void processing();
  void opt_up();
  void opt_prt(struct print);
  uint8_t opt_fnd(uint8_t pos);
  void cursor_prt();
#endif

#if (DEBUG == 1)
  void go_debug();
  void debug_start();
#endif

#if (SENSORS == 1)
// пределы отображения для графиков
  #define TEMP_MIN 15         // Минимальны уровень отображения графика температуры
  #define TEMP_MAX 35         // Максимальный уровень отображения графика температуры
  #define HUM_MIN 0           // Минимальный уровень отображения графика влажности
  #define HUM_MAX 100         // Максимальный уровень отображения графика влажности
  #define PRESS_MIN -100      // Минимальный уровень отображения графика давления
  #define PRESS_MAX 100       // Максимальный уровень отображения графика давления
  #define CO2_MIN 300         // Минимальный уровень отображения графика СО2
  #define CO2_MAX 2000        // Максимальный уровень отображения графика СО2

  #if(GRAPH == 1)
    void modesTick();
    void drawPlot(uint8_t pos, uint8_t row, uint8_t width, uint8_t height, int min_val, int max_val, int *plot_array, uint8_t label);
    void loadPlot();
    void redrawPlot();
    void plotSensorsTick();
  #endif

  void init_sens();
  void readSensors();
  void drawSensors();
#endif

#endif