/*
 * uart_dma.c
 *
 *  Created on: 12.12.2017
 *      Author: gwarchol
 */
#include "usart_dma.h"




USART_InitTypeDef USART_InitStructure;
DMA_InitTypeDef DMA_InitStructure;



uint8_t RX_USART_TEMP_BUFFER[TX_USART_BUFFER_SIZE];


volatile data_buffor_t _TX_USART_BUFFOR;
volatile data_buffor_t _RX_USART_BUFFOR;





void ini(void)
{

}



void usart_dma_init()
{


	 RCC_DMA_Configuration();
	 GPIO_DMA_Configuration();
	 DMA_Configuration();

	  //USART_InitStructure.USART_BaudRate = 230400;
	  USART_InitStructure.USART_BaudRate = 9600;
	  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	  USART_InitStructure.USART_StopBits = USART_StopBits_1;
	  USART_InitStructure.USART_Parity = USART_Parity_No;
	  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;


	  USART_Init(USARTy, &USART_InitStructure);
//	  /* Configure USARTz */

	  USART_DMACmd(USARTy, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);

	  DMA_Cmd(USARTy_Tx_DMA_Channel, ENABLE);
	  DMA_Cmd(USARTy_Rx_DMA_Channel, ENABLE);


	  USART_Cmd(USARTy, ENABLE);

	  initBuffor(&_TX_USART_BUFFOR);
	  initBuffor(&_RX_USART_BUFFOR);



	  /* Wait until USARTy TX DMA1 Channel  Transfer Complete */
}


void GPIO_DMA_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Configure USARTy Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = USARTy_RxPin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(USARTy_GPIO, &GPIO_InitStructure);

  /* Configure USARTy Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = USARTy_TxPin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(USARTy_GPIO, &GPIO_InitStructure);
}

void RCC_DMA_Configuration()
{
  /* DMA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(USARTy_GPIO_CLK  | RCC_APB2Periph_AFIO, ENABLE);
  /* Enable USARTy Clock */
  RCC_APB2PeriphClockCmd(USARTy_CLK, ENABLE);
}


void DMA_Configuration(void)
{


  /* USARTy TX DMA1 Channel (triggered by USARTy Tx event) Config */
  DMA_DeInit(USARTy_Tx_DMA_Channel);
  DMA_InitStructure.DMA_PeripheralBaseAddr = USARTy_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)_TX_USART_BUFFOR.data;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = TX_USART_BUFFER_SIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(USARTy_Tx_DMA_Channel, &DMA_InitStructure);

  /* USARTy RX DMA1 Channel (triggered by USARTy Rx event) Config */
  DMA_DeInit(USARTy_Rx_DMA_Channel);
  DMA_InitStructure.DMA_PeripheralBaseAddr = USARTy_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RX_USART_TEMP_BUFFER;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = RX_USART_BUFFER_SIZE;
  DMA_Init(USARTy_Rx_DMA_Channel, &DMA_InitStructure);

}


void controlUartTransfer(){


	/* Wait until USARTy TX DMA1 Channel Transfer Complete */
	while (DMA_GetFlagStatus(USARTy_Tx_DMA_FLAG) == RESET)
	{
	}
	if(!isBufforEmpty(&_TX_USART_BUFFOR)){
		int buff_size = getBufforSize(&_TX_USART_BUFFOR);
		reconfigure_dma_to_transmision(buff_size);
		clearBuffor(&_TX_USART_BUFFOR);
	}




//	while (DMA_GetFlagStatus(USARTy_Rx_DMA_FLAG) == RESET)
//	{
//	}
	unsigned short rx_data_counter = DMA_GetCurrDataCounter(USARTy_Rx_DMA_Channel);
	if(rx_data_counter > 0 ){
		int i;
		for(i = 0; i < rx_data_counter; i++){
			addToBuffor(&_RX_USART_BUFFOR, RX_USART_TEMP_BUFFER[i]);
		}
		reconfigure_dma_to_reception();
	}
}


void analizeIncomingDMAData()
{
	unsigned char temp_byte;

	if(!isBufforEmpty(&_RX_USART_BUFFOR) ){
		if(getFromBuffor(&_RX_USART_BUFFOR, &temp_byte) == true){
			printf("%x ", temp_byte);

		}
	}
	printf("\n");

}






void uart_dma_send_data(unsigned char *data, unsigned char data_size)
{

	int i;
	for(i = 0; i < data_size; i++){
		addToBuffor(&_TX_USART_BUFFOR, data[i]);
	}


}

void reconfigure_dma_to_reception(){
	DMA_DeInit(USARTy_Rx_DMA_Channel);
	DMA_InitStructure.DMA_PeripheralBaseAddr = USARTy_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RX_USART_TEMP_BUFFER;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = RX_USART_BUFFER_SIZE;
	DMA_Init(USARTy_Rx_DMA_Channel, &DMA_InitStructure);
	USART_DMACmd(USARTy, USART_DMAReq_Rx, ENABLE);
	DMA_Cmd(USARTy_Rx_DMA_Channel, ENABLE);
}

void reconfigure_dma_to_transmision(int _buff_size){
	DMA_DeInit(USARTy_Tx_DMA_Channel);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)_TX_USART_BUFFOR.data;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = _buff_size;
	DMA_Init(USARTy_Tx_DMA_Channel, &DMA_InitStructure);
	USART_DMACmd(USARTy, USART_DMAReq_Tx, ENABLE);
	DMA_Cmd(USARTy_Tx_DMA_Channel, ENABLE);


}




void NVIC_DMA_Configuration(void)
{
//   NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USARTz Interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = USARTz_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
}










