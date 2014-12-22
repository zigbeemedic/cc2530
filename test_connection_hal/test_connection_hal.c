#include "mcs51/cc2530.h"
#include "basicrf/basic_rf.h"

#define PACKET_LENGTH 64

enum Error {
	OK,
	ERR_TIMEOUT,
	ERR_DATA_CORRUPT
};

static basicRfCfg_t cfg;
uint8 packet_payload[PACKET_LENGTH];

void init(void)
{
	cfg.myAddr = 0;
	cfg.panId = 0;
	cfg.channel = 0;
	cfg.ackRequest = 0;
	basicRfInit(&cfg);
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

void send_ping(void)
{
	uint8 err;

	bsp_led_blue(1);
	err = basicRfSendPacket(0, packet_payload, PACKET_LENGTH);
	bsp_led_red(SUCCESS != err);
	bsp_led_blue(0);
}

void recv_ping(void)
{
	uint8 err;
	int16 rssi;

	while(0 == basicRfPacketIsReady()) {;}
	err = basicRfReceive(packet_payload, PACKET_LENGTH, &rssi);
	bsp_led_red(err != SUCCESS);
}

void main(void) {
	init();
	while(1) {
		send_ping();
		recv_ping();
	}
}
