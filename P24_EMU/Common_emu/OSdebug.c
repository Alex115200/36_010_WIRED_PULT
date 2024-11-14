
#include "globalIncludes.h"

keyState_type       KeyState;
//LEDState_type		lampState;

//===================================================================
//              �������� ��� VS
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
        // ��������� �����
    memcpy(VideoRAM_Copy, VideoRAM, sizeof(VideoRAM_Copy));
        // ����� �����
    return;
}

//===================================================================
//      ��������� ������ ���������� ��� VS
//===================================================================
keyState_type readKeyPC(void)
{
    return  KeyState;
}
//===================================================================
//                      ������ �� ��� VS
//===================================================================
void initOS(void);
int openFlash(void);
//===================================================================
void __stdcall  OS_init(void)
{
    openFlash();        // ������������������� ���� ������
    initOS();           // ��������� ����������� � ��� � ���
}
//===================================================================

