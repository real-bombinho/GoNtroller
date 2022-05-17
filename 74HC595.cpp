#include "74HC595.h"

//  MIT License
//
//  Copyright (c) 2022 real-bombinho; https://github.com/real-bombinho/GoNtroller/
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.

// important info: https://lastminuteengineers.com/74hc595-shift-register-arduino-tutorial/

chip74HC595::chip74HC595(const int latchP, const int clkP, const int dataP) {
  pin[0] = latchP;
  pin[1] = clkP;
  pin[2] = dataP;
  for (int i = 0; i<3; i++) pinMode(pin[i], OUTPUT);  
  lastErr = c74ok;
  wait = 50;
  rawWrite(0);
}

int chip74HC595::latchPin () {
  return pin[0];
}

int chip74HC595::clockPin () {
  return pin[1];
}

int chip74HC595::dataPin () {
  return pin[2];
}

int chip74HC595::lastError () {
  return lastErr;
}


bool chip74HC595::rawWrite(const uint8_t value) {
  if (rawValue != value) {
    rawValue = value;
    // take the latchPin low so
    // the LEDs don't change while you're sending in bits:
    digitalWrite(pin[0], LOW);
    // shift out the bits:
    shiftOut(pin[2], pin[1], MSBFIRST, 0);
    shiftOut(pin[2], pin[1], MSBFIRST, rawValue);
    //take the latch pin high so the LEDs will light up:
    digitalWrite(pin[0], HIGH);
    delay(wait);
    return true;
  }
  return false;
}

bool chip74HC595::pinWrite(const int pin, const int value) {
  if ((pin < 0) || (pin > 7)) {
    lastErr = c74wrongPin;
    return false;
  }
  if ((value != HIGH) && (value != LOW)) {
    lastErr = c74wrongValue;
    return false;
  }
  uint8_t v = rawValue;
  if (value == HIGH) { bitSet(v, pin);}
  else { bitClear(v, pin);} 
  lastErr = c74ok;
  return rawWrite(v);
}
