/*
 * timers.c
 *
 *  Created on: 05.12.2017
 *      Author: gwarchol
 */


#include <my_timers.h>


TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;

__IO uint16_t CCR1_Val = 40961;
__IO uint16_t CCR2_Val = 27309;
__IO uint16_t CCR3_Val = 24000;
__IO uint16_t CCR4_Val = 20000;
uint16_t PrescalerValue = 0;




void timer_2_init()
{


	  RCC_PCLK1Config(RCC_HCLK_Div4);

	  /* TIM2 clock enable */
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	  /* GPIOC clock enable */
//	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);


	  NVIC_InitTypeDef NVIC_InitStructure;

	  /* Enable the TIM2 global Interrupt */
	  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	  NVIC_Init(&NVIC_InitStructure);




	  /* Compute the prescaler value */
	  PrescalerValue = (uint16_t) (SystemCoreClock / 8000000) - 1;

	  /* Time base configuration */
	  TIM_TimeBaseStructure.TIM_Period = 65535;
	  TIM_TimeBaseStructure.TIM_Prescaler = 0;
	  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	  /* Prescaler configuration */
	  TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);

	  /* Output Compare Timing Mode configuration: Channel1 */
	  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	  TIM_OC1Init(TIM2, &TIM_OCInitStructure);

	  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);

	  /* Output Compare Timing Mode configuration: Channel2 */
	  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;

	  TIM_OC2Init(TIM2, &TIM_OCInitStructure);

	  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);

	  /* Output Compare Timing Mode configuration: Channel3 */
	  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;

	  TIM_OC3Init(TIM2, &TIM_OCInitStructure);

	  TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable);

	  /* Output Compare Timing Mode configuration: Channel4 */
	  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;

	  TIM_OC4Init(TIM2, &TIM_OCInitStructure);

	  TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Disable);

	  /* TIM IT enable */
	  TIM_ITConfig(TIM2, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4, ENABLE);

	  /* TIM2 enable counter */
	  TIM_Cmd(TIM2, ENABLE);



}



//void TIM2_IRQHandler(void){
//
//	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
//	{
//		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
//		//x ^= x;
//		TIM_SetCompare1(TIM2, TIM_GetCapture1(TIM2) + CCR1_Val);
//	}
//	else if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
//	{
//		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
//
//		TIM_SetCompare2(TIM2, TIM_GetCapture2(TIM2) + CCR2_Val);
//	}
//	else if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
//	{
//		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
//
//		TIM_SetCompare3(TIM2, TIM_GetCapture3(TIM2) + CCR3_Val);
//	}
//	else if(TIM_GetITStatus(TIM1, TIM_IT_CC4) != RESET)
//	{
//		TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);
//
//		TIM_SetCompare4(TIM2, TIM_GetCapture4(TIM2) + CCR4_Val);
//	}
//}
uint16_t capture = 0;

volatile static bool x = false;


void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

//    switch(x){
//    case true:
//    	x = false;
//		GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET );
//    	break;
//    case false:
//    	x = true;
//    	GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_SET );
//    	break;
//    }


    capture = TIM_GetCapture1(TIM2);
    TIM_SetCompare1(TIM2, capture + CCR1_Val);
  }
  else if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);

    /* Pin PC.07 toggling with frequency = 109.8 Hz */

    capture = TIM_GetCapture2(TIM2);
    TIM_SetCompare2(TIM2, capture + CCR2_Val);
  }
  else if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);

    /* Pin PC.08 toggling with frequency = 219.7 Hz */

    capture = TIM_GetCapture3(TIM2);
    TIM_SetCompare3(TIM2, capture + CCR3_Val);
  }
  else
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);

    /* Pin PC.09 toggling with frequency = 439.4 Hz */
    capture = TIM_GetCapture4(TIM2);
    TIM_SetCompare4(TIM2, capture + CCR4_Val);
  }
}





// TIM_TimeBaseInitTypeDef tim;
// NVIC_InitTypeDef nvic;

//void init_timers(){
//
//
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
//
//
//	TIM_TimeBaseStructInit(&tim);
//	tim.TIM_CounterMode = TIM_CounterMode_Up;
//	tim.TIM_Prescaler = 72;
//	tim.TIM_Period = 1000 - 1;
//	tim.TIM_RepetitionCounter = 0;
//	tim.TIM_ClockDivision = TIM_CKD_DIV1;
//	TIM_TimeBaseInit(TIM2, &tim);
//
//
//	TIM_ClearFlag( TIM2, TIM_FLAG_Update );
//	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
//
//	 nvic.NVIC_IRQChannel = TIM2_IRQn;
//	 nvic.NVIC_IRQChannelPreemptionPriority = 0;
//	 nvic.NVIC_IRQChannelSubPriority = 1;
//	 nvic.NVIC_IRQChannelCmd = ENABLE;
//	 NVIC_Init(&nvic);
//
//	 NVIC_EnableIRQ(TIM2_IRQn);
//
//	TIM_Cmd(TIM2, ENABLE);
//
//
//
//
//}
//

//}




