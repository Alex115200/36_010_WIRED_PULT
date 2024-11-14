
//======================================================================
#include "globalIncludes.h"
//#include "AT_24_prog_Pult_Mem_One.h"
#include "AT_24_prog_Pult_Mem.h"
//======================================================================
extern AllPrm_Type *AllPrmPnt;
extern uint8_t gIsNvsaOld;

//const char *const msgError = GetStringMess(MessageMess_ERROR_CONECT);

MsgReqDrvIn_type    MsgReqDrvIn[8]; // Массив сообщений из задачи дисплея в задачу обмена
//MsgReqDrvOut_type   MsgReqDrvOut[8];
MsgReqKbdIn_type    MsgReqKbdIn[8];

void                *pntMsgReqDrvIn[SzQueue];
void                *pntMsgReqDrvOut[SzQueue];
void                *pntMsgReqDrvOutStartStop[SzQueue];
void                *pntMsgReqDrvInStartStop[SzQueue];

u8 WaitMsgFromDrv(MsgReqDrvIn_type *pMsg);
Drv_err_type ReQuestDrv(OS_EVENT *QueueMsg, MsgReqDrvIn_type *MsgIn);
void ClearQueueDrv(void);
u32  MissingFileWord;
void sendRequest(portUart_type *uart);

//======================================================================
// ЗАДАЧА ПРИЕМА И ПЕРЕДАЧИ ЗАПРОСОВ ДЛЯ ПРИВОДА
//  Задача вызывается по семафору из TaskLCD
//  Функции:
//    Формирование команд "Пуск" и "Стоп"
//    Обмен с приводом
//======================================================================
#if 0
typedef struct{
    USART_TypeDef      				*reg;
    uint32_t           				dma_ch_rx;
    uint32_t           				dma_ch_tx;
    DMA_Stream_TypeDef 				*dma_rx;
    DMA_Stream_TypeDef 				*dma_tx;
    u8                 				*bufRx;
    u8                 				*bufTx;
    u16                				size_buf;
    u16                				tx_size;
    u16                				rx_size;
    stateUart_type     				state;
    stateUartTransaction_type		stateTA;
    u32                             minc;
}portUart_type;
#endif
//======================================================================
#define szTempArUart    64
union{
    u16 w[szTempArUart];      // Массив для формирования запроса до перекодировки в манчестер
    u8  b[szTempArUart*2];
}tmpArUart;
//======================================================================
inline void ManchesterEncode(void *OutPtr, void *InPtr, u16 size );

#define MAX_REPEAT_UART  5-1      /* Максимальное количество автоповторов */

