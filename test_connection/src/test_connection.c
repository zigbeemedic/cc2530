#include "mcs51/cc2530.h"

/* PACKET_LENGTH must be < 128-4=124*/
#define PACKET_LENGTH 100
#define TIMEOUT 10000

#define CSP_SFLUSHTX  0xDE
#define CSP_ISTXON    0xE9
#define CSP_ISCLEAR   0xFF

enum Error {
	OK,
	ERR_TIMEOUT,
	ERR_DATA_CORRUPT
};

void hal_cmd2rf(unsigned char cmd)
{
	RFST = cmd;
	while(RFST != 0x0) {;}
}

void hal_led_blue(unsigned char on)
{
	P1_0 = !on;
}

void hal_led_red(unsigned char on)
{
	P1_1 = !on;
}

void send_packet(void)
{
	unsigned char i;
	hal_cmd2rf(CSP_SFLUSHTX);
	RFD = PACKET_LENGTH;
	for(i = 0; i < PACKET_LENGTH; i++)
		RFD = i; 
	hal_cmd2rf(CSP_ISTXON);
	/* wait TX fineshed */
	while(!(RFIRQF1 & 0x20)) {;}
}

enum Error recv_packet_timeout(void)
{
	int i;
	unsigned char length, len_i, data;
	hal_cmd2rf(0xE3); //ISRXON
	i = TIMEOUT;
	while(1) {
		if (RFIRQF1 & 0x40) // RXPKTDONE
			break;
		if (0 == i--) {
			return ERR_TIMEOUT;
		}
	}
	length = RFD;
	if (PACKET_LENGTH != length)
		return ERR_DATA_CORRUPT;
	for(len_i = 0; len_i < length; len_i++) {
		data = RFD;
		if(data != len_i)
			return ERR_DATA_CORRUPT;
	}
	return OK;
}

void main(void) {
	enum Error err;

	P1DIR |= 0x3;
	hal_cmd2rf(0xFF); // ISCLEAR
	while(1) {
		hal_led_blue(1);
		send_packet();
		hal_led_blue(0);
		err = recv_packet_timeout();
		hal_led_red(OK != err);
	}
}

