#include "mcs51/cc2530.h"

void delay(int value) {
	int i;

	for(i = 0; i < value; i++) {;}
}


void main_func(void) {
	P0DIR |= 0xFF;
	P1DIR |= 0xFF;
	P0 ^= 0xFF;
	P1 ^= 0xFF;
}

void main(void) {
	while(1) {
		main_func();
		delay(10000);
	}
}

