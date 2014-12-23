#include "hal_cc2530.h"

static uint8 txMpdu[128];
static uint8 rxMpdu[128];


static void hal_init_rf(void)
{
        // 23.15.1 Recommended RX settings 
        TXFILTCFG = 0x09;
        AGCCTRL1 = 0x15;
        FSCAL1 = 0x00;

        FRMCTRL0 = 0x60;
	FRMCTRL1 = 0x1F;
	TXPOWER  = 0xD5;
	FRMFILT0 = 0x0C;
}

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

	hal_init_rf();
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

void halRfReceiveOn()
{
	hal_cmd2rf(CSP_ISFLUSHRX);
	hal_cmd2rf(CSP_ISRXON);
}

void halRfWaitTransceiverReady(void)
{
	// Wait for SFD not active and TX_Active not active
	while (FSMSTAT1 & (BV(1) | BV(5))) {
		__asm__("NOP");
	}
}

static uint8 basicRfBuildMpdu(uint16 destAddr, uint8* pPayload, uint8 payloadLength)
{
	uint8 hdrLength, n;

	hdrLength = basicRfBuildHeader(txMpdu, destAddr, payloadLength);

	for(n=0;n<payloadLength;n++)
	{
		txMpdu[hdrLength+n] = pPayload[n];
	}
	return hdrLength + payloadLength; // total mpdu length
}


