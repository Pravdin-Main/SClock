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

  // if(!fs_sound_flag){
  //   switch (wakeSound) {
  //     case 1:
  //       play->start((__FlashStringHelper*) sound1);
  //       break;
  //     case 2:
  //       play->start((__FlashStringHelper*) sound2);
  //       break;
  //     case 3:
  //       play->start((__FlashStringHelper*) sound3);
  //       break;
  //     default:
  //       play->start((__FlashStringHelper*) sound1);
  //       break;
  //   }
  //   fs_sound_flag = true;
  // }

  // play->touch();

  // if (soundNum > len(soundsArray) {
  //   return
  // }
  // play->start((__FlashStringHelper*) soundsArray[soundNum]); 
}

void alarm::stop(){
  wakeStatus = false;
  state = false;
}

bool alarm::isRunning(){
  return state;
}