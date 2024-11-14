
//

#include "crc.h"
#include "filemp.h"
#include "TskDrv.h"
#include "menu.h"

#include <stdlib.h>

//#define UST_IN_AT45 // Разрешает сохранять уставки через память AT45
#define UST_IN_FM25  // Разрешает сохранять уставки через память FM25

extern uint8_t gIsNvsaOld;

//
void DataRotate(u8 *data, s32 size)
{
    if(size%2)
    {
        return;
    }
    
    for(int i = 0; i < size - 1; i += 2)
    {
        u8 buf = data[i];
        data[i] = data[i + 1];
        data[i + 1] = buf;
    }
}

//
void ShowFileMPProgress(u16 Per, u16 FileMPNum, u16 Mode)
{
    GrafBffClear();

    //DrawRectangle(3, 3, 156, 156);
    ProgressBar(40, 90, 80, Per);
    
    if(Mode == FILE_MP_STORE_MODE)
    {
        //sprintf(&StrOut[0], "%s %d", "Сохр. уст. ", FileMPNum);
      sprintf(&StrOut[0], "%s%d", GetStringMess(MessageMess_READ_SET), FileMPNum);
    }
    else
    {
        //sprintf(&StrOut[0], "%s %d", "Восст. уст. ", FileMPNum);
      sprintf(&StrOut[0], "%s%d", GetStringMess(MessageMess_WRITE_SET), FileMPNum);
    }
    
    StrOutFont(0, 50, NO_INV, &StrOut[0], &fontNormal, leftAlign);
    
    GrfOut();
}

//
u8 FillInCrcs(FileMPLink *Link)
{
    Drv_err_type Err = Ok_err;
    FileFunctional_type FunctDrv;
    HeadOfFile_type HOFDrv;
    HeadOfFileOld_type HOFDrvOld;
    u8 NetAddr = (u8)AllPrmPnt->ADD_DRIVE_ADR;
    u16 Tmp = 0;
    
    if (gIsNvsaOld)
    {
        // Read FOP header from AT24
        Err = ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], NetAddr, 0, sizeof(HOFDrvOld),
                              fileDescrPrm, &HOFDrvOld, sizeof(HOFDrvOld));
        
        // Read AT24 function
        Err |= ReadVerFunct(&MsgReqDrvIn[MSG_ID], NetAddr,
                            fileFunctional, &FunctDrv);
            
        // Check
        if(Err != Ok_err)
        {
            return 0; // Error
        }
        
        // Check crc
        Link->FileCrc = GetCrc(&HOFDrvOld, sizeof(HOFDrvOld) - sizeof(HOFDrvOld.HeaderCrc));
        if (Link->FileCrc != HOFDrvOld.HeaderCrc)
        {
           return 0; // Error
        }
        Link->FileCrc = HOFDrvOld.FileCrc;
        
        Tmp = swapU16(FunctDrv.curFunct.functMask);
        Link->FuncMaskCrc = GetCrc(&Tmp, sizeof(FunctDrv) - sizeof(FunctDrv.crc));
        if (Link->FuncMaskCrc != FunctDrv.crc)
        {
            return 0; // Error
        }
    }
    else
    {
         // Read FOP header from AT24
        Err = ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], NetAddr, 0, sizeof(HOFDrv),
                              fileDescrPrm, &HOFDrv, sizeof(HOFDrv));
        
        // Read AT24 function
        Err |= ReadVerFunct(&MsgReqDrvIn[MSG_ID], NetAddr,
                            fileFunctional, &FunctDrv);
            
        // Check
        if(Err != Ok_err)
        {
            return 0; // Error
        }
        
        // Check crc
        Link->FileCrc = GetCrc(&HOFDrv, sizeof(HOFDrv) - sizeof(HOFDrv.HeaderCrc));
        if (Link->FileCrc != HOFDrv.HeaderCrc)
        {
           return 0; // Error
        }
        Link->FileCrc = HOFDrv.FileCrc;
        
        Tmp = swapU16(FunctDrv.curFunct.functMask);
        Link->FuncMaskCrc = GetCrc(&Tmp, sizeof(FunctDrv) - sizeof(FunctDrv.crc));
        if (Link->FuncMaskCrc != FunctDrv.crc)
        {
            return 0; // Error
        }     
    }
    
    return 0xff; // Success!!! 
}

