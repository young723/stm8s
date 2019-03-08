/**
  **********************************  STM8S  ***********************************
  * @???t??     ?那o bsp_timer.h
  * @?芍????       ?那o strongerHuang
  * @?a????角?     ?那o V2.2.0
  * @???t????角?   ?那o V1.0.0
  * @““??“2       ?那o 2017?“o04??10““?
  * @?a“～a       ?那o TIM??∫“o?角???“a?∟???t
  ******************************************************************************/

/* ??∫“～??∟“∟?1|足Y1“|??“1o? ----------------------------------------------------------*/
#ifndef _BSP_TIMER_H
#define _BSP_TIMER_H

/* ??“1o?|足?“a?∟???t --------------------------------------------------------------*/
#include "stm8s.h"


/* o“o??∫“～? --------------------------------------------------------------------*/

/* ““????角?“⊿? ------------------------------------------------------------------*/
enum
{
	BSP_TIMER_0,
	BSP_TIMER_1,
	BSP_TIMER_2,
	BSP_TIMER_3,

	BSP_TIMER_MAX
};


/* o?ㄓ“oy“|“o??? ------------------------------------------------------------------*/
void bsp_start_timer(int timerId, unsigned short millTime, void (*callback)(int id));
void bsp_stop_timer(int timerId);

void bsp_timer_proc(void);
void bsp_timer_hw_enable(int timerId, unsigned char flag);
void bsp_timer_hw_init(void);

#endif /* _BSP_TIMER_H */

/**** Copyright (C)2017 strongerHuang. All Rights Reserved **** END OF FILE ****/