//======================================================================
//  Программная задача обмена с приводом через индукционную запитку
//              Актуальна для местного пульта
//======================================================================
#ifdef _PROJECT_IAR_
__noreturn void TaskDRV(void *pdata)
{
    u8                              err;
    u8                              cntRepeat;  // Счетчик автоповторов
    u16                             size;

    Drv_err_type                    errTA;
    portUart_type                   *uart;
    MsgReqDrvIn_type                *pntMsgIn; //, MsgIn;

#ifndef _PROJECT_IAR_
    rt_hw_usart_init();             // инициализация драйвера Uart
#endif
    uart = &uart1;                  // Указатель на uart1

    OSSemPend (UARTSem, 200, &err);

    cntRepeat       = MAX_REPEAT_UART;
    uart->state     = NON_TA;

    while(1){
      ResetWD(tsk_drv);
      OSSemPend( UARTSem, 1000, &err );     // Вешаем на семафоре
     
      if(uart->state == NON_TA){                                  // Первый запрос
          pntMsgIn = OSQPend(QueueDrvIn, SYS_FOREVER, &err);      // Вынимаем запрос с очереди
          
          if(pntMsgIn->CleanQueue == 1){                          // Требование прочистить очередь сообщений
              SendMsgToTask(pntMsgIn, NULL, 0);                   // Проквитировать запрос
              pntMsgIn = OSQPend(QueueDrvIn, SYS_FOREVER, &err);  // Опять ждем сообщения
          }

          // Формируем запрос к драйверу порта
          size = ModbusFunc( pntMsgIn, tmpArUart.w );             // Сформировали запрос
          ManchesterEncode( uart->bufTx, tmpArUart.w, size );     // Закодировали в Манчестер
          uart->tx_size = size;
          uart->state = TX_READY;                                 // Данные готовы
          cntRepeat = 0;                                          // Инициализация счетчика повторов передачи запроса
          sendRequest(uart);                                      //!Запустить передачу запроса в привод
        
      }
      else if((uart->state == RX_READY)&&(err == OS_ERR_NONE)){     // Пришёл ответ от ПЧ
          errTA = Ok_err;
          
          size = uart->rx_size;
          ManchesterDecode( DataBuf, UartBufRx, size );             // содержимое UartBufRx копируется в DataBuf 
          
          errTA = SendMsgToTask(pntMsgIn, DataBuf, size );          // Анализ принятых данных
          if((errTA == Ok_err)||(errTA == ErrReqMB)){               // Получили корректный ответ от ПЧ                                  
              pntMsgIn = OSQPend(QueueDrvIn, SYS_FOREVER, &err);    // Вынимаем следующий запрос из очереди запросов к порту
              
              if(pntMsgIn->CleanQueue == 1 ){                       // Требование прочистить очередь сообщений
                SendMsgToTask(pntMsgIn, NULL, 0);                   // Проквитировать запрос
                pntMsgIn = OSQPend(QueueDrvIn, SYS_FOREVER, &err);  // Опять ждем сообщения
              }

              // Формируем запрос к драйверу порта
              size = ModbusFunc( pntMsgIn, tmpArUart.w );             // Сформировали запрос
              ManchesterEncode( uart->bufTx, tmpArUart.w, size );     // Закодировали в Манчестер
              uart->tx_size = size;
              uart->state = TX_READY;                                 // Данные готовы
              cntRepeat = 0;                                          // Инициализация счетчика повторов передачи запроса
              sendRequest(uart);                                      // Запустить передачу запроса в привод
          }
          else{                                                       // Ответ от ПЧ не корректный!!!
              uart->state = RX_TIME_ERR;                             // Запрос на повторный запрос
              OSSemPost(UARTSem);                                     // Отпустить симофор задачи
          }
     }
     else if((uart->state == RX_TIME_ERR)||(err != OS_ERR_NONE)){    // Ответ не получен за положенное время
         if(cntRepeat < MAX_REPEAT_UART){
             cntRepeat++;                                            // Счетчик автоповторов запросов к ПЧ
             
             size = ModbusFunc( pntMsgIn, tmpArUart.w );             // Сформировали повторно запрос
             ManchesterEncode( uart->bufTx, tmpArUart.w, size );     // Копируем запрос в буффер передатчика
             uart->tx_size = size;
             uart->state = TX_READY;                                 // Данные готовы
             sendRequest(uart);                                      // Запустить передачу запроса в привод
         }
         else{                                                       // Не дождались MAX_REPEAT_UART посылок
              cntRepeat = 0;                                         //!Сброс счетчика проходов задачи, в течении которых ждем ответ
              uart->state = NON_TA;                                  //!Состояние первого прохода. Будем отправлять новый запрос
              
              // Сообщение в задачу об отказе по связи с приводом
              pntMsgIn->Err = NonRdk_err;                            // Ошибка нет ответа
              if(pntMsgIn->Func != WRITE_COIL ){
                 OSQPost(QueueDrvOut, pntMsgIn);
              }
              else{
                 OSQPost(QueueDrvOutStartStop, pntMsgIn);
              }
         }
     }
     else{
         // ????????????????
     }
     
     
 /*    
     
     
     
      // Вешаем на семафоре
     if( ((err == OS_ERR_NONE)||(uart->state == NON_TA))&&(uart->state != RX_TIME_ERR)  ){      // Получен ответ от порта или первый проход
          errTA = Ok_err;
          if( uart->state == RX_READY )
          {                          // Вычитываем ответ
              //size = uart->rx_size / 2; //!делим на 2, так как на два байта принятых данных приходится 1 значащий байт 
              size = uart->rx_size;
              ManchesterDecode( DataBuf, UartBufRx, size );       // содержимое UartBufRx копируется в DataBuf 
              errTA = SendMsgToTask(pntMsgIn, DataBuf, size );    // Отправляем сообщение обратно
          }

          if( Ok_err == errTA )
          {
              pntMsgIn = OSQPend(QueueDrvIn, SYS_FOREVER, &err);  // Вынимаем следующий запрос
          }
          else
          {
              // Отправляем старый запрос заново
              debugStop();
          }
          if(pntMsgIn->CleanQueue == 1 ){                         // Требование прочистить очередь сообщений
              SendMsgToTask(pntMsgIn, NULL, 0);                   // Проквитировать запрос
              pntMsgIn = OSQPend(QueueDrvIn, SYS_FOREVER, &err);  // Опять ждем сообщения
          }

          // Формируем запрос к драйверу порта
          size = ModbusFunc( pntMsgIn, tmpArUart.w );             // Сформировали запрос
          ManchesterEncode( uart->bufTx, tmpArUart.w, size );     // Закодировали в Манчестер
          uart->tx_size = size;
          uart->state = TX_READY;                                 // Данные готовы
          cntRepeat = 0;                                          // Инициализация счетчика повторов передачи запроса
          sendRequest(uart);                                      //!Запустить передачу запроса в привод
     }
     else if( (uart->state == RX_TIME_ERR) || err != OS_ERR_NONE ){          // Ответ не получен за положенное время
     if( cntRepeat < MAX_REPEAT_UART ){
             cntRepeat++; //!счетчик проходов задачи
             uart->state = TX_READY;                              // повтор запроса (поддержание состояния "Ждем ответ")
         }
         else{ //!Ответной посылки не дождались
              // Сообщение в задачу об отказе по связи с приводом
              cntRepeat = 0; //!Сброс счетчика проходов задачи, в течении которых ждем ответ
              uart->state = NON_TA; //!Состояние первого прохода. Будем отправлять запрос снова
         }
     }
     else{
         // ????????????????
     }*/
  }
}
#else
//===================================================================
// Задача для симулятора пока заглушка

bool hw_usart_init(void);
//===================================================================
void TaskDRV(void *pdata)
{
    u8                              err;
    u16                             size;
    Drv_err_type                    errTA;
    portUart_type                   *uart;
    MsgReqDrvIn_type                *pntMsgIn; //, MsgIn;
    errUART_type                    errRx;
    bool                            errInitUart; 

    uart            = &uart1;                  // Указатель на uart1
    uart->state     = NON_TA;

    while(1){
        do{
            OSTimeDly(100);
            errInitUart = hw_usart_init();                          // инициализация драйвера Uart

            err = 1;
            while(1){
                pntMsgIn = OSQAccept( QueueDrvIn, &err );
                if( pntMsgIn == NULL ){
                    break;
                }
                else{
                    SendMsgToTask(pntMsgIn, NULL, 0 );              // Отправляем сообщение обратно
                }
            };
        }while(errInitUart == false);


        while(1){
            pntMsgIn = OSQPend(QueueDrvIn, SYS_FOREVER, &err);      // Опять ждем сообщения
            
            if(pntMsgIn != NULL){
                if(pntMsgIn->CleanQueue == 1 ){                         // Требование прочистить очередь сообщений
                    SendMsgToTask(pntMsgIn, NULL, 0);                   // Проквитировать запрос
                    continue;                                           // Опять ждем сообщения
                }

                if(comPortSetup.comIsInit == 0 ){                       // Проверка если порт уже закрыт 
                    ClosePort(&comPortSetup);
                    pntMsgIn->Err = ErrDebug;
                    size = 0;
                    errTA = SendMsgToTask(pntMsgIn, DataBuf, size );    // Отправляем сообщение обратно
                    break; 
                }  

                pntMsgIn->countByteWrite = ModbusFunc( pntMsgIn, &tmpArUart.b[0] );     // Формируем запрос к драйверу порта
                pntMsgIn->cntRepeat = 0;                                                // Сбросить счетчик повторов запроса
                while(1){                                                               // SendMsgToTask считает количество повторов и правильность принятого ответа
                    errRx = txRxUart(pntMsgIn, &tmpArUart.b[0], DataBuf );              // Послать сообщение и ожидать ответа
                    //if (errRx == noErr)
                    //{
                        errTA = SendMsgToTask(pntMsgIn, DataBuf, pntMsgIn->SzOut );         // пытаемся отправить сообщение обратно
                        if( errTA == Ok_err ) break;
                    //}
                }
            }
        }
    }
}
//===================================================================
//          Инициализация СОМ порта в симуляторе
//===================================================================
bool hw_usart_init(void)
{
	return OpenPort( &comPortSetup ) ;
}
//===================================================================
#endif
//===================================================================
// формирование ответа в задачу меню от драйвера
//===================================================================
#ifndef _PROJECT_IAR_
    extern int         isFlashChanged;      // Признак сохранить в файл при закрытии
