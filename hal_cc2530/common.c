#include "cc2530_hal.h"

void XWWK_Init(void)
{
	P1DIR |= 0x3;
	XWWK_LedBlue(0);
	XWWK_LedRed(0);
	cc2530_UART_Init();
	cc2530_Rf_Init();
}

void XWWK_LedBlue(int on)
{
	P1_0 = !on;
}

void XWWK_LedRed(int on)
{
	P1_1 = !on;
}

