#ifndef __HAL_CC2530__
#define __HAL_CC2530__

#include <mcs51/cc2530.h>

#define CSP_SFLUSHTX  0xDE
#define CSP_ISFLUSHTX 0xEE
#define CSP_ISFLUSHRX 0xED
#define CSP_ISTXON    0xE9
#define CSP_ISRXON    0xE3
#define CSP_ISCLEAR   0xFF

#define RFIRQF1_PINGRSP   (1 << 7)
#define RFIRQF1_TASKDONE  (1 << 6)
#define RFIRQF1_TXDONE    (1 << 5)
#define RFIRQF1_RXEMPTY   (1 << 4)
#define RFIRQF1_RXIGNORED (1 << 3)
#define RFIRQF1_RXNOK     (1 << 2)
#define RFIRQF1_TXFLUSHED (1 << 1)
#define RFIRQF1_RXOK      (1 << 0)

#define AUTO_ACK  (1 << 5)
#define AUTO_CRC  (1 << 6)
#define RXMODE(val) (val << 2)

void hal_init(void);
void hal_cmd2rf(unsigned char cmd);
void hal_led_blue(unsigned char on);
void hal_led_red(unsigned char on);

#endif