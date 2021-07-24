#include "alarm.h"
// QBPlay* play;
// static bool fs_sound_flag;

alarm::alarm(){
  wakeHour = 0;
  wakeMinute = 0;
  wakeSound = 1;
  wakeStatus = false;
  // play = new QBPlay(BUZZER);    
};

void alarm::alarm_set(uint8_t hour, uint8_t minute, uint8_t sound, bool status){
  wakeHour = hour;
  wakeMinute = minute;
  wakeSound = sound;
  wakeStatus = status;
  // play = new QBPlay(speaker_pin);    
};

uint8_t alarm::get_wakeHour(){
  return wakeHour;
}

uint8_t alarm::get_wakeMinute(){
  return wakeMinute;
}

uint8_t alarm::get_wakeSound(){
  return wakeSound;
}

bool alarm::get_wakeStatus(){
  return wakeStatus;
}

bool alarm::checkAlarm(uint8_t hour, uint8_t minute){
  if(wakeHour == hour && wakeMinute == minute && wakeStatus) {
    state = true;
    return true;
  }
    else{
      return false;
    }
}

uint8_t alarm::wakeUP_Sound(){
  return wakeSound;
}

void alarm::stop(){
  wakeStatus = false;
  state = false;
}

bool alarm::isRunning(){
  return state;
}

void alarm::putEEPROM(uint8_t h_addr, uint8_t m_addr, uint8_t snd_addr, uint8_t st_addr){
  EEPROM.update(h_addr, wakeHour);
  EEPROM.update(m_addr, wakeMinute);
  EEPROM.update(snd_addr, wakeSound);
  EEPROM.update(st_addr, wakeStatus);
}

void alarm::getEEPROM(uint8_t h_addr, uint8_t m_addr, uint8_t snd_addr, uint8_t st_addr){
  EEPROM.get(h_addr, wakeHour);
  EEPROM.get(m_addr, wakeMinute);
  EEPROM.get(snd_addr, wakeSound);
  EEPROM.get(st_addr, wakeStatus);
}