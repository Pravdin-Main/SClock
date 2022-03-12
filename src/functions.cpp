#include "functions.h"

static uint8_t set_param;
static uint8_t mode = 0;
// static bool change_flag;

static int8_t hrs;
static int8_t mins;
static int8_t secs;
static byte ch_flg_clc; //  0 - first symbol of hours, 
                        //  1 - second symbol of hours
                        //  2 - first symbol of minuts
                        //  3 - second symbol of minuts
                        //  4 - day of month
                        //  5 - month
                        //  6 - day of week

#if (ALARM >= 1)
  static byte ch_flg_alarm; //  0 - enc is turned or pushed
                            //  1 - flag of alarm status "ON"/"OFF"
                            //  2 - flag of updating alarm disp
                            //  3 - 
                            //  
                            //  
                            //  
                            //  

#endif                        
const static String off_msg = "OFF";
const static String on_msg = "ON";
RTC_DS3231 rtc;
DateTime now;

#if (POWER_IND == 1)
  static bool ch_flg_power = true;
  static bool power_state;
  static bool cach_power_state;
#elif (POWER_IND == 2)
  static bool ch_flg_power = true;
  static uint8_t cach_power_state;
  static uint8_t power_level_cur;
#endif

bool dotFlag;
bool firstStartFlag = false;

#if (OPTION == 1)
  option N_B, A_OFF, A_RST, D_brs_day, D_brs_night, LED_brs_day, LED_brs_night, Display_mode, time_date, vol, debug, up;
  static byte opt_status;   // 0 - N_B, 1 - A_OFF, 2 - A_RST, 3 - debug; 4 - display_mode
  static uint8_t cursor_pos = 1;
  print opt,lst_opt;
  time_set t_d_set;
  const static String error_msg = "error";
  const static String hold_msg = "hold";
  const static String wait_msg = "wait";
  const static String click_msg = "click";
  const static String blink_msg = "blink";
  const static String slow_msg = "pulse";
#endif

#if (SENSORS == 1)
  #if (SENS_TEMP == 1)
    float dispTemp;
    static bool ch_flg_temp = true;
  #endif
  #if (SENS_HUM == 1)
    uint8_t dispHum;
    static bool ch_flg_hum = true;
  #endif
  #if (SENS_PRESS == 1)
    uint16_t dispPres;
    static bool ch_flg_press = true;
  #endif
  #if (SENS_CO2 == 1)
    uint16_t dispCO2;
    static bool ch_flg_co2 = true;
    MHZ19_uart mhz19;
  #endif
#endif

#if (DISPLAY_TYPE == 1)
  LiquidCrystal_I2C lcd(DISPLAY_ADDR, 20, 4);
#else
  LiquidCrystal_I2C lcd(DISPLAY_ADDR, 16, 2); 
#endif

Encoder enc(CLK, DT, SW, TYPE2);
BME280_I2C bme(0x76);
GButton button(BTN_PIN, LOW_PULL, NORM_OPEN);

GTimer_ms drawDown_param(1500);
GTimer_ms drawUp_param(400);
GTimer_ms backToMain(20000);
GTimer_ms brs_Ref(ZOOM_LED);
GTimer_ms disp_pulse(50);
GTimer_ms disp_blink(1000);

