#include "functions.h"

static int8_t hrs;
static int8_t mins;
static int8_t secs;
static uint8_t set_param;

alarmTuner alarmTune;

uint8_t mode = 0;
bool dotFlag;
bool firstStartFlag = false;
uint8_t set_alarm;
bool draw_param;
float dispTemp;
uint8_t dispHum;
int dispPres;
int dispCO2;
int dispRain;
int tempHour[15], tempDay[15];
int humHour[15], humDay[15];
int pressHour[15], pressDay[15];
int co2Hour[15], co2Day[15];
int delta;
uint32_t pressure_array[6];
uint32_t sumX, sumY, sumX2, sumXY;
float a, b;
uint8_t time_array[6];

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

#if (LED_MODE == 0)
  uint8_t LED_ON = (LED_BRIGHT_MAX);
  uint8_t LED_OFF = (LED_BRIGHT_MIN);
#else
  uint8_t LED_ON = (255 - LED_BRIGHT_MAX);
  uint8_t LED_OFF = (255 - LED_BRIGHT_MIN);
#endif

#if (DISPLAY_TYPE == 1)
LiquidCrystal_I2C lcd(DISPLAY_ADDR, 20, 4);
#else
LiquidCrystal_I2C lcd(DISPLAY_ADDR, 16, 2); 
#endif

Encoder enc(CLK, DT, SW, TYPE2);
BME280_I2C bme(0x76);
GButton button(BTN_PIN, LOW_PULL, NORM_OPEN);

GTimer_ms hourPlotTimer((long)4 * 60 * 1000);         // 4 минуты
GTimer_ms dayPlotTimer((long)1.6 * 60 * 60 * 1000);   // 1.6 часа
GTimer_ms predictTimer((long)10 * 60 * 1000);         // 10 минут
GTimer_ms drawDown_param(1000);
GTimer_ms drawUp_param(200);
GTimer_ms backToMain(10000);

