/**
  **********************************  STM8S  ***********************************
  * @�ļ���     �� bsp_timer.h
  * @����       �� strongerHuang
  * @��汾     �� V2.2.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2017��04��10��
  * @ժҪ       �� TIM��ʱ��ͷ�ļ�
  ******************************************************************************/

/* �����ֹ�ݹ���� ----------------------------------------------------------*/
#ifndef _BSP_TIMER_H
#define _BSP_TIMER_H

/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "stm8s.h"


/* �궨�� --------------------------------------------------------------------*/

/* ȫ�ֱ��� ------------------------------------------------------------------*/
extern uint16_t gTIMTiming_Num;                            //��ʱ����
extern uint8_t  gTIMTiming_Flag;                           //��ʱ��־(0-��Ч 1-��Ч)

/* �������� ------------------------------------------------------------------*/
void TIMER_Initializes(void);

void TIMTiming_Nms(uint16_t Times);
void TIMTiming_Off(void);


#endif /* _BSP_TIMER_H */

/**** Copyright (C)2017 strongerHuang. All Rights Reserved **** END OF FILE ****/
