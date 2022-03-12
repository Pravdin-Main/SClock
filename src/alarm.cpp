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

void alarm::rst(){
  wakeMinute = 0;
  wakeHour = 0;
  wakeSound = 0;
  wakeStatus = false;
}

bool alarm::isRunning(){
  return state;
}

#if (EEPROM_A == 1)
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
#endif

void alarm::change_param(uint8_t param, bool dir){
  switch (param){
      case 1:
        if(dir) {
          wakeHour >= 23 ? wakeHour = 0 : ++wakeHour;
        }
          else {
            wakeHour == 0 ? wakeHour = 23 : --wakeHour;
          }
        break;
      case 2:
        if(dir) {
          wakeMinute >= 59 ? wakeMinute = 0 : ++wakeMinute;
        }
          else {
            wakeMinute == 0 ? wakeMinute = 59 : --wakeMinute;
          }
        break;
      case 3:
        if(dir){
          wakeSound >= 3 ? wakeSound = 1 : ++wakeSound;
        }
          else{
            wakeSound == 1 ? wakeSound = 3 : --wakeSound;
          }
        break;
      case 4:
        wakeStatus = !wakeStatus;         
        break;
      default:
        break;
    }
}