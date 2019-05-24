/*
 * gpio.c
 *
 *  Created on: 10.05.2019
 *      Author: gwarchol
 */


#include "gpio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "my_tasks.h"

BitFilterInstance KONTRAKTON_IO_filter;


void vGPIOInit()
{

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

//	GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_StructInit(&GPIO_InitStructure);
//
//
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
////	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
//
//
//	EXTI_InitTypeDef EXTI_InitStructure;
//
//	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);
//
//
////	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
//
//	NVIC_InitTypeDef NVIC_InitStructure;
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13;
////	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//
//	NVIC_SystemLPConfig(NVIC_LP_SLEEPONEXIT, DISABLE);
//
//
//
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
//
//	GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET );



	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

	GPIO_Init(GPIOB, &GPIO_InitStructure);


	initBitFilter(&KONTRAKTON_IO_filter, 5);

	bitFilter_setOnBitChangeListener_signal(&KONTRAKTON_IO_filter,KONTRAKTON_IO_callback);



}










void scanButtonsPins(void) {
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) != Bit_SET){
		buttonIsPressedNotFiltered(&KONTRAKTON_IO_filter);
	}
	else {
		buttonIsReleasedNotFiltered(&KONTRAKTON_IO_filter);

	}

}
extern SemaphoreHandle_t xSemaphore;

void KONTRAKTON_IO_callback(BitEvent bitEvent){
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	switch(bitEvent){
		case ACTION_UP:

			printf("action up\n");

		break;
		case ACTION_DOWN:

			printf("action down\n");
			xSemaphoreGiveFromISR( xSemaphore, &xHigherPriorityTaskWoken );
			 portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

		break;
	}
}




static void nullCallback(BitEvent bitEvent){}


void initBitFilter(BitFilterInstance *bitFilterInstance, unsigned int  numberOfCorrectProbes){
	bitFilterInstance->callback = nullCallback;
	bitFilterInstance->previousFilteredKeyState = 2; // 2 - not defined, used for first scan
	bitFilterInstance->previousNotFilteredState = 0;
	bitFilterInstance->count = 0;
	bitFilterInstance->numberOfCorrectProbes = numberOfCorrectProbes;
}

void bitFilter_setOnBitChangeListener_signal(BitFilterInstance *bitFilterInstance, void (*nCallback)(BitEvent)){
	bitFilterInstance->callback = nCallback;
}


void callbackEventPressed(BitFilterInstance *bitFilterInstance){
	bitFilterInstance->callback(ACTION_DOWN);
}

void callbackEventReleased(BitFilterInstance *bitFilterInstance){
	bitFilterInstance->callback(ACTION_UP);
}

void buttonIsPressedFiltered(BitFilterInstance *bitFilterInstance){
	if(bitFilterInstance->previousFilteredKeyState != 1){
		bitFilterInstance->previousFilteredKeyState = 1;
		callbackEventPressed(bitFilterInstance);
	}
}

void buttonIsReleasedFiltered(BitFilterInstance *bitFilterInstance){
	if(bitFilterInstance->previousFilteredKeyState != 0){
		bitFilterInstance->previousFilteredKeyState = 0;
		callbackEventReleased(bitFilterInstance);
	}
}


void incrementPressedProbe(BitFilterInstance *bitFilterInstance){

	if(bitFilterInstance->previousNotFilteredState == 1){
		bitFilterInstance->count++;
		if(bitFilterInstance->count == bitFilterInstance->numberOfCorrectProbes){
			buttonIsPressedFiltered(bitFilterInstance);
			bitFilterInstance->count = 1;
		}
		}else{
		bitFilterInstance->previousNotFilteredState = 1;
		bitFilterInstance->count = 1;
	}
}

void incrementReleasedProbe(BitFilterInstance *bitFilterInstance){
	if(bitFilterInstance->previousNotFilteredState == 0){
		bitFilterInstance->count++;
		if(bitFilterInstance->count == bitFilterInstance->numberOfCorrectProbes){
			buttonIsReleasedFiltered(bitFilterInstance);
			bitFilterInstance->count = 1;
		}
		}else{
		bitFilterInstance->previousNotFilteredState = 0;
		bitFilterInstance->count = 1;
	}
}

void buttonIsPressedNotFiltered(BitFilterInstance *bitFilterInstance){
	incrementPressedProbe(bitFilterInstance);
}

void buttonIsReleasedNotFiltered(BitFilterInstance *bitFilterInstance){
	incrementReleasedProbe(bitFilterInstance);
}




