
#include "filemp.h"

#include "Pult_Mem.h"
#include "Pult_Pult_Mem.h"
#include "fixedUst.h"
#include "TskDrv.h"
#include "uc1698.h"
#include "MenuStatDrv.h"
#include "Menu.h"

/**************************************************************************/
//#define  BLOCK_STM_MEMORY
/**************************************************************************/


extern AllPrm_Type *AllPrmPnt;
extern const u16 PultProgTablOne[];
extern uint8_t gIsNvsaOld;

extern HandlerMenuChangeDrive_struct_type HandlerMenuChangeDrive_struct;
extern HandlerMenuPultNew_struct_type   HandlerMenuPultNew_struct;

Full_DescriptorPrm_type     DPDrive;

//extern const char noValid[];
/*struct{
    LnkNetFDP_type      *LnkNetFDP;
    void                *AdrInFile;
    void                *CurAdrAT45;
    u32                 SzFDP;
    u32                 SzLast;
}InstallCurDrv_struct;*/

//Drv_err_type WriteFileToAt45FromDrv(u8 netAdr, u8 numFile, void *beginAdrInFile, u16 sz, void *beginAdrInAt45);
//Drv_err_type WriteFileToDrvFromAt45(u8 netAdr, u8 numFile, void *beginAdrInFile, u16 sz, void *beginAdrInAt45);
u16 CrcAT45(void *beginAdrInAt45, u16 sz);

void InstallCurDrv(void);

u32 LevelAccess = NO_PASSW;
u16 CurPassword = 0;
u16 PrevPassword = 0;
//u16 EtalonUser = 0;
//u16 EtalonTexn = 0;
//u16 EtalonMaster = 0;

//u8 str[100];

