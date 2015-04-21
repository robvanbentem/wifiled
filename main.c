#include "config.h"

#include <xc.h>
#include <pic18f13k50.h>

#include "ws2812b.h"

#include <string.h>
#include <stdio.h>

unsigned short delay_counter;

void delay(unsigned short ms) {
    for (delay_counter = 0; delay_counter < ms; delay_counter++) {
        __delay_ms(1);
    }
}
#define LED PORTBbits.RB4
#define MAXBUF 255

volatile unsigned char t;
volatile unsigned short rxindex;
volatile unsigned char rxbuff[MAXBUF];
volatile unsigned char rxstor = 0;

volatile unsigned volatile char newline = 1;

void write_eeprom() {
    for (int i = 0; i < MAXBUF; i++) {
        eeprom_write(i, rxbuff[i]);
    }
}

void USART_putc(unsigned char c) {
    while (!TXSTAbits.TRMT); // wait until transmit shift register is empty
    TXREG = c; // write character to TXREG and start transmission

}

void USART_puts(unsigned char *s) {
    while (*s) {
        USART_putc(*s); // send character pointed to by s
        s++; // increase pointer location to the next character
    }
}

void clear_buff() {
    for (unsigned short i = 0; i < rxindex; i++) {
        rxbuff[i] = '\0';
    }
}

void setup() {
    ANSEL = 0;
    ANSELH = 0;
    TRISB = 0;
    TRISC = 0;
    PORTB = 0;
}

void setup_usart() {
    TXSTAbits.TXEN = 1; // enable transmitter
    TXSTAbits.BRGH = 1; // high baud rate mode
    RCSTAbits.CREN = 1; // enable continous receiving

    // configure I/O pins
    TRISBbits.RB5 = 1; // RX pin is input
    TRISBbits.RB7 = 1; // TX pin is input (automatically configured)

    SPBRG = 12; // set baud rate to 115200 baud (48MHz/(16*baudrate))-1

    PIE1bits.RCIE = 1; // enable USART receive interrupt
    RCSTAbits.SPEN = 1; // enable USART
}

void make_test() {
    char data[] = "AT\r";
    USART_puts(&data);
}

void set_mux_off() {
    char data[] = "AT+CIPMUX=0\r";
    USART_puts(&data);
}

void set_mux_on() {
    char data[] = "AT+CIPMUX=1\r";
    USART_puts(&data);
}

void make_req() {
    char data[] = "AT+CIPSTART=\"TCP\",\"192.168.1.2\",4444\r";
    USART_puts(&data);
}

void make_send() {
    char data[] = "AT+CIPSEND=8\r\nABCD1234\r";
    USART_puts(&data);
}

void get_ip() {
    char data[] = "AT+CIFSR\r\n";
    USART_puts(&data);
}

void start_server() {
    char data[] = "AT+CIPSERVER=1,6666\r";
    USART_puts(&data);
}

void get_gmr() {
    char data[] = "AT+GMR\r";
    USART_puts(&data);
}

void set_update() {
    char data[] = "AT+CIUPDATE\r";
    USART_puts(&data);
}

void echo_on() {
    char data[] = "ATE1\r";
    USART_puts(&data);
}

void echo_off() {
    char data[] = "ATE0\r";
    USART_puts(&data);
}

void handle_response() {
    INTCONbits.GIE = 0;
    if (strncmp(rxbuff, "+IPD", 4) == 0) {

        char *e;
        short idx;

        e = strchr(rxbuff, ':');
        idx = (short)(e - rxbuff) + 1;

        for(unsigned char i = idx; i < rxindex; i++){
            ws_write_byte(rxbuff[i]);
        }
        
    } else if (strncmp(rxbuff, "0,CLOSED", 8) == 0) {
        start_server();
    }

    //clear_buff();
    rxindex = 0;
    INTCONbits.GIE = 1;
}

void rx_read_byte() {
    while (!PIR1bits.RCIF);
    t = RCREG;
}

void rx_show_leds() {
    for (char n = 0; n < 24; n++) {
        rx_read_byte();
        rxbuff[n] = t;
    }
    for (char n = 0; n < 24; n++) {
        ws_write_byte(rxbuff[n]);
    }

}

void rx_data() {

    // disable interrupt
    INTCONbits.GIE = 0;

    // read until data
    while (t != 0x3A) {
        rx_read_byte();
    }

    // read the command byte
    rx_read_byte();

    // show leds
    if (t == 0x10) {
        rx_show_leds();
    }

    // enable interrupt
    INTCONbits.GIE = 1;
}

void interrupt rx() {
    if (PIR1bits.RCIF == 1) {

        if (RCSTAbits.FERR == 1) { // framing error
            t = RCREG;
            return;
        }

        if (RCSTAbits.OERR == 1) { // overun error
            RCSTAbits.CREN = 0; // clear overrun if yes
            RCSTAbits.CREN = 1;
            return;
        }

        rx_read_byte();

        if (t == '\n') { // end of response line
            //handle_response();
            newline = 1;
            rxindex = 0;
        } else {

            if (newline == 1) {
                newline = 0;
                if (t == '+') {
                    rx_data();
                }
            }
        }
    }
}

int main() {
    setup();
    delay(5000);

    setup_usart();
    delay(10);

    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;



    echo_off();
    delay(10);

    set_mux_on();
    delay(10);

    start_server();

    LED = 1;

    while (1) {
        //        delay(500);
        //
        //        if(listen == 0){
        //            write_eeprom();
        //            start_server();
        //
        //            LED = 1;
        //        }
    };

}

