#ifndef __HAL_CC2530__
#define __HAL_CC2530__

#include <stdint.h>
#include <stdbool.h>
#include <mcs51/cc2530.h>

// #define CC2530_RF_LOW_POWER_RX

#define CSP_CMD(x) RFST = x

#define SFLUSHTX    0xDE
#define ISFLUSHTX   0xEE
#define ISFLUSHRX   0xED
#define ISTXON      0xE9
#define ISRXON      0xE3
#define ISSAMPLECCA 0xEB
#define ISCLEAR     0xFF
#define ISRFOFF     0xEF

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

#define FSMSTAT1_RX_ACTIVE  BV(0)
#define FSMSTAT1_TX_ACTIVE  BV(1)
#define FSMSTAT1_CCA        BV(4)
#define FSMSTAT1_SFD        BV(5)
#define FSMSTAT1_FIFOP      BV(6)
#define FSMSTAT1_FIFO       BV(7)

#define RF_TURN_ON_TIME   192 /*us*/

enum HalError {
	OK,
	TX_CH_NOT_CLEAR,
	TX_SFD_NOW,
	TX_NEVER_ACTIVE,
	TX_ERROR,
	RX_PACKET_TOO_BIG,
	RX_PACKET_TOO_SMALL,
	RX_BUF_TOO_SMALL
};

enum Hal_RfState {
	RFSTATE_OFF,
	RFSTATE_ON_TX,
	RFSTATE_ON_RX
};

void Hal_Init(uint8_t rf_ch, uint16_t pan_id, uint16_t self_addr);
void Hal_LedBlue(bool on);
void Hal_LedRed(bool on);
void Hal_CLock_WaitUs(uint16_t delay);

void Hal_Rf_Init(uint8_t channel, uint16_t pan_id, uint16_t self_addr);
void Hal_Rf_SetPower(uint8_t power);
void Hal_Rf_SetChannel(uint8_t channel);
void Hal_Rf_SetAddr(uint16_t pan_id, uint16_t self_addr);
enum HalError Hal_Rf_Send(void *payload, unsigned short payload_len);

void Hal_UART_Init(void);

#endif
