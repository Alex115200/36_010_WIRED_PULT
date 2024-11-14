/************************** (C) Copyright 2013 Triol ***************************
* File Name          : VersionPo.c
* Author             : Kats Eyvhen
* TS Idea            : 2013.04.19
* TS Version         : 2013.04.19
* Description        : Function show CRC and version PO
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "globalIncludes.h"
#include <stdlib.h>


/* Private define ------------------------------------------------------------*/

// Указываем актуальные размеры памяти, для расчета CRC программы
#define START_PULT_SOFT   ADDR_FLASH_SECTOR_4   /* Start @ of user Flash area */
#define END_PULT_SOFT     ADDR_FLASH_SECTOR_8   /* End @ of user Flash area */

//#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
//#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
//#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
//#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
//#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */


/* Private macro -------------------------------------------------------------*/



/* Private types -------------------------------------------------------------*/



/* Private variables ---------------------------------------------------------*/



/* Private function prototypes -----------------------------------------------*/



/* Private functions ---------------------------------------------------------*/
// **************************************************************************
//  Подсчитывает и выводит на экран CRC прошивки, 
// а также версию ПО
// **************************************************************************
void Version(void)
{
    keyName_type        Key;
    u8                  err;
    u8                  first = 1;
    
    while(1)
    {
        OSSemPend(SemMenu, SYS_FOREVER, &err );
        Key = WaitKey(NULL);

        switch (Key) 
        {
         case NON_KEY:
             break;
         case KEY_ENTER:
            if (first)
            {
                //==========================================================
                // Очищаем буфер экрана
                //==========================================================
                GrafBffClear();
                //====================================================================
                // Выводим информацию о версии ПО и CRC 
                //====================================================================
                sprintf(&StrOut[0], "Software ver:");
                StrOutFont(0, 0, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //Вывод внизу номера группы и параметра
                sprintf(&StrOut[0], "%.3f  %d", ((f32)(GRP_VERSION_PO_PU24E) + (f32)(SUB_VERSION_PO_PU24E)/1000), MRK_VERSION_PO_PU24E );
                StrOutFont(0, INCY, NO_INV, &StrOut[0], &fontNormal, leftAlign );
                sprintf(&StrOut[0], "Software CRC:");
                StrOutFont(0, INCY*3, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //Вывод внизу номера группы и параметра
                // sprintf(&StrOut[0], "%d", GetCrc((void *)((u32)(64*1024)), 512*1024));  // Старый вариант задания диапазона памяти для счета CRC
                sprintf(&StrOut[0], "%d", GetCrc((void *)((u32)START_PULT_SOFT), (END_PULT_SOFT-START_PULT_SOFT)));                
                StrOutFont(0, INCY*4, NO_INV, &StrOut[0], &fontNormal, leftAlign );
                //==========================================================
                // Вывод на экран
                //==========================================================
                GrfOut();
                first = 0;
            }
            else
            {
                return;
            }
            break;
        case KEY_ESC:
            return;
            break;
        default:
            return;
            break;
        }
        if (first)
        {
            return;
        }
    }
}


/************************** (C) Copyright 2013 Triol **************************/