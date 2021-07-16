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

#include "main.h"

//------------------------SETUP-----------------------

void setup() {
  Serial.begin(9600);

  pinMode(BACKLIGHT, OUTPUT);
  pinMode(LED_COM, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  setLED(0);

  // digitalWrite(LED_COM, LED_MODE);
  analogWrite(BACKLIGHT, LCD_BRIGHT_MAX);

//---------------------DEBUG--------------------------------

  // #if (DEBUG == 1)
  //   debug_start();
  // #endif

//---------------------DEBUG END----------------------------

  // if (RESET_CLOCK || rtc.lostPower()) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  display_init();

  inition (); // Взять текущее время и показания с датчиков и вывести на экран

}

//----------------------LOOP--------------------------

void loop() {
  if (brightTimer.isReady() ) checkBrightness();          // яркость
  if (sensorsTimer.isReady() ) readSensors();             // читаем показания датчиков с периодом SENS_TIME

  uint8_t alarm_s;
  if (checkAlarm.isReady() ) {                            // проверить соответствие времени будильника текущему  
    alarm_s = alarmControl();
    if(alarm_s != 0) alarmIs_ON = true;
  }

  if (alarmIs_ON){ 
    alarmStart(alarm_s);
    if (Button_IsDouble()){
      alarmStop();
      alarmIs_ON = false;
    }
  }           
 
  #if (DISPLAY_TYPE == 1)
    if (clockTimer.isReady()) clockTick();        // два раза в секунду пересчитываем время и мигаем точками
    plotSensorsTick();                            // тут внутри несколько таймеров для пересчёта графиков (за час, за день и прогноз)
    modesTick();                                  // тут ловим нажатия на кнопку и переключаем режимы
    if (Mode(0) == 0) {                                  // в режиме "главного экрана"
      if (drawSensorsTimer.isReady()) drawSensors();  // обновляем показания датчиков на дисплее с периодом SENS_TIME
      if (Enc_IsHolded()) Mode(9);
      // if (enc.isHolded() && button.isHold()) mode = 10;
    } 
    if(Mode(0) == 9){
      alarmTuning();
    }
    // if(mode == 10){

    // }
    else {                                          // в любом из графиков
      if (plotTimer.isReady()) redrawPlot();          // перерисовываем график
    }
  #else
    if (drawSensorsTimer.isReady()) drawSensors();
  #endif
}
