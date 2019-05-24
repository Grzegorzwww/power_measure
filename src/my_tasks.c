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
#include "stoper.h"
#include "calculations.h"



QueueHandle_t QueueAdcValue = NULL;
QueueHandle_t QueueTensometr = NULL;
SemaphoreHandle_t xSemaphore = NULL;
xTimerHandle timerHndlSec = NULL;

static int crank_rate_ms = 0;
static int crank_candence = 0;

static int mass = 0;

int poprzednia_probka = 0;
int zakres = 160;


inline int ipIncrementalFilter(int aktualna_probka){

	if((aktualna_probka - poprzednia_probka) > zakres ){
		aktualna_probka = (poprzednia_probka + zakres);
		return aktualna_probka;

	}else if((aktualna_probka - poprzednia_probka)  < (-zakres) ){
		aktualna_probka = (poprzednia_probka - zakres);
		return aktualna_probka;

	}else{
		return aktualna_probka;
	}

}




void vTimerCallbackMinCadenceExpired(xTimerHandle pxTimer) {
	//

	if(crank_rate_ms > 0 ){
		crank_candence = ((1000 * 60) /  crank_rate_ms);
		crank_rate_ms = 0;
		xTimerReset(timerHndlSec, 2000);
		if (crank_candence  <= 30){
			crank_rate_ms = 0;
			crank_candence  = 0;
		}
	}
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
	for (;;) {


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


void vOneSecondInterval(void *pvParameters)
{
	for (;;) {
		static bool led_toogle = false;
		addOneSecToStoper();

		calculate_work();

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




void vDisplayRefresh(void *pvParameters)
{
	static int tekst_len = 0;
	static int tekst_cadence_len = 0;
	static int tekst_moc_len = 0;
	static int tekst_praca_len = 0;;
	static int tekst_czas_len = 0;;
	unsigned short temp_adc =0;


	char tekst_waga[20];
	char tekst_kadencja[20];
	char tekst_moc[20];
	char tekst_praca[20];
	char tekst_czas[20];
	unsigned int tensometr_val =0;


	LCD_setTextSize(3);
	LCD_setTextColor(WHITE);
	LCD_setTextBgColor(BLACK);

	for (;;) {

		tekst_waga[0] = '\0';
		tekst_kadencja[0] = '\0';


		while(xQueueReceive(QueueTensometr, &tensometr_val, 1 / portTICK_RATE_MS ) == pdTRUE){
			tensometr_val = ((tensometr_val / 10000)  *  4.240 )- 26.0;
			if(tensometr_val > 10000){
				tensometr_val = 0;
			}

			mass = tensometr_val;
			sprintf(tekst_waga, "Waga =  %d [g]\0",  tensometr_val);
			if(tekst_len != strlen(tekst_waga)){
				tekst_len = strlen(tekst_waga);
				LCD_setCursor(2, 20);
				LCD_writeString("                ");
			}
			LCD_setTextColor(WHITE);
			LCD_setCursor(2, 20);
			LCD_writeString(tekst_waga);


		}

		LCD_setTextColor(YELLOW);
		sprintf(tekst_kadencja, "Cadencja = %d \0",  crank_candence );
		if(tekst_cadence_len != strlen(tekst_kadencja)){
			tekst_cadence_len = strlen(tekst_kadencja);
			LCD_setCursor(2, 60);
			LCD_writeString("                ");

		}


		LCD_setTextColor(RED);
		LCD_setCursor(2, 100);
		sprintf(tekst_moc, "Moc =  %d [W]\0",  iGetPower());
		if(tekst_moc_len != strlen(tekst_moc)){
			tekst_moc_len = strlen(tekst_moc);
			LCD_setCursor(2, 100);
			LCD_writeString("                 ");

		}


		LCD_setTextColor(GREEN);

		LCD_setCursor(2, 140);
		sprintf(tekst_praca, "Praca = %d [Kj]\0", (int)fGetWork());

		if(tekst_praca_len != strlen(tekst_praca)){
			tekst_praca_len = strlen(tekst_praca);
			LCD_setCursor(2, 140);
			LCD_writeString("                 ");

		}

		LCD_setTextColor(ORANGE);
		LCD_setCursor(2, 180);
		char temp_tekst[9];
		vReadStoperTime(temp_tekst);
		sprintf(tekst_czas, "%s%s\0","Czas = ", temp_tekst);
		if(tekst_czas_len != strlen(tekst_czas)){
			tekst_czas_len = strlen(tekst_czas);
			LCD_setCursor(2, 180);
			LCD_writeString("                 ");

		}


		LCD_setTextColor(YELLOW);
		LCD_setCursor(2, 60);
		LCD_writeString(tekst_kadencja);


		LCD_setTextColor(RED);
		LCD_setCursor(2, 100);
		LCD_writeString(tekst_moc);


		LCD_setTextColor(GREEN);
		LCD_setCursor(2, 140);
		LCD_writeString(tekst_praca);


		LCD_setTextColor(ORANGE);
		LCD_setCursor(2, 180);
		LCD_writeString(tekst_czas);


		vTaskDelay( 250 / portTICK_RATE_MS );
	}
}


void vCalculatePower(void *pvParameters)
{
	while(1){
	 calculate_power(mass, crank_candence);
		vTaskDelay( 200 / portTICK_RATE_MS );
	}

}



void vReadTensometr(void *pvParameters){

	for (;;) {
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
			if(crank_rate_ms > 100 ){
				crank_candence = ((1000 * 60) /  crank_rate_ms);

				crank_candence = ipIncrementalFilter(crank_candence);

				crank_rate_ms = 0;
				xTimerReset(timerHndlSec, 2000);

			}else if (crank_rate_ms == 0){

			}
		}
		else {

		}
		crank_rate_ms++;
	}
	vTaskDelay( 1 / portTICK_RATE_MS );
}


void vGpioScanPins(void *pvParameters) {

	while(1){
		scanButtonsPins();
		vTaskDelay( 1 / portTICK_RATE_MS );
	}

}



//void  EXTI0_IRQHandler(void) {
//
//    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//
//    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
//
//        xSemaphoreGiveFromISR( xSemaphore, &xHigherPriorityTaskWoken );
//
//        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
//
//        EXTI_ClearITPendingBit(EXTI_Line0);
//    }
//}

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


