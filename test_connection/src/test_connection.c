#include "mcs51/cc2530.h"
#include <stdio.h>

/* PACKET_LENGTH must be < 128-4=124*/
#define PACKET_LENGTH 100
#define TIMEOUT 32767

#define CSP_SFLUSHTX  0xDE
#define CSP_ISFLUSHTX 0xEE
#define CSP_ISFLUSHRX 0xED
#define CSP_ISTXON    0xE9
#define CSP_ISRXON    0xE3
#define CSP_ISCLEAR   0xFF

#define RFIRQF0_RXMASKZERO      (1 << 7)
#define RFIRQF0_RXPKTDONE       (1 << 6)
#define RFIRQF0_FRAME_ACCEPTED  (1 << 5)
#define RFIRQF0_SRC_MATCH_FOUND (1 << 4)
#define RFIRQF0_SRC_MATCH_DONE  (1 << 3)
#define RFIRQF0_FIFOP           (1 << 2)
#define RFIRQF0_SFD             (1 << 1)
#define RFIRQF0_ACT_UNUSED      (1 << 0)

#define RFIRQF1_CSP_WAIT   (1 << 5)
#define RFIRQF1_CSP_STOP   (1 << 4)
#define RFIRQF1_CSP_MANINT (1 << 3)
#define RFIRQF1_RFIDLE     (1 << 2)
#define RFIRQF1_TXDONE     (1 << 1)
#define RFIRQF1_TXACKDONE  (1 << 0)

#define RFIRQM0_RXMASKZERO      (1 << 7)
#define RFIRQM0_RXPKTDONE       (1 << 6)
#define RFIRQM0_FRAME_ACCEPTED  (1 << 5)
#define RFIRQM0_SRC_MATCH_FOUND (1 << 4)
#define RFIRQM0_SRC_MATCH_DONE  (1 << 3)
#define RFIRQM0_FIFOP           (1 << 2)
#define RFIRQM0_SFD             (1 << 1)
#define RFIRQM0_ACT_UNUSED      (1 << 0)

#define RFERRM_STROBEERR (3 << 6)
#define RFERRM_TXUNDERF  (1 << 5) 
#define RFERRM_TXOVERF   (1 << 4)
#define RFERRM_RXUNDERF  (1 << 3)
#define RFERRM_RXOVERF   (1 << 2)
#define RFERRM_RXABO     (1 << 1)
#define RFERRM_NLOCK     (1 << 0)

#define BAUD_E(baud, clkDivPow) (     \
	(baud==2400)   ? 6  +clkDivPow : \
	(baud==4800)   ? 7  +clkDivPow : \
	(baud==9600)   ? 8  +clkDivPow : \
	(baud==14400)  ? 8  +clkDivPow : \
	(baud==19200)  ? 9  +clkDivPow : \
	(baud==28800)  ? 9  +clkDivPow : \
	(baud==38400)  ? 10 +clkDivPow : \
	(baud==57600)  ? 10 +clkDivPow : \
	(baud==76800)  ? 11 +clkDivPow : \
	(baud==115200) ? 11 +clkDivPow : \
	(baud==153600) ? 12 +clkDivPow : \
	(baud==230400) ? 12 +clkDivPow : \
	0)

#define BAUD_M(baud) (      \
	(baud==2400)   ? 59  : \
	(baud==4800)   ? 59  : \
	(baud==9600)   ? 59  : \
	(baud==14400)  ? 216 : \
	(baud==19200)  ? 59  : \
	(baud==28800)  ? 216 : \
	(baud==38400)  ? 59  : \
	(baud==57600)  ? 216 : \
	(baud==76800)  ? 59  : \
	(baud==115200) ? 216 : \
	(baud==153600) ? 59  : \
	(baud==230400) ? 216 : \
	0)


enum Error {
	OK,
	ERR_TIMEOUT,
	ERR_DATA_CORRUPT
};

enum BSP_Led {
	LED_RED,
	LED_BLUE
};

void hal_cmd2rf(unsigned char cmd)
{
	RFST = cmd;
}

void BSP_LedSwitch(enum BSP_Led led, unsigned char on)
{
	switch(led) {
	case LED_BLUE: 
		P1_0 = !on; 
		break;
	case LED_RED: 
		P1_1 = !on; 
		break;
	};
}

void isr_rf(void) __interrupt(RF_VECTOR) __critical
{
	volatile int i;

	RFIRQF1 = 0x0;
	RFIRQF0 = 0x0;
	BSP_LedSwitch(LED_BLUE, 1);
	for(i = 0; i < 32760; i++) {;}
	BSP_LedSwitch(LED_BLUE, 0);
	for(i = 0; i < 32760; i++) {;}
}

void isr_rferr(void) __interrupt(RFERR_VECTOR) __critical
{
	volatile int i;

	RFERRF = 0x0;
	BSP_LedSwitch(LED_RED, 1);
	for(i = 0; i < 32760; i++) {
		__asm__("nop");
	}
	BSP_LedSwitch(LED_RED, 0);
}

void send_packet(void)
{
	unsigned char i;
	hal_cmd2rf(CSP_ISFLUSHTX);
	RFD = PACKET_LENGTH;
	for(i = 0; i < PACKET_LENGTH; i++)
		RFD = i; 
	hal_cmd2rf(CSP_ISTXON);
	/* wait TX fineshed */
	while(!(RFIRQF1 & RFIRQF1_TXDONE)) {;}
	RFIRQF1 = ~RFIRQF1_TXDONE;
}

enum Error recv_packet_timeout(void)
{
	volatile int i;
/*
	unsigned char length, len_i, data;
*/
	hal_cmd2rf(CSP_ISFLUSHRX);
	hal_cmd2rf(CSP_ISRXON);
	i = TIMEOUT;
	while(1) {
		if (RFIRQF0 & RFIRQF0_RXPKTDONE) {
			RFIRQF0 = ~RFIRQF0_RXPKTDONE;
			break;
		}
		if (0 == i--)
			return ERR_TIMEOUT;
	}
/*
	length = RFD;
	if (PACKET_LENGTH != length)
		return ERR_DATA_CORRUPT;
	for(len_i = 0; len_i < length; len_i++) {
		data = RFD;
		if(data != len_i)
			return ERR_DATA_CORRUPT;
	}
*/
	return OK;
}

void init(void) {
	// int baud = 115200;

	P1DIR |= 0x3;
	hal_cmd2rf(CSP_ISCLEAR);
	FRMCTRL0 |= 0x3 | 0x3 << 2 | 1 << 5 | 1 << 6;
	TXFILTCFG = 0x09;
	AGCCTRL1 = 0x15;
	FSCAL1 = 0x00;
	//RFIRQM1 = 0xFF;
	//RFIRQM1 = RFIRQM1_RXOK;
	//RFIRQM0 = 0xFD;
	//RFERRM = 0xFF;
	//IEN2 |= 0x1;
   	//EA = 1; /* global interrupt enable */

	U0GCR |= 11;
	U0BAUD |= 216;
	U0CSR = 1 << 7 | 1 << 6;

	U1GCR |= 11;
	U1BAUD |= 216;
	U1CSR = 1 << 7 | 1 << 6;
}

void putchar(char c)
{
	BSP_LedSwitch(LED_RED, 1);
	U0DBUF = c;
	U1DBUF = c;
	BSP_LedSwitch(LED_RED, 0);
}

void main(void) {
//	enum Error err;

	init();
	putchar('h');
	while(1) {
		//printf("hello, world\n");
		putchar('h');
		//send_packet();
		//hal_cmd2rf(CSP_ISRXON);
		// err = recv_packet_timeout();
	}
}

