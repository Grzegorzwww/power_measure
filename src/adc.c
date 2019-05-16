/*
 * adc.c
 *
 *  Created on: 06.05.2019
 *      Author: gwarchol
 */


#include "adc.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"


volatile uint16_t ADCBuffer[] = {0xAAAA};

volatile uint16_t ADCFilterBuff[ADC_FILER_SIZE] = {0,0,0,0,0};

static void sortowanie(uint16_t *source, uint16_t len){
	int i, j;
	int temp = 0;
    for(i = 0; i < len - 1 ; i++){
        for(j = 0; j < len-1 ; j++){
            if(source[j] > source[j+1]){
                temp = source[j];
                source[j] = source[j+1];
                source[j+1] = temp;
            }
            else{
                continue;
            }
        }
    }
}

void sortowanie_v2(unsigned short *d, unsigned short x)
{
	int j, i;
	int temp = 0;
	for(j = x - 1; j > 0; j--){
		for(i = 0; i < j; i++)
			if(d[i] > d[i + 1]){
				temp = d[i];
				d[i] = d[i + 1];
				d[i + 1] = temp;
			}
	}
}


volatile uint16_t vADCFilter(uint16_t sample)
{
	int i;
	for(i = 0; i < ADC_FILER_SIZE - 1; i++){
		ADCFilterBuff[i+1] = ADCFilterBuff[i];
	}
	ADCFilterBuff[0] = sample;
	//sortowanie(ADCFilterBuff, ADC_FILER_SIZE);
	return ADCFilterBuff[2];
}


inline volatile uint16_t xGetADCValue(){
	return vADCFilter(ADCBuffer[0] & ~(0x07 << 0));
	//return vADCFilter(ADCBuffer[0]);
}



void vInitADC()
{

	RCC->APB2ENR |= RCC_APB2Periph_ADC1;
	RCC->APB2ENR |= RCC_APB2Periph_AFIO;
	RCC->APB2ENR |= RCC_APB2Periph_GPIOA;
	RCC->AHBENR |= RCC_AHBPeriph_DMA1;

	//DMA CONFIG
	DMA1_Channel1->CCR &= (uint32_t)0xFFFF800F; // CLEAR

	DMA1_Channel1->CCR |= DMA_DIR_PeripheralSRC;
	DMA1_Channel1->CCR |= DMA_Mode_Circular;
	DMA1_Channel1->CCR |= DMA_PeripheralInc_Disable;
	DMA1_Channel1->CCR |= DMA_MemoryInc_Enable;
	DMA1_Channel1->CCR |= DMA_MemoryDataSize_HalfWord;
	DMA1_Channel1->CCR |= DMA_PeripheralDataSize_HalfWord;
	DMA1_Channel1->CCR |= DMA_Priority_High;
	DMA1_Channel1->CCR |= DMA_M2M_Disable;

	DMA1_Channel1->CNDTR = 1; 						// DMA_BufferSize;
	DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;
	DMA1_Channel1->CMAR = (uint32_t)ADCBuffer;

	DMA1_Channel1->CCR |= DMA_CCR1_EN; // ENABLE DMA
	// DMA1_Channel1->CCR &= (uint16_t)(~DMA_CCR1_EN); // DISBALE DMA

	// GPIO CONFIG
	GPIOA->CRL &= ~GPIO_CRL_CNF1;
	GPIOA->CRL &= ~GPIO_CRL_MODE1;


	ADC1->CR1 |= ADC_Mode_Independent;
	ADC1->CR1 |= (1 << 8); 	// ADC_ScanConvMode

	ADC1->CR2 |= ADC_DataAlign_Right;
	ADC1->CR2 |= ADC_ExternalTrigConv_None;
	ADC1->CR2 |= (1 << 1); 	//  ADC_ContinuousConvMode

	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_7Cycles5);


    ADC1->CR2 |= (1 << 0); // ADC ON

    ADC1->CR2 |= (1 << 8 ); // DMA ON

	ADC1->CR2 |= (1 << 3); // reset calibration

	while ((ADC1->CR2 & (1 << 3)) != (uint32_t)RESET){
		;
	}

	ADC1->CR2 |= (1 << 2);

	while ((ADC1->CR2 & (1 << 2)) != (uint32_t)RESET){
		;
	}

	ADC1->CR2 |= (1 << 20 ); // EXTTRIG
	ADC1->CR2 |= (1 << 22 ); // SWSTART


}
