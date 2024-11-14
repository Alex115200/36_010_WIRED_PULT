


#include <ucos_ii.h>
#include "OS_Obj.h"

/*----------------------------------------------------------
Создание объектов ядра
------------------------------------------------------------*/

OS_EVENT *UARTSem;
OS_EVENT *AT45Sem;
OS_EVENT *FRAMSem;
OS_EVENT *SPISem;
OS_EVENT *SemMenu;

OS_EVENT *MBoxKbdReq;
OS_EVENT *MBoxKbdResp;

OS_EVENT *QueueDrvIn;
OS_EVENT *QueueDrvOut;
OS_EVENT *QueueDrvInStartStop;
OS_EVENT *QueueDrvOutStartStop;

#ifdef TEST_P24
    OS_EVENT *UARTSemTx;
    OS_EVENT *UARTSemRx;
#endif

#ifdef _PROJECT_IAR_
    OS_EVENT *SemMenu;
#endif // _PROJECT_IAR_