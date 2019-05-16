/*
 * usart_dma.h
 *
 *  Created on: 14.12.2017
 *      Author: gwarchol
 */

#ifndef USART_DMA_H_
#define USART_DMA_H_

/*
 * uart_dma.h
 *
 *  Created on: 12.12.2017
 *      Author: gwarchol
 */

#ifndef UART_DMA_H_
#define UART_DMA_H_
#include <device_config.h>
#include "string.h"
#include "buffors.h"



  #define USARTy                   USART1
  #define USARTy_GPIO              GPIOA
  #define USARTy_CLK               RCC_APB2Periph_USART1
  #define USARTy_GPIO_CLK          RCC_APB2Periph_GPIOA
  #define USARTy_RxPin             GPIO_Pin_10
  #define USARTy_TxPin             GPIO_Pin_9
  #define USARTy_Tx_DMA_Channel    DMA1_Channel4
  #define USARTy_Tx_DMA_FLAG       DMA1_FLAG_TC4
  #define USARTy_Rx_DMA_Channel    DMA1_Channel5
  #define USARTy_Rx_DMA_FLAG       DMA1_FLAG_TC5
  #define USARTy_DR_Base           0x40013804



#define TX_USART_BUFFER_SIZE 128
#define RX_USART_BUFFER_SIZE 128


//#define TxBufferSize1   (countof(TxBuffer1) - 1)
//#define TxBufferSize2   (countof(TxBuffer2) - 1)




void ini(void);


void GPIO_DMA_Configuration(void);
void NVIC_DMA_Configuration(void);
void DMA_Configuration(void);
void controlUartTransfer();

void usart_dma_init();

void reconfigure_dma_to_reception();
void reconfigure_dma_to_transmision(int _buf_size);

void uart_dma_send_data(unsigned char *data, unsigned char data_size);

void analizeIncomingDMAData();









#endif /* UART_DMA_H_ */



#endif /* USART_DMA_H_ */
