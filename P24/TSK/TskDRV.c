
//======================================================================
#include "globalIncludes.h"
//#include "AT_24_prog_Pult_Mem_One.h"
#include "AT_24_prog_Pult_Mem.h"
//======================================================================
extern AllPrm_Type *AllPrmPnt;
extern uint8_t gIsNvsaOld;

//const char *const msgError = GetStringMess(MessageMess_ERROR_CONECT);

MsgReqDrvIn_type    MsgReqDrvIn[8]; // ������ ��������� �� ������ ������� � ������ ������
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
// ������ ������ � �������� �������� ��� �������
//  ������ ���������� �� �������� �� TaskLCD
//  �������:
//    ������������ ������ "����" � "����"
//    ����� � ��������
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
    u16 w[szTempArUart];      // ������ ��� ������������ ������� �� ������������� � ���������
    u8  b[szTempArUart*2];
}tmpArUart;
//======================================================================
inline void ManchesterEncode(void *OutPtr, void *InPtr, u16 size );

#define MAX_REPEAT_UART  5-1      /* ������������ ���������� ������������ */

//======================================================================
//  ����������� ������ ������ � �������� ����� ������������ �������
//              ��������� ��� �������� ������
//======================================================================
#ifdef _PROJECT_IAR_
__noreturn void TaskDRV(void *pdata)
{
    u8                              err;
    u8                              cntRepeat;  // ������� ������������
    u16                             size;

    Drv_err_type                    errTA;
    portUart_type                   *uart;
    MsgReqDrvIn_type                *pntMsgIn; //, MsgIn;

#ifndef _PROJECT_IAR_
    rt_hw_usart_init();             // ������������� �������� Uart
#endif
    uart = &uart1;                  // ��������� �� uart1

    OSSemPend (UARTSem, 200, &err);

    cntRepeat       = MAX_REPEAT_UART;
    uart->state     = NON_TA;

    while(1){
      ResetWD(tsk_drv);
      OSSemPend( UARTSem, 1000, &err );     // ������ �� ��������
     
      if(uart->state == NON_TA){                                  // ������ ������
          pntMsgIn = OSQPend(QueueDrvIn, SYS_FOREVER, &err);      // �������� ������ � �������
          
          if(pntMsgIn->CleanQueue == 1){                          // ���������� ���������� ������� ���������
              SendMsgToTask(pntMsgIn, NULL, 0);                   // �������������� ������
              pntMsgIn = OSQPend(QueueDrvIn, SYS_FOREVER, &err);  // ����� ���� ���������
          }

          // ��������� ������ � �������� �����
          size = ModbusFunc( pntMsgIn, tmpArUart.w );             // ������������ ������
          ManchesterEncode( uart->bufTx, tmpArUart.w, size );     // ������������ � ���������
          uart->tx_size = size;
          uart->state = TX_READY;                                 // ������ ������
          cntRepeat = 0;                                          // ������������� �������� �������� �������� �������
          sendRequest(uart);                                      //!��������� �������� ������� � ������
        
      }
      else if((uart->state == RX_READY)&&(err == OS_ERR_NONE)){     // ������ ����� �� ��
          errTA = Ok_err;
          
          size = uart->rx_size;
          ManchesterDecode( DataBuf, UartBufRx, size );             // ���������� UartBufRx ���������� � DataBuf 
          
          errTA = SendMsgToTask(pntMsgIn, DataBuf, size );          // ������ �������� ������
          if((errTA == Ok_err)||(errTA == ErrReqMB)){               // �������� ���������� ����� �� ��                                  
              pntMsgIn = OSQPend(QueueDrvIn, SYS_FOREVER, &err);    // �������� ��������� ������ �� ������� �������� � �����
              
              if(pntMsgIn->CleanQueue == 1 ){                       // ���������� ���������� ������� ���������
                SendMsgToTask(pntMsgIn, NULL, 0);                   // �������������� ������
                pntMsgIn = OSQPend(QueueDrvIn, SYS_FOREVER, &err);  // ����� ���� ���������
              }

              // ��������� ������ � �������� �����
              size = ModbusFunc( pntMsgIn, tmpArUart.w );             // ������������ ������
              ManchesterEncode( uart->bufTx, tmpArUart.w, size );     // ������������ � ���������
              uart->tx_size = size;
              uart->state = TX_READY;                                 // ������ ������
              cntRepeat = 0;                                          // ������������� �������� �������� �������� �������
              sendRequest(uart);                                      // ��������� �������� ������� � ������
          }
          else{                                                       // ����� �� �� �� ����������!!!
              uart->state = RX_TIME_ERR;                             // ������ �� ��������� ������
              OSSemPost(UARTSem);                                     // ��������� ������� ������
          }
     }
     else if((uart->state == RX_TIME_ERR)||(err != OS_ERR_NONE)){    // ����� �� ������� �� ���������� �����
         if(cntRepeat < MAX_REPEAT_UART){
             cntRepeat++;                                            // ������� ������������ �������� � ��
             
             size = ModbusFunc( pntMsgIn, tmpArUart.w );             // ������������ �������� ������
             ManchesterEncode( uart->bufTx, tmpArUart.w, size );     // �������� ������ � ������ �����������
             uart->tx_size = size;
             uart->state = TX_READY;                                 // ������ ������
             sendRequest(uart);                                      // ��������� �������� ������� � ������
         }
         else{                                                       // �� ��������� MAX_REPEAT_UART �������
              cntRepeat = 0;                                         //!����� �������� �������� ������, � ������� ������� ���� �����
              uart->state = NON_TA;                                  //!��������� ������� �������. ����� ���������� ����� ������
              
              // ��������� � ������ �� ������ �� ����� � ��������
              pntMsgIn->Err = NonRdk_err;                            // ������ ��� ������
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
     
     
     
      // ������ �� ��������
     if( ((err == OS_ERR_NONE)||(uart->state == NON_TA))&&(uart->state != RX_TIME_ERR)  ){      // ������� ����� �� ����� ��� ������ ������
          errTA = Ok_err;
          if( uart->state == RX_READY )
          {                          // ���������� �����
              //size = uart->rx_size / 2; //!����� �� 2, ��� ��� �� ��� ����� �������� ������ ���������� 1 �������� ���� 
              size = uart->rx_size;
              ManchesterDecode( DataBuf, UartBufRx, size );       // ���������� UartBufRx ���������� � DataBuf 
              errTA = SendMsgToTask(pntMsgIn, DataBuf, size );    // ���������� ��������� �������
          }

          if( Ok_err == errTA )
          {
              pntMsgIn = OSQPend(QueueDrvIn, SYS_FOREVER, &err);  // �������� ��������� ������
          }
          else
          {
              // ���������� ������ ������ ������
              debugStop();
          }
          if(pntMsgIn->CleanQueue == 1 ){                         // ���������� ���������� ������� ���������
              SendMsgToTask(pntMsgIn, NULL, 0);                   // �������������� ������
              pntMsgIn = OSQPend(QueueDrvIn, SYS_FOREVER, &err);  // ����� ���� ���������
          }

          // ��������� ������ � �������� �����
          size = ModbusFunc( pntMsgIn, tmpArUart.w );             // ������������ ������
          ManchesterEncode( uart->bufTx, tmpArUart.w, size );     // ������������ � ���������
          uart->tx_size = size;
          uart->state = TX_READY;                                 // ������ ������
          cntRepeat = 0;                                          // ������������� �������� �������� �������� �������
          sendRequest(uart);                                      //!��������� �������� ������� � ������
     }
     else if( (uart->state == RX_TIME_ERR) || err != OS_ERR_NONE ){          // ����� �� ������� �� ���������� �����
     if( cntRepeat < MAX_REPEAT_UART ){
             cntRepeat++; //!������� �������� ������
             uart->state = TX_READY;                              // ������ ������� (����������� ��������� "���� �����")
         }
         else{ //!�������� ������� �� ���������
              // ��������� � ������ �� ������ �� ����� � ��������
              cntRepeat = 0; //!����� �������� �������� ������, � ������� ������� ���� �����
              uart->state = NON_TA; //!��������� ������� �������. ����� ���������� ������ �����
         }
     }
     else{
         // ????????????????
     }*/
  }
}
#else
//===================================================================
// ������ ��� ���������� ���� ��������

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

    uart            = &uart1;                  // ��������� �� uart1
    uart->state     = NON_TA;

    while(1){
        do{
            OSTimeDly(100);
            errInitUart = hw_usart_init();                          // ������������� �������� Uart

            err = 1;
            while(1){
                pntMsgIn = OSQAccept( QueueDrvIn, &err );
                if( pntMsgIn == NULL ){
                    break;
                }
                else{
                    SendMsgToTask(pntMsgIn, NULL, 0 );              // ���������� ��������� �������
                }
            };
        }while(errInitUart == false);


        while(1){
            pntMsgIn = OSQPend(QueueDrvIn, SYS_FOREVER, &err);      // ����� ���� ���������
            
            if(pntMsgIn != NULL){
                if(pntMsgIn->CleanQueue == 1 ){                         // ���������� ���������� ������� ���������
                    SendMsgToTask(pntMsgIn, NULL, 0);                   // �������������� ������
                    continue;                                           // ����� ���� ���������
                }

                if(comPortSetup.comIsInit == 0 ){                       // �������� ���� ���� ��� ������ 
                    ClosePort(&comPortSetup);
                    pntMsgIn->Err = ErrDebug;
                    size = 0;
                    errTA = SendMsgToTask(pntMsgIn, DataBuf, size );    // ���������� ��������� �������
                    break; 
                }  

                pntMsgIn->countByteWrite = ModbusFunc( pntMsgIn, &tmpArUart.b[0] );     // ��������� ������ � �������� �����
                pntMsgIn->cntRepeat = 0;                                                // �������� ������� �������� �������
                while(1){                                                               // SendMsgToTask ������� ���������� �������� � ������������ ��������� ������
                    errRx = txRxUart(pntMsgIn, &tmpArUart.b[0], DataBuf );              // ������� ��������� � ������� ������
                    //if (errRx == noErr)
                    //{
                        errTA = SendMsgToTask(pntMsgIn, DataBuf, pntMsgIn->SzOut );         // �������� ��������� ��������� �������
                        if( errTA == Ok_err ) break;
                    //}
                }
            }
        }
    }
}
//===================================================================
//          ������������� ��� ����� � ����������
//===================================================================
bool hw_usart_init(void)
{
	return OpenPort( &comPortSetup ) ;
}
//===================================================================
#endif
//===================================================================
// ������������ ������ � ������ ���� �� ��������
//===================================================================
#ifndef _PROJECT_IAR_
    extern int         isFlashChanged;      // ������� ��������� � ���� ��� ��������
#endif
//===================================================================
Drv_err_type SendMsgToTask( MsgReqDrvIn_type *MsgIn, u8 *pBuf, u16 size )
{
    if( MsgIn->CleanQueue ){                // ��������� ������� ������� ���������
        MsgIn->Err = Ok_err;          
    }
    else if ( size < 4 ){
        MsgIn->Err = ErrSzMB;               // ���� ������
        
    }
    else if(GetCrc( pBuf, size ) != 0 ){
        MsgIn->Err = ErrKsMB;               // ������ ����������� �����
    }
    else if( pBuf[1] & 0x80){
        MsgIn->Err = ErrReqMB;              // ����� �� ������� �� �����������
    }
    else{
        MsgIn->Err = Ok_err;                // �����

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
                 isFlashChanged = 1;      // ������� ��������� � ���� ��� ��������
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
            //return  MsgIn->Err;     // ������ �������
        }
        return  MsgIn->Err;           // ������ �������
    }
    if(MsgIn->Func != WRITE_COIL ){
        OSQPost(QueueDrvOut, MsgIn);
    }
    else{
        OSQPost(QueueDrvOutStartStop, MsgIn);
    }
    return Ok_err;                  // ��������� ��������� �������
}
//===================================================================
//      ������������ ������� �� ������ �������������� �������
//===================================================================
Drv_err_type Request0x2B(MsgReqDrvIn_type *MsgIn, u8 AdrNet, void *Buf, u16 SzBuf)
{

    MsgIn->AdrNet   = AdrNet;
    MsgIn->AdrData  = 0;
    MsgIn->Param    = 1;            // 1-� ����
    MsgIn->Sz       = 64;
    MsgIn->Func     = READ_DEV_ID;
    MsgIn->Buf      = Buf;
    MsgIn->SzBuf    = SzBuf;

#ifndef _PROJECT_IAR_    
    MsgIn->countByteRead = 256;     // ������� �� �������
#endif

    return ReQuestDrv(QueueDrvIn, MsgIn);
}

