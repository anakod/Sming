/*
 * File: Esp SDK Hardware PWM demo
 * Original Author: https://github.com/hrsavla
 *
 * This HardwarePWM library enables Sming framework user to use ESP SDK PWM API
 * Period of PWM is fixed to 1000us / Frequency = 1khz
 * Duty at 100% = 22222. Duty at 0% = 0
 * You can use function setPeriod() to change frequency/period.
 * Calculate the Duty as per the formulae give in ESP8266 SDK
 * Duty = (Period *1000)/45
 *
 * PWM can be generated on up to 8 pins (ie All pins except pin 16)
 * Created on August 17, 2015, 2:27 PM
 *
 * See also ESP8266 Technical Reference, Chapter 12:
 * http://espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf
 */
#include <SmingCore.h>
//#include <HardwarePWM.h>
#include "/opt/sming/Sming/Core/HardwarePWM.h"
//#include "singleton.h"
using namespace HardwarePWM;
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  delay(2000);
  Serial.println("starting");
  Serial.println("========");
  for(int i=1;i<=4;i++){
    Serial.printf("gettig timer %i\n", Timer::instance()->getTimer(0));
  }
  Timer::instance()->freeTimer(0,3);
  Serial.printf("freed timer 3\n");
  Serial.printf("gettig timer %i\n", Timer::instance()->getTimer(0));
  Timer::instance()->freeTimer(0,2);
  Timer::instance()->freeTimer(0,4);
  Serial.printf("freed timer 2 and 4\n");

  for(int i=1;i<=4;i++){
    Serial.printf("gettig timer %i\n", Timer::instance()->getTimer(0));
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}


