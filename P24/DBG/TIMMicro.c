
#include "globalIncludes.h"

// =================================================
#define tim9PSinit      1200      /* 120МГц / 120 = 10мкс */
#define time9(ms)   (ms * 100)  /* Преобразование в мс */
// =================================================
// =================================================
void TIM9MicroInit(void)
{
    NVIC_InitTypeDef                NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef         TIM_TimeBaseStructure;

    NVIC_InitStructure.NVIC_IRQChannel                      = TIM1_BRK_TIM9_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 15;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
    DBGMCU_APB1PeriphConfig(DBGMCU_TIM9_STOP, ENABLE);

    TIM_TimeBaseStructure.TIM_Prescaler       = (tim9PSinit-1);//  60000 * 4;//4мс
    TIM_TimeBaseStructure.TIM_Period          = time9( 20.1 );

    TIM_TimeBaseStructure.TIM_ClockDivision   = 0;
    TIM_TimeBaseStructure.TIM_CounterMode     = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);

    DBGMCU_APB2PeriphConfig(DBGMCU_TIM9_STOP, ENABLE);

    TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);
    TIM_ClearITPendingBit(TIM9, TIM_IT_Update);
    TIM_ITConfig(TIM9, TIM_IT_Update, ENABLE);

    TIM_SelectOnePulseMode(TIM9, TIM_OPMode_Single);

//    TIM_Cmd(TIM9, ENABLE);
}
// =================================================
// =================================================

/*
void TIM1_BRK_TIM9_IRQHandler(void)
{

    TIM_ClearITPendingBit(TIM9, TIM_IT_Update);
}
*/
// =================================================
