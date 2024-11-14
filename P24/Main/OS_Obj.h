#ifndef OS_OBJ_H
#define OS_OBJ_H

#include "ucOS_ii.h"


#define SYS_FOREVER     0       /* Указание операционной системе ждать события всегда */

extern OS_EVENT *UARTSem;
extern OS_EVENT *AT45Sem;
extern OS_EVENT *FRAMSem;
extern OS_EVENT *SPISem;
extern OS_EVENT *SemMenu;

extern OS_EVENT *MBoxKbdReq;
extern OS_EVENT *MBoxKbdResp;

extern OS_EVENT *QueueDrvIn;
extern OS_EVENT *QueueDrvOut;
extern OS_EVENT *QueueDrvInStartStop;
extern OS_EVENT *QueueDrvOutStartStop;

#endif  // OS_OBJ_H
