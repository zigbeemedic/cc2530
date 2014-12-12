#include "hal_cc2530.h"

void hal_init(void)
{
	U0CSR |= 0x80;
	/* set 115200 */
	U0GCR |= 11;   /* BAUD_E */
	U0BAUD |= 216; /* BAUD_M */
	P0SEL |= 0x3C;
	CLKCONCMD = 0;
}

void putchar(char c)
{
	U0DBUF = c;
	while(U0CSR & 0x01) {__asm__("NOP");}
}

void hal_cmd2rf(unsigned char cmd)
{
	RFST = cmd;
}

void hal_led_blue(unsigned char on)
{
	P1_0 = !on;
}

void hal_led_red(unsigned char on)
{
	P1_1 = !on;
}