#endif
//===================================================================
Drv_err_type SendMsgToTask( MsgReqDrvIn_type *MsgIn, u8 *pBuf, u16 size )
{
    if( MsgIn->CleanQueue ){                // Прочистка очереди входных сообщений
        MsgIn->Err = Ok_err;          
    }
    else if ( size < 4 ){
        MsgIn->Err = ErrSzMB;               // Мало данных
        
    }
    else if(GetCrc( pBuf, size ) != 0 ){
        MsgIn->Err = ErrKsMB;               // Ошибка контрольной суммы
    }
    else if( pBuf[1] & 0x80){
        MsgIn->Err = ErrReqMB;              // Отказ по Модбасу от контроллера
    }
    else{
        MsgIn->Err = Ok_err;                // норма

        switch (MsgIn->Func){
            case READ_H_REGS:
                MsgIn->SzOut = size - 5;
                memcpy( MsgIn->Buf, pBuf + 3, MsgIn->Sz * 2 );
                break;

            case READ_DEV_ID:
                MsgIn->SzOut = 31;
                memcpy(MsgIn->Buf, pBuf + 7, size - 7);
                break;

            case READ_FILE:
                 MsgIn->SzOut = size - 10;
                 memcpy(MsgIn->Buf, pBuf + 8, MsgIn->Sz * 2);
#ifndef _PROJECT_IAR_
                 isFlashChanged = 1;      // Признак сохранить в файл при закрытии
#endif
                 break;
            case WRITE_FILE:
                 break;
            case WRITE_REGS:
                break;
            case WRITE_COIL:
                break;

            default:
                MsgIn->Err = ErrReqMB;
                break;
        }
     }

    if(MsgIn->Err != Ok_err ){
        if( MsgIn->cntRepeat < MAX_REPEAT_UART ){
            MsgIn->cntRepeat++;
            //return  MsgIn->Err;     // Повтор запроса
        }
        return  MsgIn->Err;           // Повтор запроса
    }
    if(MsgIn->Func != WRITE_COIL ){
        OSQPost(QueueDrvOut, MsgIn);
    }
    else{
        OSQPost(QueueDrvOutStartStop, MsgIn);
    }
    return Ok_err;                  // Отправить сообщение обратно
}
//===================================================================
//      Формирование запроса на чтение идентификатора привода
//===================================================================
Drv_err_type Request0x2B(MsgReqDrvIn_type *MsgIn, u8 AdrNet, void *Buf, u16 SzBuf)
{

    MsgIn->AdrNet   = AdrNet;
    MsgIn->AdrData  = 0;
    MsgIn->Param    = 1;            // 1-й файл
    MsgIn->Sz       = 64;
    MsgIn->Func     = READ_DEV_ID;
    MsgIn->Buf      = Buf;
    MsgIn->SzBuf    = SzBuf;

#ifndef _PROJECT_IAR_    
    MsgIn->countByteRead = 256;     // Ожидаем по времени
#endif

    return ReQuestDrv(QueueDrvIn, MsgIn);
}

//===================================================================
// Формирование запроса на чтение описателя параметров ПЧ
//===================================================================
/*
Drv_err_type RequestFDP(MsgReqDrvIn_type *MsgIn, u8 AdrNet, void *Adr, u8 SzReq, void *Buf, u16 SzBuf)
{
    MsgIn->AdrNet   = AdrNet;
    MsgIn->AdrData  = (u32)Adr;
    MsgIn->Param    = 1;        //0-й файл
    MsgIn->Sz       = SzReq;
    MsgIn->Func     = READ_FILE;
    MsgIn->Buf      = Buf;
    MsgIn->SzBuf    = SzBuf;
//    MsgIn->MsgOut   = MsgOut;
    //MsgOut->Buf     = (u8 *)Buf;

#ifndef _PROJECT_IAR_ 
    MsgIn->countByteRead = 10 + SzReq*2 ;  // Ожидаем по колич данных
#endif
    return ReQuestDrv(QueueDrvIn, MsgIn);
}*/
/*********************************************
 * Формирование запроса на чтение версии     *
 * функционала                               *
 *  Входные параметры:                       *
 *  - указатель на сообщение                 *
 *  - сетевой адрес                          *
 *  - номер файла                            *
 *  - указатель на структуру куда читать вер-*
 * сию                                       *
 *  Выходные параметры                       *
 *  - ошибка драйвера                        *
 *********************************************/
Drv_err_type ReadVerFunct(MsgReqDrvIn_type *MsgIn, u8 AdrNet, u8 NumFile, FileFunctional_type *pFunct)
{
    Drv_err_type        errCode = Ok_err; // Читаем файл функционала с проверкой ошибки возврата
    u16                 crc, tmp;
    
    errCode = ReadFileFromDrv(MsgIn, AdrNet, 0, sizeof(FileFunctional_type), NumFile, pFunct, sizeof(FileFunctional_type));
    
    // Если приходит отвергнутый ответ - то надо поставить бит, что файла нету и больше его не читать
    if (errCode == ErrReqMB) {
        MissingFileWord |= (1 << NumFile); // Блокируем файл и возвращаем 
        errCode = Ok_err; // успешный код операции

        // Подсовываем системе "липовый" функционал, чтобы она считала что все в порядке.
        pFunct->curFunct.functMask = FULL_FUNCTIONAL_MASK; // Маска при отсутствии функционала !*!*!
        tmp = swapU16(pFunct->curFunct.functMask);
        crc = GetCrc(&tmp, sizeof(u16)); // Вычисляем CRC и подсовываем в структуру
        pFunct->crc = crc;
    }
    else { // Если файл существует - сбросить бит в статусе наличия файлов
      MissingFileWord &= ~(1 << NumFile); // 
    }
  
    return (errCode); // Возврат ошибки
}
/*********************************************
 * Формирование запроса на чтение индексов   *
 * журнала                                   *
 *  Входные параметры:                       *
 *  - указатель на сообщение                 *
 *  - сетевой адрес                          *
 *  - номер файла                            *
 *  - указатель на структуру индексов журна  *
 *  ла                                       *
 *  Выходные параметры                       *
 *  - ошибка драйвера                        *
 *********************************************/
