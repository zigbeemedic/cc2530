#include <stdio.h>
#include "hal_cc2530.h"

#define CCA_THRES_USER_GUIDE          0xF8
#define CCA_THRES_ALONE               0xFC   /* -4-76=-80dBm when CC2530 operated alone or with CC2591 in LGM */
#define CCA_THR_HGM                   0x06   /* 6-76=-70dBm when CC2530 operated with CC2591 in HGM */
#define CORR_THR                      0x14

#define CC2530_RF_MAX_PACKET_LEN      127
#define CC2530_RF_MIN_PACKET_LEN        4
#define CHECKSUM_LEN 2

#define CC2530_RF_CCA_CLEAR             1
#define CC2530_RF_CCA_BUSY              0

#ifdef CC2530_RF_CONF_CCA_THRES
#define CC2530_RF_CCA_THRES CC2530_RF_CONF_CCA_THRES
#else
#define CC2530_RF_CCA_THRES CCA_THRES_USER_GUIDE /* User guide recommendation */
#endif

#define CC2530_RF_TX_POWER  0xD5

/* Bit Masks for the last byte in the RX FIFO */
#define CRC_BIT_MASK 0x80
#define LQI_BIT_MASK 0x7F
/* RSSI Offset */
#define RSSI_OFFSET    73

static enum Hal_RfState rf_state = RFSTATE_OFF;

static void Hal_Rf_On(void)
{
	if(RFSTATE_ON_RX == rf_state)
		return;
	CSP_CMD(ISFLUSHRX);
	CSP_CMD(ISRXON);
	rf_state = RFSTATE_ON_RX;
}

static void Hal_Rf_Off(void)
{
	CSP_CMD(ISRFOFF);
	CSP_CMD(ISFLUSHRX);
	rf_state = RFSTATE_OFF;
}

void Hal_Rf_SetPower(uint8_t power)
{
	/* TODO: need a conversion from human readable units to register value */
	TXPOWER = power;
}

void Hal_Rf_SetChannel(uint8_t channel)
{
	/* Changes to FREQCTRL take effect after the next recalibration */
	Hal_Rf_Off();
	FREQCTRL = (CC2530_RF_CHANNEL_MIN + 
		(channel - CC2530_RF_CHANNEL_MIN) * CC2530_RF_CHANNEL_SPACING);
	Hal_Rf_On();
}

void Hal_Rf_SetAddr(uint16_t pan_id, uint16_t self_addr)
{
	PANIDL = pan_id & 0xFF;
	PANIDH = pan_id >> 8;

	SHORTADDRL = self_addr & 0xFF;
	SHORTADDRH = self_addr >> 8;
}

void Hal_Rf_Init(uint8_t channel, uint16_t pan_id, uint16_t self_addr)
{
#if 	CC2530_RF_LOW_POWER_RX
	/* Reduce RX power consumption current to 20mA at the cost of sensitivity */
	RXCTRL = 0x00;
	FSCTRL = 0x50;
#else
	RXCTRL = 0x3F;
	FSCTRL = 0x55;
#endif 
	CCACTRL0 = CC2530_RF_CCA_THRES;

	/*
	 * According to the user guide, these registers must be updated from their
	 * defaults for optimal performance
	 *
	 * Table 23-6, Sec. 23.15.1, p. 259
	 */
	TXFILTCFG = 0x09; /* TX anti-aliasing filter */
	AGCCTRL1 = 0x15;  /* AGC target value */
	FSCAL1 = 0x00;    /* Reduce the VCO leakage */

	/* Auto ACKs and CRC calculation, default RX and TX modes with FIFOs */
	FRMCTRL0 |= BV(6); // FRMCTRL0_AUTOCRC
	FRMCTRL0 |= BV(5); // FRMCTRL0_AUTOACK

	/* Disable source address matching and autopend */
	SRCMATCH = 0; /* TODO: investigate */
	
	/* MAX FIFOP threshold */
	FIFOPCTRL = CC2530_RF_MAX_PACKET_LEN;
	
	Hal_Rf_SetPower(CC2530_RF_TX_POWER);
	Hal_Rf_SetChannel(channel);
	Hal_Rf_SetAddr(pan_id, self_addr);

	CSP_CMD(ISCLEAR);

	Hal_Rf_On();
}

void Hal_Rf_RecvOn()
{
	CSP_CMD(ISFLUSHRX);
	CSP_CMD(ISRXON);
	rf_state = RFSTATE_ON_RX;
}

void Hal_Rf_WaitTXReady(void)
{
	// Wait for SFD not active and TX_Active not active
	while (FSMSTAT1 & (BV(1) | BV(5))) {
		__asm__("NOP");
	}
}

static int is_channel_clear(void)
{
	return !(FSMSTAT1 & FSMSTAT1_CCA);
}

enum HalError Hal_Rf_Send(void *payload, unsigned short payload_len)
{
	uint8_t counter;
	enum HalError ret = TX_ERROR;
	uint8_t i;
	