//===================================================================
// ������������ ������� �� ������ ��������� ���������� ��
//===================================================================
/*
Drv_err_type RequestFDP(MsgReqDrvIn_type *MsgIn, u8 AdrNet, void *Adr, u8 SzReq, void *Buf, u16 SzBuf)
{
    MsgIn->AdrNet   = AdrNet;
    MsgIn->AdrData  = (u32)Adr;
    MsgIn->Param    = 1;        //0-� ����
    MsgIn->Sz       = SzReq;
    MsgIn->Func     = READ_FILE;
    MsgIn->Buf      = Buf;
    MsgIn->SzBuf    = SzBuf;
//    MsgIn->MsgOut   = MsgOut;
    //MsgOut->Buf     = (u8 *)Buf;

#ifndef _PROJECT_IAR_ 
    MsgIn->countByteRead = 10 + SzReq*2 ;  // ������� �� ����� ������
#endif
    return ReQuestDrv(QueueDrvIn, MsgIn);
}*/
/*********************************************
 * ������������ ������� �� ������ ������     *
 * �����������                               *
 *  ������� ���������:                       *
 *  - ��������� �� ���������                 *
 *  - ������� �����                          *
 *  - ����� �����                            *
 *  - ��������� �� ��������� ���� ������ ���-*
 * ���                                       *
 *  �������� ���������                       *
 *  - ������ ��������                        *
 *********************************************/
