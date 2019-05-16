/*
 * config.c
 *
 *  Created on: 05.12.2017
 *      Author: gwarchol
 */

#include "device_config.h"


GPIO_InitTypeDef GPIO_InitStruct;

static volatile bool sleep_mode_status = false;
static volatile bool exit_standby_mode = false;
static int goto_sleep_counter = 0;


bool getStandByModeState(){
	return sleep_mode_status;
}

void set_sleep_mode() {
	TFT_LED_RESET;
	NVIC_SystemLPConfig(NVIC_LP_SLEEPONEXIT, ENABLE);
	sleep_mode_status = true;

#ifdef DEBUG
	//	printf("uspienie \n");
#endif

}

void clr_sleep_mode() {
	NVIC_SystemLPConfig(NVIC_LP_SLEEPONEXIT, DISABLE);
	TFT_LED_SET
	sleep_mode_status = false;


#ifdef DEBUG
	//printf("wybudzenie \n");
#endif
}


void sleep_mode_init()
{

	DBGMCU_Config(DBGMCU_STANDBY, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);



	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	//	GPIO_Init(GPIOA, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
	//	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	//	EXTI_Init(&EXTI_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4);
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	//  PWR_WakeUpPinCmd(ENABLE);
//
//	NVIC_SystemLPConfig(NVIC_LP_SLEEPONEXIT, DISABLE);

}



void control__goto_sleep_mode(){
//	if(goto_sleep_counter++ >= SPEEP_TIMEOUT_SEK){
//		if(!sleep_mode_status){
//			set_sleep_mode();
//		}
//		goto_sleep_counter = 0;
//
//	}



}

//void EXTI0_IRQHandler(void)
//{
////	EXTI_ClearITPendingBit(EXTI_Line0);
//////	printf("EXTI0_IRQHandler\n");
////	goto_sleep_counter = 0;
////	if(sleep_mode_status){
////		clr_sleep_mode();
////
////	}
//}


void EXTI1_IRQHandler(void)
{
//	//printf("EXTI1_IRQHandler\n");
//	EXTI_ClearITPendingBit(EXTI_Line1);
//
//		if(sleep_mode_status){
//			clr_sleep_mode();
//		}
//	goto_sleep_counter = 0;
////	printf("Touch irq\n");

}

void EXTI4_IRQHandler(void)
{
//	EXTI_ClearITPendingBit(EXTI_Line4);
//	//printf("Przerwanie linia 3\n");
//	if(get_touch_force() > 0){
//		if(sleep_mode_status){
//			clr_sleep_mode();
//		}
//		goto_sleep_counter = 0;
//		//	printf("Touch irq\n");
//		//
//	}

}






void device_init(){


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStruct);


}
void init_nvic(){

}


void turnOnOffLed(bool state){
	switch(state){
	case true:
		GPIO_WriteBit(GPIOC, GPIO_Pin_13,Bit_RESET );
		break;
	case false:
		GPIO_WriteBit(GPIOC, GPIO_Pin_13,Bit_SET );
		break;
	}
}

void RCC_Conf(void) {
	/* Konfiguracja zegara -------------------------------------------------------*/
	/* Wyzerowanie poprzednich ustawień RCC*/
	RCC_DeInit();

	ErrorStatus HSEStartUpStatus;

	/* Włączenie HSE */
	RCC_HSEConfig(RCC_HSE_ON);

	/* Oczekiwanie na gotowość HSE
 	while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET) {
 	}
	 */
	/* Czekaj, aż HSE sie uruchomi lub zostanie przekroczony dozwolony czas */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if (HSEStartUpStatus == SUCCESS)
	{
		/* Włączenie opóźnień dla FLASH */
		FLASH_SetLatency(FLASH_Latency_2);
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		/* HCLK = SYSCLK */
		RCC_HCLKConfig(RCC_SYSCLK_Div1);

		/* PCLK2 = HCLK */
		RCC_PCLK2Config(RCC_HCLK_Div1);

		/* PCLK1 = HCLK/2 */
		RCC_PCLK1Config(RCC_HCLK_Div2);

		/* PLLCLK = 8MHz * 9 = 72 MHz
		 * w linii Conectivity Line nie ma mozliwosci skorzystania bezposrednio z HSE jako zrodla dla petli PLL
		 * */
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1 , RCC_PLLMul_9); // ????

		/* Włączenie PLL */
		RCC_PLLCmd(ENABLE);

		/* Oczekiwanie na gotowość PLL */
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {
		}

		/* Wybór PLL na zegar systemowy */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		/* Oczekiwanie na wybór zegara systemowego */
		while (RCC_GetSYSCLKSource() != 0x08) {
		}
	}
}







