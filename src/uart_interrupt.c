/*
 * uart_interrupt.c
 *
 *  Created on: 12.12.2017
 *      Author: gwarchol
 */
#include "uart_interrupt.h"

volatile data_buffor_t _TX_USART_BUFFOR;
volatile data_buffor_t _RX_USART_BUFFOR;

data_buffor_t *get_usart_tx_buffor_ptr()
{

	return &_TX_USART_BUFFOR;
}

USART_InitTypeDef USART_InitStructure;

void uart_interrup_init(){


	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB |  RCC_APB2Periph_AFIO, ENABLE);

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure; 	// Configure USARTy Rx as input floating
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

	GPIO_Init( GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 	// Configure USARTy Tx as alternate function push-pull
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	initBuffor(&_TX_USART_BUFFOR);
	initBuffor(&_RX_USART_BUFFOR);

	USART_StructInit( &USART_InitStructure);
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);

	NVIC_InitTypeDef NVIC_InitStructure;
	/* Configure the NVIC Preemption Priority Bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 9;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);


	USART_Cmd(USART3, ENABLE);

}







void uart_send_data(unsigned char *data, unsigned int data_size){

	if(data_size > 0){
		int i;
			for(i = 0; i < data_size; i++){
				addToBuffor(&_TX_USART_BUFFOR, data[i]);
			}
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
	}
}



void  USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		//printf("%c\n", USART_ReceiveData(USART3));
		addToBuffor(&_RX_USART_BUFFOR, USART_ReceiveData(USART3));

		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	}

	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
	{
		/* Write one byte to the transmit data register */
		//printf("przerwanie tx\n");
		unsigned char data_byte = 0x00;
		if(getFromBuffor(&_TX_USART_BUFFOR, &data_byte))
		USART_SendData(USART3, data_byte);
		if(isBufforEmpty(&_TX_USART_BUFFOR))
		{
			USART_ITConfig(USART3, USART_IT_TXE, DISABLE); 	/* Disable the USARTy Transmit interrupt */
		}
		else{
			USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
		}
	}
}


void init_uart_polling(){
	USART_InitTypeDef uart;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure; 	// Configure USARTy Rx as input floating
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

	GPIO_Init( GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 	// Configure USARTy Tx as alternate function push-pull
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
//	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);

	USART_StructInit(&uart);
	uart.USART_BaudRate = 9600;
	uart.USART_WordLength = USART_WordLength_8b;
	uart.USART_StopBits = USART_StopBits_1;
	uart.USART_Parity = USART_Parity_No;

	USART_Init(USART3, &uart);
	USART_Cmd(USART3, ENABLE);

}

void uart_send_char(char c)
{
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, c);
}


void analizeIncomingMp3Data()
{
	unsigned char temp_byte;
	if(getFromBuffor(&_RX_USART_BUFFOR, &temp_byte) == true){
//		printf("%c\n", temp_byte);
		//analizeDataFromMP3(temp_byte);
	}
}