Drv_err_type ReadVerFunct(MsgReqDrvIn_type *MsgIn, u8 AdrNet, u8 NumFile, FileFunctional_type *pFunct)
{
    Drv_err_type        errCode = Ok_err; // ������ ���� ����������� � ��������� ������ ��������
    u16                 crc, tmp;
    
    errCode = ReadFileFromDrv(MsgIn, AdrNet, 0, sizeof(FileFunctional_type), NumFile, pFunct, sizeof(FileFunctional_type));
    
    // ���� �������� ����������� ����� - �� ���� ��������� ���, ��� ����� ���� � ������ ��� �� ������
    if (errCode == ErrReqMB) {
        MissingFileWord |= (1 << NumFile); // ��������� ���� � ���������� 
        errCode = Ok_err; // �������� ��� ��������

        // ����������� ������� "�������" ����������, ����� ��� ������� ��� ��� � �������.
        pFunct->curFunct.functMask = FULL_FUNCTIONAL_MASK; // ����� ��� ���������� ����������� !*!*!
        tmp = swapU16(pFunct->curFunct.functMask);
        crc = GetCrc(&tmp, sizeof(u16)); // ��������� CRC � ����������� � ���������
        pFunct->crc = crc;
    }
    else { // ���� ���� ���������� - �������� ��� � ������� ������� ������
      MissingFileWord &= ~(1 << NumFile); // 
    }
  
    return (errCode); // ������� ������
}
/*********************************************
 * ������������ ������� �� ������ ��������   *
 * �������                                   *
 *  ������� ���������:                       *
 *  - ��������� �� ���������                 *
 *  - ������� �����                          *
 *  - ����� �����                            *
 *  - ��������� �� ��������� �������� �����  *
 *  ��                                       *
 *  �������� ���������                       *
 *  - ������ ��������                        *
 *********************************************/
