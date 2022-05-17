#ifndef L74HC595_h
#define L74HC595_h

#include <Arduino.h>

#define c74ok         0x00
#define c74wrongPin   0x01
#define c74wrongValue 0x02

class chip74HC595 {
private:
  int pin[3];
  uint8_t rawValue;
  int lastErr;
  int wait;
public:  
  chip74HC595(const int latchP, const int clkP, const int dataP);
  int latchPin ();
  int clockPin ();
  int dataPin ();
  int lastError ();
  bool rawWrite(const uint8_t value);
  bool pinWrite(const int pin, const int value);
};

#endif  // L74HC595_h
