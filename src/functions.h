#ifndef functions_h
#define functions_h

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
//#include <Adafruit_BME280.h>
#include <cactus_io_BME280_I2C.h>
#include <GyverTimer.h>
#include <GyverButton.h>
GButton button(BTN_PIN, LOW_PULL, NORM_OPEN);
#include <QBPlay.h>
QBPlay* play;
const byte buzzer = A7; // Динамики
// Тема из Star Wars
const char sound1[] = "MNT150L2O3CGP16L16FEDL2>C<GP16L16FEDL2>C<GP16L16F\
EFL2DP16L16<GGGL2>CGP32L16FEDL2>C<GP16L16FEDL2>C<GP16L16A+AA+L1GL2G.L8<G.\
L16GL4A.L8A>FEDCL16CDEDP16L8<AL4BL8G.L16GL4A.L8A>FEDCGP8L4D.P8L8<G.L16GL4\
A.L8A>FEDCL16CDEDP16L8<AL4BP16L8>G.L16GL8>C.L16<A+L8G+.L16GL8F.L16D+L8D.L\
16CL1GL2G.P16L16GGGL8>CP8L16<CCCL2C.";
// 
const char sound2[];
//
const char sound3[];

// const custom_t sounds[] = {custom_t{sound1}, custom_t{sound2}, custom_t{sound3}};

byte mode = 0;
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

void drawDig(byte dig, byte x, byte y);
void drawdots(byte x, byte y, boolean state);
void drawClock(byte hours, byte minutes, byte x, byte y, boolean dotState);
void drawData();
void drawPlot(byte pos, byte row, byte width, byte height, int min_val, int max_val, int *plot_array, String label);
void loadClock();
void loadPlot();
void setLED(byte color);
void checkBrightness();
void modesTick();
void redrawPlot();
void readSensors();
void drawSensors();
void plotSensorsTick();
void clockTick();
void inition ();
void alarmTuning();
bool alarmControl();
void drawAlarmClock(byte hours, byte minutes, byte x, byte y, bool draw);
void alarmStop();
class alarm {
  
  public:
    alarm (uint8_t hour, uint8_t minute, uint8_t sound, bool status){
      set(hour, minute, sound, status);
    }
    void set(uint8_t hour, uint8_t minute, uint8_t sound, bool status);
    uint8_t get_wakeHour();
    uint8_t get_wakeMinute();
    uint8_t get_wakeSound();
    bool get_wakeStatus();
    bool checkAlarm (uint8_t hour, uint8_t minute);
    void wakeUP();
    void stop();
    bool isRunning();

  private:
    uint8_t wakeHour;
    uint8_t wakeMinute;
    uint8_t wakeSound;
    bool wakeStatus = false;
    bool state = false;
};

struct alarmTune {
    uint8_t hour;
    uint8_t minute;
    uint8_t sound;
    bool status;
} alarmTune;

bool firstStartFlag = false;
uint8_t set_alarm;
static uint8_t set_param;
bool draw_param;

#endif