//==============================================================================
//void InstallDrv(void)
//==============================================================================
u8 ReadLnk(LnkNetFDP_type *pLnkNetFDP, u8 Num)
{
    u16 crc;
    u8 Tmp = 0;
    if (Num < MAXDRIVES_CUT){
        //FM25RD(pLnkNetFDP, &nv.LnkNetFDP[Num], sizeof(LnkNetFDP_type));
        FM25RD(pLnkNetFDP, &nv.OpisDrv[Num].LnkNetFDP, sizeof(LnkNetFDP_type));
        crc = GetCrc(pLnkNetFDP, sizeof(LnkNetFDP_type) - 2);
        if (crc == pLnkNetFDP->Crc) Tmp = 1;
    }
    return Tmp;
}
//===================================================================
//===================================================================
u8 WriteLnk(LnkNetFDP_type *pLnkNetFDP, u8 Num)
{
    u8 Tmp = 0;
    if (Num < MAXDRIVES_CUT){
        FM25WR(&nv.OpisDrv[Num].LnkNetFDP, pLnkNetFDP, sizeof(LnkNetFDP_type));
        Tmp = 1;
    }
    return Tmp;
}
//===================================================================
//===================================================================
void InstallDrvProgress(u16 procNet, u16 procInst, u16 cntFind, u16 cntInst, u16 netAdr, u16 mode)
{
    GrafBffClear();

    if (mode != 2) {
        /*ProgressBar(40, 40, 80, procNet);
        sprintf(&StrOut[0], "%s %d", "Адрес №", netAdr);
        StrOutFont(0, 20, NO_INV, &StrOut[0], &fontNormal, leftAlign );*/

        /*sprintf(&StrOut[0], "%s %d", "Найд. прив.:", cntFind);
        StrOutFont(0, 132, NO_INV, &StrOut[0], &fontNormal, leftAlign );
        sprintf(&StrOut[0], "%s %d", "Уст. прив.: ", cntInst);
        StrOutFont(0, 144, NO_INV, &StrOut[0], &fontNormal, leftAlign );
        //sprintf(&StrOut[0], "%s", "<Отм./?> - выход");
        //StrOutFont(0, 144, NO_INV, &StrOut[0], &fontNormal, leftAlign );*/
    }

    if (mode == 1 || mode == 2) {
        ProgressBar(40, 90, 80, procInst);
        sprintf(&StrOut[0], "%s%d%%", GetStringMess(MessageMess_SETUP), procInst);
        StrOutFont(0, 50, NO_INV, &StrOut[0], &fontNormal, leftAlign);
    }
    GrfOut();
}
//===================================================================
//===================================================================
void *FindLnk(HeadOfFile_type *pHOF)
{
    LnkNetFDP_type      LnkNetFDP = {0};
    HeadOfFile_type     HOF;
    u8                  i;

    for (i = 0; i < MAXDRIVES_CUT; i++)
    {
        if (ReadLnk(&LnkNetFDP, i))
        {                    //Чтение ссылки
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&HOF, LnkNetFDP.Lnk, sizeof(HeadOfFile_type)); //Чтение строки идентификации привода (0х2В)
            #else            
            memcpy(&HOF, (void *)((u32)LnkNetFDP.Lnk + STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса            
            #endif
            
            if (!strcmp((char*)&HOF, (char*)pHOF))
            {       //Совпали иденитификаторы
                //return LnkNetFDP.Lnk;
                //return &nv.LnkNetFDP[i];
                return &nv.OpisDrv[i].LnkNetFDP;
            }
        }
        else break;
    }
    return NULL;
}
//===================================================================
//===================================================================
void *FindLnkAndAdr(HeadOfFile_type *pHOF, u16 netAdr)
{
    LnkNetFDP_type      LnkNetFDP = {0};
    HeadOfFile_type     HOF;
    u8                  i;

    for (i = 0; i < MAXDRIVES_CUT; i++)
    {
        if (ReadLnk(&LnkNetFDP, i))
        {  //Чтение ссылки
          
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&HOF, LnkNetFDP.Lnk, sizeof(HeadOfFile_type)); //Чтение строки идентификации привода (0х2В)
            #else            
            memcpy(&HOF, (void *)((u32)LnkNetFDP.Lnk + STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса            
            #endif
            
            if (!strcmp((char*)&HOF, (char*)pHOF) && netAdr == LnkNetFDP.NetAdr)
            {   //Совпали иденитификаторы
                //return LnkNetFDP.Lnk;
                return &nv.OpisDrv[i].LnkNetFDP;
            }
        }
        else break;
    }
    return NULL;
}
/*void *FindLnk(char *str)
{
    LnkNetFDP_type      LnkNetFDP = {0};
    u8                  i;

    for (i = 0; i < MAXDRIVES; i++){
        if (ReadLnk(&LnkNetFDP, i)){                    //Чтение ссылки
            AT45RD(&StrOut[0],                          //Чтение строки идентификации привода (0х2В)
                   LnkNetFDP.Lnk,
                   sizeof(spfl.FileDescr->str0x2b));
            if (!strcmp((char*)&StrOut[0], str)){       //Совпали иденитификаторы
                //return LnkNetFDP.Lnk;
                return &nv.LnkNetFDP[i];
            }
        }
    }
    return NULL;
}*/

//===================================================================
//===================================================================
void *FindFreeLnk(void)
{
    LnkNetFDP_type      LnkNetFDP = {0};
    u8                  i;

    for (i = 0; i < MAXDRIVES_CUT; i++){
        if (!ReadLnk(&LnkNetFDP, i)){                    //Чтение ссылки
            return &nv.OpisDrv[i].LnkNetFDP;
        }
    }
    return &nv.OpisDrv[0].LnkNetFDP; //возврощаем если необходимо записать поверх старой 27.03.2013!
    //return NULL;
}
//===================================================================
//===================================================================
/*void *FindFreeStatus(void)
{
    LnkNetFDP_type      LnkNetFDP = {0};
    u8                  i;

    for (i = 0; i < MAXDRIVES; i++){
        if (!ReadLnk(&LnkNetFDP, i)){                    //Чтение ссылки
            return &nv.OpisDrv[i].Status;
        }
    }
    return NULL;
}*/

//===================================================================

//===================================================================
void *FindFreeFileDescr(void)
{
    LnkNetFDP_type      LnkNetFDP = {0};
    LnkNetFDP_type      prevLnkNetFDP = {0};
    u8                  i;

    for (i = 0; i < MAXDRIVES_CUT; i++){
        if (ReadLnk(&LnkNetFDP, i)){                //Чтение ссылки
            memcpy(&prevLnkNetFDP, &LnkNetFDP, sizeof(LnkNetFDP_type));
        }
        else{
            if (i == 0) {
                //break;
                return &spfl.FileDescr[0];
            }
            else{
                return (void*)((u32)prevLnkNetFDP.Lnk + sizeof(Descr_type));
            }
        }
    }
    return &spfl.FileDescr[0];
    //return NULL;
}
//===================================================================
const char txtNoLnk[] = "Не возможно установить больше приводов";
//===================================================================
HeadOfFile_type debHOF;

//===================================================================
void InstallDrv(void)
{
    LnkNetFDP_type      LnkNetFDP = {0};
    HeadOfFile_type     HOF_L;
    HeadOfFileOld_type  HOF_L_Old;
    u32                 Sz, sz, SzALL;
    f32                 procNet, procInst;
    FileFunctional_type Funct;
    OpisDrv_type        *pOpisDrv;
    void                *AdrFRAM[2];
    void                *AdrFreeAT45;
    void                *CurAdrSTM32; // *!* 
    void                *CurAdrAT45;
    void                *AdrInFile;
    u16                 i, cntFind = 0, cntInst = 0;
    keyName_type        key;
    u16                 tmp;
    u16                 crc;
    u8                  netAdr, NetAdrBegin, NetAdrEnd;
    u8                  cmp;
    Drv_err_type        code;
    Status_type         StatusRef[COUNT_STATUS];
    MsgReqKbdIn_type    MsgReqKbd;
    s32 numErrs = 0;
    
    Status_type                 Status;

    procNet = 0.f;
    procInst = 0;
    MsgReqKbd.KbdStateStartStop = KbdStartStop_OFF;
    OSQPost(QueueDrvInStartStop, &MsgReqKbd);
    
    if (AllPrmPnt->USTDRIVES || AllPrmPnt->ADD_DRIVE)
    {   
        StatusRef[0].NumGrp = 42;
        StatusRef[0].NumPrm = 3;

        StatusRef[1].NumGrp = 42;
        StatusRef[1].NumPrm = 5;

        StatusRef[2].NumGrp = 42;
        StatusRef[2].NumPrm = 11;
        
        StatusRef[17].NumGrp = 42;
        StatusRef[17].NumPrm = 0;
        
        StatusRef[3].NumGrp = 42;
        StatusRef[3].NumPrm = 1;

        StatusRef[4].NumGrp = 42;
        StatusRef[4].NumPrm = 2;

        StatusRef[5].NumGrp = 42;
        StatusRef[5].NumPrm = 4;
        
        StatusRef[6].NumGrp = 42;
        StatusRef[6].NumPrm = 6;

        StatusRef[7].NumGrp = 42;
        StatusRef[7].NumPrm = 7;

        StatusRef[8].NumGrp = 42;
        StatusRef[8].NumPrm = 8;
        
        StatusRef[9].NumGrp = 42;
        StatusRef[9].NumPrm = 9;

        StatusRef[10].NumGrp = 42;
        StatusRef[10].NumPrm = 10;

        StatusRef[11].NumGrp = 42;
        StatusRef[11].NumPrm = 12;
        
        StatusRef[12].NumGrp = 42;
        StatusRef[12].NumPrm = 13;

        StatusRef[13].NumGrp = 42;
        StatusRef[13].NumPrm = 14;

        StatusRef[14].NumGrp = 42;
        StatusRef[14].NumPrm = 15;
        
        StatusRef[15].NumGrp = 42;
        StatusRef[15].NumPrm = 16;

        StatusRef[16].NumGrp = 42;
        StatusRef[16].NumPrm = 17;
        
        StatusRef[18].NumGrp = 44;
        StatusRef[18].NumPrm = 0;

        StatusRef[19].NumGrp = 44;
        StatusRef[19].NumPrm = 2;
       
        if (AllPrmPnt->USTDRIVES)
        {
            memset(AdrFRAM, 0, sizeof(AdrFRAM));
            for (i = 0; i < MAXDRIVES_CUT; i++)
            {
                WriteLnk(&LnkNetFDP, (u8)i);
            }
        }

        netAdr = (u8)AllPrmPnt->ADD_DRIVE_ADR;
            if (AllPrmPnt->NET == 0)
            {//установлен режим местного пульта
                NetAdrBegin = (u8)AllPrmPnt->ADR_FOR_LOCAL;
                NetAdrEnd = NetAdrBegin + 1;
            }
            else
            {
                NetAdrBegin = 1;
                NetAdrEnd = 255;
            }
        for (netAdr = NetAdrBegin; netAdr < NetAdrEnd; netAdr++)
        {
            key = WaitKey(NULL);
            if (key == KEY_ESC)
            {
                break;
            }
            procNet = (f32)netAdr / (NetAdrEnd - 1) * 100;
            if (gIsNvsaOld)
            {
                code = ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], netAdr, 0, sizeof(HOF_L_Old),    fileDescrPrm, &HOF_L_Old, sizeof(HOF_L_Old));
            }
            else
            {
                code = ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], netAdr, 0, sizeof(HOF_L),    fileDescrPrm, &HOF_L, sizeof(HOF_L));
            }
            
            code |= ReadVerFunct(&MsgReqDrvIn[MSG_ID], netAdr, fileFunctional, &Funct);
            if (code == Ok_err)
            {
                cmp = 0;
                cntFind++;
                if (gIsNvsaOld)
                {
                    crc = GetCrc(&HOF_L_Old, sizeof(HOF_L_Old) - sizeof(HOF_L_Old.HeaderCrc));
                    if (crc != HOF_L_Old.HeaderCrc) continue;
                }
                else
                {
                    crc = GetCrc(&HOF_L, sizeof(HOF_L) - sizeof(HOF_L.HeaderCrc));
                    if (crc != HOF_L.HeaderCrc) continue;
                }
                tmp = swapU16(Funct.curFunct.functMask);
                crc = GetCrc(&tmp, sizeof(u16));
                if (crc != Funct.crc) {
                        MessageCancel(GetStringMess(MessageMess_ERROR_FUNCT));
                        break;
//                  continue;
                }
            
                //Поиск одинакового идентификатора для ПЧ
                if (TestValidDrv(netAdr, 0))
                { //есть такой иденитфикатор                  
                        procInst = 100.0;
                        cntInst++;
                        break;
                }
                 if (cmp == 0)
                 {
                    LnkNetFDP.NetAdr = (u8)netAdr;
                    CurAdrSTM32 = (void *)FLASH_OPIS_START_ADDR; // *!*  Указываем начальный адрес расположения памяти файла меню
                    if (!(AdrFreeAT45 = FindFreeFileDescr()))  // !!! ВЕРОЯТНО ЗДЕСЬ НАДО ЗАФИКСИРОВАТЬ АДРЕС ЗАГРУЗКИ ДАННЫХ В ПАМЯТЬ STM
                    {  //Нет свободных дескрипторов
                        MessageCancel(txtNoLnk);
                        break;
                    }
                    if (!(pOpisDrv = FindFreeLnk()))  ///   ????????
                    {        //нет свободных ссылок
                        MessageCancel(txtNoLnk);
                        break;
                    }
                    LnkNetFDP.NetAdr = 0;
                    LnkNetFDP.Lnk = 0;
                    LnkNetFDP.Crc = 0;
                    FM25WR(&pOpisDrv->LnkNetFDP, &LnkNetFDP, sizeof(LnkNetFDP));
                    LnkNetFDP.NetAdr = (u8)netAdr;
                    LnkNetFDP.Lnk = AdrFreeAT45;
                    LnkNetFDP.Crc = GetCrc(&LnkNetFDP, sizeof(LnkNetFDP_type) - 2);
                    AdrInFile = 0;
                    
                    #ifndef BLOCK_STM_MEMORY
                    // *!*  Стираем память в STM 
                    EraseMCUFlash();
                    #endif

                    if (gIsNvsaOld)
                    {
                        AdrInFile = (void *)((u32)AdrInFile + sizeof(HOF_L_Old)/2);     //Адрес внутри файла для привода
                        CurAdrAT45 = (void *)((u32)AdrFreeAT45);             // АДРЕС ВНУТРИ АТ45
 
                        #ifdef BLOCK_STM_MEMORY
                        AT45WR(CurAdrAT45, &HOF_L_Old, sizeof(HOF_L_Old));  // !!! ЗАПИСЬ ЗАГОЛОВКА ФАЙЛА ПАРАМЕТРОВ  !!!
                        #else
                        // *!* Копируем данные HOF в память STM
                        WriteMCUFlash(CurAdrSTM32, &HOF_L_Old, sizeof(HOF_L_Old));
                        #endif
                        
                        #ifdef BLOCK_STM_MEMORY
                        AT45RD(&HOF_L_Old, CurAdrAT45, sizeof(HOF_L_Old));                                          //Проверка
                        #else            
                        memcpy(&HOF_L_Old, (void *)((u32)CurAdrAT45 + STM32_FLASH_OFFSET), sizeof(HOF_L_Old)); //  *!* Загрузка из памяти STM по сдвигу адреса           
                        #endif

                        CurAdrAT45 = (void *)((u32)CurAdrAT45 + sizeof(HOF_L_Old));
                        SzALL = Sz = HOF_L_Old.SizeOfFileDescr - sizeof(HOF_L_Old) / 2;
                        
                        CurAdrSTM32 = (void *)((u32)CurAdrSTM32 + sizeof(HOF_L_Old));
                    }
                    else
                    {
                        AdrInFile = (void *)((u32)AdrInFile + sizeof(HOF_L)/2);     //Адрес внутри файла для привода
                        CurAdrAT45 = (void *)((u32)AdrFreeAT45);

                        #ifdef BLOCK_STM_MEMORY
                        AT45WR(CurAdrAT45, &HOF_L, sizeof(HOF_L));  // !!! ЗАПИСЬ ЗАГОЛОВКА ФАЙЛА ПАРАМЕТРОВ  !!!
                        #else
                        // *!* Копируем данные HOF в память STM
                        WriteMCUFlash(CurAdrSTM32, &HOF_L, sizeof(HOF_L));                        
                        #endif
                                                
                        #ifdef BLOCK_STM_MEMORY
                        AT45RD(&HOF_L, CurAdrAT45, sizeof(HOF_L));                                          //Проверка
                        #else            
                        memcpy(&HOF_L, (void *)((u32)CurAdrAT45 + STM32_FLASH_OFFSET), sizeof(HOF_L)); //  *!* Загрузка из памяти STM по сдвигу адреса           
                        #endif
                        
                        CurAdrAT45 = (void *)((u32)CurAdrAT45 + sizeof(HOF_L));
                        SzALL = Sz = HOF_L.SizeOfFileDescr - sizeof(HOF_L) / 2;
                        
                        CurAdrSTM32 = (void *)((u32)CurAdrSTM32 + sizeof(HOF_L));
                    }
                    crc = 0xffff;
                    do
                    {
                        procInst = (f32)Sz / (f32)SzALL;
                        procInst = (1.0f - procInst) * 100.0f;
                        InstallDrvProgress((u16)procNet, (u16)procInst, cntFind, cntInst, netAdr, 1);

                        if (Sz > 120)
                        {      //в словах
                            sz = 120;
                            Sz -= 120;
                        }
                        else
                        {
                            sz = Sz;
                            Sz = 0;
                        }
                        SzALL += sz;
                        code = ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], netAdr, AdrInFile, sz<<1, fileDescrPrm, &DataBuf[0], sizeof(DataBuf)); // Чтение из ПЧ файла параметров
                        if (code == Ok_err)
                        {
                            AdrInFile = (void *)((u32)AdrInFile + sz);
                            
                            #ifdef BLOCK_STM_MEMORY
                            AT45WR(CurAdrAT45, &DataBuf[0], sz * 2);  // !!! СОБСТВЕННО ЗАПИСЬ ФАЙЛА ОПИСАНИЙ ИЗ ПЧ В АТ45 ПУЛЬТА !!!
                            #else
                            // *!*  Записали блок в память STM
                            WriteMCUFlash(CurAdrSTM32, &DataBuf[0], sz * 2);                        
                            #endif

                            CurAdrAT45 = (void *)((u32)CurAdrAT45 + sz * 2);
                            crc = CrcPice(&DataBuf[0], sz * 2, crc); 
                            numErrs = 0;
                            
                            CurAdrSTM32 = (void *)((u32)CurAdrSTM32 + sz * 2);
                        }
                        else
                        {
                            if(numErrs < 100)
                            {
                                code = Ok_err;
                                Sz += 120;
                                numErrs++;
                            }
                            else
                            {
                                break;
                            }
                        }
                    }while (Sz != 0);
                    if (gIsNvsaOld)
                    {
                        if (code == Ok_err && HOF_L_Old.FileCrc == crc) 
                        {
                            cntInst++;
                            FM25WR(&pOpisDrv->LnkNetFDP, &LnkNetFDP, sizeof(LnkNetFDP));
                            //=================================================
                            //Записываем параметры статусного меню по умолчанию
                            //===================================================
                            FM25WR(&pOpisDrv->StatusRef, &StatusRef, sizeof(StatusRef));
                            FM25WR(&pOpisDrv->Functional, &Funct, sizeof(Funct));
                            //=================================================
                        }
                    }
                    else
                    {
                        if (code == Ok_err && HOF_L.FileCrc == crc) 
                        {
                            cntInst++;
                            FM25WR(&pOpisDrv->LnkNetFDP, &LnkNetFDP, sizeof(LnkNetFDP));
                            //=================================================
                            //Записываем параметры статусного меню по умолчанию
                            //===================================================
                            FM25WR(&pOpisDrv->StatusRef, &StatusRef, sizeof(StatusRef));
                            FM25WR(&pOpisDrv->Functional, &Funct, sizeof(Funct));
                            FM25WR(&pOpisDrv->Lang, &TypeLanguage, sizeof(TypeLanguage));
                            //=================================================
                        }
                    }
                }
            }
        }
        // Если количество найденых и установленых приводов не равно выводим сообщение
        if (cntFind != cntInst)
        {
            MessageCancel(GetStringMess(MessageMess_ERROR_SET));
        }
    }
    
    MsgReqKbd.KbdStateStartStop = KbdStartStop_ON;
    OSQPost(QueueDrvInStartStop, &MsgReqKbd);
    AllPrmPnt->USTDRIVES = 0;
    AllPrmPnt->ADD_DRIVE = 0;
    
    Status.NumGrp = (CONTRAST_GRPPRM >> 7) & 0x7f;
    Status.NumPrm = CONTRAST_GRPPRM & 0x7f;

    if (gIsNvsaOld)
    {       
        if(FindDescriptorWhithCheck(&MDPDrive.DPDrive[0], &Status,  
                                    &HOF_L_Old,
                                    (void*)((u32)(&spfl.FileDescr[0])), 1,
                                    0))
        {
            UpdateContrast(CONTRAST_GRPPRM);
        }
        else
        {
            AllPrmPnt->KONTRAST = DEFAULT_CONTRAST;
            setContrast();
        }        
    }
    else
    {       
        if(FindDescriptorWhithCheck(&MDPDrive.DPDrive[0], &Status,  
                                    &HOF_L,
                                    (void*)((u32)(&spfl.FileDescr[0])), 1,
                                    0))
        {
            UpdateContrast(CONTRAST_GRPPRM);
        }
        else
        {
            AllPrmPnt->KONTRAST = DEFAULT_CONTRAST;
            setContrast();
        }        
    }
    
    Ust_Copy(RAM_to_NV); // Добавлено сохранение в явном виде.
    // Были выявлены случаи, когда новенький пульт постоянно инсталлировался, так как
    // ряд параметров не сохранялся в nv структуру.
