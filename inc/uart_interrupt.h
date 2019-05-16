/*
 * uart_interrupt.h
 *
 *  Created on: 12.12.2017
 *      Author: gwarchol
 */

#ifndef UART_INTERRUPT_H_
#define UART_INTERRUPT_H_
#include <device_config.h>
#include "string.h"
#include "buffors.h"




typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

#define countof(a)   (sizeof(a) / sizeof(*(a)))








void uart_interrup_init();

void uart_send_data(unsigned char *data, unsigned int data_size);


void init_uart_polling();

void uart_send_char(char c);


void analizeIncomingMp3Data();
data_buffor_t *get_usart_tx_buffor_ptr();



#endif /* UART_INTERRUPT_H_ */
