#ifndef alarm_h
#define alarm_h

#include <Arduino.h>
// #define BUZZER A7
// const byte speaker_pin = A7;

class alarm {
  
  public:
    alarm();
    void alarm_set(uint8_t hour, uint8_t minute, uint8_t sound, bool status);
    uint8_t get_wakeHour();
    uint8_t get_wakeMinute();
    uint8_t get_wakeSound();
    bool get_wakeStatus();
    bool checkAlarm(uint8_t hour, uint8_t minute);
    uint8_t wakeUP_Sound();
    void stop();
    void rst();
    bool isRunning();
    void getEEPROM(uint8_t h_addr, uint8_t m_addr, uint8_t snd_addr, uint8_t st_addr);
    void putEEPROM(uint8_t h_addr, uint8_t m_addr, uint8_t snd_addr, uint8_t st_addr);

  private:
    uint8_t wakeHour;
    uint8_t wakeMinute;
    uint8_t wakeSound;
    bool wakeStatus = false;
    bool state = false;
};

#include <EEPROM.h>

#endif