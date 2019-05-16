#include "scan_timer.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "misc.h"


//Podstawa czasu to 50 Khz -> 0.02 ms
//czyli 1 ms = 0.02 * 50 -> czyli CC... = 50 dla 1 ms
//dziekuje za uwage

__IO uint16_t CC4 = 50 ;// 1ms to 50
__IO uint16_t CC3 = 500;// 1ms to 50 -> 500 to 10 ms id...
__IO uint16_t CC2 = 5000; // 100 ms
__IO uint16_t CC1 = 50000 ; //1s


volatile bool _1_ms_flag  =false;
volatile bool _10_ms_flag = false;
volatile bool _100_ms_flag = false;
volatile bool _1_s_flag = false;



bool getTimerChannelState(timer_channel_t channel){
	switch(channel){
	case TIMER_1ms:
		return _1_ms_flag;
		break;
	case TIMER_10ms:
		return _10_ms_flag;
		break;
	case TIMER_100ms:
		return _100_ms_flag;
		break;
	case TIMER_1s:
		return _1_s_flag;
		break;
	default :
		return false;
	}
}

void setTimerChannelState(timer_channel_t channel, bool state){
	switch(channel){
	case TIMER_1ms:
		 _1_ms_flag = state;
		break;
	case TIMER_10ms:
		_10_ms_flag = state;
		break;
	case TIMER_100ms:
		_100_ms_flag = state;
		break;
	case TIMER_1s:
		_1_s_flag = state;
		break;
	default :
		;
	}
}



void init_timer(){

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	// Wlacz taktowanie GPIOB i TIM1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	// Ustawienia ukladu podstawy czasu
	TIM_TimeBaseStructure.TIM_Period = 65535;
	TIM_TimeBaseStructure.TIM_Prescaler = 1440;  //fclk = 72MHz/1440 = 50kHz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);



	// Konfiguracja kanalu 1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CC1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Disable);

	// Konfiguracja kanalu 2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CC2;
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Disable);

	// Konfiguracja kanalu 3
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CC3;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);

	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Disable);

	// Konfiguracja kanalu 4
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CC4;
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Disable);


	// Wlaczenie przerwan od kanalow
	TIM_ITConfig(TIM1, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4, ENABLE);
	// Wlaczenie timera
	TIM_Cmd(TIM1, ENABLE);



	NVIC_InitTypeDef NVIC_InitStructure;

	// Wlacz przerwanie od TIM1
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}


void init_signle_shot_timer(){



}

volatile static bool x = false;




void TIM1_CC_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
		_1_s_flag = true;
		TIM_SetCompare1(TIM1, TIM_GetCapture1(TIM1) + CC1);
	}
	else if (TIM_GetITStatus(TIM1, TIM_IT_CC2) != RESET)
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_CC2);
		_100_ms_flag = true;
		TIM_SetCompare2(TIM1, TIM_GetCapture2(TIM1) + CC2);
	}
	else if (TIM_GetITStatus(TIM1, TIM_IT_CC3) != RESET)
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_CC3);
		_10_ms_flag = true;
		TIM_SetCompare3(TIM1, TIM_GetCapture3(TIM1) + CC3);
	}
	else if(TIM_GetITStatus(TIM1, TIM_IT_CC4) != RESET)
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_CC4);
		_1_ms_flag = true;
		TIM_SetCompare4(TIM1, TIM_GetCapture4(TIM1) + CC4);
	}
}
