#include "GoN_Relay.h"

uint8_t Relay::state() {
  return state_;
}

Relay::Relay (uint8_t pin) {
  pin_ = pin;
  state_ = HIGH;               // initialise in off state
  pinMode(pin, OUTPUT);
  digitalWrite(pin, state_);   // turn relay off (high).
  lastSwitch_ = 0;
}

bool Relay::setState(uint8_t state) {
  if ((state != HIGH) && (state != LOW)) return false; 
  if (state != state_) {
    state_ = state;
    digitalWrite(pin_, state_);
    lastSwitch_ = time(nullptr);
    Serial.printf("Relay (%i) switched ", pin_);
    if (state_ == HIGH) {
      Serial.printf("High\n");
    }
    else {
       Serial.printf("Low\n");
    }
  }
  return true;
}

time_t Relay::lastSwitch () {
  return lastSwitch_;
}