Drv_err_type ReadJrnIndex(MsgReqDrvIn_type *MsgIn, u8 AdrNet, u8 NumFile, Index_type *indexJrn)
{
    return ReadFileFromDrv(MsgIn, AdrNet, 0, sizeof(Index_type), NumFile, indexJrn, sizeof(Index_type));
}

/*********************************************
 * ������������ ������� �� ������ �������    *
 *  ������� ���������:                       *
 *  - ��������� �� ���������                 *
 *  - ������� �����                          *
 *  - ����� �����                            *
 *  - ��������� �� ��������� �������         *
 *  �������� ���������                       *
 *  - ������ ��������                        *
 *********************************************/
/*Drv_err_type ReadJrn(u8 AdrNet, void *AdrInFile, u8 NumFile, u8 *RecJrn, u16 szReq)
{
    return ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], AdrNet, AdrInFile, sizeof(RecJrn_type), NumFile, RecJrn, sizeof(RecJrn_type));
    ReadFileFromDrv()
}*/


/*********************************************
 * ������������ ������� �� ������ �����      *
 *  ������� ���������:                       *
 *  - ��������� �� ���������                 *
 *  - ����� ������ �����                     *
 *  - ������������� ����� � ������           *
 *  - ����� �����                            *
 *  - ��������� �� ����� ��� �������� ������ *
 *  - ������ ������                          *
 *  �������� ���������                       *
 *  - ������ ��������                        *
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
// WARNING + ������� �� �������
    if (SzReq % 2 == 0)             MsgIn->Sz =  SzReq >> 1;
    else                            MsgIn->Sz = (SzReq >> 1) + 1;
    MsgIn->Func     = READ_FILE;
    MsgIn->Buf      = Buf;
    MsgIn->SzBuf    = SzBuf;
    //MsgOut->Buf     = (u8 *)Buf;

#ifndef _PROJECT_IAR_ 
    MsgIn->countByteRead = 10 + SzReq*2 ;  // ������� �� ����� ������
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
// ������ ������� ���������� � �� ������� �� 125 ���� 0�67 ��������

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
    MsgIn->countByteRead = 10 + SzData;  // ������� �� ����� ������
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
// WARNING + ������� �� �������
    if (SzReq % 2 == 0)             MsgIn->Sz =  SzReq >> 1;
    else                            MsgIn->Sz = (SzReq >> 1) + 1;
    MsgIn->Func     = READ_FILE;
    MsgIn->Buf      = Buf;
    MsgIn->SzBuf    = SzBuf;
    //MsgIn->cntRepeat= 8;
    //MsgOut->Buf     = (u8 *)Buf;

#ifndef _PROJECT_IAR_ 
    MsgIn->countByteRead = 10 + SzReq;  // ������� �� ����� ������
#endif
    
    return ReQuestDrv(QueueDrvIn, MsgIn);;
}
//---------------------------------------------------

/*********************************************
 * ������������ ������� �� ������ �����      *
 *  ������� ���������:                       *
 *  - ��������� �� ���������                 *
 *  - ����� ������ �����                     *
 *  - ����������� ����� � ������             *
 *  - ����� �����                            *
 *  - ��������� �� ����� ������������ ������ *
 *  - ������ ������                          *
 *  �������� ���������                       *
 *  - ������ ��������                        *
 *********************************************/