Drv_err_type ReadJrnIndex(MsgReqDrvIn_type *MsgIn, u8 AdrNet, u8 NumFile, Index_type *indexJrn)
{
    return ReadFileFromDrv(MsgIn, AdrNet, 0, sizeof(Index_type), NumFile, indexJrn, sizeof(Index_type));
}

/*********************************************
 * Формирование запроса на чтение журнала    *
 *  Входные параметры:                       *
 *  - указатель на сообщение                 *
 *  - сетевой адрес                          *
 *  - номер файла                            *
 *  - указатель на структуру журнала         *
 *  Выходные параметры                       *
 *  - ошибка драйвера                        *
 *********************************************/
/*Drv_err_type ReadJrn(u8 AdrNet, void *AdrInFile, u8 NumFile, u8 *RecJrn, u16 szReq)
{
    return ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], AdrNet, AdrInFile, sizeof(RecJrn_type), NumFile, RecJrn, sizeof(RecJrn_type));
    ReadFileFromDrv()
}*/


/*********************************************
 * Формирование запроса на чтение файла      *
 *  Входные параметры:                       *
 *  - указатель на сообщение                 *
 *  - адрес внутри файла                     *
 *  - запрашиваемый объем в байтах           *
 *  - номер файла                            *
 *  - указатель на буфер для принятых данных *
 *  - размер буфера                          *
 *  Выходные параметры                       *
 *  - ошибка драйвера                        *
 *********************************************/
/*Drv_err_type ReadFileFromDrv(MsgReqDrvIn_type *MsgIn, u8 AdrNet, void *Adr, u8 SzReq, u8 NumFile, void *Buf, u16 SzBuf)
{
#ifdef TEST_MENU
    FileFunctional_type FileFunctional; 
    u16 tmp;
#endif
    MsgIn->AdrNet   = AdrNet;
    MsgIn->AdrData  = (u32)Adr;
    MsgIn->Param    = NumFile;
    MsgIn->Sz       = SzReq << 1;
// WARNING + Остаток от деления
    if (SzReq % 2 == 0)             MsgIn->Sz =  SzReq >> 1;
    else                            MsgIn->Sz = (SzReq >> 1) + 1;
    MsgIn->Func     = READ_FILE;
    MsgIn->Buf      = Buf;
    MsgIn->SzBuf    = SzBuf;
    //MsgOut->Buf     = (u8 *)Buf;

#ifndef _PROJECT_IAR_ 
    MsgIn->countByteRead = 10 + SzReq*2 ;  // Ожидаем по колич данных
#endif
#ifdef TEST_MENU
    if ((AdrNet == 1) && (NumFile == fileDescrPrm))
    {
        memcpy(Buf, (void *)(((u32)&PultProgTablOne[0]) + ((u32)Adr << 1)), SzReq);
        return Ok_err;
    }
    else if ((AdrNet == 1) && (NumFile == fileFunctional))
    {
        FileFunctional.curFunct.functMask = TYPE_FUNCTIONAL;
        tmp = swapU16(FileFunctional.curFunct.functMask);
        FileFunctional.crc = GetCrc(&tmp, sizeof(u16));  
        memcpy(Buf, (void *)&FileFunctional, SzReq);
        return Ok_err;
    }
    else
    {
        return NonRdk_err;
    }
#else
    return ReQuestDrv(QueueDrvIn, MsgIn);
#endif
}*/

//-----------------------------------------------------------------------------
// Чтение текущих параметров с ПЧ пачками по 125 штук 0х67 функцией

Drv_err_type ReadTekAllPrmFromDrv(MsgReqDrvIn_type *MsgIn, u32 AdrData, void *Buf, u16 SzData)
{
    MsgIn->AdrNet   = 0x01;
    MsgIn->AdrData  = (u32)AdrData;
    MsgIn->Param    = 0;
    MsgIn->Sz       = SzData;

    MsgIn->Func     = READ_H_REGS;
    MsgIn->Buf      = Buf;
    MsgIn->SzBuf    = SzData*2;
    
#ifndef _PROJECT_IAR_ 
    MsgIn->countByteRead = 10 + SzData;  // Ожидаем по колич данных
#endif
    
    return ReQuestDrv(QueueDrvIn, MsgIn);;
}




//---------------------------------------------------
Drv_err_type ReadFileFromDrv(MsgReqDrvIn_type *MsgIn, u8 AdrNet, void *Adr, u8 SzReq, u8 NumFile, void *Buf, u16 SzBuf)
{
    MsgIn->AdrNet   = AdrNet;
    MsgIn->AdrData  = (u32)Adr;
    MsgIn->Param    = NumFile;
    MsgIn->Sz       = SzReq << 1;
// WARNING + Остаток от деления
    if (SzReq % 2 == 0)             MsgIn->Sz =  SzReq >> 1;
    else                            MsgIn->Sz = (SzReq >> 1) + 1;
    MsgIn->Func     = READ_FILE;
    MsgIn->Buf      = Buf;
    MsgIn->SzBuf    = SzBuf;
    //MsgIn->cntRepeat= 8;
    //MsgOut->Buf     = (u8 *)Buf;

#ifndef _PROJECT_IAR_ 
    MsgIn->countByteRead = 10 + SzReq;  // Ожидаем по колич данных
#endif
    
    return ReQuestDrv(QueueDrvIn, MsgIn);;
}
//---------------------------------------------------

