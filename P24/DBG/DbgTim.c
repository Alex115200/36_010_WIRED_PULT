
#include "globalIncludes.h"

    u32     measTim;
    u32     measMin;
    u32     measMax;
    u32     beginTime;


MeasTimeDbg_type MeasTimeDbg[QNT_MEAS_EVENT] = {
    0, 0xFFFFFFFF, 0, 0,
    0, 0xFFFFFFFF, 0, 0,
    0, 0xFFFFFFFF, 0, 0,
    0, 0xFFFFFFFF, 0, 0,
    0, 0xFFFFFFFF, 0, 0,
    0, 0xFFFFFFFF, 0, 0,
    0, 0xFFFFFFFF, 0, 0,
    0, 0xFFFFFFFF, 0, 0,
    0, 0xFFFFFFFF, 0, 0,
    0, 0xFFFFFFFF, 0, 0,
/*    0, 0xFFFFFFFF, 0, 0,
    0, 0xFFFFFFFF, 0, 0,
    0, 0xFFFFFFFF, 0, 0,
    0, 0xFFFFFFFF, 0, 0,
    0, 0xFFFFFFFF, 0, 0,
    0, 0xFFFFFFFF, 0, 0,
    0, 0xFFFFFFFF, 0, 0,
    0, 0xFFFFFFFF, 0, 0,
    0, 0xFFFFFFFF, 0, 0,
    0, 0xFFFFFFFF, 0, 0,*/
};

// ==================================================================
//               Таймер измерений событий
// ==================================================================
void TIM5_Debug_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    //TIM_OCInitTypeDef  TIM_OCInitStructure;
    //u32 period;

    //period = SystemCoreClock / freq / 2;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    DBGMCU_APB1PeriphConfig(DBGMCU_TIM5_STOP, ENABLE);

    TIM_TimeBaseStructure.TIM_Period          = 0xFFFFFFFF;
    TIM_TimeBaseStructure.TIM_Prescaler       = 59;//PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision   = 0;
    TIM_TimeBaseStructure.TIM_CounterMode     = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    TIM_SetAutoreload(TIM5, 0xFFFFFFFF);

    TIM_ARRPreloadConfig(TIM5, ENABLE);

    TIM_Cmd(TIM5, ENABLE);
}
// ==================================================================
// ==================================================================
void BeginMeasEvent(u8 numEvent)
{
    if (numEvent < QNT_MEAS_EVENT) {
        MeasTimeDbg[numEvent].beginTime = TIM_GetCounter(TIM5);
    }
}
// ==================================================================
// ==================================================================
void EndMeasEvent(u8 numEvent)
{
    u32 tmp = 0;

    if (numEvent < QNT_MEAS_EVENT) {
        tmp = TIM_GetCounter(TIM5);
        tmp = tmp - MeasTimeDbg[numEvent].beginTime;
            MeasTimeDbg[numEvent].measTim = tmp;
            if(tmp > MeasTimeDbg[numEvent].measMax)	MeasTimeDbg[numEvent].measMax = tmp;
            if(tmp < MeasTimeDbg[numEvent].measMin)	MeasTimeDbg[numEvent].measMin = tmp;
    }
}
// ==================================================================
void debugStop()
{
    return;
}
