#include "Kbd.h"
#include "Uart.h"

#include "ucos_ii.h"
#include "type.h"

extern OS_EVENT *MBoxKbd;
extern OS_EVENT *KBDSem;

//======================================================================
// ЗАДАЧА ОБРАБОТКИ КЛАВИАТУРЫ
//  Задача вызывается каждую 1 мсек (по семафору из таймера)
//  Функции:
//    Опрос кнопок клавиатуры (с ликвидацией дребезга)
//    Регулирование контрастности дисплея 
//=====================================================================
u16 modeKey = 0;
u16 GetModeKey(void)
{
    return modeKey;
}

void SetModeKey(u16 mode)
{
    modeKey = mode;
}

#define TIME_FAST1      300
#define TIME_FAST2      100
#define CNT_SHOT        5
#define DELAY_ONE_SHOT  1000
__noreturn void TaskKBD(void *pdata)
{
    u8 i;
//    u8 BufStartStop[16];
    u8 MsgEn;
    u16 FlagOneShot = 0;
    u16 FlagFast1 = 0;
    u16 TimeFast1 = 0;
    u16 FlagFast2 = 0;
    u16 TimeFast2 = 0;
    u16 CntShot = 0;
    u16 Spec1    = 0;
    u16 Spec2    = 0;
    u16 Spec3    = 0;
    //u16 TimeDlySpec2 = 0;
    u16 DelayOneShot = 0;
    OS_MBOX_DATA    MBData;
//    MsgReqDrvOut_type *pntMsgDrvOut;
//    u8 Err;

    //    unsigned char err;

    OS_CPU_SysTickInit();   ///////////////////////////////////
    #if OS_STAT_TASK_EN > 0
    OSStatInit();
    #endif
    
    while(1){
        OSTimeDly(1);
        
        //Опрос кнопок Отмена, Функция и Enter
//        OSSemPend(KBDSem, 0, &err);
        MsgEn = 0;
        

        for (i = 0; i < KOLVO_KEY && !Spec1 && !Spec2 && !Spec3; i ++){
            ClearQWER();
            switch (i)
            {
                case STOPq:                      if(FuncKey(KEY_STOP_P, STOPq))     Spec2 = 1; break;
                case STARTq:                    if(FuncKey(KEY_START_P, STARTq))  Spec2 = 1; break;
                case LEFT:      POLL_LEFT();    if(FuncKey(KEY_LEFT_P, LEFT))     Spec2 = 1; break;
                case RIGHT:     POLL_RIGHT();   if(FuncKey(KEY_RIGHT_P, RIGHT))   Spec2 = 1; break;
                case UP:        POLL_UP();      if(FuncKey(KEY_UP_P, UP))         Spec1  = 1; break;
                case DOWN:      POLL_DOWN();    if(FuncKey(KEY_DOWN_P, DOWN))     Spec1  = 1; break;
                case ENTER:     POLL_ENTER();   if(FuncKey(KEY_ENTER_P, ENTER))   Spec2 = 1; break;
                case FUNC:      POLL_FUNC();    if(FuncKey(KEY_FUNC_P, FUNC))     Spec2 = 1; break;
                case CANCEL:    POLL_CANCEL();  if(FuncKey(KEY_CANCEL_P, CANCEL)) Spec3 = 1; break;
                default:                                                                   break;
            }
        }
        
        if (GetModeKey() && (Spec1 || Spec2 || Spec3)){
            MsgEn = 1;
        }
        else{
            if (Spec1 == 1) {//Кнопки вверх вниз
                if(FlagOneShot == 0){
                    FlagOneShot = 1;
                    MsgEn = 1;
                }
                else{
                    if (FlagFast1 == 0){
                        TimeFast1 ++;
                        if (TimeFast1 > TIME_FAST1){
                            TimeFast1 = 0;
                            MsgEn = 1;
                            CntShot ++;
                            if (CntShot > CNT_SHOT){
                                FlagFast1 = 1;
                            }
                        }
                    }
                    else if (FlagFast2 == 0) {
                        TimeFast2 ++;
                        if (TimeFast2 > TIME_FAST2){
                            TimeFast2 = 0;
                            MsgEn = 1;                    
                        }
                    }
                }
            }
            else if (Spec2 == 1) {
                if(FlagOneShot == 0){
                    FlagOneShot = 1;
                    MsgEn = 1;
                }
                else{
                    DelayOneShot++;
                    if (DelayOneShot > DELAY_ONE_SHOT){
                        DelayOneShot = 0;
                        FlagOneShot = 0;
                    }
                }
            }
            else if (Spec3 == 1) {
                if(FlagOneShot == 0){
                    FlagOneShot = 1;
                    MsgEn = 1;
                }
            }
            else{                           //Сброс всех флагов и счетчиков
                FlagOneShot = 0;
                CntShot = 0;
                TimeFast1 = 0;
                TimeFast2 = 0;
                FlagFast1 = 0;
                FlagFast2 = 0;
            }
        }
        Spec1 = 0;
        Spec2 = 0;
        Spec3 = 0;


        if (MsgKbd.Code == 0)
        {
            Spec3++;
            Spec3--;
        }
        OSMboxQuery (MBoxKbd, &MBData);

        if (MsgEn){

            MsgEn = 0;
            OSMboxPost(MBoxKbd, &MsgKbd);
            do {
                OSTimeDly(1);
                OSMboxQuery (MBoxKbd, &MBData);
            }while (MBData.OSMsg != NULL);
        }
    }//while (1)   

}