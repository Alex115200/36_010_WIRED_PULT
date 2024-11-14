
#include "globalIncludes.h"

//===================================================================
unsigned char DataBuf[256];        // Буфер уарта
//===================================================================
// функция формирует запросы в соответствии с протоколом MODBUS
//===================================================================
u8 ModbusFunc(MsgReqDrvIn_type *mfMsgReqDrvIn, void *Buf )
{
    u8      *ptr;
    u8      Sz;
    u16     crcVal;

    ptr = Buf;

    switch (mfMsgReqDrvIn->Func){
        case READ_H_REGS:   // 0x03 0x04
            *ptr++ = mfMsgReqDrvIn->AdrNet;
            *ptr++ = 0x03;
            *ptr++ = (u8)(mfMsgReqDrvIn->AdrData >> 8);
            *ptr++ = (u8)(mfMsgReqDrvIn->AdrData & 0xFF);
            *ptr++ = 0;
            *ptr++ = mfMsgReqDrvIn->Sz;
            break;

        case READ_DEV_ID:   // 0x2b
            *ptr++ = mfMsgReqDrvIn->AdrNet;
            *ptr++ = 0x2b;
            *ptr++ = 0x0e;
            *ptr++ = 0x01;
            *ptr++ = 0x00;
            break;

        case READ_FILE:    // 0x68
            *ptr++ = mfMsgReqDrvIn->AdrNet;
            *ptr++ = 0x68;                                  //Чтение файла
            *ptr++ = (u8) mfMsgReqDrvIn->Param;               //Номер файла
            *ptr++ = (u8)(mfMsgReqDrvIn->AdrData >> 24);      // WARNING     //Стартовый адрес относительно начала
            *ptr++ = (u8)(mfMsgReqDrvIn->AdrData >> 16);      //Стартовый адрес относительно начала
            *ptr++ = (u8)(mfMsgReqDrvIn->AdrData >> 8);       //Стартовый адрес относительно начала
            *ptr++ = (u8)mfMsgReqDrvIn->AdrData;              //Стартовый адрес относительно начала
            *ptr++ = mfMsgReqDrvIn->Sz;
            break;

        case WRITE_FILE:   // 0x69
            Sz = mfMsgReqDrvIn->Sz * 2;
            *ptr++ = mfMsgReqDrvIn->AdrNet;
            *ptr++ = 0x69;                                  // Запмсь файла
            *ptr++ = (u8)mfMsgReqDrvIn->Param;                // Номер файла
            *ptr++ = (u8)(mfMsgReqDrvIn->AdrData >> 24);      // WARNING   //Стартовый адрес относительно начала
            *ptr++ = (u8)(mfMsgReqDrvIn->AdrData >> 16);      // Стартовый адрес относительно начала
            *ptr++ = (u8)(mfMsgReqDrvIn->AdrData >> 8);       // Стартовый адрес относительно начала
            *ptr++ = (u8)mfMsgReqDrvIn->AdrData;              // Стартовый адрес относительно начала
            *ptr++ = mfMsgReqDrvIn->Sz;
            memcpy(ptr, mfMsgReqDrvIn->Buf, Sz);
            ptr =  (void *)((u32)ptr + Sz);
            break;

        case WRITE_REGS:    // 0x10
            Sz = mfMsgReqDrvIn->Sz * 2;
            *ptr++ = mfMsgReqDrvIn->AdrNet;
            *ptr++ = 0x10;
            *ptr++ = (u8)(mfMsgReqDrvIn->AdrData >> 8);
            *ptr++ = (u8)mfMsgReqDrvIn->AdrData;
            *ptr++ = 0;         //(u8)(mfMsgReqDrvIn->Sz >> 8);     // WARNING
            *ptr++ = (u8)mfMsgReqDrvIn->Sz;
            *ptr++ = (u8)Sz;
            memcpy(ptr, mfMsgReqDrvIn->Buf, Sz);
            ptr =  (void *)((u32)ptr + Sz);
            break;

        case WRITE_COIL:    // 0x05
            Sz = 4;
            *ptr++ = mfMsgReqDrvIn->AdrNet;
            *ptr++ = WRITE_COIL;
            memcpy(ptr, mfMsgReqDrvIn->Buf, Sz);
            ptr =  (void *)((u32)ptr + Sz);
            break;
        default:
            return 0;
    }

    Sz = ptr - (u8*)Buf;

    crcVal = GetCrc( Buf, Sz );
    *ptr++ = crcVal & 0xFF;             // low CRC
    *ptr++ = (crcVal >> 8) & 0xFF;      // high CRC

    return Sz + 2;
}
//===================================================================
