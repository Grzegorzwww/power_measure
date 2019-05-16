/*
 * flash.h
 *
 *  Created on: 09.08.2018
 *      Author: gwarchol
 */

#ifndef FLASH_FLASH_H_
#define FLASH_FLASH_H_
#include "stm32f10x.h"
#include "stdbool.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_flash.h"


#define MY_FLASH_PAGE_ADDR 0x800FC00

#define SETTEINGS_MODYFIED 0xFF
#define SETTEINGS_NOT_MODYFIED 0x00



typedef	struct
  {

	uint8_t date_day;
	uint8_t date_month;
	uint16_t date_year;
	uint32_t write_param;


	// 8 byte = 2  32-bits words.  It's - OK
	// !!! Full size (bytes) must be a multiple of 4 !!!
  } tpSettings;

tpSettings settings;

#define SETTINGS_WORDS sizeof(settings)/4


void FLASH_Init(void);
void FLASH_ReadSettings(void);
void FLASH_WriteSettings(void);

void flash_set_date_storage(uint8_t day, uint8_t month, uint16_t year);
void flash_get_date_storage(uint8_t *day, uint8_t *month, uint16_t *year);


bool read_date_was_modyfied();
void write_date_was_modyfied();





#endif /* FLASH_FLASH_H_ */