/*********************************************
 * Формирование запроса на запись файла      *
 *  Входные параметры:                       *
 *  - указатель на сообщение                 *
 *  - адрес внутри файла                     *
 *  - записываемы объем в байтах             *
 *  - номер файла                            *
 *  - указатель на буфер записываемых данных *
 *  - размер буфера                          *
 *  Выходные параметры                       *
 *  - ошибка драйвера                        *
 *********************************************/
Drv_err_type WriteFileToDrv(MsgReqDrvIn_type *MsgIn, u8 AdrNet, void *Adr, u8 SzReq, u8 NumFile, void *Buf, u16 SzBuf)
{
    MsgIn->AdrNet   = AdrNet;
    MsgIn->AdrData  = (u32)Adr;
    MsgIn->Param    = NumFile;
    MsgIn->Sz       = SzReq << 1;

// WARNING + Остаток от деления
    if (SzReq % 2 == 0){
        MsgIn->Sz = SzReq >> 1;
    }
    else{
        MsgIn->Sz = (SzReq >> 1) + 1;
    }

    MsgIn->Func     = WRITE_FILE;
    MsgIn->Buf      = Buf;
    MsgIn->SzBuf    = SzBuf;
    //MsgOut->Buf     = (u8 *)Buf;

#ifndef _PROJECT_IAR_ 
    MsgIn->countByteRead = 5;  // Ожидаем по колич данных
#endif

    return ReQuestDrv(QueueDrvIn, MsgIn);
}

//===================================================================
// Формирвование запроса на уровень драйвера
//  =>* AdrNet - сетевой адрес привода
//      AdrData - адрес регистра данных
//      Sz - размер запрашиваемых данных
// *=>  TypeReq - тип запроса
//===================================================================
//char ReQuestDrv(unsigned char AdrNet, unsigned short int AdrData, unsigned char Sz, unsigned char TypeReq)
Drv_err_type ReQuestDrv(OS_EVENT *QueueMsg, MsgReqDrvIn_type *MsgIn)
{
    u8                      Err;
    MsgReqDrvIn_type        *pntMsgDrv;

    SendMsgClearQueueDrv();

    MsgIn->CleanQueue = 0;

    if(OSQPost(QueueMsg, MsgIn)){
        return ErrPostMB;
    }
    else{
        pntMsgDrv = OSQPend(QueueDrvOut, 1000, &Err);
        //memcpy(&MsgDrvOut, pntMsgDrvOut, sizeof(MsgReqDrvOut_type));
//        pntMsgDrv->Busy = 0;
        if (Err != OS_ERR_NONE){
            return ErrPendMB;
        }
        else if(pntMsgDrv->Err){
            return pntMsgDrv->Err;
        }
    }
    return Ok_err;
}
/*********************************************
 * Формирвование запроса на уровень драйвера *
 * без Pend                                  *
 *  =>* AdrNet - сетевой адрес привода       *
 *      AdrData - адрес данных               *
 *      Sz - размер запрашиваемых данных     *
 * *=>  TypeReq - тип запроса                *
 *********************************************/
//char ReQuestDrv(unsigned char AdrNet, unsigned short int AdrData, unsigned char Sz, unsigned char TypeReq)
Drv_err_type ReQuestDrvWoPend(OS_EVENT *QueueMsg, MsgReqDrvIn_type *MsgIn)
{
    MsgIn->CleanQueue = 0;
    if(OSQPost(QueueMsg, MsgIn)){
        return ErrPostMB;
    }
    return Ok_err;
}

/*********************************************
 * Формирование запроса на параметра         *
 *  =>*                                      *
 *                                           *
 *                                           *
 * *=>                                       *
 *********************************************/
Drv_err_type RequestModbusOpisPrmExt(Full_DescriptorPrm_type *DPDrive)
{
/*    if (DPDrive->Busy) {
        return ErrPrmBusy;
    }*/
    DPDrive->msgReqDrvIn.AdrNet   = DPDrive->AdrNet;
    DPDrive->msgReqDrvIn.AdrData  = 0x8000 + DPDrive->wm.AdrPrm;

// WARNING + Остаток от деления
    if (DPDrive->SzReq % 2 == 0)    DPDrive->msgReqDrvIn.Sz = DPDrive->SzReq >> 1;
    else                            DPDrive->msgReqDrvIn.Sz = (DPDrive->SzReq >> 1) + 1;

    DPDrive->msgReqDrvIn.Func     = READ_H_REGS;
    DPDrive->msgReqDrvIn.SzBuf    = sizeof(DPDrive->BufParam);
    DPDrive->msgReqDrvIn.Buf      = DPDrive->BufParam;
    DPDrive->msgReqDrvIn.NumParam = DPDrive->NumParam;

#ifndef _PROJECT_IAR_ 
    DPDrive->msgReqDrvIn.countByteRead = DPDrive->msgReqDrvIn.Sz*2 + 5;  // Ожидаем по колич данных
#endif

    return ReQuestDrvWoPend(QueueDrvIn, &DPDrive->msgReqDrvIn);
}
/*********************************************
 * Формирование запроса на параметра         *
 *  =>*                                      *
 *                                           *
 *                                           *
 * *=>                                       *
 *********************************************/
Drv_err_type RequestModbusOpisPrm(Full_DescriptorPrm_type *DPDrive)
{
/*    if (DPDrive->Busy) {
        return ErrPrmBusy;
    }*/
    DPDrive->msgReqDrvIn.AdrNet   = DPDrive->AdrNet;
    DPDrive->msgReqDrvIn.AdrData  = DPDrive->wm.AdrPrm;

// WARNING + Остаток от деления
    if (DPDrive->SzReq % 2 == 0)    DPDrive->msgReqDrvIn.Sz = DPDrive->SzReq >> 1;
    else                            DPDrive->msgReqDrvIn.Sz = (DPDrive->SzReq >> 1) + 1;
    DPDrive->msgReqDrvIn.Func     = READ_H_REGS;
    DPDrive->msgReqDrvIn.Buf      = DPDrive->BufParam;
    DPDrive->msgReqDrvIn.SzBuf    = sizeof(DPDrive->BufParam);
//    DPDrive->msgReqDrvIn.MsgOut   = &DPDrive->MsgReqDrvOut;
    DPDrive->msgReqDrvIn.Buf     =  DPDrive->BufParam;
    DPDrive->msgReqDrvIn.NumParam = DPDrive->NumParam;
//    DPDrive->msgReqDrvIn.MsgOut->Busy = 1;                          //Предотвращение избыточности запросов
//    DPDrive->Busy                 = 1;                        //Параметр занят обработкой, снимается при разборе принятых данных


#ifndef _PROJECT_IAR_ 
    DPDrive->msgReqDrvIn.countByteRead = DPDrive->msgReqDrvIn.Sz*2 + 5;  // Ожидаем по колич данных
#endif

    return ReQuestDrvWoPend(QueueDrvIn, &DPDrive->msgReqDrvIn);

}
/*********************************************
 * Формирование запроса на параметра         *
 *  =>*                                      *
 *                                           *
 *                                           *
 * *=>                                       *
 *********************************************/
