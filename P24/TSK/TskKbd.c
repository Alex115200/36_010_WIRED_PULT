//======================================================================
//              Файл содержит задачу с опросом клавиатуры
//======================================================================

#include "globalIncludes.h"

//======================================================================
autoRepeatStateKey_type 	ARSKeyUp;
autoRepeatStateKey_type 	ARSKeyDown;

extern keyState_type       KeyState;


//===================================================================
// ЗАДАЧА ОБРАБОТКИ КЛАВИАТУРЫ
// Задача вызывается каждую 1 мсек (по семафору из таймера)
// Функции:
// Опрос кнопок клавиатуры (с ликвидацией дребезга)
//=====================================================================
__noreturn void TaskKBD(void *pData)
{
    OS_MBOX_DATA                    pMbox;
    MsgKbd_type                     *MsgKbd;
    keyState_type                   kbdState;
    keyName_type                    firstKey;
    currStateKey_type               currStateKey;
    static currStateKey_type        prevStateKey;
    static u8                       stopPrev, startPrev;
    MsgReqKbdIn_type                *pMsgReqKbdIn;
    static KbdStartStop_type        KbdStartStop = KbdStartStop_OFF;
    u8                              Err;
    
    u32                             Time_1s = OSTimeGet();

    static u8  deb = 0;
    OS_CPU_SysTickInit(120000);         // WARNING проверить число на 1 мс

    //LCD_BACK_LIGHT_SET();               // Включить подсветку дисплея
    
    initKey();                          // Проинициализировать клавиатуру

    while(1){
        //OSTimeDly(KBD_delay_TYME);
//        OSTimeDly( 1 );
        ResetWD(tsk_kbd);       
        EndMeasEvent(0);
        BeginMeasEvent(0);
        do
        {
            pMsgReqKbdIn = OSQPend(QueueDrvInStartStop, KBD_delay_TYME, &Err);
            if (pMsgReqKbdIn)
            {
                KbdStartStop = pMsgReqKbdIn->KbdStateStartStop;
            }
        }while (pMsgReqKbdIn);
		kbdState = readKey();			// Получить состояние кнопок
		kbdState = filtrKey( &kbdState, &StateKbd[0]); // Отфильтровать состояние кнопок

        if(kbdState.bit.DOWN != 0 || kbdState.bit.UP != 0 ){
            deb++;
        }
		
        currStateKey.keyState.all = kbdState.all;

        currStateKey.autoMask = autoRepeatKey(kbdState.bit.DOWN, &ARSKeyDown );
        if(currStateKey.autoMask == 0){
            currStateKey.autoMask = autoRepeatKey(kbdState.bit.UP, &ARSKeyUp );
        }

        // Обработка клавиш СТАРТ СТОП
        if ( kbdState.bit.START == 1 )
        {
            if( stopPrev == 0 )
            {
                stopPrev = 1;
                if (KbdStartStop == KbdStartStop_ON)
                {
                    if (StartStopDrive( 1, 0, COIL_OFF ) != Ok_err)    // WARNING Дописать сетевой адрес
                    {
                        MessageSetAlarm( MessageFlashFlag_NONE, 3); // сообщение что команда стоп не прошла
                    }
                }
                if( GetTickLeft( Time_1s ) < 1000 )
                {
                     StartStopDrive( 1, 1, COIL_OFF );    // WARNING Дописать сетевой адрес -- Аварийный стоп
                }
                Time_1s = OSTimeGet();
            }
        }
        else if (kbdState.bit.STOP == 1 )
        {
            if( startPrev == 0 )
            {
                startPrev = 1;
                stopPrev = 0;  
                if (KbdStartStop == KbdStartStop_ON)
                {
                     StartStopDrive( 1, 0, COIL_ON );    // WARNING Дописать сетевой адрес
                }
            }
        }
        else
        {
            startPrev = 0;
            stopPrev  = 0;
        }

        OSMboxQuery( MBoxKbdReq, &pMbox );
        if(pMbox.OSMsg != NULL){

            MsgKbd = pMbox.OSMsg;                              // Получить указатель на сообщение

            firstKey = findFirstKey(&currStateKey.keyState);

            if( !((firstKey == KEY_UP) || (firstKey == KEY_DOWN)) ){
                currStateKey.autoMask   = 0;
                MsgKbd->keyPressed      = 0;
            }
            else{
                MsgKbd->keyPressed = 1;
            }

            if( currStateKey.all !=  prevStateKey.all ){
                MsgKbd->Code = firstKey;
            }
            else{
                MsgKbd->Code = NON_KEY;
            }

            currStateKey.autoMask = 0;
            prevStateKey.all = currStateKey.all;

            ARSKeyUp.maskVgen   = 0;
            ARSKeyDown.maskVgen = 0;

            OSMboxAccept(MBoxKbdReq);              // Прочистить мейлбокс
            OSMboxPost( MBoxKbdResp, MsgKbd );     // отправляем сообщение с кодом нажатой клавиши
        }
    }
}
//==================== End Of File  =================================
