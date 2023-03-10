#ifndef OCTOPUS_H
#define OCTOPUS_H

#include <time.h>
#include <ESP8266WiFi.h>
#include "GoN_LED.h"

#define MY_TZ "GMT0BST,M3.5.0/1,M10.5.0" // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

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
public:
  static const uint16_t port;
  static const char *   host;
  static const char *   path;
  struct Averages averages;
  struct TimeSlot priceData[24 * 2 * 2]; // 2 days of half hourly slots
  OctopusAPI();	
  void WiFiOn ();
  void WiFiOff ();
  void setClock();  
  bool CalculateSwitching();
};

#endif
