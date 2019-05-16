/*
 * my_tasks.c
 *
 *  Created on: 06.05.2019
 *      Author: gwarchol
 */

#include "my_tasks.h"

#include "stdbool.h"

#include "FreeRTOS.h"

#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "adc.h"
#include "hx711.h"
#include "text.h"
#include "string.h"
#include  "timers.h"
#include "stdint.h"



QueueHandle_t QueueAdcValue = NULL;
QueueHandle_t QueueTensometr = NULL;
SemaphoreHandle_t xSemaphore = NULL;
xTimerHandle timerHndlSec = NULL;

static int crank_rate_ms = 0;
static int crank_candence = 0;

void vTimerCallbackMinCadenceExpired(xTimerHandle pxTimer) {
	//

//	if(crank_rate_ms > 0 ){
//	        		crank_candence = ((1000 * 60) /  crank_rate_ms);
//	        		crank_rate_ms = 0;
//	        		xTimerReset(timerHndlSec, 2000);
//
//	        	}else if (crank_rate_ms == 0){
//	        		 crank_rate_ms = 0;
//	        		 crank_candence  = 0;
//	        	}


}



void vxCreateRTOSMachanisms(){
	QueueAdcValue = xQueueCreate(3, sizeof(unsigned short));
	if(QueueAdcValue == NULL){
		while(true);
	}

	QueueTensometr = xQueueCreate(3, sizeof(uint64_t));
	if(QueueTensometr == NULL){
		while(true);
	}

    xSemaphore = xSemaphoreCreateBinary();
    if( xSemaphore == NULL ) while(1);



    timerHndlSec = xTimerCreate(
          "timer1", /* name */
          pdMS_TO_TICKS(2000), /* period/time */
          pdTRUE, /* auto reload */
          (void*)0, /* timer ID */
          vTimerCallbackMinCadenceExpired); /* callback */
    if (timerHndlSec==NULL) {
      for(;;); /* failure! */
    }


    if (xTimerStart(timerHndlSec, 0)!=pdPASS) {
      for(;;); /* failure!?! */
    }


}



void vApplicationTickHook( void ){

	vIncrementDelayTicks();


}


void vLEDTask(void *pvParameters)
{
	static bool led_toogle = false;

	for (;;) {

		switch(led_toogle){
		case true :
			turnOnOffLed(led_toogle);
			led_toogle = false;
			break;
		case false :
			turnOnOffLed(led_toogle);
			led_toogle = true;
			break;
		}
		vTaskDelay( 1000 / portTICK_RATE_MS );
	}
}

void vControlADC(void *pvParameters)
{

	for (;;) {
		unsigned short temp = xGetADCValue();
		xQueueSend( QueueAdcValue, &temp, portMAX_DELAY);
		vTaskDelay( 20 / portTICK_RATE_MS );
	}
}

void vDisplayRefresh(void *pvParameters)
{
	static int tekst_len = 0;;
	static int tekst_cadence_len = 0;;
	unsigned short temp_adc =0;


	unsigned int tensometr_val =0;
	char tekst_moc[20];
	char tekst_kadencja[20];

	LCD_setTextSize(4);
    LCD_setTextColor(WHITE);
    LCD_setTextBgColor(BLACK);

	for (;;) {

		tekst_moc[0] = '\0';
		tekst_kadencja[0] = '\0';


		while(xQueueReceive(QueueTensometr, &tensometr_val, 1 / portTICK_RATE_MS ) == pdTRUE){
//			sprintf(tekst_moc, "MOC = %.2f\0",  tensometr_val );
//			tensometr_val  = tensometr_val  & ~(0xFF << 0);

//			tensometr_val = ((tensometr_val / 10000)  *  4.246 )+ 44.0;
			tensometr_val = ((tensometr_val / 10000)  *  4.240 )- 26.0;

			sprintf(tekst_moc, "W = %d\0",  tensometr_val);
			if(tekst_len != strlen(tekst_moc)){
				tekst_len = strlen(tekst_moc);
				LCD_setCursor(20, 40);
				LCD_writeString("               ");
			}
			LCD_setCursor(20, 40);
			LCD_writeString(tekst_moc);


		}


		sprintf(tekst_kadencja, "KAD = %d\0",  crank_candence );
		if(tekst_cadence_len != strlen(tekst_kadencja)){
			tekst_cadence_len = strlen(tekst_kadencja);
			LCD_setCursor(20, 100);
			LCD_writeString("               ");
		}
//		sprintf(tekst_kadencja, "KAD = %d\0",  crank_rate_ms );

		LCD_setCursor(20, 100);
		LCD_writeString(tekst_kadencja);


		vTaskDelay( 100 / portTICK_RATE_MS );
	}
}

void vReadTensometr(void *pvParameters){

	for (;;) {
//		float temp =  hx711_read_gram();
		uint64_t temp = hx711_avg_read(8);
		if(temp < 0){
			temp = 0;
		}
		xQueueSend( QueueTensometr, &temp, portMAX_DELAY);
		vTaskDelay( 200 / portTICK_RATE_MS );
	}


}


void vGpioHandlerTask(void *pvParameters) {

    while (1) {

        if ( xSemaphoreTake( xSemaphore, 1 / portTICK_RATE_MS ) == pdTRUE) {
//        	 crank_rate_ms < 10000
        	if(crank_rate_ms > 200 ){
        		crank_candence = ((1000 * 60) /  crank_rate_ms);
        		crank_rate_ms = 0;
        		//xTimerReset(timerHndlSec, 2000);

        	}else if (crank_rate_ms == 0){

        	}
        }
        else {

        }
    	crank_rate_ms++;
    }
    vTaskDelay( 1 / portTICK_RATE_MS );
}

void vCoutCadence(void *pvParameters) {

//	crank_candence

}

void  EXTI0_IRQHandler(void) {

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {

        xSemaphoreGiveFromISR( xSemaphore, &xHigherPriorityTaskWoken );

        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

//void EXTI0_IRQHandler(void)
//{
//	EXTI_ClearITPendingBit(EXTI_Line0);
//
//    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//
//    xSemaphoreGiveFromISR( xSemaphore, &xHigherPriorityTaskWoken );
//
//    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
//
//
//}


