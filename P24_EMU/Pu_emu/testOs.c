
#include "globalIncludes.h"


/*
extern 		__no_init OS_STK KBDStack[STKKBDSIZE];
extern 		__no_init OS_STK DRVStack[STKDRVSIZE];
extern 		__no_init OS_STK LCDStack[STKLCDSIZE];
*/


#define  STKSIZE 	256

OS_STK Stack_1[STKSIZE];
OS_STK Stack_2[STKSIZE];
OS_STK Stack_3[STKSIZE];


extern keyState_type       KeyState;
extern lampState_type		lampState;


void task1(void *pdata)
{
  volatile	u32 		cnt = 0;
	while(1){
		OSTimeDly(400);
		cnt++;
	}

}

void task2(void *pdata)
{
  volatile	u32 		cnt = 0;
	while(1){
		OSTimeDly(200);

        if(KeyState.bit.UP != 0){
            memcpy(VideoRAM, Triol, sizeof(VideoRAM));
        }

    	lampState.all =  KeyState.all;

		cnt++;
	}

}

void task3(void *pdata)
{
  volatile	u32 		cnt = 0;
	while(1){
		OSTimeDly(300);
		cnt++;
	}

}

/*
void __stdcall  OS_init(void)
{
	OSInit();

	OSTaskCreate(task1, (void *)0, &Stack_1[STKSIZE - 1], 4);
	OSTaskCreate(task2, (void *)0, &Stack_2[STKSIZE - 1], 5);
	OSTaskCreate(task3, (void *)0, &Stack_3[STKSIZE - 1], 6);

	OSStart();
}
*/