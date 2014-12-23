#include "mcs51/cc2530.h"
#include "basicrf/basic_rf.h"
#include "hal_rf.h"
#include "hal_uart.h"
#include <stdio.h>

#define PACKET_LENGTH 64

enum Error {
	OK,
	ERR_TIMEOUT,
	ERR_DATA_CORRUPT
};

static basicRfCfg_t cfg;
uint8 packet_payload[PACKET_LENGTH];
uint8 packet_length;

void hal_init_uart(void)
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

void init(void)
{
	P1DIR |= 0x3;	
	cfg.myAddr = SELF;
	cfg.panId = 1;
	cfg.channel = 25;
	cfg.ackRequest = TRUE;
	halRfInit();
	basicRfInit(&cfg);

	hal_init_uart();
	printf("Hello\n\r");
}

void bsp_led_blue(unsigned char on)
{
	P1_0 = !on;
}

void bsp_led_red(unsigned char on)
{
	P1_1 = !on;
}

void halAssertHandler(void)
{
	bsp_led_red(1);
}

void payload_init(void)
{
	uint8 i;
	packet_length = PACKET_LENGTH;
	for(i = 0; i < PACKET_LENGTH; i++) {
		packet_payload[i] = i;
	}
}

void send_ping(void)
{
	uint8 err;

	bsp_led_blue(1);
	payload_init();
	basicRfReceiveOff();
	err = basicRfSendPacket(TARGET, packet_payload, PACKET_LENGTH);
	printf("send err = %d\n\r", err);
	bsp_led_blue(0);
}

enum Error payload_check(void)
{
	uint8 i;
	for(i = 0; i < packet_length; i++) {
		if ( i != packet_payload[i])
			return ERR_DATA_CORRUPT;
	}
	return SUCCESS;
}

void recv_ping(void)
{
	int16 rssi;

	printf("recv1\n\r");
	basicRfReceiveOn();
	printf("recv2\n\r");
	while(!basicRfPacketIsReady()) {__asm__("NOP");}
	printf("recv3\n\r");
	packet_length = basicRfReceive(packet_payload, PACKET_LENGTH, &rssi);
	printf("recv4\n\r");
	bsp_led_red(payload_check());
	printf("recv5\n\r");
	basicRfReceiveOff();
	printf("recv6\n\r");
}

void main(void) {
	init();
	while(1) {
		send_ping();
		recv_ping();
	}
}