/*    
    OpisDrv_type OpisDrv;
    FM25RD(&OpisDrv, &nv.OpisDrv[0], sizeof(OpisDrv_type));
    if (gIsNvsaOld)
    {   
        initMessPult(0);
    }
    else
    {
        //TypeLanguage = OpisDrv.Lang;
        initMessPult(OpisDrv.LnkNetFDP.Lnk);
    }    
*/    
}
//===================================================================

//===================================================================
void Password(void)
{
    if (AllPrmPnt->PASSW_MASTER == AllPrmPnt->ETALON_MASTER) {
        LevelAccess = MASTER_PASSW;
    }
    else if (AllPrmPnt->PASSW_TEXN == AllPrmPnt->ETALON_TEXN) {
        LevelAccess = TEXN_PASSW;
    }
    else if (AllPrmPnt->PASSW_USER == AllPrmPnt->ETALON_USER) {
        LevelAccess = USER_PASSW;
    }
    else LevelAccess = NO_PASSW;

    //LevelAccess = MASTER_PASSW;
}

void ResetPassword(void)
{
    Drv_err_type            drvErr;
    MsgReqDrvIn_type        reqMsgReqDrvIn = {0};
    LnkNetFDP_type          LnkNetFDP;
    u8                      Buf[20];
    char                    strtmp[100];
    
    u8						cntRepeat;

	cntRepeat = 0;
    
    if(LevelAccess == MASTER_PASSW)
    {
        memset(&HandlerMenuPultNew_struct, 0, sizeof(HandlerMenuPultNew_struct));
    }

    if (ReadLnk(&LnkNetFDP, 0)) {
        reqMsgReqDrvIn.AdrNet = LnkNetFDP.NetAdr;
    }
    else{
//        MessageCancel("Сеть не установлена");
        reqMsgReqDrvIn.AdrNet = 1;
    }

//        MessageThrough("Чтение паролей");
    reqMsgReqDrvIn.AdrData  = ADDR_MASTER;
    reqMsgReqDrvIn.Sz       = 1;
    reqMsgReqDrvIn.Func     = WRITE_REGS;
    Buf[0]                  = 0;
    Buf[1]                  = 0;
    reqMsgReqDrvIn.Buf      = &Buf[0];
    reqMsgReqDrvIn.SzBuf    = sizeof(Buf);
    reqMsgReqDrvIn.SzOut    = 2;
    reqMsgReqDrvIn.NumParam = 0;
#ifndef _PROJECT_IAR_ 
    reqMsgReqDrvIn.countByteRead = reqMsgReqDrvIn.Sz*2 + 6;  // Ожидаем по колич данных
#endif

    cntRepeat = 2;
	while(1){
	    drvErr = ReQuestDrv(QueueDrvIn, &reqMsgReqDrvIn);
	    if( drvErr != Ok_err ){
	    	cntRepeat--;
			if( cntRepeat == 0 ){ 	// Пароль не вычитан
				return;
			}
	        sprintf(&strtmp[0], GetStringMess(MessageMess_ERROR_CONECT));
	        MessageCancel( strtmp );
	    }
        else{
	        AllPrmPnt->PASSW_MASTER = swapU16(*(u16 *)&Buf[0]);
	        break;
	    }
	}
        
    reqMsgReqDrvIn.AdrData  = ADDR_TEXN;
    reqMsgReqDrvIn.Sz       = 1;
    reqMsgReqDrvIn.Func     = WRITE_REGS;
    Buf[0]                  = 0;
    Buf[1]                  = 0;
    reqMsgReqDrvIn.Buf      = &Buf[0];
    reqMsgReqDrvIn.SzBuf    = sizeof(Buf);
    reqMsgReqDrvIn.SzOut    = 2;
    reqMsgReqDrvIn.NumParam = 0;
#ifndef _PROJECT_IAR_ 
    reqMsgReqDrvIn.countByteRead = reqMsgReqDrvIn.Sz*2 + 6;  // Ожидаем по колич данных
#endif
     
    drvErr = ReQuestDrv(QueueDrvIn, &reqMsgReqDrvIn);

        if( drvErr == Ok_err ){
            AllPrmPnt->PASSW_TEXN = swapU16(*(u16 *)&Buf[0]);
        }

    reqMsgReqDrvIn.AdrData  = ADDR_USER;
    reqMsgReqDrvIn.Sz       = 1;
    reqMsgReqDrvIn.Func     = WRITE_REGS;
    Buf[0]                  = 0;
    Buf[1]                  = 0;
    reqMsgReqDrvIn.Buf      = &Buf[0];
    reqMsgReqDrvIn.SzBuf    = sizeof(Buf);
    reqMsgReqDrvIn.SzOut    = 2;
    reqMsgReqDrvIn.NumParam = 0;
#ifndef _PROJECT_IAR_ 
    reqMsgReqDrvIn.countByteRead = reqMsgReqDrvIn.Sz*2 + 6;  // Ожидаем по колич данных
#endif
   
    drvErr = ReQuestDrv(QueueDrvIn, &reqMsgReqDrvIn);

        if( drvErr == Ok_err ){
            AllPrmPnt->PASSW_USER = swapU16(*(u16 *)&Buf[0]);
        }
}
//===================================================================
//===================================================================
f32 CorrectKontrast(void)
{
//    u16 codeADC;
//    codeADC = GetCodeADC();
//    AllPrmPnt->NET00000 = (u32)((f32)codeADC * 0.0427350427350427) - 50;
    return  1.0; //KoefKontrast(codeADC);
}
//===================================================================
//===================================================================
void ChangeSpeedUart(void)
{// с индукционной запиткой скорость не меняется
/*    if (AllPrmPnt->SPEED <  GetNumUartSpeed()) {
        UartMsg.BaudRate = AllPrmPnt->SPEED;
    }
*/
}
//===================================================================
//      Размер заголовка файла описателя параметров
//===================================================================
#define SIZE_HEAD_UST       sizeof(spfl.FileSettingsDrv[0].ustDrv.Crc) +                \
                            sizeof(spfl.FileSettingsDrv[0].ustDrv.MicroSignatura) +     \
                            sizeof(spfl.FileSettingsDrv[0].ustDrv.kolvUst) +            \
                            sizeof(spfl.FileSettingsDrv[0].ustDrv.rsrvd)