//
u8 WriteFileMPLink(u8 SlotNum, u8 IsFull)
{
    FileMPLink Link;
    
    // Set starting address of the slot
    Link.FlashAddr = (void*)(FILE_MP_FLASH_OFFSET +
                     SlotNum * FILE_MP_FLASH_BYTES);
    
    Link.IsFullSign = 0; // Slot is empty
    if(IsFull)
    {
        Link.IsFullSign = FILEMP_LINK_FULL_SIGN; // Slot is full
        if(!FillInCrcs(&Link))
        {
            Link.IsFullSign = 0;
        }
    }
    
    // Calculate link crc
    Link.LinkCrc = GetCrc(&Link, sizeof(Link) - 2);
    
    // Save link to FRAM
    FM25WR(&nv.FileMPLinks[SlotNum], &Link, sizeof(Link));
    
    return 0; // Success
}
//*****************************************************************************

//*****************************************************************************
FileMPLink ReadFileMPLink(u8 SlotNum)
{
    FileMPLink Link;
    u16 Crc = 0;
    
    // Read link from FRAM
    FM25RD(&Link, 
           &nv.FileMPLinks[SlotNum],
           sizeof(Link));
    
    // Check crc
    Crc = GetCrc(&Link, sizeof(Link) - 2);
    if(Crc != Link.LinkCrc)
    {
        Link.IsFullSign = 0;
    }
    
    return Link;
}
//*****************************************************************************

