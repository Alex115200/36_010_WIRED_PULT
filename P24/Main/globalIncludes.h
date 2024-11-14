

/*
#include "globalIncludes.h"
*/

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#include "os_cfg.h"
#include "ucos_ii.h"
#include "OS_Obj.h"

//#ifdef _PROJECT_IAR_
    #include <os_cpu.h>
//#endif

#include "global_types.h"
#include "types.h"
#include "globdefs.h"
#include "BoardConfig.h"
#include "DrvOpisStruct.h"

#include "Pult_RAM.h"

#include "Calc_New.h"
#include "DrvOpisStruct.h"
#include "Kbd.h"
#include "menu.h"
#include "makeopis.h"
#include "nonvolatile.h"
#include "main.h"
#include "initOSfile.h"
#include "TskDrv.h"
#include "crc.h"
#ifdef _PROJECT_IAR_
    #include "uc1698.h"
#endif
#include "anyfunc.h"
#include "manchester.h"
#include "uart.h"
#include "modbus.h"
#include "icon.h"
#include "microprog.h"

#include "Font.h"
#include "monitor.h"
#include "TskLcd.h"
//#include "lamp.h"
#include "LevelMenu.h"
#include "SetupMenu.h"
#include "VersionPo.h"
#include "MessageFlash.h"
#include "TimeMy.h"

#include "LED.h"

#include "TskControl.h"

#ifdef _PROJECT_IAR_
    #include <intrinsics.h>

    #include "stm32f2xx.h"
    #include "stm32f2xx_spi.h"
    #include "stm32f2xx_exti.h"
    #include "Spi.h"
    #include "dma.h"
    #include "misc.h"
    #include <yfuns.h>// для чтения из флеш

    #include "DbgTim.h"

#else

    #include "OSdebug.h"
    #include "emustubs.h"

    #include "Comms.h"

#endif 

