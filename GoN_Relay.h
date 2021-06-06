#ifndef GoN_RELAY_H
#define GoN_RELAY_H

#include <Arduino.h>

struct Relay {
private:
  uint8_t state_;                  // carrries current state
  uint8_t pin_;                    // carries pin of the assigned relay
  time_t lastSwitch_;              // carries time of last state_ change
public:
  Relay (const uint8_t pin);       // constructor, being initialised with pin
  uint8_t state ();                // getter for state_ (HIGH ; LOW)
  bool setState(uint8_t state);    // setter for state_ (HIGH ; LOW), only at change of state
                                   // exits + returns false for values other than HIGH or LOW 
  time_t lastSwitch ();            // getter for time of the last actual change of state_
};

#endif