//===================================================================
// Функция чтения всех уставок скопом из привода и сохранение
//===================================================================
/* // Заблокировал - Сосновчик
void LoadSettingsFromDrv(void)
{
    FileFunctional_type     Funct;
    void                    *curAdrAT45;
    HeadOfFile_type         HOF;
    Drv_err_type            code;
    u16                     kolvUst, crc, signatUst;
    s16                     numUstPult;
    u8                      netAdr;


    if (AllPrmPnt->EN_UST_READ == 1){
        AllPrmPnt->EN_UST_READ = 0;
        netAdr = (u8)AllPrmPnt->ADR_DRV_UST_READ;
        if(ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], netAdr, 0, sizeof(HOF), fileDescrPrm ,&HOF, sizeof(HOF))){
      //if(RequestFDP     (&MsgReqDrvIn[MSG_ID], netAdr, 0, sizeof(HOF)>>1,               &HOF, sizeof(HOF))){
            MessagePopUp(GetStringMess(MessageMess_ERROR_CONECT));
        }
        else{
            crc = GetCrc(&HOF, sizeof(HOF) - sizeof(HOF.HeaderCrc));
            if (crc != HOF.HeaderCrc){
                MessagePopUp("Ошибка чтения!");
                return;
            }
        }
        kolvUst = HOF.QuantityUst;
        signatUst = HOF.Sign;
        //code = Request0x2B(&MsgReqDrvIn[MSG_ID],
        //                   netAdr,
        //                   &DataBuf[0],
        //                   sizeof(DataBuf));
        code = ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], netAdr, 0, sizeof(HOF),    fileDescrPrm,&HOF, sizeof(HOF));
      //code = RequestFDP     (&MsgReqDrvIn[MSG_ID], netAdr, 0, sizeof(HOF)>>1,              &HOF, sizeof(HOF));
        if (code != Ok_err){
            MessagePopUp(GetStringMess(MessageMess_ERROR_CONECT));
            return;
        }
        numUstPult = AllPrmPnt->NUM_UST_PULT_WRITE;                             //Получаем номер уставок в пульте
        curAdrAT45 = &spfl.FileSettingsDrv[numUstPult].HOF;                     //Получаем адрес строки 0х2b уставок во флеши
        //AT45WR(curAdrAT45,                                                    //Записываем строку 0х2b во флешь      ---   ИСКЛЮЧЕНА
        //      &DataBuf[0],
        //       sizeof(spfl.FileSettingsDrv[0].str0x2b));
        AT45WR(curAdrAT45,                                                      //Записываем строку 0х2b во флешь   ---   ИСКЛЮЧЕНА
               &HOF,
               sizeof(HeadOfFile_type));

        curAdrAT45 = &spfl.FileSettingsDrv[numUstPult].nomI;                    //Вычисление адреса массива уставок
        if (WriteFileToAt45FromDrv(netAdr, 14, (void *)3, 2, curAdrAT45)){      //Вычитывание номинального тока двигателя
            MessagePopUp(GetStringMess(MessageMess_ERROR_CONECT));
            return;
        }
        curAdrAT45 = &spfl.FileSettingsDrv[numUstPult].ustDrv.masUstDrv;        //Вычисление адреса массива уставок
        if (WriteFileToAt45FromDrv(netAdr, 15, 0, kolvUst * 4, curAdrAT45)){    //Вычитывание уставок из привода и запись во флешь
            MessagePopUp(GetStringMess(MessageMess_ERROR_CONECT));
            return;
        }
        if (ReadVerFunct(&MsgReqDrvIn[MSG_ID], netAdr, 18, &Funct)) {
            MessagePopUp(GetStringMess(MessageMess_ERROR_CONECT));
            return;
        }

        curAdrAT45 = &spfl.FileSettingsDrv[numUstPult].ustDrv.kolvUst;          //Адрес куда записываеть количество уставок
        //kolvUst = swapU16(kolvUst);
        kolvUst = swapU16(kolvUst);
        AT45WR(curAdrAT45, &kolvUst, sizeof(kolvUst));
        curAdrAT45 = &spfl.FileSettingsDrv[numUstPult].ustDrv.MicroSignatura;   //Адрес куда записывать сигнатуру
        //signatUst = swapU16(signatUst);
        signatUst = swapU16(signatUst);
        AT45WR(curAdrAT45, &signatUst, sizeof(signatUst));
        kolvUst = swapU16(kolvUst);
        signatUst = swapU16(signatUst);
        crc = CrcAT45(curAdrAT45,
                      (SIZE_HEAD_UST -
                      sizeof(spfl.FileSettingsDrv[0].ustDrv.Crc)) +
                      kolvUst * 4);                                             //Подсчет КС на файл
        curAdrAT45 = &spfl.FileSettingsDrv[numUstPult].ustDrv.Crc;              //Адрес куда писать КС
        crc = swapU16(crc);
        AT45WR(curAdrAT45,
               &crc,
               sizeof(spfl.FileSettingsDrv[0].ustDrv.Crc));                     //Запись КС

        AT45WR(&spfl.FileSettingsDrv[numUstPult].Functional,
               &Funct,
               sizeof(FileFunctional_type));                                    //Запись функционала

        //MessagePopUp("Уставки прочитаны");
    }
}
*/
//===================================================================
// Функция записи сохраненного набора уставок из пульта в привод
//===================================================================
/*  /// Заблокировал - Сосновчик
void SaveSettingsToDrv(void)
{
    FileFunctional_type FunctInst, FunctDrv;
    void            *curAdrAT45;
    Drv_err_type    code;
    HeadOfFile_type HOFDrv, HOFFile;
    u16             kolvUst, crcCalc, crcAt45;
    u16             nomI, nomIDrv;
    char            strtmp[60] = {0};
    char            strtmp1[60] = {0};
    s16             numUstDrv, numUstPult;
    u8              netAdr;

    if(AllPrmPnt->EN_UST_WRITE == 1) {
        AllPrmPnt->EN_UST_WRITE = 0;
        netAdr = (u8)AllPrmPnt->ADR_DRV_UST_WRITE;
        code = Request0x2B(&MsgReqDrvIn[MSG_ID],
                           netAdr,
                           &strtmp1[0],
                           sizeof(strtmp1));
        if (code != Ok_err){
            MessagePopUp(GetStringMess(MessageMess_ERROR_CONECT));
        }
        else{
            numUstDrv = AllPrmPnt->NUM_UST_DRV_WRITE + 5;
            numUstPult = AllPrmPnt->NUM_UST_PULT_WRITE;
            curAdrAT45 = &spfl.FileSettingsDrv[numUstPult].ustDrv.kolvUst;  //Адрес откуда считать КС
            if(ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], netAdr, 0, sizeof(HeadOfFile_type),    fileDescrPrm, &HOFDrv, sizeof(HeadOfFile_type))){
          //if(RequestFDP     (&MsgReqDrvIn[MSG_ID], netAdr, 0, sizeof(HeadOfFile_type)>>1,               &HOFDrv, sizeof(HeadOfFile_type))){
                MessagePopUp(GetStringMess(MessageMess_ERROR_CONECT));
                return;
            }
            
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&kolvUst, curAdrAT45, sizeof(kolvUst));
            #else            
            memcpy(&kolvUst, (void *)((u32)curAdrAT45 + STM32_FLASH_OFFSET), sizeof(kolvUst)); //  *!* Загрузка из памяти STM по сдвигу адреса            
            #endif
            
            kolvUst = swapU16(kolvUst);
            if (kolvUst != HOFDrv.QuantityUst) {
                MessagePopUp(GetStringMess(MessageMess_ERROR_COMPATIBILITY));
                return;
            }
            curAdrAT45 = &spfl.FileSettingsDrv[numUstPult].ustDrv.MicroSignatura;
            crcCalc = CrcAT45(curAdrAT45, SIZE_HEAD_UST -
                                          sizeof(spfl.FileSettingsDrv[0].ustDrv.Crc) +
                                          kolvUst * 4);                    //Подсчет КС на файл
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&crcAt45, &spfl.FileSettingsDrv[numUstPult].ustDrv.Crc, sizeof(crcAt45)); //Чтение КС из АТ45
            #else            
            memcpy(&crcAt45, (void *)((u32)&spfl.FileSettingsDrv[numUstPult].ustDrv.Crc + STM32_FLASH_OFFSET), sizeof(crcAt45)); //  *!* Загрузка из памяти STM по сдвигу адреса            
            #endif
           
            crcCalc = swapU16(crcCalc);
            if (crcCalc != crcAt45) {                                       // Файл не существует !!!
                MessagePopUp(GetStringMess(MessageMess_ERROR_COMPATIBILITY));
                return;
            }
            //AT45RD(&strtmp[0],                                              //Чтение строки 0х2b
            //       &spfl.FileSettingsDrv[numUstPult].str0x2b,
            //       sizeof(spfl.FileSettingsDrv[0].str0x2b));
            
            
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&HOFFile, &spfl.FileSettingsDrv[numUstPult].HOF, sizeof(HeadOfFile_type)); //Чтение HOF
            #else            
            memcpy(&HOFFile, (void *)((u32)&spfl.FileSettingsDrv[numUstPult].HOF + STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса            
            #endif

            curAdrAT45 = &spfl.FileSettingsDrv[numUstPult].nomI;                //Вычисление номинального тока

            #ifdef BLOCK_STM_MEMORY
            AT45RD(&nomI, curAdrAT45, sizeof(nomI));                        //Вычитывание номинального тока двигателя
            #else            
            memcpy(&nomI, (void *)((u32)curAdrAT45 + STM32_FLASH_OFFSET), sizeof(nomI)); //  *!* Загрузка из памяти STM по сдвигу адреса           
            #endif
            
            if (ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], netAdr, (void *)3, 2, 14, &nomIDrv, sizeof(nomI))){          //Вычитывание номинального тока двигателя
                MessagePopUp(GetStringMess(MessageMess_ERROR_CONECT));
                return;
            }

            nomIDrv = swapU16(nomIDrv);
            nomI = swapU16(nomI);
            if (nomIDrv != nomI) {
                MessagePopUp("Различие по номинальному току");
                sprintf(&strtmp[0], "-ток привода %.1f; -ток в наборе уставок %.1f", (f32)nomIDrv/10., (f32)nomI/10.);
                MessageCancel(&strtmp[0]);
                return;
            }

            if (ReadVerFunct(&MsgReqDrvIn[MSG_ID], netAdr, 18, &FunctDrv)) {
                MessagePopUp(GetStringMess(MessageMess_ERROR_CONECT));
                return;
            }

            #ifdef BLOCK_STM_MEMORY
            AT45RD(&FunctInst, &spfl.FileSettingsDrv[numUstPult].Functional, sizeof(FileFunctional_type)); //Запись функционала
            #else            
            memcpy(&FunctInst, (void *)((u32)&spfl.FileSettingsDrv[numUstPult].Functional + STM32_FLASH_OFFSET), sizeof(FileFunctional_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
            #endif

            if (FunctDrv.curFunct.functMask != FunctInst.curFunct.functMask || FunctDrv.crc != FunctInst.crc) {
                MessagePopUp(GetStringMess(MessageMess_DRIVE_NO_TEST));
                return;
            }


            //if (0 == strcmp(&strtmp[0], &strtmp1[0])) {                  //Сравнение строки привода и строки файла
            if (0 == strncmp((char *)&HOFDrv, (char *)&HOFFile, sizeof(HeadOfFile_type))) 
            { //Сравнение строки привода и строки файла
                //AT45RD(&kolvUst,
                //       &spfl.FileSettingsDrv[numUstPult].ustDrv.kolvUst,
                //       sizeof(kolvUst));
                curAdrAT45 = &spfl.FileSettingsDrv[numUstPult].ustDrv;
                if(WriteFileToDrvFromAt45((u8)netAdr, (u8)numUstDrv, 0, SIZE_HEAD_UST + kolvUst * 4, curAdrAT45)){
                    MessagePopUp(GetStringMess(MessageMess_ERROR_CONECT));
                }
                else{
                    //MessagePopUp("Уставки записаны");
                }

            }
            else{
                MessagePopUp(GetStringMess(MessageMess_ERROR_COMPATIBILITY));
                return;
            }
        }
    }
} */
                             