// символы
// график
uint8_t row8[8] = {0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
uint8_t row7[8] = {0b00000,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
uint8_t row6[8] = {0b00000,  0b00000,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
uint8_t row5[8] = {0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
uint8_t row4[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111,  0b11111};
uint8_t row3[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};
uint8_t row2[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111};
uint8_t row1[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111};

// цифры
uint8_t LT[8] = {0b00111,  0b01111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
uint8_t UB[8] = {0b11111,  0b11111,  0b11111,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000};
uint8_t RT[8] = {0b11100,  0b11110,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
uint8_t LL[8] = {0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b01111,  0b00111};
uint8_t LB[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};
uint8_t LR[8] = {0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11110,  0b11100};
uint8_t UMB[8] = {0b11111,  0b11111,  0b11111,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111};
uint8_t LMB[8] = {0b11111,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};

#if (CO2_SENSOR == 1)
MHZ19_uart mhz19;
#endif

RTC_DS3231 rtc;
DateTime now;

alarm alarm1;
alarm alarm2;
alarm alarm3;

void display_init(){
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void inition () {
    now = rtc.now();
    secs = now.second();
    mins = now.minute();
    hrs = now.hour();

    bme.readSensor();
    uint32_t Pressure = bme.getPressure_MB();
    for (byte i = 0; i < 6; i++) {   // счётчик от 0 до 5
      pressure_array[i] = Pressure;  // забить весь массив текущим давлением
      time_array[i] = i;             // забить массив времени числами 0 - 5
  }

   if (DISPLAY_TYPE == 1) {
    loadClock();
    drawClock(hrs, mins, 0, 0, 1);
    drawData();
   }
   readSensors();
   drawSensors();
}

bool Button_IsDouble(){
  return button.isDouble();
}

bool Enc_IsHolded(){
  return enc.isHolded();
}

uint8_t Mode(uint8_t x){
  if(x == 0) return mode;
  else{
    if(x > 0 && x < 10){
      mode = x;
      return mode;
    }
  }
  return 0;
}

void drawDig(uint8_t dig, uint8_t x, uint8_t y) {
  switch (dig) {
    case 0:
      lcd.setCursor(x, y); // set cursor to column 0, line 0 (first row)
      lcd.write(0);  // call each segment to create
      lcd.write(1);  // top half of the number
      lcd.write(2);
      lcd.setCursor(x, y + 1); // set cursor to colum 0, line 1 (second row)
      lcd.write(3);  // call each segment to create
      lcd.write(4);  // bottom half of the number
      lcd.write(5);
      break;
    case 1:
      lcd.setCursor(x + 1, y);
      lcd.write(1);
      lcd.write(2);
      lcd.setCursor(x + 2, y + 1);
      lcd.write(5);
      break;
    case 2:
      lcd.setCursor(x, y);
      lcd.write(6);
      lcd.write(6);
      lcd.write(2);
      lcd.setCursor(x, y + 1);
      lcd.write(3);
      lcd.write(7);
      lcd.write(7);
      break;
    case 3:
      lcd.setCursor(x, y);
      lcd.write(6);
      lcd.write(6);
      lcd.write(2);
      lcd.setCursor(x, y + 1);
      lcd.write(7);
      lcd.write(7);
      lcd.write(5);
      break;
    case 4:
      lcd.setCursor(x, y);
      lcd.write(3);
      lcd.write(4);
      lcd.write(2);
      lcd.setCursor(x + 2, y + 1);
      lcd.write(5);
      break;
    case 5:
      lcd.setCursor(x, y);
      lcd.write(0);
      lcd.write(6);
      lcd.write(6);
      lcd.setCursor(x, y + 1);
      lcd.write(7);
      lcd.write(7);
      lcd.write(5);
      break;
    case 6:
      lcd.setCursor(x, y);
      lcd.write(0);
      lcd.write(6);
      lcd.write(6);
      lcd.setCursor(x, y + 1);
      lcd.write(3);
      lcd.write(7);
      lcd.write(5);
      break;
    case 7:
      lcd.setCursor(x, y);
      lcd.write(1);
      lcd.write(1);
      lcd.write(2);
      lcd.setCursor(x + 1, y + 1);
      lcd.write(0);
      break;
    case 8:
      lcd.setCursor(x, y);
      lcd.write(0);
      lcd.write(6);
      lcd.write(2);
      lcd.setCursor(x, y + 1);
      lcd.write(3);
      lcd.write(7);
      lcd.write(5);
      break;
    case 9:
      lcd.setCursor(x, y);
      lcd.write(0);
      lcd.write(6);
      lcd.write(2);
      lcd.setCursor(x + 1, y + 1);
      lcd.write(4);
      lcd.write(5);
      break;
    case 10:
      lcd.setCursor(x, y);
      lcd.write(32);
      lcd.write(32);
      lcd.write(32);
      lcd.setCursor(x, y + 1);
      lcd.write(32);
      lcd.write(32);
      lcd.write(32);
      break;
  }
}

void drawdots(uint8_t x, uint8_t y, bool state) {
  byte code;
  if (state) code = 165;
  else code = 32;
  lcd.setCursor(x, y);
  lcd.write(code);
  lcd.setCursor(x, y + 1);
  lcd.write(code);
}

void drawClock(uint8_t hours, uint8_t minutes, uint8_t x, uint8_t y, bool dotState) {
  // чисти чисти!
  lcd.setCursor(x, y);
  lcd.print("               ");
  lcd.setCursor(x, y + 1);
  lcd.print("               ");

  //if (hours > 23 || minutes > 59) return;
  if (hours / 10 == 0) drawDig(10, x, y);
  else drawDig(hours / 10, x, y);
  drawDig(hours % 10, x + 4, y);
  // тут должны быть точки. Отдельной функцией
  drawDig(minutes / 10, x + 8, y);
  drawDig(minutes % 10, x + 12, y);
}

void drawData() {
  lcd.setCursor(15, 0);
  if (now.day() < 10) lcd.print(0);
  lcd.print(now.day());
  lcd.print(".");
  if (now.month() < 10) lcd.print(0);
  lcd.print(now.month());

  if (DISP_MODE == 0) {
    lcd.setCursor(16, 1);
    lcd.print(now.year());
  } else if (DISP_MODE == 1) {
    lcd.setCursor(16, 1);
    int dayofweek = now.dayOfTheWeek();
    lcd.print(dayNames[dayofweek]);
  }
}

void drawPlot(uint8_t pos, uint8_t row, uint8_t width, uint8_t height, int min_val, int max_val, int *plot_array, String label) {
  int max_value = -32000;
  int min_value = 32000;

  for (byte i = 0; i < 15; i++) {
    if (plot_array[i] > max_value) max_value = plot_array[i];
    if (plot_array[i] < min_value) min_value = plot_array[i];
  }
  lcd.setCursor(16, 0); lcd.print(max_value);
  lcd.setCursor(16, 1); lcd.print(label);
  lcd.setCursor(16, 2); lcd.print(plot_array[14]);
  lcd.setCursor(16, 3); lcd.print(min_value);

  for (byte i = 0; i < width; i++) {                  // каждый столбец параметров
    int fill_val = plot_array[i];
    fill_val = constrain(fill_val, min_val, max_val);
    byte infill, fract;
    // найти количество целых блоков с учётом минимума и максимума для отображения на графике
    if (plot_array[i] > min_val)
      infill = floor((float)(plot_array[i] - min_val) / (max_val - min_val) * height * 10);
    else infill = 0;
    fract = (float)(infill % 10) * 8 / 10;                   // найти количество оставшихся полосок
    infill = infill / 10;

    for (byte n = 0; n < height; n++) {     // для всех строк графика
      if (n < infill && infill > 0) {       // пока мы ниже уровня
        lcd.setCursor(i, (row - n));        // заполняем полными ячейками
        lcd.write(0);
      }
      if (n >= infill) {                    // если достигли уровня
        lcd.setCursor(i, (row - n));
        if (fract > 0) lcd.write(fract);          // заполняем дробные ячейки
        else lcd.write(16);                       // если дробные == 0, заливаем пустой
        for (byte k = n + 1; k < height; k++) {   // всё что сверху заливаем пустыми
          lcd.setCursor(i, (row - k));
          lcd.write(16);
        }
        break;
      }
    }
  }
}

void loadClock() {
  lcd.createChar(0, LT);
  lcd.createChar(1, UB);
  lcd.createChar(2, RT);
  lcd.createChar(3, LL);
  lcd.createChar(4, LB);
  lcd.createChar(5, LR);
  lcd.createChar(6, UMB);
  lcd.createChar(7, LMB);
}

void loadPlot() {
  lcd.createChar(0, row8);
  lcd.createChar(1, row1);
  lcd.createChar(2, row2);
  lcd.createChar(3, row3);
  lcd.createChar(4, row4);
  lcd.createChar(5, row5);
  lcd.createChar(6, row6);
  lcd.createChar(7, row7);
}

void setLED(uint8_t color) {
  // сначала всё выключаем
  if (!LED_MODE) {
    analogWrite(LED_R, 0);
    analogWrite(LED_G, 0);
    analogWrite(LED_B, 0);
  } else {
    analogWrite(LED_R, 255);
    analogWrite(LED_G, 255);
    analogWrite(LED_B, 255);
  }
  switch (color) {    // 0 выкл, 1 красный, 2 зелёный, 3 синий (или жёлтый)
    case 0:
      break;
    case 1: analogWrite(LED_R, LED_ON);
      break;
    case 2: analogWrite(LED_G, LED_ON);
      break;
    case 3:
      if (!BLUE_YELLOW) analogWrite(LED_B, LED_ON);
      else {
        analogWrite(LED_R, LED_ON - 50);    // чутка уменьшаем красный
        analogWrite(LED_G, LED_ON);
      }
      break;
  }
}

void checkBrightness() {
  if (analogRead(PHOTO) < BRIGHT_THRESHOLD) {   // если темно
    analogWrite(BACKLIGHT, LCD_BRIGHT_MIN);
#if (LED_MODE == 0)
    LED_ON = (LED_BRIGHT_MIN);
#else
    LED_ON = (255 - LED_BRIGHT_MIN);
#endif
  } else {                                      // если светло
    analogWrite(BACKLIGHT, LCD_BRIGHT_MAX);
#if (LED_MODE == 0)
    LED_ON = (LED_BRIGHT_MAX);
#else
    LED_ON = (255 - LED_BRIGHT_MAX);
#endif
  }
  if (dispCO2 < 800) setLED(2);
  else if (dispCO2 < 1200) setLED(3);
  else if (dispCO2 >= 1200) setLED(1);
}

void modesTick() {
  button.tick();
  bool changeFlag = false;
  if (button.isClick()) {
    mode++;

#if (CO2_SENSOR == 1)
    if (mode > 9) mode = 0;
#else
    if (mode > 6) mode = 0;
#endif
    changeFlag = true;
  }
  if (button.isHolded()) {
    mode = 0;
    changeFlag = true;
  }

  if (changeFlag) {
    if (mode == 0) {
      lcd.clear();
      loadClock();
      drawClock(hrs, mins, 0, 0, 1);
      if (DISPLAY_TYPE == 1) drawData();
      drawSensors();
    }
    else if(mode > 0 && mode < 9) {
      lcd.clear();
      loadPlot();
      redrawPlot();
    }
  }
}

void redrawPlot() {
  lcd.clear();
  switch (mode) {
    case 1: drawPlot(0, 3, 15, 4, TEMP_MIN, TEMP_MAX, (int*)tempHour, "t hr");
      break;
    case 2: drawPlot(0, 3, 15, 4, TEMP_MIN, TEMP_MAX, (int*)tempDay, "t day");
      break;
    case 3: drawPlot(0, 3, 15, 4, HUM_MIN, HUM_MAX, (int*)humHour, "h hr");
      break;
    case 4: drawPlot(0, 3, 15, 4, HUM_MIN, HUM_MAX, (int*)humDay, "h day");
      break;
    case 5: drawPlot(0, 3, 15, 4, PRESS_MIN, PRESS_MAX, (int*)pressHour, "p hr");
      break;
    case 6: drawPlot(0, 3, 15, 4, PRESS_MIN, PRESS_MAX, (int*)pressDay, "p day");
      break;
    case 7: drawPlot(0, 3, 15, 4, CO2_MIN, CO2_MAX, (int*)co2Hour, "c hr");
      break;
    case 8: drawPlot(0, 3, 15, 4, CO2_MIN, CO2_MAX, (int*)co2Day, "c day");
      break;
  }
}

void readSensors() {
  bme.readSensor();
  dispTemp = bme.getTemperature_C();
  dispHum = bme.getHumidity();
  dispPres = (float)bme.getPressure_MB() * 0.750062;
#if (CO2_SENSOR == 1)
  dispCO2 = mhz19.getPPM();

  if (dispCO2 < 800) setLED(2);
  else if (dispCO2 < 1200) setLED(3);
  else if (dispCO2 >= 1200) setLED(1);
#endif
}

void drawSensors() {
#if (DISPLAY_TYPE == 1)
  // дисплей 2004
  lcd.setCursor(0, 2);
  lcd.print(String(dispTemp, 1));
  lcd.write(223);
  lcd.setCursor(6, 2);
  lcd.print(" " + String(dispHum) + "%  ");

#if (CO2_SENSOR == 1)
  lcd.print(String(dispCO2) + " ppm");
  if (dispCO2 < 1000) lcd.print(" ");
#endif

  lcd.setCursor(0, 3);
  lcd.print(String(dispPres) + " mm  rain ");
  lcd.print(F("       "));
  lcd.setCursor(13, 3);
  lcd.print(String(dispRain) + "%");

#else
  // дисплей 1602
  lcd.setCursor(0, 0);
  lcd.print(String(dispTemp, 1));
  lcd.write(223);
  lcd.setCursor(6, 0);
  lcd.print(String(dispHum) + "% ");

#if (CO2_SENSOR == 1)
  lcd.print(String(dispCO2) + "ppm");
  if (dispCO2 < 1000) lcd.print(" ");
#endif

  lcd.setCursor(0, 1);
  lcd.print(String(dispPres) + " mm  rain ");
  lcd.print(String(dispRain) + "% ");
#endif
}

void plotSensorsTick() {
  // 4 минутный таймер
  if (hourPlotTimer.isReady()) {
    for (byte i = 0; i < 14; i++) {
      tempHour[i] = tempHour[i + 1];
      humHour[i] = humHour[i + 1];
      pressHour[i] = pressHour[i + 1];
      co2Hour[i] = co2Hour[i + 1];
    }
    tempHour[14] = dispTemp;
    humHour[14] = dispHum;
    co2Hour[14] = dispCO2;

    if (PRESSURE) pressHour[14] = dispRain;
    else pressHour[14] = dispPres;
  }

  // 1.5 часовой таймер
  if (dayPlotTimer.isReady()) {
    long averTemp = 0, averHum = 0, averPress = 0, averCO2 = 0;

    for (byte i = 0; i < 15; i++) {
      averTemp += tempHour[i];
      averHum += humHour[i];
      averPress += pressHour[i];
      averCO2 += co2Hour[i];
    }
    averTemp /= 15;
    averHum /= 15;
    averPress /= 15;
    averCO2 /= 15;

    for (byte i = 0; i < 14; i++) {
      tempDay[i] = tempDay[i + 1];
      humDay[i] = humDay[i + 1];
      pressDay[i] = pressDay[i + 1];
      co2Day[i] = co2Day[i + 1];
    }
    tempDay[14] = averTemp;
    humDay[14] = averHum;
    pressDay[14] = averPress;
    co2Day[14] = averCO2;
  }

  // 10 минутный таймер
  if (predictTimer.isReady()) {
    // тут делаем линейную аппроксимацию для предсказания погоды
    long averPress = 0;
    for (byte i = 0; i < 10; i++) {
      bme.readSensor();
      averPress += bme.getPressure_MB();
      delay(1);
    }
    averPress /= 10;

    for (byte i = 0; i < 5; i++) {                   // счётчик от 0 до 5 (да, до 5. Так как 4 меньше 5)
      pressure_array[i] = pressure_array[i + 1];     // сдвинуть массив давлений КРОМЕ ПОСЛЕДНЕЙ ЯЧЕЙКИ на шаг назад
    }
    pressure_array[5] = averPress;                    // последний элемент массива теперь - новое давление
    sumX = 0;
    sumY = 0;
    sumX2 = 0;
    sumXY = 0;
    for (int i = 0; i < 6; i++) {                    // для всех элементов массива
      sumX += time_array[i];
      sumY += (long)pressure_array[i];
      sumX2 += time_array[i] * time_array[i];
      sumXY += (long)time_array[i] * pressure_array[i];
    }
    a = 0;
    a = (long)6 * sumXY;             // расчёт коэффициента наклона приямой
    a = a - (long)sumX * sumY;
    a = (float)a / (6 * sumX2 - sumX * sumX);
    delta = a * 6;      // расчёт изменения давления
    dispRain = map(delta, -250, 250, 100, -100);  // пересчитать в проценты
    //Serial.println(String(pressure_array[5]) + " " + String(delta) + " " + String(dispRain));   // дебаг
  }
}

void clockTick() {
  dotFlag = !dotFlag;
  if (dotFlag) {          // каждую секунду пересчёт времени
    secs++;
    if (secs > 59) {      // каждую минуту
      secs = 0;
      mins++;
      if (mins <= 59 && mode == 0) drawClock(hrs, mins, 0, 0, 1);
    }
    if (mins > 59) {      // каждый час
      now = rtc.now();
      secs = now.second();
      mins = now.minute();
      hrs = now.hour();
      if (mode == 0) drawClock(hrs, mins, 0, 0, 1);
      if (hrs > 23) {
        hrs = 0;
      }
      if (mode == 0 && DISPLAY_TYPE) drawData();
    }
    if (DISP_MODE == 2 && mode == 0) {
      lcd.setCursor(16, 1);
      if (secs < 10) lcd.print(" ");
      lcd.print(secs);
    }
  }
  if (mode == 0) drawdots(7, 0, dotFlag);
  if (dispCO2 >= 1200) {
    if (dotFlag) setLED(1);
    else setLED(0);
  }
}

//---------------------ALARM--------------------------------------------------

void drawAlarmClock(uint8_t hours, uint8_t minutes, uint8_t x, uint8_t y, bool draw) {
  lcd.setCursor(x, y);
  lcd.print("               ");
  lcd.setCursor(x, y + 1);
  lcd.print("               ");

  //if (hours > 23 || minutes > 59) return;
  if(set_param == 1 && draw){
    if (hours / 10 == 0) drawDig(10, x, y);
    else drawDig(hours / 10, x, y);
    drawDig(hours % 10, x + 4, y);
  }

  drawdots(7, 0, true);
  
  if(set_param == 2 && draw){
    drawDig(minutes / 10, x + 8, y);
    drawDig(minutes % 10, x + 12, y);
  }
}

void alarmTuning(){
  
  if (!firstStartFlag){
    alarmTune.hour = hrs;
    alarmTune.minute = mins;
    alarmTune.sound = 1;
    alarmTune.status = false;
    firstStartFlag = true;
    set_alarm = 1;
    set_param = 1;
    loadClock();
  }

  if (enc.isDouble()){
    switch (set_alarm){
      case 1:
        alarm1.alarm_set(alarmTune.hour, alarmTune.minute, alarmTune.sound, alarmTune.status);
        break;
      case 2:
        alarm2.alarm_set(alarmTune.hour, alarmTune.minute, alarmTune.sound, alarmTune.status);
        break;
      case 3:
        alarm3.alarm_set(alarmTune.hour, alarmTune.minute, alarmTune.sound, alarmTune.status);
        break;
      default:
        break;
    }
    ++set_alarm;
    if (set_alarm > 3) set_alarm = 1;
  }
  
  if(enc.isSingle()){
    ++set_param;
    if(set_param > 4) set_param = 1;
    backToMain.reset();
  }

  if(enc.isRightH()){
    ++set_alarm;
    if (set_alarm > 3) set_alarm = 1;
    backToMain.reset();
  }
  
  if (enc.isLeftH()){
    --set_alarm;
    if (set_alarm < 1) set_alarm = 3;
    backToMain.reset();
  }

  if(enc.isRight()){
    switch (set_param){
      case 1:
        ++alarmTune.hour;
        if (alarmTune.hour > 23) alarmTune.hour = 0;
        break;
      case 2:
        ++alarmTune.minute;
        if(alarmTune.minute > 59) alarmTune.minute = 0;
        break;
      case 3:
        ++alarmTune.sound;
        if(alarmTune.sound > 3) alarmTune.sound = 1;
        break;
      case 4:
        if(alarmTune.status) alarmTune.status = false;
          else{alarmTune.status = true;}          
        break;
      default:
        break;
    }
    backToMain.reset();
  }

  if(enc.isLeft()){
    switch (set_param){
      case 1:
        --alarmTune.hour;
        if (alarmTune.hour < 0) alarmTune.hour = 23;
        break;
      case 2:
        --alarmTune.minute;
        if(alarmTune.minute < 0) alarmTune.minute = 59;
        break;
      case 3:
        --alarmTune.sound;
        if(alarmTune.sound < 1) alarmTune.sound = 3;
        break;
      case 4:
        if(alarmTune.status) alarmTune.status = false;
          else{alarmTune.status = true;} 
        break;
      default:
        break;
    }
    backToMain.reset();
  }

  //--------------------------- print alarmTune -------------------------------------------  
  lcd.clear();
  
  if(drawDown_param.isReady()) {
    drawDown_param.stop();
    draw_param = false;
    drawUp_param.start();
  }

  if(drawUp_param.isReady()) {
    drawUp_param.stop();
    draw_param = true;
    drawDown_param.start();
  }
  
  drawAlarmClock(alarmTune.hour, alarmTune.minute, 0, 0, draw_param);

  if(set_param == 3 && draw_param){
    lcd.setCursor(15,0); lcd.print("s-" + String(alarmTune.sound));
  }

  if(set_param == 4 && draw_param){
    lcd.setCursor(15,1); alarmTune.status ? lcd.print("ON") : lcd.print("OFF");
  }

  switch(set_param){
    case 1:
      drawDig(alarmTune.minute / 10, 8, 0);
      drawDig(alarmTune.minute % 10, 12, 0);
      lcd.setCursor(15,0); lcd.print("s-" + String(alarmTune.sound));
      lcd.setCursor(15,1); alarmTune.status ? lcd.print("ON") : lcd.print("OFF");
      break;
    case 2:
      drawDig(alarmTune.hour / 10, 8, 0);
      drawDig(alarmTune.hour % 10, 12, 0);
      lcd.setCursor(15,0); lcd.print("s-" + String(alarmTune.sound));
      lcd.setCursor(15,1); alarmTune.status ? lcd.print("ON") : lcd.print("OFF");
      break;
    case 3:
      drawDig(alarmTune.hour / 10, 8, 0);
      drawDig(alarmTune.hour % 10, 12, 0);
      drawDig(alarmTune.minute / 10, 8, 0);
      drawDig(alarmTune.minute % 10, 12, 0);
      lcd.setCursor(15,1); alarmTune.status ? lcd.print("ON") : lcd.print("OFF");
      break;
    case 4:
      drawDig(alarmTune.hour / 10, 8, 0);
      drawDig(alarmTune.hour % 10, 12, 0);
      drawDig(alarmTune.minute / 10, 8, 0);
      drawDig(alarmTune.minute % 10, 12, 0);
      lcd.setCursor(15,0); lcd.print("s-" + String(alarmTune.sound));
      break;
    default:
      break;
  }

  //--------------------------- print alarm1 -----------------------------------------------
  lcd.setCursor(0,2);
  if (alarm1.get_wakeHour() >= 10) lcd.print(String(alarm1.get_wakeHour())+":");
    else { lcd.print("0" + String(alarm1.get_wakeHour()) + ":"); }
  
  lcd.setCursor(3,2);
  if (alarm1.get_wakeMinute() >= 10) lcd.print(String(alarm1.get_wakeMinute()));
    else {lcd.print("0" + String(alarm1.get_wakeMinute()));}

  lcd.setCursor(0,3);
  lcd.print("s" + String(alarm1.get_wakeSound())); 

  lcd.setCursor(3,3);
  alarm1.get_wakeStatus() ? lcd.print("ON") : lcd.print ("OFF");
  
  //--------------------------- print alarm2 -----------------------------------------------
  lcd.setCursor(7,2);
  if (alarm2.get_wakeHour() >= 10) lcd.print(String(alarm2.get_wakeHour())+":");
    else { lcd.print("0" + String(alarm2.get_wakeHour()) + ":"); }

  lcd.setCursor(10,2);
  if (alarm2.get_wakeMinute() >= 10) lcd.print(String(alarm2.get_wakeMinute()));
    else {lcd.print("0" + String(alarm2.get_wakeMinute()));}

  lcd.setCursor(7,3);
  lcd.print("s" + String(alarm2.get_wakeSound()));

  lcd.setCursor(10,3);
  alarm2.get_wakeStatus() ? lcd.print("ON") : lcd.print ("OFF");

  //--------------------------- print alarm3 -----------------------------------------------
  lcd.setCursor(14,2);
  if (alarm3.get_wakeHour() >= 10) lcd.print(String(alarm3.get_wakeHour())+":");
    else { lcd.print("0" + String(alarm3.get_wakeHour()) + ":"); }
  
  lcd.setCursor(17,2);
  if (alarm3.get_wakeMinute() >= 10) lcd.print(String(alarm3.get_wakeMinute()));
    else {lcd.print("0" + String(alarm3.get_wakeMinute()));}

  lcd.setCursor(14,3);
  lcd.print("s" + String(alarm3.get_wakeSound()));

  lcd.setCursor(17,3);
  alarm3.get_wakeStatus() ? lcd.print("ON") : lcd.print ("OFF");
  //--------------------------------------------------------------------------------------

  switch (set_alarm){
    case 1:
      lcd.setCursor(5,2); lcd.print(60);
      break;
    case 2:
      lcd.setCursor(12,2); lcd.print(60);
      break;
    case 3:
      lcd.setCursor(19,2); lcd.print(60);
      break;
    default:
      break;
  }

  if(backToMain.isReady() || enc.isHolded()){
    firstStartFlag = false;
    mode = 0;
  }
}

void alarmStart(uint8_t set){
  switch(set) {
    case 1:
      alarm1.wakeUP();
      break;
    case 2:
      alarm1.wakeUP();
      break;
    case 3:
      alarm1.wakeUP();
      break;
    default:
     break;
  }
}

uint8_t alarmControl(){
  if(alarm1.checkAlarm(hrs, mins)) return 1;
  if(alarm2.checkAlarm(hrs, mins)) return 2;
  if(alarm3.checkAlarm(hrs, mins)) return 3;
    else{
      return 0;
    }
  
}

void alarmStop(){
    if (alarm1.isRunning()) alarm1.stop();
    if (alarm2.isRunning()) alarm2.stop();
    if (alarm3.isRunning()) alarm3.stop();
}
