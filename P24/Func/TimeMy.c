/************************** (C) Copyright 2013 Triol ***************************
* File Name          : TimeMy.c
* Author             : Kats Eyvhen
* TS Idea            : 2013.04.25
* TS Version         : 2013.04.25
* Description        : Function for work with time
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "globalIncludes.h"
#include <stdlib.h>


/* Private define ------------------------------------------------------------*/



/* Private macro -------------------------------------------------------------*/



/* Private types -------------------------------------------------------------*/



/* Private variables ---------------------------------------------------------*/



/* Private function prototypes -----------------------------------------------*/



/* Private functions ---------------------------------------------------------*/
//******************************************************************************
// Возвращает количество прошедших Tick'ов от прошлого значения
u32 GetTickLeft(u32 TickPrev)
{
	u32 TickCur;
	TickCur = OSTimeGet();

	// Учитываем переполнение u32
	if (TickPrev > TickCur)
		return TickCur + (0xFFFFFFFF - TickPrev + 1);

	return TickCur - TickPrev;
}
//******************************************************************************
// Возвращает количество прошедших секунд от прошлого значения
u16 GetSecLeft(u16 SecPrev)
{
	u16 SecCur;
	SecCur = (u16)GetSecCount();

	// Учитываем переполнение u16
	if (SecPrev > SecCur)
		return SecCur + (0xFFFF - SecPrev + 1);

	return SecCur - SecPrev;
}
//******************************************************************************
// Возвращает количество прошедших Tick'ов от прошлого значения в U16
u16 GetTickLeftU16(u16 TickPrev)
{
	u16 TickCur;
	TickCur = (u16)OSTimeGet();

	// Учитываем переполнение u16
	if (TickPrev > TickCur)
		return TickCur + (0xFFFF - TickPrev + 1);

	return TickCur - TickPrev;
}
//******************************************************************************


/************************** (C) Copyright 2013 Triol **************************/