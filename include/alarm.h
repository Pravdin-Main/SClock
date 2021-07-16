#ifndef alarm_h
#define alarm_h

#include <Arduino.h>
#define BUZZER A7

class alarm {
  
  public:
    alarm();
    void alarm_set(uint8_t hour, uint8_t minute, uint8_t sound, bool status);
    uint8_t get_wakeHour();
    uint8_t get_wakeMinute();
    uint8_t get_wakeSound();
    bool get_wakeStatus();
    bool checkAlarm(uint8_t hour, uint8_t minute);
    void wakeUP();
    void stop();
    bool isRunning();

  private:
    uint8_t wakeHour;
    uint8_t wakeMinute;
    uint8_t wakeSound;
    bool wakeStatus = false;
    bool state = false;
};

#include <QBPlay.h>
// QBPlay* play;

// Тема из Star Wars
const char sound1[] = "MNT150L2O3CGP16L16FEDL2>C<GP16L16FEDL2>C<GP16L16F\
EFL2DP16L16<GGGL2>CGP32L16FEDL2>C<GP16L16FEDL2>C<GP16L16A+AA+L1GL2G.L8<G.\
L16GL4A.L8A>FEDCL16CDEDP16L8<AL4BL8G.L16GL4A.L8A>FEDCGP8L4D.P8L8<G.L16GL4\
A.L8A>FEDCL16CDEDP16L8<AL4BP16L8>G.L16GL8>C.L16<A+L8G+.L16GL8F.L16D+L8D.L\
16CL1GL2G.P16L16GGGL8>CP8L16<CCCL2C.";
// 
// const char sound2[] = "MNT150L2O3CGP16L16FEDL2>C<GP16L16FEDL2>C<GP16L16F\
// EFL2DP16L16<GGGL2>CGP32L16FEDL2>C<GP16L16FEDL2>C<GP16L16A+AA+L1GL2G.L8<G.\
// L16GL4A.L8A>FEDCL16CDEDP16L8<AL4BL8G.L16GL4A.L8A>FEDCGP8L4D.P8L8<G.L16GL4\
// A.L8A>FEDCL16CDEDP16L8<AL4BP16L8>G.L16GL8>C.L16<A+L8G+.L16GL8F.L16D+L8D.L\
// 16CL1GL2G.P16L16GGGL8>CP8L16<CCCL2C.";
// //
// const char sound3[] = "MNT150L2O3CGP16L16FEDL2>C<GP16L16FEDL2>C<GP16L16F\
// EFL2DP16L16<GGGL2>CGP32L16FEDL2>C<GP16L16FEDL2>C<GP16L16A+AA+L1GL2G.L8<G.\
// L16GL4A.L8A>FEDCL16CDEDP16L8<AL4BL8G.L16GL4A.L8A>FEDCGP8L4D.P8L8<G.L16GL4\
// A.L8A>FEDCL16CDEDP16L8<AL4BP16L8>G.L16GL8>C.L16<A+L8G+.L16GL8F.L16D+L8D.L\
// 16CL1GL2G.P16L16GGGL8>CP8L16<CCCL2C.";

const char sound2[] = "F";
const char sound3[] = "F";

#endif