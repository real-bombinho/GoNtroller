#ifndef GoN_LED_H
#define GoN_LED_H

#define LED_RED   5 // PIN_D1
#define LED_GREEN 4 // PIN_D2

void blink(uint8_t led, int onTime, int offTime, int repetitions);

#endif
