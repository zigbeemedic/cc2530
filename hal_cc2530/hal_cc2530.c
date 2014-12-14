#include "hal_cc2530.h"

void hal_init(void)
{
	U0CSR |= 0x80;
	/* set 115200 */
	U0GCR |= 11;   /* BAUD_E */
	U0BAUD |= 216; /* BAUD_M */
	P0SEL |= 0x3C;
	CLKCONCMD = 0;
	P1DIR |= 0x3;
	hal_cmd2rf(CSP_ISCLEAR);
	hal_led_red(0);
	hal_led_blue(0);

        // Recommended RX settings
        TXFILTCFG = 0x09;
        AGCCTRL1 = 0x15;
        FSCAL1 = 0x00;
        FRMCTRL0 = 0x0;
	FRMCTRL1 = 0x1F;
	TXPOWER  = 0xD5;
	FRMFILT0 = 0x0C;
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



