#include "main.h"
//------------------------SETUP-----------------------

void setup() {
  // Serial.begin(9600);

  #if (EEPROM_P == 1)
    EEPROM_init();
  #endif

  pinMode(BACKLIGHT, OUTPUT);
  pinMode(LED_COM, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  setLED(0, 0, 0);

  analogWrite(BACKLIGHT, LCD_BRIGHT_MAX);

  #if (CLOCK ==  1)
    display_init();
    // Serial.println("Display inition is DONE");
  #elif (CLOCK == 2)
  #elif (CLOCK == 3)
  #elif (CLOCK == 4)
  #endif

//---------------------DEBUG--------------------------------

  #if (DEBUG == 1)
    debug_start();
  #endif

//---------------------DEBUG END----------------------------

  #if (SENSORS == 1)
    init_sens();
    // Serial.println("Sensors inition is DONE");
  #endif

  power_control();

  draw_main_disp(); // Update time and sensors and display it
}

//----------------------LOOP--------------------------

void loop() {

  #if (SENSORS == 1)
    if (sensorsTimer.isReady()) {
      readSensors();             // Read sensors with frequency of SENS_TIME
      // Serial.println("Check sensors is DONE");
    }
  #endif

  #if (ALARM >= 1)
    if (checkAlarm.isReady() && alarmIs_ON == 0) {       // проверить соответствие времени будильника текущему  
      alarmIs_ON = alarmControl();
    }

    if (alarmIs_ON != 0){ 
      #if (ALARM == 1)
        alarmStart();
      #else
        alarmStart(alarmIs_ON);
      #endif
      // display_blinking(alarmIs_ON);
      if (BTN_IsHolded()){
        alarmStop();
        alarmIs_ON = 0;
        // display_blinking(alarmIs_ON);
      }
    }
  #endif     
 
  #if (DISPLAY_TYPE == 1)
    if (clockTimer.isReady()) clockTick();        // два раза в секунду пересчитываем время и мигаем точками

    #if (SENSORS == 1 && GRAPH == 1)
      plotSensorsTick();
    #endif 

    switch (Mode(100)) {
      case 0:
        #if (SENSORS == 1)

          #if (GRAPH == 1)
            if(100) != 9 && Mode(100) != 10){                         // тут внутри несколько таймеров для пересчёта графиков (за час, за день и прогноз)
            modesTick();
          }
            #else
              brightnessControl();
          #endif

          if (drawSensorsTimer.isReady()) {
            drawSensors();  // обновляем показания датчиков на дисплее с периодом SENS_TIME
            PowerControlCheck++;
            if(PowerControlCheck == 60){  // Every 10 min check power voltage
              power_control();
              PowerControlCheck = 0;
            }
            drawFlags();
          }
        #else
          #if (POWER_IND == 2)
            power_control();
          #endif
        #endif
        #if (ALARM >= 1)
          if (Enc_IsDouble()) Mode(9);
          if (Enc_IsHolded()) Mode(10);
        #else
          if(Enc_IsHolded()) Mode(10);
        #endif
        break;
      case 9:
        #if (ALARM >= 1)
          alarmTuning();
        #else
          draw_main_disp();
        #endif
        break;
      case 10:
        #if (OPTION == 1)
          options();
        #else
          draw_main_disp();
        #endif
        break;
      default:
        #if (SENSORS == 1 && GRAPH == 1)                                        // в любом из графиков
          if (plotTimer.isReady()) redrawPlot();          // перерисовываем график
        #endif
        break;
      }                                  // в режиме "главного экрана"
     
    #else
      #if (SENSORS == 1)
        if (drawSensorsTimer.isReady()) drawSensors();
      #endif
    #endif
}
