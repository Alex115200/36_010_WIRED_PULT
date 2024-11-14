
#include "globalIncludes.h"

/*
#include    <windows.h>
#include    <winbase.h>
#include    <mmsystem.h>

extern HANDLE uartRxEvent;
*/

__noreturn void TaskControl(void *pdata)
{
//    testLed();

    while(1){
      ResetWD(tsk_cont);
        OSTimeDly(80);
    	forceLED();
    }        
}
// =======================================================
// Подвал
//        res = WaitForSingleObject(uartRxEvent, INFINITE );


