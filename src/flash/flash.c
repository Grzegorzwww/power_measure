/*
 * flash.c
 *
 *  Created on: 09.08.2018
 *      Author: gwarchol
 */
#include "flash/flash.h"

void FLASH_Init(void) {
	/* Next commands may be used in SysClock initialization function
	   In this case using of FLASH_Init is not obligatorily */
	/* Enable Prefetch Buffer */
	FLASH_PrefetchBufferCmd( FLASH_PrefetchBuffer_Enable);
	/* Flash 2 wait state */
	FLASH_SetLatency( FLASH_Latency_2);
}

void FLASH_ReadSettings(void) {
	//Read settings
	uint32_t *source_addr = (uint32_t *)MY_FLASH_PAGE_ADDR;
	uint32_t *dest_addr = (void *)&settings;
	for (uint16_t i=0; i<SETTINGS_WORDS; i++) {
		*dest_addr = *(__IO uint32_t*)source_addr;
		source_addr++;
		dest_addr++;
	}
}

void FLASH_WriteSettings(void) {
	FLASH_Unlock();
	FLASH_ErasePage(MY_FLASH_PAGE_ADDR);

	// Write settings
	uint32_t *source_addr = (void *)&settings;
	uint32_t *dest_addr = (uint32_t *) MY_FLASH_PAGE_ADDR;
	for (uint16_t i=0; i<SETTINGS_WORDS; i++) {
		FLASH_ProgramWord((uint32_t)dest_addr, *source_addr);
		source_addr++;
		dest_addr++;
	}

	FLASH_Lock();
}

void flash_set_date_storage(uint8_t day, uint8_t month, uint16_t year){
	settings.date_day = day;
	settings.date_month = month;
	settings.date_day = day;

	FLASH_WriteSettings();

}

void flash_get_date_storage(uint8_t *day, uint8_t *month, uint16_t *year){
	FLASH_ReadSettings();

	*day = settings.date_day;
	*month = settings.date_month;
	*year = settings.date_year;

}
