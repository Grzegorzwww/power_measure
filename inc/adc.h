/*
 * adc.h
 *
 *  Created on: 06.05.2019
 *      Author: gwarchol
 */

#ifndef ADC_H_
#define ADC_H_

#include "stm32f10x.h"

#define ADC_FILER_SIZE 5

static void sortowanie(uint16_t *source, uint16_t len);
void sortowanie_v2(unsigned short *d, unsigned short x);

volatile uint16_t xGetADCValue();

void vInitADC();

#endif /* ADC_H_ */
