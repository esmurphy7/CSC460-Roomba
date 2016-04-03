#include <avr/io.h>
#include "LED_Test.h"
#include <util/delay.h>

void pulse_pin(int count)
{
    for(int i = 0; i < count; i ++) {
        enable_LED(PORTL0);
        _delay_us(250);
        disable_LED(PORTL0);
        _delay_us(250);
    }
}

void init_LED_PORTL_pin0(void) {
    DDRL |= _BV(DDL0);
}

void init_LED_PORTL_pin1(void) {
    DDRL |= _BV(DDL1);
}

void init_LED_PORTL_pin2(void) {
    DDRL |= _BV(DDL2);
}

void init_LED_PORTL_pin5(void) {
    DDRL |= _BV(DDL5);
}

void init_LED_PORTL_pin6(void) {
    DDRL |= _BV(DDL6);
}

void init_LED_PORTL_pin7(void) {
    DDRL |= _BV(DDL7);
}

void enable_LED(unsigned int mask) {
    PORTL |= _BV(mask);
}

void disable_LED(unsigned int mask) {
    PORTL &= ~_BV(mask);
}

void toggle_LED(unsigned int mask) {
    PORTL ^= _BV(mask);
}