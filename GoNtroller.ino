#include "octopus.h"
#include "ModBusRelay.h"
#include "NTC.h"
#include "GoN_Relay.h"

#ifndef STASSID
  #define STASSID "YourSSIDhere"
  #define STAPSK  "YourPasswordHere"
#endif

#ifndef GoN_LED_H
  #define LED_RED   5 // PIN_D1
  #define LED_GREEN 4 // PIN_D2
#endif

#define RELAY_1   12 // PIN_D6
#define RELAY_2   13 // PIN_D7

#define sOK 0
#define sLowTemperature 1
#define sMissingThermistor 1 << 1
#define MinimumTemperature 35 // degC minimum cylinder temperature
#define MinimumTime 5*60      // prevents rapid switching
#define Hysteresis 1          // defines the total hysteresis, used is 1/2 up and 1/2 down

Relay relay1(RELAY_1);
Relay relay2(RELAY_2);

const char *ssid = STASSID;
const char *pass = STAPSK;

OctopusAPI octopus;
time_t lastTimeFetched = 0;
uint8_t g_Status = sOK;

SoftwareSerial S(RXPin, TXPin);
//MB_Relay MBR(&S, 3);  // check if constant writing of address is needed

void blink(uint8_t led, int onTime, int offTime, int repetitions) {
  for (int i = 0; i < repetitions; i++) {
    digitalWrite(led, LOW);
    delay(onTime);
    digitalWrite(led, HIGH);
    delay(offTime);  
  }
}

void setup() {
  Serial.begin(19200);
  Serial.println("Hello!");  

  pinMode(LED_RED, OUTPUT);      // LED pin as output.
  pinMode(LED_GREEN, OUTPUT);    // LED pin as output.
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  octopus.WiFiOn();
  
  octopus.setClock();
  lastTimeFetched = time(nullptr);
  if (lastTimeFetched > 10000) {
    blink(LED_GREEN, 500, 500, 5);
  }
  
  octopus.WiFiOff();

}

void loop() {
  struct tm timeinfo;
  // put your main code here, to run repeatedly:
  time_t now = time(nullptr);
  if (now > (lastTimeFetched + (24 * 60 * 30))) {
    WiFi.forceSleepWake();
    octopus.WiFiOn();
    octopus.setClock();
    lastTimeFetched = time(nullptr);
    blink(LED_GREEN, 500, 500, 5);
    octopus.WiFiOff();
  }
  localtime_r(&now, &timeinfo);
  float cylinderTemperature = temperature();

  int tc = (timeinfo.tm_hour * 60) + timeinfo.tm_min;
  if ((tc >  (0*60+30)) && (tc < (4*60 + 30))) {  // >0.30 & < 4.30
    relay1.setState(LOW);  // on
    delay(5000);
    relay2.setState(LOW); 
  }
  else {
    if (now > (relay1.lastSwitch() + MinimumTime)) {
      if (cylinderTemperature > (MinimumTemperature + (Hysteresis/2))) {
        relay1.setState(HIGH); // off
        g_Status = g_Status & (0xFF - sLowTemperature);
        
        Serial.printf("Cylinder temperature: %f degC\n", cylinderTemperature);
      }
      else       
      if ((cylinderTemperature < (MinimumTemperature - 0.5)) &&
      (temperature() < MinimumTemperature - (Hysteresis/2))) {
        if (cylinderTemperature != -400) {
          relay1.setState(LOW); // on
          g_Status = g_Status | sLowTemperature;
          g_Status = g_Status & (0xFF - sMissingThermistor);
          Serial.printf("Cylinder temperature: %f degC\n", cylinderTemperature);
        }
        else {
           g_Status = g_Status | sMissingThermistor;  
        }
      }
    }  
    relay2.setState(HIGH); // off
  }
  if ((g_Status & sMissingThermistor) != 0) {
    blink(LED_RED, 100, 1, 1);
    blink(LED_GREEN, 100, 100, 2);
    delay(4700);
  }  
  if (g_Status == sOK) {
    blink(LED_GREEN, 100, 5000, 1);
  }
  if (g_Status == sLowTemperature) {
    blink(LED_GREEN, 100, 100, 2);
    delay(4800);
  }
}
