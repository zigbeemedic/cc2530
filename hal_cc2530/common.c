#include "hal_cc2530.h"

void Hal_Init(uint8_t rf_ch, uint16_t pan_id, uint16_t self_addr)
{
	P1DIR |= 0x3;
	Hal_LedBlue(0);
	Hal_LedRed(0);
	Hal_UART_Init();
	Hal_Rf_Init(rf_ch, pan_id, self_addr);
}

void Hal_LedBlue(bool on)
{
	P1_0 = !on;
}

void Hal_LedRed(bool on)
{
	P1_1 = !on;
}

void Hal_CLock_WaitUs(uint16_t delay)
{
	/* TODO: implement it correctly */
	uint16_t i;
	for(i = delay; i > 0; i--)
		__asm__("NOP");
}
