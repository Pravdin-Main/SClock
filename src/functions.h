#ifndef functions_h
#define functions_h

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
//#include <Adafruit_BME280.h>
#include <cactus_io_BME280_I2C.h>
#include <GyverTimer.h>
#include <RTClib.h>

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
void alarm();
void alarmTuning();

#endif