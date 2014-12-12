#include "mcs51/cc2530.h"
#include <stdio.h>

enum Error {
	OK,
	ERR_TIMEOUT,
	ERR_DATA_CORRUPT
};

void putchar(char c)
{
	U0DBUF = c;
	while(U0CSR & 0x01) {__asm__("NOP");}
}

void init()
{
	U0CSR |= 0x80;
	U0GCR |= 11;   /* BAUD_E */
	U0BAUD |= 216; /* BAUD_M */
	P0SEL |= 0x3C;
	CLKCONCMD = 0;
}

void main(void) {
	init();
	while(1) {
		printf("Hello, World!\n\r");
	}
}

