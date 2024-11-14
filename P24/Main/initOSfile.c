
#include "globalIncludes.h"

void createTimTask(void);
//======================================================================
// Выделение стека для каждой программной задачи
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
//          Заполнение стека задачи определенным значением
//======================================================================
void staskClear(OS_STK* pnt, u32 sz)
{
    for( ; sz; sz--)
        { *pnt++ = 0xdeaddead; }
}
//======================================================================
//          Инициализация объектов ОС и запуск задач в ОС
//======================================================================
void initOS(void)
{
    unsigned char 				err;

    OSInit();                               				// Инициализация объектов ядра

//    initTaskComPort();

    UARTSem     = OSSemCreate(1);           				//  - семафор принятого ответа по UART
    OSEventNameSet(UARTSem, "UARTSem", &err);

    MBoxKbdReq   = OSMboxCreate(NULL);       				// Содержит указатель на структуру с кодом нажатой клавиши
    OSEventNameSet(MBoxKbdReq, "MBoxKbdReq", &err);

    MBoxKbdResp   = OSMboxCreate(NULL);       				// Содержит указатель на структуру с кодом нажатой клавиши
    OSEventNameSet(MBoxKbdResp, "MBoxKbdResp", &err);

    AT45Sem     = OSSemCreate(0);           				//Семафор для работы с АТ45
    OSEventNameSet(AT45Sem, "AT45Sem", &err);

    SPISem      = OSSemCreate(1);           				//Семафор для защиты SPI
    OSEventNameSet(SPISem, "SPISem", &err);

    SemMenu      = OSSemCreate(0);           				// Семафор для запуска задачи дисплея 1 раз в 100 мс
    OSEventNameSet(SemMenu, "SemMenu", &err);

#ifdef TEST_P24
    UARTSemRx = OSSemCreate(0);
    UARTSemTx = OSSemCreate(0);
#endif

    QueueDrvIn = OSQCreate(pntMsgReqDrvIn, SzQueue);        // Создание очереди сообщений в драйвер
    OSEventNameSet(QueueDrvIn, "QueueDrvIn", &err);

    QueueDrvOut = OSQCreate(pntMsgReqDrvOut, SzQueue);      // Создание очереди сообщений из драйвера
    OSEventNameSet(QueueDrvOut, "QueueDrvOut", &err);
    
    QueueDrvOutStartStop = OSQCreate(pntMsgReqDrvOutStartStop, SzQueue);      // Создание очереди сообщений из драйвера
    OSEventNameSet(QueueDrvOutStartStop, "QueueDrvOutStartStop", &err);
    
    QueueDrvInStartStop = OSQCreate(pntMsgReqDrvInStartStop, SzQueue);      // Создание очереди сообщений в задачу клавиатуры
    OSEventNameSet(QueueDrvInStartStop, "QueueDrvInStartStop", &err);

    staskClear( LCDStack, STKLCDSIZE );                     // Заполнение стека
    staskClear( KBDStack, STKKBDSIZE );
    staskClear( DRVStack, STKDRVSIZE );
    staskClear( ControlTaskStack, STKDRVSIZE );

#ifndef _PROJECT_IAR_
    createTimTask();
#endif
    OSTaskCreate(TaskKBD, (void *)0, &KBDStack[STKKBDSIZE - 1], 6);  // Создание задачи KBD

    #if OS_TASK_NAME_EN > 0
        OSTaskNameSet(5, "KBD Task", &err);
    #endif

    OSTaskCreate(TaskLCD, (void *)0, &LCDStack[STKLCDSIZE - 1], 7);  // Создание задачи LCD
    #if OS_TASK_NAME_EN > 0
        OSTaskNameSet(6, "LCD Task", &err);
    #endif

        
    OSStart();                           // Запуск uC/OS-II
}
//======================================================================
//              Задачка для запуска задачи меню 
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
    OSTaskCreate( TimerTask, (void *)0, &TimerTaskStack[128 - 1], 4 );  // Создание задачи таймера
}
//======================================================================
//              Задачк для симуляции СОМ порта под VS
//======================================================================
#ifndef _PROJECT_IAR_

__no_init OS_STK    TaskComPortStack [128];

void TaskComPort(void *pdata);

void initTaskComPort(void)
{
    OSTaskCreate( TaskComPort, (void *)0, &TaskComPortStack[128 - 1], 3 );  // Создание задачи таймера
}

#endif
//======================================================================

