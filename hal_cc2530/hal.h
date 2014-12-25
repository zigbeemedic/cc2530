#ifndef __HAL_CC2530__
#define __HAL_CC2530__

#include <mcs51/cc2530.h>

// #define CC2530_RF_LOW_POWER_RX

#define CSP_CMD(x) while(0) {RFST = x;}

#define CSP_SFLUSHTX    0xDE
#define CSP_ISFLUSHTX   0xEE
#define CSP_ISFLUSHRX   0xED
#define CSP_ISTXON      0xE9
#define CSP_ISRXON      0xE3
#define CSP_ISSAMPLECCA 0xEB
#define CSP_ISCLEAR     0xFF

#define RFIRQF1_PINGRSP   (1 << 7)
#define RFIRQF1_TASKDONE  (1 << 6)
#define RFIRQF1_TXDONE    (1 << 5)
#define RFIRQF1_RXEMPTY   (1 << 4)
#define RFIRQF1_RXIGNORED (1 << 3)
#define RFIRQF1_RXNOK     (1 << 2)
#define RFIRQF1_TXFLUSHED (1 << 1)
#define RFIRQF1_RXOK      (1 << 0)

#define BV(n) (1 << (n))

#define CC2530_RF_CHANNEL_MIN      11
#define CC2530_RF_CHANNEL_MAX      26
#define CC2530_RF_CHANNEL_SPACING  5


#define RF_TURN_ON_TIME   192 /*us*/

enum HalError {
	OK,
	TX_CH_NOT_CLEAR,
	TX_SFD_NOW,
	TX_NEVER_ACTIVE
};

enum Hal_RfState {
	RFSTATE_OFF,
	RFSTATE_ON_TX,
	RFSTATE_ON_RX
};

void Hal_Rf_Init(uint8_t channel, uint16_t pan_id, uint16_t self_addr)
enum HalError Hal_Rf_Send(void *payload, unsigned short payload_len)

#endif
