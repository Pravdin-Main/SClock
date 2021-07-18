#include "sound.h"

QBPlay* play;
bool fs_sound_flag;
GTimer_ms AutoOFF (10*60*1000);     // 10 min

void play_sound(uint8_t sound){
  if(!fs_sound_flag && sound != 0){
    AutoOFF.start();
    AutoOFF.reset();
    play = new QBPlay(speaker_pin);
    switch (sound) {
      case 1:
        play->start((__FlashStringHelper*) sound1, 1);
        break;
      case 2:
        play->start((__FlashStringHelper*) sound2, 1);
        break;
      case 3:
        play->start((__FlashStringHelper*) sound3, 1);
        break;
      default:
        play->start((__FlashStringHelper*) sound1, 1);
        break;
    }
    fs_sound_flag = true;
  }
  if(sound == 0) {
    fs_sound_flag = false;
    AutoOFF.stop();
  }

  if(!AutoOFF.isReady()){
    play->touch();
  }
}