Drv_err_type WritePrm(Full_DescriptorPrm_type *DPDrive)
{
    DPDrive->msgReqDrvIn.AdrNet   = DPDrive->AdrNet;
    DPDrive->msgReqDrvIn.AdrData  = DPDrive->wm.AdrPrm;

// WARNING + Остаток от деления
    if (DPDrive->SzReq % 2 == 0)    DPDrive->msgReqDrvIn.Sz = DPDrive->SzReq >> 1;
    else                            DPDrive->msgReqDrvIn.Sz = (DPDrive->SzReq >> 1) + 1;

    DPDrive->msgReqDrvIn.Func     = WRITE_REGS;
    DPDrive->msgReqDrvIn.Buf      = DPDrive->BufParam;
    DPDrive->msgReqDrvIn.SzBuf    = sizeof(DPDrive->BufParam);
    DPDrive->msgReqDrvIn.Buf      = DPDrive->BufParam;
    DPDrive->msgReqDrvIn.NumParam = DPDrive->NumParam;

#ifndef _PROJECT_IAR_ 
    DPDrive->msgReqDrvIn.countByteRead = 8;  // Ожидаем по колич данных
#endif

    return ReQuestDrvWoPend(QueueDrvIn, &DPDrive->msgReqDrvIn);
}
//===================================================================
// Формирование запроса на формирование запроса на запись состояния катушек
//===================================================================
u8  BufStartStop[16];   // Спец буфер для посылок пуск стоп

Drv_err_type StartStopDrive( u8 AdrNet, u16 AdrCoil, u8 onOff )
{
    MsgReqDrvIn_type    *MsgIn;
    u8                  *pntBuf;
    //Drv_err_type        drvErr;
    MsgReqDrvIn_type        *pntMsgDrv;
    u8                      Err;

    MsgIn   = &MsgReqDrvIn[MSG_STARTSTOP];
    pntBuf  = BufStartStop;

    pntBuf[0] = AdrCoil>>8;
    pntBuf[1] = (u8)AdrCoil;
    pntBuf[2] = onOff;
    pntBuf[3] = 0;

    MsgIn->AdrNet   = AdrNet;
    MsgIn->Func     = WRITE_COIL;
    MsgIn->Buf      = BufStartStop;
    MsgIn->SzBuf    = sizeof(BufStartStop);

#ifndef _PROJECT_IAR_ 
    MsgIn->countByteRead = 8;  // Ожидаем по колич данных
#endif
    
    //drvErr = ReQuestDrv(QueueDrvIn, MsgIn);
    if(OSQPostFront(QueueDrvIn, MsgIn)){
        return ErrPostMB;
    }
    else{
        //do{
        pntMsgDrv = OSQPend(QueueDrvOutStartStop, 100, &Err);
        //}while (pntMsgDrv != MsgIn);
        if (Err != OS_ERR_NONE){
            return ErrPendMB;
        }
        else if(pntMsgDrv->Err){
            return pntMsgDrv->Err;
        }
    }
    return Ok_err;
}

//
s32 ReqS16Prm(s16 GrpPrm)
{
    Drv_err_type Err = Ok_err;
    MsgReqDrvIn_type Msg = {0};
    u8 Buf[2] = {0};
    
    Msg.AdrNet = 1;
    Msg.Sz = 1;
    Msg.Buf = &Buf[0];
    Msg.Func = READ_H_REGS;
    Msg.SzBuf = sizeof(Buf);
    
#ifndef _PROJECT_IAR_ 
    Msg.countByteRead = reqMsgReqDrvIn.Sz*2 + 5;
#endif
    
    Msg.AdrData = GrpPrm;
    
    Err = ReQuestDrv(QueueDrvIn, &Msg);
    
    if(Err != Ok_err)
    {
        return -1;
    }
    
    return ((Buf[0] << 8) | Buf[1]);
}

