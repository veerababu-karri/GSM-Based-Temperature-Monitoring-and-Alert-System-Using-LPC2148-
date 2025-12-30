#ifndef __UART_H__
#define __UART_H__

#include "gsm.h"

extern volatile char rx_buffer[100];

extern char rx_line[100];

extern volatile int rx_ready;

extern volatile GSM_Response_t gsm_resp;

void UART0_Init(void);

void UART0_TxChar(char ch);

void UART0_TxString(char *str);


#endif

