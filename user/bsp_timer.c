/**
  **********************************  STM8S  ***********************************
  * @???     : bsp_timer.c
  * @??       : strongerHuang
  * @???     : V2.2.0
  * @????   : V1.0.0
  * @??       : 2017?04?10?
  * @??       : TIM??????
  ******************************************************************************/
/*----------------------------------------------------------------------------
  ????:
  2017-04-10 V1.0.0:????
  ----------------------------------------------------------------------------*/
/* ?????? --------------------------------------------------------------*/
#include "bsp_timer.h"

#define BSP_IRQ_TIME	1		//ms


typedef struct
{
	//unsigned short millTime;
	unsigned char  used;
	unsigned short irqCount;
	unsigned short irqCountMax;
	void (*callback)(int timeId);
} bsp_timer_t;

static bsp_timer_t	bsp_timer_array[BSP_TIMER_MAX];

void bsp_timer_array_init(void)
{
	int i;

	for(i=0; i<BSP_TIMER_MAX; i++)
	{
		bsp_timer_array[i].used = 0;
		bsp_timer_array[i].irqCount = 0;
		bsp_timer_array[i].irqCountMax = 0;
		bsp_timer_array[i].callback = 0;
	}
}

void bsp_timer_update_count(int timerId)
{
	//if(timerId < BSP_TIMER_MAX)
	//{
		bsp_timer_array[timerId].irqCount++;
	//}
}

void bsp_start_timer(int timerId, unsigned short millTime, void (*callback)(int id))
{
	if(timerId >= BSP_TIMER_MAX)
		return;

	bsp_timer_array[timerId].irqCount = 0;
	if(millTime%BSP_IRQ_TIME)
		bsp_timer_array[timerId].irqCountMax = ((unsigned short)(millTime/BSP_IRQ_TIME))+1;
	else
		bsp_timer_array[timerId].irqCountMax = ((unsigned short)(millTime/BSP_IRQ_TIME));
	
	bsp_timer_array[timerId].callback = callback;
	bsp_timer_array[timerId].used = 1;
	bsp_timer_hw_enable(timerId, 1);
}

void bsp_stop_timer(int timerId)
{
	if(timerId >= BSP_TIMER_MAX)
		return;

	bsp_timer_array[timerId].used = 0;
	bsp_timer_array[timerId].callback = 0;
	bsp_timer_hw_enable(timerId, 0);
}


void bsp_timer_proc(void)
{
	int i;

	for(i=0; i<BSP_TIMER_MAX; i++)
	{
		if(bsp_timer_array[i].used)
		{
			if(bsp_timer_array[i].irqCount >= bsp_timer_array[i].irqCountMax)
			{
				bsp_timer_array[i].irqCount = 0;
				if(bsp_timer_array[i].callback)
				{
					//console_write("timer 0 callback \n");
					bsp_timer_array[i].callback(i);
				}
			}
		}
	}
}

void bsp_timer_hw_enable(int timerId, unsigned char flag)
{
	if(timerId >= BSP_TIMER_MAX)
		return;

	if(flag)
	{
		if(timerId == BSP_TIMER_0)
		{
			TIM1_SetCounter(0); 						   //?????
			TIM1_Cmd(ENABLE);							   //?????
		}
		else if(timerId == BSP_TIMER_1)
		{
			TIM2_SetCounter(0); 						   //?????
			TIM2_Cmd(ENABLE);							   //?????
		}
		else if(timerId == BSP_TIMER_2)
		{
#if defined(STM8S105)
			TIM3_SetCounter(0); 						   //?????
			TIM3_Cmd(ENABLE);							   //?????
#endif
		}
		else if(timerId == BSP_TIMER_3)
		{
			TIM4_SetCounter(0); 						   //?????
			TIM4_Cmd(ENABLE);							   //?????
		}
	}
	else
	{
		if(timerId == BSP_TIMER_0)
		{
			TIM1_SetCounter(0);
			TIM1_Cmd(DISABLE);
		}
		else if(timerId == BSP_TIMER_1)
		{
			TIM2_SetCounter(0);
			TIM2_Cmd(DISABLE);
		}
		else if(timerId == BSP_TIMER_2)
		{
#if defined(STM8S105)
			TIM3_SetCounter(0);
			TIM3_Cmd(DISABLE);
#endif
		}
		else if(timerId == BSP_TIMER_3)
		{
			TIM4_SetCounter(0);
			TIM4_Cmd(DISABLE);
		}
	}
}



/************************************************
???? : bsp_timer_hw_init
?    ? : ??????
?    ? : ?
? ? ? : ?
?    ? : strongerHuang
*************************************************/
void bsp_timer_hw_init(void)
{
	TIM1_TimeBaseInit(16-1,TIM1_COUNTERMODE_UP, 1000, 0);  //??1ms (16M/16/1000 = 1000)
	TIM1_ARRPreloadConfig(ENABLE);
	TIM1_ClearFlag(TIM1_FLAG_UPDATE);			   //?????
	TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);		   //????UPDATE??

	TIM2_TimeBaseInit(TIM2_PRESCALER_128, 125-1);  //??1ms (16M/128/125 = 1000)
	TIM2_ClearFlag(TIM2_FLAG_UPDATE);			   //?????
	TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);		   //????UPDATE??

#if defined(STM8S105)
	TIM3_TimeBaseInit(TIM3_PRESCALER_128, 125-1);  //??1ms (16M/128/125 = 1000)
	TIM3_ClearFlag(TIM3_FLAG_UPDATE);			   //?????
	TIM3_ITConfig(TIM3_IT_UPDATE, ENABLE);		   //????UPDATE??
#endif	
	TIM4_TimeBaseInit(TIM4_PRESCALER_128, 125-1);  //??1ms (16M/128/125 = 1000)
	TIM4_ClearFlag(TIM4_FLAG_UPDATE);              //?????
	TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);         //????UPDATE??

	bsp_timer_array_init();

	enableInterrupts();                           //??????
}


/**** Copyright (C)2017 strongerHuang. All Rights Reserved **** END OF FILE ****/