//
s32 SendS16Prm(s16 GrpPrm, s16 Val)
{
    Drv_err_type Err = Ok_err;
    MsgReqDrvIn_type Msg = {0};
    u8 Buf[2] = {0};
    
    // Rotate value to save
    Buf[0] = Val >> 8;
    Buf[1] = Val;
    
    Msg.AdrNet = 1;
    Msg.Sz = 1;
    Msg.Buf = &Buf[0];
    Msg.Func = WRITE_REGS;
    Msg.SzBuf = sizeof(Buf);
    
#ifndef _PROJECT_IAR_ 
    Msg.countByteRead = 8;
#endif
    
    Msg.AdrData = GrpPrm;
    
    Err = ReQuestDrv(QueueDrvIn, &Msg);
    
    return Err;
}
//====================================================================
//  Формирование запросов на чтение типа языка
//====================================================================
u8 ReqLanguage(void)
{
  Drv_err_type            drvErr;
  MsgReqDrvIn_type        reqMsgReqDrvIn = {0};
  LnkNetFDP_type          LnkNetFDP;
  u8                      Buf[16];

  if (gIsNvsaOld){
      return 1;                     // У нас один язык - выходим
  }

  if (ReadLnk(&LnkNetFDP, 0)) {
      reqMsgReqDrvIn.AdrNet = LnkNetFDP.NetAdr;
  }
  else{
  //        MessageCancel("Сеть не установлена");
      reqMsgReqDrvIn.AdrNet = 1;
  }

  reqMsgReqDrvIn.Sz       = 1;
  reqMsgReqDrvIn.Func     = READ_H_REGS;

  reqMsgReqDrvIn.Buf      = &Buf[0];
  reqMsgReqDrvIn.SzBuf    = sizeof(Buf);

  #ifndef _PROJECT_IAR_ 
  reqMsgReqDrvIn.countByteRead = reqMsgReqDrvIn.Sz*2 + 5;  // Ожидаем по колич данных
  #endif
    reqMsgReqDrvIn.AdrData  = ADDR_LANGUAGE;

  drvErr = ReQuestDrv(QueueDrvIn, &reqMsgReqDrvIn);
  if( drvErr != Ok_err ){
     return 0;                                           // Тип языка не прочитан                                
  }
  else{
      TypeLanguage = swapU16(*(u16 *)&Buf[0]);
      return 1;                                          // Тип языка прочитан
  }
}
//====================================================================
//  Формирование запросов на чтение паролей из привода 
//====================================================================
u8 ReqEtalonPassword(void)
{
    // Проверяем - если группа паролей отсутствует, то не зачем долбить ПЧ 
//    if (GrpPassword == GRP_NOT_FOUND)
//       return;

    Drv_err_type            drvErr;
    MsgReqDrvIn_type        reqMsgReqDrvIn = {0};
    LnkNetFDP_type          LnkNetFDP;
    u8                      Buf[16];
    u8                      Err = 0;
            
    if (ReadLnk(&LnkNetFDP, 0)) {
        reqMsgReqDrvIn.AdrNet = LnkNetFDP.NetAdr;
    }
    else{
//        MessageCancel("Сеть не установлена");
        reqMsgReqDrvIn.AdrNet = 1;
    }

    reqMsgReqDrvIn.Sz       = 1;
    reqMsgReqDrvIn.Func     = READ_H_REGS;

    reqMsgReqDrvIn.Buf      = &Buf[0];
    reqMsgReqDrvIn.SzBuf    = sizeof(Buf);

#ifndef _PROJECT_IAR_ 
    reqMsgReqDrvIn.countByteRead = reqMsgReqDrvIn.Sz*2 + 5;  // Ожидаем по колич данных
#endif

//        MessageThrough("Чтение паролей");
    reqMsgReqDrvIn.AdrData  = ADDR_ETALON_USER;

    	
    drvErr = ReQuestDrv(QueueDrvIn, &reqMsgReqDrvIn);
    if( drvErr != Ok_err ){
        return 1;
    }
    else{
        AllPrmPnt->ETALON_USER = swapU16(*(u16 *)&Buf[0]);
        Err = 0;
    }
	

#ifndef _PROJECT_IAR_ 
    reqMsgReqDrvIn.countByteRead = reqMsgReqDrvIn.Sz*2 + 5;  // Ожидаем по колич данных
#endif
    reqMsgReqDrvIn.AdrData  = ADDR_USER;
    drvErr = ReQuestDrv(QueueDrvIn, &reqMsgReqDrvIn);

    if( drvErr == Ok_err ){
        AllPrmPnt->PASSW_USER = swapU16(*(u16 *)&Buf[0]);
    }

#ifndef _PROJECT_IAR_ 
    reqMsgReqDrvIn.countByteRead = reqMsgReqDrvIn.Sz*2 + 5;  // Ожидаем по колич данных
#endif
    reqMsgReqDrvIn.AdrData  = ADDR_ETALON_TEXN;
    drvErr = ReQuestDrv(QueueDrvIn, &reqMsgReqDrvIn);

    if( drvErr == Ok_err ){
        AllPrmPnt->ETALON_TEXN = swapU16(*(u16 *)&Buf[0]);
        Err &= ~2;
    }
    else{
        Err |= 2;
    }
        
#ifndef _PROJECT_IAR_ 
    reqMsgReqDrvIn.countByteRead = reqMsgReqDrvIn.Sz*2 + 5;  // Ожидаем по колич данных
#endif
    reqMsgReqDrvIn.AdrData  = ADDR_TEXN;
    drvErr = ReQuestDrv(QueueDrvIn, &reqMsgReqDrvIn);

    if( drvErr == Ok_err ){
        AllPrmPnt->PASSW_TEXN = swapU16(*(u16 *)&Buf[0]);
    }

#ifndef _PROJECT_IAR_ 
    reqMsgReqDrvIn.countByteRead = reqMsgReqDrvIn.Sz*2 + 5;  // Ожидаем по колич данных
#endif
        reqMsgReqDrvIn.AdrData  = ADDR_ETALON_MASTER;
    drvErr = ReQuestDrv(QueueDrvIn, &reqMsgReqDrvIn);
    if( drvErr == Ok_err ){
        AllPrmPnt->ETALON_MASTER = swapU16(*(u16 *)&Buf[0]);
        Err &= ~4;
    }
    else{
        Err |= 4;
    }
        
#ifndef _PROJECT_IAR_ 
    reqMsgReqDrvIn.countByteRead = reqMsgReqDrvIn.Sz*2 + 5;  // Ожидаем по колич данных
#endif
    reqMsgReqDrvIn.AdrData  = ADDR_MASTER;
    drvErr = ReQuestDrv(QueueDrvIn, &reqMsgReqDrvIn);

    if( drvErr == Ok_err ){
        AllPrmPnt->PASSW_MASTER = swapU16(*(u16 *)&Buf[0]);
    }

    //#warning А ЗАЧЕМ ЗДЕСЬ ДЕЛАТЬ ПРОВЕРКУ СВЯЗИ? А ЕСЛИ ПАРОЛЕЙ НЕТУ, ТО КАК БЫТЬ???
    if (Err) {
      return 1;  
      //MessageCancel(GetStringMess(MessageMess_ERROR_CONECT));
    }
    return 0;
}
/*********************************************
 * Формирование запроса на параметра         *
 *  =>*                                      *
 *                                           *
 *                                           *
 * *=>                                       *
 *********************************************/
