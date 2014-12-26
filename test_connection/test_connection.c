#include "hal_cc2530.h"
#include <stdio.h>

/* PACKET_LENGTH must be < 128-4=124*/
#define PACKET_LENGTH 64
__xdata char packet[PACKET_LENGTH];

void show_status(void)
{
	printf("FSMSTAT0(FSM) = 0x%X FSMSTAT1 = 0x%X\n\r", FSMSTAT0, FSMSTAT1);
//	printf("RSSI = %X RSSISTAT = %X\n\r", RSSI, RSSISTAT);
}

// enum HalError send_packet(void)
// {
// 	return Hal_Rf_Send(void *payload, unsigned short payload_len);
// 
// 	unsigned char i;
// 	halRfReceiveOn();
// 	halRfWaitTransceiverReady();
// 
// 	hal_cmd2rf(CSP_ISFLUSHTX);
// 	RFD = PACKET_LENGTH;
// 	for(i = 0; i < PACKET_LENGTH; i++)
// 		RFD = i; 
// 	hal_cmd2rf(CSP_ISTXON);
// 	/* wait TX finished */
// 	RFIRQF1 = 0;
// 	while(!(RFIRQF1 & 0x02)) {
// 		__asm__("NOP");
// 	}
// }
// 
// enum Error recv_packet(void)
// {
// 	while(!(RFIRQF0 & 0x40)) {
// 		__asm__("NOP");
// 	}
// 	printf("YRA!!!\n\r");
// 	RFIRQF0 = ~0x40;
// 	return OK;
// }

void init(void)
{
	Hal_Init(1, 1, SELF_ADDR);

	// RFIRQM0 = 0xFF; //0x60;
	// IEN2 = 0x01;
	// EA = 1;

#ifdef RECV
	// RXENABLE = 0xFF;
	// FRMFILT0 = 0x0C;
	// hal_cmd2rf(CSP_ISFLUSHRX);
	// hal_cmd2rf(CSP_ISRXON);
	// hal_cmd2rf(CSP_ISSAMPLECCA);
#endif
}

void main(void) {
	enum HalError hal_err;
	uint8_t received;

	init();
	while(1) {
		// show_status();
		Hal_LedBlue(1);
		hal_err = Hal_Rf_Send(packet, PACKET_LENGTH);
		printf("send status:%d\n\r", hal_err);
		Hal_LedBlue(0);
		hal_err = Hal_Rf_Read(packet, PACKET_LENGTH, &received);
		printf("recv status:%d received:%d\n\r", hal_err, received);
		Hal_LedRed(OK != hal_err);
	}
}