// цифры
uint8_t LT[8] = {0b00111,  0b01111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
uint8_t UB[8] = {0b11111,  0b11111,  0b11111,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000};
uint8_t RT[8] = {0b11100,  0b11110,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
uint8_t LL[8] = {0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b01111,  0b00111};
uint8_t LB[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};
uint8_t LR[8] = {0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11110,  0b11100};
uint8_t UMB[8] = {0b11111,  0b11111,  0b11111,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111};
uint8_t LMB[8] = {0b11111,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};

#if (ALARM >= 1)
  #if (ALARM == 1)
    alarm alarm1, alarmTune;
  #elif (ALARM == 3)
    uint8_t set_alarm;
    alarm alarm1, alarm2, alarm3, alarmTune;
  #endif
#endif


void display_init(){
  lcd.init();
  lcd.backlight();
  lcd.clear();
  loadClock();
  backToMain.stop();
}

void get_time() {
  now = rtc.now();
  secs = now.second();
  mins = now.minute();
  hrs = now.hour();
}

void draw_main_disp(){
  #if (DISPLAY_TYPE == 1)
    get_time();
    #if (CLOCK == 1)
      // loadClock();
      lcd.clear();
      reload_ch_flg();
      drawClock(hrs, mins, 0, 0);
      drawData();
      #if (SENSORS == 1)
        drawSensors();
      #endif
      drawFlags();
      backToMain.stop();
    #elif (CLOCK == 2)
    #elif (CLOCK == 3)
    #elif (CLOCK == 4)
    #endif

  #endif
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

bool BTN_IsHolded(){
  return button.isHold();
}

uint8_t Mode(uint8_t x){
  if(x != 100) mode = x;
  return mode;
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

void drawDots(uint8_t x, uint8_t y, bool state) {
  uint8_t code;
  if (state) code = 165;
  else code = 32;
  lcd.setCursor(x, y);
  lcd.write(code);
  lcd.setCursor(x, y + 1);
  lcd.write(code);
}

void drawClock(uint8_t hours, uint8_t minutes, uint8_t x, uint8_t y) {
  if(bitRead(ch_flg_clc, 0)){
    space_prt(3, x, y);
    space_prt(3, x, y + 1);
    drawDig(hours / 10, x, y);
    // if (hours / 10 == 0) drawDig(hours / 10, x, y);
    //   else drawDig(hours / 10, x, y);
    // ch_flg_hr1 = false;
    bitClear(ch_flg_clc, 0);
  }
  
  if(bitRead(ch_flg_clc, 1)){
    space_prt(3, x + 4, y);
    space_prt(3, x + 4, y + 1);
    drawDig(hours % 10, x + 4, y);
    // ch_flg_hr2 = false;
    bitClear(ch_flg_clc, 1);
  }

  if(bitRead(ch_flg_clc, 2)){
    space_prt(3, x + 8, y);
    space_prt(3, x + 8, y + 1);
    drawDig(minutes / 10, x + 8, y);
    // ch_flg_min1 = false;
    bitClear(ch_flg_clc, 2);
  }

  if(bitRead(ch_flg_clc, 3)){
    space_prt(3, x + 12, y);
    space_prt(3, x + 12, y + 1);
    drawDig(minutes % 10, x + 12, y);
    // ch_flg_min2 = false;
    bitClear(ch_flg_clc, 3);
  }
}

void drawData() {
  if(bitRead(ch_flg_clc, 4)){
    space_prt(3, 15, 0);
    if (now.day() < 10) lcd.print(0);
    lcd.print(now.day());
    lcd.print(".");
    // ch_flg_day = false;
    bitClear(ch_flg_clc, 4);
  }
  
  if(bitRead(ch_flg_clc, 5)){
    space_prt(2, 18, 0);
    if (now.month() < 10) lcd.print(0);
    lcd.print(now.month());
    // ch_flg_month = false;
    bitClear(ch_flg_clc, 5);
  }
  

  if(bitRead(ch_flg_clc, 6)){
    space_prt(4, 16, 1);
    if (DISP_MODE == 0) {
      lcd.print(now.year());
    }
      else if (DISP_MODE == 1) {
        int dayofweek = now.dayOfTheWeek();
        lcd.print(dayNames[dayofweek]);
      }
    // ch_flg_week = false;
    bitClear(ch_flg_clc, 6);
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

void setLED(uint8_t color, uint8_t day, uint8_t night) {
    #if(OPTION)
      switch (color) {    // 0 OFF, 1 RED, 2 GREEN, 3 BLUE (or YELLOW)
        case 0:
          analogWrite(LED_R, OFF_LED);
          analogWrite(LED_G, OFF_LED);
          analogWrite(LED_B, OFF_LED);
          break;
        case 1:
          (analogRead(PHOTO) < BRIGHT_THRESHOLD) ? analogWrite(LED_R, night) : analogWrite(LED_R, day);
          analogWrite(LED_G, OFF_LED);
          analogWrite(LED_B, OFF_LED);
          break;
        case 2:
          (analogRead(PHOTO) < BRIGHT_THRESHOLD && N_B.param) ? analogWrite(LED_G, night) : analogWrite(LED_G, day);
          analogWrite(LED_R, OFF_LED);
          analogWrite(LED_B, OFF_LED);
          break;
        case 3:
          if(analogRead(PHOTO) < BRIGHT_THRESHOLD && N_B.param) { 
            #if (!BLUE_YELLOW)
              analogWrite(LED_B, night);
              analogWrite(LED_R, OFF_LED);
              analogWrite(LED_G, OFF_LED);
            #else 
                analogWrite(LED_R, night);
                analogWrite(LED_G, night);
            #endif
          }
            else{
              #if (!BLUE_YELLOW)
                analogWrite(LED_B, day);
                analogWrite(LED_R, OFF_LED);
                analogWrite(LED_G, OFF_LED);
              #else 
                  analogWrite(LED_R, day);
                  analogWrite(LED_G, day);
              #endif
            }
          break;
        default:
          break;
      }
      #else
        switch (color) {    // 0 OFF, 1 RED, 2 GREEN, 3 BLUE (or YELLOW)
          case 0:
            analogWrite(LED_R, OFF_LED);
            analogWrite(LED_G, OFF_LED);
            analogWrite(LED_B, OFF_LED);
            break;
          case 1:
            (analogRead(PHOTO) < BRIGHT_THRESHOLD) ? analogWrite(LED_R, night) : analogWrite(LED_R, day);
            analogWrite(LED_G, OFF_LED);
            analogWrite(LED_B, OFF_LED);
            break;
          case 2:
            (analogRead(PHOTO) < BRIGHT_THRESHOLD) ? analogWrite(LED_G, night) : analogWrite(LED_G, day);
            analogWrite(LED_R, OFF_LED);
            analogWrite(LED_B, OFF_LED);
            break;
          case 3:
            if(analogRead(PHOTO) < BRIGHT_THRESHOLD) { 
              #if (!BLUE_YELLOW)
                analogWrite(LED_B, night);
                analogWrite(LED_R, OFF_LED);
                analogWrite(LED_G, OFF_LED);
              #else 
                analogWrite(LED_R, night);
                analogWrite(LED_G, night);
              #endif
            }
              else{
                #if (!BLUE_YELLOW)
                  analogWrite(LED_B, day);
                  analogWrite(LED_R, OFF_LED);
                  analogWrite(LED_G, OFF_LED);
                #else 
                  analogWrite(LED_R, day);
                  analogWrite(LED_G, day);
                #endif
              }
            break;
          default:
            break;
        }
      #endif
}

void brightnessControl(){
  checkInput();
  if(enc.isRight()){
    #if (OPTION)
      if(analogRead(PHOTO) < BRIGHT_THRESHOLD && N_B.param){
        (D_brs_night.param >= 245) ? D_brs_night.param = 255 : D_brs_night.param += LCD_BRIGHT_STEP;
        (LED_brs_night.param >= 245) ? LED_brs_night.param = 255 : LED_brs_night.param += LCD_BRIGHT_STEP;
      }
        else{
          (D_brs_day.param >= 245) ? D_brs_day.param = 255 : D_brs_day.param += LCD_BRIGHT_STEP;
          (LED_brs_day.param >= 245) ? LED_brs_day.param = 255 : LED_brs_day.param += LCD_BRIGHT_STEP;
        }
      brightnessRef(D_brs_day.param, D_brs_night.param, LED_brs_day.param, LED_brs_night.param);
    #endif
  }

  if(enc.isLeft()){
    #if (OPTION)
      if(analogRead(PHOTO) < BRIGHT_THRESHOLD && N_B.param){
        (D_brs_night.param <= 10) ? D_brs_night.param = 0 : D_brs_night.param -= LCD_BRIGHT_STEP;
        (LED_brs_night.param <= 10) ? LED_brs_night.param = 0 : LED_brs_night.param -= LCD_BRIGHT_STEP;
      }
        else{
          (D_brs_day.param <= 10) ? D_brs_day.param = 0 : D_brs_day.param -= LCD_BRIGHT_STEP;
          (LED_brs_day.param <= 10) ? LED_brs_day.param = 0 : LED_brs_day.param -= LCD_BRIGHT_STEP;
        }
      brightnessRef(D_brs_day.param, D_brs_night.param, LED_brs_day.param, LED_brs_night.param);
    #endif
  }
  
  // static uint8_t LCD_brs_night_cache;
  // static uint8_t LCD_brs_day_cache;

  if(button.isClick()){
    brs_Ref.reset();
    
    #if (OPTION)
      if(analogRead(PHOTO) < BRIGHT_THRESHOLD && N_B.param){
        brightnessRef(D_brs_day.param, LCD_BRIGHT_MAX, LED_brs_day.param, LED_brs_night.param);
      }
        else{
          brightnessRef(LCD_BRIGHT_MAX, D_brs_night.param, LED_brs_day.param, LED_brs_night.param);
        }
    #endif
  }

  if(brs_Ref.isReady()){
    #if (OPTION)
      brightnessRef(D_brs_day.param, D_brs_night.param, LED_brs_day.param, LED_brs_night.param);
    #endif
  }
}

#if (OPTION)
void brightnessRef(uint8_t dbd, uint8_t dbn, uint8_t lbd, uint8_t lbn) {
  if (analogRead(PHOTO) < BRIGHT_THRESHOLD && N_B.param) {   // if dark and Night Mode is ON
    analogWrite(BACKLIGHT, dbn);
  }
   else {                                      // if lighty
    analogWrite(BACKLIGHT, dbd);
   }
  #if (SENSORS == 1 && SENS_CO2 == 1)
    if (dispCO2 < CO2_GREEN_THRESHOLD) setLED(2, lbd, lbn);
    else if (dispCO2 < CO2_YELLOW_THRESHOLD) setLED(3, lbd, lbn);
    else if (dispCO2 >= CO2_YELLOW_THRESHOLD) setLED(1, lbd, lbn);
  #endif
}
#endif

void checkInput(){
  button.tick();
  enc.tick();
}

//------------------------- SENSORS MODUL ---------------------------------
#if (SENSORS == 1)
void init_sens(){
  #if (SENS_CO2 == 1)
    mhz19.begin(MHZ_TX, MHZ_RX);
    mhz19.setAutoCalibration(false);
  #endif
  rtc.begin();
  bme.begin();
  bme.setTempCal(-1);
  if (RESET_CLOCK || rtc.lostPower()) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  readSensors();
  drawSensors();
  #if (EEPROM_P == 0)
    N_B.id = 1; N_B.param = bitRead(opt_status, 0); N_B.d_param = bitRead(opt_status, 0);
    A_OFF.id = 2; A_OFF.param = bitRead(opt_status, 1); A_OFF.d_param = bitRead(opt_status, 1);
    A_RST.id = 3; A_RST.param = bitRead(opt_status, 2); A_RST.d_param = bitRead(opt_status, 2);
    D_brs_day.id = 4; D_brs_day.param = LCD_BRIGHT_MAX; D_brs_day.d_param = map(LCD_BRIGHT_MAX, 0, 255, 0, 100);
    D_brs_night.id = 5; D_brs_night.param = LCD_BRIGHT_MIN; D_brs_night.d_param = map(LCD_BRIGHT_MIN, 0, 255, 0, 100);
    #if(LED_MODE == 0)
      LED_brs_day.id = 6; LED_brs_day.param = LCD_BRIGHT_MAX; LED_brs_day.d_param = map(LCD_BRIGHT_MAX, 0, 255, 0, 100);
      LED_brs_night.id = 7; LED_brs_night.param = LCD_BRIGHT_MIN; LED_brs_night.d_param = map(LCD_BRIGHT_MIN, 0, 255, 0, 100);
    #else
      LED_brs_day.id = 6; LED_brs_day.param = 255 - LCD_BRIGHT_MAX; LED_brs_day.d_param = map(255 - LCD_BRIGHT_MAX, 0, 255, 0, 100);
      LED_brs_night.id = 7; LED_brs_night.param = 255 - LCD_BRIGHT_MIN; LED_brs_night.d_param = map(255 - LCD_BRIGHT_MIN, 0, 255, 0, 100);
    #endif
    Display_mode.id = 8; Display_mode.param = bitRead(opt_status, 4); Display_mode.d_param = bitRead(opt_status, 4);
    time_date.id = 9; time_date.param = false;
    vol.id = 10; vol.param = VOLUME_DT; vol.d_param = VOLUME_DT;
    debug.id = 11; debug.param = bitRead(opt_status, 3); debug.d_param = bitRead(opt_status, 3);
    up.id = 12; up.param = false; up.d_param = false;
  #endif
}

void readSensors() {
  bme.readSensor();

  #if (SENS_PRESS == 1)
    uint16_t dispPres_cache = (float)bme.getPressure_MB() * 0.750062;
    if(dispPres_cache != dispPres){
      dispPres = dispPres_cache;
      ch_flg_press = true;
    }
  #endif

  #if (SENS_TEMP == 1)
    float dispTemp_cache = bme.getTemperature_C();
    if(dispTemp_cache != dispTemp){
      dispTemp = dispTemp_cache;
      ch_flg_temp = true;
    }
  #endif

  #if (SENS_HUM == 1)
    uint8_t dispHum_cache = bme.getHumidity();
    if(dispHum_cache != dispHum){
      dispHum = dispHum_cache;
      ch_flg_hum = true;
    }
  #endif

  #if (SENS_CO2 == 1)
    uint16_t dispCO2_cache = mhz19.getPPM();
    if(dispCO2_cache != dispCO2){
      dispCO2 = dispCO2_cache;
      ch_flg_co2 = true;
    }

    #if (OPTION)
      if (dispCO2 < CO2_GREEN_THRESHOLD) setLED(2, LED_brs_day.param, LED_brs_night.param);
      else if (dispCO2 < CO2_YELLOW_THRESHOLD) setLED(3, LED_brs_day.param, LED_brs_night.param);
      else if (dispCO2 >= CO2_YELLOW_THRESHOLD) setLED(1, LED_brs_day.param, LED_brs_night.param);
    #endif
  #endif
}

void drawSensors() {
#if (DISPLAY_TYPE == 1)
  // for display model 2004
  #if (SENS_TEMP == 1)
    if(ch_flg_temp){
      space_prt(7, 0, 2);
      lcd.print(dispTemp, 1);
      lcd.write(223);
      ch_flg_temp = false;
    }
  #endif

  #if (SENS_HUM == 1)
    if(ch_flg_hum){
      space_prt(5, 7, 2);
      lcd.print(dispHum);
      lcd.print("%");
      ch_flg_hum = false;
    }
  #endif

  #if (SENS_CO2 == 1)
    if(ch_flg_co2){
      space_prt(8, 12, 2);
      lcd.print(dispCO2);
      lcd.print("ppm");
      ch_flg_co2 = false;
    }
  #endif

  #if (SENS_PRESS == 1)
    if(ch_flg_press){
      space_prt(6, 0, 3);
      lcd.print(dispPres);
      lcd.print("mm");
      ch_flg_press = false;
    }
  #endif
  #else
    // for display model 1602
    lcd.setCursor(0, 0);
    lcd.print(String(dispTemp, 1));
    lcd.write(223);
    lcd.setCursor(6, 0);
    lcd.print(String(dispHum) + "% ");

    #if (SENS_CO2 == 1)
      lcd.print(String(dispCO2) + "ppm");
      if (dispCO2 < 1000) lcd.print(" ");
    #endif
    lcd.setCursor(0, 1);
    lcd.print(String(dispPres) + " mm");
  #endif
}



#endif

//----------------------- CLOCK MODULE ------------------------------------

void clockTick() {
  dotFlag = !dotFlag;
  if (dotFlag) {          // Every second recalculation of time
    secs++;
    if (secs > 59) {      // Every minute
      secs = 0;
      mins++;
      if(mins % 10 == 0){
        bitSet(ch_flg_clc, 2);
        bitSet(ch_flg_clc, 3);
      }
        else bitSet(ch_flg_clc, 3);
    }
    if (mins > 59) {      // каждый час
      now = rtc.now();
      secs = now.second();
      mins = now.minute();
      hrs = now.hour();

      if(hrs % 10 == 0){
        bitSet(ch_flg_clc, 0);
        bitSet(ch_flg_clc, 1);
      }
        else bitSet(ch_flg_clc, 1);
        
      if (hrs == 0) {
        bitSet(ch_flg_clc, 0);
        bitSet(ch_flg_clc, 1);
        bitSet(ch_flg_clc, 4);
        bitSet(ch_flg_clc, 5);
        bitSet(ch_flg_clc, 6);
      }
    }

    if (mode == 0) {
        drawClock(hrs, mins, 0, 0);
        drawData();
      }    
      
    #if (DISP_MODE == 2) 
      if(mode == 0) {
        if (secs < 10) space_prt(1, 16, 1);
        lcd.setCursor(17, 1);
        lcd.print(secs);
      }
    #endif
  }

  if (mode == 0) {
      drawDots(7, 0, dotFlag);
    }

  // #if (SENSORS == 1 && SENS_CO2 == 1)
  // if (dispCO2 >= CO2_YELLOW_THRESHOLD) {
  //   if (dotFlag) setLED(1, LED_brs_day.param, LED_brs_night.param);
  //   else setLED(0, LED_brs_day.param, LED_brs_night.param);
  // }
  // #endif
}

void space_prt(uint8_t q, uint8_t x, uint8_t y){
  lcd.setCursor(x, y);
  for(uint8_t x = 0; x < q; x++){
    lcd.print(" ");
  }
  lcd.setCursor(x, y);
}

//---------------------ALARM--------------------------------------------------

#if (ALARM >= 1)

void drawAlarmDisplay(bool draw) {
  uint8_t wakeHour = alarmTune.get_wakeHour();
  uint8_t wakeMinute = alarmTune.get_wakeMinute();
  uint8_t wakeSound = alarmTune.get_wakeSound();
  bool wakeStatus = alarmTune.get_wakeStatus(); 

  if(bitRead(ch_flg_alarm, 2)){
    lcd.clear();

    drawDig(wakeHour / 10, 0, 0);
    drawDig(wakeHour % 10, 4, 0);

    drawDig(wakeMinute / 10, 8, 0);
    drawDig(wakeMinute % 10, 12, 0);

    lcd.setCursor(15,0); lcd.print("s-"); lcd.print(wakeSound);

    lcd.setCursor(15,1); wakeStatus ? lcd.print(on_msg) : lcd.print(off_msg);
    
    drawDots(7, 0, true);


  //--------------------------- print alarm1 -----------------------------------------------
  // lcd.setCursor(0,2);
  // if (alarm1.get_wakeHour() >= 10) { lcd.print(alarm1.get_wakeHour()); lcd.print(":"); }
  //   else { lcd.print("0"); lcd.print(alarm1.get_wakeHour()); lcd.print(":"); }
  
  // lcd.setCursor(3,2);
  // if (alarm1.get_wakeMinute() >= 10) lcd.print(alarm1.get_wakeMinute());
  //   else {  lcd.print("0"); lcd.print(alarm1.get_wakeMinute()); }

  // lcd.setCursor(0,3);
  // lcd.print("s"); lcd.print(alarm1.get_wakeSound()); 

  // lcd.setCursor(3,3);
  // alarm1.get_wakeStatus() ? lcd.print(on_msg) : lcd.print (off_msg);
    alarmPrint(alarm1, 0, 2);
  
  //--------------------------- print alarm2 -----------------------------------------------
  // lcd.setCursor(7,2);
  // if (alarm2.get_wakeHour() >= 10){ lcd.print(alarm2.get_wakeHour()); lcd.print(":"); }
  //   else { lcd.print("0"); lcd.print(alarm2.get_wakeHour()); lcd.print(":"); }

  // lcd.setCursor(10,2);
  // if (alarm2.get_wakeMinute() >= 10) lcd.print(alarm2.get_wakeMinute());
  //   else { lcd.print("0"); lcd.print(alarm2.get_wakeMinute()); }

  // lcd.setCursor(7,3);
  // lcd.print("s"); lcd.print(alarm2.get_wakeSound());

  // lcd.setCursor(10,3);
  // alarm2.get_wakeStatus() ? lcd.print(on_msg) : lcd.print (off_msg);
    #if (ALARM > 1)
      alarmPrint(alarm2, 7, 2);
    #endif

  //--------------------------- print alarm3 -----------------------------------------------
  // lcd.setCursor(14,2);
  // if (alarm3.get_wakeHour() >= 10) { lcd.print(alarm3.get_wakeHour()); lcd.print(":"); }
  //   else { lcd.print("0"); lcd.print(alarm3.get_wakeHour()); lcd.print(":"); }
  
  // lcd.setCursor(17,2);
  // if (alarm3.get_wakeMinute() >= 10) lcd.print(alarm3.get_wakeMinute());
  //   else { lcd.print("0"); lcd.print(alarm3.get_wakeMinute()); }

  // lcd.setCursor(14,3);
  // lcd.print("s"); lcd.print(alarm3.get_wakeSound());

  // lcd.setCursor(17,3);
  // alarm3.get_wakeStatus() ? lcd.print(on_msg) : lcd.print (off_msg);
    #if (ALARM > 2)
      alarmPrint(alarm3, 14, 2);
    #endif
    bitClear(ch_flg_alarm, 2);
  //--------------------------------------------------------------------------------------
  }

  switch(set_param) {
   case 1:
    space_prt(7, 0, 0);
    space_prt(7, 0, 1);
    if(draw){
      drawDig(wakeHour / 10, 0, 0);
      drawDig(wakeHour % 10, 4, 0);
    }
    break;
   case 2:
    space_prt(7, 8, 0);
    space_prt(7, 8, 1);
    if(draw){
      drawDig(wakeMinute / 10, 8, 0);
      drawDig(wakeMinute % 10, 12, 0);
    }
    break;
   case 3:
      space_prt(5, 15, 0);
      if(draw){
        lcd.print("s-"); lcd.print(wakeSound);
      }
      break;
    case 4:
      space_prt(5, 15, 1);
      if(draw){
        wakeStatus ? lcd.print(on_msg) : lcd.print(off_msg);
      }
      break;
    default:
      break;
  }

  #if (ALARM > 1)
    switch (set_alarm){
      case 1:
        lcd.setCursor(5, 2); lcd.print((char)CURSOR_R);
        space_prt(1, 12, 2);
        space_prt(1, 19, 2);
        break;
      case 2:
        space_prt(1, 5, 2);
        lcd.setCursor(12, 2); lcd.print((char)CURSOR_R);
        space_prt(1, 19, 2);
        break;
      case 3:
        space_prt(1, 5, 2);
        space_prt(1, 12, 2);
        lcd.setCursor(19, 2); lcd.print((char)CURSOR_R);
        break;
      default:
        break;
    }
  #endif
}

void alarmPrint(alarm prt, uint8_t x, uint8_t y){
  space_prt(5, x, y);
  if (prt.get_wakeHour() >= 10) { lcd.print(prt.get_wakeHour()); lcd.print(":"); }
    else { lcd.print("0"); lcd.print(prt.get_wakeHour()); lcd.print(":"); }
  
  // lcd.setCursor(x + 3, y);
  if (prt.get_wakeMinute() >= 10) lcd.print(prt.get_wakeMinute());
    else {  lcd.print("0"); lcd.print(prt.get_wakeMinute()); }

  space_prt(6, x, y + 1);
  lcd.print("s"); lcd.print(prt.get_wakeSound()); 

  lcd.setCursor(x + 3, y + 1);
  prt.get_wakeStatus() ? lcd.print(on_msg) : lcd.print (off_msg);
}

void alarmTuning(){
  checkInput();
  if (!firstStartFlag){
    alarmTune.alarm_set(hrs, mins, 1, false);
    firstStartFlag = true;
    #if (ALARM > 1)
      set_alarm = 1;
    #endif
    set_param = 1;
    enc.resetStates();

    backToMain.start();
    backToMain.reset();
    
    bitSet(ch_flg_alarm, 2);    // Updat alarm disp
    drawAlarmDisplay(true);
  }

  bitClear(ch_flg_alarm, 0);    // Enc action flag
  // change_flag = false;

  if (enc.isDouble()){      // Save the current options of alarm

    #if (ALARM == 1)
      alarm1.alarm_set(alarmTune.get_wakeHour(), alarmTune.get_wakeMinute(), alarmTune.get_wakeSound(), alarmTune.get_wakeStatus());
      #if (EEPROM_P == 1)
        alarm1.putEEPROM(ALARM1_HOUR_ADDR, ALARM1_MIN_ADDR, ALARM1_SOUND_ADDR, ALARM1_STATUS_ADDR);
      #endif
    #elif (ALARM == 3)
      switch (set_alarm){
        case 1:
          alarm1.alarm_set(alarmTune.get_wakeHour(), alarmTune.get_wakeMinute(), alarmTune.get_wakeSound(), alarmTune.get_wakeStatus());
          #if (EEPROM_P == 1)
            alarm1.putEEPROM(ALARM1_HOUR_ADDR, ALARM1_MIN_ADDR, ALARM1_SOUND_ADDR, ALARM1_STATUS_ADDR);
          #endif
          break;
        case 2:
          alarm2.alarm_set(alarmTune.get_wakeHour(), alarmTune.get_wakeMinute(), alarmTune.get_wakeSound(), alarmTune.get_wakeStatus());
          #if (EEPROM_P == 1)
            alarm2.putEEPROM(ALARM2_HOUR_ADDR, ALARM2_MIN_ADDR, ALARM2_SOUND_ADDR, ALARM2_STATUS_ADDR);
            #endif
          break;
        case 3:
          alarm3.alarm_set(alarmTune.get_wakeHour(), alarmTune.get_wakeMinute(), alarmTune.get_wakeSound(), alarmTune.get_wakeStatus());
          #if (EEPROM_P == 1)
            alarm3.putEEPROM(ALARM3_HOUR_ADDR, ALARM3_MIN_ADDR, ALARM3_SOUND_ADDR, ALARM3_STATUS_ADDR);
          #endif
          break;
        default:
          break;
      }
      ++set_alarm;
      if (set_alarm > 3) set_alarm = 1;
    #endif
    lcd.clear();
    bitSet(ch_flg_alarm, 0);    // Enc action flag
    bitSet(ch_flg_alarm, 2);    // Update disp flag
    // change_flag = true;
  }
  
  if(enc.isSingle()){       // Switch selected parameter on next
    ++set_param;
    if(set_param > 4) set_param = 1;
    backToMain.reset();
    bitSet(ch_flg_alarm, 0);    // Enc action flag
    // bitSet(ch_flg_alarm, 2);    // Update disp flag
  }

  #if (ALARM > 1)
    if(enc.isRightH()){       // Switch selected alarm on next 
      ++set_alarm;
      if (set_alarm > 3) set_alarm = 1;
      backToMain.reset();
      bitSet(ch_flg_alarm, 0);    // Enc action flag
    }

    if (enc.isLeftH()){       // Switch selected alarm on previous
      --set_alarm;
      if (set_alarm < 1) set_alarm = 3;
      backToMain.reset();
      bitSet(ch_flg_alarm, 0);    // Enc action flag
    }
  #endif

  if(enc.isRight()){        // Param increase
    alarmTune.change_param(set_param, true);
    backToMain.reset();
    bitSet(ch_flg_alarm, 0);    // Enc action flag
  }

  if(enc.isLeft()){         // Param decrease
    alarmTune.change_param(set_param, false);
    backToMain.reset();
    bitSet(ch_flg_alarm, 0);    // Enc action flag
  }

  //--------------------------- print alarmTune -------------------------------------------  

  if(drawDown_param.isReady()) {
    drawDown_param.stop();
    drawUp_param.start();
    drawUp_param.reset();
    drawAlarmDisplay(false);
  }

  if(drawUp_param.isReady() || bitRead(ch_flg_alarm, 0)) {
    drawUp_param.stop();
    drawDown_param.start();
    drawDown_param.reset();
    drawAlarmDisplay(true);
  }

  if(backToMain.isReady() || button.isHolded()){
    Mode(0);
    firstStartFlag = false;
    draw_main_disp();
    backToMain.stop();
  }
}

#if (ALARM == 1)
  void alarmStart(){
    play_sound(alarm1.wakeUP_Sound());
  }

  bool alarmControl(){
    return alarm1.checkAlarm(hrs, mins);
  }
#elif (ALARM > 1)
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
#endif

void alarmStop(){
    if (alarm1.isRunning()) {
      alarm1.stop();
      play_sound(0);
    }

    #if (ALARM > 1)
      if (alarm2.isRunning()) {
        alarm2.stop();
        play_sound(0);
      }
    #endif

    #if (ALARM > 2)
      if (alarm3.isRunning()) {
        alarm3.stop();
        play_sound(0);
      }
    #endif
    bitSet(ch_flg_alarm, 1);
}

  #if(OPTION == 1)
    void alarm_OFF(){
      alarm1.stop();

      #if (ALARM > 1)
        alarm2.stop();
      #endif

      #if (ALARM > 2)
        alarm3.stop();
      #endif

      space_prt(4, 14, 1);
      lcd.print(wait_msg);
      delay(DELAY);           // bad thing, needs to replace
      space_prt(4, 14, 1);
      lcd.print(hold_msg);
    }

    void alarm_RST(){
      alarm1.rst();

      #if (ALARM > 1)
        alarm2.rst();
      #endif

      #if (ALARM > 2)
        alarm3.rst();
      #endif

      space_prt(4, 14, 2);
      lcd.print(wait_msg);
      delay(DELAY);           // bad thing, needs to replace
      space_prt(4, 14, 2);
      lcd.print(hold_msg);
    }
  #endif
#endif

void drawFlags(){
  #if(ALARM >= 1)
    if(bitRead(ch_flg_alarm, 1)){
      space_prt(5, 7, 3);
      lcd.print("A:"); lcd.setCursor(9, 3);

      #if (ALARM == 1)
        alarm1.get_wakeStatus() ? lcd.print(on_msg) : lcd.print(off_msg);
      #endif
      #if (AlARM > 1)
        if(alarm1.get_wakeStatus() || alarm2.get_wakeStatus()){
           lcd.print(on_msg);
        }
          else { lcd.print(off_msg); }
      #endif
      #if (ALARM > 2)
        if(alarm1.get_wakeStatus() || alarm2.get_wakeStatus() || alarm3.get_wakeStatus()){
           lcd.print(on_msg);
        }
          else { lcd.print(off_msg); }
      #endif
      bitClear(ch_flg_alarm, 1);
    }
  #endif

  #if(POWER_IND == 1)
    if(ch_flg_power){
      space_prt(4, 18, 3);
      if(power_state){
        lcd.setCursor(19, 3); lcd.print("!");
      }
        else { space_prt(2, 19, 3); }
      ch_flg_power = false;
    }
  #elif(POWER_IND == 2)
    if(ch_flg_power){
      space_prt(6, 13, 3);
      lcd.print("B:"); lcd.print(map(power_level_cur, BAT_MIN_V, BAT_MAX_V, 0, 100)); lcd.print("%");
      ch_flg_power = false;

      #if (VOLT_DEBUG == 1)
        space_prt(2, 10, 3);
        lcd.print((String)power_level_cur);
      #endif
    }
  #endif  
}

#if (DEBUG == 1)
void debug_start(){
    #if (DISPLAY_TYPE == 1)
        Serial.begin(9600);
        boolean status = true;
        lcd.clear();

        setLED(1, LED_brs_day.param, LED_brs_night.param);

    #if (SENS_CO2 == 1)
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
            lcd.print(F(error_msg));
            Serial.println(F(error_msg));
            status = false;
        }
    #endif

    setLED(2, LED_brs_day.param, LED_brs_night.param);
    lcd.setCursor(0, 1);
    lcd.print(F("RTC... "));
    Serial.print(F("RTC... "));
    delay(50);
    if(rtc.begin()) {
        lcd.print(F("OK"));
        Serial.println(F("OK"));
    } else {
        lcd.print(F(error_msg));
        Serial.println(F(error_msg));
        status = false;
    }

    setLED(3, LED_brs_day.param, LED_brs_night.param);
    lcd.setCursor(0, 2);
    lcd.print(F("BME280... "));
    Serial.print(F("BME280... "));
    delay(50);
    if(bme.begin()) {
        lcd.print(F("OK"));
        Serial.println(F("OK"));
    } else {
        lcd.print(F(error_msg));
        Serial.println(F(error_msg));
        status = false;
    }

    setLED(0, LED_brs_day.param, LED_brs_night.param);
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

    // #if (SENS_CO2 == 1)
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

}

void go_debug(){

}
#endif

#if (EEPROM_P == 1)
  void EEPROM_init(){
    // Serial.begin(9600);

    uint8_t key = EEPROM.read(EEPROM_KEY_ADDR);
    if(key != EEPROM_KEY){
        EEPROM.write(EEPROM_KEY_ADDR, EEPROM_KEY);
        #if (ALARM >= 1)
          alarm1.putEEPROM(ALARM1_HOUR_ADDR, ALARM1_MIN_ADDR, ALARM1_SOUND_ADDR, ALARM1_STATUS_ADDR);

          #if (ALARM > 1)
            alarm2.putEEPROM(ALARM2_HOUR_ADDR, ALARM2_MIN_ADDR, ALARM2_SOUND_ADDR, ALARM2_STATUS_ADDR);
          #endif

          #if (ALARM > 2)
            alarm3.putEEPROM(ALARM3_HOUR_ADDR, ALARM3_MIN_ADDR, ALARM3_SOUND_ADDR, ALARM3_STATUS_ADDR);
          #endif
        #endif

        #if (OPTION == 1)
          opt_status = OPTION_DT;
          N_B.id = 1; N_B.param = bitRead(opt_status, 0); N_B.d_param = bitRead(opt_status, 0);
          A_OFF.id = 2; A_OFF.param = bitRead(opt_status, 1); A_OFF.d_param = bitRead(opt_status, 1);
          A_RST.id = 3; A_RST.param = bitRead(opt_status, 2); A_RST.d_param = bitRead(opt_status, 2);
          D_brs_day.id = 4; D_brs_day.param = LCD_BRIGHT_MAX; D_brs_day.d_param = map(LCD_BRIGHT_MAX, 0, 255, 0, 100);
          D_brs_night.id = 5; D_brs_night.param = LCD_BRIGHT_MIN; D_brs_night.d_param = map(LCD_BRIGHT_MIN, 0, 255, 0, 100);
          #if(LED_MODE == 0)
            LED_brs_day.id = 6; LED_brs_day.param = LCD_BRIGHT_MAX; LED_brs_day.d_param = map(LCD_BRIGHT_MAX, 0, 255, 0, 100);
            LED_brs_night.id = 7; LED_brs_night.param = LCD_BRIGHT_MIN; LED_brs_night.d_param = map(LCD_BRIGHT_MIN, 0, 255, 0, 100);
          #else
            LED_brs_day.id = 6; LED_brs_day.param = 255 - LCD_BRIGHT_MAX; LED_brs_day.d_param = map(255 - LCD_BRIGHT_MAX, 0, 255, 0, 100);
            LED_brs_night.id = 7; LED_brs_night.param = 255 - LCD_BRIGHT_MIN; LED_brs_night.d_param = map(255 - LCD_BRIGHT_MIN, 0, 255, 0, 100);
          #endif
          Display_mode.id = 8; Display_mode.param = bitRead(opt_status, 4); Display_mode.d_param = bitRead(opt_status, 4);
          time_date.id = 9; time_date.param = false;
          vol.id = 10; vol.param = VOLUME_DT; vol.d_param = VOLUME_DT;
          debug.id = 11; debug.param = bitRead(opt_status, 3); debug.d_param = bitRead(opt_status, 3);
          up.id = 12; up.param = false; up.d_param = false;
          opt_eeprom_save();
        #endif
    }
      else{
        #if (OPTION == 1)
          opt_eeprom_dwl();
        #endif

        #if (ALARM >= 1)
          alarm1.getEEPROM(ALARM1_HOUR_ADDR, ALARM1_MIN_ADDR, ALARM1_SOUND_ADDR, ALARM1_STATUS_ADDR);

          #if (ALARM > 1)
            alarm2.getEEPROM(ALARM2_HOUR_ADDR, ALARM2_MIN_ADDR, ALARM2_SOUND_ADDR, ALARM2_STATUS_ADDR);
          #endif

          #if (ALARM > 2)
            alarm3.getEEPROM(ALARM3_HOUR_ADDR, ALARM3_MIN_ADDR, ALARM3_SOUND_ADDR, ALARM3_STATUS_ADDR);
          #endif
        #endif
      }

  }
#endif

void reload_ch_flg(){
  bitSet(ch_flg_clc, 0);
  bitSet(ch_flg_clc, 1);
  bitSet(ch_flg_clc, 2);
  bitSet(ch_flg_clc, 3);
  bitSet(ch_flg_clc, 4);
  bitSet(ch_flg_clc, 5);
  bitSet(ch_flg_clc, 6);

  #if (SENSORS == 1)
    #if(SENS_TEMP == 1)
      ch_flg_temp = true;
    #endif
    #if(SENS_HUM == 1)
      ch_flg_hum = true;
    #endif
    #if(SENS_CO2 == 1)
      ch_flg_co2 = true;
    #endif
    #if(SENS_PRESS == 1)
      ch_flg_press = true;
    #endif
  #endif

  #if (ALARM >= 1)
    bitSet(ch_flg_alarm, 1);
    // ch_flg_alarm = true;
  #endif

  #if(POWER_IND != 0)
    ch_flg_power = true;
  #endif
}

void power_control(){
  analogReference(INTERNAL);

  for (uint8_t i; i < 50; i++){
    analogRead(PC_PIN);
  }
  
  // uint16_t curAV = analogRead(PC_PIN);
  // // Serial.println("Va: " + (String)curAV);
  // power_level_cur = (int)(curAV * 0.377);

  // Serial.println("Vcur: " + (String)power_level_cur);

  // #if (POWER_IND == 1)
  //   power_level_cur >= PC_LEVEL ? power_state = true : power_state = false;

  // #elif (POWER_IND == 2)

  //   uint8_t level_counter = sizeof(power_levels)/sizeof(power_levels[0]);
  //   uint8_t level = 0;
  //   while (level < level_counter) {
  //     if(power_level_cur >= power_levels[level]) {
  //       level++;
  //     }
  //       else break;
  //   }
  //   power_state = level;
    
  // #endif

  // if(power_state != cach_power_state){
  //   cach_power_state = power_state;
  //   ch_flg_power = true;
  // }

  
  uint16_t curAV = analogRead(PC_PIN);
  // Serial.println("Va: " + (String)curAV);
  
  power_level_cur = (int)(curAV * 0.083);
  // Serial.println("Vcur: " + (String)power_level_cur);

  power_level_cur = constrain(power_level_cur, BAT_MIN_V, BAT_MAX_V);
  

  if(power_level_cur != cach_power_state){
    cach_power_state = power_level_cur;
    ch_flg_power = true;
  }

  analogReference(DEFAULT);
}

//----------------------OPTIONS-------------------------------------

#if (OPTION == 1)

void options(){
  if(!firstStartFlag){
    lcd.clear();
    enc.resetStates();
    backToMain.start();
    backToMain.reset();
    opt_ref();
    opt_up();
    cursor_pos = 1;
    opt_prt(opt);
  }

  checkInput();

  if (time_date.param) time_date_set();
  if(!time_date.param){
    if (cursor_get_pos()){
      opt_prt(opt);
    }
  }
  if(button.isHolded() || backToMain.isReady()){
    Mode(0);
    firstStartFlag = false;
    draw_main_disp();
    cursor_pos = 1;
  }
}

void opt_ref(){
  N_B.d_param = N_B.param;
  A_OFF.d_param = A_OFF.param;
  A_RST.d_param = A_RST.param;
  D_brs_day.d_param = map(D_brs_day.param, 0, 255, 0, 100);
  D_brs_night.d_param = map(D_brs_night.param, 0, 255, 0, 100);
  LED_brs_day.d_param = map(LED_brs_day.param, 0, 255, 0, 100);
  LED_brs_night.d_param = map(LED_brs_night.param, 0, 255, 0, 100);
  Display_mode.d_param = Display_mode.param;
  // time_date.d_param = time_date.param;
  vol.d_param = map(vol.param, 0, 255, 0, 100);
  debug.d_param = debug.param;
  up.d_param = up.param;
}

void opt_save(){
  switch(cursor_pos){
  case 101:
    N_B.param = N_B.d_param;
    N_B.param ? bitSet(opt_status, 0) : bitClear(opt_status, 0);
    break;
  case 104:
    D_brs_day.param = map(D_brs_day.d_param, 0, 100, 0, 255);
    #if (EEPROM_P == 1)
      EEPROM.update(DIS_BRS_DAY_ADDR, D_brs_day.param);
    #endif
    break;
  case 105:
    D_brs_night.param = map(D_brs_night.d_param, 0, 100, 0, 255);
    #if (EEPROM_P == 1)
      EEPROM.update(DIS_BRS_NIGHT_ADDR, D_brs_night.param);
    #endif
    break;
  case 106:
    LED_brs_day.param = map(LED_brs_day.d_param, 0, 100, 0, 255);
    #if (EEPROM_P == 1)
      EEPROM.update(RGB_BRS_DAY_ADDR, LED_brs_day.param);
    #endif
    break;
  case 107:
    LED_brs_night.param = map(LED_brs_night.d_param, 0, 100, 0, 255);
    #if (EEPROM_P == 1)
      EEPROM.update(RGB_BRS_NIGHT_ADDR, LED_brs_night.param);
    #endif
    break;
  case 108:
    Display_mode.param = Display_mode.d_param;
    (Display_mode.param != 0) ? bitSet(opt_status, 4) : bitClear(opt_status, 4);
    break;
  case 110:
    vol.param = vol.d_param;
    #if (EEPROM_P == 1)
      EEPROM.update(VOLUME_ADDR, vol.param);
    #endif
    break;
  default:
    break;
  }
}

void opt_change(bool dir){
  switch(cursor_pos){
    case 101:
      N_B.d_param = !N_B.d_param;
      opt.s1_c2 = N_B.d_param;
      break;
    case 104:
      if(dir) (D_brs_day.d_param == 100) ? : ++D_brs_day.d_param;
        else (D_brs_day.d_param == 0) ? : --D_brs_day.d_param;
        opt.s4_c2 = D_brs_day.d_param;
        brightnessRef(map(D_brs_day.d_param, 0, 100, 0, 255), map(D_brs_night.d_param, 0, 100, 0, 255), map(LED_brs_day.d_param, 0, 100, 0, 255), map(LED_brs_night.d_param, 0, 100, 0, 255));
      break;
    case 105:
      if(dir) (D_brs_night.d_param == 100) ? : ++D_brs_night.d_param;
        else (D_brs_night.d_param == 0) ? : --D_brs_night.d_param;
      opt.s1_c2 = D_brs_night.d_param;
      brightnessRef(map(D_brs_day.d_param, 0, 100, 0, 255), map(D_brs_night.d_param, 0, 100, 0, 255), map(LED_brs_day.d_param, 0, 100, 0, 255), map(LED_brs_night.d_param, 0, 100, 0, 255));
      break;
    case 106:
      if(dir) (LED_brs_day.d_param == 100) ? : ++LED_brs_day.d_param;
       else (LED_brs_day.d_param == 0) ? : --LED_brs_day.d_param;
      opt.s2_c2 = LED_brs_day.d_param;
      brightnessRef(map(D_brs_day.d_param, 0, 100, 0, 255), map(D_brs_night.d_param, 0, 100, 0, 255), map(LED_brs_day.d_param, 0, 100, 0, 255), map(LED_brs_night.d_param, 0, 100, 0, 255));
      break;
    case 107:
      if(dir) (LED_brs_night.d_param == 100) ? : ++LED_brs_night.d_param;
        else (LED_brs_night.d_param == 0) ? : --LED_brs_night.d_param;
      opt.s3_c2 = LED_brs_night.d_param;
      brightnessRef(map(D_brs_day.d_param, 0, 100, 0, 255), map(D_brs_night.d_param, 0, 100, 0, 255), map(LED_brs_day.d_param, 0, 100, 0, 255), map(LED_brs_night.d_param, 0, 100, 0, 255));
      break;
    case 108:
      if(dir) (Display_mode.d_param == 2) ? : ++Display_mode.d_param;
        else (Display_mode.d_param == 0) ? : --Display_mode.d_param;
      opt.s4_c2 = Display_mode.d_param;
      break;
    case 110:
      if(dir) (vol.d_param == 100) ? : ++vol.d_param;
        else (vol.d_param == 0) ? : --vol.d_param;
      opt.s1_c2 = vol.d_param;
      break;
    default:
      break;
  }
}

// void opt_switch_disp(uint8_t x){
//   opt.s1_c1 = opt_fnd(x); opt.s1_c2 = opt_fnd(x + 100);
//   opt.s2_c1 = opt_fnd(x + 1); opt.s2_c2 = opt_fnd(x + 101);
//   opt.s3_c1 = opt_fnd(x + 2); opt.s3_c2 = opt_fnd(x + 102);
//   opt.s4_c1 = opt_fnd(x + 3); opt.s4_c2 = opt_fnd(x + 103);
// }

#if (EEPROM_P == 1)
  void opt_eeprom_save(){
    EEPROM.update(OPTION_STATUS_ADDR, opt_status);
    EEPROM.update(DIS_BRS_DAY_ADDR, D_brs_day.param);
    EEPROM.update(DIS_BRS_NIGHT_ADDR, D_brs_night.param);
    EEPROM.update(RGB_BRS_DAY_ADDR, LED_brs_day.param);
    EEPROM.update(RGB_BRS_NIGHT_ADDR, LED_brs_night.param);
    EEPROM.update(VOLUME_ADDR, vol.param);
  }

  void opt_eeprom_dwl(){
    EEPROM.get(OPTION_STATUS_ADDR, opt_status);
    N_B.id = 1; N_B.param = bitRead(opt_status, 0); N_B.d_param = bitRead(opt_status, 0);
    A_OFF.id = 2; A_OFF.param = bitRead(opt_status, 1); A_OFF.d_param = bitRead(opt_status, 1);
    A_RST.id = 3; A_RST.param = bitRead(opt_status, 2); A_RST.d_param = bitRead(opt_status, 2);
    D_brs_day.id = 4; D_brs_day.param = EEPROM.read(DIS_BRS_DAY_ADDR); D_brs_day.d_param = map(EEPROM.read(DIS_BRS_DAY_ADDR), 0, 255, 0, 100);
    D_brs_night.id = 5; D_brs_night.param = EEPROM.read(DIS_BRS_NIGHT_ADDR); D_brs_night.d_param = map(EEPROM.read(DIS_BRS_NIGHT_ADDR), 0, 255, 0, 100);
    LED_brs_day.id = 6; LED_brs_day.param = EEPROM.read(RGB_BRS_DAY_ADDR); LED_brs_day.d_param = map(EEPROM.read(RGB_BRS_DAY_ADDR), 0, 255, 0, 100);
    LED_brs_night.id = 7; LED_brs_night.param = EEPROM.read(RGB_BRS_NIGHT_ADDR); LED_brs_night.d_param = map(EEPROM.read(RGB_BRS_NIGHT_ADDR), 0, 255, 0, 100);
    Display_mode.id = 8; Display_mode.param = bitRead(opt_status, 4); Display_mode.d_param = bitRead(opt_status, 4);
    time_date.id = 9; time_date.param = false;
    vol.id = 10; vol.param = EEPROM.read(VOLUME_ADDR); vol.d_param = EEPROM.read(VOLUME_ADDR);
    debug.id = 11; debug.param = bitRead(opt_status, 3); debug.d_param = bitRead(opt_status, 3);
    up.id = 12; up.param = false; up.d_param = false;
  }
#endif

bool cursor_get_pos(){
  if(enc.isRight()){
    backToMain.reset();
    if(cursor_pos < 100){
      cursor_pos < 12 ? cursor_pos++ : false;
      cursor_prt();
    //   if(cursor_pos >= 1 && cursor_pos <= 4 && opt.s1_c1 != opt_fnd(1)){
    //     opt_switch_disp(1);
    //     firstStartFlag = false;
    //     return true;
    //   }
    //   else if(cursor_pos >= 5 && cursor_pos <= 8 && opt.s1_c1 != opt_fnd(5)){
    //     opt_switch_disp(5);
    //     firstStartFlag = false;
    //     return true;
    //   }
    //   else if(cursor_pos >= 9 && cursor_pos <= 11 && opt.s1_c1 != opt_fnd(9)){
    //     opt_switch_disp(9);
    //     firstStartFlag = false;
    //     return true;
    //   }
      switch(cursor_pos) {
        case 5:
          opt.s1_c1 = opt_fnd(5); opt.s1_c2 = opt_fnd(105);
          opt.s2_c1 = opt_fnd(6); opt.s2_c2 = opt_fnd(106);
          opt.s3_c1 = opt_fnd(7); opt.s3_c2 = opt_fnd(107);
          opt.s4_c1 = opt_fnd(8); opt.s4_c2 = opt_fnd(108);
          firstStartFlag = false;
          return true;
        case 9:
          opt.s1_c1 = opt_fnd(9); opt.s1_c2 = opt_fnd(109);
          opt.s2_c1 = opt_fnd(10); opt.s2_c2 = opt_fnd(110);
          opt.s3_c1 = opt_fnd(11); opt.s3_c2 = opt_fnd(111);
          opt.s4_c1 = opt_fnd(12); opt.s4_c2 = opt_fnd(112);
          firstStartFlag = false;
          return true;
      default:
        break;
      }
    }
    else if(cursor_pos > 100){
      if(cursor_pos == 101 || (cursor_pos >= 104 && cursor_pos <= 108) || cursor_pos == 110){
        opt_change(true);
        return true;
      }
    }
  }

  if(enc.isLeft()){
    backToMain.reset();
    if(cursor_pos < 100){
      cursor_pos > 1 ? cursor_pos-- : false;
      cursor_prt();
      // if(cursor_pos >= 1 && cursor_pos <= 4 && opt.s1_c1 != opt_fnd(1)){
      //   opt_switch_disp(1);
      //   firstStartFlag = false;
      //   return true;
      // }
      // else if(cursor_pos >= 5 && cursor_pos <= 8 && opt.s1_c1 != opt_fnd(5)){
      //   opt_switch_disp(5);
      //   firstStartFlag = false;
      //   return true;
      // }
      // else if(cursor_pos >= 9 && cursor_pos <= 11 && opt.s1_c1 != opt_fnd(9)){
      //   opt_switch_disp(9);
      //   firstStartFlag = false;
      //   return true;
      // }
      switch(cursor_pos){
        case 4:
          opt.s1_c1 = opt_fnd(1); opt.s1_c2 = opt_fnd(101);
          opt.s2_c1 = opt_fnd(2); opt.s2_c2 = opt_fnd(102);
          opt.s3_c1 = opt_fnd(3); opt.s3_c2 = opt_fnd(103);
          opt.s4_c1 = opt_fnd(4); opt.s4_c2 = opt_fnd(104);
          firstStartFlag = false;
          return true;
        case 8:
          opt.s1_c1 = opt_fnd(5); opt.s1_c2 = opt_fnd(105);
          opt.s2_c1 = opt_fnd(6); opt.s2_c2 = opt_fnd(106);
          opt.s3_c1 = opt_fnd(7); opt.s3_c2 = opt_fnd(107);
          opt.s4_c1 = opt_fnd(8); opt.s4_c2 = opt_fnd(108);
          firstStartFlag = false;
          return true;
        default:
          break;
      }
    }
    else if(cursor_pos > 100){
      if(cursor_pos == 101 || (cursor_pos >= 104 && cursor_pos <= 108) || cursor_pos == 110){
        opt_change(false);
        return true;
      }
    }
  }

  if(enc.isHolded()){
    backToMain.reset();
        switch (cursor_pos){
          case 102:
            #if (ALARM >= 1)
              alarm_OFF();
            #endif
            return true;
            break;
          case 103:
            #if (ALARM >= 1)
              alarm_RST();
            #endif
            return true;
            break;
          case 109:
            time_date.param = true;
            break;
          case 111:
            #if (DEBUG == 1)
              go_debug();
            #endif
            break;
          default:
            if(cursor_pos == 101 || (cursor_pos >= 104 && cursor_pos <= 110)){
              opt_save();
              // if(cursor_pos == 108) display_blinking(false);
              brightnessRef(D_brs_day.param, D_brs_night.param, LED_brs_day.param, LED_brs_night.param);
              cursor_pos -= 100;
              cursor_prt();
              return true;
            }
            break;
        }
  }

  if(enc.isClick()){
    backToMain.reset();
    if(cursor_pos == 12){
      opt_up();
      return true;
    }
      else{
        if(cursor_pos < 100){
          cursor_pos += 100;
          cursor_prt();
        }
        else {
          if(cursor_pos == 101 || (cursor_pos >= 104 && cursor_pos <= 108) || cursor_pos == 110){
            switch (cursor_pos) {
              case 101:
                N_B.d_param = N_B.param;
                opt.s1_c2 = N_B.d_param;
                break;
              case 104:
                D_brs_day.d_param = map(D_brs_day.param, 0, 255, 0, 100);
                opt.s4_c2 = D_brs_day.d_param;
                break;
              case 105:
                D_brs_night.d_param = map(D_brs_night.param, 0, 255, 0, 100);
                opt.s1_c2 = D_brs_night.d_param;
                break;
              case 106:
                LED_brs_day.d_param = map(LED_brs_day.param, 0, 255, 0, 100);
                opt.s2_c2 = LED_brs_day.d_param;
                break;
              case 107:
                LED_brs_night.d_param = map(LED_brs_night.param, 0, 255, 0, 100);
                opt.s3_c2 = LED_brs_night.d_param;
                break;
              case 108:
                Display_mode.d_param = Display_mode.param;
                opt.s4_c2 = Display_mode.d_param;
                // display_blinking(false);
                break;
              case 110:
                vol.d_param = vol.param;
                opt.s1_c2 = vol.d_param;
                break;
              default:
                break;
              }
            brightnessRef(D_brs_day.param, D_brs_night.param, LED_brs_day.param, LED_brs_night.param);
            cursor_pos -= 100;
            cursor_prt();
            return true;
          }
          cursor_pos -= 100;
          cursor_prt();
        }
      }
  }

  // if(cursor_pos == 108) display_blinking(true);
  
  // if(enc.isDouble()){
  //   if(cursor_pos < 100){
  //     return true;
  //   }
  //   else if(cursor_pos >= 100){
  //     return true;
  //   }
  // }
  return false;
}

void opt_prt(print cur_opt){
  String d_cur_param;
  if(lst_opt.s1_c1 != cur_opt.s1_c1 || !firstStartFlag){
    lst_opt.s1_c1 = cur_opt.s1_c1;
    space_prt(14, 1, 0);
    lcd.print(optNames[cur_opt.s1_c1 - 1]);
  }
  if(lst_opt.s1_c2 != cur_opt.s1_c2 || !firstStartFlag){
    lst_opt.s1_c2 = cur_opt.s1_c2;
    space_prt(5, 14, 0);
    switch (cur_opt.s1_c1){
      case 1:
        if(cur_opt.s1_c2 == 0){
          lcd.setCursor(14 + (5 - off_msg.length()), 0);
          lcd.print(off_msg);
        }
          else{
            lcd.setCursor(14 + (5 - on_msg.length()), 0);
            lcd.print(on_msg);
          }
        break;
      case 5:
        d_cur_param = (String)cur_opt.s1_c2;
        lcd.setCursor(14 + (5 - d_cur_param.length()), 0);
        lcd.print(D_brs_night.d_param);
        break;
      case 9:
        lcd.setCursor(14 + (5 - hold_msg.length()), 0);
        lcd.print(hold_msg);
        break;
      default:
        lcd.setCursor(14 + (5 - error_msg.length()), 0);
        lcd.print(error_msg);
        break;
    }
  }
  if(lst_opt.s2_c1 != cur_opt.s2_c1 || !firstStartFlag){
    lst_opt.s2_c1 = cur_opt.s2_c1;
    space_prt(14, 1, 1);
    lcd.print(optNames[cur_opt.s2_c1 - 1]);
  }
  if(lst_opt.s2_c2 != cur_opt.s2_c2 || !firstStartFlag){
    lst_opt.s2_c2 = cur_opt.s2_c2;
    space_prt(5, 14, 1);
    switch (cur_opt.s2_c1){
      case 2:
        if(cur_opt.s2_c2 == 0){
          lcd.setCursor(14 + (5 - hold_msg.length()), 1);
          lcd.print(hold_msg);
        }
          else{
            lcd.setCursor(14 + (5 - wait_msg.length()), 1);
            lcd.print(wait_msg);
          }
        break;
      case 6:
        d_cur_param = (String)cur_opt.s2_c2;
        lcd.setCursor(14 + (5 - d_cur_param.length()), 1);
        lcd.print(LED_brs_day.d_param);
        break;
      case 10:
        d_cur_param = (String)cur_opt.s2_c2;
        lcd.setCursor(14 + (5 - d_cur_param.length()), 1);
        lcd.print(vol.d_param);
        break;
      default:
        lcd.setCursor(14 + (5 - error_msg.length()), 1);
        lcd.print(error_msg);
        break;
    }
  }
  if(lst_opt.s3_c1 != cur_opt.s3_c1 || !firstStartFlag){
    lst_opt.s3_c1 = cur_opt.s3_c1;
    space_prt(14, 1, 2);
    lcd.print(optNames[cur_opt.s3_c1 - 1]);
  }
  if(lst_opt.s3_c2 != cur_opt.s3_c2 || !firstStartFlag){
    lst_opt.s3_c2 = cur_opt.s3_c2;
    space_prt(5, 14, 2);
    switch (cur_opt.s3_c1){
      case 3:
        if(cur_opt.s3_c2 == 0){
          lcd.setCursor(14 + (5 - hold_msg.length()), 2);
          lcd.print(hold_msg);
        }
          else{
            lcd.setCursor(14 + (5 - wait_msg.length()), 2);
            lcd.print(wait_msg);
          }
        break;
      case 7:
        d_cur_param = (String)cur_opt.s3_c2;
        lcd.setCursor(14 + (5 - d_cur_param.length()), 2);
        lcd.print(LED_brs_night.d_param);
        break;
      case 11:
        if(cur_opt.s2_c2 == 0){
          lcd.setCursor(14 + (5 - hold_msg.length()), 2);
          lcd.print(hold_msg);
        }
          else{
            lcd.setCursor(14 + (5 - wait_msg.length()), 2);
            lcd.print(wait_msg);
          }
        break;
      default:
        lcd.setCursor(14 + (5 - error_msg.length()), 2);
        lcd.print(error_msg);
        break;
    }
  }
  if(lst_opt.s4_c1 != cur_opt.s4_c1 || !firstStartFlag){
    lst_opt.s4_c1 = cur_opt.s4_c1;
    space_prt(14, 1, 3);
    lcd.print(optNames[cur_opt.s4_c1 - 1]);
  }
  if(lst_opt.s4_c2 != cur_opt.s4_c2 || !firstStartFlag){
    lst_opt.s4_c2 = cur_opt.s4_c2;
    space_prt(5, 14, 3);
    switch (cur_opt.s4_c1){
      case 4:
        d_cur_param = (String)cur_opt.s4_c2;
        lcd.setCursor(14 + (5 - d_cur_param.length()), 3);
        lcd.print(D_brs_day.d_param);
        break;
      case 8:
        switch (cur_opt.s4_c2){
          case 0:
            lcd.setCursor(14 + (5 - blink_msg.length()), 3);
            lcd.print(blink_msg);
            break;
          case 1:
            lcd.setCursor(14 + (5 - slow_msg.length()), 3);
            lcd.print(slow_msg);
            break;
          default:
            lcd.setCursor(14 + (5 - off_msg.length()), 3);
            lcd.print(off_msg);
            break;
        }
        break;
      case 12:
        lcd.setCursor(14 + (5 - click_msg.length()), 3);
        lcd.print(click_msg);
        break;
      default:
        lcd.setCursor(14 + (5 - error_msg.length()), 3);
        lcd.print(error_msg);
        break;
    }
  }
  firstStartFlag = true;
  cursor_prt();
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
      return time_date.id;
    case 10:
      return vol.id;
    case 11:
      return debug.id;
    case 12:
      return up.id;
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
      return time_date.param;
    case 110:
      return vol.d_param;
    case 111:
      return debug.d_param;
    case 112:
      return up.d_param;
    default:
      break;
  }
  return 0;
}

void cursor_prt(){
  static uint8_t last_curs_pos;
  if(last_curs_pos != cursor_pos || !firstStartFlag){
    if(last_curs_pos == 1 || last_curs_pos == 5 || last_curs_pos == 9){
      space_prt(1, 0, 0);
    }
    else if(last_curs_pos == 2 || last_curs_pos == 6 || last_curs_pos == 10){
      space_prt(1, 0, 1);
    }
    else if(last_curs_pos == 3 || last_curs_pos == 7 || last_curs_pos == 11){
      space_prt(1, 0, 2);
    }
    else if(last_curs_pos == 4 || last_curs_pos == 8 || last_curs_pos == 12){
      space_prt(1, 0, 3);
    }
    else if(last_curs_pos == 101 || last_curs_pos == 105 || last_curs_pos == 109){
      space_prt(1, 19, 0);
    }
    else if(last_curs_pos == 102 || last_curs_pos == 106 || last_curs_pos == 110){
      space_prt(1, 19, 1);
    }
    else if(last_curs_pos == 103 || last_curs_pos == 107 || last_curs_pos == 111){
      space_prt(1, 19, 2);
    }
    else if(last_curs_pos == 104 || last_curs_pos == 108 || last_curs_pos == 112){
      space_prt(1, 19, 3);
    }
    
    if(cursor_pos == 1 || cursor_pos == 5 || cursor_pos == 9){
      lcd.setCursor(0, 0);
      lcd.print(char(CURSOR_L));
    }
    else if(cursor_pos == 2 || cursor_pos == 6 || cursor_pos == 10){
      lcd.setCursor(0, 1);
      lcd.print(char(CURSOR_L));
    }
    else if(cursor_pos == 3 || cursor_pos == 7 || cursor_pos == 11){
      lcd.setCursor(0, 2);
      lcd.print(char(CURSOR_L));
    }
    else if(cursor_pos == 4 || cursor_pos == 8 || cursor_pos == 12){
      lcd.setCursor(0, 3);
      lcd.print(char(CURSOR_L));
    }
    else if(cursor_pos == 101 || cursor_pos == 105 || cursor_pos == 109){
      lcd.setCursor(19, 0);
      lcd.print(char(CURSOR_R));
    }
    else if(cursor_pos == 102 || cursor_pos == 106 || cursor_pos == 110){
      lcd.setCursor(19, 1);
      lcd.print(char(CURSOR_R));
    }
    else if(cursor_pos == 103 || cursor_pos == 107 || cursor_pos == 111){
      lcd.setCursor(19, 2);
      lcd.print(char(CURSOR_R));
    }
    else if(cursor_pos == 104 || cursor_pos == 108 || cursor_pos == 112){
      lcd.setCursor(19, 3);
      lcd.print(char(CURSOR_R));
    }
    last_curs_pos = cursor_pos;
  }
}

void opt_up(){
  opt.s1_c1 = opt_fnd(1); opt.s1_c2 = opt_fnd(101);
  opt.s2_c1 = opt_fnd(2); opt.s2_c2 = opt_fnd(102);
  opt.s3_c1 = opt_fnd(3); opt.s3_c2 = opt_fnd(103);
  opt.s4_c1 = opt_fnd(4); opt.s4_c2 = opt_fnd(104);
  cursor_pos = 1;
  cursor_prt();
}

// void display_blinking(bool ON){
//   static uint8_t param_d;
//   static bool dir;
//   static bool fsf;

//   if(ON){
//     switch (Display_mode.d_param){
//       case 0:
//         if(!fsf){
//           fsf = true;
//           disp_blink.start();
//           disp_blink.reset();
//           dir = true;
//         }
//         if(disp_blink.isReady()) {
//           dir ? param_d = LCD_BRIGHT_MAX : param_d = LCD_BRIGHT_MAX - 220;
//           dir = !dir;
//           brightnessRef(param_d, param_d, LED_brs_day.param, LED_brs_night.param);
//         }
//         break;
//       case 1:
//         if(!fsf){
//           fsf = true;
//           disp_pulse.start();
//           disp_pulse.reset();
//         }

//         if(disp_pulse.isReady()){
//           dir ? param_d += 5 : param_d -= 5;
//           (param_d <= 5) ? dir = true : ((param_d >= 250) ? dir = false : false);
//           brightnessRef(param_d, param_d, LED_brs_day.param, LED_brs_night.param);
//         }
//         break;
//       default:
//         break;
//     }
//   }
//     else {
//       switch (Display_mode.d_param){
//         case 0:
//           fsf = false;
//           disp_blink.stop();
//           break;
//         case 1:
//           fsf = false;
//           disp_pulse.stop();
//           break;
//         default:
//           break;
//       }
//     }
// }

void time_date_set(){
  static bool fsf;
  bool draw_flag;
  static uint8_t set_param;
  if(!fsf){
    t_d_set.year = now.year();
    t_d_set.month = now.month();
    t_d_set.day = now.day();
    t_d_set.hrs = hrs;
    t_d_set.mins = mins;
    fsf = true;
    draw_flag = true;
    set_param = 1;

    drawDown_param.stop();
    drawUp_param.start();
    drawUp_param.reset();

    lcd.clear();
    draw_time_date_set(t_d_set, draw_flag, set_param);
  }

  if(enc.isRight()){
    switch (set_param){
    case 1:
      (t_d_set.hrs >= 23) ? t_d_set.hrs = 0 : ++t_d_set.hrs;
      break;
    case 2:
      (t_d_set.mins >= 59) ? t_d_set.mins = 0 : ++t_d_set.mins;
      break;
    case 3:
      (t_d_set.day >= 31) ? t_d_set.day = 1 : ++t_d_set.day;
      break;
    case 4:
      (t_d_set.month >= 12) ? t_d_set.month = 0 : ++t_d_set.month;
      break;
    case 5:
      (t_d_set.year >= 2099) ? t_d_set.year = 2022 : ++t_d_set.year;
      break;
    default:
      break;
    }
    backToMain.reset();
    draw_time_date_set(t_d_set, true, set_param);
  } 

  if(enc.isLeft()){
    switch (set_param){
    case 1:
      (t_d_set.hrs > 0) ? --t_d_set.hrs : t_d_set.hrs = 23;
      break;
    case 2:
      (t_d_set.mins > 0) ? --t_d_set.mins : t_d_set.mins = 59;
      break;
    case 3:
      (t_d_set.day > 1) ? --t_d_set.day : t_d_set.day = 31;
      break;
    case 4:
      (t_d_set.month > 1) ? --t_d_set.month : t_d_set.month = 12;
      break;
    case 5:
      (t_d_set.year > 2022) ? --t_d_set.year : t_d_set.year = 2099;
      break;
    default:
      break;
    }
    backToMain.reset();
    draw_time_date_set(t_d_set, true, set_param);
  } 

  if(enc.isClick()) {
    set_param == 5 ? set_param = 1 : set_param++;
    draw_time_date_set(t_d_set, true, set_param);
    backToMain.reset();
  }

  if(Enc_IsDouble()) {
    drawDown_param.stop();
    drawUp_param.stop();
    enc.resetStates();
    time_date.param = false;
    firstStartFlag = false;
    fsf = false;
    backToMain.reset();
    lcd.clear();
    cursor_pos = 9;
    opt_prt(opt);
  }

  if(drawUp_param.isReady()){
    drawUp_param.stop();
    drawDown_param.start();
    drawDown_param.reset();
    draw_flag = true;
    draw_time_date_set(t_d_set, draw_flag, set_param);
  }

  if(drawDown_param.isReady()){
    drawDown_param.stop();
    drawUp_param.start();
    drawUp_param.reset();
    draw_flag = false;
    draw_time_date_set(t_d_set, draw_flag, set_param);
  }

  if(backToMain.isReady() || enc.isHolded()){
    drawDown_param.stop();
    drawUp_param.stop();
    enc.resetStates();
    time_date.param = false;
    fsf = false;
    firstStartFlag = false;
    backToMain.reset();
    rtc.adjust(DateTime(t_d_set.year, t_d_set.month, t_d_set.day, t_d_set.hrs, t_d_set.mins));
    get_time();
    lcd.clear();
    cursor_pos = 9;
    opt_prt(opt);
  }
}

void draw_time_date_set(time_set t_d, bool df, uint8_t param){
  if(param == 1){
    if(df){
        space_prt(7, 0, 0);
        space_prt(7, 0, 1);
        drawDig(t_d.hrs / 10, 0, 0);
        drawDig(t_d.hrs % 10, 4, 0);
    }
      else{
        space_prt(7, 0, 0);
        space_prt(7, 0, 1);
      }
  }
    else{
        space_prt(7, 0, 0);
        space_prt(7, 0, 1);
        drawDig(t_d.hrs / 10, 0, 0);
        drawDig(t_d.hrs % 10, 4, 0);
    }

  if(param == 2){
    if(df){
        space_prt(7, 8, 0);
        space_prt(7, 8, 1);
        drawDig(t_d.mins / 10, 8, 0);
        drawDig(t_d.mins % 10, 12, 0);
    }
      else{
        space_prt(7, 8, 0);
        space_prt(7, 8, 1);
      }
  }
    else{
        space_prt(7, 8, 0);
        space_prt(7, 8, 1);
        drawDig(t_d.mins / 10, 8, 0);
        drawDig(t_d.mins % 10, 12, 0);
    }

  if(param == 3){
    if(df){
        space_prt(2, 15, 0);
        t_d.day < 10 ? lcd.print(0) : false;
        lcd.print(t_d.day);
    }
      else{
        space_prt(2, 15, 0);
      }
  }
    else{
        space_prt(2, 15, 0);
        t_d.day < 10 ? lcd.print(0) : false;
        lcd.print(t_d.day);
    }

  if(param == 4){
    if(df){
        space_prt(2, 18, 0);
        t_d.month < 10 ? lcd.print(0) : false;
        lcd.print(t_d.month);
    }
      else{
        space_prt(2, 18, 0);
      }
  }
    else{
        space_prt(2, 18, 0);
        t_d.month < 10 ? lcd.print(0) : false;
        lcd.print(t_d.month);
    }

  if(param == 5){
    if(df){
        space_prt(4, 16, 1);
        lcd.print(t_d.year);
    }
      else{
        space_prt(4, 16, 1);
      }
  }
    else{
        space_prt(4, 16, 1);
        lcd.print(t_d.year);
    }

  drawDots(7, 0, true);
  lcd.setCursor(17, 0); lcd.print(".");
}

#endif