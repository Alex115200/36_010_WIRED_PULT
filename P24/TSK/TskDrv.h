#ifndef TSKDRV_H
#define TSKDRV_H

#include "fixedUst.h"

//===================================================================
//                      Ошибки от драйвера
//===================================================================

#define ERR_NONE_MB     0   //Ошибок нет
#define ERR_TYPE_MB     1   //Принят не тот тип, маловероятно
#define ERR_KS_MB       2   //Не совпала КС
#define ERR_OTKAZ_MB    3   //Отказ


#define MSG_STARTSTOP   0
#define MSG_ID          1
#define MSG_PARAM       2
#define MSG_FDP         3

#define SzQueue         8

//===================================================================

#define COIL_ON     0xff
#define COIL_OFF    0x00

//===================================================================
// Номера файлов в ПЧ
enum{
    fileDescrPrm        = 1,
    
    fileMicroProgFact   = 4,
    fileMicroProg1      = 5,
    fileMicroProg2      = 6,
    fileLoad            = 8,   
    
    fileFunctional      = 18,
    fileLogger          = 22,
    FileKolvUst         = 16,
    FileBase            = 14,
    FileSignatura       = 17,
};

// Слово состояния наличия файлов в системе контролируемого устройства
// Индексы битов соответсвует выше указанному enum. 
// Например, статус файла функционала находится на 18 позиции: 1 << fileFunctional;
// Если установлена 1 - значит файла нет в системе
extern u32  MissingFileWord;
#define   FULL_FUNCTIONAL_MASK    0x7FFF
//===================================================================

extern MsgReqDrvIn_type    MsgReqDrvIn[SzQueue];
extern MsgReqKbdIn_type    MsgReqKbdIn[SzQueue];
//extern MsgReqDrvOut_type   MsgReqDrvOut[SzQueue];
extern void                *pntMsgReqDrvIn[SzQueue];
extern void                *pntMsgReqDrvOut[SzQueue];
extern void                *pntMsgReqDrvOutStartStop[SzQueue];
extern void                *pntMsgReqDrvInStartStop[SzQueue];

//extern const char *const msgError;
extern u16 TypeLanguage;
//===================================================================
//===================================================================
typedef struct{
        u8 Adr  ;
        u8 Speed;
}NetPrm_type;
//===================================================================
//                      Прототипы
//===================================================================
__noreturn void TaskDRV(void *pdata);

Drv_err_type SendMsgToTask( MsgReqDrvIn_type *MsgIn, u8 *pBuf, u16 size );
//Drv_err_type RequestFDP(MsgReqDrvIn_type *MsgIn, u8 AdrNet, void *Adr, u8 SzReq, void *Buf, u16 SzBuf);
Drv_err_type Request0x2B(MsgReqDrvIn_type *MsgIn, u8 AdrNet, void *Buf, u16 SzBuf);
Drv_err_type RequestModbusOpisPrmExt(Full_DescriptorPrm_type *DPDrive);
Drv_err_type RequestModbusOpisPrm(Full_DescriptorPrm_type *DPDrive);
Drv_err_type WritePrm(Full_DescriptorPrm_type *DPDrive);

u8 ReqEtalonPassword(void);

Drv_err_type StartStopDrive( u8 AdrNet, u16 AdrCoil, u8 onOff );
//Drv_err_type ReqStatus(Full_DescriptorPrm_type *DPDrive);
Drv_err_type ScanNet(NetPrm_type *NetPrm);
Drv_err_type ReadFileFromDrv(MsgReqDrvIn_type *MsgIn, u8 AdrNet, void *Adr, u8 SzReq, u8 NumFile, void *Buf, u16 SzBuf);
Drv_err_type WriteFileToDrv(MsgReqDrvIn_type *MsgIn, u8 AdrNet, void *Adr, u8 SzReq, u8 NumFile, void *Buf, u16 SzBuf);
Drv_err_type ReadJrnIndex(MsgReqDrvIn_type *MsgIn, u8 AdrNet, u8 NumFile, Index_type *indexJrn);
Drv_err_type ReadVerFunct(MsgReqDrvIn_type *MsgIn, u8 AdrNet, u8 NumFile, FileFunctional_type *pFunct);
Drv_err_type SendMsgClearQueueDrv(void);
Drv_err_type ReQuestDrv(OS_EVENT *QueueMsg, MsgReqDrvIn_type *MsgIn);
//Drv_err_type ReadJrn(MsgReqDrvIn_type *MsgIn, u8 AdrNet, void *AdrInFile, u8 NumFile, RecJrn_type *RecJrn);
Drv_err_type ReadTekAllPrmFromDrv(MsgReqDrvIn_type *MsgIn, u32 AdrData, void *Buf, u16 SzData);
#ifdef TEST_P24
    char WaitRx(void);
    char WaitRxWoPend(void);
    void testSendString(void);
#endif // TEST_P24
//===================================================================
    
s32 ReqS16Prm(s16 GrpPrm);
s32 SendS16Prm(s16 GrpPrm, s16 Val);
//====================================================================
//  Формирование запросов на чтение типа языка
//====================================================================
u8 ReqLanguage(void);

#endif  // TSKDRV_H