Drv_err_type WriteFileToDrv(MsgReqDrvIn_type *MsgIn, u8 AdrNet, void *Adr, u8 SzReq, u8 NumFile, void *Buf, u16 SzBuf)
{
    MsgIn->AdrNet   = AdrNet;
    MsgIn->AdrData  = (u32)Adr;
    MsgIn->Param    = NumFile;
    MsgIn->Sz       = SzReq << 1;

// WARNING + ������� �� �������
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
    MsgIn->countByteRead = 5;  // ������� �� ����� ������
#endif

    return ReQuestDrv(QueueDrvIn, MsgIn);
}

//===================================================================
// ������������� ������� �� ������� ��������
//  =>* AdrNet - ������� ����� �������
//      AdrData - ����� �������� ������
//      Sz - ������ ������������� ������
// *=>  TypeReq - ��� �������
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
 * ������������� ������� �� ������� �������� *
 * ��� Pend                                  *
 *  =>* AdrNet - ������� ����� �������       *
 *      AdrData - ����� ������               *
 *      Sz - ������ ������������� ������     *
 * *=>  TypeReq - ��� �������                *
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
 * ������������ ������� �� ���������         *
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

// WARNING + ������� �� �������
    if (DPDrive->SzReq % 2 == 0)    DPDrive->msgReqDrvIn.Sz = DPDrive->SzReq >> 1;
    else                            DPDrive->msgReqDrvIn.Sz = (DPDrive->SzReq >> 1) + 1;

    DPDrive->msgReqDrvIn.Func     = READ_H_REGS;
    DPDrive->msgReqDrvIn.SzBuf    = sizeof(DPDrive->BufParam);
    DPDrive->msgReqDrvIn.Buf      = DPDrive->BufParam;
    DPDrive->msgReqDrvIn.NumParam = DPDrive->NumParam;

