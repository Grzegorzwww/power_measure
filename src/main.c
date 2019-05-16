/**
 ******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */


#define HSE_VALUE    ((uint32_t)8000000)


#include "stm32f10x.h"

#include "device_config.h"
#include <my_timers.h>
#include "usart_dma.h"
#include "uart_interrupt.h"
#include "scan_timer.h"
#include "touch_screen.h"

#include "stdbool.h"

#include "rtc.h"
#include "text.h"
#include "itoa.h"
#include "flash/flash.h"

//#include "usb/usb.h"
#include "adc.h"
#include "gpio.h"


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "my_tasks.h"



#ifdef DEBUG
extern void initialise_monitor_handles(void);
#endif


static void prvSetupHardware(void);




int main(void) {


#ifdef DEBUG
	initialise_monitor_handles();
	printf("start\n");
#endif



	BaseType_t xReturned;
	TaskHandle_t xHandle = NULL;



	prvSetupHardware();

	//

//	xReturned =  xTaskCreate(vControlADC, "control_adc_value", 600, NULL, 3, NULL);

	xTaskCreate(vLEDTask, "led_blink", 200, NULL, 1, NULL);

	 xTaskCreate(vDisplayRefresh, "display", 600, NULL, 8, NULL);

	 xTaskCreate(vReadTensometr, "tensometr", 600, NULL, 2, NULL);

	xReturned =  xTaskCreate(vGpioHandlerTask, "gpio", 100, NULL, 8, NULL);



//	if( xReturned == pdPASS )
//	{
//		printf("create ok \n");
//	}else{
//		printf("create not ok \n");
//	}

	// Start the scheduler
	vTaskStartScheduler();



	while (1) {
		;
	}
}

static void prvSetupHardware(void)
{

	RCC_Conf();

	device_init();

	//vInitADC();

	LCD_init();

	hx711_init();

	vGPIOInit();

	vxCreateRTOSMachanisms();

	// It's place to hardware configuration, like e.g. clock external source

}