//===================================================================
//===================================================================
/* Заблокировал - Сосновчик
void Microprog(void * pHOF)
{
    InstallDrv();                                                           //Установка приводов
    //    InstallDrvOne();                                                  //Установка одного привода
   // ReqTekPassword(pHOF);
    Password();                                                             //Проверка пароля
    ChangeSpeedUart();                                                      //изменение скорости УАРТА
    LoadSettingsFromDrv();                                                  //Чтение уставок из привода
    SaveSettingsToDrv();                                                    //Запись уставок в привод
}
*/
//===================================================================
/*********************************************
 * Запись файла в привод из AT45             *
 *  Входные параметры:                       *
 *  - сетевой адрес привода                  *
 *  - номер файла                            *
 *  - адрес внутри файла                     *
 *  - объем записи                           *
 *  - адрес файла внутри флеши               *
 *  Выходные параметры                       *
 *  - ошибка драйвера                        *
 *********************************************/
/*
Drv_err_type WriteFileToDrvFromAt45(u8 netAdr, u8 numFile, void *beginAdrInFile, u16 sz, void *beginAdrInAt45)
{
    Drv_err_type    code;
    u8              *curAdrInAt45, *curAdrInFile;
    u8              Data[256];//, Data1[256];
    u16             szRemain, szWr, szRd;

    curAdrInFile = beginAdrInFile;
    curAdrInAt45 = beginAdrInAt45;

    szRemain = sz;
    szWr = 176; //!!!!Кратность странице флеши
    szRd = 176;

    do{
        if (szRemain > 176) {
            szRemain -= 176;
        }
        else{
            szWr = szRemain;
            szRd = szWr;// * 2;
            szRemain = 0;
        }

        #ifdef BLOCK_STM_MEMORY
        AT45RD(&Data[0], curAdrInAt45, szRd);
        #else            
        memcpy(&Data[0], (void *)((u32)curAdrInAt45 + STM32_FLASH_OFFSET), szRd); //  *!* Загрузка из памяти STM по сдвигу адреса
        #endif

        //for(i = 0; i < szRd; i += 2){
        //    *(u16 *)&Data[i] = swapU16(*(u16 *)&Data[i]);
        //}

        code =  WriteFileToDrv(&MsgReqDrvIn[MSG_ID], netAdr, curAdrInFile, (u8)szWr, numFile, &Data[0], sizeof(Data));//Запрос файла количества уставок, 0-й адрес, 2 байта, 16 файл
        //code =  ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], netAdr, curAdrInFile, szRd, numFile, &Data1[0], sizeof(Data1));//Запрос файла количества уставок, 0-й адрес, 2 байта, 16 файл
        if (code != Ok_err) {
            return code;
        }

        curAdrInFile += szWr >> 1;
        curAdrInAt45 += szRd;
    }while (szRemain > 0);

    return code;
}
*/
/*********************************************
 * Запись файла в AT45 из привода            *
 *  Входные параметры:                       *
 *  - сетевой адрес привода                  *
 *  - номер файла                            *
 *  - адрес внутри файла                     *
 *  - объем вычитываемых данных из привода   *
 *  - адрес файла внутри флеши               *
 *  Выходные параметры                       *
 *  - ошибка драйвера                        *
 *********************************************/