#ifndef _PROJECT_IAR_ 
    DPDrive->msgReqDrvIn.countByteRead = DPDrive->msgReqDrvIn.Sz*2 + 5;  // ������� �� ����� ������
#endif

    return ReQuestDrvWoPend(QueueDrvIn, &DPDrive->msgReqDrvIn);
}
/*********************************************
 * ������������ ������� �� ���������         *
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

// WARNING + ������� �� �������
    if (DPDrive->SzReq % 2 == 0)    DPDrive->msgReqDrvIn.Sz = DPDrive->SzReq >> 1;
    else                            DPDrive->msgReqDrvIn.Sz = (DPDrive->SzReq >> 1) + 1;
    DPDrive->msgReqDrvIn.Func     = READ_H_REGS;
    DPDrive->msgReqDrvIn.Buf      = DPDrive->BufParam;
    DPDrive->msgReqDrvIn.SzBuf    = sizeof(DPDrive->BufParam);
//    DPDrive->msgReqDrvIn.MsgOut   = &DPDrive->MsgReqDrvOut;
    DPDrive->msgReqDrvIn.Buf     =  DPDrive->BufParam;
    DPDrive->msgReqDrvIn.NumParam = DPDrive->NumParam;
//    DPDrive->msgReqDrvIn.MsgOut->Busy = 1;                          //�������������� ������������ ��������
//    DPDrive->Busy                 = 1;                        //�������� ����� ����������, ��������� ��� ������� �������� ������


#ifndef _PROJECT_IAR_ 
    DPDrive->msgReqDrvIn.countByteRead = DPDrive->msgReqDrvIn.Sz*2 + 5;  // ������� �� ����� ������
#endif

    return ReQuestDrvWoPend(QueueDrvIn, &DPDrive->msgReqDrvIn);

}
/*********************************************
 * ������������ ������� �� ���������         *
 *  =>*                                      *
 *                                           *
 *                                           *
 * *=>                                       *
 *********************************************/
