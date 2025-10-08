#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    DDRC = 0b1000000;  // configura pino 13 como saída

    while (1) {
        PORTC = 0b1000000; // acende LED
    }
}

