#include "debugger.h"

void debug_start(){
    #if (DEBUG == 1 && DISPLAY_TYPE == 1)
        Serial.begin(9600);
        boolean status = true;

        setLED(1);

    #if (CO2_SENSOR == 1)
        lcd.setCursor(0, 0);
        lcd.print(F("MHZ-19... "));
        Serial.print(F("MHZ-19... "));
        mhz19.begin(MHZ_TX, MHZ_RX);
        mhz19.setAutoCalibration(false);
        mhz19.getStatus();    // первый запрос, в любом случае возвращает -1
        delay(500);
        if (mhz19.getStatus() == 0) {
            lcd.print(F("OK"));
            Serial.println(F("OK"));
        } else {
            lcd.print(F("ERROR"));
            Serial.println(F("ERROR"));
            status = false;
        }
    #endif

    setLED(2);
    lcd.setCursor(0, 1);
    lcd.print(F("RTC... "));
    Serial.print(F("RTC... "));
    delay(50);
    if(rtc.begin()) {
        lcd.print(F("OK"));
        Serial.println(F("OK"));
    } else {
        lcd.print(F("ERROR"));
        Serial.println(F("ERROR"));
        status = false;
    }

    setLED(3);
    lcd.setCursor(0, 2);
    lcd.print(F("BME280... "));
    Serial.print(F("BME280... "));
    delay(50);
    if(bme.begin()) {
        lcd.print(F("OK"));
        Serial.println(F("OK"));
    } else {
        lcd.print(F("ERROR"));
        Serial.println(F("ERROR"));
        status = false;
    }

    setLED(0);
    lcd.setCursor(0, 3);
    if(status) {
        lcd.print(F("All good"));
        Serial.println(F("All good"));
    } else {
        lcd.print(F("Check wires!"));
        Serial.println(F("Check wires!"));
    }
    while(1) {
        lcd.setCursor(14, 1);
        lcd.print("P:    ");
        lcd.setCursor(16, 1);
        lcd.print(analogRead(PHOTO), 1);
        Serial.println(analogRead(PHOTO));
        delay(300);
    }
#else

    // #if (CO2_SENSOR == 1)
    //     mhz19.begin(MHZ_TX, MHZ_RX);
    //     mhz19.setAutoCalibration(false);
    // #endif

    // rtc.begin();
    // bme.begin();
    // bme.setTempCal(-1);
    /*bme.setSampling(Adafruit_BME280::MODE_FORCED,
        Adafruit_BME280::SAMPLING_X1, // temperature
        Adafruit_BME280::SAMPLING_X1, // pressure
        Adafruit_BME280::SAMPLING_X1, // humidity
        Adafruit_BME280::FILTER_OFF   );*/
#endif

}