//*****************************************************************************
u8 SaveFileMPFromAT24(u8 FileMPNum, 
                      u8 SlotNum)
{
    Drv_err_type Err = Ok_err;
    u8 NetAddr = 0;
    u16 Crc = 0xffff, OriginalCrc = 0;
    u16 NumBytesLeft = 0, NumBytesToRead = 0;
    void *pFileAddr = 0, *pFlashAddr = 0;
    s32 i = 0;
    u16 NumBytes = 0;
    
    u8 FileData[FILE_MP_READ_BYTES] = {0};
    u8 HeadData[FILE_MP_HEAD_BYTES] = {0};
    u16 buf = 0;
    
    NetAddr = (u8)AllPrmPnt->ADD_DRIVE_ADR;
    
    // Read specified file header from AT24
    Err = ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], NetAddr,
                          0, FILE_MP_HEAD_BYTES, FileMPNum,
                          &HeadData[0], FILE_MP_HEAD_BYTES);
    DataRotate(&HeadData[0], FILE_MP_HEAD_BYTES);
    
    // Проверяем входную сигнатуру файла от привода АТ24
    if((HeadData[2] | HeadData[3] << 8) != FILE_MP_SIGN)
    {   // Если в файле привода глюк, его нельзя брать.
        MessagePopUp(GetStringMess(MessageMess_ERROR_COMPATIBILITY));
        return 1; // Error - signature test is not passed
    }
    
    // Calculate checksum - we ignore CRC and setting size in WORDS
    Crc = CrcPortion((u16*)&HeadData[2], (FILE_MP_HEAD_BYTES - 2) / 2, Crc);
    OriginalCrc = (HeadData[0] | HeadData[1] << 8);       // Get original CRC
    
    // Set starting addresses
    pFileAddr = (void*)(FILE_MP_HEAD_BYTES / 2);    // In WORDS
    
    // Делам выбор, где идет размещение файла данных - Если в AT45, задаем адрес в структуре spfl
    #ifdef UST_IN_AT45
    pFlashAddr = (void*)(&spfl.FileSettingsDrv[SlotNum]);    // Адрес в памяти AT45 в байтах указывает на FileSettingsDrv[SlotNum] - в структуре spfl
    #endif
    
    // Делам выбор, где идет размещение файла данных - Если в FM25, задаем адрес в структуре nv
    #ifdef UST_IN_FM25
    pFlashAddr = (void*)(&nv.FileSettingsDrv[SlotNum]);    // Адрес в памяти FM45 в байтах указывает на FileSettingsDrv[SlotNum] - в структуре nv
    #endif
    
    // Сохраням заголовок файла
    HeadData[2]--; // Но при этом искажаем сигнатуру файла до завершения загрузки файла.
    #ifdef UST_IN_AT45
    AT45WR(pFlashAddr, &HeadData[0], FILE_MP_HEAD_BYTES);   //  Сохраняем заголовок в AT45
    #endif
    
    #ifdef UST_IN_FM25
    FM25WR(pFlashAddr, &HeadData[0], FILE_MP_HEAD_BYTES);   //  Сохраняем заголовок в FM25
    #endif
    
    pFlashAddr = (void*)((u32)pFlashAddr + FILE_MP_HEAD_BYTES);
    
    // Try to read file body from AT24 & save to flash
    NumBytesLeft = (HeadData[4] | HeadData[5] << 8) * 4;    // 1 UST = 4 BYTES
    NumBytes = NumBytesLeft;
    i = 0;
    
    while(Err == Ok_err && NumBytesLeft)
    {
        // Calculate number of bytes we need to read
        if(NumBytesLeft > FILE_MP_READ_BYTES)
        {
            NumBytesToRead = FILE_MP_READ_BYTES;
            NumBytesLeft -= FILE_MP_READ_BYTES;
        }
        else
        {
            NumBytesToRead = NumBytesLeft;
            NumBytesLeft = 0;
        }
        
        Err = ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], NetAddr,
                              pFileAddr, NumBytesToRead, FileMPNum,
                              &FileData[0], FILE_MP_READ_BYTES);    // Size - in BYTES
        DataRotate(&FileData[0], FILE_MP_READ_BYTES);
        
        if(Err != Ok_err && i < NUM_ATTEMPTS)
        {
            Err = ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], NetAddr,
                                  0, FILE_MP_HEAD_BYTES, FileMPNum,
                                  &FileData[0], FILE_MP_READ_BYTES);
            
            Err = Ok_err;
            NumBytesLeft += NumBytesToRead;
            i++;
        }
        else
        {
            // Save data to flash
            #ifdef UST_IN_AT45
            AT45WR(pFlashAddr, &FileData[0], NumBytesToRead); // Size - in BYTES - Данные сохраняются в AT45
            #endif
            
            #ifdef UST_IN_FM25
            FM25WR(pFlashAddr, &FileData[0], NumBytesToRead); // Size - in BYTES - Данные сохраняются в FM25
            #endif
            
            // Calculate checksum
            Crc = CrcPortion((u16*)&FileData[0], NumBytesToRead / 2, Crc);    // Size - in WORDS
            
            // Increase pointers
            pFileAddr = (void*)((u32)pFileAddr + NumBytesToRead / 2);   // Size - in WORDS
            pFlashAddr = (void*)((u32)pFlashAddr + NumBytesToRead);     // Size - in BYTES
            
            i = 0;
            
            ShowFileMPProgress((u16)(100.0 - ((f32)NumBytesLeft / (f32)NumBytes) * 100.0),
                               (FileMPNum - (u8)fileMicroProg1 + 1),
                               FILE_MP_STORE_MODE);
        }
    }      
    
    // Check results & create link
    if(Err == Ok_err && OriginalCrc == Crc)
    {
        // После того как завершена загрузка файла, восстанавливаем сигнатуру и записываем в память
        buf = FILE_MP_SIGN;      
        
        #ifdef UST_IN_AT45
        pFlashAddr = (void*)((u32)&spfl.FileSettingsDrv[SlotNum] + 2);// Адрес сигнатуры сдвинут на два байта от начала. Первым идет CRC
        AT45WR(pFlashAddr, &buf, sizeof(buf));
        #endif
      
        #ifdef UST_IN_FM25
        pFlashAddr = (void*)((u32)&nv.FileSettingsDrv[SlotNum] + 2);// Адрес сигнатуры сдвинут на два байта от начала. Первым идет CRC
        FM25WR(pFlashAddr, &buf, sizeof(buf)); // Запись в FM25
        #endif
    
        // Create link
        WriteFileMPLink(SlotNum, 0xff);        
        
        return 0; // Allright
    }
    
    WriteFileMPLink(SlotNum, 0);   // Empty link
    MessagePopUp(GetStringMess(MessageMess_ERROR_READ));
    
    return 2; // There was an error during reading
}

