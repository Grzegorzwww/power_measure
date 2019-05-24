/*
 * gpio.h
 *
 *  Created on: 10.05.2019
 *      Author: gwarchol
 */

#ifndef GPIO_H_
#define GPIO_H_
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

void vGPIOInit();



#define TRUE 1
#define FALSE 0

typedef enum{
	ACTION_DOWN,
	ACTION_UP
} BitEvent;



typedef enum{
	PRESSED,
	RELEASED
}ButtonState;




typedef struct{
	volatile	unsigned int previousNotFilteredState;
	volatile	unsigned int count;
	unsigned int  previousFilteredKeyState;
	unsigned int  numberOfCorrectProbes;
	void (*callback)(BitEvent);
}BitFilterInstance;

//BitFilterInstance ALARM_BUTTON_filter;


//typedef enum{
//	ALARM = 0,
//
//
//}Button;







void initBitFilter(BitFilterInstance *bitFilterInstance, unsigned int numberOfCorrectProbes);
void bitFilter_setOnBitChangeListener_signal(BitFilterInstance *bitFilterInstance, void (*nCallback)(BitEvent));
void buttonIsPressedNotFiltered(BitFilterInstance *bitFilterInstance);
void buttonIsReleasedNotFiltered(BitFilterInstance *bitFilterInstance);
void buttonIsPressedFiltered(BitFilterInstance *bitFilterInstance);
void buttonIsReleasedFiltered(BitFilterInstance *bitFilterInstance);



void scanButtonsPins(void);







void KONTRAKTON_IO_callback(BitEvent bitEvent);



#endif /* GPIO_H_ */
