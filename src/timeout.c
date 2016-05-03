/*
 * timeout.c
 *
 *  Created on: Jan 15, 2016
 *      Author: Isaac
 */

#include "stm32l1xx.h"

TIM_TimeBaseInitTypeDef TIM_InitStruct;

volatile uint8_t timed_out;

/*
 * @brief Initializes the structs and interrupts for the timeout timer
 * @retval None
 */
void Timeout_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB1PeriphClockLPModeCmd(RCC_APB1Periph_TIM3,ENABLE);

	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&NVIC_InitStruct);

	TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV4;
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStruct.TIM_Prescaler = 32000;
}

/*
 * @brief Starts a timeout countdown
 * @param t: The number of milliseconds to wait
 * @retval None
 */
void Timeout_Start(uint16_t t)
{
	if(t > 0)
	{
		TIM_DeInit(TIM3);

		TIM_InitStruct.TIM_Period = t;

		TIM_TimeBaseInit(TIM3,&TIM_InitStruct);
		TIM_SelectOnePulseMode(TIM3,TIM_OPMode_Single);
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
		TIM_Cmd(TIM3,ENABLE);

		timed_out = 0;
	}
	else
	{
		timed_out = 1;
	}
}

void TIM3_IRQHandler(void)
{
	//Indicate that the timeout has gone off
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		timed_out = 1;
	}
}
