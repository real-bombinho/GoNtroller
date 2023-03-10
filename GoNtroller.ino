#include "octopus.h"
#include "ModBusRelay.h"
#include "NTC.h"
#include "GoN_Relay.h"

#ifndef STASSID
  #define STASSID "YourSSID"
  #define STAPSK  "YourPassword"
#endif

#ifndef GoN_LED_H
  #define LED_RED   5 // PIN_D1
  #define LED_GREEN 4 // PIN_D2
#endif

#define RELAY_1   12 // PIN_D6
#define RELAY_2   13 // PIN_D7

/////////// Needs work at ModBusRelay ////////////////////////
// settings doubled up in there

#define RXPin     14 // PIN_D5  // Serial Receive pin (ModBus) 
#define TXPin      2 // PIN_D4  // Serial Transmit pin (ModBus)

//////////////////////////////////////////////////////////////

#define sOK                0x00
#define sLowTemperature    0x01
#define sMissingThermistor 0x01 << 1
#define sMissingBattery    0x01 << 2
#define MinimumTemperature 34 // degC minimum cylinder temperature
#define MinimumTime 5*60      // prevents rapid switching
#define Hysteresis 1          // defines the total hysteresis, used is 1/2 up and 1/2 down

Relay relay1(RELAY_1);
Relay relay2(RELAY_2);

const char *ssid = STASSID;
const char *pass = STAPSK;

OctopusAPI octopus;
time_t lastTimeFetched = 0;
time_t lastBatteryCheck = 0;
uint8_t g_Status = sOK;
uint8_t randomStartDelay = 0;

//SoftwareSerial S(RXPin, TXPin);
MB_Relay MBR(RXPin, TXPin, 3);  

void blink(uint8_t led, int onTime, int offTime, int repetitions) {
  for (int i = 0; i < repetitions; i++) {
    digitalWrite(led, LOW);
    delay(onTime);
    digitalWrite(led, HIGH);
    delay(offTime);  
  }
}

void setup() {
  bool limp = false;
  randomSeed(analogRead(A0));
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

  for (int i = mbRelay1; i< mbRelayNumber; i++) {
    MBR.switchRelay(i, Relay_Close);
  }
  
  srand(time(nullptr));          // set up randomStartDelay
  randomStartDelay = rand() % 6;  
}

void loop() {
  struct tm timeinfo;
  // put your main code here, to run repeatedly:
  time_t now = time(nullptr);
  if (now > (lastTimeFetched + (24 * 60 * 60))) {
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

// midnight  
  
  if ((tc < 5) && (now > (lastStatusSent + (30*60)))) {
    randomStartDelay = rand() % 6;  // set random delay figure once per 24h
  }
  
// check temperature and set state accordingly //////////////////////

  if (cylinderTemperature == -400) {
    g_Status = g_Status | sMissingThermistor;  // set missing thermistor state
  }
  else {
    g_Status = g_Status & (0xFF - sMissingThermistor); // undo missing thermistor
    if ((cylinderTemperature < (MinimumTemperature - 0.5)) &&    
    (temperature() < MinimumTemperature - (Hysteresis/2))) { // double check
       g_Status = g_Status | sLowTemperature;          // set low temperature state
    }
    else {
      if ((cylinderTemperature > (MinimumTemperature + (Hysteresis/2))) &&
      (g_Status == sLowTemperature)) {
        g_Status = g_Status & (0xFF - sLowTemperature); // undo low temperature
      }      
    }
       
  }

// check on modbus device
  
  if ((now < lastBatteryCheck) || (now > (lastBatteryCheck + (1 * 60) ))) {
    
    if ( !MBR.isPresent() ) {  // checks for presence and restores state if changed
      g_Status = g_Status | sMissingBattery;
      Serial.printf("Battery missing \n");
    }  
    else g_Status = g_Status & (~sMissingBattery);
    lastBatteryCheck = now;
  }

  
// Battery charge + discharge ///////////////////////////////////////
  
  if ((tc >  (0*60+30)) && (tc < (4*60 + 30))) {  // >0.30 & < 4.30
    MBR.switchRelay(mbRelay1, Relay_Open);  // switch on battery charger
  }
  else {
    MBR.switchRelay(mbRelay1, Relay_Close);
  }
  if (((tc > (8*60+00)) && (tc < (11*60 + 30)))     // >16.00 & < 23.30
  || ((tc > (16*60+00)) && (tc < (23*60 + 30)))) {  // >16.00 & < 23.30
    MBR.switchRelay(mbRelay2, Relay_Open);  // switch on inverter
  }
  else {
    MBR.switchRelay(mbRelay2, Relay_Close);
  }
  
// Cylinder bottom group of elements ////////////////////////////////
  
  if ((tc >  (0*60+31 + randomStartDelay)) && (tc < (4*60 + 30))) {  // >0.30 & < 4.30
    relay2.setState(LOW);  // switch on main load cylinder
  }
  else {
    relay2.setState(HIGH); // off
  }

// Cylinder top element /////////////////////////////////////////////
  
  if ((tc > (0*60+30)) && (tc < (4*60 + 30))) {  // >0.30 & < 4.30
    relay1.setState(LOW);  // on
  }
  else {
    if ((g_Status & sMissingThermistor) != 0 ) {
      relay1.setState(HIGH); // off
    }
    else
    if (now > (relay1.lastSwitch() + MinimumTime)) {
      if (g_Status == sLowTemperature) {
        if (relay1.setState(LOW)) { // on
          Serial.printf("l Cylinder temperature: %f degC\n", cylinderTemperature);
        }       
      }
      else {      
        if (relay1.setState(HIGH)) { // off
          Serial.printf("h Cylinder temperature: %f degC\n", cylinderTemperature);
        }  
      }
    }  
  }

// Bathroom heating demand ///////////////////////////////////////
  
  if (((tc > (0*60+25)) && (tc < (0*60 + 36))) ||  // >0.25 & < 0.36
  ((tc > (1*60+30)) && (tc < (1*60 + 35))) ||      // >1.30 & < 1.35
  ((tc > (2*60+30)) && (tc < (2*60 + 35))) ||      // >2.30 & < 2.35
  ((tc > (3*60+30)) && (tc < (3*60 + 35)))) {      // >3.30 & < 3.35
    MBR.switchRelay(mbRelay3, Relay_Open);  // switch on pump
  }
  else {
    MBR.switchRelay(mbRelay3, Relay_Close);
  }
  
// Status LED signing
  
  if ((g_Status & sMissingThermistor) != 0) {
    blink(LED_RED, 200, 100, 1);
    blink(LED_GREEN, 100, 100, 2);
    delay(4600);
  } 
  if ((g_Status & sMissingBattery) != 0) {
    blink(LED_RED, 200, 100, 2);
    blink(LED_GREEN, 200, 100, 1);
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
