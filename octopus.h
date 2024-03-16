#ifndef OCTOPUS_H
#define OCTOPUS_H

#include <time.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
//#include <WiFiClientSecure.h>
#include <sntp.h>
#include "GoN_LED.h"

//#define Agile_Slots 96 

const char MyTZ[] PROGMEM = "GMT0BST,M3.5.0/1,M10.5.0\0"; // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
const char timeServer1[] PROGMEM = "uk.pool.ntp.org";
const char timeServer2[] PROGMEM = "time.nist.gov";

struct Averages {
  float Avg24h, Avg30d, Avg365d;
};

struct TimeSlot {
  float PriceIncVAT;
  float PriceExVAT;
  time_t From;
  time_t Till;
  uint8_t Switches;
  bool parse(const char *value);
  time_t parseISO8601(const char *value);
};

struct OctopusAPI {
private:
  bool WiFiSleeping;
public:
  static const uint16_t port;
  static const char *   host;
  static const char *   path;
  struct Averages averages;
  struct TimeSlot priceData[Agile_Slots]; // 2 days of half hourly slots
  OctopusAPI();	
  void setPortalEnabled(const bool enable);
  void WiFiOn ();
  void WiFiOff ();
  void WiFiReset ();
  bool isWifiOn () { return (!WiFiSleeping); };
  void setClock();  
  bool CalculateSwitching();
};

#endif