	/*
	 * When we transmit in very quick bursts, make sure previous transmission
	 * is not still in progress before re-writing to the TX FIFO
	 */
	while(FSMSTAT1 & FSMSTAT1_TX_ACTIVE)
		__asm__("NOP");
	
	if(RFSTATE_ON_TX == rf_state) {
		Hal_Rf_On();
		Hal_CLock_WaitUs(RF_TURN_ON_TIME); 
	}
	
	CSP_CMD(ISFLUSHTX);
	
	/* Send the phy length byte first */
	RFD = payload_len + CHECKSUM_LEN; /* Payload plus FCS */
	for(i = 0; i < payload_len; i++) {
		RFD = ((unsigned char*) (payload))[i];
	}
	
	/* Leave space for the FCS */
	RFD = 0;
	RFD = 0;

	while(!is_channel_clear())
		__asm__("NOP");
	
	/* prepare() double checked that TX_ACTIVE is low. If SFD is high we are
	 * receiving. Abort transmission and bail out with RADIO_TX_COLLISION */
	if(FSMSTAT1 & FSMSTAT1_SFD) {
		return TX_SFD_NOW;
	}
	
	/* Start the transmission */
	CSP_CMD(ISTXON);
	
	counter = 0;
	while(!(FSMSTAT1 & FSMSTAT1_TX_ACTIVE) && (counter++ < 3)) {
		Hal_CLock_WaitUs(6);
	}
	
	if(!(FSMSTAT1 & FSMSTAT1_TX_ACTIVE)) {
		CSP_CMD(ISFLUSHTX);
		ret = TX_NEVER_ACTIVE;
	} else {
		/* Wait for the transmission to finish */
		while(FSMSTAT1 & FSMSTAT1_TX_ACTIVE);
		ret = OK;
	}
	
	Hal_Rf_Off(); /* TODO: we need the flag - should we turn off transmitter? */
	
	return ret;
}

enum HalError Hal_Rf_Read(void *buf, unsigned short bufsize, uint8_t *received)
{
	uint8_t i;
	uint8_t len;
	uint8_t crc_corr;
	int8_t rssi;
	
	*received = 0;

	if(rf_state != RFSTATE_ON_RX)
		Hal_Rf_On();

	while(!(FSMSTAT1 & FSMSTAT1_FIFO))
		__asm__("NOP");

	/* Check the length */
	len = RFD;
	printf("len = %d\n\r", len);
	
	/* Check for validity */
	if(len > CC2530_RF_MAX_PACKET_LEN) {
		/* Oops, we must be out of sync. */
		CSP_CMD(ISFLUSHRX);
		return RX_PACKET_TOO_BIG;
	}
	
	if(len <= CC2530_RF_MIN_PACKET_LEN) {
		CSP_CMD(ISFLUSHRX);
		return RX_PACKET_TOO_SMALL;
	}
	
	if(len - CHECKSUM_LEN > bufsize) {
		CSP_CMD(ISFLUSHRX);
		return RX_BUF_TOO_SMALL;
	}
	
	len -= CHECKSUM_LEN;
	for(i = 0; i < len; ++i) {
		((unsigned char*)(buf))[i] = RFD;
  	}

	/* Read the RSSI and CRC/Corr bytes */
	rssi = ((int8_t) RFD) - RSSI_OFFSET;
	crc_corr = RFD;

	/* MS bit CRC OK/Not OK, 7 LS Bits, Correlation value */
	if(crc_corr & CRC_BIT_MASK) {
		// TODO: implement it
		// packetbuf_set_attr(PACKETBUF_ATTR_RSSI, rssi);
		// packetbuf_set_attr(PACKETBUF_ATTR_LINK_QUALITY, crc_corr & LQI_BIT_MASK);
	} else {
		CSP_CMD(ISFLUSHRX);
		*received = 0;
	}
	
	/* If FIFOP==1 and FIFO==0 then we had a FIFO overflow at some point. */
	if((FSMSTAT1 & (FSMSTAT1_FIFO | FSMSTAT1_FIFOP)) == FSMSTAT1_FIFOP) {
		/*
		 * If we reach here means that there might be more intact packets in the
		 * FIFO despite the overflow. This can happen with bursts of small packets.
		 *
		 * Only flush if the FIFO is actually empty. If not, then next pass we will
		 * pick up one more packet or flush due to an error.
		 */
		if(!RXFIFOCNT) {
			CSP_CMD(ISFLUSHRX);
		}
	}
	
	*received = len;
	return OK;
}

static int receiving_packet(void)
{
	/*
	 * SFD high while transmitting and receiving.
	 * TX_ACTIVE high only when transmitting
	 *
	 * FSMSTAT1 & (TX_ACTIVE | SFD) == SFD <=> receiving
	 */
	return (FSMSTAT1 & (FSMSTAT1_TX_ACTIVE | FSMSTAT1_SFD) == FSMSTAT1_SFD);
}

static int pending_packet(void)
{
	return (FSMSTAT1 & FSMSTAT1_FIFOP);
}

