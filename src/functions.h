#ifndef FUNCTION_HEADER_H
#define FUNCTION_HEADER_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <GyverTimer.h>
#include <RTClib.h>

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

#endif