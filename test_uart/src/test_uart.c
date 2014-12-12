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
	while(!UTX0IF) {;}
}

void init()
{
	U0CSR |= 0x80;
	P0SEL |= 0x0C;
	U0GCR = 8;   /* BAUD_E */
	U0BAUD = 59; /* BAUD_M */
}

void main(void) {
	init();
	while(1) {
		printf("Hello World!\n");
	}
}