Drv_err_type WritePrm(Full_DescriptorPrm_type *DPDrive)
{
    DPDrive->msgReqDrvIn.AdrNet   = DPDrive->AdrNet;
    DPDrive->msgReqDrvIn.AdrData  = DPDrive->wm.AdrPrm;

// WARNING + ������� �� �������
    if (DPDrive->SzReq % 2 == 0)    DPDrive->msgReqDrvIn.Sz = DPDrive->SzReq >> 1;
    else                            DPDrive->msgReqDrvIn.Sz = (DPDrive->SzReq >> 1) + 1;

    DPDrive->msgReqDrvIn.Func     = WRITE_REGS;
    DPDrive->msgReqDrvIn.Buf      = DPDrive->BufParam;
    DPDrive->msgReqDrvIn.SzBuf    = sizeof(DPDrive->BufParam);
    DPDrive->msgReqDrvIn.Buf      = DPDrive->BufParam;
    DPDrive->msgReqDrvIn.NumParam = DPDrive->NumParam;

#ifndef _PROJECT_IAR_ 
    DPDrive->msgReqDrvIn.countByteRead = 8;  // ������� �� ����� ������
#endif

    return ReQuestDrvWoPend(QueueDrvIn, &DPDrive->msgReqDrvIn);
}
//===================================================================
// ������������ ������� �� ������������ ������� �� ������ ��������� �������
//===================================================================
u8  BufStartStop[16];   // ���� ����� ��� ������� ���� ����

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
    MsgIn->countByteRead = 8;  // ������� �� ����� ������
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
//  ������������ �������� �� ������ ���� �����
//====================================================================
u8 ReqLanguage(void)
{
  Drv_err_type            drvErr;
  MsgReqDrvIn_type        reqMsgReqDrvIn = {0};
  LnkNetFDP_type          LnkNetFDP;
  u8                      Buf[16];

  if (gIsNvsaOld){
      return 1;                     // � ��� ���� ���� - �������
  }

  if (ReadLnk(&LnkNetFDP, 0)) {
      reqMsgReqDrvIn.AdrNet = LnkNetFDP.NetAdr;
  }
  else{
  //        MessageCancel("���� �� �����������");
      reqMsgReqDrvIn.AdrNet = 1;
  }

  reqMsgReqDrvIn.Sz       = 1;
  reqMsgReqDrvIn.Func     = READ_H_REGS;

  reqMsgReqDrvIn.Buf      = &Buf[0];
  reqMsgReqDrvIn.SzBuf    = sizeof(Buf);

  #ifndef _PROJECT_IAR_ 
  reqMsgReqDrvIn.countByteRead = reqMsgReqDrvIn.Sz*2 + 5;  // ������� �� ����� ������
  #endif
    reqMsgReqDrvIn.AdrData  = ADDR_LANGUAGE;

  drvErr = ReQuestDrv(QueueDrvIn, &reqMsgReqDrvIn);
  if( drvErr != Ok_err ){
     return 0;                                           // ��� ����� �� ��������                                
  }
  else{
      TypeLanguage = swapU16(*(u16 *)&Buf[0]);
      return 1;                                          // ��� ����� ��������
  }
}
//====================================================================
//  ������������ �������� �� ������ ������� �� ������� 
//====================================================================
u8 ReqEtalonPassword(void)
{
    // ��������� - ���� ������ ������� �����������, �� �� ����� ������� �� 
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
//        MessageCancel("���� �� �����������");
        reqMsgReqDrvIn.AdrNet = 1;
    }

    reqMsgReqDrvIn.Sz       = 1;
    reqMsgReqDrvIn.Func     = READ_H_REGS;

    reqMsgReqDrvIn.Buf      = &Buf[0];
    reqMsgReqDrvIn.SzBuf    = sizeof(Buf);

#ifndef _PROJECT_IAR_ 
    reqMsgReqDrvIn.countByteRead = reqMsgReqDrvIn.Sz*2 + 5;  // ������� �� ����� ������
#endif

//        MessageThrough("������ �������");
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
    reqMsgReqDrvIn.countByteRead = reqMsgReqDrvIn.Sz*2 + 5;  // ������� �� ����� ������
#endif
    reqMsgReqDrvIn.AdrData  = ADDR_USER;
    drvErr = ReQuestDrv(QueueDrvIn, &reqMsgReqDrvIn);

    if( drvErr == Ok_err ){
        AllPrmPnt->PASSW_USER = swapU16(*(u16 *)&Buf[0]);
    }

