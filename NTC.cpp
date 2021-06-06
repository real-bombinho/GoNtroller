#include "NTC.h"

#define AbsoluteZero 273.15
#define TemperatureCorrection 1.99
#define SensorPort 0 // A0

const float VCC = 3.3;             // NodeMCU on board 3.3v vcc
const float R2 = 47000;            // 47k ohm series resistor
const int adc_resolution = 1023;   // 10-bit adc

const double A = 0.000416822;   // thermistor equation parameters
const double B = 0.000262234;   // measured + calculated for 100K sensor
const double C = -0.00000006726371; 


float temperature () {

  float Vout, Rth;
  double temp; 
  uint64_t adc_value = 0;
  int noOfMeasurements = 10;
  for (int i = 0; i < noOfMeasurements; i++) {
    adc_value = adc_value + analogRead(SensorPort);
    delay(1);
  }
  
  Vout = VCC * adc_value / noOfMeasurements / adc_resolution;
  Rth = (VCC * R2 / Vout) - R2;

  if (Rth > 250000) {
    Serial.println("No temperature sensor");
    return -400;
  }
  
//  Steinhart-Hart Thermistor Equation:
//  Temperature in Kelvin = 1 / (A + B[ln(R)] + C[ln(R)]^3)
//  where A = 0.000416822, B = 0.000262234 and C = -0.00000006726371 

  temp = (1 / (A + (B * log(Rth)) + (C * pow((log(Rth)),3))));   // Temperature in kelvin

  return float(temp - AbsoluteZero - TemperatureCorrection);  // Temperature in degC 
}
