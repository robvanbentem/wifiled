#ifndef _ws2812b_h
#define _ws2812b_h

#include "config.h"
#include <xc.h>

#define WS PORTCbits.RC7

#define WSLO WS = 0;
#define WSHI WS = 1;

#define __delay_100ns(x) _delay((unsigned long)((x)*(_XTAL_FREQ/40000000.0)))

void ws_fast1();

void ws_fast0();

void ws_write_byte(char c);

void ws_write_led(char r, char g, char b, unsigned char count);

void ws_write_led_array(char* rgb, unsigned char count);

unsigned char ws_reverse_byte(char b);

#endif