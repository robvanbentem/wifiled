#include "ws2812b.h"

unsigned char color;

void ws_fast1() {
    WSHI;
    color >>= 1;
    __delay_100ns(6);
    WSLO;
}

void ws_fast0() {
    WSHI;
    color >>= 1;
    __delay_100ns(2);
    WSLO;
}

void ws_write_byte(char c) {
    color = c;
    (color & 1) ? ws_fast1() : ws_fast0();
    (color & 1) ? ws_fast1() : ws_fast0();
    (color & 1) ? ws_fast1() : ws_fast0();
    (color & 1) ? ws_fast1() : ws_fast0();
    (color & 1) ? ws_fast1() : ws_fast0();
    (color & 1) ? ws_fast1() : ws_fast0();
    (color & 1) ? ws_fast1() : ws_fast0();
    (color & 1) ? ws_fast1() : ws_fast0();
}

void ws_write_led(char r, char g, char b, unsigned char count) {
    for (unsigned char n = 0; n < count; n++) {
        ws_write_byte(g);
        ws_write_byte(r);
        ws_write_byte(b);
    }
}

void ws_write_led_array(char* rgb, unsigned char count) {
    ws_write_led(rgb[0], rgb[1], rgb[2], count);
}

unsigned char ws_reverse_byte(char b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}