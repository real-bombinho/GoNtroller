#include "GoN_Relay.h"

Relay::Relay (uint8_t pin) {
  pin_ = pin;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);   // turn relay off (low).
  lastSwitch_ = 0;
}

bool Relay::setState(uint8_t state) {
  if (state != getState()) {
    digitalWrite(pin_, state);
    time_t priorSwitch = lastSwitch_;
    lastSwitch_ = time(nullptr);
    static int16_t duration = 0;
    if (priorSwitch != 0) {
      duration = lastSwitch_ - priorSwitch;
      if (duration < 0) { duration = duration * -1; }
    }
    Serial.printf("Relay (pin %i) switched ", pin_);
    delay(10);
    if (state == HIGH) {
      Serial.println("High");
    }
    else {
      Serial.printf("Low\n");
    }
    return true;
  }
  else return false;
}

uint8_t Relay::getState() {
  return digitalRead(pin_);
  delay(1);  
}


time_t Relay::lastSwitch () {
  return lastSwitch_;
}
