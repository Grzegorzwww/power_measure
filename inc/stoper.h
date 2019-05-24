/*
 * stoper.h
 *
 *  Created on: 24.05.2019
 *      Author: gwarchol
 */

#ifndef STOPER_H_
#define STOPER_H_

#include "stm32f10x.h"
#include "stdbool.h"
#include "stdint.h"


typedef struct
{
  u8 u8_SecLow;
  u8 u8_SecHigh;
  u8 u8_MinLow;
  u8 u8_MinHigh;
  u8 u8_HourLow;
  u8 u8_HourHigh;
} StoperTime_t;


void addOneSecToStoper();


#endif /* STOPER_H_ */
