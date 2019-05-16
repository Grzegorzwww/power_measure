/*
 * my_tasks.h
 *
 *  Created on: 06.05.2019
 *      Author: gwarchol
 */

#ifndef MY_TASKS_H_
#define MY_TASKS_H_




void vLEDTask(void *pvParameters);
void vControlADC(void *pvParameters);
void vDisplayRefresh(void *pvParameters);
void vReadTensometr(void *pvParameters);
void vGpioHandlerTask(void *pvParameters);


void vApplicationTickHook( void );




#endif /* MY_TASKS_H_ */