#ifndef _PROJECT_IAR_ 
    reqMsgReqDrvIn.countByteRead = reqMsgReqDrvIn.Sz*2 + 5;  // ������� �� ����� ������
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
    reqMsgReqDrvIn.countByteRead = reqMsgReqDrvIn.Sz*2 + 5;  // ������� �� ����� ������
#endif
    reqMsgReqDrvIn.AdrData  = ADDR_TEXN;
    drvErr = ReQuestDrv(QueueDrvIn, &reqMsgReqDrvIn);

    if( drvErr == Ok_err ){
        AllPrmPnt->PASSW_TEXN = swapU16(*(u16 *)&Buf[0]);
    }

#ifndef _PROJECT_IAR_ 
    reqMsgReqDrvIn.countByteRead = reqMsgReqDrvIn.Sz*2 + 5;  // ������� �� ����� ������
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
    reqMsgReqDrvIn.countByteRead = reqMsgReqDrvIn.Sz*2 + 5;  // ������� �� ����� ������
#endif
    reqMsgReqDrvIn.AdrData  = ADDR_MASTER;
    drvErr = ReQuestDrv(QueueDrvIn, &reqMsgReqDrvIn);

    if( drvErr == Ok_err ){
        AllPrmPnt->PASSW_MASTER = swapU16(*(u16 *)&Buf[0]);
    }

    //#warning � ����� ����� ������ �������� �����? � ���� ������� ����, �� ��� ����???
    if (Err) {
      return 1;  
      //MessageCancel(GetStringMess(MessageMess_ERROR_CONECT));
    }
    return 0;
}
/*********************************************
 * ������������ ������� �� ���������         *
 *  =>*                                      *
 *                                           *
 *                                           *
 * *=>                                       *
 *********************************************/
Drv_err_type ReqStatus(Full_DescriptorPrm_type *DPDrive)
{
    DPDrive->msgReqDrvIn.AdrNet   = DPDrive->AdrNet;
    DPDrive->msgReqDrvIn.AdrData  = ADDR_STATUS;//DPDrive->wm.AdrPrm;

    // WARNING + ������� �� �������
    if (DPDrive->SzReq % 2 == 0)    DPDrive->msgReqDrvIn.Sz = DPDrive->SzReq >> 1;
    else                            DPDrive->msgReqDrvIn.Sz = (DPDrive->SzReq >> 1) + 1;

    DPDrive->msgReqDrvIn.Func     = READ_H_REGS;
    DPDrive->msgReqDrvIn.Buf      = DPDrive->BufParam;
    DPDrive->msgReqDrvIn.SzBuf    = sizeof(DPDrive->BufParam);
    DPDrive->msgReqDrvIn.NumParam = DPDrive->NumParam;

#ifndef _PROJECT_IAR_ 
    DPDrive->msgReqDrvIn.countByteRead = DPDrive->msgReqDrvIn.Sz*2 + 5;  // ������� �� ����� ������
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
/////            sprintf((char *)&Buf[0], "�������� %s ���/�, ����� %d", "230400"/*BdrtName[Speed]*/, Adr);
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
 * ������� �������         *
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
        WaitMsgFromDrv(&MsgDrv);             //�������� ������� �������� ���������
        OSQQuery(QueueDrvIn, &QData);           //����������� ���������� ������� ���������
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
 * ������� ��������� �������� ��� �������    *
 * �������                                   *
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
    OSTaskCreate(TaskDRV, (void *)0, &DRVStack[STKDRVSIZE - 1], 4);  // �������� ������ DRV*/

    return Ok_err;
}
/*********************************************
 * ������� ��������� �������� ��� �������    *
 * �������                                   *
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
 * �������� ������ �� �����                  *
 *  =>*                                      *
 *                                           *
 *                                           *
 * *=>                                       *
 *********************************************/
/*********************************************
 * ������������� ������� �� ������� �������� *
 *  =>* AdrNet - ������� ����� �������       *
 *      AdrData - ����� ������               *
 *      Sz - ������ ������������� ������     *
 * *=>  TypeReq - ��� �������                *
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
