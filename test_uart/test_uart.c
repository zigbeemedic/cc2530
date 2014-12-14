#include "hal_cc2530.h"
#include <stdio.h>

void main(void) {
	hal_init();
	while(1) {
		printf("Hello, World!\n\r");
	}
}

