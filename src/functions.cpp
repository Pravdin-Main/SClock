#include "functions.h"

static int8_t hrs;
static int8_t mins;
static int8_t secs;
static uint8_t set_param;
static uint8_t mode = 0;
static bool change_flag;
static const char *labels[]  = {
  "t hr",
  "t day",
  "h hr",
  "h day",
  "p hr",
  "p day",
  "c hr",
  "c day"
};

static bool ch_flg_hr1 = true;
static bool ch_flg_hr2 = true;
static bool ch_flg_min1 = true;
static bool ch_flg_min2 = true;
static bool ch_flg_day = true;
static bool ch_flg_month = true;
static bool ch_flg_week = true;
static bool ch_flg_temp = true;
static bool ch_flg_hum = true;
static bool ch_flg_co2 = true;
static bool ch_flg_press = true;
static bool ch_flg_alarm = true;
static bool ch_flg_power = true;
static bool low_power;

option N_B, A_OFF, A_RST, D_brs_day, D_brs_night, LED_brs_day, LED_brs_night, Display_mode, vol, debug, up;
static bool opt_status;   // 0 - N_B, 1 - A_OFF, 2 - A_RST, 3 - debug; 4 - up; 5 - display_mode
static uint8_t cursor_pos = 1;
// uint8_t Dis_brs_day, Dis_brs_night, RGB_brs_day, RGB_brs_night, volume, disp_mode_param;
print opt,lst_opt;

alarmTuner alarmTune;

bool dotFlag;
bool firstStartFlag = false;
uint8_t set_alarm;
bool draw_param;
float dispTemp;
uint8_t dispHum;
int dispPres;
int dispCO2;
// int dispRain;
int tempHour[15], tempDay[15];
int humHour[15], humDay[15];
int pressHour[15], pressDay[15];
int co2Hour[15], co2Day[15];
// int delta;
uint32_t pressure_array[6];
uint32_t sumX, sumY, sumX2, sumXY;
float a, b;
uint8_t time_array[6];

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
GTimer_ms drawDown_param(1500);
GTimer_ms drawUp_param(400);
GTimer_ms backToMain(20000);

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
  backToMain.stop();

  #if (CO2_SENSOR == 1)
    mhz19.begin(MHZ_TX, MHZ_RX);
    mhz19.setAutoCalibration(false);
  #endif
  rtc.begin();
  bme.begin();
  bme.setTempCal(-1);
  if (RESET_CLOCK || rtc.lostPower()) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
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

void Enc_Tick(){
  enc.tick();
}

bool Enc_IsClick(){
  return enc.isSingle();
}

bool Enc_IsDouble(){
  return enc.isDouble();
}

bool Enc_IsHolded(){
  return enc.isHolded();
}

void Enc_Reset(){
  enc.resetStates();
}

