/*
 * cellpipe.c
 *
 *  Created on: Apr 1, 2016
 *      Author: Isaac
 */

#include "cellpipe.h"
#include "timeout.h"
#include "stm32l1xx_nucleo.h"
#include <stm32l1xx.h>

void CellPipe_RCC_Config(void);
void CellPipe_GPIO_Config(void);
void CellPipe_USART_Config(uint32_t);
void CellPipe_NVIC_Config(void);

void CellPipe_TurnOn(void);

void CellPipe_Init(void)
{
	 CellPipe_RCC_Config();
	 CellPipe_GPIO_Config();
	 CellPipe_USART_Config(115200);
	 CellPipe_NVIC_Config();

	 GPIO_ResetBits(GPIOA,GPIO_Pin_14);
	 GPIO_ResetBits(GPIOA,GPIO_Pin_15);
	 GPIO_SetBits(GPIOC,GPIO_Pin_3);

	 Timeout_Init();

	 Timeout_Start(2000);
	 while(timed_out == 0);

	 CellPipe_TurnOn();
}

void CellPipe_TurnOn(void)
{
	uint8_t stat = Bit_RESET;
	//Turn on MODEN
	GPIO_SetBits(GPIOA,GPIO_Pin_14);
	Timeout_Start(500);

	while(timed_out == 0);

	do
	{
		//Power On Pulse
		GPIO_SetBits(GPIOB,GPIO_Pin_7);

		Timeout_Start(5500);

		while((timed_out == 0) && (stat == Bit_RESET))
		{
			stat = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_13);
		}

		Timeout_Start(500);
		while(timed_out == 0);

		GPIO_ResetBits(GPIOB,GPIO_Pin_7);

		if(stat == Bit_RESET)
		{
			Timeout_Start(10000);

			while(timed_out == 0);
		}
	}while(/*stat == Bit_RESET*/0);
}

void CellPipe_RCC_Config(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB1PeriphClockLPModeCmd(RCC_APB1Periph_USART2,ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
	RCC_APB1PeriphClockLPModeCmd(RCC_APB1Periph_UART4,ENABLE);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	RCC_AHBPeriphClockLPModeCmd(RCC_AHBPeriph_GPIOA,ENABLE);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,ENABLE);
	RCC_AHBPeriphClockLPModeCmd(RCC_AHBPeriph_GPIOC,ENABLE);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	RCC_AHBPeriphClockLPModeCmd(RCC_AHBPeriph_GPIOB,ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	RCC_APB1PeriphClockLPModeCmd(RCC_APB1Periph_PWR,ENABLE);
}

void CellPipe_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

	//GPIO For Cell UART4
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_Init(GPIOC,&GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4);

	//GPIO For Debug USART2
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);

	//GPIO For MODEN, ON/OFF, and STAT_LED
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_Init(GPIOA,&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOA,&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
}

void CellPipe_USART_Config(uint32_t b)
{
	USART_InitTypeDef USART_InitStruct;

	USART_InitStruct.USART_BaudRate = b;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;

	USART_Init(USART2,&USART_InitStruct);
	USART_Init(UART4,&USART_InitStruct);

	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	USART_ITConfig(UART4,USART_IT_RXNE,ENABLE);

	USART_Cmd(USART2,ENABLE);
	USART_Cmd(UART4,ENABLE);
}

void CellPipe_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;

	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;

	NVIC_Init(&NVIC_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&NVIC_InitStruct);
}

void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET)
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		while(USART_GetFlagStatus(UART4,USART_FLAG_TXE) != SET);
		UART4->DR = USART2->DR;
		//STM_EVAL_LEDToggle(LED2);
	}
}

void UART4_IRQHandler(void)
{
	if(USART_GetITStatus(UART4,USART_IT_RXNE) == SET)
	{
		USART_ClearITPendingBit(UART4,USART_IT_RXNE);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) != SET);
		USART2->DR = UART4->DR;
	}
}
