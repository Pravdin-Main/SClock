#ifndef alarm_h
#define alarm_h

#include <Arduino.h>
// #define BUZZER A7
// const byte speaker_pin = A7;

#define EEPROM_A 0

class alarm {
  
  public:
    alarm();
    void alarm_set(uint8_t, uint8_t, uint8_t, bool);
    uint8_t get_wakeHour();
    uint8_t get_wakeMinute();
    uint8_t get_wakeSound();
    bool get_wakeStatus();
    bool checkAlarm(uint8_t, uint8_t);
    uint8_t wakeUP_Sound();
    void stop();
    void rst();
    bool isRunning();
    #if (EEPROM_A == 1)
      void getEEPROM(uint8_t, uint8_t, uint8_t, uint8_t);
      void putEEPROM(uint8_t, uint8_t, uint8_t, uint8_t);
    #endif
    void change_param(uint8_t, bool);

  private:
    uint8_t wakeHour;
    uint8_t wakeMinute;
    uint8_t wakeSound;
    bool wakeStatus = false;
    bool state = false;
    // struct atribute {
    //   uint8_t wakeHour;
    //   uint8_t wakeMinute;
    //   uint8_t wakeSound;
    //   bool wakeStatus = false;
    //   bool state = false;
    // };
};

#if (EEPROM_A == 1)
  #include <EEPROM.h>
#endif

#endif