extern HandlerMenuPultNew_struct_type   HandlerMenuPultNew_struct;
extern HandlerMenuChangeDrive_struct_type HandlerMenuChangeDrive_struct;
//
u8 SaveFileMPToAT24(u8 SlotNum,
                    u8 FileMPNum)
{
    Drv_err_type Err = Ok_err;
    u8 NetAddr = 0;
    u16 Crc = 0xffff, OriginalCrc = 0, tmp = 0;
    u16 NumBytesLeft = 0, NumBytesToWrite = 0;
    void *pFileAddr = 0, *pFlashAddr = 0;
    u8 *pBuf = 0;
    s32 i = 0;
    u16 NumBytes = 0;
    u32  TotalError = 0;
    u32  TotalPing = 0;    
        
    // Отключение разрешения прерываний таймера ШИМа управления подсветкой.
    // Отключение происхоит на момент передачи данных. А потом восстанавливается.
    // Таким образом снижаем энергопотребление пульта и передача происходит стабильно отлично.
    TIM3->CCER &= (uint16_t)~TIM_CCER_CC3E;
    
    //int j;
    
    u8 FileData[FILE_MP_WRITE_BYTES] = {0};
    u8 HeadData[FILE_MP_HEAD_BYTES] = {0};
    u8 TempBuf[FILE_MP_HEAD_BYTES] = {0};
    u16 buf = FILE_MP_SIGN;
    
    NetAddr = (u8)AllPrmPnt->ADD_DRIVE_ADR;
    
    // Get starting address & Read file header

    #ifdef UST_IN_AT45  // Выбираем из какой памяти берем файл данных 
    // Read file header
    pFlashAddr = (void*)(&spfl.FileSettingsDrv[SlotNum]);    // Адрес в памяти AT45 в байтах указывает на FileSettingsDrv[SlotNum] - в структуре spfl
    AT45RD(&HeadData[0], pFlashAddr, FILE_MP_HEAD_BYTES);    
    #endif
    
    #ifdef UST_IN_FM25
    // Read file header
    pFlashAddr = (void*)(&nv.FileSettingsDrv[SlotNum]);    // Адрес в памяти FM25 в байтах указывает на FileSettingsDrv[SlotNum] - в структуре nv
    FM25RD(&HeadData[0], pFlashAddr, FILE_MP_HEAD_BYTES); // Чтение из FM25
    #endif

    pFlashAddr = (void*)((u32)pFlashAddr + FILE_MP_HEAD_BYTES);   // Size - in BYTES
    
    // Check signature
    if((HeadData[2] | HeadData[3] << 8) != FILE_MP_SIGN)
    {
        MessagePopUp(GetStringMess(MessageMess_ERROR_COMPATIBILITY));
        return 1; // Error - signature test is not passed
    }
    
    // Calculate checksum - we ignore CRC and setting size in WORDS
    OriginalCrc = (HeadData[0] | HeadData[1] << 8);       // Get original CRC
    Crc = CrcPortion((u16*)&HeadData[2], (FILE_MP_HEAD_BYTES - 2) / 2, Crc);
    
    // Save header to file on AT24
    pFileAddr = (void*)0;
    HeadData[2]--;       // Screw up signature
    pBuf = &HeadData[0];
    NumBytesLeft = FILE_MP_HEAD_BYTES;
    DataRotate(&HeadData[0], FILE_MP_HEAD_BYTES);
    i = 0;
    
    while(Err == Ok_err && NumBytesLeft)
    {
      ResetWD(tsk_lcd);
        // Calculate number of bytes we need to write
          if(NumBytesLeft > FILE_MP_WRITE_BYTES)
        {
            NumBytesToWrite = FILE_MP_WRITE_BYTES;
            NumBytesLeft -= FILE_MP_WRITE_BYTES;
        }
        else
        {
            NumBytesToWrite = NumBytesLeft;
            NumBytesLeft = 0;
        }
        
        //
//        OSTimeDly(5); // Были обнаружены глюки при записи данных, поэтому для нормализации работы канал добавлены паузы в посылках
        Err = WriteFileToDrv(&MsgReqDrvIn[MSG_ID], NetAddr,
                             pFileAddr, NumBytesToWrite, FileMPNum,
                             pBuf, FILE_MP_HEAD_BYTES);                   // Size - in BYTES
        TotalPing++;
        
        if(Err != Ok_err && i < NUM_ATTEMPTS)
        {
            Err = ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], NetAddr,
                                  0, FILE_MP_HEAD_BYTES, FileMPNum,
                                  &TempBuf[0], FILE_MP_HEAD_BYTES);
            
            Err = Ok_err;
            NumBytesLeft += NumBytesToWrite;
            i++;
            TotalError ++;
            OSTimeDly(25); // Были обнаружены глюки при записи данных, поэтому для нормализации работы канал добавлены паузы в посылках
        }
        else
        {
            pFileAddr = (void*)((u32)pFileAddr + NumBytesToWrite / 2);   // Size - in WORDS
            pBuf += NumBytesToWrite;                                     // Size - in BYTES
            
            i = 0;
        }
    }
    
    if(Err != Ok_err)
    {
        MessagePopUp(GetStringMess(MessageMess_ERROR_CONECT));
        return 4; // Error while writing 
    }
    
    // Read & save file contents
    pFileAddr = (void*)(FILE_MP_HEAD_BYTES / 2);          // Size - in WORDS (Missing header)
    NumBytesLeft = (HeadData[5] | HeadData[4] << 8) * 4;  // 1 UST = 4 BYTES (we rotated data before);
    NumBytes = NumBytesLeft;
    i = 0;
    
    while(Err == Ok_err && NumBytesLeft)
    {
     ResetWD(tsk_lcd);
        // Calculate number of bytes we need to write
        if(NumBytesLeft > FILE_MP_WRITE_BYTES)
        {
            NumBytesToWrite = FILE_MP_WRITE_BYTES;
            NumBytesLeft -= FILE_MP_WRITE_BYTES;
        }
        else
        {
            NumBytesToWrite = NumBytesLeft;
            NumBytesLeft = 0;
        }
        
        // Read data from external memory 
        #ifdef UST_IN_AT45
        AT45RD(&FileData[0], pFlashAddr, NumBytesToWrite); // Size - in BYTES 
        #endif
        
        #ifdef UST_IN_FM25
        FM25RD(&FileData[0], pFlashAddr, NumBytesToWrite); // Size - in BYTES - Чтение из FM25
        #endif
                
        // Compute CRC
        tmp = Crc;
        Crc = CrcPortion((u16*)&FileData[0], NumBytesToWrite / 2, Crc);       // Size - in WORDS
        DataRotate(&FileData[0], NumBytesToWrite);                  // Size - in BYTES

//        OSTimeDly(5); // Были обнаружены глюки при записи данных, поэтому для нормализации работы канал добавлены паузы в посылках
        //ResetWD(tsk_lcd);
        // Save to AT24
        Err = WriteFileToDrv(&MsgReqDrvIn[MSG_ID], NetAddr,
                             pFileAddr, NumBytesToWrite, FileMPNum,
                             &FileData[0], FILE_MP_WRITE_BYTES);        // Size - in BYTES
        TotalPing++;
        
        
        if(Err != Ok_err && i < NUM_ATTEMPTS)
        {
            Err = ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], NetAddr,
                                   0, FILE_MP_HEAD_BYTES, FileMPNum,
                                   &TempBuf[0], FILE_MP_HEAD_BYTES);
            
            Err = Ok_err;
            Crc = tmp;
            NumBytesLeft += NumBytesToWrite;
            i++;
            TotalError++;
            OSTimeDly(25); // Были обнаружены глюки при записи данных, поэтому для нормализации работы канал добавлены паузы в посылках
        }
        else
        {
            pFileAddr = (void*)((u32)pFileAddr + NumBytesToWrite / 2);     // Size - in WORDS
            pFlashAddr = (void*)((u32)pFlashAddr + NumBytesToWrite);       // Size - in BYTES
            
            i = 0;            
            ShowFileMPProgress((u16)(100.0 - ((f32)NumBytesLeft / (f32)NumBytes) * 100.0),
                               (FileMPNum - (u8)fileMicroProg1 + 1),
                               FILE_MP_LOAD_MODE);
//            OSTimeDly(25); // Были обнаружены глюки при записи данных, поэтому для нормализации работы канал добавлены паузы в посылках
        }
    }


    // Check CRC
    if(Err == Ok_err && OriginalCrc == Crc)
    { 
        // Restore signature
        pFileAddr = (void*)1;     // First 2 bytes are CRC (we use WORDS)
        buf = FILE_MP_SIGN << 8;  // Rotate
        
        i = 0;
        while(i < NUM_ATTEMPTS)
        {
          //ResetWD(tsk_lcd);
            Err = WriteFileToDrv(&MsgReqDrvIn[MSG_ID], NetAddr,
                                 pFileAddr, sizeof(buf), FileMPNum,
                                 &buf, sizeof(buf));
        
            if(Err != Ok_err)
            {
                ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], NetAddr,
                                0, FILE_MP_HEAD_BYTES, FileMPNum,
                                &TempBuf[0], FILE_MP_HEAD_BYTES);
            }
            else
            {
                Err = Ok_err;
                break;
            }
           
            i++;
        }

        if(Err != Ok_err)
        {
            // Восстановление разрешения на прерывания. Подстветка включается.
            TIM3->CCER |= (uint16_t)TIM_CCER_CC3E;
            MessagePopUp(GetStringMess(MessageMess_ERROR_CONECT));
            return 3; // Error while writing 
        }
        
        // Восстановление разрешения на прерывания. Подстветка включается.
        TIM3->CCER |= (uint16_t)TIM_CCER_CC3E;
        return 0; // Allright
    }
    
    // Восстановление разрешения на прерывания. Подстветка включается.
    TIM3->CCER |= (uint16_t)TIM_CCER_CC3E;
    
    MessagePopUp(GetStringMess(MessageMess_ERROR_WRITE));
    return 2; // Error while writing
}

