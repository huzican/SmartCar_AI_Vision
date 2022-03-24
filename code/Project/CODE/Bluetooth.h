#ifndef     __BLUETOOTH_H__
#define     __BLUETOOTH_H__

#include "common.h"

#define BULETOOTH_UART  USART_6
#define BULETOOTH_BAUD  115200
#define BULETOOTH_TX    UART6_TX_B2
#define BLUETOOTH_RX    UART6_RX_B3

#define ZIGBEE_UART     USART_1
#define ZIGBEE_BAUD     115200
#define ZIGBEE_TX       UART1_TX_B12
#define ZIGBEE_RX       UART1_RX_B13

extern int16 var[8];

void vcan_sendware(void *wareaddr, uint32 waresize);
void bluetooth_init(void);
void zigbee_init(void);
void zigbee_IRQHandle();

#endif
