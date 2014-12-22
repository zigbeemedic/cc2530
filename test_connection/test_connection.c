#include "hal_cc2530.h"
#include <stdio.h>

/* PACKET_LENGTH must be < 128-4=124*/
#define PACKET_LENGTH 100
#define TIMEOUT 1 << 20

enum Error {
	OK,
	ERR_TIMEOUT,
	ERR_DATA_CORRUPT
};

void show_status(void)
{
	printf("FSMSTAT0(FSM) = 0x%X FSMSTAT1 = 0x%X\n\r", FSMSTAT0, FSMSTAT1);
//	printf("RSSI = %X RSSISTAT = %X\n\r", RSSI, RSSISTAT);
}

void send_packet(void)
{
	unsigned char i;
	hal_cmd2rf(CSP_ISFLUSHTX);
	RFD = PACKET_LENGTH;
	for(i = 0; i < PACKET_LENGTH; i++)
		RFD = i; 
	hal_cmd2rf(CSP_ISTXON);
	/* wait TX finished */
	RFIRQF1 = 0;
	while(!(RFIRQF1 & 0x02)) {
		__asm__("NOP");
	}
}

enum Error recv_packet(void)
{
	while(!(RFIRQF0 & 0x40)) {
		__asm__("NOP");
	}
	printf("YRA!!!\n\r");
	RFIRQF0 = ~0x40;
	return OK;
}

void init(void)
{
	hal_init();
	// RFIRQM0 = 0xFF; //0x60;
	// IEN2 = 0x01;
	// EA = 1;
	SHORTADDRH = 0;
#ifdef SEND
	SHORTADDRL = 1;
#else
	SHORTADDRL = 2;
#endif
	PANIDH = 0;
	PANIDL = 1;

#ifdef RECV
	RXENABLE = 0xFF;
	FRMFILT0 = 0x0C;
	hal_cmd2rf(CSP_ISFLUSHRX);
	hal_cmd2rf(CSP_ISRXON);
	hal_cmd2rf(CSP_ISSAMPLECCA);
#endif

}

void main(void) {
#ifdef RECV
	enum Error err;
#endif

	init();
	while(1) {
		show_status();
#ifdef SEND
		hal_led_blue(1);
		send_packet();
		hal_led_blue(0);
#endif		
#ifdef RECV
		err = recv_packet();
		hal_led_red(OK != err);
#endif
	}
}