//
u8 CanSave(u8 SlotNum)
{
    FileMPLink Link;
    
    // Read link
    Link = ReadFileMPLink(SlotNum);
    
    if(Link.IsFullSign == FILEMP_LINK_FULL_SIGN)
    {
        if(!MessageYesNo(GetStringMess(MessageMess_FILE_FULL)))
        {
            return 0; // No, we can not write
        }
    }
    
    return 0xff; // Yes we can
}

//
void ZeroMPFilesUst()
{
    // Set values back to NO
    SendS16Prm(SAVE_MPFILE1_GRPPRM, 0);
    SendS16Prm(SAVE_MPFILE2_GRPPRM, 0);
    SendS16Prm(RESTORE_MPFILE1_GRPPRM, 0);
    SendS16Prm(RESTORE_MPFILE2_GRPPRM, 0);
}

//
u8 CanRestore(u8 SlotNum)
{
    FileMPLink Link, LinkCrcs;
    
    // Read drive crcs from AT24
    FillInCrcs(&LinkCrcs);
    
    // Read link
    Link = ReadFileMPLink(SlotNum);
    
    if(Link.IsFullSign != FILEMP_LINK_FULL_SIGN)
    {
        MessagePopUp(GetStringMess(MessageMess_FILE_EMPTY));
        return 0; 
    }
    
    if(LinkCrcs.FileCrc != Link.FileCrc ||
       LinkCrcs.FuncMaskCrc != Link.FuncMaskCrc)
    {
        MessagePopUp(GetStringMess(MessageMess_ERROR_COMPATIBILITY));
        return 0; // No we can not (fuctions differ)
    }
    
    return 0xff; // Yes we can
}

