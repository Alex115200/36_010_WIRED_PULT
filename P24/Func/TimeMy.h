/************************** (C) Copyright 2013 Triol ***************************
* File Name          : TimeMy.h
* Author             : Kats Eyvhen
* TS Idea            : 2013.04.25
* TS Version         : 2013.04.25
* Description        : Function for work with time
*******************************************************************************/

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef TIME_MY_H
#define TIME_MY_H



/* Includes ------------------------------------------------------------------*/



/* Exported define -----------------------------------------------------------*/
// ���������� ���������� ��������� ������ �� ������ �������
#define GetSecCount() ((u16)(OSTimeGet() / 1000))


/* Exported macro ------------------------------------------------------------*/



/* Exported types ------------------------------------------------------------*/



/* Exported constants --------------------------------------------------------*/



/* Exported variables --------------------------------------------------------*/



/* Exported function prototypes ----------------------------------------------*/
// ���������� ���������� ��������� Tick'�� �� �������� ��������
u32 GetTickLeft(u32 TickPrev);
// ���������� ���������� ��������� ������ �� �������� ��������
u16 GetSecLeft(u16 SecPrev);
// ���������� ���������� ��������� Tick'�� �� �������� �������� � U16
u16 GetTickLeftU16(u16 TickPrev);


/* Exported functions --------------------------------------------------------*/



#endif // TIME_MY_H

/************************** (C) Copyright 2013 Triol **************************/