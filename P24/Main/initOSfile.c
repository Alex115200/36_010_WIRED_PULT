
#include "globalIncludes.h"

void createTimTask(void);
//======================================================================
// ��������� ����� ��� ������ ����������� ������
//======================================================================
#ifdef _PROJECT_IAR_
    #pragma data_alignment=8
#endif
__no_init OS_STK KBDStack[STKKBDSIZE];
#ifdef _PROJECT_IAR_
    #pragma data_alignment=8
#endif
__no_init OS_STK DRVStack[STKDRVSIZE];
#ifdef _PROJECT_IAR_
    #pragma data_alignment=8
#endif
__no_init OS_STK LCDStack [STKLCDSIZE];



__no_init OS_STK ControlTaskStack [STK_CONTROL_SIZE];


#ifdef _PROJECT_IAR_
    #define initTaskComPort()
#else
    void initTaskComPort(void);
#endif

//======================================================================
//          ���������� ����� ������ ������������ ���������
//======================================================================
void staskClear(OS_STK* pnt, u32 sz)
{
    for( ; sz; sz--)
        { *pnt++ = 0xdeaddead; }
}
//======================================================================
//          ������������� �������� �� � ������ ����� � ��
//======================================================================
void initOS(void)
{
    unsigned char 				err;

    OSInit();                               				// ������������� �������� ����

//    initTaskComPort();

    UARTSem     = OSSemCreate(1);           				//  - ������� ��������� ������ �� UART
    OSEventNameSet(UARTSem, "UARTSem", &err);

    MBoxKbdReq   = OSMboxCreate(NULL);       				// �������� ��������� �� ��������� � ����� ������� �������
    OSEventNameSet(MBoxKbdReq, "MBoxKbdReq", &err);

    MBoxKbdResp   = OSMboxCreate(NULL);       				// �������� ��������� �� ��������� � ����� ������� �������
    OSEventNameSet(MBoxKbdResp, "MBoxKbdResp", &err);

    AT45Sem     = OSSemCreate(0);           				//������� ��� ������ � ��45
    OSEventNameSet(AT45Sem, "AT45Sem", &err);

    SPISem      = OSSemCreate(1);           				//������� ��� ������ SPI
    OSEventNameSet(SPISem, "SPISem", &err);

    SemMenu      = OSSemCreate(0);           				// ������� ��� ������� ������ ������� 1 ��� � 100 ��
    OSEventNameSet(SemMenu, "SemMenu", &err);

#ifdef TEST_P24
    UARTSemRx = OSSemCreate(0);
    UARTSemTx = OSSemCreate(0);
#endif

    QueueDrvIn = OSQCreate(pntMsgReqDrvIn, SzQueue);        // �������� ������� ��������� � �������
    OSEventNameSet(QueueDrvIn, "QueueDrvIn", &err);

    QueueDrvOut = OSQCreate(pntMsgReqDrvOut, SzQueue);      // �������� ������� ��������� �� ��������
    OSEventNameSet(QueueDrvOut, "QueueDrvOut", &err);
    
    QueueDrvOutStartStop = OSQCreate(pntMsgReqDrvOutStartStop, SzQueue);      // �������� ������� ��������� �� ��������
    OSEventNameSet(QueueDrvOutStartStop, "QueueDrvOutStartStop", &err);
    
    QueueDrvInStartStop = OSQCreate(pntMsgReqDrvInStartStop, SzQueue);      // �������� ������� ��������� � ������ ����������
    OSEventNameSet(QueueDrvInStartStop, "QueueDrvInStartStop", &err);

    staskClear( LCDStack, STKLCDSIZE );                     // ���������� �����
    staskClear( KBDStack, STKKBDSIZE );
    staskClear( DRVStack, STKDRVSIZE );
    staskClear( ControlTaskStack, STKDRVSIZE );

#ifndef _PROJECT_IAR_
    createTimTask();
#endif
    OSTaskCreate(TaskKBD, (void *)0, &KBDStack[STKKBDSIZE - 1], 6);  // �������� ������ KBD

    #if OS_TASK_NAME_EN > 0
        OSTaskNameSet(5, "KBD Task", &err);
    #endif

    OSTaskCreate(TaskLCD, (void *)0, &LCDStack[STKLCDSIZE - 1], 7);  // �������� ������ LCD
    #if OS_TASK_NAME_EN > 0
        OSTaskNameSet(6, "LCD Task", &err);
    #endif

        
    OSStart();                           // ������ uC/OS-II
}
//======================================================================
//              ������� ��� ������� ������ ���� 
//======================================================================
void TimerTask(void *pdata)
{
    u8      err;
    while(1){ 
#ifdef _PROJECT_IAR_
        OSTimeDly(100);
#else
        OSTimeDly(100);
#endif
        OSSemSet ( SemMenu, 0, &err);
        OSSemPost( SemMenu );
    }
}
//======================================================================
__no_init OS_STK    TimerTaskStack [128];

void createTimTask(void)
{
    OSTaskCreate( TimerTask, (void *)0, &TimerTaskStack[128 - 1], 4 );  // �������� ������ �������
}
//======================================================================
//              ������ ��� ��������� ��� ����� ��� VS
//======================================================================
#ifndef _PROJECT_IAR_

__no_init OS_STK    TaskComPortStack [128];

void TaskComPort(void *pdata);

void initTaskComPort(void)
{
    OSTaskCreate( TaskComPort, (void *)0, &TaskComPortStack[128 - 1], 3 );  // �������� ������ �������
}

#endif
//======================================================================

