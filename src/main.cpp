/*
  Скетч к проекту "Домашняя метеостанция"
  Страница проекта (схемы, описания): https://alexgyver.ru/meteoclock/
  Исходники на GitHub: https://github.com/AlexGyver/MeteoClock
  Нравится, как написан и закомментирован код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver Technologies, 2018
  http://AlexGyver.ru/
*/

/*
  Время и дата устанавливаются атвоматически при загрузке прошивки (такие как на компьютере)
  График всех величин за час и за сутки (усреднённые за каждый час)
  В модуле реального времени стоит батарейка, которая продолжает отсчёт времени после выключения/сброса питания
  Как настроить время на часах. У нас есть возможность автоматически установить время на время загрузки прошивки, поэтому:
	- Ставим настройку RESET_CLOCK на 1
  - Прошиваемся
  - Сразу ставим RESET_CLOCK 0
  - И прошиваемся ещё раз
  - Всё
*/

/* Версия 1.5
  - Добавлено управление яркостью
  - Яркость дисплея и светодиода СО2 меняется на максимальную и минимальную в зависимости от сигнала с фоторезистора
  Подключите датчик (фоторезистор) по схеме. Теперь на экране отладки справа на второй строчке появится величина сигнала
  с фоторезистора.
*/

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

#include "main.h"
//------------------------SETUP-----------------------

void setup() {
  // Serial.begin(9600);

  EEPROM_init();

  pinMode(BACKLIGHT, OUTPUT);
  pinMode(LED_COM, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  setLED(0, 0, 0);

  // digitalWrite(LED_COM, LED_MODE);
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

  draw_main_disp(); // Update time and sensors and display it
}

//----------------------LOOP--------------------------

void loop() {
  if(powerControlTimer.isReady()){
    power_control();
  }

  #if (SENSORS == 1)
    if (sensorsTimer.isReady()) {
      readSensors();             // Read sensors with frequency of SENS_TIME
      // Serial.println("Check sensors is DONE");
  }
  #endif

  #if (ALARM == 1)
    uint8_t alarm_s;
    if (checkAlarm.isReady() && !alarmIs_ON) {                            // проверить соответствие времени будильника текущему  
      alarm_s = alarmControl();
      // Serial.print("Alarm is ON ----  "); Serial.println(alarm_s);
      if(alarm_s != 0) alarmIs_ON = true;
      // Serial.println("Alarm checked");
    }

    if (alarmIs_ON){ 
      alarmStart(alarm_s);
      // Serial.print("Alarm --- "); Serial.println(alarm_s);
      if (Enc_IsDouble()){
        alarmStop();
        // Serial.println("Alarm stopped");
        alarmIs_ON = false;
      }
    }
  #endif     
 
  #if (DISPLAY_TYPE == 1)
    if (clockTimer.isReady()) clockTick();        // два раза в секунду пересчитываем время и мигаем точками

    #if (SENSORS == 1 && GRAPH == 1)
      plotSensorsTick();
    #endif 

    switch (Mode(0)) {
      case 0:
        #if (SENSORS == 1)

          #if (GRAPH == 1)
            if(Mode(0) != 9 && Mode(0) != 10){                         // тут внутри несколько таймеров для пересчёта графиков (за час, за день и прогноз)
            modesTick();
          }
            #else
              brightnessControl();
          #endif

          if (drawSensorsTimer.isReady()) {
            drawSensors();  // обновляем показания датчиков на дисплее с периодом SENS_TIME
            drawFlags();
            // Serial.println("Sensors updated");
          }
        #endif
        #if (ALARM == 1)
          if (Enc_IsDouble() && !alarmIs_ON) Mode(9);
          if(Enc_IsHolded()) Mode(10);
        #else
          if(Enc_IsHolded()) Mode(10);
        #endif
        break;
      case 9:
        #if (ALARM == 1)
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