/*
Drv_err_type WriteFileToAt45FromDrv(u8 netAdr, u8 numFile, void *beginAdrInFile, u16 sz, void *beginAdrInAt45)
{
    Drv_err_type    code;
    u8              *curAdrInAt45, *curAdrInFile;
    u16             szRemain, szReq, szWr;

    curAdrInFile = beginAdrInFile;
    curAdrInAt45 = beginAdrInAt45;

    szRemain = sz;
    szReq = 240;
    szWr  = 240;
    do{
        if (szRemain > 240) {
            szRemain -= 240;
        }
        else{
            szReq = szRemain;
            szWr = szReq;// * 2;
            szRemain = 0;
        }
        code = ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], netAdr, curAdrInFile, (u8)szReq, numFile, &DataBuf[0], sizeof(DataBuf));//Запрос файла количества уставок, 0-й адрес, 2 байта, 16 файл
        if (code != Ok_err) {
            return code;
        }
//        for(i = 0; i < szReq; i += 2){
//            *(u16 *)&DataBuf[i] = swapU16(*(u16 *)&DataBuf[i]);
//        }
        AT45WR(curAdrInAt45, &DataBuf[0], szWr);  //   ----------   WriteFileToAt45FromDrv ()
        //AT45RD(&ddd[0], curAdrInAt45, szWr);
        curAdrInFile += szReq >> 1;
        curAdrInAt45 += szWr;
    }while (szRemain > 0);

    return code;
}
*/
//===================================================================
//===================================================================
u16 CrcAT45(void *beginAdrInAt45, u16 sz)
{
    u16             szRemain, szRd;
    u16             crc;
    u8              *curAdrInAt45;
    u8              Buf[40];

    szRemain = sz;
    curAdrInAt45 = beginAdrInAt45;

    crc = 0xffff;
    szRd = 40;
    do{
        if (szRemain > 40) {
            szRemain -= 40;
        }
        else{
            szRd = szRemain;
            szRemain = 0;
        }
        
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&Buf[0], curAdrInAt45, 40);
        #else            
        memcpy(&Buf[0], (void *)((u32)curAdrInAt45 + STM32_FLASH_OFFSET), 40); //  *!* Загрузка из памяти STM по сдвигу адреса           
        #endif

        crc = CrcPice(&Buf[0], szRd, crc);
        curAdrInAt45 += szRd;
    }while (szRemain > 0);

    return crc;
}

