
#include "globalIncludes.h"

keyState_type       KeyState;
//LEDState_type		lampState;

//===================================================================
//              Заглушки для VS
//===================================================================
void  OS_CPU_SysTickInit(INT32U  cnts)  {}
void EndMeasEvent(u8 numEvent)          {}
void BeginMeasEvent(u8 numEvent)        {}
void LCD_init(void)                     {}
void LCD_init_hw(void)                  {}
void InitKeyboardDriver(void)           {}

extern u8 VideoRAM_Copy[YPIXELS*XPIXELS/8];
void GrfOut(void)
{
        // Поставить локер
    memcpy(VideoRAM_Copy, VideoRAM, sizeof(VideoRAM_Copy));
        // Снять локер
    return;
}

//===================================================================
//      Симулятор опроса клавиатуры для VS
//===================================================================
keyState_type readKeyPC(void)
{
    return  KeyState;
}
//===================================================================
//                      Запуск ОС под VS
//===================================================================
void initOS(void);
int openFlash(void);
//===================================================================
void __stdcall  OS_init(void)
{
    openFlash();        // Проинициализировать флэш память
    initOS();           // Запустить операционку и все в ней
}
//===================================================================