uint8_t Mode(uint8_t x){
  if(x == 0) return mode;
  else{
    if(x > 0 && x <= 10){
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
  if(ch_flg_hr1){
    lcd.setCursor(x, y);
    lcd.print("   ");
    lcd.setCursor(x, y + 1);
    lcd.print("   ");
    if (hours / 10 == 0) drawDig(10, x, y);
      else drawDig(hours / 10, x, y);
    ch_flg_hr1 = false;
  }
  
  if(ch_flg_hr2){
    lcd.setCursor(x + 4, y);
    lcd.print("   ");
    lcd.setCursor(x + 4, y + 1);
    lcd.print("   ");
    drawDig(hours % 10, x + 4, y);
    ch_flg_hr2 = false;
  }

  if(ch_flg_min1){
    lcd.setCursor(x + 8, y);
    lcd.print("   ");
    lcd.setCursor(x + 8, y + 1);
    lcd.print("   ");
    drawDig(minutes / 10, x + 8, y);
    ch_flg_min1 = false;
  }

  if(ch_flg_min2){
    lcd.setCursor(x + 12, y);
    lcd.print("   ");
    lcd.setCursor(x + 12, y + 1);
    lcd.print("   ");
    drawDig(minutes % 10, x + 12, y);
    ch_flg_min2 = false;
  }
}

void drawData() {
  if(ch_flg_day){
    lcd.setCursor(15, 0);
    lcd.print("   ");
    lcd.setCursor(15, 0);
    if (now.day() < 10) lcd.print(0);
    lcd.print(now.day());
    lcd.print(".");
    ch_flg_day = false;
  }
  
  if(ch_flg_month){
    lcd.setCursor(18, 0);
    lcd.print("  ");
    lcd.setCursor(18, 0);
    if (now.month() < 10) lcd.print(0);
    lcd.print(now.month());
    ch_flg_month = false;
  }
  

  if(ch_flg_week){
    lcd.setCursor(16, 1);
    lcd.print("    ");
    lcd.setCursor(16, 1);
    if (DISP_MODE == 0) {
      lcd.print(now.year());
    }
      else if (DISP_MODE == 1) {
        int dayofweek = now.dayOfTheWeek();
        lcd.print(dayNames[dayofweek]);
      }
  }
  
}

void drawPlot(uint8_t pos, uint8_t row, uint8_t width, uint8_t height, int min_val, int max_val, int *plot_array, uint8_t label) {
  int max_value = -32000;
  int min_value = 32000;

  for (byte i = 0; i < 15; i++) {
    if (plot_array[i] > max_value) max_value = plot_array[i];
    if (plot_array[i] < min_value) min_value = plot_array[i];
  }
  lcd.setCursor(16, 0); lcd.print(max_value);
  lcd.setCursor(16, 1); lcd.print(labels[label]);
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
  if (button.isClick() || enc.isRight()) {
    mode++;

#if (CO2_SENSOR == 1)
    if (mode > 8) mode = 0;
#else
    if (mode > 6) mode = 0;
#endif
    changeFlag = true;
    backToMain.start();
    backToMain.reset();
  }

  if(enc.isLeft()){
    if(mode != 0) mode--;
      else { mode = 8; }
    changeFlag = true;
    backToMain.start();
    backToMain.reset();
  }

  
  if(mode == 0){
    if(button.isHolded()){
      mode = 10;
    }
  }
    else{
      if (button.isHolded() || backToMain.isReady()) {
      mode = 0;
      changeFlag = true;
      }
    }

  if (changeFlag) {
    if (mode == 0) {
      lcd.clear();
      reload_ch_flg();
      loadClock();
      drawClock(hrs, mins, 0, 0, 1);
      if (DISPLAY_TYPE == 1) drawData();
      drawSensors();
      drawFlags();
      backToMain.stop();
    }
    else if(mode > 0 && mode < 9) {
      lcd.clear();
      loadPlot();
      redrawPlot();
    }
  }
}

void redrawPlot() {
  // lcd.clear();
  switch (mode) {
    case 1: drawPlot(0, 3, 15, 4, TEMP_MIN, TEMP_MAX, (int*)tempHour, 0);
      break;
    case 2: drawPlot(0, 3, 15, 4, TEMP_MIN, TEMP_MAX, (int*)tempDay, 1);
      break;
    case 3: drawPlot(0, 3, 15, 4, HUM_MIN, HUM_MAX, (int*)humHour, 2);
      break;
    case 4: drawPlot(0, 3, 15, 4, HUM_MIN, HUM_MAX, (int*)humDay, 3);
      break;
    case 5: drawPlot(0, 3, 15, 4, PRESS_MIN, PRESS_MAX, (int*)pressHour, 4);
      break;
    case 6: drawPlot(0, 3, 15, 4, PRESS_MIN, PRESS_MAX, (int*)pressDay, 5);
      break;
    case 7: drawPlot(0, 3, 15, 4, CO2_MIN, CO2_MAX, (int*)co2Hour, 6);
      break;
    case 8: drawPlot(0, 3, 15, 4, CO2_MIN, CO2_MAX, (int*)co2Day, 7);
      break;
  }
}

void readSensors() {
  bme.readSensor();
  if(bme.getTemperature_C() != dispTemp){
    dispTemp = bme.getTemperature_C();
    ch_flg_temp = true;
  }

  if(bme.getHumidity() != dispHum){
    dispHum = bme.getHumidity();
    ch_flg_hum = true;
  }

  int dispPres_cache = (float)bme.getPressure_MB() * 0.750062;
  if(dispPres_cache != dispPres){
    dispPres = dispPres_cache;
    ch_flg_press = true;
  }
#if (CO2_SENSOR == 1)
  int dispCO2_cache = mhz19.getPPM();
  if(dispCO2_cache != dispCO2){
    dispCO2 = dispCO2_cache;
    ch_flg_co2 = true;
  }

  if (dispCO2 < 800) setLED(2);
  else if (dispCO2 < 1200) setLED(3);
  else if (dispCO2 >= 1200) setLED(1);
#endif
}

void drawSensors() {
#if (DISPLAY_TYPE == 1)
  // дисплей 2004
  if(ch_flg_temp){
    lcd.setCursor(0, 2);
    lcd.print("       ");
    lcd.setCursor(0, 2);
    lcd.print(dispTemp, 1);
    lcd.write(223);
    ch_flg_temp = false;
  }
  
  if(ch_flg_hum){
    lcd.setCursor(7, 2);
    lcd.print("     ");
    lcd.setCursor(7, 2);
    lcd.print(dispHum);
    lcd.print("%");
    ch_flg_hum = false;
  }

#if (CO2_SENSOR == 1)
  if(ch_flg_co2){
    lcd.setCursor(12, 2);
    lcd.print("        ");
    lcd.setCursor(12, 2);
    lcd.print(dispCO2);
    lcd.print("ppm");
    ch_flg_co2 = false;
  }
#endif

  if(ch_flg_press){
    lcd.setCursor(0, 3);
    lcd.print("        ");
    lcd.setCursor(0, 3);
    lcd.print(dispPres);
    lcd.print("mm");
    ch_flg_press = false;
  }
  
  // lcd.print(F("       "));
  // lcd.setCursor(13, 3);
  // lcd.print(dispRain); lcd.print("%");

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
  lcd.print(String(dispPres) + " mm");
  // lcd.print(String(dispRain) + "% ");
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

  //   if (PRESSURE) pressHour[14] = dispRain;
  //   else pressHour[14] = dispPres;
  }

  pressHour[14] = dispPres;

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
    // a = 0;
    // a = (long)6 * sumXY;             // расчёт коэффициента наклона приямой
    // a = a - (long)sumX * sumY;
    // a = (float)a / (6 * sumX2 - sumX * sumX);
    // delta = a * 6;      // расчёт изменения давления
    // dispRain = map(delta, -250, 250, 100, -100);  // пересчитать в проценты
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
      if(mins % 10 == 0){
        ch_flg_min1 = true;
      }
        else{ ch_flg_min2 = true; }
      if (mins <= 59 && mode == 0) drawClock(hrs, mins, 0, 0, 1);
    }
    if (mins > 59) {      // каждый час
      now = rtc.now();
      secs = now.second();
      mins = now.minute();
      hrs = now.hour();
      if(hrs % 10 == 0){
        ch_flg_hr1 = true;
        ch_flg_hr2 = true;
      }
        else { ch_flg_hr2 = true; }
      if (hrs > 23) {
        ch_flg_hr1 = true;
        ch_flg_hr2 = true;
        ch_flg_day = true;
        ch_flg_month = true;
        ch_flg_week = true;
      }
      if (mode == 0) {
        drawClock(hrs, mins, 0, 0, 1);
        drawData();
      }
      if (hrs > 23) {
        hrs = 0;
      }
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

  if(change_flag){
    switch(set_param) {
      case 1:
        lcd.setCursor(x, y);
        lcd.print("       ");
        lcd.setCursor(x, y + 1);
        lcd.print("       ");
        break;
      case 2:
        lcd.setCursor(x + 8, y);
        lcd.print("       ");
        lcd.setCursor(x + 8, y + 1);
        lcd.print("       ");
        break;
      case 3:
        lcd.setCursor(15, 0); lcd.print("     ");
        break;
      case 4:
        lcd.setCursor(15, 1); lcd.print("     ");
        break;
      default:
        break;
    }
  }

  //if (hours > 23 || minutes > 59) return;
  if(set_param == 1 && draw){
    if (hours / 10 == 0) drawDig(10, x, y);
    else drawDig(hours / 10, x, y);
    drawDig(hours % 10, x + 4, y);
  }
  else if(set_param == 1 && !draw){
    lcd.setCursor(x, y);
    lcd.print("       ");
    lcd.setCursor(x, y + 1);
    lcd.print("       ");
  }

  drawdots(7, 0, true);
  
  if(set_param == 2 && draw){
    drawDig(minutes / 10, 8, y);
    drawDig(minutes % 10, 12, y);
  }
  else if(set_param == 2 && !draw){
    lcd.setCursor(x + 8, y);
    lcd.print("       ");
    lcd.setCursor(x + 8, y + 1);
    lcd.print("       ");
  }

  if(set_param == 3 && draw){
    lcd.setCursor(15, 0); lcd.print("s-"); lcd.print(alarmTune.sound);
  }
  else if (set_param == 3 && !draw){
    lcd.setCursor(15, 0); lcd.print("     ");
  }

  if(set_param == 4 && draw){
    lcd.setCursor(15, 1); alarmTune.status ? lcd.print("ON") : lcd.print("OFF");
  }
  else if(set_param == 4 && !draw){
    lcd.setCursor(15, 1); lcd.print("     ");
  }

  switch(set_param){
    case 1:
      drawDig(alarmTune.minute / 10, 8, 0);
      drawDig(alarmTune.minute % 10, 12, 0);
      lcd.setCursor(15,0); lcd.print("s-"); lcd.print(alarmTune.sound);
      lcd.setCursor(15,1); alarmTune.status ? lcd.print("ON") : lcd.print("OFF");
      break;
    case 2:
      drawDig(alarmTune.hour / 10, 0, 0);
      drawDig(alarmTune.hour % 10, 4, 0);
      lcd.setCursor(15,0); lcd.print("s-"); lcd.print(alarmTune.sound);
      lcd.setCursor(15,1); alarmTune.status ? lcd.print("ON") : lcd.print("OFF");
      break;
    case 3:
      drawDig(alarmTune.hour / 10, 0, 0);
      drawDig(alarmTune.hour % 10, 4, 0);
      drawDig(alarmTune.minute / 10, 8, 0);
      drawDig(alarmTune.minute % 10, 12, 0);
      lcd.setCursor(15,1); alarmTune.status ? lcd.print("ON") : lcd.print("OFF");
      break;
    case 4:
      drawDig(alarmTune.hour / 10, 0, 0);
      drawDig(alarmTune.hour % 10, 4, 0);
      drawDig(alarmTune.minute / 10, 8, 0);
      drawDig(alarmTune.minute % 10, 12, 0);
      lcd.setCursor(15,0); lcd.print("s-"); lcd.print(alarmTune.sound);
      break;
    default:
      break;
  }

  //--------------------------- print alarm1 -----------------------------------------------
  lcd.setCursor(0,2);
  int8_t hour = alarm1.get_wakeHour();
  if (hour >= 10) { lcd.print(hour); lcd.print(":"); }
    else { lcd.print("0"); lcd.print(alarm1.get_wakeHour()); lcd.print(":"); }
  
  lcd.setCursor(3,2);
  if (alarm1.get_wakeMinute() >= 10) lcd.print(alarm1.get_wakeMinute());
    else {  lcd.print("0"); lcd.print(alarm1.get_wakeMinute()); }

  lcd.setCursor(0,3);
  lcd.print("s"); lcd.print(alarm1.get_wakeSound()); 

  lcd.setCursor(3,3);
  alarm1.get_wakeStatus() ? lcd.print("ON") : lcd.print ("OFF");
  
  //--------------------------- print alarm2 -----------------------------------------------
  lcd.setCursor(7,2);
  if (alarm2.get_wakeHour() >= 10){ lcd.print(alarm2.get_wakeHour()); lcd.print(":"); }
    else { lcd.print("0"); lcd.print(alarm2.get_wakeHour()); lcd.print(":"); }

  lcd.setCursor(10,2);
  if (alarm2.get_wakeMinute() >= 10) lcd.print(alarm2.get_wakeMinute());
    else { lcd.print("0"); lcd.print(alarm2.get_wakeMinute()); }

  lcd.setCursor(7,3);
  lcd.print("s"); lcd.print(alarm2.get_wakeSound());

  lcd.setCursor(10,3);
  alarm2.get_wakeStatus() ? lcd.print("ON") : lcd.print ("OFF");

  //--------------------------- print alarm3 -----------------------------------------------
  lcd.setCursor(14,2);
  if (alarm3.get_wakeHour() >= 10) { lcd.print(alarm3.get_wakeHour()); lcd.print(":"); }
    else { lcd.print("0"); lcd.print(alarm3.get_wakeHour()); lcd.print(":"); }
  
  lcd.setCursor(17,2);
  if (alarm3.get_wakeMinute() >= 10) lcd.print(alarm3.get_wakeMinute());
    else { lcd.print("0"); lcd.print(alarm3.get_wakeMinute()); }

  lcd.setCursor(14,3);
  lcd.print("s"); lcd.print(alarm3.get_wakeSound());

  lcd.setCursor(17,3);
  alarm3.get_wakeStatus() ? lcd.print("ON") : lcd.print ("OFF");
  //--------------------------------------------------------------------------------------

  switch (set_alarm){
    case 1:
      lcd.setCursor(5, 2); lcd.print("<");
      lcd.setCursor(12, 2); lcd.print(" ");
      lcd.setCursor(19, 2); lcd.print(" ");
      break;
    case 2:
      lcd.setCursor(5, 2); lcd.print(" ");
      lcd.setCursor(12, 2); lcd.print("<");
      lcd.setCursor(19, 2); lcd.print(" ");
      break;
    case 3:
      lcd.setCursor(5, 2); lcd.print(" ");
      lcd.setCursor(12, 2); lcd.print(" ");
      lcd.setCursor(19, 2); lcd.print("<");
      break;
    default:
      break;
  }
}

void alarmTuning(){
  button.tick();
  if (!firstStartFlag){
    alarmTune.hour = hrs;
    alarmTune.minute = mins;
    alarmTune.sound = 1;
    alarmTune.status = false;
    firstStartFlag = true;
    set_alarm = 1;
    set_param = 1;

    loadClock();
    lcd.clear();
    enc.resetStates();

    backToMain.start();
    backToMain.reset();
    
    drawAlarmClock(alarmTune.hour, alarmTune.minute, 0, 0, true);
  }
  change_flag = false;

  if (enc.isDouble()){
    switch (set_alarm){
      case 1:
        alarm1.alarm_set(alarmTune.hour, alarmTune.minute, alarmTune.sound, alarmTune.status);
        alarm1.putEEPROM(ALARM1_HOUR_ADDR, ALARM1_MIN_ADDR, ALARM1_SOUND_ADDR, ALARM1_STATUS_ADDR);
        break;
      case 2:
        alarm2.alarm_set(alarmTune.hour, alarmTune.minute, alarmTune.sound, alarmTune.status);
        alarm2.putEEPROM(ALARM2_HOUR_ADDR, ALARM2_MIN_ADDR, ALARM2_SOUND_ADDR, ALARM2_STATUS_ADDR);
        break;
      case 3:
        alarm3.alarm_set(alarmTune.hour, alarmTune.minute, alarmTune.sound, alarmTune.status);
        alarm3.putEEPROM(ALARM3_HOUR_ADDR, ALARM3_MIN_ADDR, ALARM3_SOUND_ADDR, ALARM3_STATUS_ADDR);
        break;
      default:
        break;
    }
    ++set_alarm;
    if (set_alarm > 3) set_alarm = 1;
    lcd.clear();
    change_flag = true;
  }
  
  if(enc.isSingle()){
    ++set_param;
    if(set_param > 4) set_param = 1;
    backToMain.reset();
    change_flag = true;
  }

  if(enc.isRightH()){
    ++set_alarm;
    if (set_alarm > 3) set_alarm = 1;
    backToMain.reset();
    change_flag = true;
  }
  
  if (enc.isLeftH()){
    --set_alarm;
    if (set_alarm < 1) set_alarm = 3;
    backToMain.reset();
    change_flag = true;
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
    change_flag = true;
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
    change_flag = true;
  }

  //--------------------------- print alarmTune -------------------------------------------  
  
  if(change_flag){
    // lcd.clear();
    drawDown_param.start();
    drawDown_param.reset();
    drawUp_param.stop();
    drawAlarmClock(alarmTune.hour, alarmTune.minute, 0, 0, true);
  }

  
  if(drawDown_param.isReady()) {
    drawDown_param.stop();
    draw_param = false;
    drawUp_param.start();
    drawUp_param.reset();
    drawAlarmClock(alarmTune.hour, alarmTune.minute, 0, 0, draw_param);
  }

  if(drawUp_param.isReady()) {
    drawUp_param.stop();
    draw_param = true;
    drawDown_param.start();
    drawDown_param.reset();
    drawAlarmClock(alarmTune.hour, alarmTune.minute, 0, 0, draw_param);
  }

  if(backToMain.isReady() || button.isHolded()){
    mode = 0;
    firstStartFlag = false;
    lcd.clear();
    reload_ch_flg();
    loadClock();
    drawClock(hrs, mins, 0, 0, 1);
    drawData();
    drawSensors();
    drawFlags();
    backToMain.stop();
  }
}

void alarmStart(uint8_t set){
  switch(set) {
    case 1:
      play_sound(alarm1.wakeUP_Sound());
      break;
    case 2:
      play_sound(alarm2.wakeUP_Sound());
      break;
    case 3:
      play_sound(alarm3.wakeUP_Sound());
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
    if (alarm1.isRunning()) {
      alarm1.stop();
      play_sound(0);
    }
    if (alarm2.isRunning()) {
      alarm2.stop();
      play_sound(0);
    }
    if (alarm3.isRunning()) {
      alarm3.stop();
      play_sound(0);
    }
}

void drawFlags(){

  if(ch_flg_alarm){
    lcd.setCursor(8, 3);
    lcd.print("          ");
    lcd.setCursor(8, 3);
    lcd.print("Alarm:");
    if(alarm1.get_wakeStatus() || alarm2.get_wakeStatus() || alarm3.get_wakeStatus()){
      lcd.setCursor(14, 3); lcd.print("ON");
    }
      else { lcd.setCursor(14, 3); lcd.print("OFF"); }
    ch_flg_alarm = false;
  }

  if(ch_flg_power){
    lcd.setCursor(18, 3);
    lcd.print("  ");
    lcd.setCursor(18, 3);
    if(low_power){
      lcd.setCursor(19, 3); lcd.print("!");
    }
      else { lcd.setCursor(19, 3); lcd.print(" "); }
    ch_flg_power = false;
  }  
}

void debug_start(){
    #if (DEBUG == 1 && DISPLAY_TYPE == 1)
        Serial.begin(9600);
        boolean status = true;
        lcd.clear();

        setLED(1);

    #if (CO2_SENSOR == 1)
        lcd.setCursor(0, 0);
        lcd.print(F("MHZ-19... "));
        Serial.print(F("MHZ-19... "));
        mhz19.begin(MHZ_TX, MHZ_RX);
        mhz19.setAutoCalibration(false);
        mhz19.getStatus();    // первый запрос, в любом случае возвращает -1
        delay(500);
        if (mhz19.getStatus() == 0) {
            lcd.print(F("OK"));
            Serial.println(F("OK"));
        } else {
            lcd.print(F("ERROR"));
            Serial.println(F("ERROR"));
            status = false;
        }
    #endif

    setLED(2);
    lcd.setCursor(0, 1);
    lcd.print(F("RTC... "));
    Serial.print(F("RTC... "));
    delay(50);
    if(rtc.begin()) {
        lcd.print(F("OK"));
        Serial.println(F("OK"));
    } else {
        lcd.print(F("ERROR"));
        Serial.println(F("ERROR"));
        status = false;
    }

    setLED(3);
    lcd.setCursor(0, 2);
    lcd.print(F("BME280... "));
    Serial.print(F("BME280... "));
    delay(50);
    if(bme.begin()) {
        lcd.print(F("OK"));
        Serial.println(F("OK"));
    } else {
        lcd.print(F("ERROR"));
        Serial.println(F("ERROR"));
        status = false;
    }

    setLED(0);
    lcd.setCursor(0, 3);
    if(status) {
        lcd.print(F("All good"));
        Serial.println(F("All good"));
    } else {
        lcd.print(F("Check wires!"));
        Serial.println(F("Check wires!"));
    }
    while(1) {
        lcd.setCursor(14, 1);
        lcd.print("P:    ");
        lcd.setCursor(16, 1);
        lcd.print(analogRead(PHOTO), 1);
        Serial.println(analogRead(PHOTO));
        delay(300);
    }
#else

    // #if (CO2_SENSOR == 1)
    //     mhz19.begin(MHZ_TX, MHZ_RX);
    //     mhz19.setAutoCalibration(false);
    // #endif

    // rtc.begin();
    // bme.begin();
    // bme.setTempCal(-1);
    /*bme.setSampling(Adafruit_BME280::MODE_FORCED,
        Adafruit_BME280::SAMPLING_X1, // temperature
        Adafruit_BME280::SAMPLING_X1, // pressure
        Adafruit_BME280::SAMPLING_X1, // humidity
        Adafruit_BME280::FILTER_OFF   );*/
#endif
// if (RESET_CLOCK || rtc.lostPower()) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

}

void EEPROM_init(){
    uint8_t key = EEPROM.read(EEPROM_KEY_ADDR);

    if(key != EEPROM_KEY){
        EEPROM.write(EEPROM_KEY_ADDR, EEPROM_KEY);
        alarm1.putEEPROM(ALARM1_HOUR_ADDR, ALARM1_MIN_ADDR, ALARM1_SOUND_ADDR, ALARM1_STATUS_ADDR);
        alarm2.putEEPROM(ALARM2_HOUR_ADDR, ALARM2_MIN_ADDR, ALARM2_SOUND_ADDR, ALARM2_STATUS_ADDR);
        alarm3.putEEPROM(ALARM3_HOUR_ADDR, ALARM3_MIN_ADDR, ALARM3_SOUND_ADDR, ALARM3_STATUS_ADDR);
        opt_status = OPTION_DT;
        option N_B {1, bitRead(opt_status, 0), bitRead(opt_status, 0), 1, optNames[0]};
        option A_OFF {2, bitRead(opt_status, 1), bitRead(opt_status, 1), 2, optNames[1]};
        option A_RST {3, bitRead(opt_status, 2), bitRead(opt_status, 2), 3, optNames[2]};
        option D_brs_day {4, DIS_BRS_DAY_DT, DIS_BRS_DAY_DT, 4, optNames[3]};
        option D_brs_night {5, DIS_BRS_NIGHT_DT, DIS_BRS_NIGHT_DT, 5, optNames[4]};
        option LED_brs_day {6, RGB_BRS_DAY_DT, RGB_BRS_DAY_DT, 6, optNames[5]};
        option LED_brs_night {7, RGB_BRS_NIGHT_DT, RGB_BRS_NIGHT_DT, 7, optNames[6]};
        option Display_mode {8, bitRead(opt_status, 5), bitRead(opt_status, 5), 8, optNames[7]};
        option vol {9, VOLUME_DT, VOLUME_DT, 9, optNames[8]};
        option debug {10, bitRead(opt_status, 3), bitRead(opt_status, 3), 10, optNames[9]};
        option up {11, false, false, 11, optNames[10]};
        opt_eeprom_upd();
    }
      else{
        opt_eeprom_dwl();
        alarm1.getEEPROM(ALARM1_HOUR_ADDR, ALARM1_MIN_ADDR, ALARM1_SOUND_ADDR, ALARM1_STATUS_ADDR);
        alarm2.getEEPROM(ALARM2_HOUR_ADDR, ALARM2_MIN_ADDR, ALARM2_SOUND_ADDR, ALARM2_STATUS_ADDR);
        alarm3.getEEPROM(ALARM3_HOUR_ADDR, ALARM3_MIN_ADDR, ALARM3_SOUND_ADDR, ALARM3_STATUS_ADDR);
      }
}

void power_control(){
  if(true){
    low_power = true;
    ch_flg_power = true;
  }
    else {
      low_power = false;
      ch_flg_power = true;
    }
}

void reload_ch_flg(){
  ch_flg_hr1 = true;
  ch_flg_hr2 = true;
  ch_flg_min1 = true;
  ch_flg_min2 = true;
  ch_flg_day = true;
  ch_flg_month = true;
  ch_flg_week = true;
  ch_flg_temp = true;
  ch_flg_hum = true;
  ch_flg_co2 = true;
  ch_flg_press = true;
  ch_flg_alarm = true;
  ch_flg_power = true;
}

void options(){
  if(!firstStartFlag){
    lcd.clear();
    backToMain.start();
    backToMain.reset();
    opt_up();
    lst_opt.s1_c1 = 0;
    lst_opt.s2_c1 = 0;
    lst_opt.s3_c1 = 0;
    lst_opt.s4_c1 = 0;
    opt_prt(opt);
    change_flag = false;
    firstStartFlag = true;
  }
  

  if (cursor_get_pos()){
    opt_prt(opt);
    change_flag = false;
  }

  if((cursor_pos < 100 && button.isHolded()) || backToMain.isReady()){
    mode = 0;
    firstStartFlag = false;
    change_flag = false;
    lcd.clear();
    reload_ch_flg();
    loadClock();
    drawClock(hrs, mins, 0, 0, 1);
    drawData();
    drawSensors();
    drawFlags();
    backToMain.stop();
  }



}

void opt_upd(){
  switch(cursor_pos){
  case 101:
    N_B.param = N_B.d_param;
    N_B.param ? bitSet(opt_status, 0) : bitClear(opt_status, 0);
    break;
  case 104:
    D_brs_day.param = D_brs_day.d_param;
    EEPROM.update(DIS_BRS_DAY_ADDR, D_brs_day.param);
    break;
  case 105:
    D_brs_night.param = D_brs_night.d_param;
    EEPROM.update(DIS_BRS_NIGHT_ADDR, D_brs_night.param);
    break;
  case 106:
    LED_brs_day.param = LED_brs_day.d_param;
    EEPROM.update(RGB_BRS_DAY_ADDR, LED_brs_day.param);
    break;
  case 107:
    LED_brs_night.param = LED_brs_night.d_param;
    EEPROM.update(RGB_BRS_NIGHT_ADDR, LED_brs_night.param);
    break;
  case 108:
    Display_mode.param = Display_mode.d_param;
    Display_mode.param ? bitSet(opt_status, 8) : bitClear(opt_status, 8);
    break;
  case 109:
    vol.param = vol.d_param;
    EEPROM.update(VOLUME_ADDR, vol.param);
    break;
  default:
    break;
  }
}

void opt_change(bool dir){
  switch(cursor_pos){
  case 101:
    N_B.d_param = !N_B.d_param;
    break;
  case 104:
    if(dir) D_brs_day.d_param = ++D_brs_day.d_param;
      else {D_brs_day.d_param = --D_brs_day.d_param;}
    D_brs_day.d_param = constrain(D_brs_day.d_param, 0, 100);
    break;
  case 105:
    if(dir) D_brs_night.d_param = ++D_brs_night.d_param;
      else {D_brs_night.d_param = --D_brs_night.d_param;}
    D_brs_night.d_param = constrain(D_brs_night.d_param, 0, 100);
    break;
  case 106:
    if(dir) LED_brs_day.d_param = ++LED_brs_day.d_param;
      else {LED_brs_day.d_param = --LED_brs_day.d_param;}
    LED_brs_day.d_param = constrain(LED_brs_day.d_param, 0, 100);
    break;
  case 107:
    if(dir) LED_brs_night.d_param = ++LED_brs_night.d_param;
      else {LED_brs_night.d_param = --LED_brs_night.d_param;}
    LED_brs_night.d_param = constrain(LED_brs_night.d_param, 0, 100);
    break;
  case 108:
    Display_mode.d_param = !Display_mode.d_param;
    break;
  case 109:
    if(dir) vol.d_param = ++vol.d_param;
      else {vol.d_param = --vol.d_param;}
    vol.d_param = constrain(vol.d_param, 0, 100);
    break;
  default:
    break;
  }
}

void opt_eeprom_upd(){
  EEPROM.update(OPTION_STATUS_ADDR, opt_status);
  EEPROM.update(DIS_BRS_DAY_ADDR, D_brs_day.param);
  EEPROM.update(DIS_BRS_NIGHT_ADDR, D_brs_night.param);
  EEPROM.update(RGB_BRS_DAY_ADDR, LED_brs_day.param);
  EEPROM.update(RGB_BRS_NIGHT_ADDR, LED_brs_night.param);
  EEPROM.update(VOLUME_ADDR, vol.param);
}

void opt_eeprom_dwl(){
  EEPROM.get(OPTION_STATUS_ADDR, opt_status);
  option N_B {1, bitRead(opt_status, 0), bitRead(opt_status, 0), 1, optNames[0]};
  option A_OFF {2, bitRead(opt_status, 1), bitRead(opt_status, 1), 2, optNames[1]};
  option A_RST {3, bitRead(opt_status, 2), bitRead(opt_status, 2), 3, optNames[2]};
  option D_brs_day {4, EEPROM.read(DIS_BRS_DAY_ADDR), EEPROM.read(DIS_BRS_DAY_ADDR), 4, optNames[3]};
  option D_brs_night {5, EEPROM.read(DIS_BRS_NIGHT_ADDR), EEPROM.read(DIS_BRS_NIGHT_ADDR), 5, optNames[4]};
  option LED_brs_day {6, EEPROM.read(RGB_BRS_DAY_ADDR), EEPROM.read(RGB_BRS_DAY_ADDR), 6, optNames[5]};
  option LED_brs_night {7, EEPROM.read(RGB_BRS_NIGHT_ADDR), EEPROM.read(RGB_BRS_NIGHT_ADDR), 7, optNames[6]};
  option Display_mode {8, bitRead(opt_status, 5), bitRead(opt_status, 5), 8, optNames[7]};
  option vol {9, EEPROM.read(VOLUME_ADDR), EEPROM.read(VOLUME_ADDR), 9, optNames[8]};
  option debug {10, bitRead(opt_status, 3), bitRead(opt_status, 3), 10, optNames[9]};
  option up {11, false, false, 11, optNames[10]};
}

bool cursor_get_pos(){
  if(cursor_pos > 100){
    if((cursor_pos >= 104 && cursor_pos <= 109) || cursor_pos == 101){
      if(enc.isRight()){
        opt_change(true);
        return true;
      }

      if(enc.isLeft()){
        opt_change(false);
        return true;
      }

      if(enc.isDouble()){
        opt_upd();
        change_flag = true;
        return true;
      }
    }

    if(enc.isClick()){
      cursor_pos = cursor_pos - 100;
      return true;
    }

    if(cursor_pos == 102, 103, 110){
      if(enc.isHolded()){
        switch (cursor_pos){
          case 102:
            alarm_OFF();
            change_flag = true;
            break;
          case 103:
            alarm_RST();
            change_flag = true;
            break;
          case 110:
            go_debug();
            break;
          default:
            break;
        }
        return true;
      }
    }

  }
    else{
      if(enc.isRight()){
        ++cursor_pos;
        cursor_pos = constrain(cursor_pos, 1, 11);
        return true;
      }

      if(enc.isLeft()){
        --cursor_pos;
        cursor_pos = constrain(cursor_pos, 1, 11);
        return true;
      }

      if(cursor_pos >= 1 && cursor_pos <= 4 && opt.s1_c1 != opt_fnd(1)){
        opt.s1_c1 = opt_fnd(1); opt.s1_c2 = opt_fnd(101);
        opt.s2_c1 = opt_fnd(2); opt.s2_c2 = opt_fnd(102);
        opt.s3_c1 = opt_fnd(3); opt.s3_c2 = opt_fnd(103);
        opt.s4_c1 = opt_fnd(4); opt.s4_c2 = opt_fnd(104);
        change_flag = true;
      }
      else if(cursor_pos >= 5 && cursor_pos <= 8 && opt.s1_c1 != opt_fnd(5)){
        opt.s1_c1 = opt_fnd(5); opt.s1_c2 = opt_fnd(105);
        opt.s2_c1 = opt_fnd(6); opt.s2_c2 = opt_fnd(106);
        opt.s3_c1 = opt_fnd(7); opt.s3_c2 = opt_fnd(107);
        opt.s4_c1 = opt_fnd(8); opt.s4_c2 = opt_fnd(108);
        change_flag = true;
      }
      else if(cursor_pos >= 9 && cursor_pos <= 11 && opt.s1_c1 != opt_fnd(9)){
        opt.s1_c1 = opt_fnd(9);   opt.s1_c2 = opt_fnd(109);
        opt.s2_c1 = opt_fnd(10);  opt.s2_c2 = opt_fnd(110);
        opt.s3_c1 = opt_fnd(11);  opt.s3_c2 = opt_fnd(111);
        opt.s4_c1 = opt_fnd(12);  opt.s4_c2 = opt_fnd(112);
        change_flag = true;
      }


      if(enc.isClick()){
        if(cursor_pos == 11){
          opt_up();
          return true;
        }
          else{
            cursor_pos = cursor_pos + 100;
            return true;
          }
      }

    }
  return false;
}

void opt_up(){
  opt.s1_c1 = opt_fnd(1); opt.s1_c2 = opt_fnd(101);
  opt.s2_c1 = opt_fnd(2); opt.s2_c2 = opt_fnd(102);
  opt.s3_c1 = opt_fnd(3); opt.s3_c2 = opt_fnd(103);
  opt.s4_c1 = opt_fnd(4); opt.s4_c2 = opt_fnd(104);
}

void opt_prt(struct print){
  for(uint8_t y = 0; y < 4; ++y){
    switch (y){
      case 0:
        if(lst_opt.s1_c1 != opt.s1_c1){
          lst_opt.s1_c1 = opt.s1_c1;
          lcd.setCursor(1, y); lcd.print("              "); // 14 slots
          lcd.setCursor(1, y);
          switch (opt.s1_c1){
            case 1:
              lcd.print(N_B.name);
              break;
            case 5:
              lcd.print(D_brs_night.name);
              break;
            case 9:
              lcd.print(vol.name);
              break;
            default:
              lcd.print("Err "); lcd.print(opt.s1_c1);
              break;
          }
        }
        if(lst_opt.s1_c2 != opt.s1_c2 || lst_opt.s1_c1 == 0){
          lst_opt.s1_c2 = opt.s1_c2;
          lcd.setCursor(14, y); lcd.print("     "); // 5 slots
          lcd.setCursor(14, y);
          switch (opt.s1_c1){
            case 1:
              if(opt.s1_c2 == 0){
                lcd.print("OFF");
              }
                else{
                  lcd.print("ON");
                }
              break;
            case 5:
              lcd.print(D_brs_night.d_param);
              break;
            case 9:
              lcd.print(vol.d_param);
              break;
          default:
            lcd.print("error");
            break;
          }
        }
        break;
      case 1:
        if(lst_opt.s2_c1 != opt.s2_c1){
          lst_opt.s2_c1 = opt.s2_c1;
          lcd.setCursor(1, y); lcd.print("              ");
          lcd.setCursor(1, y);
          switch (opt.s2_c1){
            case 2:
              lcd.print(A_OFF.name);
              break;
            case 6:
              lcd.print(LED_brs_day.name);
              break;
            case 10:
              lcd.print(debug.name);
              break;
            default:
              lcd.print("Err "); lcd.print(opt.s2_c1);
              break;
          }
        }
        if(lst_opt.s2_c2 != opt.s2_c2 || lst_opt.s2_c1 == 0){
          lst_opt.s2_c2 = opt.s2_c2;
          lcd.setCursor(14, y); lcd.print("     "); // 5 slots
          lcd.setCursor(14, y);
          switch (opt.s2_c1){
            case 2:
              if(opt.s2_c2 == 0){
                lcd.print("hold");
              }
                else{
                  lcd.print("wait");
                }
              break;
            case 6:
              lcd.print(LED_brs_day.d_param);
              break;
            case 10:
              if(opt.s2_c2 == 0){
                lcd.print("hold");
              }
                else{
                  lcd.print("wait");
                }
              break;
          default:
            lcd.print("error");
            break;
          }
        }
        break;
      case 2:
        if(lst_opt.s3_c1 != opt.s3_c1){
          lst_opt.s3_c1 = opt.s3_c1;
          lcd.setCursor(1, y); lcd.print("              ");
          lcd.setCursor(1, y);
          switch (opt.s3_c1){
            case 3:
              lcd.print(A_RST.name);
              break;
            case 7:
              lcd.print(LED_brs_night.name);
              break;
            case 11:
              lcd.print(up.name);
              break;
            default:
              lcd.print("error");
              break;
          }
        }
        if(lst_opt.s3_c2 != opt.s3_c2 || lst_opt.s3_c1 == 0){
          lst_opt.s3_c2 = opt.s3_c2;
          lcd.setCursor(14, y); lcd.print("     "); // 5 slots
          lcd.setCursor(14, y);
          switch (opt.s3_c1){
            case 3:
              if(opt.s3_c2 == 0){
                lcd.print("hold");
              }
                else{
                  lcd.print("wait");
                }
              break;
            case 7:
              lcd.print(LED_brs_night.d_param);
              break;
            case 11:
              lcd.print("click");
              break;
          default:
            lcd.print("error");
            break;
          }
        }
        break;
      case 3:
        if(lst_opt.s4_c1 != opt.s4_c1){
          lst_opt.s4_c1 = opt.s4_c1;
          lcd.setCursor(1, y); lcd.print("              ");
          lcd.setCursor(1, y);
          switch (opt.s4_c1){
            case 4:
              lcd.print(D_brs_day.name);
              break;
            case 8:
              lcd.print(Display_mode.name);
              break;
            case 12:
              lcd.print("              ");
              break;
            default:
              break;
          }
        }
        if(lst_opt.s2_c2 != opt.s2_c2 || lst_opt.s4_c1 == 0){
          lst_opt.s2_c2 = opt.s2_c2;
          lcd.setCursor(14, y); lcd.print("     "); // 5 slots
          lcd.setCursor(14, y);
          switch (opt.s2_c1){
            case 4:
              lcd.print(D_brs_day.d_param);
              break;
            case 8:
              if(opt.s3_c2 == 0){
                lcd.print("blink");
              }
                else{
                  lcd.print("slow");
                }
              break;
            case 12:
              lcd.print("     ");
              break;
            default:
              lcd.print("error");
              break;
          }
        }
        break;
      default:
        break;
    } 
  }
}

uint8_t opt_fnd(uint8_t pos){
  switch(pos){
    case 1:
      return N_B.id;
    case 2:
      return A_OFF.id;
    case 3:
      return A_RST.id;
    case 4:
      return D_brs_day.id;
    case 5:
      return D_brs_night.id;
    case 6:
      return LED_brs_day.id;
    case 7:
      return LED_brs_night.id;
    case 8:
      return Display_mode.id;
    case 9:
      return vol.id;
    case 10:
      return debug.id;
    case 11:
      return up.id;
    default:
      break;
  }
  switch(pos){
    case 101:
      return N_B.d_param;
    case 102:
      return A_OFF.d_param;
    case 103:
      return A_RST.d_param;
    case 104:
      return D_brs_day.d_param; 
    case 105:
      return D_brs_night.d_param;
    case 106:
      return LED_brs_day.d_param;
    case 107:
      return LED_brs_night.d_param;
    case 108:
      return Display_mode.d_param;
    case 109:
      return vol.d_param;
    case 110:
      return debug.d_param;
    case 111:
      return up.d_param;
    default:
      break;
  }
  return 0;
}

void alarm_OFF(){

}

void alarm_RST(){

}

void go_debug(){

}