//===================================================================

//===================================================================
//Стирание флеши контроллера
LoadError_type EraseMCUFlash(void)
{
    LoadError_type      LoadError;
    uint32_t StartSector = 0, EndSector = 0, SectorCounter = 0;
    
    /* Unlock the Flash to enable the flash control register access *************/ 
    FLASH_Unlock();
    
    /* Erase the user Flash area
      (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

    /* Clear pending flags (if any) */  
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                      FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 

    /* Get the number of the start and end sectors */
    StartSector = GetSector(FLASH_OPIS_START_ADDR);
    EndSector = GetSector(FLASH_OPIS_END_ADDR);

    for (SectorCounter = StartSector; SectorCounter <= EndSector; SectorCounter += 8)
    {
        /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
           be done by word */ 
        if (FLASH_EraseSector(SectorCounter, VoltageRange_3) != FLASH_COMPLETE)
        { 
            /* Error occurred while sector erase. 
             User can add here some code to deal with this error  */
            LoadError = LoadError_ERROR;
        }
        else LoadError = LoadError_OK;
    }
    
    FLASH_Lock(); 
    return  LoadError;    
}
//===================================================================

//===================================================================
// Запись массива данных в Флэш-память STM32
// pFlashAddr - указатель места назначения
// pData - указатель на данные
// size - размер данных для записи
// ДЛЯ КОРРЕКТНОЙ ЗАПИСИ ИСПОЛЬЗУЕМАЯ ПАМЯТЬ ДОЛЖНА БЫТЬ ОЧИЩЕНА ПРИ ПОМОЩИ EraseMCUFlash()
// Загрузка данных идет побайтно
//===================================================================
LoadError_type WriteMCUFlash( void *pFlashAddr, void *pData, u16 size )
{

    uint32_t      Address = 0;
    u8            data_8;
    u32           index;
    u8            *pBuf;
    pBuf = (u8*)pData;
    
    if(size == 0 ) { return LoadError_OK; }
    
    /* Разблокировка флэшки для начала записи */ 
    FLASH_Unlock();
    
    /* Очистка всех флагов */  
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);   
    
    Address = (u32)pFlashAddr;
    index = 0;
    while (size)
    {
        data_8 = pBuf[index++];
        if (FLASH_ProgramByte(Address, data_8) == FLASH_COMPLETE)
        {
            Address++;
            size--;
        }
        else
        { 
         /* Error occurred while writing data in Flash memory. 
            User can add here some code to deal with this error */
          FLASH_Lock(); 
            return LoadError_ERROR;
        }
    }    
//    OSSemPend( SPISem, SYS_FOREVER, &err);
    FLASH_Lock(); 
    return LoadError_OK;
}
//===================================================================

//===================================================================
// Возвращает номер сектора
uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_Sector_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_Sector_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_Sector_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_Sector_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_Sector_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_Sector_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_Sector_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_Sector_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_Sector_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_Sector_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_Sector_10;  
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
  {
    sector = FLASH_Sector_11;  
  }

  return sector;
}
//===================================================================