//
void UpdateMPFiles(u16 GrpPrm)
{
    u16 Slot = 0;
#warning ОПИСАНИЕ ОБНОВЛЕННОЙ ПРОЦЕДУРЫ ПО РАБОТЕ С ЗАВОДСКИМИ УСТАВКАМИ И ИХ ПЕРЕНОСОМ.
    /*  
    1. При запросе через параметр "Набор параметров" система делает копию уставок к себе в память.
    2. Далее через пульт я в параметре "Сохр.наб в пульт" делаю запрос и копирую данные в указанный слот. При этом я должен установить в данном параметре номер слота
    и в параметре "Восст. наб." одинаковые значения слотов. И еще сохранить в памяти пульта параметр "Восст. наб.". 
    3. При подключении к новому ПЧ, пульт автоматич. восстанавливает параметр "Восст. наб." в параметрах ПЧ и далее пульт уже будет показывать свой актуиальный
    статус по сохраненым значениям по слотам памяти.
      
    А вообще это надо перетаскивать чисто в работу ПУЛЬТА, без внешних параметров.
    */
    switch(GrpPrm)
    {
    case SAVE_MPFILE1_GRPPRM:
        Slot = ReqS16Prm(SAVE_MPFILE1_GRPPRM);
        
        if(Slot && CanSave(Slot - 1))
        {
            SaveFileMPFromAT24(fileMicroProg1, Slot - 1);   
        }
        SendS16Prm(SAVE_MPFILE1_GRPPRM, Slot);
        break;
        
    case SAVE_MPFILE2_GRPPRM:
        Slot = ReqS16Prm(SAVE_MPFILE2_GRPPRM);
        
        if(Slot && CanSave(Slot - 1))
        {
            SaveFileMPFromAT24(fileMicroProg2, Slot - 1);
        }
        SendS16Prm(SAVE_MPFILE2_GRPPRM, Slot);
        break;
        
    case RESTORE_MPFILE1_GRPPRM:
        Slot = ReqS16Prm(RESTORE_MPFILE1_GRPPRM);
        
        if(Slot && CanRestore(Slot - 1))
        {
            SaveFileMPToAT24(Slot - 1, fileMicroProg1);
        }
        SendS16Prm(RESTORE_MPFILE1_GRPPRM, 0);
        break;
        
    case RESTORE_MPFILE2_GRPPRM:
        Slot = ReqS16Prm(RESTORE_MPFILE2_GRPPRM);
        
        if(Slot && CanRestore(Slot - 1))
        {
            SaveFileMPToAT24(Slot - 1, fileMicroProg2);
        }
        SendS16Prm(RESTORE_MPFILE2_GRPPRM, 0);
        break;    
    }
    
    
}