Drv_err_type ReqStatus(Full_DescriptorPrm_type *DPDrive)
{
    DPDrive->msgReqDrvIn.AdrNet   = DPDrive->AdrNet;
    DPDrive->msgReqDrvIn.AdrData  = ADDR_STATUS;//DPDrive->wm.AdrPrm;

    // WARNING + Остаток от деления
    if (DPDrive->SzReq % 2 == 0)    DPDrive->msgReqDrvIn.Sz = DPDrive->SzReq >> 1;
    else                            DPDrive->msgReqDrvIn.Sz = (DPDrive->SzReq >> 1) + 1;

    DPDrive->msgReqDrvIn.Func     = READ_H_REGS;
    DPDrive->msgReqDrvIn.Buf      = DPDrive->BufParam;
    DPDrive->msgReqDrvIn.SzBuf    = sizeof(DPDrive->BufParam);
    DPDrive->msgReqDrvIn.NumParam = DPDrive->NumParam;

#ifndef _PROJECT_IAR_ 
    DPDrive->msgReqDrvIn.countByteRead = DPDrive->msgReqDrvIn.Sz*2 + 5;  // Ожидаем по колич данных
#endif

    return ReQuestDrvWoPend(QueueDrvIn, &DPDrive->msgReqDrvIn);
}
//===================================================================
//  WARNING  WARNING  WARNING  WARNING  WARNING  WARNING
//===================================================================
Drv_err_type ScanNet(NetPrm_type *NetPrm)
{
/////    MsgReqDrvIn_type MsgIn;
/////    Drv_err_type Err;
/////    u16 Adr, Speed;
/////
/////    u8 Buf[130];
/////
/////   for (Adr = 1; Adr < 256; Adr++) {
/////        /*for (Speed = 0; Speed < GetNumUartSpeed(); Speed++)*/ {
/////            //SetUartSpeed(Speed);
/////            Err = Request0x2B(&MsgIn, Adr, &Buf[0], sizeof(Buf));
/////            sprintf((char *)&Buf[0], "Скорость %s бит/с, адрес %d", "230400"/*BdrtName[Speed]*/, Adr);
/////            MessageThrough((char *)&Buf[0]);
/////            if (!Err) {
/////                break;
/////            }
/////        }
/////      if (!Err) {
/////           break;
/////       }
/////   }
/////        //SetUartUstSpeed();

    NetPrm->Adr     = 1;    //(u8)Adr;   WARNING
    NetPrm->Speed   = 10;   //(u8)Speed; WARNING

//    return Err;
    return Ok_err;
}
/*********************************************
 * Очистка очереди         *
 *  =>*                                      *
 *                                           *
 *                                           *
 * *=>                                       *
 *********************************************/
void WaitCleanQueryFromDrv(void)
{
    OS_Q_DATA               QData;

    MsgReqDrvIn_type       MsgDrv;


    do{
        WaitMsgFromDrv(&MsgDrv);             //Холостая вычитка выходных сообщений
        OSQQuery(QueueDrvIn, &QData);           //Определение количества входных сообщений
    }while(QData.OSNMsgs > 0);
}

u8 WaitMsgFromDrv(MsgReqDrvIn_type *pMsg)
{
    MsgReqDrvIn_type   *pMsgWait;
    u8                  Err;

    pMsgWait = OSQPend(QueueDrvOut, 1000, &Err);
    if (Err == OS_ERR_NONE) {
        memcpy(pMsg, pMsgWait, sizeof(MsgReqDrvIn_type));
        //pMsgWait->Busy = 0;
    }
    return Err;
}
/*********************************************
 * Посылка сообщения драйверу для очистки    *
 * очереди                                   *
 *  =>*                                      *
 *                                           *
 *                                           *
 * *=>                                       *
 *********************************************/
Drv_err_type SendMsgClearQueueDrv(void)
{
    OS_Q_DATA               QDataIn, QDataOut;
    MsgReqDrvIn_type        MsgDrv, *pntMsgDrv;
    u8                      err;

    OSQQuery(QueueDrvIn, &QDataIn);
    OSQQuery(QueueDrvOut, &QDataOut);

    OSQFlush(QueueDrvIn);

    MsgDrv.CleanQueue = 1;
    MsgDrv.Func = READ_H_REGS;
    OSQPost(QueueDrvIn, &MsgDrv);
    do{
        pntMsgDrv = OSQPend(QueueDrvOut, 1000, &err);
    }while (pntMsgDrv != &MsgDrv);

/*    OSTaskDel(4);
    OSQFlush(QueueDrvIn);
    OSQFlush(QueueDrvOut);
    OSTaskCreate(TaskDRV, (void *)0, &DRVStack[STKDRVSIZE - 1], 4);  // Создание задачи DRV*/

    return Ok_err;
}
/*********************************************
 * Посылка сообщения драйверу для очистки    *
 * очереди                                   *
 *  =>*                                      *
 *                                           *
 *                                           *
 * *=>                                       *
 *********************************************/
/*void ClearQueueDrv(void)
{
    u16 i;
    OSQFlush(QueueDrvOut);
    for (i = 0; i < 8; i++) {
        MsgOut[i].Busy = 0;
    }

}*/
#ifdef TEST_P24
/*********************************************
 * Передача строки по уарту                  *
 *  =>*                                      *
 *                                           *
 *                                           *
 * *=>                                       *
 *********************************************/
/*********************************************
 * Формирвование запроса на уровень драйвера *
 *  =>* AdrNet - сетевой адрес привода       *
 *      AdrData - адрес данных               *
 *      Sz - размер запрашиваемых данных     *
 * *=>  TypeReq - тип запроса                *
 *********************************************/
//char ReQuestDrv(unsigned char AdrNet, unsigned short int AdrData, unsigned char Sz, unsigned char TypeReq)
Drv_err_type ReQuestDrvTest(OS_EVENT *QueueMsg, MsgReqDrvIn_type *MsgIn)
{
    INT8U err;

    if(OSQPost(QueueMsg, MsgIn))
        return ErrPostMB;
    else{
        OSSemPend(UARTSemTx, 0, &err);
        return Ok_err;
    }
}
//===================================================================
//===================================================================
MsgReqDrvIn_type    MsgReqDrvInTest;
void testSendString(void)
{
    MsgReqDrvInTest.Buf      = (u8*)&StrOut[0];
    MsgReqDrvInTest.Sz    = strlen(&StrOut[0]);

    //ReQuestDrvWoPend(QueueDrvIn, &MsgReqDrvInTest);
    ReQuestDrvTest(QueueDrvIn, &MsgReqDrvInTest);

}
//===================================================================
#endif
