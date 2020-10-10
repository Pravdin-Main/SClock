/*
 * God_of_Glory_God_of_Light.ino
 * Файл с примером использования библиотеки QBPlay
 * https://en.wikibooks.org/wiki/QBasic/Appendix#PLAY
 *
 * created 10.11.2017
 * modified 12.11.2017
 * with Arduino 1.8.3 (tested on Arduino Uno)
 *
 * Copyright 2017 Vitaliy Fust <aslok.zp@gmail.com>
 *
 * This work is licensed under the MIT License (MIT). To view a copy of this
 * license, visit http://opensource.org/licenses/MIT or send a letter to:
 * Open Source Initiative
 * 855 El Camino Real
 * Ste 13A, #270
 * Palo Alto, CA 94301
 * United States.
 *
 *
 */

#include "QBPlay.h"
QBPlay* play;
const byte speaker_pin = A3;

const char God_of_Glory_God_of_Light[] PROGMEM = "MNT150L4O3EEFGGFEDCCDEL2EDL4EEFGGFEDCCDEL2DL8CL2CDL4ECDL8EFL4ECDL8EFL4EDCDL2<GL4>EEFGGFEDCCDEL16DEDEDEDL8CL2C";

void setup(){
    play = new QBPlay(speaker_pin);
    play->start((__FlashStringHelper*) God_of_Glory_God_of_Light);
}

void loop(){
    play->touch();
}
