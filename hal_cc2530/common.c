#include "cc2530_hal.h"

void Hal_Init(void)
{
	P1DIR |= 0x3;
	Hal_LedBlue(0);
	Hal_LedRed(0);
	cc2530_UART_Init();
	cc2530_Rf_Init();
}

void Hal_LedBlue(int on)
{
	P1_0 = !on;
}

void Hal_LedRed(int on)
{
	P1_1 = !on;
}

