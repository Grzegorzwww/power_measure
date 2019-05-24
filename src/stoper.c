/*
 * stoper.c
 *
 *  Created on: 24.05.2019
 *      Author: gwarchol
 */
#include "stoper.h"




static StoperTime_t StoperTime;

static long int total_stoper_ticks = 0;

void addOneSecToStoper()
{
	total_stoper_ticks++;
}


void vReadStoperTime(unsigned char *data_buff){
	uint32_t timevar;
	timevar = total_stoper_ticks;
	timevar = timevar % 86400;
	StoperTime.u8_HourHigh = (uint8_t)(timevar / 3600 ) / 10;
	StoperTime.u8_HourLow = (uint8_t)(timevar / 3600 ) % 10;
	StoperTime.u8_MinHigh = (uint8_t)((timevar % 3600 ) / 60) / 10;
	StoperTime.u8_MinLow = (uint8_t)((timevar % 3600 ) / 60) % 10;
	StoperTime.u8_SecHigh = ((uint8_t)(timevar % 60 ) % 60) / 10;
	StoperTime.u8_SecLow = ((uint8_t)(timevar % 60 ) % 60) % 10;
	sprintf(data_buff, "%d%d:%d%d:%d%d",
			StoperTime.u8_HourHigh,
			StoperTime.u8_HourLow,
			StoperTime.u8_MinHigh,
			StoperTime.u8_MinLow,
			StoperTime.u8_SecHigh,
			StoperTime.u8_SecLow);
}


