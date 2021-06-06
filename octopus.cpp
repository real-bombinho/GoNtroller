#include "octopus.h"

extern const char *ssid; 
extern const char *pass;

const char *timeServer1 = "uk.pool.ntp.org";
const char *timeServer2 = "time.nist.gov";

const uint16_t OctopusAPI::port = 443;
const char *   OctopusAPI::host = "api.octopus.energy";
const char *   OctopusAPI::path = "/v1/products/AGILE-18-02-21/electricity-tariffs/E-1R-AGILE-18-02-21-N/standard-unit-rates/";

bool TimeSlot::parse(const char *value) {
  bool result = true;  //"value_exc_vat":9.14,"value_inc_vat":9.597,"valid_from":"2021-02-28T22:30:00Z","valid_to":"2021-02-28T23:00:00Z" 
  char *nl;
  char *eptr;
  nl = strstr( value, "\"value_exc_vat\":");
  if (nl) { 
    nl = &nl[16];
    this->PriceExVAT = strtod(nl, &eptr);
//    Serial.printf("price exc VAT is: %f\n", ts.PriceExVAT);
  } 
  else {
    this->From = 0;
    this->Till = 0;
    this->PriceExVAT = 0;
    this->PriceIncVAT = 0;
    return false;
  }
  
  nl = strstr( eptr, "\"value_inc_vat\":");
  if (nl) { 
    nl = &nl[16];
    PriceIncVAT = strtod(nl, &eptr);
    Serial.printf("price inc VAT is: %f\n", PriceIncVAT);
  } 
  nl = strstr( eptr, "\"valid_from\":\"");
  if (nl) { 
    nl = &nl[14];
    this->From =  parseISO8601(nl); 
    struct tm timeinfo;                     // debug purpose only
    localtime_r(&this->From, &timeinfo);    // localtime_r or gmtime_r
    Serial.print(asctime(&timeinfo));       //
  }
  nl = strstr( eptr, "\"valid_to\":\"");
  if (nl) { 
    nl = &nl[12];
    this->Till = parseISO8601(nl);
    struct tm timeinfo;                     // debug purpose only 
    localtime_r(&this->Till, &timeinfo);       // localtime_r or gmtime_r
    Serial.print(asctime(&timeinfo));       //
  }
  Switches = 0;
  return result; 	
}	

OctopusAPI::OctopusAPI () {
  averages = {0, 0, 0};	
}

void OctopusAPI::setClock() {

  configTime(MY_TZ, timeServer1); // fetches time and daylight saving settings

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

time_t TimeSlot::parseISO8601(const char *value)
{
  if (value[10] == 'T' && value[19] == 'Z') {
  register time_t result;
  #define MONTHSPERYEAR   12      /* months per calendar year */
  static const int cumdays[MONTHSPERYEAR] =
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

    /*@ +matchanyintegral @*/
    
  int year = (value[0] - '0') * 1000 + (value[1] - '0') * 100 + (value[2] - '0') * 10 + (value[3] - '0');
  int month = (value[5] - '0') * 10 + (value[6] - '0') - 1;
  result = (year - 1970) * 365 + cumdays[month % MONTHSPERYEAR];
  result += (year - 1968) / 4;
  result -= (year - 1900) / 100;
  result += (year - 1600) / 400;    
  if ((year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0) &&
    (month % MONTHSPERYEAR) < 2)
      result--;
  int day = (value[8] - '0') * 10 + (value[9] - '0');    
  result += day - 1;
  result *= 24;
  int hour = (value[11] - '0') * 10 + (value[12] - '0');
  result += hour;
  result *= 60;
  int minute = (value[14] - '0') * 10 + (value[15] - '0');
  result += minute;
  result *= 60;
  int second = (value[17] - '0') * 10 + (value[18] - '0');
  result += second;
  
    /*@ -matchanyintegral @*/
    return (result);
  }
  else
    return -1;
}

bool OctopusAPI::CalculateSwitching() {
  bool result = true;
  time_t priorTime = priceData[0].From;
  for (int i = 1; i < 64; i++) {
    if (priorTime != priceData[i].From + (30 * 60) ) {
      //clk.SetStatus(TFT_BLUE);
	  result = false;
    }
    priorTime = priceData[i].From;
  }
  float avgSum = 0;
  for (int i = 0; i < 48; i++) {
    avgSum = avgSum + priceData[i].PriceIncVAT;
  }
//    double/float Avg24h, Avg30d, Avg365d;
  averages.Avg24h = avgSum/48;
  if (averages.Avg30d == 0) {
    averages.Avg30d = averages.Avg24h;
  }
  else {
    averages.Avg30d = ((averages.Avg30d * 30) + averages.Avg24h) / 30.5;
  }
  return result;
}

void OctopusAPI::WiFiOff () {
  WiFi.mode( WIFI_OFF );
  WiFi.forceSleepBegin();
  delay( 10 );
  Serial.println("WiFi off");
}

void OctopusAPI::WiFiOn() {
  delay( 10 );
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Bring up the WiFi connection
  WiFi.persistent(false); 
  WiFi.mode( WIFI_STA );
  WiFi.begin(ssid, pass); 
#ifdef GoN_LED_H
  digitalWrite(LED_GREEN, LOW);
#endif  
  while (WiFi.status() != WL_CONNECTED) {
#ifdef GoN_LED_H
    blink(LED_RED, 333, 333, 5);
#else
    delay(500)    
#endif     
    Serial.print(".");
  }
#ifdef GoN_LED_H
  digitalWrite(LED_GREEN, HIGH);
#endif   
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
