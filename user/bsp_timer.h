/**
  **********************************  STM8S  ***********************************
  * @???t??     ?��o bsp_timer.h
  * @?��????       ?��o strongerHuang
  * @?a????��?     ?��o V2.2.0
  * @???t????��?   ?��o V1.0.0
  * @����??��2       ?��o 2017?��o04??10����?
  * @?a����a       ?��o TIM??�졧o?��???��a?��???t
  ******************************************************************************/

/* ??�졧��??�衧��?1|��Y1��|??��1o? ----------------------------------------------------------*/
#ifndef _BSP_TIMER_H
#define _BSP_TIMER_H

/* ??��1o?|��?��a?��???t --------------------------------------------------------------*/
#include "stm8s.h"


/* o��o??�졧��? --------------------------------------------------------------------*/

/* ����????��?����? ------------------------------------------------------------------*/
enum
{
	BSP_TIMER_0,
	BSP_TIMER_1,
	BSP_TIMER_2,
	BSP_TIMER_3,

	BSP_TIMER_MAX
};


/* o?����oy��|��o??? ------------------------------------------------------------------*/
void bsp_start_timer(int timerId, unsigned short millTime, void (*callback)(int id));
void bsp_stop_timer(int timerId);

void bsp_timer_proc(void);
void bsp_timer_hw_enable(int timerId, unsigned char flag);
void bsp_timer_hw_init(void);

#endif /* _BSP_TIMER_H */

/**** Copyright (C)2017 strongerHuang. All Rights Reserved **** END OF FILE ****/

