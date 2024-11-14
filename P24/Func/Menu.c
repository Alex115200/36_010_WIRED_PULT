/**********************************************
 *                                            *
 *    (c) Copyright _Triol Corporation 2010   *
 *                                            *
 * File name   : Menu.c                       *
 * Description : Модуль работы с меню         *
 *                                            *
 * Date        : 29.03.2010                   *
 * Author      :                              *
 * Modified    :                              *
 *                                            *
 **********************************************/
#define  Menu_PRIVAT
//#define  BLOCK_STM_MEMORY  // Блокировка работы с памятью STM и на работу с AT45


#include "globalIncludes.h"
#include "fixedUst.h"
#include "TskDrv.h"
#include "filemp.h"


#include "EdIzm.c"              // Массив строк с единицами измерения
#include "Month.c"              // Массивы с определениями для месяцев
#include "MenuStatDrv.h"        //


extern char const povtor[];
extern HandlerMenuSetup_struct          HandlerMenuSetup;

extern char MasMess[COUNT_MESS][LEN_MESS];
extern char MasTitle[COUNT_TITLE][LEN_TITLE];
extern char MasFlash[COUNT_FLASH][LEN_FLASH];
extern const char * msgStringsAlarm[];
extern const char * msgStringsTitle[];
extern const char * msgStringsMess[];
extern const char * msgMasEdIzm[];

u16 CntReq = 0;
u16 TypeLanguage;
uint8_t gIsNvsaOld;
//extern const u16 PultProgTablOne[];
extern u8 ReadLnk(LnkNetFDP_type *pLnkNetFDP, u8 Num);
HandlerMenuPultNew_struct_type HandlerMenuPultNew_struct;
HandlerMenuChangeDrive_struct_type HandlerMenuChangeDrive_struct;

u16                             DMNumGlobalHOF[MAX_NUM_MENU];
DescriptorMenu_type             *DMGlobal[MAX_NUM_MENU];             
DescriptorPrm_type              *DPGlobal[MAX_NUM_PRM];
DescriptorMenu_type             DMDrive[MAX_NUM_MENU];//DMDrive[MAX_STR_OTOBR_MENU];
Full_DescriptorPrm_type         DPStatus;
Full_MenuDescriptorPrm_type     MDPDrive;
Full_DescriptorPrm_type         DPDriveStatus;

char                            StrOut[256];
u8                              CURRENT_COUNT_STATUS;
static u8                       flag_from_status_word2 = 0;

DescriptorMenuOld_type             *DMGlobalOld[MAX_NUM_MENU];             
DescriptorPrmOld_type              *DPGlobalOld[MAX_NUM_PRM];
DescriptorMenuOld_type             DMDriveOld[MAX_NUM_MENU];

void setContrast(void);
#ifndef _PROJECT_IAR_
void setContrast(void) {}
#endif // _PROJECT_IAR_
//===================================================================


//const char noInst[] = "Привод не установлен";
//const char noValid[] = "Привод не прошел проверку"; 

//===================================================================
//                  Главное меню пульта
//===================================================================
//const
Menu_type MainMenu[] = {
    {"Меню пульта",                     &CallMenuPult},
    {"Меню привода",                    &CallMenuDrive},//&HandlerMenuChangeDrive},
    {"Настр. статуса",                  &CallMenuStatusReplace},//&HandlerMenuChangeDrive},
//    {"Тест кнопок",                     &CallMenuKeyTest},//&HandlerMenuChangeDrive},
    {NULL,                              NULL}
};
//===================================================================
// Просмотрщики параметров в зависимости от типа параметра
//===================================================================
void (*const FuncViewParam[])(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str) = {
    NULL,               	// sec2Frmt
    &TxtToStr,          	// NtxtFrmt
    &HexToStr,          	// Hex16Frmt
    &UIntToStr,         	// int16Frmt
    HourMinToStr,       	// HourMinFrmt
    NULL,               	// MinSecFrmt
    &FloatToStr,        	// floatFrmt
    &UIntToStr,         	// int32Frmt
    NULL,               	// HmsFrmt
    &UnixToStr,         	// UTimeFrmt
    &RefToStr,          	// refFormat
    &DayMonToStr,       	// MonDayFrmt
    &UIntToStr,         	// uns16Frmt
    &UIntToStr          	// uns32Frmt
};
//===================================================================

//===================================================================
struct{
    u16             GetOpisPrm;
    LnkNetFDP_type  LnkNetFDP;
    void            *AdrFRAM[MAXDRIVES];
    u8              NetAdr[MAXDRIVES];
    u8              CurLnk;
    u8              MaxDrv;
}HandlerMenuStatus_struct;
//===================================================================

//===================================================================
void (*const FuncViewEditParam[])(wm_type *, char *) = {
    NULL,                   // sec2Frmt
    NULL,                   // NtxtFrmt
    NULL,                   // Hex16Frmt
    NULL,//&UIntToStrEdit,  // int16Frmt
    NULL,                   // HourMinFrmt
    NULL,                   // MinSecFrmt
    &FloatViewEdit,         // floatFrmt
    NULL,//&UIntViewEdit,   // int32Frmt
    NULL,                   // HmsFrmt
    NULL,                   // UTimeFrmt
    NULL,                   // refFormat
    NULL,                   // MonDayFrmt
    NULL,//&UIntViewEdit,   // uns16Frmt
    NULL//&UIntViewEdit     // uns32Frmt
};
//===================================================================

//===================================================================
u32 (*const FuncEditParam[])(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *Str) = {
    NULL,           					//sec2Frmt
    &TextToStrEdit, 					//NtxtFrmt
    NULL,           					//Hex16Frmt
    &UIntToStrEdit, 					//int16Frmt
    &HMorMSorMDToStrEdit,  				//HourMinFrmt
    &HMorMSorMDToStrEdit,  				//MinSecFrmt
    &FloatToStrEdit,					//floatFrmt
    &UIntToStrEdit, 					//int32Frmt
    NULL,           					//HmsFrmt
    NULL,           					//UTimeFrmt
    NULL,//&HMorMSorMDorRefToStrEdit,   //refFormat
    &HMorMSorMDToStrEdit,  				//MonDayFrmt
    &UIntToStrEdit, 					//uns16Frmt
    &UIntToStrEdit  					//uns32Frmt
};
//===================================================================
//Функции выполняемые при редактировании параметра
//===================================================================
void (*const ExecFuncParam[16])(void) = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    &setContrast
};
//===================================================================

const char noValidData[] = "---";

//#define MODE_HandlerEditMenuStatus          6
//#define MODE_HandlerMenuStatusDel           7

#define AddMODE_NONE                        0
#define AddMODE_RepalceStatus               1

struct {
//    u8 Mode;                            //Режим меню, тип меню
    u8 AdditionalMode;                  //Дополнительный режим
    u8 ParamAddMode;
    u8 SwitchMode;

}HandlerMenuPult_struct;

struct {
    u8 Pos;
}HandlerMainMenuPult_struct;

HandlerMenuJrn_struct_type HandlerMenuJrn_struct;
//===================================================================

//Full_DescriptorPrm_type DPDrive[MAX_STR_OTOBR_PRM];
//wm_type WMGlobal[MAX_STR_OTOBR_PRM];
//HeadOfFile_type HOFGlobal;

//===================================================================
//===================================================================
u8 lastDrvAdr = 0;
void SetLastDrvAdr(u8 Adr)
{
    lastDrvAdr = Adr;
}
//===================================================================
u8 GetLastDrvAdr(void)
{
    return lastDrvAdr;
}
//===================================================================
// Работа со статусом связи с ПЧ
u8 StatLinkDRV = 0;
u8 FlagResetGid = 0;

// Установить аварию связи
void SetErrLinkDRV(void){
    StatLinkDRV = 1;        // Установить флаг аварии
}

// Получить статус связи с ПЧ
u8 GetStatLinkDRV(void){
  if(StatLinkDRV){
    StatLinkDRV = 0;        // При наличии аварии сбросим бит аварии
    return 1;
  }
  else{
    return 0;
  }
}



// Мигаем лампой авария
void BlinkErr(void){
  LEDState_type   led;
  static u8       CountBlinkErr;
    
  CountBlinkErr++;
  if(CountBlinkErr > 1){
    CountBlinkErr = 0;
    ledOff(led);
    ledFaultOn(led);
  }
  else{
    ledOff(led);
  }
  LEDState = led;
}



//--------------------------------------
u32 newMode;
u32 prevMode;
//--------------------------------------
inline void SetModePrevMenu(void)
{
    newMode = prevMode;
}

inline void SetModeMenu(u32 mode)
{
    prevMode = newMode;
    newMode = mode;
}

inline u32 GetModeMenu(void)
{
    return newMode;
}
//--------------------------------------
inline void CallMenuPult(void)
{
    //HandlerMenuPultNew(0, LPC_OPIS, NULL);
    //HandlerMenuPult_struct.Mode = MODE_HandlerMenuPultNew;
    SetModeMenu(MODE_HandlerMenuPultAndDrive);

}
inline void CallMenuDrive(void)
{
    //HandlerMenuPult_struct.Mode = MODE_HandlerMenuChangeDriveForMenuPult;
    SetModeMenu(MODE_HandlerMenuChangeDriveForMenuPult);
    //HandlerMenuChangeDrive();
}
inline void CallMenuStatusReplace(void)
{
    //HandlerMenuPult_struct.Mode = MODE_HandlerMenuChangeDriveForMenuStatusReplace;
    SetModeMenu(MODE_HandlerMenuChangeDriveForMenuStatusReplace);
}
inline void CallHandlerMenuMasterSet(void)
{
    //HandlerMenuPult_struct.Mode = MODE_HandlerMenuTestKey;
    SetModeMenu(MODE_HandlerMenuSetup);
}

/*void CallMenuStatusEdit(void)
{
    HandlerMenuPult_struct.Mode = MODE_HandlerMenuStatusEdit;
}*/

uint8_t IsNvsaOld(void)
{ 
    HeadOfFile_type     HOF_L;
    HeadOfFileOld_type  HOF_L_Old;
    u16                 crc;
    u8                  netAdr;
    Drv_err_type        code;
    
    crc = sizeof (HOF_L);
    crc = sizeof (HOF_L_Old);
//    crc = sizeof (HOF_L_Spec_Old);
//    crc = sizeof (HOF_L_Spec);    
      
    
    gIsNvsaOld = 0xFF;
    netAdr = 1;
    code = ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], netAdr, 0, sizeof(HOF_L_Old),    fileDescrPrm, &HOF_L_Old, sizeof(HOF_L_Old));
    if (code == Ok_err)
    {
        crc = GetCrc(&HOF_L_Old, sizeof(HOF_L_Old) - sizeof(HOF_L_Old.HeaderCrc));
        if (crc == HOF_L_Old.HeaderCrc)
        {
            gIsNvsaOld = 0x01;
        }
    }
    code = ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], netAdr, 0, sizeof(HOF_L),    fileDescrPrm, &HOF_L, sizeof(HOF_L));
    if (code == Ok_err)
    {
        crc = GetCrc(&HOF_L, sizeof(HOF_L) - sizeof(HOF_L.HeaderCrc));
        if (crc == HOF_L.HeaderCrc)
        {
            gIsNvsaOld = 0x00;
        }
    }
    return gIsNvsaOld;
}
// Функция на проверку соответствия HOF привода, HOF сохраненному в ПЧ
u8 IsTekHOFLeng(void * BaseHOF)
{
    HeadOfFile_type     HOF_L;
    u16                 crc;
    u8                  netAdr;
    Drv_err_type        code;
    u8                  Err = 0;
    
    netAdr = 1;
    code = ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], netAdr, 0, sizeof(HOF_L),    fileDescrPrm, &HOF_L, sizeof(HOF_L));
    if ((code == Ok_err) && ((HOF_L.Pnt1LngTxt & 0x0000FFFF) != 0x0000FFFF) && HOF_L.QuantityLng && (HOF_L.QuantityLng < 5))
    {
        crc = GetCrc(&HOF_L, sizeof(HOF_L) - sizeof(HOF_L.HeaderCrc));
        if (crc == HOF_L.HeaderCrc)
        {

            #ifdef BLOCK_STM_MEMORY
            AT45RD(&HOF_L, BaseHOF, sizeof(HOF_L));
            #else            
            memcpy(&HOF_L, (void *)((u32)BaseHOF+STM32_FLASH_OFFSET), sizeof(HOF_L)); //  *!* Загрузка из памяти STM по сдвигу адреса                       
            #endif
            
            //AT45WR(BaseHOF, &HOF_L, sizeof(HOF_L));      ---   ИСКЛЮЧЕНА
            crc = GetCrc(&HOF_L, sizeof(HOF_L) - sizeof(HOF_L.HeaderCrc));
            if ((crc == HOF_L.HeaderCrc) && ((HOF_L.Pnt1LngTxt & 0x0000FFFF) != 0x0000FFFF) && HOF_L.QuantityLng && (HOF_L.QuantityLng < 5))
            {
                Err = 0x01;
            }
        }
    }
    return Err;
}


/**
 *@fn  - NvStructIntegrityTest 
 * @brief – функция проверки и исправления целостности структуры nv
 * @return - true - проверка успешна
 */
bool NvStructIntegrityTest()
{ 
    u8         dataBuffer[256];
    u32        dataCursor = 0;
    u32        readSize;        //размер куска прочитанных данных
    u32        nvDataSize;      //размер структуры пульта без crc
    u16        crcCalc = 0;     //расчётная контрольная сумма
    u16        crcStored;       //сохранённая в структуре контрольная сумма  
    
    //нельзя использовать sizeof по причине выравнивания структуры
    nvDataSize = (u32)&nv.crc - (u32)&nv;
    
    while(dataCursor < nvDataSize)
    {
        if(dataCursor + sizeof(dataBuffer) > nvDataSize)
            readSize = nvDataSize - dataCursor;
        else
            readSize = sizeof(dataBuffer);
      
        FM25RD(dataBuffer, (u32*)((u32)&nv + dataCursor), readSize);        
        crcCalc = CrcPice(dataBuffer, readSize, crcCalc);
        dataCursor += readSize;
    }
    
    //вычитываем crc во временный буфер
    FM25RD(&crcStored, &nv.crc, sizeof(nv.crc));    
    if(crcCalc != crcStored)
    {
        //контрольная сумма не совпала, очищаем структуру nv (не включая Bootloader)
        FM25Fill(&nv.ParamPult, 0xFF, nvDataSize - sizeof(nv.Bootloader));
        return false;
    }

    return true;
}


// **************************************************************************
//  обработчик меню
// **************************************************************************
#ifdef TEST_P24
void HandlerMenuPult(void)
{
    Test123();
}
#else
// **************************************************************************
//                      обработчик меню
// **************************************************************************
__noreturn
void HandlerMenuPult(void)
{
    OpisDrv_type OpisDrv;
    //void            *BaseHOF;
    keyName_type    Key;
    LEDState_type   led;
    u32             i;
    u16             cnt5Min;
    u16             cnt10Sec;
    //u32             gotoStatus;
    u8              err;

    Key     = NON_KEY;
    cnt5Min = (u16)GetSecCount();
    cnt10Sec = (u16)GetSecCount();

    //HandlerMenuPult_struct.Mode = MODE_HandlerMenuStatus;//MODE_HandlerMainMenuPult;
#ifndef TEST_MENU//TEST_P24
    //LevelAccess = MASTER_PASSW;
    AllPrmPnt->ADD_DRIVE = 1;
    //Проверяем тип описателя
    if (IsNvsaOld() == 0xFF)
    {
        MessageCancel("Connection\nerror!");
    }
    
    //проверка целостности структуры пульта
    NvStructIntegrityTest();
    
    FM25RD(&OpisDrv, &nv.OpisDrv[0], sizeof(OpisDrv_type));
    
    initMessPult(0);        
    
    ReqEtalonPassword();    // Опросить привод и прочитать пароли
    ReqLanguage();          // Тип языка? 
    ResetPassword();
    Password();
    InstallDrv(); 
    
    if (!IsTekHOFLeng(OpisDrv.LnkNetFDP.Lnk) || gIsNvsaOld)
    {   
        initMessPult(0);
    }
    else
    {
        TypeLanguage = OpisDrv.Lang;
        initMessPult(OpisDrv.LnkNetFDP.Lnk);
    }
    
#endif

#ifdef TEST_MENU
    LevelAccess = MASTER_PASSW;
    AllPrmPnt->ADD_DRIVE = 1;
    InstallDrv(); 
    SetModeMenu(MODE_HandlerMenuPultNewWithDrive);
    for (i = 0; i < MAXDRIVES_CUT; i++) 
    {
        if (ReadLnk(&HandlerMenuChangeDrive_struct.LnkNetFDP, i)) 
        {
            HandlerMenuChangeDrive_struct.AdrFRAM[i] = HandlerMenuChangeDrive_struct.LnkNetFDP.Lnk;
            HandlerMenuChangeDrive_struct.NetAdr[i] = HandlerMenuChangeDrive_struct.LnkNetFDP.NetAdr;
            HandlerMenuChangeDrive_struct.MaxDrv = i + 1;
        }
        else break;
    }
#else
    SetModeMenu(MODE_HandlerMenuStatus); //MODE_HandlerMenuPultNewWithDrive (MODE_HandlerMenuPultAndDrive);//
#endif
    
    while(1){
      ResetWD(tsk_lcd);
        OSSemPend(SemMenu, SYS_FOREVER, &err );
        Key = WaitKey(NULL);

        switch (GetModeMenu()) 
        {
            case MODE_HandlerMainMenuPult:
                HandlerMainMenuPult(Key);
                break;

            case MODE_HandlerMenuChangeDriveForMenuPult:
                HandlerMenuPult_struct.SwitchMode = MODE_HandlerMenuPultNewWithDrive;
                HandlerMenuChangeDrive(Key);
                break;

            case MODE_HandlerMenuChangeDriveForMenuStatusReplace:
                HandlerMenuPult_struct.SwitchMode = MODE_HandlerMenuStatusReplace;
                HandlerMenuChangeDrive(Key);
                break;

            case MODE_HandlerMenuPultAndDrive:
                i = 0;
                HandlerMenuPultAndDrive(Key, HandlerMenuChangeDrive_struct.NetAdr[i], FLASH_OPIS,
                    (void *)((u32)HandlerMenuChangeDrive_struct.AdrFRAM[i]));
                break;

            case MODE_HandlerMenuPultNewWithDrive:
                SetHandlerMenuDrive(&HandlerMenuChangeDrive_struct);
                i = 0;
                HandlerMenuPultAndDrive(Key, HandlerMenuChangeDrive_struct.NetAdr[i], FLASH_OPIS,
                    (void *)((u32)HandlerMenuChangeDrive_struct.AdrFRAM[i]));
                break;

            case MODE_HandlerMenuStatusReplace:
                HandlerMenuStatusReplace(Key);
                break;

            case MODE_HandlerMenuStatus:
                HandlerMenuStatus(Key);
                break;

            case MODE_HandlerMenuJrn:
                HandlerMenuJrn(Key);
                break;

            case MODE_HandlerMenuSetup:
                if ((getStatusWorkAll(MDPDrive.DPDrive[NUM_STATUS].wm.s32.Value) || 
                    getStatusCharge(MDPDrive.DPDrive[NUM_STATUS].wm.s32.Value)) &&
                    MDPDrive.DPDrive[NUM_STATUS].ValidData) 
                {
                    MessagePopUp(GetStringMess(MessageMess_PARAM_EDIT_STOP));
                    Key = KEY_LEFT;
                }
                HandlerMenuMasterSet(Key);
                break;

        }
        if (Key == NON_KEY)
        {
            if (299 < GetSecLeft(cnt5Min))
            {    //5минут
                cnt5Min = (u16)GetSecCount();
                //gotoStatus = 1;
                
                HandlerMenuPult_struct.SwitchMode = AddMODE_NONE;
                HandlerMenuPult_struct.AdditionalMode = AddMODE_NONE;
                
                ResetPassword();
                //memset(&HandlerMenuPultNew_struct, 0, sizeof(HandlerMenuPultNew_struct));
                HandlerMenuStatus_struct.GetOpisPrm = 0;
                SetModeMenu(MODE_HandlerMenuStatus);
            }
            if (9 <GetSecLeft(cnt10Sec))
            {    //5минут
                cnt10Sec = (u16)GetSecCount();
                if (MDPDrive.DPDrive[NUM_STATUS].ValidDataFirst == 1) 
                {
                    statusDRV_tipe      StatusDrv;
                    StatusDrv.all = (u16)MDPDrive.DPDrive[NUM_STATUS].wm.s32.Value;
                    MessageSetAlarm((MessageFlashFlag_type)(StatusDrv.all & 0xff), 3);
                }
            }
        }
        else
        {
            #warning А ЗАЧЕМ ТАК ЧАСТО ВЫЧИТЫВАТЬ ПАРОЛИ. КАЖДЫЙ РАЗ ПРИ НАЖАТИИ КЛАВИШИ ?!?!?!?!                    
            ReqEtalonPassword();                                                    // Опросить привод и прочитать пароли
            Password();                                                             //Проверка пароля
            cnt5Min = (u16)GetSecCount();
            cnt10Sec = (u16)GetSecCount();
        }
    }
}
#endif
// **************************************************************************
//  Обработчик журналов
// **************************************************************************
//RecJrn_type     RecJrn;
//MenuRecJrn_type  MenuRecJrn;

/*#define FAIL_JRN        0
#define WARN_JRN        1

#define FILE_FAIL_JRN   10
#define FILE_WARN_JRN   11*/

#define CNT_JRN         32

#define MODE_ID_JRN     0
#define MODE_PRM_JRN    1
//===================================================================
inline void *GetAdrJrnInFile(u32 numRec, u32 szStruct)
{
//    return (void *)(((u32)((RecJrn_type *)(0) + numRec) + sizeof(Index_type)) >> 1);
    //return (void *)(((u32)((RecJrn_type *)(0) + numRec) + sizeof(Index_type) >> 1));
    return (void *)(szStruct * numRec + (sizeof(Index_type) >> 1));
}
//===================================================================
//Поиск в журналах начало описателей параметров с токами и т.д.
//===================================================================
typedef struct {
    u16     indexDescrPrm;
    u16     indexJrnPrm;
    u16     qntPrm;
}BeginPrm_type;
//===================================================================
//===================================================================
//void OutPrmJrn(JrnData_type *RecJrn, u16 posCursorPrm, u16 posPrm, BeginPrm_type *beginPrm)
void OutPrmJrn(u16 *RecJrn, u16 qntDisplayJrn, u16 posCursorPrm, u16 posPrm)
{
    Full_DescriptorPrm_type     DPDrive;
    wm_type                     *wm;
    u16                         indexPrm, indexDescr;
    //u16         beginIndexPrm, beginIndexDescr;
    char                        Name[16];
    u16                         yPos, i;
    invers_type                 inv;
    HeadOfFile_type             HOF;

    //beginIndexDescr = beginPrm->indexDescrPrm + posPrm - posCursorPrm;
    //beginIndexPrm = beginPrm->indexJrnPrm + posPrm - posCursorPrm;
    if (posPrm < posCursorPrm){
        indexDescr = qntDisplayJrn - posCursorPrm + posPrm + 1;
    }
    else{
        indexDescr = posPrm - posCursorPrm + 1;    //Не учитывается 0-й параметр - номер файла
    }
    indexPrm = 0;

    for (i = 1; i < indexDescr; i++) {
        InitDP(&DPDrive,
               HandlerMenuJrn_struct.BaseHOF,
               DPGlobal[i],
               1);
        ModbusMakeOpis(&DPDrive, DO_NONE_UST);
        if (DPDrive.wm.kolvShort == 2) {
            indexPrm += 2;
        }
        else{
            indexPrm += 1;
        }
    }
    yPos = 16;
    for(i = 0; i < qntDisplayJrn && i < MAX_STR_OTOBR_PRM; i++, indexDescr++) {
        if (indexDescr > qntDisplayJrn) {
            indexDescr = 1;
            indexPrm = 0;
        }
        InitDP(&DPDrive, HandlerMenuJrn_struct.BaseHOF, DPGlobal[indexDescr], 1);
        ModbusMakeOpis(&DPDrive, DO_NONE_UST);
        if (DPDrive.wm.kolvShort == 2) {
            DPDrive.wm.s32.Value = (RecJrn[indexPrm]) + (RecJrn[indexPrm + 1] << 16);//RecJrn[indexPrm]; //<< 16);// + *RecJrn[j][indexPrm + 1] ;
            indexPrm += 2;
        }
        else{
            DPDrive.wm.s32.Value = RecJrn[indexPrm];//RecJrn[i].jrnData;//[indexPrm];
            indexPrm += 1;
        }

        if (i == posCursorPrm) {
            inv = STR_INV;
        }
        else{
            inv = NO_INV;
        }
        //sprintf(&StrOut[0], "%s", DPDrive.DP.Name);
        
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&HOF, HandlerMenuJrn_struct.BaseHOF, sizeof(HeadOfFile_type));        
        #else            
        memcpy(&HOF, (void *)((u32)HandlerMenuJrn_struct.BaseHOF+STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                  
        #endif
        
        getNamePrm(HandlerMenuJrn_struct.BaseHOF, &HOF, &DPDrive.DP, &Name[0]);
        sprintf(&StrOut[0], "%s", Name);
        StrOutFont(MENUX, yPos, inv, &StrOut[0], &fontNormal, leftAlign );
        wm = &DPDrive.wm;
        ParamToStr(HandlerMenuJrn_struct.BaseHOF,
                   &DPDrive,
                   wm,
                   &StrOut[0]);
        yPos += INCY;
        StrOutFont(MENUX, yPos, inv, &StrOut[0], &fontNormal, leftAlign );
        yPos += INCY;
    }
}
//===================================================================
//===================================================================
void OutIdJrn(JrnData_type *RecJrn, u16 qntDisplayJrn, u16 posCursor)
{
    wm_type         *wm;
    u16             indexPrm;
    u16             numRec, indexMDP, numPrm, yPos;
    invers_type     inv;
    char            Name[16];
    HeadOfFile_type HOF;

    yPos = 16;

    for (numRec = 0; numRec < qntDisplayJrn; numRec++) {//проход по журнальным записях вычитанных из привода
        indexPrm    = 0;
        numPrm      = 1;                                //Начинаем с 1-го параметра журнала, 0-й  это номер файла
        indexMDP    = 0;
        do{
            //================================================================================
            //Инициализация 2-х дескрипторов описателей меню журнала (код события и дата)
            //================================================================================
            InitDP(&MDPDrive.DPDrive[indexMDP], HandlerMenuJrn_struct.BaseHOF, DPGlobal[numPrm], 1);
            //================================================================================
            //Заполнение wm структуры
            //================================================================================
            ModbusMakeOpis(&MDPDrive.DPDrive[indexMDP], DO_NONE_UST);
            if (MDPDrive.DPDrive[indexMDP].wm.kolvShort == 2) {
                MDPDrive.DPDrive[indexMDP].wm.s32.Value = (RecJrn->jrnData[numRec][indexPrm]) + (RecJrn->jrnData[numRec][indexPrm + 1] << 16);//RecJrn[indexPrm]; //<< 16);// + *RecJrn[j][indexPrm + 1] ;
                indexPrm += 2;
            }
            else{
                MDPDrive.DPDrive[indexMDP].wm.s32.Value = RecJrn->jrnData[numRec][indexPrm];//RecJrn[i].jrnData;//[indexPrm];
                indexPrm += 1;
            }
            if (MDPDrive.DPDrive[indexMDP].DP.DefaultSet != 0) {                   //Признак видимости параметра в структуре группы
            //================================================================================
            //Вывод на экран
            //================================================================================
                if (numRec == posCursor) {
                    inv = STR_INV;
                }
                else{
                    inv = NO_INV;
                }
                
                #ifdef BLOCK_STM_MEMORY
                AT45RD(&HOF, HandlerMenuJrn_struct.BaseHOF, sizeof(HeadOfFile_type));
                #else            
                memcpy(&HOF, (void *)((u32)HandlerMenuJrn_struct.BaseHOF+STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
                #endif
               
                getNamePrm(HandlerMenuJrn_struct.BaseHOF, &HOF, &MDPDrive.DPDrive[indexMDP].DP, &Name[0]);
                sprintf(&StrOut[0], "%s", Name);
                StrOutFont(MENUX, yPos, inv, &StrOut[0], &fontNormal, leftAlign );
                wm = &MDPDrive.DPDrive[indexMDP].wm;
                ParamToStr(HandlerMenuJrn_struct.BaseHOF,
                           &MDPDrive.DPDrive[indexMDP], wm, &StrOut[0]);

                yPos += INCY;
                StrOutFont(MENUX, yPos, inv, &StrOut[0], &fontNormal, leftAlign );
                yPos += INCY;

                indexMDP++;
            }
            numPrm++;
        }while (indexMDP < 2);
    }
}
//===================================================================
//===================================================================
void FindBeginPrmJrn(void *BaseHOF, BeginPrm_type *beginPrm, u16 szJrn)
{
    Full_DescriptorPrm_type DPDrive;
    wm_type                 *wm;
    u16                     indexMDP;

    wm = &DPDrive.wm;
    memset(beginPrm, 0, sizeof(BeginPrm_type));
    indexMDP = 0;
    beginPrm->qntPrm = szJrn - 1;           //-1 потомучто 0-й п-р номер файла

    beginPrm->indexDescrPrm = 1;            //0-й дескриптор номер файла
    do{
        InitDP(&DPDrive,
               BaseHOF,
               DPGlobal[beginPrm->indexDescrPrm],
               1);
        ModbusMakeOpis(&DPDrive, DO_NONE_UST);

        if (wm->kolvShort == 1) {
            beginPrm->indexJrnPrm += 1;
        }
        else{
            beginPrm->indexJrnPrm += 2;
        }

        beginPrm->indexDescrPrm++;
        if (DPDrive.DP.DefaultSet != 0) {//Признак видимости параметра журнала
            indexMDP++;
        }
        beginPrm->qntPrm--;
    }while (indexMDP < 2);

}
// **************************************************************************
//  Обработчик журналов
// !!! неотьемлимая часть меню привода
// **************************************************************************
void HandlerMenuJrn(u16 Key)
{
    static struct{
        u16         posId;
        u16         posCursorId;
        u16         posPrm;
        u16         posCursorPrm;
        struct{
            u16     getIndex        :1;
            u16     modeJrn         :1;
        };
        u16         numFileJrn;
        u16         cntPrm;
    }MenuJrn = {0};

    DescriptorMenu_type                 DM;
    HeadOfFile_type                     HOF;
    void                                *AdrInFile;
    static Index_type                   IndexJrn;
    static JrnData_type                 recJrn;
//    static BeginPrm_type                beginPrm;
    Full_DescriptorPrm_type             DPDrive;

    u32                                 numRec;
    u32                                 numFileJrn;
    u32                                 netAdr;
    u32                                 crc;
    u32                                 refreshJrn;
    u32                                 tmp, i, j;
    u32                                 cntPrm;
//    u32                                 szPrm;

    //Вычитка индексов
    refreshJrn = 0;
    netAdr = HandlerMenuJrn_struct.netAdr;

    if (0 == MenuJrn.getIndex) {
        tmp = 1;
        //========================
        //Чтение заголовка файла
        //========================

        #ifdef BLOCK_STM_MEMORY
        AT45RD(&HOF, HandlerMenuJrn_struct.BaseHOF, sizeof(HeadOfFile_type));
        #else            
        memcpy(&HOF, (void *)((u32)HandlerMenuJrn_struct.BaseHOF+STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                   
        #endif

        //========================
        //Чтение описателя группы
        //========================
        if (HandlerMenuJrn_struct.BaseDM)
        {
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&DM, HandlerMenuJrn_struct.BaseDM, sizeof(DescriptorMenu_type));
            #else 
            memcpy(&DM, (void *)((u32)HandlerMenuJrn_struct.BaseDM+STM32_FLASH_OFFSET), sizeof(DescriptorMenu_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
            #endif
        }
        else
        {
            memcpy(&DM, &HandlerMenuJrn_struct.DM, sizeof(DescriptorMenu_type));
            //DM = HandlerMenuJrn_struct.BaseDM;
        }
        //========================
        //Поиск параметров и инициализация массива указателей на DP DPGlobal
        //========================
        cntPrm = FindVisibleParam(&HOF,&DM,HandlerMenuJrn_struct.BaseHOF, HandlerMenuChangeDrive_struct.Pos);
        MenuJrn.cntPrm = cntPrm - 1; //Без 0-го параметра
        if (cntPrm < 2) {
            SetModePrevMenu();
            DeInitAllDP();
            memset(&MenuJrn, 0, sizeof(MenuJrn));
            return;
        }
        //========================
        //Описатель 1-го параметра (номер файла)
        //========================
        InitDP(&DPDrive, HandlerMenuJrn_struct.BaseHOF, DPGlobal[0], netAdr);
        numFileJrn = MenuJrn.numFileJrn = DPDrive.DP.DefaultSet;

        if (Ok_err == ReadJrnIndex(&MsgReqDrvIn[MSG_ID],
                                   netAdr,
                                   numFileJrn,
                                   &IndexJrn)){                 //Чтение индексов журналов
            for (i = 0; i < sizeof(Index_type) >> 1; i++)
                ((u16*)&IndexJrn)[i] = swapU16(((u16*)&IndexJrn)[i]);
            crc = GetCrc(&IndexJrn, sizeof(Index_type) - sizeof(IndexJrn.Crc));
            if (crc == IndexJrn.Crc) {
                tmp = 0;
                MenuJrn.getIndex = 1;
            }
            else{
                MessageCancel(GetStringMess(MessageMess_NO_JORNAL));
            }
        }
        else{
           MessageCancel(GetStringMess(MessageMess_ERROR_CONECT));
        }
        if (tmp != 0) {
            SetModePrevMenu();
            memset(&MenuJrn, 0, sizeof(MenuJrn));
            return;
        }
        refreshJrn = 1;
    }

    //Обработка кнопок
    switch (Key) {
        case KEY_RIGHT:
            if (MenuJrn.modeJrn == MODE_ID_JRN) {
                MenuJrn.modeJrn = MODE_PRM_JRN;
                refreshJrn = 1;
                //FindBeginPrmJrn(HandlerMenuJrn_struct.BaseHOF, &beginPrm, MenuJrn.cntPrm);//Поиск где в группе начинаются параметры журналов и вычисление количества параметров
            }
            break;
        case KEY_LEFT:          
            if (MenuJrn.modeJrn == MODE_PRM_JRN) {
                MenuJrn.modeJrn = MODE_ID_JRN;
                refreshJrn = 1;
                MenuJrn.posPrm = 0;
                MenuJrn.posCursorPrm = 0;
            }
            else{
                SetModePrevMenu();
                DeInitAllDP();
                memset(&MenuJrn, 0, sizeof(MenuJrn));
                return;
            }
            break;
        case KEY_UP:
            refreshJrn = 1;
            if (MenuJrn.modeJrn == MODE_ID_JRN){
                if (MenuJrn.posId > 0) MenuJrn.posId--;
                else MenuJrn.posId = IndexJrn.quantRec - 1;//CNT_JRN - 1;
                if (MenuJrn.posCursorId > 0) MenuJrn.posCursorId--;
            }
            else{
                if (MenuJrn.posPrm > 0) MenuJrn.posPrm--;
                else MenuJrn.posPrm = MenuJrn.cntPrm - 1;//CNT_JRN - 1;
                if (MenuJrn.posCursorPrm > 0) MenuJrn.posCursorPrm--;
            }
            break;
        case KEY_DOWN:
            refreshJrn = 1;
            if (MenuJrn.modeJrn == MODE_ID_JRN){
                MenuJrn.posId++;
                if (MenuJrn.posId >= IndexJrn.quantRec)
                    MenuJrn.posId = 0;

                if (MenuJrn.posCursorId < MAX_STR_OTOBR_JRN - 1 &&
                    MenuJrn.posCursorId < IndexJrn.quantRec - 1) {
                    MenuJrn.posCursorId++;
                }
            }
            else{
                MenuJrn.posPrm++;
                if (MenuJrn.posPrm >= MenuJrn.cntPrm)
                    MenuJrn.posPrm = 0;

                if (MenuJrn.posCursorPrm < MAX_STR_OTOBR_PRM - 1 &&
                    MenuJrn.posCursorPrm < MenuJrn.cntPrm - 1) {
                    MenuJrn.posCursorPrm++;
                }
            }
            break;
    }

    if (0 != refreshJrn) {
        //========================
        //Вывод на экран идентификаторов журнала
        //========================
        GrafBffClear();
        if (MenuJrn.modeJrn == MODE_ID_JRN) {
            numRec = MenuJrn.posId - MenuJrn.posCursorId;
            if (numRec >= IndexJrn.quantRec) {
                numRec = IndexJrn.quantRec - 1;
            }
        //========================
        //Описатель 1-го параметра (номер файла)
        //========================
            numFileJrn = MenuJrn.numFileJrn;
        //========================
        //Чтение журналов из привода
        //========================
            for (j = 0; j < MAX_STR_OTOBR_JRN && j < IndexJrn.numRec; j++) {
                AdrInFile = GetAdrJrnInFile(numRec, IndexJrn.sizeRec);
                ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], netAdr,
                                AdrInFile, IndexJrn.sizeRec << 1,
                                numFileJrn, &recJrn.jrnData[j][0],
                                sizeof(recJrn.jrnData[j][0]));
                for (i = 1; i < IndexJrn.sizeRec; i++){
                    recJrn.jrnData[j][i] = swapU16(recJrn.jrnData[j][i]);
                }
                recJrn.jrnData[j][0] = recJrn.jrnData[j][0] >> 8;       //В нулевом элементе выделяем словосостояние
                numRec++;
                if (numRec >= IndexJrn.numRec) numRec = 0;
            }
        //========================
        //Скроллинг
        //========================
            if (IndexJrn.numRec > MAX_STR_OTOBR_JRN)
                ScrollBarVert(IndexJrn.numRec, MAX_STR_OTOBR_JRN * 4, 16, MenuJrn.posId, 156, 16);
        //========================
        //Вывод идентификатора журанла
        //========================
            OutIdJrn(&recJrn,
                     (IndexJrn.quantRec < MAX_STR_OTOBR_JRN) ? IndexJrn.quantRec : MAX_STR_OTOBR_JRN,
                     MenuJrn.posCursorId);
        }
        else if (MenuJrn.modeJrn == MODE_PRM_JRN) {
        //========================
        //Скроллинг
        //========================
            if (MenuJrn.cntPrm > MAX_STR_OTOBR_PRM)
                ScrollBarVert(MenuJrn.cntPrm, MAX_STR_OTOBR_PRM * 2, 16, MenuJrn.posPrm, 156, 16);
            OutPrmJrn(&recJrn.jrnData[MenuJrn.posCursorId][0], MenuJrn.cntPrm, MenuJrn.posCursorPrm, MenuJrn.posPrm);
        }
        else{
        }
        GrfOut();
    }
}
//**************************************************************************
//Формирование статусного меню
//**************************************************************************
struct{
    u16 Pos;
    u16 Cursor;
}HandlerMenuStatusReplace_struct;
//===================================================================
void HandlerMenuStatusReplace(u16 Key)
{
    invers_type             Inv;
    Status_type             StatusRef[COUNT_STATUS];
    u8                      xPos, count;
    
    FM25RD(&StatusRef, &nv.OpisDrv[HandlerMenuChangeDrive_struct.Pos].StatusRef[0], sizeof(StatusRef));     //Читаем статус для первого установленного привода

    switch (Key){
        case KEY_DOWN:
            if (HandlerMenuStatusReplace_struct.Pos < COUNT_STATUS - 1)
            { 
                HandlerMenuStatusReplace_struct.Pos++;
                if (HandlerMenuStatusReplace_struct.Cursor < 2)
                {
                    HandlerMenuStatusReplace_struct.Cursor++;
                }
            }
            break;

        case KEY_UP:
            if (HandlerMenuStatusReplace_struct.Pos > 0)
            {
                HandlerMenuStatusReplace_struct.Pos--;
                if (HandlerMenuStatusReplace_struct.Cursor > 0)
                {
                    HandlerMenuStatusReplace_struct.Cursor--;
                }
            }
            break;

        case KEY_RIGHT:
            SetModeMenu(MODE_HandlerMenuPultNewWithDrive);
            HandlerMenuPult_struct.AdditionalMode = AddMODE_RepalceStatus;                            //Режим выбора заменяемого статусного параметра
            HandlerMenuPult_struct.ParamAddMode = (u8)HandlerMenuStatusReplace_struct.Pos;      //Номер параметра
            HandlerMenuPult_struct.SwitchMode = AddMODE_NONE;
            HandlerMenuPultNew_struct.Mode = MODE_MENU;
            HandlerMenuPultNew_struct.GetOpisMenu = 0;
            SetModeMenu(MODE_HandlerMenuPultNewWithDrive);
            break;

        case KEY_LEFT:
            //HandlerMenuPult_struct.Mode = MODE_HandlerMainMenuPult;
            /*if (HandlerMenuChangeDrive_struct.MaxDrv > 1){                               //Если установленных приводов больше чем 1, то возвращаемся в меню выбора привода
                SetModeMenu(MODE_HandlerMenuChangeDriveForMenuPult);
            }
            else{
                //SetModeMenu(MODE_HandlerMainMenuPult);
                SetModeMenu(MODE_HandlerMenuPultNewWithDrive);
            }*/
            HandlerMenuPultNew_struct.GetOpisMenu = 0;
            HandlerMenuPult_struct.SwitchMode = AddMODE_NONE;
            HandlerMenuPult_struct.AdditionalMode = AddMODE_NONE;
            SetModeMenu(MODE_HandlerMenuPultNewWithDrive);
            memset(&HandlerMenuStatusReplace_struct, 0, sizeof(HandlerMenuStatusReplace_struct));
            break;

        case KEY_ENTER:
            //HandlerMenuPult_struct.Mode = MODE_HandlerMenuPultNewWithDrive;//MODE_HandlerMenuChangeDrive;   //Переходим в меню привода для которого меняется статусный параметр
            SetModeMenu(MODE_HandlerMenuPultNewWithDrive);
            HandlerMenuPult_struct.AdditionalMode = AddMODE_RepalceStatus;                            //Режим выбора заменяемого статусного параметра
            HandlerMenuPult_struct.ParamAddMode = (u8)HandlerMenuStatusReplace_struct.Pos;      //Номер параметра
            
            HandlerMenuPult_struct.SwitchMode = AddMODE_NONE;
            HandlerMenuPultNew_struct.Mode = MODE_MENU;
            HandlerMenuPultNew_struct.GetOpisMenu = 0;
            
            SetModeMenu(MODE_HandlerMenuPultNewWithDrive);
            //memset(&HandlerMenuStatusReplace_struct, 0, sizeof(HandlerMenuStatusReplace_struct));
            break;

        case KEY_ESC:
            //HandlerMenuPult_struct.Mode = MODE_HandlerMainMenuPult;
            /*if (HandlerMenuChangeDrive_struct.MaxDrv > 1){                               //Если установленных приводов больше чем 1, то возвращаемся в меню выбора привода
                SetModeMenu(MODE_HandlerMenuChangeDriveForMenuPult);
            }
            else{
                //SetModeMenu(MODE_HandlerMainMenuPult);
                SetModeMenu(MODE_HandlerMenuPultNewWithDrive);
            }*/
            HandlerMenuPult_struct.SwitchMode = AddMODE_NONE;
            HandlerMenuPult_struct.AdditionalMode = AddMODE_NONE;
            
            SetModeMenu(MODE_HandlerMenuPultNewWithDrive);
            memset(&HandlerMenuStatusReplace_struct, 0, sizeof(HandlerMenuStatusReplace_struct));
            break;
        default: break;
    }

    GrafBffClear();

    sprintf(&StrOut[0], "%s", GetStringTitle(MessageTitul_EDIT));
    StrOutFont(5, 0, NO_INV, &StrOut[0], &fontNormal, leftAlign );
    
    xPos = 65;
    for (count = HandlerMenuStatusReplace_struct.Pos - HandlerMenuStatusReplace_struct.Cursor; 
         count < HandlerMenuStatusReplace_struct.Pos - HandlerMenuStatusReplace_struct.Cursor + 3; count++)
    {
        sprintf(&StrOut[0], "[%d, %d]", StatusRef[count].NumGrp, StatusRef[count].NumPrm);
        if (HandlerMenuStatusReplace_struct.Cursor == count - (HandlerMenuStatusReplace_struct.Pos - HandlerMenuStatusReplace_struct.Cursor))   Inv = INV;
        else                                            Inv = NO_INV;
        StrOutFont(xPos, 35 + 40 * (count - (HandlerMenuStatusReplace_struct.Pos - HandlerMenuStatusReplace_struct.Cursor)), Inv, &StrOut[0], &fontNormal, leftAlign );
        sprintf(&StrOut[0], "%d)", count + 1);
        StrOutFont(2, 35 + 40 * (count - (HandlerMenuStatusReplace_struct.Pos - HandlerMenuStatusReplace_struct.Cursor)), NO_INV, &StrOut[0], &fontNormal, leftAlign );
    }
    
    ScrollBarVert(COUNT_STATUS, 8, 16, HandlerMenuStatusReplace_struct.Pos, 156, 16); 
    WordStatusForLed();
    GrfOut();
}

//===================================================================
inline void FillingStatusMDP(Status_type *StatusRef, void *HOF, void *DM, void *DP, void *pHOF)
{
    u16                     i;
    DescriptorPrm_type      *pDP;
    DescriptorPrmOld_type   *pDPOld;
    DescriptorMenu_type     *pDM;
    DescriptorMenuOld_type  *pDMOld;
    HeadOfFile_type         *pHOFNew;
    HeadOfFileOld_type      *pHOFOld;
    
    if (gIsNvsaOld)
    {
        pDPOld = DP;
        pDMOld = DM;
        pHOFOld = HOF;
        for (i = 0; i < COUNT_STATUS; i++) 
        {
            if (FindGroup(pHOFOld, pDMOld, pHOF, StatusRef[i].NumGrp))
            {
                if(MDPDrive.DPDrive[i].BaseDP = FindParam(pHOFOld, pDMOld, pDPOld, pHOF, StatusRef[i].NumPrm))
                {
                    MDPDrive.DPDrive[i].AdrNet = HandlerMenuStatus_struct.NetAdr[HandlerMenuStatus_struct.CurLnk];
                    MDPDrive.DPDrive[i].BaseHOF = pHOF;
                    memcpy(&MDPDrive.DPDrive[i].DPOld, pDPOld, sizeof(DescriptorPrmOld_type));
                    MDPDrive.DPDrive[i].NumParam = i;
                    MDPDrive.DPDrive[i].ValidData = 0;
                    MDPDrive.DPDrive[i].ValidDataFirst = 0;
                    MDPDrive.DPDrive[i].Ignore = 1;
                    MDPDrive.DPDrive[i].msgReqDrvIn.Busy = 0;
                }
            }
        }
        //Статус
        if (FindGroup(pHOFOld, pDMOld, pHOF, GRP_STATUS))
        {
            if(MDPDrive.DPDrive[NUM_STATUS].BaseDP = FindParam(pHOFOld, pDMOld, pDPOld, pHOF, PRM_STATUS))
            {
                MDPDrive.DPDrive[NUM_STATUS].AdrNet          = HandlerMenuStatus_struct.NetAdr[HandlerMenuStatus_struct.CurLnk];
                MDPDrive.DPDrive[NUM_STATUS].BaseHOF         = pHOF;
                memcpy(&MDPDrive.DPDrive[NUM_STATUS].DPOld, pDPOld, sizeof( DescriptorPrmOld_type));
                MDPDrive.DPDrive[NUM_STATUS].NumParam        = NUM_STATUS;
                MDPDrive.DPDrive[NUM_STATUS].ValidData       = 0;
                MDPDrive.DPDrive[NUM_STATUS].ValidDataFirst  = 0;
                MDPDrive.DPDrive[NUM_STATUS].Ignore          = 1;
                MDPDrive.DPDrive[NUM_STATUS].msgReqDrvIn.Busy= 0;
            }
            if(MDPDrive.DPDrive[NUM_STATUS_2].BaseDP = FindParam(pHOFOld, pDMOld, pDPOld, pHOF, PRM_STATUS_2))
            {
                MDPDrive.DPDrive[NUM_STATUS_2].AdrNet          = HandlerMenuStatus_struct.NetAdr[HandlerMenuStatus_struct.CurLnk];
                MDPDrive.DPDrive[NUM_STATUS_2].BaseHOF         = pHOF;
                memcpy(&MDPDrive.DPDrive[NUM_STATUS_2].DPOld, pDPOld, sizeof( DescriptorPrmOld_type));
                MDPDrive.DPDrive[NUM_STATUS_2].NumParam        = NUM_STATUS_2;
                MDPDrive.DPDrive[NUM_STATUS_2].ValidData       = 0;
                MDPDrive.DPDrive[NUM_STATUS_2].ValidDataFirst  = 0;
                MDPDrive.DPDrive[NUM_STATUS_2].Ignore          = 1;
                MDPDrive.DPDrive[NUM_STATUS_2].msgReqDrvIn.Busy= 0;
            }
        }
    }
    else
    {
        pDP = DP;
        pDM = DM;
        pHOFNew = HOF;
        for (i = 0; i < COUNT_STATUS; i++) 
        {
            if (FindGroup(pHOFNew, pDM, pHOF, StatusRef[i].NumGrp))
            {
                if(MDPDrive.DPDrive[i].BaseDP = FindParam(pHOFNew, pDM, pDP, pHOF, StatusRef[i].NumPrm))
                {
                    MDPDrive.DPDrive[i].AdrNet = HandlerMenuStatus_struct.NetAdr[HandlerMenuStatus_struct.CurLnk];
                    MDPDrive.DPDrive[i].BaseHOF = pHOF;
                    memcpy(&MDPDrive.DPDrive[i].DP, pDP, sizeof(DescriptorPrm_type));
                    MDPDrive.DPDrive[i].NumParam = i;
                    MDPDrive.DPDrive[i].ValidData = 0;
                    MDPDrive.DPDrive[i].ValidDataFirst = 0;
                    MDPDrive.DPDrive[i].Ignore = 1;
                    MDPDrive.DPDrive[i].msgReqDrvIn.Busy = 0;
                }
            }
        }
        //Статус
        if (FindGroup(pHOFNew, pDM, pHOF, GRP_STATUS))
        {
            if(MDPDrive.DPDrive[NUM_STATUS].BaseDP = FindParam(pHOFNew, pDM, pDP, pHOF, PRM_STATUS))
            {
                MDPDrive.DPDrive[NUM_STATUS].AdrNet          = HandlerMenuStatus_struct.NetAdr[HandlerMenuStatus_struct.CurLnk];
                MDPDrive.DPDrive[NUM_STATUS].BaseHOF         = pHOF;
                memcpy(&MDPDrive.DPDrive[NUM_STATUS].DP, pDP, sizeof( DescriptorPrm_type));
                MDPDrive.DPDrive[NUM_STATUS].NumParam        = NUM_STATUS;
                MDPDrive.DPDrive[NUM_STATUS].ValidData       = 0;
                MDPDrive.DPDrive[NUM_STATUS].ValidDataFirst  = 0;
                MDPDrive.DPDrive[NUM_STATUS].Ignore          = 1;
                MDPDrive.DPDrive[NUM_STATUS].msgReqDrvIn.Busy= 0;
            }
            if(MDPDrive.DPDrive[NUM_STATUS_2].BaseDP = FindParam(pHOFNew, pDM, pDP, pHOF, PRM_STATUS_2))
            {
                MDPDrive.DPDrive[NUM_STATUS_2].AdrNet          = HandlerMenuStatus_struct.NetAdr[HandlerMenuStatus_struct.CurLnk];
                MDPDrive.DPDrive[NUM_STATUS_2].BaseHOF         = pHOF;
                memcpy(&MDPDrive.DPDrive[NUM_STATUS_2].DP, pDP, sizeof( DescriptorPrm_type));
                MDPDrive.DPDrive[NUM_STATUS_2].NumParam        = NUM_STATUS_2;
                MDPDrive.DPDrive[NUM_STATUS_2].ValidData       = 0;
                MDPDrive.DPDrive[NUM_STATUS_2].ValidDataFirst  = 0;
                MDPDrive.DPDrive[NUM_STATUS_2].Ignore          = 1;
                MDPDrive.DPDrive[NUM_STATUS_2].msgReqDrvIn.Busy= 0;
            }
        }
    }
}
//===================================================================


//===================================================================
u16 TestValidDrv(u8 netAdr, u16 numCurOpis)
{
    HeadOfFile_type         HOFInst;
    HeadOfFileOld_type      HOFInstOld;
    HeadOfFile_type         HOFDrv;
    HeadOfFileOld_type      HOFDrvOld;
    MsgReqDrvIn_type        tvdMsgReqDrvIn;
    FileFunctional_type     FunctDrv;
    OpisDrv_type            OpisDrv;
    Drv_err_type            code;
    u16                     crc;
    u16                     tmp;
    u8                      i;

    if (gIsNvsaOld)
    {
        memset(&HOFDrvOld, 0, sizeof(HeadOfFileOld_type));
    }
    else
    {
        memset(&HOFDrv, 0, sizeof(HeadOfFile_type));
    }

    if (numCurOpis < MAXDRIVES_CUT)
    {   
        i = 10;
        while (i)
        {
            if (gIsNvsaOld)
            {
                code = ReadFileFromDrv(&tvdMsgReqDrvIn, netAdr, 0, sizeof(HOFDrvOld),    fileDescrPrm, &HOFDrvOld, sizeof(HOFDrvOld));
            }
            else
            {
                code = ReadFileFromDrv(&tvdMsgReqDrvIn, netAdr, 0, sizeof(HOFDrv),    fileDescrPrm, &HOFDrv, sizeof(HOFDrv));
            }
            code |= ReadVerFunct(&tvdMsgReqDrvIn, netAdr, fileFunctional, &FunctDrv);
            if (code == Ok_err)
            {
                break;
            }
            i--;
        }

        if (code == Ok_err)
        {
            if (gIsNvsaOld)
            {
                crc = GetCrc(&HOFDrvOld, sizeof(HOFDrvOld) - sizeof(HOFDrvOld.HeaderCrc));
                if (crc != HOFDrvOld.HeaderCrc)
                {
                    return 0;
                }
            }
            else
            {
                crc = GetCrc(&HOFDrv, sizeof(HOFDrv) - sizeof(HOFDrv.HeaderCrc));
                if (crc != HOFDrv.HeaderCrc)
                {
                    return 0;
                }
            }
            tmp = swapU16(FunctDrv.curFunct.functMask);
            crc = GetCrc(&tmp, sizeof(FunctDrv) - sizeof(FunctDrv.crc));
            if (crc != FunctDrv.crc){
                return 0;
            }
        }
        else{
            return 0;
        }
        FM25RD(&OpisDrv, &nv.OpisDrv[numCurOpis], sizeof(OpisDrv_type));
        if (FunctDrv.crc != OpisDrv.Functional.crc || FunctDrv.curFunct.functMask != OpisDrv.Functional.curFunct.functMask)
        {
            return 0;
        }
        if (gIsNvsaOld)
        {
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&HOFInstOld, OpisDrv.LnkNetFDP.Lnk, sizeof(HOFInstOld)); // Чтение CRC HOF
            #else            
            memcpy(&HOFInstOld, (void *)((u32)OpisDrv.LnkNetFDP.Lnk+STM32_FLASH_OFFSET), sizeof(HOFInstOld)); //  *!* Загрузка из памяти STM по сдвигу адреса           
            #endif

            if (HOFInstOld.HeaderCrc != HOFDrvOld.HeaderCrc)
            {
                return 0;
            }
        }
        else
        {
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&HOFInst, OpisDrv.LnkNetFDP.Lnk, sizeof(HOFInst));
            #else            
            memcpy(&HOFInst, (void *)((u32)OpisDrv.LnkNetFDP.Lnk+STM32_FLASH_OFFSET), sizeof(HOFInst)); //  *!* Загрузка  из памяти STM по сдвигу адреса           
            #endif
            
            if (HOFInst.HeaderCrc != HOFDrv.HeaderCrc)
            {
                return 0;
            }
        }
        return 1;
    }
    else{
        return 0;
    }
}
//===================================================================
//===================================================================
struct {
    u8 NumIcon;
    //u8 NumStop;
    //u8 NumAvar;
    u8 Delay;
}IconStatus_struct;

#define DELAY_ICON 6

// Сохраняем полные опесатели во временный буффер
void saveToTemp(Full_DescriptorPrm_type     *DPDriveTemp)
{
    u8  i;
    
    for (i = 0; i < MAX_REQ_PRM + 1; i++) 
    {
        memcpy(&DPDriveTemp[i], &MDPDrive.DPDrive[i], sizeof(Full_DescriptorPrm_type));
    }
}
// Востонавливаем из времменного буффера в глобальную переменную
void refreshFromTemp(Full_DescriptorPrm_type    *DPDriveTemp)
{
    u8  i;
    
    for (i = 0; i < MAX_REQ_PRM + 1; i++) 
    {
        memcpy(&MDPDrive.DPDrive[i], &DPDriveTemp[i], sizeof(Full_DescriptorPrm_type));
    }
}
//===================================================================
//              Обработчик статусного меню пульта
//===================================================================
void HandlerMenuStatus(u16 Key)
{
    keyName_type                KeyTemp;
    static void                 *pHOF;
    DescriptorMenu_type         DM;
    DescriptorPrm_type          DP;
    static HeadOfFile_type      HOF;
    DescriptorMenuOld_type      DMOld;
    DescriptorPrmOld_type       DPOld;
    static HeadOfFileOld_type   HOFOld;
    OS_Q_DATA                   QData;
    Status_type                 StatusRef[COUNT_STATUS];
    wm_type                     *wm;
    u16                         i, Len, refr;// , x;
    char                        *ptrStr, strtmp[17];
    u32                         xPos, yPos, yPosTemp;
    static u16                  StatusDaleyValid = 0;
    static u16                  StatusDaleyValid2 = 0;
    static u16                  PrmDaleyValid[COUNT_STATUS] = { 0, 0, 0, 0,
                                                            0, 0, 0, 0,
                                                            0, 0, 0, 0,
                                                            0, 0, 0, 0,
                                                            0, 0, 0, 0};
    char                        Name[16];
    static u8                   cursor = 0;
    Full_DescriptorPrm_type     DPDriveSave[MAX_REQ_PRM + 1];

    
    if (0 == HandlerMenuStatus_struct.GetOpisPrm) {         //При первом входе в меню ищем все привода
        for (i = 0; i < MAXDRIVES_CUT; i++) {
            if (ReadLnk(&HandlerMenuStatus_struct.LnkNetFDP, (u8)i)) {
                HandlerMenuStatus_struct.AdrFRAM[i] = HandlerMenuStatus_struct.LnkNetFDP.Lnk;
                HandlerMenuStatus_struct.NetAdr[i] = HandlerMenuStatus_struct.LnkNetFDP.NetAdr;
                HandlerMenuStatus_struct.MaxDrv = i + 1;
            }
            else if (i == 0){//не нашли ниодного
                SetModeMenu(MODE_HandlerMenuPultNewWithDrive);
                MessagePopUp(GetStringMess(MessageMess_DRIVE_NO_SETUP));
                return;
            }
            else{
                break;
            }
        }
        if (!ReadLnk(&HandlerMenuStatus_struct.LnkNetFDP, HandlerMenuStatus_struct.CurLnk)) {
            HandlerMenuStatus_struct.CurLnk = 0;
            i = 0;
        }
        else{
            i = HandlerMenuStatus_struct.CurLnk;
        }
        pHOF = (void *)((u32)HandlerMenuStatus_struct.AdrFRAM[i]);
        // Сохраняем значения полных опесателей из глобальной переменной
        saveToTemp(&DPDriveSave[0]);
        if (gIsNvsaOld)
        {
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&HOFOld, pHOF, sizeof(HeadOfFileOld_type)); 
            #else            
            memcpy(&HOFOld, (void *)((u32)pHOF+STM32_FLASH_OFFSET), sizeof(HeadOfFileOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
            #endif
            
            FM25RD(&StatusRef, &nv.OpisDrv[HandlerMenuStatus_struct.CurLnk].StatusRef[0], sizeof(StatusRef));//Читаем статус для первого установленного привода
            FillingStatusMDP(&StatusRef[0], &HOFOld, &DMOld, &DPOld, pHOF);
        }
        else
        {
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&HOF, pHOF, sizeof(HeadOfFile_type)); 
            #else            
            memcpy(&HOF, (void *)((u32)pHOF+STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
            #endif

            FM25RD(&StatusRef, &nv.OpisDrv[HandlerMenuStatus_struct.CurLnk].StatusRef[0], sizeof(StatusRef));//Читаем статус для первого установленного привода
            FillingStatusMDP(&StatusRef[0], &HOF, &DM, &DP, pHOF);
        }
        SetLastDrvAdr(HandlerMenuStatus_struct.NetAdr[HandlerMenuStatus_struct.CurLnk]);

       /*if (!TestValidDrv(HandlerMenuStatus_struct.NetAdr[HandlerMenuStatus_struct.CurLnk], HandlerMenuStatus_struct.CurLnk))
       {
            // Востонавливаем значения полных опесателей в глобальную переменную
            refreshFromTemp(&DPDriveSave[0]);
            SetModeMenu(MODE_HandlerMenuPultNewWithDrive);
            MessagePopUp(GetStringMess(MessageMess_DRIVE_NO_TEST));
            HandlerMenuStatus_struct.GetOpisPrm = 1;
            return;
        }*/
       // Инициализация единиц измерения
        initEdIzm(pHOF);
        initMessPult(pHOF);
        HandlerMenuStatus_struct.GetOpisPrm = 1;
    }
    //------------

    //------------
    refr = 0;
    KeyTemp = NON_KEY;
    if (MessageActiv() == MessageFlashErr_OK)
    {
        if (Key == KEY_STOP || Key == KEY_START)
        {
            KeyTemp = NON_KEY; 
        }
        else
        {
            KeyTemp = (keyName_type)Key;
        }
        Key = NON_KEY;
    }
    switch(Key){
        case KEY_RIGHT:
            if (HandlerMenuStatus_struct.MaxDrv > 1){
                if (HandlerMenuStatus_struct.CurLnk < HandlerMenuStatus_struct.MaxDrv - 1) HandlerMenuStatus_struct.CurLnk++;
                else HandlerMenuStatus_struct.CurLnk = 0;
                refr = 1;
            }
            else
            {
                // Восстанавливаем значения полных описателей в глобальную переменную
                HandlerMenuPultNew_struct.GetOpisStatus = 0;
                refreshFromTemp(&DPDriveSave[0]);
                SetModeMenu(MODE_HandlerMenuPultNewWithDrive);
                return;
            }
            break;
        case KEY_LEFT:
            if (HandlerMenuStatus_struct.MaxDrv > 1){
                if (HandlerMenuStatus_struct.CurLnk > 0) HandlerMenuStatus_struct.CurLnk--;
                else HandlerMenuStatus_struct.CurLnk = HandlerMenuStatus_struct.MaxDrv - 1;
                refr = 1;
            }
            break;
        case KEY_FUNC:
            // Востонавливаем значения полных опесателей в глобальную переменную
            HandlerMenuPultNew_struct.GetOpisStatus = 0;
            refreshFromTemp(&DPDriveSave[0]);
            SetModeMenu(MODE_HandlerMenuPultNewWithDrive);
            return;
            break;
        case KEY_UP:   
            if (cursor > 0)
            {
                cursor--; 
            }
            break;
        case KEY_DOWN:   
            if (cursor < COUNT_STATUS - 3)
            {
                cursor++;
            }
            break;
        default:
            break;
    }
    if(refr){
        OSQFlush(QueueDrvIn);                                                   //Очистка очереди
        i = HandlerMenuStatus_struct.CurLnk;
        pHOF = (void *)((u32)HandlerMenuStatus_struct.AdrFRAM[i]);
        if (gIsNvsaOld)
        {
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&HOFOld, pHOF, sizeof(HeadOfFileOld_type));
            #else            
            memcpy(&HOFOld, (void *)((u32)pHOF+STM32_FLASH_OFFSET), sizeof(HeadOfFileOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
            #endif
            
            FM25RD(&StatusRef, &nv.OpisDrv[HandlerMenuStatus_struct.CurLnk].StatusRef[0], sizeof(StatusRef));//Читаем статус для первого установленного привода
            FillingStatusMDP(&StatusRef[0], &HOFOld, &DMOld, &DPOld, pHOF);
        }
        else
        {
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&HOF, pHOF, sizeof(HeadOfFile_type));
            #else            
            memcpy(&HOF, (void *)((u32)pHOF+STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                       
            #endif
            FM25RD(&StatusRef, &nv.OpisDrv[HandlerMenuStatus_struct.CurLnk].StatusRef[0], sizeof(StatusRef));//Читаем статус для первого установленного привода
            FillingStatusMDP(&StatusRef[0], &HOF, &DM, &DP, pHOF);
        }
        SetLastDrvAdr(HandlerMenuStatus_struct.NetAdr[HandlerMenuStatus_struct.CurLnk]);
        if (!TestValidDrv(HandlerMenuStatus_struct.NetAdr[HandlerMenuStatus_struct.CurLnk],
                          HandlerMenuStatus_struct.CurLnk)){
            MessagePopUp(GetStringMess(MessageMess_DRIVE_NO_TEST));
            // Восстанавливаем значения полных описателей в глобальную переменную
            refreshFromTemp(&DPDriveSave[0]);
            SetModeMenu(MODE_HandlerMenuPultNewWithDrive);
            memset(&HandlerMenuStatus_struct, 0, sizeof(HandlerMenuStatus_struct));
            //return;
        }
    }

    OSQQuery(QueueDrvIn, &QData);
    if(QData.OSNMsgs == 0)
    {
        ModbusMakeOpis(&MDPDrive.DPDrive[NUM_STATUS], READ_UST);
        ModbusMakeOpis(&MDPDrive.DPDrive[NUM_STATUS_2], READ_UST);
        for (i = cursor; i < cursor + 3; i++) {
            ModbusMakeOpis(&MDPDrive.DPDrive[i], READ_UST);
        }
    }

    RazborRx();
    GrafBffClear();

    // Верхний параметр
    xPos = 110;
    yPos = 44;
    DrawLine(0, yPos - 26, 160, yPos - 26, 1, 2, 3);
    for (i = cursor; i < cursor + 3; i++) 
    {
        if (MDPDrive.DPDrive[i].ValidData || ((PrmDaleyValid[ i ] < 8) && (MDPDrive.DPDrive[i].ValidDataFirst == 1))) 
        {
            wm = &MDPDrive.DPDrive[i].wm;
            ParamToStr(pHOF, &MDPDrive.DPDrive[i], wm, &StrOut[0]);
            if (wm->TypeVar != NtxtFrmt)
            {
                ptrStr = strstr(&StrOut[0], &MasEdIzmTek[MDPDrive.DPDrive[i].wm.StepEdIsm & 0xff][0]);
                if (ptrStr != NULL) *ptrStr = 0;
                Len = strlen(&StrOut[0]);
                if (Len < 6){
                    Len = (6 - Len) / 2 + Len;
                    sprintf(&strtmp[0], "%5s", &StrOut[0]);
                }
                else{
                    StrOut[Len] = 0;
                    sprintf(&strtmp[0], "%s", &StrOut[0]);
                }

                xPos = StrOutFont(xPos, yPos - 13, NO_INV, &strtmp[0], &fontBig, rightAlign); 
                sprintf(&strtmp[0], "%s", &MasEdIzmTek[MDPDrive.DPDrive[i].wm.StepEdIsm & 0xff][0]);

                yPosTemp = yPos + fontBig.propertyFont.height - fontNormal.propertyFont.height - 13;
                StrOutFont(xPos, yPosTemp, NO_INV, &strtmp[0], &fontNormal, leftAlign );
            }
            else
            {
                xPos = 110;
                Len = strlen(&StrOut[0]);
                if (Len > 11)
                {
                    StrOut[11] = 0;
                    StrOutFont(6, yPos - 6, NO_INV, &StrOut[0],  &fontMiddle, leftAlign ); 
                }
                else
                {
                    StrOutFont(159, yPos - 6, NO_INV, &StrOut[0],  &fontMiddle, rightAlign ); 
                }
                yPosTemp = yPos + fontBig.propertyFont.height - fontNormal.propertyFont.height - 13;
            }
            if (MDPDrive.DPDrive[i].ValidData)
            {
                PrmDaleyValid[ i ] = 0;
            }
        }
        else
        {
            sprintf(&StrOut[0], "%5s", &noValidData[0]);
            StrOutFont(xPos, yPos - 16, NO_INV, &StrOut[0], &fontBig, rightAlign );
        }
        PrmDaleyValid[ i ]++;
        if (gIsNvsaOld)
        {
            getNamePrm(pHOF, &HOFOld, &MDPDrive.DPDrive[i].DPOld, &Name[0]);
        }
        else
        {
            getNamePrm(pHOF, &HOF, &MDPDrive.DPDrive[i].DP, &Name[0]);
        }
        sprintf(&StrOut[0], "%s", Name);
        StrOutFont(2, yPos - 26, NO_INV, &StrOut[0], &fontNormal, leftAlign );
        DrawLine(0, yPos + 16, 160, yPos + 16, 1, 2, 3);
        yPos += 42;
    }
    //==========================================================
    //Указатели наличия параметров за пределами видимой области
    //==========================================================
    if (cursor != 0)
    {
        DrawIcon16x16(25, 150, upSmall);
    }
    if (cursor < COUNT_STATUS - 3)
    {
        DrawIcon16x16(35, 150, downSmall);
    }
    //Разбор статуса
    i = HandlerMenuStatus_struct.CurLnk;
    if (MDPDrive.DPDrive[NUM_STATUS].ValidData) 
    {
        statusDRV_tipe      StatusDrv;
        StatusDrv.all = (u16)MDPDrive.DPDrive[NUM_STATUS].wm.s32.Value;
        parseWordStatus(HandlerMenuStatus_struct.NetAdr[i], StatusDrv );
        StatusDaleyValid = 0;
    }
    else 
    {
        if ((StatusDaleyValid < 4) && (MDPDrive.DPDrive[NUM_STATUS].ValidDataFirst == 1))
        {
            statusDRV_tipe      StatusDrv;
            StatusDrv.all = (u16)MDPDrive.DPDrive[NUM_STATUS].wm.s32.Value;
            parseWordStatus(HandlerMenuStatus_struct.NetAdr[i], StatusDrv );
        }
        else
        {
            statusDRV_tipe      StatusDrv;
            StatusDrv.all = 0xFFFF;
            parseWordStatus(HandlerMenuStatus_struct.NetAdr[i], StatusDrv);
            MDPDrive.DPDrive[NUM_STATUS].ValidDataFirst = 0;
        }
        StatusDaleyValid++;
    }
    //*************************************************************
    //Обработка статуса 2
    //*************************************************************
    if (MDPDrive.DPDrive[NUM_STATUS_2].ValidData == 1)
    {
        statusDRV_2_type      StatusDrv2;
        StatusDrv2.all = (u16)MDPDrive.DPDrive[NUM_STATUS_2].wm.s32.Value;
        parseWordStatus2(MDPDrive.DPDrive[NUM_STATUS_2].AdrNet, StatusDrv2);
        StatusDaleyValid2 = 0;
    }
    else
    {
        if ((StatusDaleyValid2 < 4) && (MDPDrive.DPDrive[NUM_STATUS_2].ValidDataFirst == 1))
        {
            statusDRV_2_type      StatusDrv2;
            StatusDrv2.all = (u16)MDPDrive.DPDrive[NUM_STATUS_2].wm.s32.Value;
            parseWordStatus2(MDPDrive.DPDrive[NUM_STATUS_2].AdrNet, StatusDrv2);
        }
        else
        {
            statusDRV_2_type      StatusDrv2;
            StatusDrv2.all = 0xFFFF;
            parseWordStatus2(HandlerMenuStatus_struct.NetAdr[i], StatusDrv2);
            MDPDrive.DPDrive[NUM_STATUS_2].ValidDataFirst = 0;
        }
        StatusDaleyValid2++;
    }
    MessageShow(KeyTemp);

    
    GrfOut();
}
//===================================================================
// Разбор статусного слова ПЧ и отображение в верхней строке диспея
// и управление индикацией
//===================================================================
void parseWordStatus(u16 Adr, statusDRV_tipe status)
{
    u16                 tmp;
    LEDState_type       led;
    static u16          TimeDelay;
    static u16          IsFail = 0;
    
    //
    if(HandlerMenuPult_struct.AdditionalMode == AddMODE_RepalceStatus)
    {
        sprintf(&StrOut[0], "%s", GetStringTitle(MessageTitul_EDIT));
        StrOutFont(5, 0, NO_INV, &StrOut[0], &fontNormal, leftAlign );
        WordStatusForLed();
        return;
    }
    
    led.all = 0;        // Прочищаем

    if (TimeMigSTATUS < GetTickLeftU16(TimeDelay)) {
        TimeDelay = OSTimeGet();
        IconStatus_struct.NumIcon++;
    }
    
    if (status.all & 0xff)
    {
        if (!IsFail && ((status.all & 0xff) != 0xff))
        {
            IsFail = 1;
            MessageSetAlarm((MessageFlashFlag_type)(status.all & 0xff), 3);
        }
    }
    else
    {
        IsFail = 0;
    }
//    sprintf(&StrOut[0], "%s %d", "Адр.", Adr);
//    sprintf(&StrOut[0], "Статус %d", Adr);
    sprintf(&StrOut[0], GetStringTitle(MessageTitul_STATUS));//"Статус");
    StrOutFont(0, 0, NO_INV, &StrOut[0], &fontNormal, leftAlign );

    if (status.all == 0xFFFF){
        StrOutFont(130, 0, NO_INV, (char *)&noValidData[0], &fontNormal, leftAlign );
        return;
    }
    //местный, удаленный
    if (status.bit.localRemote ){ 
        ledRemotOn(led);
        DrawIcon16x16(144, 0, remoteIcon);
    }
    else{
        ledLocalOn(led);
        DrawIcon16x16(144, 0, localIcon);
    }

    if ( status.bit.run ){              // Проверка состояния ШИМ на выходе
        ledWorkOn(led);

        if( status.bit.speedup){        // Разгон
            tmp = IconStatus_struct.NumIcon % 4;
            if(status.bit.fNegative )   { DrawIcon16x16(124, 0, RazgonIcon[tmp] ); }
            else                        { DrawIcon16x16(124, 0, RazgonReverseIcon[tmp] );}
        }
        else if(status.bit.rundown){    // Торможение
            tmp = IconStatus_struct.NumIcon % 4;
            if (status.bit.fNegative)   { DrawIcon16x16(124, 0, RazgonReverseIcon[tmp] ); }
            else                        { DrawIcon16x16(124, 0, RazgonIcon[tmp] ); }
        }
        else{                           // Вышел на задание
            if( flag_from_status_word2 == 0 )
            {
                tmp = IconStatus_struct.NumIcon % 3;
                if (status.bit.fNegative)   { DrawIcon16x16(124, 0, ZadanieReverseIcon[tmp] ); }
                else                        { DrawIcon16x16(124, 0, ZadanieIcon[tmp] ); }
            }
        }
    }
    else{ 
        if (status.bit.breakdown){    		//Авария
            ledFaultOn(led);

            tmp = IconStatus_struct.NumIcon % 2;
            if (tmp == 0)   DrawIcon16x16(106, 0, avarIcon );
            else            DrawIcon16x16(106, 0, avar_iIcon );
        }
        else{
            if( status.bit.charge){			// Заряд
                ledReadyOff(led);

                tmp = IconStatus_struct.NumIcon % 4;
                DrawIcon16x16(124, 0, chargeIcon[tmp] );
            }
            else if (status.bit.ready) {    //Готовность
                ledReadyOn(led);
                DrawIcon16x16(124, 0, readyIcon );
            }
            else{							// Выбег
				tmp = IconStatus_struct.NumIcon % 2;
			    if (tmp == 0)           DrawIcon16x16(124, 0, stopIcon );
			    else if(tmp == 1)       DrawIcon16x16(124, 0, stop_iIcon );
            }
        }
    }
  
    LEDState = led;     // Из локальной в глобальную структуру
    return;
}
//===================================================================
// Разбор статусного слова ПЧ и отображение в верхней строке диспея
// и управление индикацией
//===================================================================
void parseWordStatus2(u16 Adr, statusDRV_2_type status)
{
    u16                 tmp;
  
    /*if (IconStatus_struct.Delay == 0) {
        IconStatus_struct.Delay = DELAY_ICON;
        IconStatus_struct.NumIcon++;
    }
    else{
        IconStatus_struct.Delay--;
    }*/
    if (status.all == 0xFFFF)
    {
        sprintf(&StrOut[0], "----");
        StrOutFont(70, 144, NO_INV, &StrOut[0], &fontNormal, leftAlign );
        return;
    }
    //ручной, авто
    if (status.bit.autoMode )
    { 
        sprintf(&StrOut[0], GetStringTitle(MessageTitul_AUTO));
        StrOutFont(70, 144, NO_INV, &StrOut[0], &fontNormal, leftAlign );
    }
    else
    {
        sprintf(&StrOut[0], GetStringTitle(MessageTitul_MANUAL));
        StrOutFont(70, 144, NO_INV, &StrOut[0], &fontNormal, leftAlign );
    }
    
    if(status.bit.run ){
      if(status.bit.pickUp )
      {         
          flag_from_status_word2 = 1;
          //tmp = IconStatus_struct.NumIcon % 4;           // Индикация режима "Подхват" иконкой 16х16
          //DrawIcon16x16(124, 0, PickUpIcon[tmp]);
          tmp = IconStatus_struct.NumIcon % 5;             // Индикация режима "Подхват" иконкой 32х16
          DrawIcon32x16(108, 0, PickUpIcon1[tmp]);
      }
      if(status.bit.stopByDC ) 
      {
          if(status.bit.syshka ) 
          {
            flag_from_status_word2 = 1;
            tmp = IconStatus_struct.NumIcon % 4;           // Индикация режима "Сушка"
            DrawIcon16x16(124, 0, SyshkaIcon[tmp]);
          }
          else{
            flag_from_status_word2 = 1;
            tmp = IconStatus_struct.NumIcon % 5;           // Индикация режима "Торможение постоянным током"
            DrawIcon16x16(124, 0, stopByDCIcon[tmp]);
          }
      }
      if(status.bit.pickUp == 0 && status.bit.syshka == 0 && status.bit.stopByDC == 0 )
      {
          flag_from_status_word2 = 0;
      }
    }
       
    return;
}
//===================================================================
// Считывание статусного слова ПЧ и управление только индикацией
//===================================================================
void WordStatusForLed(void)
{
    statusDRV_tipe          StatusDrv;
    OS_Q_DATA               QData;
    LnkNetFDP_type          LnkNetFDP;
    u8                      NetAdr;
    
    if (ReadLnk(&LnkNetFDP, 0)) {
        NetAdr = LnkNetFDP.NetAdr;
    }
    else{
//        MessageCancel("Сеть не установлена");
        NetAdr = 1;
    }
    //Разбор статуса
    RazborRx();
    if (MDPDrive.DPDrive[NUM_STATUS].ValidData) 
    {        
        StatusDrv.all = (u16)MDPDrive.DPDrive[NUM_STATUS].wm.s32.Value;
        parseWordStatusForLed(NetAdr, StatusDrv);
    }
    OSQQuery(QueueDrvIn, &QData);
    if(QData.OSNMsgs == 0)
    {
        ModbusMakeOpis(&MDPDrive.DPDrive[NUM_STATUS], READ_UST);
    }
}
//===================================================================
// Разбор статусного слова ПЧ и управление только индикацией
//===================================================================
void parseWordStatusForLed(u16 Adr, statusDRV_tipe status)
{
    LEDState_type       led;

    led.all = 0;        // Прочищаем
    
    /* We do not need these LEDs in menu edit mode
    if(HandlerMenuPult_struct.AdditionalMode == AddMODE_RepalceStatus)
    {
        ledRemotOff(led);
        ledLocalOff(led);
        ledWorkOff(led);
        ledFaultOff(led);
        ledReadyOff(led);
        
        return;
    }*/
    
    //местный, удаленный
    if (status.bit.localRemote ){ 
        ledRemotOn(led);
    }
    else{
        ledLocalOn(led);
    }

    if ( status.bit.run ){              // Проверка состояния ШИМ на выходе
        ledWorkOn(led);
    }
    else{ 
        if (status.bit.breakdown)    		//Авария
        {
            ledFaultOn(led);
        }
        else{
            if( status.bit.charge)			// Заряд
            {
                ledReadyOff(led);
            }
            else if (status.bit.ready)     //Готовность
            {
                ledReadyOn(led);
            }
            else					// Выбег
            {		
            }
        }
    }

    LEDState = led;     // Из локальной в глобальную структуру
    return;
}
//===================================================================
//      Не убирать для тестированя пригодится
//===================================================================
/*
u32 fgfgfgfg = 0;
    switch( fgfgfgfg ){
        case 0:
            DrawIcon16x16(124, 0, RazgonReverseIcon[tmp] );
            break;
        case 1:
            DrawIcon16x16(124, 0, RazgonIcon[tmp] );
            break;
        case 2:
            DrawIcon16x16(124, 0, RazgonReverseIcon[2-tmp]  );
            break;
        case 3:
            DrawIcon16x16(124, 0, RazgonIcon[2-tmp]  );
            break;

        default:
        case 5:
            DrawIcon16x16(124, 0, readyIcon );
            break;
        case 6:
            tmp = IconStatus_struct.NumIcon % 3;
            DrawIcon16x16(124, 0, ZadanieIcon[tmp] );
            break;
        case 7:
            DrawIcon16x16(124, 0, stopIcon );
            break;
        case 8:
            DrawIcon16x16(124, 0, stop_iIcon );
            break;
    }

goto debugGoto;

debugGoto:  //// WARNING
*/
//===================================================================
//          Функция разбора принятой информации
//===================================================================
void RazborRx_mk(void)
{
    void                    		*AdrDP;
    ModbusOpisPrm4byte_type 		ModbusOpisPrm4byte;
    ModbusOpisPrm2byte_type 		ModbusOpisPrm2byte;
    DescriptorPrm_type      		DP;
    DescriptorPrmOld_type      		DPOld;
    wm_type                 		*wm;
    MsgReqDrvIn_type       			*pntMsgDrv, MsgDrv;
    Full_DescriptorPrm_type 		*FDP;
    u8                      		*pBuf;
    u8                      		j;
    u8                      		Err;
    OS_Q_DATA               		QDataIn, QDataOut;

    OSQQuery(QueueDrvIn,  &QDataIn );
    OSQQuery(QueueDrvOut, &QDataOut);

    do{
        pntMsgDrv = OSQAccept(QueueDrvOut, &Err);                                    //Запрос обратного сообщения из драйвера
    
        if(Err == OS_ERR_NONE){
	        memcpy(&MsgDrv, pntMsgDrv, sizeof(MsgReqDrvIn_type));
            //pntMsgDrv->Busy = 0;
            //MsgDrvOut.Busy = 0;                                                     //!!!!Снятие занятости с параметра
            //DPDrive->Busy
            if (MsgDrv.NumParam < MAX_REQ_PRM && MsgDrv.Func == READ_H_REGS){
                CntReq--;
                //pntMsgDrvOut->
                if(MsgDrv.Err == Ok_err){
                    j = pntMsgDrv->NumParam;
                    //FDP = HandlerMenuPultNew_struct.pntDPDriveReq[j];!!!!!
                    FDP = &MDPDrive.DPDrive[j];
                    if (FDP == NULL) return;
                    //FDP->Busy = 0;//Освобождаем параметр от занятости
                    if (MsgDrv.AdrData != FDP->msgReqDrvIn.AdrData  ||
                        MsgDrv.AdrNet != FDP->msgReqDrvIn.AdrNet ||
                        MsgDrv.Func != FDP->msgReqDrvIn.Func ||
                        MsgDrv.NumParam != FDP->msgReqDrvIn.NumParam ||
                        FDP->Ignore) {
                        FDP->ValidData = 0;
                        FDP->Ignore = 0;
                        continue;
                    }
                    //if ((HandlerMenuPultNew_struct.pntDPDrive[i]->DP.IDGrp << 7) + HandlerMenuPultNew_struct.pntDPDrive[i]->DP.IDPrm == MDPDrive.DPDrive[j].AdrParam){
                    pBuf = FDP->BufParam;
                    FDP->ValidData = 1;
                    FDP->ValidDataFirst = 1;

                    wm = &FDP->wm;
                    if (gIsNvsaOld)
                    {
                        if (FDP->DPOld.FlgPrm.LimChange || FDP->DPOld.FlgPrm.FormatChange || FDP->DPOld.FlgPrm.NumBase)
                        {//Сложный параметр
                            if(wm->kolvShort == 2) 
                            {                                            //4-х байтный параметр
                                memcpy(&ModbusOpisPrm4byte, pBuf, sizeof(ModbusOpisPrm4byte));
                                wm->s32.Value       = swapU32(ModbusOpisPrm4byte.Value);
                                wm->s32.HighLim     = swapU32(ModbusOpisPrm4byte.HighLim);
                                wm->s32.LowLim      = swapU32(ModbusOpisPrm4byte.LowLim);
                                wm->Flags           = swapU32(ModbusOpisPrm4byte.Flags);
                                wm->StepEdIsm       = swapU16(ModbusOpisPrm4byte.StepEdIsm);
                            }
                            else
                            {                                                               //2-х байтный параметр
                                memcpy(&ModbusOpisPrm2byte, pBuf, sizeof(ModbusOpisPrm2byte));
                                wm->s32.Value       = (u16)swapU16(ModbusOpisPrm2byte.Value);
                                wm->s32.HighLim     = (u16)swapU16(ModbusOpisPrm2byte.HighLim);
                                wm->s32.LowLim      = (u16)swapU16(ModbusOpisPrm2byte.LowLim);
                                wm->Flags           = swapU32(ModbusOpisPrm2byte.Flags);
                                wm->StepEdIsm       = swapU16(ModbusOpisPrm2byte.StepEdIsm);
                            }
                        }
                        else
                        {                                                                   //Простой параметр
                            if(wm->kolvShort == 2) 
                            {                                            //4-х байтный параметр
                                wm->s32.Value       = swapU32(*(u32 *)pBuf);
                                AdrDP = (void *)((DescriptorPrm_type *)((u32)FDP->BaseDP + sizeof(DescriptorPrmOld_type)));
                                
                                #ifdef BLOCK_STM_MEMORY
                                AT45RD(&DPOld, AdrDP, sizeof(DescriptorPrmOld_type));
                                #else            
                                memcpy(&DPOld, (void *)((u32)AdrDP + STM32_FLASH_OFFSET), sizeof(DescriptorPrmOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
                                #endif
                                
                                wm->s32.LowLim  = (FDP->DPOld.LowLim & 0xffff) + ((u32)DPOld.LowLim << 16);
                                wm->s32.HighLim = (FDP->DPOld.HighLim & 0xffff) + ((u32)DPOld.HighLim << 16);
                            }
                            else
                            {                                                   //2-х байтный параметр
                                wm->s32.Value       = (u16)swapU16(*(u16 *)pBuf);
                                wm->s32.LowLim      = (u16)FDP->DPOld.LowLim;
                                wm->s32.HighLim      = (u16)FDP->DPOld.HighLim;
                                if (wm->TypeVar == refFormat)
                                {
                                    // Проводим замену дескриптора для ссылочного параметра
                                    ChangeDescriptorPrm(FDP, FDP->BaseHOF, FDP->msgReqDrvIn.AdrNet);
                                }
                            }
                        }
                    }
                    else
                    {
                        if (FDP->DP.FlgPrm.LimChange || FDP->DP.FlgPrm.FormatChange || FDP->DP.FlgPrm.NumBase)
                        {//Сложный параметр
                            if(wm->kolvShort == 2) 
                            {                                            //4-х байтный параметр
                                memcpy(&ModbusOpisPrm4byte, pBuf, sizeof(ModbusOpisPrm4byte));
                                wm->s32.Value       = swapU32(ModbusOpisPrm4byte.Value);
                                wm->s32.HighLim     = swapU32(ModbusOpisPrm4byte.HighLim);
                                wm->s32.LowLim      = swapU32(ModbusOpisPrm4byte.LowLim);
                                wm->Flags           = swapU32(ModbusOpisPrm4byte.Flags);
                                wm->StepEdIsm       = swapU16(ModbusOpisPrm4byte.StepEdIsm);
                            }
                            else
                            {                                                               //2-х байтный параметр
                                memcpy(&ModbusOpisPrm2byte, pBuf, sizeof(ModbusOpisPrm2byte));
                                wm->s32.Value       = (u16)swapU16(ModbusOpisPrm2byte.Value);
                                wm->s32.HighLim     = (u16)swapU16(ModbusOpisPrm2byte.HighLim);
                                wm->s32.LowLim      = (u16)swapU16(ModbusOpisPrm2byte.LowLim);
                                wm->Flags           = swapU32(ModbusOpisPrm2byte.Flags);
                                wm->StepEdIsm       = swapU16(ModbusOpisPrm2byte.StepEdIsm);
                            }
                        }
                        else
                        {                                                                   //Простой параметр
                            if(wm->kolvShort == 2) 
                            {                                            //4-х байтный параметр
                                wm->s32.Value       = swapU32(*(u32 *)pBuf);
                                AdrDP = (void *)((DescriptorPrm_type *)((u32)FDP->BaseDP + sizeof(DescriptorPrm_type)));

                                #ifdef BLOCK_STM_MEMORY
                                AT45RD(&DP, AdrDP, sizeof(DescriptorPrm_type));
                                #else            
                                memcpy(&DP, (void *)((u32)AdrDP + STM32_FLASH_OFFSET), sizeof(DescriptorPrm_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
                                #endif
                                
                                wm->s32.LowLim  = (FDP->DP.LowLim & 0xffff) + ((u32)DP.LowLim << 16);
                                wm->s32.HighLim = (FDP->DP.HighLim & 0xffff) + ((u32)DP.HighLim << 16);
                            }
                            else
                            {                                                   //2-х байтный параметр
                                wm->s32.Value       = (u16)swapU16(*(u16 *)pBuf);
                                wm->s32.LowLim      = (u16)FDP->DP.LowLim;
                                wm->s32.HighLim      = (u16)FDP->DP.HighLim;
                                if (wm->TypeVar == refFormat)
                                {
                                    // Проводим замену дескриптора для ссылочного параметра
                                    ChangeDescriptorPrm(FDP, FDP->BaseHOF, FDP->msgReqDrvIn.AdrNet);
                                }
                            }
                        }
                    }
                }
                else{
                    debugStop();
                }
            }
        }
    }while (Err == OS_ERR_NONE);
}
//===================================================================
//===================================================================
char *FindEndSym(const char *str, const char sym)
{
    char    *pStr, *pStrSym, *pStrSymPrev;
    pStr = (char *)str;
    pStrSym = NULL;
    do{
        pStrSymPrev = strstr(pStr, &sym);
        if (pStrSymPrev) {
            //pStr = pStrSymPrev + 1;
            pStr = pStrSymPrev + 1;
            pStrSym = pStrSymPrev;
            if (sym == '\n' || sym == '\r')
            {
                break;
            }
        }
        else break;
    }while (1);
    return pStrSym;
}
//===================================================================
#define MAX_STR_MESSAGE     16
//===================================================================
//===================================================================
u32 FindSpace(const char *str)
{
    char    *pStrFrmt, *pStrFrmtPrev;
    char    symFind[] = {'\r','\n',' ', '/', '.', ':', '~', ',', '^', '%', '&', '*', '\\'};
    u16     posSpace, i;

    posSpace = 0;

    pStrFrmt = NULL;
    for (i = 0; i < sizeof(symFind) / sizeof(symFind[0]); i++) 
    {
        pStrFrmtPrev = FindEndSym(str, symFind[i]);
        if (pStrFrmtPrev > pStrFrmt) 
        {
            pStrFrmt = pStrFrmtPrev;
            if (i == 0)
            {
                pStrFrmtPrev = FindEndSym(str, symFind[i + 1]);
                if (pStrFrmt < pStrFrmtPrev)
                {
                    pStrFrmt = pStrFrmtPrev;
                    break;
                }
            }
            else if (i == 1)
            {
                break;
            }
        }
    }

    posSpace = 0;
    if (pStrFrmt != NULL) {
        posSpace = pStrFrmt - str;
     }
    return posSpace;
}
//===================================================================
//          Расстановка знаков пунктуации
//===================================================================
u32 FindHyphen(const char *str)
{
    char    *pStrFrmt, *pStrFrmtPrev;
    char    vowel[] = {'й', 'у', 'е', 'ъ', 'ы', 'а', 'о', 'э', 'я', 'и', 'ь', 'ю',
                       'Й', 'У', 'Е', 'Ъ', 'Ы', 'А', 'О', 'Э', 'Я', 'И', 'Ь', 'Ю',
                       'e', 'y', 'u', 'i', 'o', 'a',
                       'E', 'Y', 'U', 'I', 'O', 'A'};
    u16     posHyphen, i;

    //pStr = (char *)str;
    posHyphen = 0;

    pStrFrmt = NULL;
    for (i = 0; i < sizeof(vowel) / sizeof(vowel[0]); i++) {
        pStrFrmtPrev = FindEndSym(str, vowel[i]);
        if (pStrFrmtPrev > pStrFrmt) {
            if (!FindSpace((const char*)pStrFrmtPrev)) {
                pStrFrmt = pStrFrmtPrev;
            }

        }
    }

    posHyphen = 0;
    if (pStrFrmt != NULL) {
        posHyphen = pStrFrmt - str;
    }
    return posHyphen;
}
//===================================================================
//===================================================================
u16 Format_Message(const char *const Msg, u16 beginLine)
{
    const char  *pStr;
    u32         posFrmt;
    u16         szMsg, szCopy, cntStr, skipStr, stop;
    char        strtmp[24];
    char        c;


    pStr = Msg;
    if (pStr == NULL) return 0;

    szMsg = strlen(pStr);
    cntStr = 0;
    skipStr = 0;
    stop = 0;

    while (szMsg != 0 && cntStr != 8) {
        if (*pStr == ' ')
        {
            pStr++;
            szMsg--;
        }
        if (szMsg > MAX_STR_MESSAGE - 1) 
        {
            memcpy(&strtmp[0], pStr, MAX_STR_MESSAGE);
            strtmp[MAX_STR_MESSAGE] = 0;
            szCopy = 0;
            posFrmt = FindSpace(&strtmp[0]);
            if (!posFrmt && strtmp[posFrmt] == '\n')
            {  
                szCopy = posFrmt + 1;
                szMsg -= szCopy;
                pStr++;
                continue;
            }
            if (posFrmt > MAX_STR_MESSAGE - 6 && strtmp[posFrmt] != '\n' && strtmp[posFrmt] != '\r') 
            {
                szCopy = posFrmt + 1;
                strtmp[szCopy] = 0;
                szMsg -= szCopy;
            }
            else if (strtmp[posFrmt] != '\n' && strtmp[posFrmt] != '\r')
            {
                c = strtmp[MAX_STR_MESSAGE - 2];
                strtmp[MAX_STR_MESSAGE - 2] = 0;
                posFrmt =  FindHyphen(&strtmp[0]);
                strtmp[MAX_STR_MESSAGE - 2] = c;
                if (posFrmt)
                {
                    if (strtmp[posFrmt - 1] != ' ')
                    {
                        szCopy = posFrmt + 1;
                        strtmp[szCopy] = '-';
                        strtmp[szCopy + 1] = 0;
                        szMsg -= szCopy;
                    }
                    else
                    {
                        szCopy = posFrmt + 2;
                        strtmp[szCopy] = '-';
                        strtmp[szCopy + 1] = 0;
                        szMsg -= szCopy;
                    }
                }
                else
                {
                    posFrmt = FindSpace(&strtmp[0]);
                    if (posFrmt)
                    {
                        szCopy = posFrmt + 1;
                        strtmp[posFrmt] = 0;
                        szMsg -= szCopy;
                    }
                    else
                    {
                        szMsg -= MAX_STR_MESSAGE - 1;
                        szCopy = MAX_STR_MESSAGE - 1;
                    }
                }
            }
            else
            {
                szCopy = posFrmt + 1;
                strtmp[posFrmt] = 0;
                szMsg -= szCopy;
            }
              
        }
        else
        {
            memcpy(&strtmp[0], pStr, szMsg);
            posFrmt = FindSpace(&strtmp[0]);
            if (!posFrmt && strtmp[posFrmt] == '\n')
            {  
                szCopy = posFrmt + 1;
                szMsg -= szCopy;
                pStr++;
            }
            memcpy(&strtmp[0], pStr, szMsg);
            strtmp[szMsg] = 0;
            szMsg = 0;
            stop = 1;
        }
        pStr += szCopy;
        skipStr++;

        if (skipStr > beginLine) {
            StrOutFont(5, cntStr * 16 + 3, NO_INV, &strtmp[0], &fontNormal, leftAlign );
            cntStr++;
        }
    }

    if (!stop){
        DrawIcon16x16(65, 135, downSmall);
    }
    if (beginLine){
        DrawIcon16x16(75, 135, upSmall);
    }

    return stop;
}
//===================================================================
//===================================================================
u32 HandlerMessage(Message_type *Msg)
{
    keyName_type        Key;
    u16                 posBegin, stop;
//    u8                  InvYesNo, InvYes, InvNo;
    invers_type         InvYesNo, InvYes, InvNo;

    u8                  err;

    InvYesNo    = NO_INV;
    InvYes      = INV;
    InvNo       = NO_INV;
    stop        = 1;
    posBegin    = 0;

    while(1){
      ResetWD(tsk_lcd);
        OSSemPend(SemMenu, SYS_FOREVER, &err );
        Key = WaitKey(NULL);

        switch(Key) {
            case KEY_UP:
                if (posBegin > 0){
                    posBegin--;
                }
            break;
            case KEY_DOWN:
                if (stop == 0){
                    posBegin++;
                }
            break;
            case KEY_LEFT:
            case KEY_RIGHT:
                InvYesNo ^= INV;
                if (InvYesNo == INV) {
                    InvNo = InvYes;
                    InvYes = NO_INV;
                }
                else{
                    InvYes = InvNo;
                    InvNo = NO_INV;
                }
                break;
            case KEY_ESC:
                if (Msg->Mode == MSG_MODE_CANCEL)
                    return 0;
                break;
            case KEY_ENTER:
                if (Msg->Mode == MSG_MODE_YESNO){
                    if (InvYes == 0) return 0;
                    else return 1;
                }

        }
        GrafBffClear();
        DrawRectangle(2, 3, 157, 130);

        stop = Format_Message(Msg->Msg, posBegin);

        if (Msg->Mode == MSG_MODE_YESNO) {
            StrOutFont(130,  144, InvYes, GetStringTitle(MessageTitul_YES), &fontNormal, leftAlign );
            StrOutFont(10, 144, InvNo, GetStringTitle(MessageTitul_NO), &fontNormal, leftAlign );
        }
        else if (Msg->Mode == MSG_MODE_CANCEL){
             StrOutFont(0, 144, INV, GetStringTitle(MessageTitul_ESC), &fontNormal, leftAlign );
        }
        else if (Msg->Mode == MSG_MODE_THROUGH) {
            GrfOut();
            return 0;
        }

        GrfOut();
        WordStatusForLed();

        if (Msg->Mode == MSG_MODE_POPUP){
            OSTimeDly(T_MSG * 1000);
            return 0;
        }
    }
}
//===================================================================
//===================================================================
u32 MessagePopUp(const char *const str)
{
    Message_type    Msg;

    Msg.Msg     = str;
    Msg.Mode    = MSG_MODE_POPUP;

    return HandlerMessage(&Msg);
}
//===================================================================
//===================================================================
u32 MessageCancel(const char *const str)
{
    Message_type    Msg;

    Msg.Msg     = str;
    Msg.Mode    = MSG_MODE_CANCEL;

    return HandlerMessage(&Msg);
}
//===================================================================
//===================================================================
u32 MessageThrough(const char *const str)
{
    Message_type    Msg;
    Msg.Msg     = str;
    Msg.Mode    = MSG_MODE_THROUGH;

    return HandlerMessage(&Msg);
}
//===================================================================
//===================================================================
u32 MessageYesNo(const char *const str)
{
    Message_type    Msg;

    Msg.Msg     = str;
    Msg.Mode    = MSG_MODE_YESNO;

    return HandlerMessage(&Msg);
}
// **************************************************************************
//              обработчик главного меню пульта
// **************************************************************************
void HandlerMainMenuPult(u16 Key)
{
    Menu_type       *pntM;
    u32             LenMenu;

    LenMenu = sizeof(MainMenu) / sizeof(Menu_type) - 1;

    pntM = &MainMenu[HandlerMainMenuPult_struct.Pos];
    MenuToGraf(&MainMenu[0], HandlerMainMenuPult_struct.Pos, LenMenu);

    switch(Key){
        case KEY_DOWN:
            if (HandlerMainMenuPult_struct.Pos < LenMenu - 1) {
                HandlerMainMenuPult_struct.Pos++;
            }
            break;

        case KEY_UP:
            if (HandlerMainMenuPult_struct.Pos > 0) {
                HandlerMainMenuPult_struct.Pos--;
            }
            break;

        case KEY_RIGHT:
            if (pntM->CallMenu != NULL){
                pntM->CallMenu();
            }
            break;

        case KEY_LEFT:
            SetModeMenu(MODE_HandlerMenuStatus);
            break;

        case KEY_ENTER:
            break;
        default: break;
        }
}
// **************************************************************************
//              обработчик меню пульта
// **************************************************************************
void InitDP(Full_DescriptorPrm_type *DPDrive, void *BaseHOF, void *BaseDP, u8 AdrNet)
{
    if (gIsNvsaOld)
    {
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&DPDrive->DPOld, BaseDP, sizeof(DescriptorPrmOld_type));
        #else            
        memcpy(&DPDrive->DPOld, (void *)((u32)BaseDP + STM32_FLASH_OFFSET), sizeof(DescriptorPrmOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
        #endif
        
        DPDrive->BaseHOF            = BaseHOF;
        DPDrive->BaseDP             = BaseDP;
        DPDrive->AdrNet             = AdrNet;
        DPDrive->Ignore             = 1;
        DPDrive->ValidData          = 0;
        DPDrive->ValidDataFirst     = 0;
        DPDrive->ValidStruct        = 1;
        DPDrive->msgReqDrvIn.Busy   = 0;
    }
    else
    {
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&DPDrive->DP, BaseDP, sizeof(DescriptorPrm_type));
        #else            
        memcpy(&DPDrive->DP, (void *)((u32)BaseDP + STM32_FLASH_OFFSET), sizeof(DescriptorPrm_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                   
        #endif

        DPDrive->BaseHOF            = BaseHOF;
        DPDrive->BaseDP             = BaseDP;
        DPDrive->AdrNet             = AdrNet;
        DPDrive->Ignore             = 1;
        DPDrive->ValidData          = 0;
        DPDrive->ValidDataFirst     = 0;
        DPDrive->ValidStruct        = 1;
        DPDrive->msgReqDrvIn.Busy   = 0;
    }
}
// **************************************************************************
//              обработчик меню пульта
// **************************************************************************
void InitDPWhithoutIgnor(Full_DescriptorPrm_type *DPDrive, void *BaseHOF, void *BaseDP, u16 Type, u8 AdrNet)
{
    if (gIsNvsaOld)
    {
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&DPDrive->DPOld, BaseDP, sizeof(DescriptorPrmOld_type));
        #else            
        memcpy(&DPDrive->DPOld, (void *)((u32)BaseDP + STM32_FLASH_OFFSET), sizeof(DescriptorPrmOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                   
        #endif

        DPDrive->BaseHOF            = BaseHOF;
        DPDrive->BaseDP             = BaseDP;
        DPDrive->AdrNet             = AdrNet;
        DPDrive->Ignore             = 0;
        DPDrive->ValidData          = 0;
        DPDrive->ValidDataFirst     = 0;
        DPDrive->ValidStruct        = 1;
        DPDrive->msgReqDrvIn.Busy   = 0;
    }
    else
    {
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&DPDrive->DP, BaseDP, sizeof(DescriptorPrm_type));
        #else            
        memcpy(&DPDrive->DP, (void *)((u32)BaseDP + STM32_FLASH_OFFSET), sizeof(DescriptorPrm_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                           
        #endif

        DPDrive->BaseHOF            = BaseHOF;
        DPDrive->BaseDP             = BaseDP;
        DPDrive->AdrNet             = AdrNet;
        DPDrive->Ignore             = 0;
        DPDrive->ValidData          = 0;
        DPDrive->ValidDataFirst     = 0;
        DPDrive->ValidStruct        = 1;
        DPDrive->msgReqDrvIn.Busy   = 0;

    }
}
//===================================================================
void DeInitAllDP(void)
{
    memset(&MDPDrive, 0, sizeof(MDPDrive));
}
//===================================================================
//===================================================================
void InitAllDM(u16 cntItem)
{
    u16 i;
    
    if (gIsNvsaOld)
    {
        for (i = 0; (i < MAX_STR_OTOBR_MENU) && (i < cntItem); i++) 
        { //Начальное наполнение дескрипторов меню
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&DMDriveOld[i], DMGlobalOld[i], sizeof(DescriptorMenuOld_type));
            #else            
            memcpy(&DMDriveOld[i], (void *)((u32)DMGlobalOld[i] + STM32_FLASH_OFFSET), sizeof(DescriptorMenuOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                           
            #endif
        }
    }
    else
    {
        for (i = 0; (i < MAX_STR_OTOBR_MENU) && (i < cntItem); i++) 
        { //Начальное наполнение дескрипторов меню
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&DMDrive[i], DMGlobal[i], sizeof(DescriptorMenu_type));
            #else            
            memcpy(&DMDrive[i], (void *)((u32)DMGlobal[i] + STM32_FLASH_OFFSET), sizeof(DescriptorMenu_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                                       
            #endif
        }
    }
}
//===================================================================
void InitDM(void *DMDriveDirect, u16 NumItem)
{
    DescriptorMenu_type     *pDMDriveDirect;
    DescriptorMenuOld_type  *pDMDriveDirectOld;
      
    if (gIsNvsaOld)
    {
        pDMDriveDirectOld = DMDriveDirect;
        #ifdef BLOCK_STM_MEMORY
        AT45RD(pDMDriveDirectOld, DMGlobalOld[NumItem], sizeof(DescriptorMenuOld_type));
        #else            
        memcpy(pDMDriveDirectOld, (void *)((u32)DMGlobalOld[NumItem] + STM32_FLASH_OFFSET), sizeof(DescriptorMenuOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
        #endif
    }
    else
    {
        pDMDriveDirect = DMDriveDirect;
        #ifdef BLOCK_STM_MEMORY
        AT45RD(pDMDriveDirect, DMGlobal[NumItem], sizeof(DescriptorMenu_type));
        #else            
        memcpy(pDMDriveDirect, (void *)((u32)DMGlobal[NumItem] + STM32_FLASH_OFFSET), sizeof(DescriptorMenu_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
        #endif
    }
}
//===================================================================
//          Поиск группы, заполнение структуры
//===================================================================
//void *GetOpisMenu(const HeadOfFile_type *HOF, DescriptorMenu_type *DM, void *BaseHOF, u16 Type, u16 NumMenu)
void *FindGroup(void *HOF, void *DM, void *BaseHOF, u8 NumGroup)
{
    u32 i;
    HeadOfFileOld_type      *pHOFOld;
    HeadOfFile_type         *pHOF;
    DescriptorMenuOld_type  *pDMOld;
    DescriptorMenu_type     *pDM;
    
    void *AdrMenu;
    if (gIsNvsaOld)
    {
        pHOFOld = HOF;
        pDMOld = DM;
        for (i = 0; i < sizeof(DMGlobal)/sizeof(DMGlobal[0]); i++) 
        {
            AdrMenu = GetOpisMenu(pHOFOld, pDMOld, BaseHOF, i);
            if (pDMOld->IDGrp == NumGroup && AdrMenu && (pDMOld->FlgMenu.TypeMenu != TypeMenu_MGRP_GRPS)) 
            {
                return AdrMenu;
            }
        }
    }
    else
    {
        pHOF = HOF;
        pDM = DM;
        for (i = 0; i < sizeof(DMGlobal)/sizeof(DMGlobal[0]); i++) 
        {
            AdrMenu = GetOpisMenu(pHOF, pDM, BaseHOF, i);
            if (pDM->IDGrp == NumGroup && AdrMenu && (pDM->FlgMenu.TypeMenu != TypeMenu_MGRP_GRPS)) 
            {
                return AdrMenu;
            }
        }
    }
    return NULL;
}
//===================================================================
//===================================================================
//void *GetOpisParam(const HeadOfFile_type *HOF, const DescriptorMenu_type *DM, DescriptorPrm_type *DP, void *BaseHOF, u16 Type, u16 NumPrm)
void *FindParam(void *HOF, void *DM, void *DP, void *BaseHOF, u8 NumParam)
{
    u32 i;
    HeadOfFileOld_type      *pHOFOld;
    HeadOfFile_type         *pHOF;
    DescriptorMenuOld_type  *pDMOld;
    DescriptorMenu_type     *pDM;
    DescriptorPrmOld_type   *pDPOld;
    DescriptorPrm_type      *pDP;
    void                    *AdrPrm;
    
    if (gIsNvsaOld)
    {
        pHOFOld = HOF;
        pDMOld = DM;
        pDPOld = DP;
        for (i = 0; i < pHOFOld->QuantityPrm && i < sizeof(DPGlobal)/ sizeof(DPGlobal[0]) ; i++) 
        {
            AdrPrm = GetOpisParam(pHOFOld, pDMOld, pDPOld, BaseHOF, i);
            if (pDPOld->IDPrm == NumParam && AdrPrm) 
            {
                return AdrPrm;
            }
        }
    }
    else
    {
        pHOF = HOF;
        pDM = DM;
        pDP = DP;
        for (i = 0; i < pHOF->QuantityPrm && i < sizeof(DPGlobal)/ sizeof(DPGlobal[0]) ; i++) 
        {
            AdrPrm = GetOpisParam(pHOF, pDM, pDP, BaseHOF, i);
            if (pDP->IDPrm == NumParam && AdrPrm) 
            {
                return AdrPrm;
            }
        }
    }
    return NULL;
}
//===================================================================
//===================================================================
inline void *getPntCurDP(void)
{
    Full_DescriptorPrm_type *pntDPDriveTmp;

    pntDPDriveTmp = HandlerMenuPultNew_struct.pntDPMenu + HandlerMenuPultNew_struct.PosCursor;
        if (pntDPDriveTmp > &MDPDrive.DPDrive[MAX_STR_OTOBR_PRM - 1])
            pntDPDriveTmp -= MAX_STR_OTOBR_PRM;
        else if (pntDPDriveTmp > &MDPDrive.DPDrive[HandlerMenuPultNew_struct.CntParam - 1])
            pntDPDriveTmp -= HandlerMenuPultNew_struct.CntParam;
    return (void *)pntDPDriveTmp;
}
//===================================================================
//===================================================================
inline void *getPntCurDM(void)
{
    DescriptorMenu_type *pntDMDriveTmp;

    pntDMDriveTmp = &DMDrive[0] + HandlerMenuPultNew_struct.PosCursor;
        if (pntDMDriveTmp > &DMDrive[MAX_STR_OTOBR_MENU - 1])
            pntDMDriveTmp -= MAX_STR_OTOBR_MENU;
        else if (pntDMDriveTmp > &DMDrive[HandlerMenuPultNew_struct.CntMenu - 1])
            pntDMDriveTmp -= HandlerMenuPultNew_struct.CntParam;
    return (void *)pntDMDriveTmp;
}
//===================================================================

//
void UpdateStatusChange(u16 GrpPrm)
{
    int j;
    if(GrpPrm == STATUS_EDIT_GRPPRM)
    {
        if(ReqS16Prm(STATUS_EDIT_GRPPRM))
        {
            //ResetPassword();
            HandlerMenuStatusReplace_struct.Pos = 0;
            HandlerMenuStatusReplace_struct.Cursor = 0;
            for(j=0; j<MAX_LEVEL_MENU ; j++)
            {
                HandlerMenuPultNew_struct.OpisLevel[ j ].LevPosBegin = 0;
                HandlerMenuPultNew_struct.OpisLevel[ j ].LevPosCursor = 0;
                HandlerMenuPultNew_struct.OpisLevel[ j ].LevPos = 0;  
                HandlerMenuPultNew_struct.OpisLevel[ j ].AdrLevel = 0;  
            }
            SetModeMenu(MODE_HandlerMenuStatusReplace);
            SendS16Prm(STATUS_EDIT_GRPPRM, 0);   
        }
    }
}

//===================================================================
//  Обработчик меню параметров пульта и параметров привода
//===================================================================
void HandlerMenuPultAndDrive(u16 Key, u8 NetAdr, u16 Type, void *HofAT45)
{
    u16                KeyTemp;
    HeadOfFile_type             *pHOF;
    u32                         i;
    u16                         crc, tmp, status;
    Full_DescriptorPrm_type     *pntDPDriveTmp;
    Full_DescriptorPrm_type     DPDrive;
    DescriptorMenu_type         DM;
    DescriptorPrm_type          DP;
    DescriptorMenuOld_type      DMOld;
    DescriptorPrmOld_type       DPOld;
    OS_Q_DATA                   QData;
    u16                         exit;
    u16         				lenStr;
    u16                         AdrMenuTemp;        // Адрес меню
    static u16                  StatusDaleyValid = 0;
    static u16                  StatusDaleyValid2 = 0;
    Status_type                 StatusRef;
    HandlerMenuPultNew_struct_type HandlerMenuPultNew_Temp;
    u16                         OldLang;

    exit = 0;

        SetLastDrvAdr(NetAdr);
        pHOF = HofAT45;
              
          
        if (HandlerMenuPultNew_struct.GetOpisMenu == 0){//Чтобы не долбать флешь каждый раз, флаг снимается при переходе из меню параметров в меню групп, при нажатии кнопки влево
            if (gIsNvsaOld)
            {
                #ifdef BLOCK_STM_MEMORY
                AT45RD(&HandlerMenuPultNew_struct.HOFOld, HofAT45, sizeof(HeadOfFileOld_type));
                #else            
                memcpy(&HandlerMenuPultNew_struct.HOFOld, (void *)((u32)HofAT45 + STM32_FLASH_OFFSET), sizeof(HeadOfFileOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                          
                #endif

                crc = GetCrc(&HandlerMenuPultNew_struct.HOFOld, sizeof(HandlerMenuPultNew_struct.HOFOld) - sizeof(HandlerMenuPultNew_struct.HOFOld.HeaderCrc));
                if (crc != HandlerMenuPultNew_struct.HOFOld.HeaderCrc){
                    if (HandlerMenuChangeDrive_struct.MaxDrv > 1)      //Если установленных приводов больше чем 1, то возвращаемся в меню выбора привода
                        SetModeMenu(MODE_HandlerMenuChangeDriveForMenuPult);
                    else
                        if (MessageYesNo(povtor))
                        {
                            AllPrmPnt->ADD_DRIVE = 1;
                            InstallDrv();
                        }
                    return;
                }
            }
            else
            {
                #ifdef BLOCK_STM_MEMORY
                AT45RD(&HandlerMenuPultNew_struct.HOF, HofAT45, sizeof(HeadOfFile_type));
                #else            
                memcpy(&HandlerMenuPultNew_struct.HOF, (void *)((u32)HofAT45 + STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
                #endif              

                crc = GetCrc(&HandlerMenuPultNew_struct.HOF, sizeof(HandlerMenuPultNew_struct.HOF) - sizeof(HandlerMenuPultNew_struct.HOF.HeaderCrc));
                if (crc != HandlerMenuPultNew_struct.HOF.HeaderCrc){
                    if (HandlerMenuChangeDrive_struct.MaxDrv > 1)      //Если установленных приводов больше чем 1, то возвращаемся в меню выбора привода
                        SetModeMenu(MODE_HandlerMenuChangeDriveForMenuPult);
                    else
                        if (MessageYesNo(povtor))
                        {
                            AllPrmPnt->ADD_DRIVE = 1;
                            InstallDrv();
                        }
                    return;
                }
            }
            /*if (!TestValidDrv(NetAdr, HandlerMenuChangeDrive_struct.Pos)){
                MessagePopUp(GetStringMess(MessageMess_DRIVE_NO_TEST));
                exit = 1;
            }*/
            if (gIsNvsaOld)
            {
                SetRootMenu(&HandlerMenuPultNew_struct);
                if (HandlerMenuPult_struct.AdditionalMode == AddMODE_RepalceStatus){                    //Ищутся группы с текущими параметрами
                    AdrMenuTemp = HandlerMenuPultNew_struct.AdrLevelMenu;
                    HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOFOld, HofAT45, HandlerMenuChangeDrive_struct.Pos, AdrMenuTemp, TypeVisible_TEK);
                }
                else{
                    AdrMenuTemp = HandlerMenuPultNew_struct.AdrLevelMenu;
                    HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOFOld, HofAT45, HandlerMenuChangeDrive_struct.Pos, AdrMenuTemp, TypeVisible_ALL);
                }
            }
            else
            {
                SetRootMenu(&HandlerMenuPultNew_struct);
                if (HandlerMenuPult_struct.AdditionalMode == AddMODE_RepalceStatus){                    //Ищутся группы с текущими параметрами
                    AdrMenuTemp = HandlerMenuPultNew_struct.AdrLevelMenu;
                    HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOF, HofAT45, HandlerMenuChangeDrive_struct.Pos, AdrMenuTemp, TypeVisible_TEK);
                }
                else{
                    AdrMenuTemp = HandlerMenuPultNew_struct.AdrLevelMenu;
                    HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOF, HofAT45, HandlerMenuChangeDrive_struct.Pos, AdrMenuTemp, TypeVisible_ALL);
                }

            }
            InitAllDM(HandlerMenuPultNew_struct.CntMP);

            for (i = 0; i < MAX_REQ_PRM; i++) {  //инициализация номеров параметров для разбора посылок
                MDPDrive.DPDrive[i].NumParam = i;
            }
            HandlerMenuPultNew_struct.GetOpisMenu = 1;
            HandlerMenuPultNew_struct.GetOpisStatus = 0;
        }
        if (HandlerMenuPultNew_struct.GetOpisStatus == 0)
        {
            if (gIsNvsaOld)
            {
                if (FindGroup(&HandlerMenuPultNew_struct.HOFOld, &DMOld, HofAT45, GRP_STATUS))
                {
                    if(MDPDrive.DPDrive[NUM_STATUS].BaseDP = FindParam(&HandlerMenuPultNew_struct.HOFOld, &DMOld, &DPOld, HofAT45, PRM_STATUS))
                    {
                        MDPDrive.DPDrive[NUM_STATUS].AdrNet          = NetAdr;
                        MDPDrive.DPDrive[NUM_STATUS].BaseHOF         = HofAT45;
                        memcpy(&MDPDrive.DPDrive[NUM_STATUS].DPOld, &DPOld, sizeof( DescriptorPrmOld_type));
                        MDPDrive.DPDrive[NUM_STATUS].NumParam        = NUM_STATUS;
                        MDPDrive.DPDrive[NUM_STATUS].ValidData       = 0;
                        MDPDrive.DPDrive[NUM_STATUS].ValidDataFirst  = 0;
                        MDPDrive.DPDrive[NUM_STATUS].Ignore          = 1;
                        MDPDrive.DPDrive[NUM_STATUS].msgReqDrvIn.Busy= 0;
                    }
                    if(MDPDrive.DPDrive[NUM_STATUS_2].BaseDP = FindParam(&HandlerMenuPultNew_struct.HOFOld, &DMOld, &DPOld, HofAT45, PRM_STATUS_2))
                    {
                        MDPDrive.DPDrive[NUM_STATUS_2].AdrNet          = NetAdr;
                        MDPDrive.DPDrive[NUM_STATUS_2].BaseHOF         = HofAT45;
                        memcpy(&MDPDrive.DPDrive[NUM_STATUS_2].DPOld, &DPOld, sizeof( DescriptorPrmOld_type));
                        MDPDrive.DPDrive[NUM_STATUS_2].NumParam        = NUM_STATUS_2;
                        MDPDrive.DPDrive[NUM_STATUS_2].ValidData       = 0;
                        MDPDrive.DPDrive[NUM_STATUS_2].ValidDataFirst  = 0;
                        MDPDrive.DPDrive[NUM_STATUS_2].Ignore          = 1;
                        MDPDrive.DPDrive[NUM_STATUS_2].msgReqDrvIn.Busy= 0;
                    }
                }
            }
            else
            {
                if (FindGroup(&HandlerMenuPultNew_struct.HOF, &DM, HofAT45, GRP_STATUS))
                {
                    if(MDPDrive.DPDrive[NUM_STATUS].BaseDP = FindParam(&HandlerMenuPultNew_struct.HOF, &DM, &DP, HofAT45, PRM_STATUS))
                    {
                        MDPDrive.DPDrive[NUM_STATUS].AdrNet          = NetAdr;
                        MDPDrive.DPDrive[NUM_STATUS].BaseHOF         = HofAT45;
                        memcpy(&MDPDrive.DPDrive[NUM_STATUS].DP, &DP, sizeof( DescriptorPrm_type));
                        MDPDrive.DPDrive[NUM_STATUS].NumParam        = NUM_STATUS;
                        MDPDrive.DPDrive[NUM_STATUS].ValidData       = 0;
                        MDPDrive.DPDrive[NUM_STATUS].ValidDataFirst  = 0;
                        MDPDrive.DPDrive[NUM_STATUS].Ignore          = 1;
                        MDPDrive.DPDrive[NUM_STATUS].msgReqDrvIn.Busy= 0;
                    }
                    if(MDPDrive.DPDrive[NUM_STATUS_2].BaseDP = FindParam(&HandlerMenuPultNew_struct.HOF, &DM, &DP, HofAT45, PRM_STATUS_2))
                    {
                        MDPDrive.DPDrive[NUM_STATUS_2].AdrNet          = NetAdr;
                        MDPDrive.DPDrive[NUM_STATUS_2].BaseHOF         = HofAT45;
                        memcpy(&MDPDrive.DPDrive[NUM_STATUS_2].DP, &DP, sizeof( DescriptorPrm_type));
                        MDPDrive.DPDrive[NUM_STATUS_2].NumParam        = NUM_STATUS_2;
                        MDPDrive.DPDrive[NUM_STATUS_2].ValidData       = 0;
                        MDPDrive.DPDrive[NUM_STATUS_2].ValidDataFirst  = 0;
                        MDPDrive.DPDrive[NUM_STATUS_2].Ignore          = 1;
                        MDPDrive.DPDrive[NUM_STATUS_2].msgReqDrvIn.Busy= 0;
                    }
                }
            }
            HandlerMenuPultNew_struct.GetOpisStatus = 1;
        }
             
        
    if ( 0 == HandlerMenuPultNew_struct.CntMenu ){
        memset(&HandlerMenuPultNew_struct, 0, sizeof(HandlerMenuPultNew_struct));   //Выход в главное меню
        exit = 1;
    }

    if (exit)
    {
        if (HandlerMenuChangeDrive_struct.MaxDrv > 1)      //Если установленных приводов больше чем 1, то возвращаемся в меню выбора привода
            SetModeMenu(MODE_HandlerMenuChangeDriveForMenuPult);
            AllPrmPnt->ADD_DRIVE = 1;
            InstallDrv();
            HandlerMenuPultNew_struct.GetOpisMenu = 0;
            HandlerMenuPult_struct.SwitchMode = AddMODE_NONE;
            HandlerMenuPult_struct.AdditionalMode = AddMODE_NONE;
            MessagePopUp("No visiable menu!");  
            SetModeMenu(MODE_HandlerMenuStatus);
            HandlerMenuStatus_struct.GetOpisPrm = 0;
            return ;
    }
    
    KeyTemp = NON_KEY;
    if (MessageActiv() == MessageFlashErr_OK)
    {
        if (Key == KEY_STOP || Key == KEY_START)
        {
            KeyTemp = NON_KEY; 
        }
        else
        {
            KeyTemp = Key;
        }
        Key = NON_KEY;
    }
    switch(Key){
    	
    case KEY_DOWN: 
      {
        if (HandlerMenuPultNew_struct.Pos < HandlerMenuPultNew_struct.CntMP - 1) 
        {
            HandlerMenuPultNew_struct.Pos++;
        if (HandlerMenuPultNew_struct.Mode == MODE_MENU)
        {
            if (HandlerMenuPultNew_struct.PosCursor < MAX_STR_OTOBR_MENU - 1 &&
                    HandlerMenuPultNew_struct.PosCursor < HandlerMenuPultNew_struct.CntMP - 1) 
            {
                HandlerMenuPultNew_struct.PosCursor++;
            }
            else
            {
                if (HandlerMenuPultNew_struct.CntMP < MAX_STR_OTOBR_MENU) 
                        tmp = HandlerMenuPultNew_struct.CntMP - 1;
                else 
                        tmp = MAX_STR_OTOBR_MENU - 1;
                if (HandlerMenuPultNew_struct.Pos != HandlerMenuPultNew_struct.CntMP)
                {
                    if (gIsNvsaOld)
                    {
                        memcpy(&DMDriveOld[0], &DMDriveOld[1], sizeof(DescriptorMenuOld_type) * MAX_STR_OTOBR_MENU);
                        
                        #ifdef BLOCK_STM_MEMORY
                        AT45RD(&DMDriveOld[tmp], DMGlobalOld[HandlerMenuPultNew_struct.Pos], sizeof(DescriptorMenuOld_type));
                        #else            
                        memcpy(&DMDriveOld[tmp], (void *)((u32)DMGlobalOld[HandlerMenuPultNew_struct.Pos] + STM32_FLASH_OFFSET), sizeof(DescriptorMenuOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса
                        #endif

                    }
                    else
                    {
                        memcpy(&DMDrive[0], &DMDrive[1], sizeof(DescriptorMenu_type) * MAX_STR_OTOBR_MENU);
                        #ifdef BLOCK_STM_MEMORY
                        AT45RD(&DMDrive[tmp], DMGlobal[HandlerMenuPultNew_struct.Pos], sizeof(DescriptorMenu_type));
                        #else            
                        memcpy(&DMDrive[tmp], (void *)((u32)DMGlobal[HandlerMenuPultNew_struct.Pos] + STM32_FLASH_OFFSET), sizeof(DescriptorMenu_type)); //  *!* Загрузка из памяти STM по сдвигу адреса
                        #endif
                    }
                }
            }
        }
        else if (HandlerMenuPultNew_struct.Mode == MODE_PRM)
        {
                if (HandlerMenuPultNew_struct.PosCursor < MAX_STR_OTOBR_PRM - 1 &&
                    HandlerMenuPultNew_struct.PosCursor < HandlerMenuPultNew_struct.CntMP - 1) 
                {
                    HandlerMenuPultNew_struct.PosCursor++;
                }
                else 
                {
                    if (HandlerMenuPultNew_struct.Pos != HandlerMenuPultNew_struct.CntMP)
                    {
                        if (gIsNvsaOld)
                        {
                            InitDP(HandlerMenuPultNew_struct.pntDPMenu, pHOF, DPGlobalOld[HandlerMenuPultNew_struct.Pos], NetAdr);
                        }
                        else
                        { 
                            InitDP(HandlerMenuPultNew_struct.pntDPMenu, pHOF, DPGlobal[HandlerMenuPultNew_struct.Pos], NetAdr);
                        }
                        HandlerMenuPultNew_struct.pntDPMenu->Ignore = 1;
                        HandlerMenuPultNew_struct.pntDPMenu->ValidData = 0;
                        HandlerMenuPultNew_struct.pntDPMenu->ValidDataFirst = 0;
                        if(HandlerMenuPultNew_struct.pntDPMenu < &MDPDrive.DPDrive[MAX_STR_OTOBR_PRM - 1] &&
                            HandlerMenuPultNew_struct.pntDPMenu < &MDPDrive.DPDrive[HandlerMenuPultNew_struct.CntMP - 1])
                            HandlerMenuPultNew_struct.pntDPMenu++;
                        else
                            HandlerMenuPultNew_struct.pntDPMenu = &MDPDrive.DPDrive[0];
                    }
                }
            }
        }
        }
        break;

    case KEY_UP:
      {
        if (HandlerMenuPultNew_struct.Pos > 0)
        {
            HandlerMenuPultNew_struct.Pos--;
        if (HandlerMenuPultNew_struct.PosCursor > 0)
        {
        	HandlerMenuPultNew_struct.PosCursor--;
        }
        else
        {
            if (HandlerMenuPultNew_struct.Mode == MODE_MENU) 
            {
                if (gIsNvsaOld)
                {
                    for (i = MAX_STR_OTOBR_MENU - 1; i > 0; i--) 
                    {
                        memcpy(&DMDriveOld[i],
                           &DMDriveOld[i - 1],
                           sizeof(DescriptorMenuOld_type));
                    }

                    #ifdef BLOCK_STM_MEMORY
                    AT45RD(&DMDriveOld[0], DMGlobalOld[HandlerMenuPultNew_struct.Pos], sizeof(DescriptorMenuOld_type));
                    #else            
                    memcpy(&DMDriveOld[0], 
                           (void *)((u32)DMGlobalOld[HandlerMenuPultNew_struct.Pos] + STM32_FLASH_OFFSET), 
                              sizeof(DescriptorMenuOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса
                    #endif

                    
                }
                else
                {
                    for (i = MAX_STR_OTOBR_MENU - 1; i > 0; i--) 
                    {
                        memcpy(&DMDrive[i],
                           &DMDrive[i - 1],
                           sizeof(DescriptorMenu_type));
                    }
                    
                    #ifdef BLOCK_STM_MEMORY
                    AT45RD(&DMDrive[0], DMGlobal[HandlerMenuPultNew_struct.Pos], sizeof(DescriptorMenu_type));
                    #else 
                    memcpy(&DMDrive[0], 
                           (void *)((u32)DMGlobal[HandlerMenuPultNew_struct.Pos] + STM32_FLASH_OFFSET), 
                              sizeof(DescriptorMenu_type)); //  *!* Загрузка из памяти STM по сдвигу адреса
                    #endif

                }
            }
            else if (HandlerMenuPultNew_struct.Mode == MODE_PRM) 
            {
                if(HandlerMenuPultNew_struct.pntDPMenu > &MDPDrive.DPDrive[0]) 
                {
                    //смещение на позицию вверх циклического указателя на меню параметров
                    HandlerMenuPultNew_struct.pntDPMenu--;
                }
                else
                {
                    HandlerMenuPultNew_struct.pntDPMenu = &MDPDrive.DPDrive[MAX_STR_OTOBR_PRM - 1];
                }
                if (gIsNvsaOld)
                {
                    InitDP(HandlerMenuPultNew_struct.pntDPMenu,
                        pHOF,
                        DPGlobalOld[HandlerMenuPultNew_struct.Pos],
                        NetAdr);
                }
                else
                {
                    InitDP(HandlerMenuPultNew_struct.pntDPMenu,
                        pHOF,
                        DPGlobal[HandlerMenuPultNew_struct.Pos],
                        NetAdr);
                }
            }
        }
        }
    }
        break;

    case KEY_RIGHT:
      {
        if( HandlerMenuPultNew_struct.Mode == MODE_MENU )
        {
			i = HandlerMenuPultNew_struct.PosCursor;     
            // Если текущее меню просмотр журнала
            if (gIsNvsaOld)
            {
                if(DMDriveOld[i].FlgMenu.TypeMenu == TypeMenu_MJRN) 
                {
                    //Просмотр журналов
                    HandlerMenuJrn_struct.netAdr = NetAdr;
                    HandlerMenuJrn_struct.BaseDM = DMGlobalOld[HandlerMenuPultNew_struct.Pos]; //Базовый адрес группы
                    HandlerMenuJrn_struct.BaseHOF = pHOF;
                    SetModeMenu(MODE_HandlerMenuJrn);
                    return;
                }
                // Если текущее меню настройки
                else if(DMDriveOld[i].FlgMenu.TypeMenu == TypeMenu_MMASTER &&
                        HandlerMenuPult_struct.AdditionalMode != AddMODE_RepalceStatus) 
                {   
                    //Меню настройки
                    HandlerMenuSetup.netAdr = NetAdr;
                    HandlerMenuSetup.BaseDM = DMGlobalOld[HandlerMenuPultNew_struct.Pos]; //Базовый адрес группы
                    HandlerMenuSetup.BaseHOF = pHOF;
                    HandlerMenuSetup.getIndex = 0;
                    SetModeMenu(MODE_HandlerMenuSetup);
                    return;
                }
                // Иначе если текущее меню, меню групп
                else if (DMDriveOld[i].FlgMenu.TypeMenu == TypeMenu_MGRP_GRPS)
                {
                    HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPosBegin =  HandlerMenuPultNew_struct.PosBegin;
                    HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPosCursor = HandlerMenuPultNew_struct.PosCursor;
                    HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPos = HandlerMenuPultNew_struct.Pos;  
                    HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].AdrLevel = HandlerMenuPultNew_struct.AdrLevelMenuFirst;
                    HandlerMenuPultNew_struct.Level++;
                    HandlerMenuPultNew_struct.AdrLevelMenuFirst = DMNumGlobalHOF[HandlerMenuPultNew_struct.Pos];
                    if (HandlerMenuPultNew_struct.AdrLevelMenuFirst == HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].AdrLevel)
                    {
                        HandlerMenuPultNew_struct.PosBegin = HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPosBegin;
                        HandlerMenuPultNew_struct.PosCursor = HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPosCursor;
                        HandlerMenuPultNew_struct.Pos = HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPos;                    
                    }
                    else
                    {
                        HandlerMenuPultNew_struct.PosBegin = 0;
                        HandlerMenuPultNew_struct.PosCursor = 0;
                        HandlerMenuPultNew_struct.Pos = 0;
                    }
                    AdrMenuTemp = HandlerMenuPultNew_struct.AdrLevelMenuFirst;
                    if(HandlerMenuPult_struct.AdditionalMode == AddMODE_RepalceStatus) 
                    {
                        HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOFOld, HofAT45, HandlerMenuChangeDrive_struct.Pos, AdrMenuTemp, TypeVisible_TEK);
                    }
                    else
                    {
                        HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOFOld, HofAT45, HandlerMenuChangeDrive_struct.Pos, AdrMenuTemp, TypeVisible_ALL);
                    }
                    InitAllDM(HandlerMenuPultNew_struct.CntMP);     
                    // Возвращаем приведущее состояние
                    GetPosPrev(HandlerMenuPultNew_struct.CntMP, HandlerMenuPultNew_struct.Pos, HandlerMenuPultNew_struct.PosCursor);
                }
                // Иначе если текущее меню, меню параметро
                else
                {
                    if (HandlerMenuPult_struct.AdditionalMode == AddMODE_RepalceStatus)
                    {
                        HandlerMenuPultNew_struct.CntParam = FindVisibleTekParam(&HandlerMenuPultNew_struct.HOFOld, &DMDriveOld[i], pHOF, HandlerMenuChangeDrive_struct.Pos);
                    }
                    else
                    {
                        HandlerMenuPultNew_struct.CntParam = FindVisibleParam(&HandlerMenuPultNew_struct.HOFOld, &DMDriveOld[i], pHOF, HandlerMenuChangeDrive_struct.Pos);
                    }
                    if (HandlerMenuPultNew_struct.CntParam)
                    {
                        HandlerMenuPultNew_struct.DMOld = &DMDriveOld[i];
                        for (i = 0; i < MAX_STR_OTOBR_PRM && i < HandlerMenuPultNew_struct.CntParam; i++) 
                        {	//Начальное наполнение дескрипторов меню
                            InitDP(&MDPDrive.DPDrive[i], pHOF, DPGlobalOld[i], NetAdr);
                        }
                        //------------------------------------------------------
                        //Считываем описатели на дополнительные параметры
                        //------------------------------------------------------
                        for (i = 0; i < MAX_REQ_PRM; i++) 
                        {  //инициализация номеров параметров для разбора посылок
                            MDPDrive.DPDrive[i].NumParam = i;
                        }
                        HandlerMenuPultNew_struct.pntDPMenu = &MDPDrive.DPDrive[0];                                            //Инициализация циклического указателя на меню параметров
                        HandlerMenuPultNew_struct.pntDPReq = &MDPDrive.DPDrive[0];

                        HandlerMenuPultNew_struct.PosBeginPrev = HandlerMenuPultNew_struct.PosBegin;
                        HandlerMenuPultNew_struct.PosCursorPrev = HandlerMenuPultNew_struct.PosCursor;
                        HandlerMenuPultNew_struct.PosPrev = HandlerMenuPultNew_struct.Pos;

                        HandlerMenuPultNew_struct.Mode = MODE_PRM;
                        HandlerMenuPultNew_struct.PosBegin = 0;
                        HandlerMenuPultNew_struct.PosCursor = 0;
                        HandlerMenuPultNew_struct.Pos = 0;
                        HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntParam;
                    } 
                }
            }
            else
            {
                if(DMDrive[i].FlgMenu.TypeMenu == TypeMenu_MJRN) 
                {
                    //Просмотр журналов
                    HandlerMenuJrn_struct.netAdr = NetAdr;
                    HandlerMenuJrn_struct.BaseDM = DMGlobal[HandlerMenuPultNew_struct.Pos]; //Базовый адрес группы
                    HandlerMenuJrn_struct.BaseHOF = pHOF;
                    SetModeMenu(MODE_HandlerMenuJrn);
                    return;
                }
                // Если текущее меню настройки
                else if(DMDrive[i].FlgMenu.TypeMenu == TypeMenu_MMASTER &&
                HandlerMenuPult_struct.AdditionalMode != AddMODE_RepalceStatus) 
                {   
                    //Просмотр журналов
                    HandlerMenuSetup.netAdr = NetAdr;
                    HandlerMenuSetup.BaseDM = DMGlobal[HandlerMenuPultNew_struct.Pos]; //Базовый адрес группы
                    HandlerMenuSetup.BaseHOF = pHOF;
                    HandlerMenuSetup.getIndex = 0;
                    SetModeMenu(MODE_HandlerMenuSetup);
                    return;
                }
                // Иначе если текущее меню, меню групп
                else if (DMDrive[i].FlgMenu.TypeMenu == TypeMenu_MGRP_GRPS)
                {
                    HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPosBegin =  HandlerMenuPultNew_struct.PosBegin;
                    HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPosCursor = HandlerMenuPultNew_struct.PosCursor;
                    HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPos = HandlerMenuPultNew_struct.Pos;  
                    HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].AdrLevel = HandlerMenuPultNew_struct.AdrLevelMenuFirst;
                    HandlerMenuPultNew_struct.Level++;
                    HandlerMenuPultNew_struct.AdrLevelMenuFirst = DMNumGlobalHOF[HandlerMenuPultNew_struct.Pos];
                    if (HandlerMenuPultNew_struct.AdrLevelMenuFirst == HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].AdrLevel)
                    {
                        HandlerMenuPultNew_struct.PosBegin = HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPosBegin;
                        HandlerMenuPultNew_struct.PosCursor = HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPosCursor;
                        HandlerMenuPultNew_struct.Pos = HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPos;                    
                    }
                    else
                    {
                        HandlerMenuPultNew_struct.PosBegin = 0;
                        HandlerMenuPultNew_struct.PosCursor = 0;
                        HandlerMenuPultNew_struct.Pos = 0;
                    }
                    AdrMenuTemp = HandlerMenuPultNew_struct.AdrLevelMenuFirst;
                    if(HandlerMenuPult_struct.AdditionalMode == AddMODE_RepalceStatus) 
                    {
                        HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOF, HofAT45, HandlerMenuChangeDrive_struct.Pos, AdrMenuTemp, TypeVisible_TEK);
                    }
                    else
                    {
                        HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOF, HofAT45, HandlerMenuChangeDrive_struct.Pos, AdrMenuTemp, TypeVisible_ALL);
                    }
                    InitAllDM(HandlerMenuPultNew_struct.CntMP);     
                    // Возвращаем приведущее состояние
                    GetPosPrev(HandlerMenuPultNew_struct.CntMP, HandlerMenuPultNew_struct.Pos, HandlerMenuPultNew_struct.PosCursor);
                }
                // Иначе если текущее меню, меню параметро
                else
                {
                    if (HandlerMenuPult_struct.AdditionalMode == AddMODE_RepalceStatus)
                    {
                        HandlerMenuPultNew_struct.CntParam = FindVisibleTekParam(&HandlerMenuPultNew_struct.HOF, &DMDrive[i], pHOF, HandlerMenuChangeDrive_struct.Pos);
                    }
                    else
                    {
                        HandlerMenuPultNew_struct.CntParam = FindVisibleParam(&HandlerMenuPultNew_struct.HOF, &DMDrive[i], pHOF, HandlerMenuChangeDrive_struct.Pos);
                    }
                    if (HandlerMenuPultNew_struct.CntParam)
                    {
                        HandlerMenuPultNew_struct.DM = &DMDrive[i];
                        for (i = 0; i < MAX_STR_OTOBR_PRM && i < HandlerMenuPultNew_struct.CntParam; i++) 
                        {	//Начальное наполнение дескрипторов меню
                            InitDP(&MDPDrive.DPDrive[i], pHOF, DPGlobal[i], NetAdr);
                        }
                        //------------------------------------------------------
                        //Считываем описатели на дополнительные параметры
                        //------------------------------------------------------
                        for (i = 0; i < MAX_REQ_PRM; i++) 
                        {  //инициализация номеров параметров для разбора посылок
                            MDPDrive.DPDrive[i].NumParam = i;
                        }
                    }
                    HandlerMenuPultNew_struct.pntDPMenu = &MDPDrive.DPDrive[0];                                            //Инициализация циклического указателя на меню параметров
                    HandlerMenuPultNew_struct.pntDPReq = &MDPDrive.DPDrive[0];

                    HandlerMenuPultNew_struct.PosBeginPrev = HandlerMenuPultNew_struct.PosBegin;
                    //if(HandlerMenuPultNew_struct.PosCursor<4)
                    HandlerMenuPultNew_struct.PosCursorPrev = HandlerMenuPultNew_struct.PosCursor;
                    HandlerMenuPultNew_struct.PosPrev = HandlerMenuPultNew_struct.Pos;

                    HandlerMenuPultNew_struct.Mode = MODE_PRM;
                    HandlerMenuPultNew_struct.PosBegin = 0;
                    HandlerMenuPultNew_struct.PosCursor = 0;
                    HandlerMenuPultNew_struct.Pos = 0;
                    HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntParam;
                }
            }
        }
    }
        break;

    case KEY_LEFT: {
        if (HandlerMenuPultNew_struct.Mode == MODE_PRM) 
        {
                HandlerMenuPultNew_struct.Pos = HandlerMenuPultNew_struct.PosPrev;
                HandlerMenuPultNew_struct.PosBegin = HandlerMenuPultNew_struct.PosBeginPrev;
                HandlerMenuPultNew_struct.PosCursor = HandlerMenuPultNew_struct.PosCursorPrev;
                HandlerMenuPultNew_struct.Mode = MODE_MENU;
            
                if (gIsNvsaOld)
                {
                    for (i = 0; i < MAX_STR_OTOBR_PRM; i++)
                    {
                        MDPDrive.DPDrive[i].ValidData = 0;
                        MDPDrive.DPDrive[i].Ignore = 1;
                    }
                    AdrMenuTemp = HandlerMenuPultNew_struct.AdrLevelMenuFirst;
                    if(HandlerMenuPult_struct.AdditionalMode == AddMODE_RepalceStatus) 
                    {
                        HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOFOld, HofAT45, HandlerMenuChangeDrive_struct.Pos, AdrMenuTemp, TypeVisible_TEK);
                    }
                    else
                    {
                        HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOFOld, HofAT45, HandlerMenuChangeDrive_struct.Pos, AdrMenuTemp, TypeVisible_ALL);
                    }
                }
                else
                {
                    for (i = 0; i < MAX_STR_OTOBR_PRM; i++)
                    {
                        MDPDrive.DPDrive[i].ValidData = 0;
                        MDPDrive.DPDrive[i].Ignore = 1;
                    }
                    AdrMenuTemp = HandlerMenuPultNew_struct.AdrLevelMenuFirst;
                    if(HandlerMenuPult_struct.AdditionalMode == AddMODE_RepalceStatus) 
                    {
                        HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOF, HofAT45, HandlerMenuChangeDrive_struct.Pos, AdrMenuTemp, TypeVisible_TEK);
                    }
                    else
                    {
                        HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOF, HofAT45, HandlerMenuChangeDrive_struct.Pos, AdrMenuTemp, TypeVisible_ALL);
                    }
                }
                InitAllDM(HandlerMenuPultNew_struct.CntMP);
                if (HandlerMenuPultNew_struct.CntMP == HandlerMenuPultNew_struct.CntMenu)
                {
                    // Возвращаем приведущее состояние
                    GetPosPrev(HandlerMenuPultNew_struct.CntMP, HandlerMenuPultNew_struct.Pos, HandlerMenuPultNew_struct.PosCursor);
                }
                else
                {
                    HandlerMenuPultNew_struct.Pos = 0;
                    HandlerMenuPultNew_struct.PosBegin = 0;
                    HandlerMenuPultNew_struct.PosCursor = 0;
                }
        }
        else if (HandlerMenuPultNew_struct.Mode == MODE_MENU && HandlerMenuPultNew_struct.Level)
        {
            HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPosBegin =  HandlerMenuPultNew_struct.PosBegin;
            HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPosCursor = HandlerMenuPultNew_struct.PosCursor;
            HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPos = HandlerMenuPultNew_struct.Pos;    
            HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].AdrLevel = HandlerMenuPultNew_struct.AdrLevelMenuFirst;  
            HandlerMenuPultNew_struct.Level--;
            HandlerMenuPultNew_struct.AdrLevelMenuFirst = HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].AdrLevel;  
            HandlerMenuPultNew_struct.Pos = HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPos;
            HandlerMenuPultNew_struct.PosBegin = HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPosBegin;
            HandlerMenuPultNew_struct.PosCursor = HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPosCursor;
            AdrMenuTemp = HandlerMenuPultNew_struct.AdrLevelMenuFirst;
                          
            if (gIsNvsaOld)
            {
                if(HandlerMenuPult_struct.AdditionalMode == AddMODE_RepalceStatus) 
                {
                    HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOFOld, HofAT45, HandlerMenuChangeDrive_struct.Pos, AdrMenuTemp, TypeVisible_TEK);
                }
                else
                {
                    HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOFOld, HofAT45, HandlerMenuChangeDrive_struct.Pos, AdrMenuTemp, TypeVisible_ALL);        
                }
            }
            else
            {
                if(HandlerMenuPult_struct.AdditionalMode == AddMODE_RepalceStatus) 
                {
                    HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOF, HofAT45, HandlerMenuChangeDrive_struct.Pos, AdrMenuTemp, TypeVisible_TEK);
                }
                else
                {
                    HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOF, HofAT45, HandlerMenuChangeDrive_struct.Pos, AdrMenuTemp, TypeVisible_ALL);
                }
            }
            InitAllDM(HandlerMenuPultNew_struct.CntMP);
            // Возвращаем приведущее состояние
            GetPosPrev(HandlerMenuPultNew_struct.CntMP, HandlerMenuPultNew_struct.Pos, HandlerMenuPultNew_struct.PosCursor);
            //-----------------------------------
        }
        else
        {
            if (HandlerMenuPult_struct.AdditionalMode == AddMODE_NONE) {
                if (HandlerMenuChangeDrive_struct.MaxDrv > 1)      //Если установленных приводов больше чем 1, то возвращаемся в меню выбора привода
                    SetModeMenu(MODE_HandlerMenuChangeDriveForMenuPult);
                else
                    SetModeMenu(MODE_HandlerMenuStatus);
                    HandlerMenuStatus_struct.GetOpisPrm = 0;
                return;
            }
            else if (HandlerMenuPult_struct.AdditionalMode == AddMODE_RepalceStatus) {
                HandlerMenuPult_struct.AdditionalMode = AddMODE_NONE;
                SetModeMenu(MODE_HandlerMenuStatusReplace);
                return;
            }
        }
    }
        break;

    case KEY_ENTER:
        if (HandlerMenuPultNew_struct.Mode == MODE_PRM)
        {
            HandlerMenuPultNew_struct.Mode = MODE_EDIT;
        }
        break;

    case KEY_ESC: {
        i = 0;

        if (HandlerMenuPultNew_struct.Mode == MODE_PRM)
        {
            pntDPDriveTmp = getPntCurDP();
            if (gIsNvsaOld)
            {         
                lenStr = getHelpPrm(pntDPDriveTmp, &StrOut[0]);
            }
            else
            {
                lenStr = getHelp(HofAT45, &StrOut[0], pntDPDriveTmp->DP.OfsHlp);
            }
            StrOut[ lenStr ] = 0;    // WARNING  	// Прочистить окончание строки
            if( lenStr != 0 )
            {						//Вывод подсказки
                MessageCancel(&StrOut[0]);
            }
            else
            {
                MessageCancel(GetStringMess(MessageMess_NO_HELP));
            }
        }
        else if (HandlerMenuPultNew_struct.Mode == MODE_MENU)
        {
            if (!gIsNvsaOld)
            { 
                lenStr = getHelp(HofAT45, &StrOut[0], DMDrive[HandlerMenuPultNew_struct.PosCursor].OfsHlp);
                StrOut[ lenStr ] = 0;    // WARNING  	// Прочистить окончание строки

                if( lenStr != 0 )
                {						//Вывод подсказки
                    MessageCancel(&StrOut[0]);
                }
                else
                {
                    MessageCancel(GetStringMess(MessageMess_NO_HELP));
                }
            }
        }
        else if(HandlerMenuPult_struct.AdditionalMode == AddMODE_RepalceStatus)
        {
            HandlerMenuPult_struct.SwitchMode = AddMODE_NONE;
            HandlerMenuPult_struct.AdditionalMode = AddMODE_NONE;
            HandlerMenuPultNew_struct.Mode = MODE_MENU;
            HandlerMenuPultNew_struct.GetOpisMenu = 0;
            memset(&HandlerMenuStatusReplace_struct, 0, sizeof(HandlerMenuStatusReplace_struct));
            SetModeMenu(MODE_HandlerMenuPultNewWithDrive);
        }
    }
            break;
        case KEY_FUNC:
            if (HandlerMenuPultNew_struct.Mode == MODE_MENU && HandlerMenuPult_struct.AdditionalMode != AddMODE_RepalceStatus)
            {
                if (gIsNvsaOld)
                {
                    if (RefToStrEdit(&StatusRef))
                    {
                        if (FindDescriptorWhithCheck(&DPDrive, &StatusRef, &HandlerMenuPultNew_struct.HOFOld, HofAT45, NetAdr, HandlerMenuChangeDrive_struct.Pos))
                        {
                            memcpy(&HandlerMenuPultNew_Temp, &HandlerMenuPultNew_struct, sizeof(HandlerMenuPultNew_struct_type));
                            tmp = FindMenuWhithPrm(&DPDrive, HofAT45, AdrMenuTemp, TypeVisible_ALL, NetAdr, HandlerMenuChangeDrive_struct.Pos);
                            if (!tmp)
                            {
                                memcpy(&HandlerMenuPultNew_struct, &HandlerMenuPultNew_Temp, sizeof(HandlerMenuPultNew_struct_type));
                                MessagePopUp(GetStringMess(MessageMess_PARAM_NO_EDIT_OR_NO_EXIST));   
                                AdrMenuTemp = HandlerMenuPultNew_struct.AdrLevelMenuFirst;
                                HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOFOld, HofAT45, HandlerMenuChangeDrive_struct.Pos, AdrMenuTemp, TypeVisible_ALL);
                                InitAllDM(HandlerMenuPultNew_struct.CntMP);     
                                // Возвращаем состояние
                                GetPosPrev(HandlerMenuPultNew_struct.CntMP, HandlerMenuPultNew_struct.Pos, HandlerMenuPultNew_struct.PosCursor);  
                            }
                        }
                        else
                        {
                            MessagePopUp(GetStringMess(MessageMess_PARAM_NO_EDIT_OR_NO_EXIST));
                        }
                    }

                }
                else
                {
                    if (RefToStrEdit(&StatusRef))
                    {
                        if (FindDescriptorWhithCheck(&DPDrive, &StatusRef, &HandlerMenuPultNew_struct.HOF, HofAT45, NetAdr, HandlerMenuChangeDrive_struct.Pos))
                        {
                            memcpy(&HandlerMenuPultNew_Temp, &HandlerMenuPultNew_struct, sizeof(HandlerMenuPultNew_struct_type));
                            tmp = FindMenuWhithPrm(&DPDrive, HofAT45, AdrMenuTemp, TypeVisible_ALL, NetAdr, HandlerMenuChangeDrive_struct.Pos);
                            if (!tmp)
                            {
                                memcpy(&HandlerMenuPultNew_struct, &HandlerMenuPultNew_Temp, sizeof(HandlerMenuPultNew_struct_type));
                                MessagePopUp(GetStringMess(MessageMess_PARAM_NO_EDIT_OR_NO_EXIST));   
                                AdrMenuTemp = HandlerMenuPultNew_struct.AdrLevelMenuFirst;
                                HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOF, HofAT45, HandlerMenuChangeDrive_struct.Pos, AdrMenuTemp, TypeVisible_ALL);
                                InitAllDM(HandlerMenuPultNew_struct.CntMP);     
                                // Возвращаем состояние
                                GetPosPrev(HandlerMenuPultNew_struct.CntMP, HandlerMenuPultNew_struct.Pos, HandlerMenuPultNew_struct.PosCursor);  
                            }
                        }
                        else
                        {
                            MessagePopUp(GetStringMess(MessageMess_PARAM_NO_EDIT_OR_NO_EXIST));
                        }
                    }
                }
            }
            else
            {
                if (HandlerMenuPult_struct.AdditionalMode == AddMODE_NONE) 
                {
                    if (HandlerMenuChangeDrive_struct.MaxDrv > 1)      //Если установленных приводов больше чем 1, то возвращаемся в меню выбора привода
                        SetModeMenu(MODE_HandlerMenuChangeDriveForMenuPult);
                    else
                        SetModeMenu(MODE_HandlerMenuStatus);
                        HandlerMenuStatus_struct.GetOpisPrm = 0;
                    return;
                }
                else if (HandlerMenuPult_struct.AdditionalMode == AddMODE_RepalceStatus) 
                {
                    HandlerMenuPult_struct.AdditionalMode = AddMODE_NONE;
                    SetModeMenu(MODE_HandlerMenuStatusReplace);
                }
            }
            break;
        default:
        	break;
    }

    //*************************************************************
    if (HandlerMenuPultNew_struct.Mode == MODE_MENU) {
        GrafBffClear();
        MenuStartOneShotReadAT45(HofAT45, HandlerMenuPultNew_struct.PosCursor, HandlerMenuPultNew_struct.Pos, HandlerMenuPultNew_struct.CntMenu);
    }
    else if (HandlerMenuPultNew_struct.Mode == MODE_PRM) {
        GrafBffClear();
        ParamStartOneShotReadAT45(HofAT45, HandlerMenuPultNew_struct.PosCursor, HandlerMenuPultNew_struct.Pos, HandlerMenuPultNew_struct.CntParam, Type);
    }

    else if (HandlerMenuPultNew_struct.Mode == MODE_EDIT) {
        pntDPDriveTmp = getPntCurDP();

        if (HandlerMenuPult_struct.AdditionalMode == AddMODE_NONE) 
        {
            HandlerMenuPultNew_struct.Mode = MODE_PRM;
            tmp = 1;
                if (MDPDrive.DPDrive[NUM_STATUS].ValidData == 0 || pntDPDriveTmp->ValidData == 0 ){
                	tmp = 0;
                }
                else{
                    status = MDPDrive.DPDrive[NUM_STATUS].wm.s32.Value;
                    if (gIsNvsaOld)
                    {
                        if (pntDPDriveTmp->DPOld.FlgPrm.EditType == NO_EDIT)
                        {
                            MessagePopUp(GetStringMess(MessageMess_PARAM_NOT_EDIT));
                            tmp = 0;
                        }
                        else if (pntDPDriveTmp->DPOld.FlgPrm.EditType == STOP_EDIT) 
                        {
                            if (getStatusWorkAll(status) || getStatusCharge(status)) 
                            {
                                MessagePopUp(GetStringMess(MessageMess_PARAM_EDIT_STOP));
                                tmp = 0;
                            }
                        }

                    }
                    else
                    {
                        if (pntDPDriveTmp->DP.FlgPrm.EditType == NO_EDIT)
                        {
                            MessagePopUp(GetStringMess(MessageMess_PARAM_NOT_EDIT));
                            tmp = 0;
                        }
                        else if (pntDPDriveTmp->DP.FlgPrm.EditType == STOP_EDIT) 
                        {
                            if (getStatusWorkAll(status) || getStatusCharge(status)) 
                            {
                                MessagePopUp(GetStringMess(MessageMess_PARAM_EDIT_STOP));
                                tmp = 0;
                            }
                        }
                    }
                }

            if (tmp == 1) 
            {
                ParamEdit(HofAT45, pntDPDriveTmp);
                ReqEtalonPassword();                                                    // Опросить привод и прочитать пароли
                Password();
                if (!gIsNvsaOld)
                {
                    ReqLanguage(); 
                    // Сохраняем при необходимости новое значение языка
                    FM25RD(&OldLang, &nv.OpisDrv[HandlerMenuChangeDrive_struct.Pos].Lang, sizeof(OldLang));
                    if (OldLang != TypeLanguage)
                    {
                        FM25WR(&nv.OpisDrv[HandlerMenuChangeDrive_struct.Pos].Lang, &TypeLanguage, sizeof(TypeLanguage));
                    }
                    // Инициализация единиц измерения
                    initEdIzm(HofAT45);
                    initMessPult(HofAT45);
                }
                // Update values with constant addresses
                UpdateStatusChange(pntDPDriveTmp->AdrParam);
                UpdateMPFiles(pntDPDriveTmp->AdrParam);
                UpdateContrast(pntDPDriveTmp->AdrParam);
                GrfOut(); // Update screen
            }
            if (gIsNvsaOld)
            {
                i = FindVisibleParam(&HandlerMenuPultNew_struct.HOFOld, HandlerMenuPultNew_struct.DMOld, pHOF, HandlerMenuChangeDrive_struct.Pos); //Поиск параметров, если вводились пароли
                if (i != HandlerMenuPultNew_struct.CntParam)
                {                           //Если параметров стало меньше, то позицию нужно сместить на разницу
                    HandlerMenuPultNew_struct.PosBegin = 0;
                    HandlerMenuPultNew_struct.PosCursor = 0;
                    HandlerMenuPultNew_struct.Pos = 0;
                    HandlerMenuPultNew_struct.CntParam = i;
                    HandlerMenuPultNew_struct.pntDPMenu = &MDPDrive.DPDrive[0];

                    for (i = 0; i < MAX_STR_OTOBR_PRM &&
                         i < HandlerMenuPultNew_struct.CntParam; i++) {                 //Начальное наполнение дескрипторов меню
                        InitDP(&MDPDrive.DPDrive[i], pHOF, DPGlobalOld[i], NetAdr);
                    }
                    if ( FindGroup(&HandlerMenuPultNew_struct.HOFOld, &DMOld, HofAT45, GRP_STATUS) ){
                        if(MDPDrive.DPDrive[NUM_STATUS].BaseDP = FindParam(&HandlerMenuPultNew_struct.HOFOld, &DMOld, &DPOld, HofAT45, PRM_STATUS)){
                            MDPDrive.DPDrive[NUM_STATUS].AdrNet = NetAdr;
                            MDPDrive.DPDrive[NUM_STATUS].BaseHOF = HofAT45;
                            InitDP(&MDPDrive.DPDrive[NUM_STATUS], pHOF, MDPDrive.DPDrive[NUM_STATUS].BaseDP, NetAdr);
                        }
                        if(MDPDrive.DPDrive[NUM_STATUS_2].BaseDP = FindParam(&HandlerMenuPultNew_struct.HOFOld, &DMOld, &DPOld, HofAT45, PRM_STATUS_2)){
                            MDPDrive.DPDrive[NUM_STATUS_2].AdrNet = NetAdr;
                            MDPDrive.DPDrive[NUM_STATUS_2].BaseHOF = HofAT45;
                            InitDP(&MDPDrive.DPDrive[NUM_STATUS_2], pHOF, MDPDrive.DPDrive[NUM_STATUS_2].BaseDP,  NetAdr);
                        }
                    }
                    HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntParam;
                }
            }
            else
            {
                i = FindVisibleParam(&HandlerMenuPultNew_struct.HOF, HandlerMenuPultNew_struct.DM, pHOF, HandlerMenuChangeDrive_struct.Pos); //Поиск параметров, если вводились пароли
                if (i != HandlerMenuPultNew_struct.CntParam){                           //Если параметров стало меньше, то позицию нужно сместить на разницу
                    HandlerMenuPultNew_struct.PosBegin = 0;
                    HandlerMenuPultNew_struct.PosCursor = 0;
                    HandlerMenuPultNew_struct.Pos = 0;
                    HandlerMenuPultNew_struct.CntParam = i;
                    HandlerMenuPultNew_struct.pntDPMenu = &MDPDrive.DPDrive[0];

                    for (i = 0; i < MAX_STR_OTOBR_PRM &&
                         i < HandlerMenuPultNew_struct.CntParam; i++) {                 //Начальное наполнение дескрипторов меню
                        InitDP(&MDPDrive.DPDrive[i], pHOF, DPGlobal[i], NetAdr);
                    }
                    if ( FindGroup(&HandlerMenuPultNew_struct.HOF, &DM, HofAT45, GRP_STATUS) ){
                        if(MDPDrive.DPDrive[NUM_STATUS].BaseDP = FindParam(&HandlerMenuPultNew_struct.HOF, &DM, &DP, HofAT45, PRM_STATUS)){
                            MDPDrive.DPDrive[NUM_STATUS].AdrNet = NetAdr;
                            MDPDrive.DPDrive[NUM_STATUS].BaseHOF = HofAT45;
                            InitDP(&MDPDrive.DPDrive[NUM_STATUS], pHOF, MDPDrive.DPDrive[NUM_STATUS].BaseDP, NetAdr);
                        }
                        if(MDPDrive.DPDrive[NUM_STATUS_2].BaseDP = FindParam(&HandlerMenuPultNew_struct.HOF, &DM, &DP, HofAT45, PRM_STATUS_2)){
                            MDPDrive.DPDrive[NUM_STATUS_2].AdrNet = NetAdr;
                            MDPDrive.DPDrive[NUM_STATUS_2].BaseHOF = HofAT45;
                            InitDP(&MDPDrive.DPDrive[NUM_STATUS_2], pHOF, MDPDrive.DPDrive[NUM_STATUS_2].BaseDP,  NetAdr);
                        }
                    }
                    HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntParam;
                }

            }
        }   
        else if (HandlerMenuPult_struct.AdditionalMode == AddMODE_RepalceStatus) 
        {    
            if (gIsNvsaOld)
            {
                if ((pntDPDriveTmp->wm.TypeVar == int16Frmt || pntDPDriveTmp->wm.TypeVar == int32Frmt ||
                    pntDPDriveTmp->wm.TypeVar == uns16Frmt || pntDPDriveTmp->wm.TypeVar == uns32Frmt) && pntDPDriveTmp->DPOld.FlgPrm.EditType == NO_EDIT) 
                {
                    tmp = pntDPDriveTmp->DPOld.IDGrp  + (pntDPDriveTmp->DPOld.IDPrm << 8);
                    FM25WR(&nv.OpisDrv[HandlerMenuChangeDrive_struct.Pos].StatusRef[HandlerMenuPult_struct.ParamAddMode], &tmp, sizeof(tmp));
                }

            }
            else
            {
                if ((pntDPDriveTmp->wm.TypeVar == int16Frmt || pntDPDriveTmp->wm.TypeVar == int32Frmt ||
                    pntDPDriveTmp->wm.TypeVar == uns16Frmt || pntDPDriveTmp->wm.TypeVar == uns32Frmt) && pntDPDriveTmp->DP.FlgPrm.EditType == NO_EDIT) 
                {
                    tmp = pntDPDriveTmp->DP.IDGrp  + (pntDPDriveTmp->DP.IDPrm << 8);
                    FM25WR(&nv.OpisDrv[HandlerMenuChangeDrive_struct.Pos].StatusRef[HandlerMenuPult_struct.ParamAddMode], &tmp, sizeof(tmp));
                }
            }
            WordStatusForLed();
            SetModeMenu(MODE_HandlerMenuStatusReplace);
            memset(&HandlerMenuPultNew_struct, 0, sizeof(HandlerMenuPultNew_struct));
        }
    }   
        
    //Разбор запросов к приводу
    RazborRx();
    MessageShow((keyName_type)KeyTemp);
	//Запрос статуса для меню групп
    if (Type == FLASH_OPIS && HandlerMenuPultNew_struct.Mode == MODE_MENU)
    {
        if (MDPDrive.DPDrive[NUM_STATUS].ValidData) 
        {
            statusDRV_tipe      StatusDrv;
            StatusDrv.all = (u16)MDPDrive.DPDrive[NUM_STATUS].wm.s32.Value;
            parseWordStatus(NetAdr, StatusDrv);
            StatusDaleyValid = 0;
        }
        else 
        {
            if ((StatusDaleyValid < 4) && (MDPDrive.DPDrive[NUM_STATUS].ValidDataFirst == 1))
            {
                statusDRV_tipe      StatusDrv;
                StatusDrv.all = (u16)MDPDrive.DPDrive[NUM_STATUS].wm.s32.Value;
                parseWordStatus(NetAdr, StatusDrv);
            }
            else
            {
                //MDPDrive.DPDrive[NUM_STATUS].ValidDataFirst = 1;
                statusDRV_tipe      StatusDrv;
                StatusDrv.all = 0xFFFF;
                parseWordStatus(MDPDrive.DPDrive[NUM_STATUS_2].AdrNet, StatusDrv);
                MDPDrive.DPDrive[NUM_STATUS].ValidDataFirst = 0;
            }
            StatusDaleyValid++;
        }
        //*************************************************************
        //Обработка статуса 2
        //*************************************************************
        if (MDPDrive.DPDrive[NUM_STATUS_2].ValidData == 1)
        {
            statusDRV_2_type      StatusDrv2;
            StatusDrv2.all = (u16)MDPDrive.DPDrive[NUM_STATUS_2].wm.s32.Value;
            parseWordStatus2(MDPDrive.DPDrive[NUM_STATUS_2].AdrNet, StatusDrv2);
            StatusDaleyValid2 = 0;
        }
        else
        {
            if ((StatusDaleyValid2 < 4) && (MDPDrive.DPDrive[NUM_STATUS_2].ValidDataFirst == 1))
            {
                statusDRV_2_type      StatusDrv2;
                StatusDrv2.all = (u16)MDPDrive.DPDrive[NUM_STATUS_2].wm.s32.Value;
                parseWordStatus2(MDPDrive.DPDrive[NUM_STATUS_2].AdrNet, StatusDrv2);
            }
            else
            {
                //MDPDrive.DPDrive[NUM_STATUS_2].ValidDataFirst = 1;
                statusDRV_tipe      StatusDrv2;
                StatusDrv2.all = 0xFFFF;
                parseWordStatus(MDPDrive.DPDrive[NUM_STATUS_2].AdrNet, StatusDrv2);
                MDPDrive.DPDrive[NUM_STATUS_2].ValidDataFirst = 0;
            }
            StatusDaleyValid2++;
        }
        OSQQuery(QueueDrvIn, &QData);
        if(QData.OSNMsgs == 0)
        {
            ModbusMakeOpis(&MDPDrive.DPDrive[NUM_STATUS], READ_UST);
            ModbusMakeOpis(&MDPDrive.DPDrive[NUM_STATUS_2], READ_UST);
        }
    }
    GrfOut();
}
// **************************************************************************
//  			Поиск смещения для языкового сектора
// **************************************************************************
void *getPntForLng(void *BaseHOF, HeadOfFile_type *HOF)
{
    void        *Adr;
    HeadOfLanguage_type HOL;
    u8          i;
    
    Adr = (void *)((u32)BaseHOF + (HOF->Pnt1LngTxt << 1));// + (FDP->OfsName << 1));
    
    #ifdef BLOCK_STM_MEMORY
    AT45RD(&HOL, Adr, sizeof(HeadOfLanguage_type));
    #else            
    memcpy(&HOL, (void *)((u32)Adr + STM32_FLASH_OFFSET), sizeof(HeadOfLanguage_type)); //  *!* Загрузка из памяти STM по сдвигу адреса               
    #endif

    for (i = 0; i < TypeLanguage; i++) //|| (FDP->DP.LenHlp != 0))
    {
        Adr = (void *)((u32)Adr + (HOL.SectSize << 1));// + (FDP->OfsName << 1));
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&HOL, Adr, sizeof(HeadOfLanguage_type));
        #else            
        memcpy(&HOL, (void *)((u32)Adr + STM32_FLASH_OFFSET), sizeof(HeadOfLanguage_type)); //  *!* Загрузка из памяти STM по сдвигу адреса               
        #endif
    }
    return Adr;
}
// **************************************************************************
//  			Инициализация единиц измерения
// **************************************************************************
void initEdIzm(void *BaseHOF)
{
    void                *Adr;
    void                *Adr_temp;
    u8                  i;
    HeadOfLanguage_type HOL;
    HeadOfFile_type     HOF;
    
    if (gIsNvsaOld)
    {
        for (i = 0; i < COUNT_ED_IZM; i++)
        {
            sprintf(&MasEdIzmTek[i][0], msgMasEdIzm[i]);
        }
    }
    else
    {
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&HOF, BaseHOF, sizeof(HeadOfFile_type));
        #else            
        memcpy(&HOF, (void *)((u32)BaseHOF+STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса
        #endif
        
        Adr = getPntForLng(BaseHOF, &HOF);
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&HOL, Adr, sizeof(HeadOfLanguage_type));
        #else            
        memcpy(&HOL, (void *)((u32)Adr+STM32_FLASH_OFFSET), sizeof(HeadOfLanguage_type)); //  *!* Загрузка из памяти STM по сдвигу адреса        
        #endif

        Adr_temp = (void *)((u32)Adr + (HOL.PntTxtEdIzm << 1));
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&MasEdIzmTek[0][0], Adr_temp, COUNT_ED_IZM * LEN_ED_IZM);
        #else            
        memcpy(&MasEdIzmTek[0][0], (void *)((u32)Adr_temp+STM32_FLASH_OFFSET), COUNT_ED_IZM * LEN_ED_IZM); //  *!* Загрузка из памяти STM по сдвигу адреса        
        #endif
    }
}
// **************************************************************************
//  			Инициализация текстов пульта
// **************************************************************************
void initMessPult(void *BaseHOF)
{
    void                *Adr;
    void                *Adr_temp;
    HeadOfLanguage_type HOL;
    HeadOfFile_type     HOF;
    u8                  i;
    DescriptorMessPult_type DescriptorMessPult;
    DescriptorStr_type    DescriptorStr;

    if (BaseHOF && !gIsNvsaOld)
    {
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&HOF, BaseHOF, sizeof(HeadOfFile_type));
        #else            
        memcpy(&HOF, (void *)((u32)BaseHOF+STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса              
        #endif

        Adr = getPntForLng(BaseHOF, &HOF);
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&HOL, Adr, sizeof(HeadOfLanguage_type));
        #else            
        memcpy(&HOL, (void *)((u32)Adr+STM32_FLASH_OFFSET), sizeof(HeadOfLanguage_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                
        #endif

        // Считывание обычных сообщений
        Adr_temp = (void *)((u32)BaseHOF + (HOF.MessPultStart << 1));
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&DescriptorMessPult, Adr_temp, sizeof(DescriptorMessPult_type));
        #else            
        memcpy(&DescriptorMessPult, (void *)((u32)Adr_temp+STM32_FLASH_OFFSET), sizeof(DescriptorMessPult_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                
        #endif
    }
    else
    {
        DescriptorMessPult.QuantityMessPult = 0;
    }
    for (i = 0; i < COUNT_MESS; i++)
    {
        if (DescriptorMessPult.QuantityMessPult && (HOF.QuantityMessPult >= ID_MAS_MESS))
        {
            Adr_temp = (void *)((u32)Adr + sizeof(HeadOfLanguage_type) + (HOL.QuantityHelp + DescriptorMessPult.OfsText + i) * sizeof(DescriptorStr_type));
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&DescriptorStr, Adr_temp, sizeof(DescriptorStr_type));
            #else            
            memcpy(&DescriptorStr, (void *)((u32)Adr_temp+STM32_FLASH_OFFSET), sizeof(DescriptorStr_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                
            #endif

            if (DescriptorStr.LenString)
            {
                Adr_temp = (void *)((u32)Adr + (HOL.PntTxtMess << 1) + (DescriptorStr.PntString << 1));
                #ifdef BLOCK_STM_MEMORY
                AT45RD(&MasMess[i][0], Adr_temp, DescriptorStr.LenString);
                #else            
                memcpy(&MasMess[i][0], (void *)((u32)Adr_temp+STM32_FLASH_OFFSET), DescriptorStr.LenString); //  *!* Загрузка из памяти STM по сдвигу адреса                
                #endif
                MasMess[i][DescriptorStr.LenString] = 0;
            }
            else
            {
                MasMess[i][0] = 0;
            }
        }
        else
        {
            sprintf(&MasMess[i][0], msgStringsMess[i]);
        }
    }
    // Считывание Титулов
    if (BaseHOF  && !gIsNvsaOld)
    {
        Adr_temp = (void *)((u32)BaseHOF + (HOF.MessPultStart << 1) + sizeof(DescriptorMessPult_type));
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&DescriptorMessPult, Adr_temp, sizeof(DescriptorMessPult_type));
        #else            
        memcpy(&DescriptorMessPult, (void *)((u32)Adr_temp+STM32_FLASH_OFFSET), sizeof(DescriptorMessPult_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                
        #endif
    }
    else
    {
        DescriptorMessPult.QuantityMessPult = 0;
    }
    for (i = 0; i < COUNT_TITLE; i++)
    {
        if (DescriptorMessPult.QuantityMessPult && (HOF.QuantityMessPult >= ID_MAS_MESS))
        {
            Adr_temp = (void *)((u32)Adr + sizeof(HeadOfLanguage_type) + (HOL.QuantityHelp + DescriptorMessPult.OfsText + i) * sizeof(DescriptorStr_type));
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&DescriptorStr, Adr_temp, sizeof(DescriptorStr_type));
            #else            
            memcpy(&DescriptorStr, (void *)((u32)Adr_temp+STM32_FLASH_OFFSET), sizeof(DescriptorStr_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                
            #endif

            if (DescriptorStr.LenString)
            {
                Adr_temp = (void *)((u32)Adr + (HOL.PntTxtMess << 1) + (DescriptorStr.PntString << 1));
                #ifdef BLOCK_STM_MEMORY
                AT45RD(&MasTitle[i][0], Adr_temp, DescriptorStr.LenString);
                #else            
                memcpy(&MasTitle[i][0], (void *)((u32)Adr_temp+STM32_FLASH_OFFSET), DescriptorStr.LenString); //  *!* Загрузка из памяти STM по сдвигу адреса                
                #endif

                MasTitle[i][DescriptorStr.LenString] = 0;
            }
            else
            {
                MasTitle[i][0] = 0;
            }
        }
        else
        {
            sprintf(&MasTitle[i][0], msgStringsTitle[i]);
        }
    }
    // Считывание всплывающих сообщений 
    if (BaseHOF  && !gIsNvsaOld)
    {
        Adr_temp = (void *)((u32)BaseHOF + (HOF.MessPultStart << 1) + sizeof(DescriptorMessPult_type)*2);
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&DescriptorMessPult, Adr_temp, sizeof(DescriptorMessPult_type));
        #else            
        memcpy(&DescriptorMessPult, (void *)((u32)Adr_temp+STM32_FLASH_OFFSET), sizeof(DescriptorMessPult_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                        
        #endif
    }
    else
    {
        DescriptorMessPult.QuantityMessPult = 0;
    }
    for (i = 0; i < COUNT_FLASH; i++)
    {
        if (i >= DescriptorMessPult.QuantityMessPult)  
          continue;
        if (DescriptorMessPult.QuantityMessPult && (HOF.QuantityMessPult >= ID_MAS_MESS))
        {
            Adr_temp = (void *)((u32)Adr + sizeof(HeadOfLanguage_type) + (HOL.QuantityHelp + DescriptorMessPult.OfsText + i) * sizeof(DescriptorStr_type));
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&DescriptorStr, Adr_temp, sizeof(DescriptorStr_type));
            #else            
            memcpy(&DescriptorStr, (void *)((u32)Adr_temp+STM32_FLASH_OFFSET), sizeof(DescriptorStr_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                
            #endif

            if (DescriptorStr.LenString)
            {
                Adr_temp = (void *)((u32)Adr + (HOL.PntTxtMess << 1) + (DescriptorStr.PntString << 1));
                #ifdef BLOCK_STM_MEMORY
                AT45RD(&MasFlash[i][0], Adr_temp, DescriptorStr.LenString);
                #else            
                memcpy(&MasFlash[i][0], (void *)((u32)Adr_temp+STM32_FLASH_OFFSET), DescriptorStr.LenString); //  *!* Загрузка из памяти STM по сдвигу адреса                
                #endif

                MasFlash[i][DescriptorStr.LenString] = 0;
            }
            else
            {
                MasFlash[i][0] = 0;
            }
        }
        else
        {
            sprintf(&MasFlash[i][0], msgStringsAlarm[i]);
        }
    }
}
// **************************************************************************
//  			Поиск помощи для параметра
// **************************************************************************
u32 getHelpPrm(Full_DescriptorPrm_type *FDP, char *str)
{
    void        *Adr;
    
    if ((FDP->DPOld.OfsHlp != 0xFFFF) || (FDP->DPOld.LenHlp != 0))
    {
        Adr = (void *)((u32)FDP->BaseHOF + FDP->DPOld.OfsHlp);
    }
    else
    {
        str[0] = 0;
        return FDP->DPOld.LenHlp;
    }

    #ifdef BLOCK_STM_MEMORY
    AT45RD(str, Adr, FDP->DPOld.LenHlp);
    #else            
     memcpy(str, (void *)((u32)Adr+STM32_FLASH_OFFSET), FDP->DPOld.LenHlp); //  *!* Загрузка из памяти STM по сдвигу адреса                
    #endif

    str[FDP->DPOld.LenHlp - 1] = 0;

    return FDP->DPOld.LenHlp;
}
// **************************************************************************
//  				Поиск помощи
// **************************************************************************
u32 getHelp(void *BaseHOF, char *str, u16 OfsHlp)
{
    void                *Adr;
    DescriptorStr_type  DescriptorStr;
    HeadOfLanguage_type HOL;
    HeadOfFile_type     HOF;
    
    if ((OfsHlp != 0xFFFF))
    {
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&HOF, BaseHOF, sizeof(HeadOfFile_type));
        #else            
        memcpy(&HOF, (void *)((u32)BaseHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                  
        #endif

        Adr = getPntForLng(BaseHOF, &HOF);
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&HOL, Adr, sizeof(HeadOfLanguage_type));
        #else            
        memcpy(&HOL, (void *)((u32)Adr + STM32_FLASH_OFFSET), sizeof(HeadOfLanguage_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                                  
        #endif

        Adr = (void *)((u32)Adr + sizeof(HeadOfLanguage_type) + OfsHlp * (sizeof(DescriptorStr_type)));
    }
    else
    {
        str[0] = 0;
        return 0;
    }

    #ifdef BLOCK_STM_MEMORY
    AT45RD(&DescriptorStr, Adr, sizeof(DescriptorStr_type));
    #else            
    memcpy(&DescriptorStr, (void *)((u32)Adr + STM32_FLASH_OFFSET), sizeof(DescriptorStr_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                          
    #endif

    Adr = getPntForLng(BaseHOF, &HOF);
    if (DescriptorStr.LenString)
    {
        Adr = (void *)((u32)Adr + (HOL.PntTxtHelp << 1) + (DescriptorStr.PntString << 1));
        #ifdef BLOCK_STM_MEMORY
        AT45RD(str, Adr, DescriptorStr.LenString);
        #else            
        memcpy(str, (void *)((u32)Adr + STM32_FLASH_OFFSET), DescriptorStr.LenString); //  *!* Загрузка из памяти STM по сдвигу адреса                          
        #endif

        str[DescriptorStr.LenString] = 0;
    }
    else
    {
        str[0] = 0;
        return 0;
    }
    return DescriptorStr.LenString;
}
// **************************************************************************
//  			Поиск названия для параметра
// **************************************************************************
u32 getNamePrm(void *BaseHOF, void *HOF, void *FDP, char *str)
{
    void                    *Adr;
    u8                      len;
    HeadOfLanguage_type     HOL;
    DescriptorPrm_type      *pDP;
    DescriptorPrmOld_type   *pDPOld;
    HeadOfFile_type         *pHOF;
    
    if (gIsNvsaOld)
    {
        pDPOld = FDP;
        memcpy(str, pDPOld->Name, sizeof(pDPOld->Name));
        len = sizeof(pDPOld->Name);
    }
    else
    {
        pHOF = HOF;
        pDP = FDP;
        if ((pDP->OfsName != 0xFFFF))
        {
            Adr = getPntForLng(BaseHOF, pHOF);
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&HOL, Adr, sizeof(HeadOfLanguage_type));
            #else            
            memcpy(&HOL, (void *)((u32)Adr + STM32_FLASH_OFFSET), sizeof(HeadOfLanguage_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                          
            #endif
        }
        else
        {
            str[0] = 0;
            return 0;
        }
        Adr = (void *)((u32)Adr + (HOL.PntTxtPrm << 1) + (pDP->OfsName << 1));
        #ifdef BLOCK_STM_MEMORY
        AT45RD(str, Adr, pHOF->MaxLengthName);
        #else            
        memcpy(str, (void *)((u32)Adr + STM32_FLASH_OFFSET), pHOF->MaxLengthName); //  *!* Загрузка из памяти STM по сдвигу адреса                          
        #endif

        str[pHOF->MaxLengthName] = 0;
        len = pHOF->MaxLengthName;
    }
    return len;
}
// **************************************************************************
//  			Поиск названия для группы
// **************************************************************************
u32 getNameMenu(void *BaseHOF, void *HOF, void *FDM, char *str)
{
    void                    *Adr;
    u8                      len;
    HeadOfLanguage_type     HOL;
    DescriptorMenu_type     *pDM;
    DescriptorMenuOld_type  *pDMOld;
    HeadOfFile_type         *pHOF;
    
    if (gIsNvsaOld)
    {
        pDMOld = FDM;
        memcpy(str, pDMOld->Name, sizeof(pDMOld->Name));
        len = sizeof(pDMOld->Name);
    }
    else
    {
        pHOF = HOF;
        pDM = FDM;
        if ((pDM->OfsName != 0xFFFF))
        {
            Adr = getPntForLng(BaseHOF, pHOF);
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&HOL, Adr, sizeof(HeadOfLanguage_type));
            #else            
            memcpy(&HOL, (void *)((u32)Adr + STM32_FLASH_OFFSET), sizeof(HeadOfLanguage_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                          
            #endif
        }
        else
        {
            str[0] = 0;
            return 0;
        }
        if (pDM->FlgMenu.TypeMenu == 1)
        {
            Adr = (void *)((u32)Adr + (HOL.PntTxtMGrp << 1) + (pDM->OfsName << 1));
        }
        else
        {
            Adr = (void *)((u32)Adr + (HOL.PntTxtGrp << 1) + (pDM->OfsName << 1));
        }
        
        #ifdef BLOCK_STM_MEMORY
        AT45RD(str, Adr, pHOF->MaxLengthName);
        #else            
        memcpy(str, (void *)((u32)Adr + STM32_FLASH_OFFSET), pHOF->MaxLengthName); //  *!* Загрузка из памяти STM по сдвигу адреса                          
        #endif

        str[pHOF->MaxLengthName] = 0;
        len = pHOF->MaxLengthName;
    }
    return len;
}
// Функция чтения значения текстового параметра
inline void ReadValueTextPrm(void *BaseHOF, void *HOF, Full_DescriptorPrm_type *DPDrive, char *Str, s32 NumPrm)
{
    void                *Adr;  
    HeadOfLanguage_type HOL;
    HeadOfFile_type     *pHOF;
    
    if (gIsNvsaOld)
    {
        Adr = (void *)((u32)DPDrive->BaseHOF + DPDrive->DPOld.OfsTxt + NumPrm * 16);
        #ifdef BLOCK_STM_MEMORY
        AT45RD(Str, Adr, 16);
        #else            
        memcpy(Str, (void *)((u32)Adr + STM32_FLASH_OFFSET), 16); //  *!* Загрузка из памяти STM по сдвигу адреса                          
        #endif
    }
    else
    {   
        pHOF = HOF;
        if ((DPDrive->DP.OfsTxt != 0xFFFF))
        {
            Adr = getPntForLng(BaseHOF, pHOF);
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&HOL, Adr, sizeof(HeadOfLanguage_type));
            #else            
            memcpy(&HOL, (void *)((u32)Adr + STM32_FLASH_OFFSET), sizeof(HeadOfLanguage_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                          
            #endif

            Adr = (void *)((u32)Adr + (HOL.PntTxtTxt << 1) + (DPDrive->DP.OfsTxt << 1) + NumPrm * 16);
            #ifdef BLOCK_STM_MEMORY
            AT45RD(Str, Adr, 16);
            #else            
            memcpy(Str, (void *)((u32)Adr + STM32_FLASH_OFFSET), 16); //  *!* Загрузка из памяти STM по сдвигу адреса                          
            #endif

            Str[16] = 0;
        }
        else
        {
            Str[0] = 0;
        }
    }
}
// **************************************************************************
//  Поиск приводов и вываливание их в меню
// Входные данные:
// - нажатая кнопка;
// **************************************************************************
void HandlerMenuChangeDrive(u16 Key)
{
    u32             i;

    for (i = 0; i < MAXDRIVES_CUT; i++) 
    {
        if (ReadLnk(&HandlerMenuChangeDrive_struct.LnkNetFDP, i)) 
        {
            HandlerMenuChangeDrive_struct.AdrFRAM[i] = HandlerMenuChangeDrive_struct.LnkNetFDP.Lnk;
            HandlerMenuChangeDrive_struct.NetAdr[i] = HandlerMenuChangeDrive_struct.LnkNetFDP.NetAdr;
            HandlerMenuChangeDrive_struct.MaxDrv = i + 1;
        }
        else if (i == 0)
        {
            MessagePopUp(GetStringMess(MessageMess_DRIVE_NO_SETUP));
            SetModeMenu(MODE_HandlerMenuPultNewWithDrive);
            return;
        }
        else if (i == 1) {
            SetModeMenu(HandlerMenuPult_struct.SwitchMode);
            return;
        }
        else break;
    }

    GrafBffClear();

    switch(Key){
        case KEY_DOWN:
            if (HandlerMenuChangeDrive_struct.Pos < HandlerMenuChangeDrive_struct.MaxDrv - 1) {
                HandlerMenuChangeDrive_struct.Pos++;
                HandlerMenuChangeDrive_struct.PosCursor = HandlerMenuChangeDrive_struct.Pos;
                if (HandlerMenuChangeDrive_struct.PosCursor >= HandlerMenuChangeDrive_struct.PosBegin + MAX_STR_OTOBR_MENU) {
                    HandlerMenuChangeDrive_struct.PosBegin++;
                }
            }
            break;
        case KEY_UP:
            if (HandlerMenuChangeDrive_struct.Pos > 0) {
                HandlerMenuChangeDrive_struct.Pos--;
                HandlerMenuChangeDrive_struct.PosCursor = HandlerMenuChangeDrive_struct.Pos;
                if (HandlerMenuChangeDrive_struct.PosCursor < HandlerMenuChangeDrive_struct.PosBegin) {
                    HandlerMenuChangeDrive_struct.PosBegin--;
                }
            }
            break;
        case KEY_RIGHT:
            SetModeMenu(HandlerMenuPult_struct.SwitchMode);
            break;

        case KEY_LEFT:
            SetModeMenu(MODE_HandlerMenuPultNewWithDrive);
            break;

        case KEY_ENTER:
            break;
        default: break;
    }

    HandlerMenuChangeDrive_struct.Ypos = 0;
    for (i = HandlerMenuChangeDrive_struct.PosBegin; i < HandlerMenuChangeDrive_struct.MaxDrv && i < MAX_STR_OTOBR_MENU; i++) {
        sprintf(&StrOut[0], "%s %d", "Привод № ", HandlerMenuChangeDrive_struct.NetAdr[i]);

        if (i == HandlerMenuChangeDrive_struct.PosCursor){
            StrOutFont(MENUX, HandlerMenuChangeDrive_struct.Ypos += INCY, INV, &StrOut[0], &fontNormal, leftAlign );
        }
        else{
            StrOutFont(MENUX, HandlerMenuChangeDrive_struct.Ypos += INCY, NO_INV, &StrOut[0], &fontNormal, leftAlign );
        }
    }

    GrfOut();
}
// **************************************************************************
//  Поиск видимых меню и складывание указателей на меню в глобальных массив
//  Входные данные:
//  - ОЗУ указатель на вычитанных HOF;
//  - адрес во флеши HOF, если это меню привода
//  - тип - 0-описатель меню пульта, 1-описатель меню привода (во флеши)
// **************************************************************************
u32 FindVisibleMenu(void *HOF, void *BaseHOF, u16 NumLnk, u16 NumMenu, TypeVisible_type TypeVisible)
{
    DescriptorMenu_type     DM;
    DescriptorMenu_type     DM_Temp;
    DescriptorMenuOld_type  DMOld;
    DescriptorMenuOld_type  DMOld_Temp;
    void                    *AdrMenu;
    u32                     CntMenu;
    u32                     CntParam;
    u16                     NumEndMenu;
    u16                     InStartMenu;
    u16                     CountMenuInto = 0;
    u16                     CountMenuIntoTemp;
    u8                      MarkRoot = 0;
    u16                     i, j;
    u16                     CountInterval;
    HeadOfFile_type         *pHOF;
    HeadOfFileOld_type      *pHOFOld;

    if (gIsNvsaOld)
    {
        pHOFOld = HOF;
        memset(&DMGlobalOld[0], 0, sizeof(DMGlobalOld));
        // Если меню корневое
        if (NumMenu == ROOT_MENU)
        {
            MarkRoot = 1;
            // Номер последнего меню соответствует количеству меню в корне
            NumMenu = 0;
            NumEndMenu = pHOFOld->QuantityMenu;
        }
        else
        {
            AdrMenu = GetOpisMenu(pHOFOld, &DMOld_Temp, BaseHOF, NumMenu);
            if (DMOld_Temp.FlgMenu.TypeMenu == TypeMenu_MGRP_GRPS)
            {
                NumEndMenu = DMOld_Temp.QuantityPrmMenu;
                AdrMenu = GetOpisMenuGrp(pHOFOld, &DMOld_Temp, BaseHOF, 0, DMOld_Temp.AdrNprm, &NumMenu);
            }
            else
            {
                 return 0;
            }
        }
        for (i = NumMenu, CntMenu = 0; i < (NumMenu + NumEndMenu) && i < sizeof(DMGlobal)/sizeof(DMGlobal[0]); i++) 
        {
            if (!MarkRoot)
            {
                CountInterval = i + CountMenuInto;
            }
            else
            {
                CountInterval = i;
            }
            AdrMenu = GetOpisMenu(pHOFOld, &DMOld, BaseHOF, CountInterval);
            if(AdrMenu) 
            {  
                CntParam = 0;
                if (DMOld.FlgMenu.TypeMenu == TypeMenu_MGRP_GRPS)
                {
                    if (!MarkRoot)
                    {
                        InStartMenu = CountInterval + 1;
                    }
                    else
                    {
                        InStartMenu = NumEndMenu + CountMenuInto;
                    }
                    CountMenuIntoTemp = 0;
                    for (j = InStartMenu; j < (InStartMenu + DMOld.QuantityPrmMenu); j++) 
                    {
                        CntParam += FindVisibleMenu_Direct(pHOFOld, BaseHOF, NumLnk, j + CountMenuIntoTemp, DMOld.AdrNprm, TypeVisible, &CountMenuIntoTemp);
                        CountMenuInto++;
                    }
                    CountMenuInto += CountMenuIntoTemp;
                }
                else if (DM.FlgMenu.TypeMenu == TypeMenu_MJRN)
                {
                    CntParam++;
                }
                else
                {
                    if (TypeVisible == TypeVisible_ALL)
                    {
                        CntParam = FindVisibleParam(pHOFOld, &DMOld, BaseHOF, NumLnk);
                    }
                    else
                    {
                        CntParam = FindVisibleTekParam(pHOFOld, &DMOld, BaseHOF, NumLnk);
                    }
                }
                if (CntParam)
                {
                    DMNumGlobalHOF[CntMenu] = CountInterval;
                    DMGlobalOld[CntMenu] = AdrMenu;
                    CntMenu++;
                }
            }
        }
    }
    else
    {
        pHOF = HOF;
        memset(&DMGlobal[0], 0, sizeof(DMGlobal));
        // Если меню корневое
        if (NumMenu == ROOT_MENU)
        {
            MarkRoot = 1;
            // Номер последнего меню соответствует количеству меню в корне
            NumMenu = 0;
            NumEndMenu = pHOF->QuantityMenu;
        }
        else
        {
            AdrMenu = GetOpisMenu(pHOF, &DM_Temp, BaseHOF, NumMenu);
            if (DM_Temp.FlgMenu.TypeMenu == TypeMenu_MGRP_GRPS)
            {
                NumEndMenu = DM_Temp.QuantityPrmMenu;
                AdrMenu = GetOpisMenuGrp(pHOF, &DM_Temp, BaseHOF, 0,DM_Temp.AdrNprm, &NumMenu);
            }
            else
            {
                 return 0;
            }
        }
        for (i = NumMenu, CntMenu = 0; i < (NumMenu + NumEndMenu) && i < sizeof(DMGlobal)/sizeof(DMGlobal[0]); i++) 
        {
            if (!MarkRoot)
            {
                CountInterval = i + CountMenuInto;
            }
            else
            {
                CountInterval = i;
            }
            AdrMenu = GetOpisMenu(pHOF, &DM, BaseHOF, CountInterval);
            if(AdrMenu) 
            {  
                CntParam = 0;
                if (DM.FlgMenu.TypeMenu == TypeMenu_MGRP_GRPS)
                {
                    if (!MarkRoot)
                    {
                        InStartMenu = CountInterval + 1;
                    }
                    else
                    {
                        InStartMenu = NumEndMenu + CountMenuInto;
                    }
                    CountMenuIntoTemp = 0;
                    for (j = InStartMenu; j < (InStartMenu + DM.QuantityPrmMenu); j++) 
                    {
                        CntParam += FindVisibleMenu_Direct(pHOF, BaseHOF, NumLnk, j + CountMenuIntoTemp, DM.AdrNprm, TypeVisible, &CountMenuIntoTemp);
                        CountMenuInto++;
                    }
                    CountMenuInto += CountMenuIntoTemp;
                }
                else if (DM.FlgMenu.TypeMenu == TypeMenu_MJRN)
                {
                    CntParam++;
                }
                else
                {
                    if (TypeVisible == TypeVisible_ALL)
                    {
                        CntParam = FindVisibleParam(pHOF, &DM, BaseHOF, NumLnk);
                    }
                    else
                    {
                        CntParam = FindVisibleTekParam(pHOF, &DM, BaseHOF, NumLnk);
                    }
                }
                if (CntParam)
                {
                    DMNumGlobalHOF[CntMenu] = CountInterval;
                    DMGlobal[CntMenu] = AdrMenu;
                    CntMenu++;
                }
            }
        }
    }
    return CntMenu;
}
// **************************************************************************
//  Поиск видимых п-ров и складывание указателей на  в глобальных массив п-ров
//  Входные данные:
//  - ОЗУ указатель на вычитанных HOF;
//  - адрес во флеши HOF, если это меню привода
//  - тип - 0-описатель меню пульта, 1-описатель меню привода (во флеши)
// **************************************************************************
u32   FindVisibleTekParam(void *HOF, void *DM, void *BaseHOF, u16 NumLnk)
{
    DescriptorPrm_type  	DP;
    DescriptorPrmOld_type  	DPOld;
    u32                 	CntParam;
    void                	*AdrParam;
    void                	*AdrFunct;
    FileFunctional_type 	Functional;
    u16                 	i, tmp;
    HeadOfFile_type         *pHOF;
    HeadOfFileOld_type      *pHOFOld;
    DescriptorMenu_type  	*pDM;
    DescriptorMenuOld_type  *pDMOld;

    if (NumLnk > MAXDRIVES_CUT - 1) 
    {
        return 0;
    }
    AdrFunct = &nv.OpisDrv[NumLnk].Functional;
    FM25RD(&Functional, AdrFunct, sizeof(Functional));

    tmp = 0;
    if (gIsNvsaOld)
    {
        pHOFOld = HOF;
        pDMOld = DM;
        memset(&DPGlobalOld[0], 0, sizeof(DPGlobalOld));
        for (i = 0, CntParam = 0; i < pDMOld->QuantityPrmMenu && i < sizeof(DPGlobalOld)/sizeof(DPGlobalOld[0]); i++) 
        {
            AdrParam = GetOpisParam(pHOFOld, pDMOld, &DPOld, BaseHOF, i);
            if (AdrParam && DPOld.FlgPrm.TypeVar != sec2Frmt) 
            {
                if (( (DPOld.Functional & (Functional.curFunct.functMask >> 8 )) != 0 )) 
                {
                    if (DPOld.FlgPrm.EditType == NO_EDIT &&
                        (DPOld.FlgPrm.TypeVar == int16Frmt || DPOld.FlgPrm.TypeVar == int32Frmt ||
                        DPOld.FlgPrm.TypeVar == uns32Frmt || DPOld.FlgPrm.TypeVar == uns16Frmt))
                    {
                        if (DPOld.FlgPrm.MasterVis == 1 && DPOld.FlgPrm.TexnVis == 1 && DPOld.FlgPrm.UserVis == 1)
                        {
                            tmp = 1;
                        }
                        else if (DPOld.FlgPrm.UserVis == 1 && LevelAccess)
                        {
                            tmp = 1;
                        }
                        else if(DPOld.FlgPrm.MasterVis  == 1 && LevelAccess == MASTER_PASSW ||
                            DPOld.FlgPrm.TexnVis        == 1 && LevelAccess == TEXN_PASSW)
                        {
                            tmp = 0;
                        }
                    }
                }
            }
            if (tmp != 0) 
            {
                tmp = 0;
                DPGlobalOld[CntParam] = AdrParam;
                CntParam ++;
            }
        }
    }
    else
    {
        pHOF = HOF;
        pDM = DM;
        memset(&DPGlobal[0], 0, sizeof(DPGlobal));
        for (i = 0, CntParam = 0; i < pDM->QuantityPrmMenu && i < sizeof(DPGlobal)/sizeof(DPGlobal[0]); i++) 
        {
            AdrParam = GetOpisParam(pHOF, pDM, &DP, BaseHOF, i);
            if (AdrParam && DP.FlgPrm.TypeVar != sec2Frmt) 
            {
                if (( (DP.Functional & (Functional.curFunct.functMask >> 8 )) != 0 )) 
                {
                    if (DP.FlgPrm.EditType == NO_EDIT &&
                        (DP.FlgPrm.TypeVar == int16Frmt || DP.FlgPrm.TypeVar == int32Frmt ||
                        DP.FlgPrm.TypeVar == uns32Frmt || DP.FlgPrm.TypeVar == uns16Frmt))
                    {
                        if (DP.FlgPrm.MasterVis == 1 && DP.FlgPrm.TexnVis == 1 && DP.FlgPrm.UserVis == 1)
                        {
                            tmp = 1;
                        }
                        else if (DP.FlgPrm.UserVis == 1 && LevelAccess)
                        {
                            tmp = 1;
                        }
                        else if(DP.FlgPrm.MasterVis  == 1 && LevelAccess == MASTER_PASSW ||
                            DP.FlgPrm.TexnVis        == 1 && LevelAccess == TEXN_PASSW)
                        {
                            tmp = 0;
                        }
                    }
                }
            }
            if (tmp != 0) 
            {
                tmp = 0;
                DPGlobal[CntParam] = AdrParam;
                CntParam ++;
            }
        }
    }
    return CntParam;
}
// **************************************************************************
//  Поиск видимых п-ров и складывание указателей на  в глобальных массив п-ров
//  Входные данные:
//  - ОЗУ указатель на вычитанных HOF;
//  - адрес во флеши HOF, если это меню привода
//  - тип - 0-описатель меню пульта, 1-описатель меню привода (во флеши)
// **************************************************************************
u32 FindVisibleParam(void *HOF, void *DM, void *BaseHOF, u16 NumLnk)
{
    DescriptorPrm_type  	DP;
    DescriptorPrmOld_type  	DPOld;
    u32                 	CntParam;
    void                	*AdrParam;
    void                	*AdrFunct;
    FileFunctional_type 	Functional;
    u16                 	i, tmp;
    HeadOfFile_type         *pHOF;
    HeadOfFileOld_type      *pHOFOld;
    DescriptorMenu_type  	*pDM;
    DescriptorMenuOld_type  *pDMOld;

    if (NumLnk > MAXDRIVES_CUT - 1) 
    {
        return 0;
    }
    AdrFunct = &nv.OpisDrv[NumLnk].Functional;
    FM25RD(&Functional, AdrFunct, sizeof(Functional));

    tmp = 0;
    if (gIsNvsaOld)
    {
        pHOFOld = HOF;
        pDMOld = DM;
        memset(&DPGlobalOld[0], 0, sizeof(DPGlobalOld));
        for (i = 0, CntParam = 0; i < pDMOld->QuantityPrmMenu && i < sizeof(DPGlobalOld)/ sizeof(DPGlobalOld[0]) ; i++) 
        {
            AdrParam = GetOpisParam(pHOFOld, pDMOld, &DPOld, BaseHOF, i);
            if (AdrParam && DPOld.FlgPrm.TypeVar != sec2Frmt) 
            {
                if ((DPOld.Functional & (Functional.curFunct.functMask >> 8)) != 0) 
                {
                    if (DPOld.FlgPrm.MasterVis == 1 && DPOld.FlgPrm.TexnVis == 1 && DPOld.FlgPrm.UserVis == 1)
                    {
                        tmp = 1;
                    }
                    else if (DPOld.FlgPrm.UserVis == 1 && LevelAccess)
                    {
                        tmp = 1;
                    }
                    else if(DPOld.FlgPrm.MasterVis  == 1 && LevelAccess == MASTER_PASSW ||
                            DPOld.FlgPrm.TexnVis    == 1 && LevelAccess == TEXN_PASSW)
                    {
                        tmp = 1;
                    }
                    else if (pDMOld->FlgMenu.TypeMenu == 4)
                    {                            //Журнал
                        tmp = 1;
                    }
                }
            }
            if (tmp != 0) 
            {
                tmp = 0;
                DPGlobalOld[CntParam] = AdrParam;
                CntParam ++;
            }
        }
    }
    else
    {
        pHOF = HOF;
        pDM = DM;
        memset(&DPGlobal[0], 0, sizeof(DPGlobal));
        for (i = 0, CntParam = 0; i < pDM->QuantityPrmMenu && i < sizeof(DPGlobal)/ sizeof(DPGlobal[0]) ; i++) 
        {
            AdrParam = GetOpisParam(pHOF, pDM, &DP, BaseHOF, i);
            if (AdrParam && DP.FlgPrm.TypeVar != sec2Frmt) 
            {
                if ((DP.Functional & (Functional.curFunct.functMask >> 8)) != 0) 
                {
                    if (DP.FlgPrm.MasterVis == 1 && DP.FlgPrm.TexnVis == 1 && DP.FlgPrm.UserVis == 1)
                    {
                        tmp = 1;
                    }
                    else if (DP.FlgPrm.UserVis == 1 && LevelAccess)
                    {
                        tmp = 1;
                    }
                    else if(DP.FlgPrm.MasterVis  == 1 && LevelAccess == MASTER_PASSW ||
                            DP.FlgPrm.TexnVis    == 1 && LevelAccess == TEXN_PASSW)
                    {
                        tmp = 1;
                    }
                    else if (pDM->FlgMenu.TypeMenu == 4)
                    {                            //Журнал
                        tmp = 1;
                    }
                }
            }
            if (tmp != 0) 
            {
                tmp = 0;
                DPGlobal[CntParam] = AdrParam;
                CntParam ++;
            }
        }
    }
    return CntParam;
} 
//===================================================================
//===================================================================
void MenuStartOneShotReadAT45(void *BaseHOF, u16 PosCursor, u16 Pos, u16 SizeMenu)
{
    u32                 Ypos;
    u16                 i;
    invers_type         Inv;
    char                strtmp[17];
    HeadOfFile_type     HOF;
    HeadOfFileOld_type  HOFOld;
    //---------------------------------------------------------------------
    //Формирование скроллинга
    //---------------------------------------------------------------------
    strtmp[16] = 0;
    if (SizeMenu > MAX_STR_OTOBR_MENU){
        ScrollBarVert(SizeMenu, MAX_STR_OTOBR_MENU, 16, Pos, 156, 16);
    }
    for (i = 0, Ypos = 0; i < MAX_STR_OTOBR_MENU && i < SizeMenu; i++) {
        if (i == PosCursor){
            Inv = STR_INV;
            sprintf(&StrOut[0], "[%d]", DMDrive[i].IDGrp);
        }
        else Inv = NO_INV;
        if (gIsNvsaOld)
        {
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&HOFOld, BaseHOF, sizeof(HeadOfFileOld_type));
            #else            
            memcpy(&HOFOld, (void *)((u32)BaseHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFileOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                          
            #endif
            getNameMenu(BaseHOF, &HOFOld, &DMDriveOld[i], &strtmp[0]);
        }
        else
        {
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&HOF, BaseHOF, sizeof(HeadOfFile_type));
            #else            
            memcpy(&HOF, (void *)((u32)BaseHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                                      
            #endif

            getNameMenu(BaseHOF, &HOF, &DMDrive[i], &strtmp[0]);
        }
        sprintf(&StrOut[0], "%s", &strtmp[0]);
        StrOutFont(MENUX, Ypos += INCY, Inv, &StrOut[0], &fontNormal, leftAlign );
    }
}
//===================================================================
//===================================================================
void ParamStartOneShotReadAT45(void *BaseHOF, u16 PosCursor, u16 Pos, u16 SizeParam, u16 Type)
{
    u32                     Ypos;
    u16                     i, j;
    invers_type             Inv;
    wm_type                 *wm, wmem;
    OS_Q_DATA               QData;
    char                    strtmp[17];
    const u8                *pntIconEdit;
    static u16              StatusDaleyValid = 0;
    static u16              StatusDaleyValid2 = 0;
    static u16              PrmDaleyValid[ MAX_STR_OTOBR_PRM ] = { 0, 0, 0, 0};
    HeadOfFile_type         HOF;
    HeadOfFileOld_type      HOFOld;


    strtmp[16]  = 0;
    //---------------------------------------------------------------------
    //Формирование скроллинга
    //---------------------------------------------------------------------
    if (SizeParam > MAX_STR_OTOBR_PRM)
    {
        ScrollBarVert(SizeParam, MAX_STR_OTOBR_PRM * 2, 16, Pos, 156, 16);
    }
    wm = &wmem;
    if (gIsNvsaOld)
    {
        for (i = 0, Ypos = 0; i < MAX_STR_OTOBR_PRM && i < SizeParam; i++) 
        {
            if (i == PosCursor)
            {
                Inv = STR_INV;
                if (HandlerMenuPultNew_struct.pntDPMenu->wm.TypeVar != refFormat && 
                    (HandlerMenuPultNew_struct.pntDPMenu->DPOld.FlgPrm.TypeVar != refFormat))
                {
                    sprintf(&StrOut[0], "[%d,%d]",
                        HandlerMenuPultNew_struct.pntDPMenu->DPOld.IDGrp,
                        HandlerMenuPultNew_struct.pntDPMenu->DPOld.IDPrm);
                    StrOutFont(0, 144, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //Вывод внизу номера группы и параметра
                }
                else
                {
                    sprintf(&StrOut[0], "[-,-]");
                    StrOutFont(0, 144, NO_INV, &StrOut[0], &fontNormal, leftAlign ); // вывод прочерков
                }
                switch( HandlerMenuPultNew_struct.pntDPMenu->DPOld.FlgPrm.EditType )
                {  //Обработка статусов доступа
                    case NO_EDIT:       pntIconEdit = InfoPrm;      break;
                    case STOP_EDIT:     pntIconEdit = EditStopPrm;  break;
                    default:            pntIconEdit = EditPrm;      break;
                }
                DrawIcon16x16(136, 145, pntIconEdit );
            }
            else
            {
                Inv = NO_INV;
            }
            if (HandlerMenuPultNew_struct.pntDPMenu->wm.TypeVar == refFormat || HandlerMenuPultNew_struct.pntDPMenu->DPOld.FlgPrm.TypeVar == refFormat)
            {
                sprintf(&StrOut[0], "%s%s", &noValidData[0], &noValidData[0]);
                StrOutFont(MENUX, Ypos += INCY, NO_INV, &StrOut[0], &fontNormal, leftAlign ); // вывод прочерков
            }
            else
            {
                #ifdef BLOCK_STM_MEMORY
                AT45RD(&HOFOld, BaseHOF, sizeof(HeadOfFileOld_type));
                #else            
                memcpy(&HOFOld, (void *)((u32)BaseHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFileOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                          
                #endif

                getNamePrm(BaseHOF, &HOFOld, &HandlerMenuPultNew_struct.pntDPMenu->DPOld, &strtmp[0]);
                sprintf(&StrOut[0], "%s", &strtmp[0]);
                StrOutFont(MENUX, Ypos += INCY, NO_INV, &StrOut[0], &fontNormal, leftAlign ); 	//Вывод параметра
            }
            DrawLine(0, Ypos + 31, 148, Ypos + 31, 1, 3, 3);        				//Рисование линии после вывода параметра

                if (HandlerMenuPultNew_struct.pntDPMenu->ValidData == 1 && 
                    HandlerMenuPultNew_struct.pntDPMenu->wm.TypeVar != refFormat)
                {
                    wm = &HandlerMenuPultNew_struct.pntDPMenu->wm;
                    ParamToStr(BaseHOF, HandlerMenuPultNew_struct.pntDPMenu, wm, &StrOut[0]);
                    sprintf(&strtmp[0], "%15s", &StrOut[0]);
                    //StrOutFont(0, Ypos += INCY, NO_INV, &strtmp[0], &fontNormal, leftAlign );
                    StrOutFont(0, Ypos += INCY, Inv,    &strtmp[0], &fontNormal, leftAlign ); 
                    PrmDaleyValid[i] = 0;
                }
                else
                {
                    if ((PrmDaleyValid[i] < 4) && (HandlerMenuPultNew_struct.pntDPMenu->ValidDataFirst == 1) && 
                        HandlerMenuPultNew_struct.pntDPMenu->wm.TypeVar != refFormat)
                    {
                        wm = &HandlerMenuPultNew_struct.pntDPMenu->wm;
                        ParamToStr(BaseHOF, HandlerMenuPultNew_struct.pntDPMenu, wm, &StrOut[0]);
                        sprintf(&strtmp[0], "%15s", &StrOut[0]);
                        StrOutFont(0, Ypos += INCY, Inv,    &strtmp[0], &fontNormal, leftAlign );    
                    }
                    else
                    {
                        sprintf(&StrOut[0], "%15s", &noValidData[0]);
                        StrOutFont(0, Ypos += INCY, Inv, &StrOut[0], &fontNormal, leftAlign );
                    }
                    PrmDaleyValid[i]++;
                }
            if (HandlerMenuPultNew_struct.pntDPMenu < &MDPDrive.DPDrive[MAX_STR_OTOBR_PRM - 1] &&
                    HandlerMenuPultNew_struct.pntDPMenu < &MDPDrive.DPDrive[SizeParam - 1]) 
            {
                    HandlerMenuPultNew_struct.pntDPMenu++;
            }
            else
            {
                HandlerMenuPultNew_struct.pntDPMenu = &MDPDrive.DPDrive[0];
            }
        }
    }
    else
    {
        for (i = 0, Ypos = 0; i < MAX_STR_OTOBR_PRM && i < SizeParam; i++) 
        {
            if (i == PosCursor)
            {
                Inv = STR_INV;
                if (HandlerMenuPultNew_struct.pntDPMenu->wm.TypeVar != refFormat && 
                    (HandlerMenuPultNew_struct.pntDPMenu->DP.FlgPrm.TypeVar != refFormat))
                {
                    sprintf(&StrOut[0], "[%d,%d]",
                        HandlerMenuPultNew_struct.pntDPMenu->DP.IDGrp,
                        HandlerMenuPultNew_struct.pntDPMenu->DP.IDPrm);
                    StrOutFont(0, 144, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //Вывод внизу номера группы и параметра
                }
                else
                {
                    sprintf(&StrOut[0], "[-,-]");
                    StrOutFont(0, 144, NO_INV, &StrOut[0], &fontNormal, leftAlign ); // вывод прочерков
                }
                switch( HandlerMenuPultNew_struct.pntDPMenu->DP.FlgPrm.EditType )
                {  //Обработка статусов доступа
                    case NO_EDIT:       pntIconEdit = InfoPrm;      break;
                    case STOP_EDIT:     pntIconEdit = EditStopPrm;  break;
                    default:            pntIconEdit = EditPrm;      break;
                }
                DrawIcon16x16(136, 145, pntIconEdit );
            }
            else
            {
                Inv = NO_INV;
            }
            if (HandlerMenuPultNew_struct.pntDPMenu->wm.TypeVar == refFormat || HandlerMenuPultNew_struct.pntDPMenu->DP.FlgPrm.TypeVar == refFormat)
            {
                sprintf(&StrOut[0], "%s%s", &noValidData[0], &noValidData[0]);
                StrOutFont(MENUX, Ypos += INCY, NO_INV, &StrOut[0], &fontNormal, leftAlign ); // вывод прочерков
            }
            else
            {
                #ifdef BLOCK_STM_MEMORY
                AT45RD(&HOF, BaseHOF, sizeof(HeadOfFile_type));
                #else            
                memcpy(&HOF, (void *)((u32)BaseHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                          
                #endif

                getNamePrm(BaseHOF, &HOF, &HandlerMenuPultNew_struct.pntDPMenu->DP, &strtmp[0]);
                sprintf(&StrOut[0], "%s", &strtmp[0]);
                StrOutFont(MENUX, Ypos += INCY, NO_INV, &StrOut[0], &fontNormal, leftAlign ); 	//Вывод параметра
            }
            DrawLine(0, Ypos + 31, 148, Ypos + 31, 1, 3, 3);        				//Рисование линии после вывода параметра

                if (HandlerMenuPultNew_struct.pntDPMenu->ValidData == 1 && 
                    HandlerMenuPultNew_struct.pntDPMenu->wm.TypeVar != refFormat)
                {
                    wm = &HandlerMenuPultNew_struct.pntDPMenu->wm;
                    ParamToStr(BaseHOF, HandlerMenuPultNew_struct.pntDPMenu, wm, &StrOut[0]);
                    sprintf(&strtmp[0], "%15s", &StrOut[0]);
                    //StrOutFont(0, Ypos += INCY, NO_INV, &strtmp[0], &fontNormal, leftAlign );
                    StrOutFont(0, Ypos += INCY, Inv,    &strtmp[0], &fontNormal, leftAlign );
                    PrmDaleyValid[i] = 0;
                }
                else
                {
                    if ((PrmDaleyValid[i] < 4) && (HandlerMenuPultNew_struct.pntDPMenu->ValidDataFirst == 1) && 
                        HandlerMenuPultNew_struct.pntDPMenu->wm.TypeVar != refFormat)
                    {
                        wm = &HandlerMenuPultNew_struct.pntDPMenu->wm;
                        ParamToStr(BaseHOF, HandlerMenuPultNew_struct.pntDPMenu, wm, &StrOut[0]);
                        sprintf(&strtmp[0], "%15s", &StrOut[0]);
                        //StrOutFont(0, Ypos += INCY, NO_INV, &strtmp[0], &fontNormal, leftAlign );
                        StrOutFont(0, Ypos += INCY, Inv,    &strtmp[0], &fontNormal, leftAlign );    
                    }
                    else
                    {
                        sprintf(&StrOut[0], "%15s", &noValidData[0]);
                        //StrOutFont(0, Ypos += INCY, NO_INV, &StrOut[0], &fontNormal, leftAlign );
                        StrOutFont(0, Ypos += INCY, Inv, &StrOut[0], &fontNormal, leftAlign );
                    }
                    PrmDaleyValid[i]++;
                }
            if (HandlerMenuPultNew_struct.pntDPMenu < &MDPDrive.DPDrive[MAX_STR_OTOBR_PRM - 1] &&
                    HandlerMenuPultNew_struct.pntDPMenu < &MDPDrive.DPDrive[SizeParam - 1]) 
            {
                    HandlerMenuPultNew_struct.pntDPMenu++;
            }
            else
            {
                HandlerMenuPultNew_struct.pntDPMenu = &MDPDrive.DPDrive[0];
            }
        }
    }
    //*************************************************************
    //Обработка статуса
    //*************************************************************
    if (MDPDrive.DPDrive[NUM_STATUS].ValidData == 1)
    {
        statusDRV_tipe      StatusDrv;
        StatusDrv.all = (u16)MDPDrive.DPDrive[NUM_STATUS].wm.s32.Value;
        parseWordStatus(MDPDrive.DPDrive[NUM_STATUS].AdrNet, StatusDrv);
        StatusDaleyValid = 0;
    }
    else
    {
        if ((StatusDaleyValid < 4) && (MDPDrive.DPDrive[NUM_STATUS].ValidDataFirst == 1))
        {
            statusDRV_tipe      StatusDrv;
            StatusDrv.all = (u16)MDPDrive.DPDrive[NUM_STATUS].wm.s32.Value;
            parseWordStatus(MDPDrive.DPDrive[NUM_STATUS].AdrNet, StatusDrv);
        }
        else
        {
            statusDRV_tipe      StatusDrv;
            StatusDrv.all = 0xFFFF;
            parseWordStatus(HandlerMenuStatus_struct.NetAdr[i], StatusDrv);
            MDPDrive.DPDrive[NUM_STATUS].ValidDataFirst = 0;
        }
        StatusDaleyValid++;
    }
    //*************************************************************
    //Обработка статуса 2
    //*************************************************************
    if (MDPDrive.DPDrive[NUM_STATUS_2].ValidData == 1)
    {
        statusDRV_2_type      StatusDrv2;
        StatusDrv2.all = (u16)MDPDrive.DPDrive[NUM_STATUS_2].wm.s32.Value;
        parseWordStatus2(MDPDrive.DPDrive[NUM_STATUS_2].AdrNet, StatusDrv2);
        StatusDaleyValid2 = 0;
    }
    else
    {
        if ((StatusDaleyValid2 < 4) && (MDPDrive.DPDrive[NUM_STATUS_2].ValidDataFirst == 1))
        {
            statusDRV_2_type      StatusDrv2;
            StatusDrv2.all = (u16)MDPDrive.DPDrive[NUM_STATUS_2].wm.s32.Value;
            parseWordStatus2(MDPDrive.DPDrive[NUM_STATUS_2].AdrNet, StatusDrv2);
        }
        else
        {
            statusDRV_2_type      StatusDrv2;
            StatusDrv2.all = 0xFFFF;
            parseWordStatus2(MDPDrive.DPDrive[NUM_STATUS_2].AdrNet, StatusDrv2);
            MDPDrive.DPDrive[NUM_STATUS_2].ValidDataFirst = 0;
        }
        StatusDaleyValid2++;
    }
    //---------------------------------------------------------------------
    //Формирование запросов приводу
    //---------------------------------------------------------------------
    OSQQuery(QueueDrvIn, &QData);
    if(QData.OSNMsgs == 0)
    {
        for (j = 0; j < 4 && j < SizeParam; j++) 
        {
            ModbusMakeOpis(&MDPDrive.DPDrive[j], READ_UST);
        }
        ModbusMakeOpis(&MDPDrive.DPDrive[NUM_STATUS], READ_UST);
        ModbusMakeOpis(&MDPDrive.DPDrive[NUM_STATUS_2], READ_UST);
    }  
}
// **************************************************************************
//  функция редактирования параметра пульта и привода
// **************************************************************************
void ParamEdit(void *BaseHOF, Full_DescriptorPrm_type *DPDrive)
{
    wm_type 				*wm, wmem;
    FlagOpisParam_type 		FlagOpisParam;
    u8 						Level;

    wm = &wmem;

    if (DPDrive->ValidData == 0) return;
    wm = &DPDrive->wm;

    *(u32 *)&FlagOpisParam = wm->Flags;
    Level = 0;
    if ( FlagOpisParam.MasterChange && LevelAccess == MASTER_PASSW) {
        Level = 1;
    }
    else if (FlagOpisParam.TexnChange && LevelAccess == TEXN_PASSW) {
        Level = 1;
    }
    else if (FlagOpisParam.UserChange && LevelAccess == USER_PASSW) {
        Level = 1;
    }
    else if (!FlagOpisParam.MasterChange && !FlagOpisParam.TexnChange && !FlagOpisParam.UserChange) {
        Level = 1;
    }

    if (!Level) {
         MessagePopUp(GetStringMess(MessageMess_NO_ACCESS));
        return;
    }

    if (PARAM_WRITE == CallHandleParam(BaseHOF, DPDrive, wm, HANDLE_EDIT_PARAM)) 
    {
        ModbusMakeOpis(DPDrive, WRITE_UST);
    }
}
// **************************************************************************
//  Вычитка текущего меню пульта и привода
// **************************************************************************
void *GetOpisMenu(void *HOF, void *DM, void *BaseHOF, u16 NumMenu)
{
    HeadOfFileOld_type      *pHOFOld;
    HeadOfFile_type         *pHOF;
    DescriptorMenuOld_type  *pDMOld;
    DescriptorMenu_type     *pDM;
    void 			*Adr;

    if (gIsNvsaOld)
    {
        pHOFOld = HOF;
        pDMOld = DM;
        Adr = (void *)((DescriptorMenuOld_type *)((u32)BaseHOF + (u32)(pHOFOld->MenuStart << 1)) + NumMenu);
        #ifdef BLOCK_STM_MEMORY
        AT45RD(pDMOld, Adr, sizeof(DescriptorMenuOld_type));
        #else            
        memcpy(pDMOld, (void *)((u32)Adr + STM32_FLASH_OFFSET), sizeof(DescriptorMenuOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                          
        #endif        
    }
    else
    {
        pHOF = HOF;
        pDM = DM;
        Adr = (void *)((DescriptorMenu_type *)((u32)BaseHOF + (pHOF->MenuStart << 1)) + NumMenu);
        #ifdef BLOCK_STM_MEMORY
        AT45RD(pDM, Adr, sizeof(DescriptorMenu_type));
        #else            
        memcpy(pDM, (void *)((u32)Adr + STM32_FLASH_OFFSET), sizeof(DescriptorMenu_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                          
        #endif
    }
    return Adr;
}
// **************************************************************************
//  Вычитка текущего параметра
// **************************************************************************
void *GetOpisParam(void *HOF, void *DM, void *DP, void *BaseHOF, u16 NumPrm)
{
    HeadOfFileOld_type      *pHOFOld;
    HeadOfFile_type         *pHOF;
    DescriptorMenuOld_type  *pDMOld;
    DescriptorMenu_type     *pDM;
    DescriptorPrmOld_type   *pDPOld;
    DescriptorPrm_type      *pDP;
    void 			*Adr;
    u16  			OffSNprmAT45;

    if (gIsNvsaOld)
    {
        pHOFOld = HOF;
        pDMOld = DM;
        pDPOld = DP;
        if (pDMOld->QuantityPrmMenu == 0) return 0;
        if (NumPrm > pDMOld->QuantityPrmMenu - 1) return 0;
        Adr = (u16*)((u32)BaseHOF + (u32)(pDMOld->AdrNprm << 1)) + NumPrm;
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&OffSNprmAT45, Adr, sizeof(u16));
        #else            
        memcpy(&OffSNprmAT45, (void *)((u32)Adr + STM32_FLASH_OFFSET), sizeof(u16)); //  *!* Загрузка из памяти STM по сдвигу адреса                                 
        #endif
        
        Adr = (void *)((DescriptorPrmOld_type *)((u32)BaseHOF + (u32)(pHOFOld->PrmStart << 1)) + OffSNprmAT45); 
        #ifdef BLOCK_STM_MEMORY
        AT45RD(pDPOld, Adr, sizeof(DescriptorPrmOld_type));
        #else            
        memcpy(pDPOld, (void *)((u32)Adr + STM32_FLASH_OFFSET), sizeof(DescriptorPrmOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                          
        #endif
    }
    else
    {
        pHOF = HOF;
        pDM = DM;
        pDP = DP;
        if (pDM->QuantityPrmMenu == 0) return 0;
        if (NumPrm > pDM->QuantityPrmMenu - 1) return 0;
        Adr = (u16*)((u32)BaseHOF + (u32)(pDM->AdrNprm << 1)) + NumPrm;
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&OffSNprmAT45, Adr, sizeof(u16));
        #else            
        memcpy(&OffSNprmAT45, (void *)((u32)Adr + STM32_FLASH_OFFSET), sizeof(u16)); //  *!* Загрузка из памяти STM по сдвигу адреса                                 
        #endif

        Adr = (void *)((DescriptorPrm_type *)((u32)BaseHOF + (u32)(pHOF->PrmStart << 1)) + OffSNprmAT45);
        #ifdef BLOCK_STM_MEMORY
        AT45RD(pDP, Adr, sizeof(DescriptorPrm_type));
        #else 
        memcpy(pDP, (void *)((u32)Adr + STM32_FLASH_OFFSET), sizeof(DescriptorPrm_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                                  
        #endif
    }
    return Adr;
}
// **************************************************************************
// Преобразование меню в строк ме
// **************************************************************************
void MenuToGraf(const Menu_type *pntMenu, u8 Pos, u8 LenMenu)
{
    u8   				Ypos, i;
    invers_type         Inv;

    const Menu_type 	*pntM;

	Ypos = 0;

    GrafBffClear();
    pntM = pntMenu;
    i = 0;

    while (LenMenu-- > 0) {
        if (Pos == i){
        	Inv = STR_INV;
        }
        else{
        	Inv = NO_INV;
        }
        StrOutFont(MENUX, Ypos += INCY, Inv, pntM->txt, &fontNormal, leftAlign );
        pntM++;
        i++;
    }
	
    GrfOut();
}
//===================================================================
//===================================================================
void ch_to_smhdmy(time_type *tpnt, u32 chislo)
{
    s32 					i;
    u32 					tmp;
    const u16 				*tbl;
    const time_type 		iniTime = {0,0,0,1,1,1,1,0};

    if (chislo < UnixConst) memcpy(tpnt, &iniTime, 8);
    chislo -= UnixConst;
    tmp = chislo;
    tmp = chislo / 60;
    tpnt->second = (u16)(chislo - tmp * 60);
    chislo = tmp;
    tmp = chislo / 60;
    tpnt->minute = (u16)(chislo - tmp * 60);
    chislo = tmp;
    tmp = chislo / 24;
    tpnt->hour = (u16)(chislo - tmp * 24);
    chislo = tmp;
    tmp = chislo / 7;
    tpnt->day = (u16)(chislo - tmp * 7 + 1);
    tpnt->year = (u16)(chislo / 365);
    if ((u32)(tpnt->year*365+(tpnt->year>>2)) > chislo) tpnt->year--;
    chislo -= tpnt->year * 365 + (tpnt->year >> 2);
    tpnt->year += 1;
    if (tpnt->year & 3) tbl = &monthSum[1];
    else tbl = &monthSumEx[1];
    i = 0;
    chislo &= 0xFFF;          //на всякий случай
    while (chislo >= tbl[i]) i++;
    tpnt->month = i;
    tpnt->date = (u16)(chislo - tbl[i-1] + 1);
}
//====================================================================
// Функция вызова обработчиков отображения и редактирования п-ров
//====================================================================
u32 CallHandleParam(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, u16 TypeHandle)
{
    u32 		i, j, k;
    void(*ViewParam)(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str);	// Создание указателей на функции
    void(*ViewEditParam)(wm_type *wm, char *str);
    u32(*EditParam)(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str);

    i = wm->TypeVar; //wm->StepEdIsm & 0xFF;
    j = wm->StepEdIsm & 0xFF;
    k = PARAM_NONE;

    switch(TypeHandle){
	    case HANDLE_VIEW_EDIT_PARAM:        /// WARNING !! Не используется
	        if ((i < sizeof(FuncViewEditParam) / sizeof(void *)) && FuncViewEditParam[i] != NULL &&
	            j < sizeof(MasEdIzmTek) / sizeof(void *)) {
	            ViewEditParam = FuncViewEditParam[i];
	            ViewEditParam(wm, &StrOut[0]);
	        }
	        else{
	        	k = PARAM_OUT;
	        }
	        break;

	    case HANDLE_VIEW_PARAM:
	        if ((i < sizeof(FuncViewParam) / sizeof(void *)) && FuncViewParam[i] != NULL &&
	            j < sizeof(MasEdIzmTek) / sizeof(void *)) {
	            ViewParam = FuncViewParam[i];
	            ViewParam(BaseHOF, DPDrive, wm, &StrOut[0]);
	        }
	        else{
	        	k = PARAM_OUT;
	        }
	        break;

	    case HANDLE_EDIT_PARAM:
	        if ((i < sizeof(FuncEditParam) / sizeof(void *)) && FuncEditParam[i] != NULL &&
	            j < sizeof(MasEdIzmTek) / sizeof(void *)) {
	            EditParam = FuncEditParam[i];
	            if (EditParam(BaseHOF, DPDrive, wm, &StrOut[0]))
	                k = PARAM_WRITE;
	        }
	        else{
	        	k = PARAM_OUT;
	        }
        break;
    }
    return k;
}
//====================================================================
// Редакторы параметров
//
//
//
//
//====================================================================
#define MAX_LEN_STR 10
//====================================================================
// Определение минимального количества цифр
//====================================================================
u32 MaxDigFloat(f32 FVal)
{
    u32 Val;
    u32 i;
    i = 1;
    Val = abs((u32)FVal);
    while (Val >= 10) {
        Val /= 10;
        i++;
    }
    return i;
}
//===================================================================
//===================================================================
u32 MaxDigInt(u32 ValInt)
{
    u32 Val;
    u32 i;
    i = 1;
    Val = abs((u32)ValInt);
    while (Val >= 10) {
        Val /= 10;
        i++;
    }
    return i;
}
//===================================================================
//===================================================================
u32 MaxDigIntS(s32 ValInt)
{
    u32 Val;
    u32 i;
    i = 1;
    Val = abs(ValInt);
    while (Val >= 10) {
        Val /= 10;
        i++;
    }
    return i;
}
//===================================================================
//===================================================================
u32 MaxDigHex(u32 ValInt)
{
    u32 Val;
    u32 i;
    i = 1;
    Val = ValInt;
    while (Val >= 0xF) {
        //сдвиг на 1 тетраду
        Val = Val >> 4;
        i++;
    }
    return i;
}


//====================================================================
// Редактирование чисел с плавающей точкой
//====================================================================
u32 CalcPosCharF(u32 VesDig, char *str)
{
    u32 Pos;

    Pos = 0;
    Pos = strlen(str) - 1;
    while (VesDig > 0) {
        VesDig--;
        Pos--;
        if (str[Pos] == '.') Pos--;
    }
    return Pos;
}
//===================================================================
//===================================================================
const char *UprStrF[] = {
    "%s%.0f",
    "%s%.1f",
    "%s%.2f",
    "%s%.3f",
    "%s%.4f",
    "%s%.5f",
    "%s%.6f",
    "%s%.7f"
};
//===================================================================
//===================================================================
u32 FloatToStrEdit(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str)
{
    u32                 PosChar;
    s32                 VesDig, MaxLimDig, MinLimDig, MinDig;
    //s32                 LenDig;
    s32                 tmp;
    f32                 ValFPrev;
    keyName_type        Key;
    char                strtmp[20];
    u8                  Mig, Neg;
    const char          *Upr;
    u8                  Dot, EdIzm;
    u8                  CntMig = 0;
    u8                  KeyPressed;
    u8                  err;
    HeadOfFile_type     HOF;
    HeadOfFileOld_type  HOFOld;

    //void(*ViewEditParam)(wm_type *wm, char *str);

    Mig = 1;
    ValFPrev = wm->f.Value;
    Dot = (u8)(wm->StepEdIsm >> 8);
    EdIzm = (u8)wm->StepEdIsm & 0xFF;

//    i = wm->TypeVar; //wm->StepEdIsm & 0xFF;
//    j = wm->StepEdIsm & 0xFF;
    //j =5;
    Upr = UprStrF[Dot];
    MinDig = VesDig = -((s32)Dot);                                    //Начальный вес цифры
//    wm->f.LowLim = -10;
    MinLimDig = MaxDigFloat(wm->f.LowLim);
    MaxLimDig = MaxDigFloat(wm->f.HighLim);
    //strtmp[0] = ' ';
    strtmp[0] = 0;
    
    //LenDig = MinLimDig;
    if (gIsNvsaOld)
    {
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&HOFOld, BaseHOF, sizeof(HeadOfFileOld_type));
        #else            
        memcpy(&HOFOld, (void *)((u32)BaseHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFileOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                                 
        #endif
    }
    else
    {
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&HOF, BaseHOF, sizeof(HeadOfFile_type));
        #else            
        memcpy(&HOF, (void *)((u32)BaseHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                                         
        #endif
    }
    while(1){
        ResetWD(tsk_lcd);
        OSSemPend(SemMenu, SYS_FOREVER, &err );
        Key = WaitKey(&KeyPressed);

        switch (Key) {
        case KEY_UP:
            if (VesDig < 0)
            {
                wm->f.Value = ((wm->f.Value * AntiLg(abs(VesDig))) + AntiLg(0))/AntiLg(abs(VesDig));
            }
            else
            {
                wm->f.Value += AntiLg(VesDig);
            }
            if (wm->f.Value > wm->f.HighLim) wm->f.Value = wm->f.HighLim;
            break;
        case KEY_DOWN:
            if (VesDig < 0)
            {
                wm->f.Value = ((wm->f.Value * AntiLg(abs(VesDig))) - AntiLg(0))/AntiLg(abs(VesDig));
            }
            else
            {
                wm->f.Value -= AntiLg(VesDig);
            }
            if (wm->f.Value < wm->f.LowLim) {
                wm->f.Value = wm->f.LowLim;
                VesDig = MaxDigFloat(wm->f.LowLim) - 1;
            }
            break;

        case KEY_LEFT:
            if (wm->f.Value < 0) {
                if (VesDig < MinLimDig - 1) {
                    VesDig++;
                }
            }
            else {
                if (VesDig < MaxLimDig - 1) {
                    VesDig++;
                }
            }
            break;
        case KEY_RIGHT:
            if (VesDig > MinDig) {
                VesDig --;
                //PosStr--;
            }
            break;

        case KEY_ENTER:
            return 1;
        case KEY_ESC:
            wm->f.Value = ValFPrev;
            return 1;
        }

        if (wm->f.Value < 0) Neg = 1;
        else Neg = 0;
        tmp = VesDig - MaxDigFloat(wm->f.Value) + 1;
        if (tmp > 0) {
            if (Neg)
                memset(&strtmp[1], '0', tmp);
            else
                memset(&strtmp[0], '0', tmp);
            strtmp[tmp] = 0;
        }
        else memset(&strtmp[0], 0, sizeof(strtmp));

        if (Neg) strtmp[0] = '-';
        sprintf(str, Upr, &strtmp[0], fabs(wm->f.Value));
        PosChar = CalcPosCharF(VesDig + Dot, str);

        if (CntMig > TimeMig) {
            CntMig = 0;
            Mig ^= 1;
        }
        else CntMig++;

        if (Mig == 0 && KeyPressed == 0){
            str[PosChar] = ' ';             // Прочистить символ
        }

        GrafBffClear();
        //==========================================================
        //Значение параметра
        //==========================================================
        if ((strlen(str) + strlen(&MasEdIzmTek[EdIzm][0])) < 12)
        {
            sprintf(&strtmp[0], "%6s %s", str, &MasEdIzmTek[EdIzm][0]);
            drawParamEdit(&strtmp[0]);
        }
        else
        {
            //Вывод размерности параметра c новой строки
            sprintf(&strtmp[0], "%6s", str);
            drawParamEdit(&strtmp[0]);
            sprintf(&strtmp[0], "%s", &MasEdIzmTek[EdIzm][0]);
            StrOutFont(160, 80, NO_INV, &strtmp[0], &fontMiddle, rightAlign);           
        }
        //==========================================================
        //Название параметра
        //==========================================================
        if (gIsNvsaOld)
        {
            getNamePrm(BaseHOF, &HOFOld, &DPDrive->DPOld, str);
        }
        else
        {
            getNamePrm(BaseHOF, &HOF, &DPDrive->DP, str);
        }
        StrOutFont(0, 0, STR_INV, str, &fontNormal, leftAlign );
        //==========================================================
        //Единицы
        //==========================================================
        sprintf(str, "%-7s %s", str, &MasEdIzmTek[EdIzm][0]);
        //==========================================================
        //Единицы - нет
        //==========================================================
        if(gIsNvsaOld){
            sprintf(str, "[%d,%d]", DPDrive->DPOld.IDGrp, DPDrive->DPOld.IDPrm);
        }
        else{
              sprintf(str, "[%d,%d]", DPDrive->DP.IDGrp, DPDrive->DP.IDPrm);
        }
      
        StrOutFont(0, 144, NO_INV, &StrOut[0], &fontNormal, leftAlign );
        //==========================================================
        //Min и Max
        //==========================================================
        sprintf(&strtmp[0], "%s %.*f", GetStringTitle(MessageTitul_MIN), Dot, wm->f.LowLim);
        drawParamEditLowLim(&strtmp[0]);
        sprintf(&strtmp[0], "%s%.*f", GetStringTitle(MessageTitul_MAX), Dot, wm->f.HighLim);
        drawParamEditHighLim(&strtmp[0]);
        //==========================================================
        //Вывод на экран
        //==========================================================
        GrfOut();
        //==========================================================
        //Вывод индикации статуса
        //==========================================================
        WordStatusForLed();
    }// While(1)
}
//====================================================================
// Редактирование целых чисел
//====================================================================
u32 CalcPosCharInt(u32 VesDig, char *str)
{
    u32 Pos;
    Pos = strlen(str) - 1;
    while (VesDig > 0) {
        VesDig--;
        Pos--;
        if (str[Pos] == '.' || str[Pos] == ':' || str[Pos] == '-') Pos--;
    }
    return Pos;
}
//===================================================================
//===================================================================
u16 absU16(s16 Val)
{
    return (Val < 0) ? -Val : Val;
}
//===================================================================
//===================================================================
u32 UIntToStrEdit(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str)
{
    u32                 PosChar, VesDig;
    u32                 MaxLimDig, MinLimDig;
    u32                 MaxDig;
    s32                 tmp;
    s32                 ValPrev;
    keyName_type        Key;
    char                strtmp[20];
    u8                  Mig, Neg;
    const char          *Upr;
    char                *pStr;
    u8                  Dot, EdIzm;
    u8                  CntMig = 0;
    u8                  KeyPressed;
    u8                  err;
    HeadOfFile_type     HOF;
    HeadOfFileOld_type  HOFOld;

    Mig = 1;
    ValPrev = wm->s32.Value;
    Dot = (u8)(wm->StepEdIsm >> 8);
    EdIzm = (u8)wm->StepEdIsm & 0xFF;

    Upr = UprStrF[Dot];
    //wm->s32.LowLim = -1000;
    VesDig = 0;
    if (wm->TypeVar == int16Frmt){
        wm->s32.LowLim &= 0xFFFF;
        wm->s32.HighLim &= 0xFFFF;
        MinLimDig = MaxDigInt(absU16(wm->s32.LowLim));
        MaxLimDig = MaxDigInt(absU16(wm->s32.HighLim));
    }
    else if (wm->TypeVar == int32Frmt){
        MinLimDig = MaxDigInt(abs(wm->s32.LowLim));
        MaxLimDig = MaxDigInt(abs(wm->s32.HighLim));
    }
    else if (wm->TypeVar == uns16Frmt){   
        wm->s32.LowLim &= 0xFFFF;
        wm->s32.HighLim &= 0xFFFF;
        MinLimDig = MaxDigInt(wm->s32.LowLim);
        MaxLimDig = MaxDigInt(wm->s32.HighLim);
    }
    else{
        MinLimDig = MaxDigInt(wm->s32.LowLim);
        MaxLimDig = MaxDigInt(wm->s32.HighLim);
    }
    strtmp[0] = 0;
    sprintf(str, Upr, &strtmp[0], abs(wm->s32.Value));
    if (gIsNvsaOld)
    {
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&HOFOld, BaseHOF, sizeof(HeadOfFileOld_type));
        #else            
        memcpy(&HOFOld, (void *)((u32)BaseHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFileOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                                 
        #endif
    }
    else
    {
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&HOF, BaseHOF, sizeof(HeadOfFile_type));
        #else            
        memcpy(&HOF, (void *)((u32)BaseHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                                 
        #endif
    }
    while(1){
      ResetWD(tsk_lcd);
        OSSemPend(SemMenu, SYS_FOREVER, &err );
        Key = WaitKey(&KeyPressed);

        switch (Key) {
            case KEY_UP:
                wm->s32.Value += (u32)AntiLg(VesDig);
                //if (wm->s32.Value > wm->s32.HighLim) wm->s32.Value = wm->s32.HighLim;
                if (wm->TypeVar == int16Frmt){
                    if (((s16)wm->s32.Value > (s16)wm->s32.HighLim) || ((s16)wm->s32.Value < (s16)(wm->s32.Value - (u32)AntiLg(VesDig)))) {
                        wm->s32.Value = (s16)wm->s32.HighLim;
                    }
                }
                else{
                    if (wm->s32.Value > wm->s32.HighLim)
                        wm->s32.Value = wm->s32.HighLim;
                }
                break;
            case KEY_DOWN:
                wm->s32.Value -= (u32)AntiLg(VesDig);
                //Vall = wm->s32.Value;
                //MinL = wm->s32.LowLim;
                if (wm->TypeVar == int16Frmt){
                     if (((s16)wm->s32.Value < (s16)wm->s32.LowLim) || ((s16)wm->s32.Value > (s16)(wm->s32.Value + (u32)AntiLg(VesDig)))) {
                        wm->s32.Value = (s16)wm->s32.LowLim;
                    }
                }
                else{
                    if (wm->s32.Value < wm->s32.LowLim) {
                        wm->s32.Value = wm->s32.LowLim;
                    }
                }
                if (wm->TypeVar == int16Frmt) {
                    if (VesDig > (u32)MaxDigInt(absU16(wm->s32.Value))) {
                        if (wm->s32.Value < 0) {
                            VesDig = MaxDigInt(absU16(wm->s32.LowLim)) - 1;
                        }
                    }
                }
                else if (wm->TypeVar == int32Frmt) {
                    if (VesDig > (u32)MaxDigInt(abs(wm->s32.Value))) {
                        if (wm->s32.Value < 0) {
                            VesDig = MaxDigInt(abs(wm->s32.LowLim)) - 1;
                        }
                    }
                }
                else {
                    if (VesDig > MaxDigInt((u32)wm->s32.Value)) 
                    {
                        //VesDig = MaxDigInt((u32)wm->s32.LowLim) - 1;
                    }
                }
                break;
            case KEY_LEFT:
                if (wm->TypeVar == int16Frmt || wm->TypeVar == int32Frmt) {
                    if (wm->s32.Value < 0) {
                        if (VesDig < MinLimDig - 1) {
                            VesDig++;
                        }
                    }
                    else {
                        if (VesDig < MaxLimDig - 1) {
                            VesDig++;
                        }
                    }
                }
                else {
                    if (VesDig < MaxLimDig - 1) {
                        VesDig++;
                    }
                }
                break;
            case KEY_RIGHT:
                if (VesDig > 0) {
                    VesDig --;
                }
                break;
            case KEY_ENTER:
                return 1;
            case KEY_ESC:
                wm->s32.Value = ValPrev;
                return 1;
        }
        memset(&strtmp[0], 0, sizeof(strtmp));
        pStr = &strtmp[0];

        if (wm->TypeVar == int16Frmt)
            if ((s16)wm->s32.Value < 0) Neg = 1;
            else Neg = 0;
        else if (wm->TypeVar == int32Frmt)
            if (wm->s32.Value < 0) Neg = 1;
            else Neg = 0;
        else Neg = 0;

        if (Neg) {
            *pStr = '-';
            pStr++;
        }
        if (VesDig > Dot) {//Вес превысил точку
            if (wm->TypeVar == int16Frmt){
                MaxDig = MaxDigInt((s16)wm->s32.Value);
            }
            else{
                MaxDig = MaxDigInt(wm->s32.Value);
            }
            if (MaxDig > Dot)
                tmp = VesDig - MaxDig + 1;
            else
                tmp = VesDig - Dot;

            if (tmp > 0) {
                memset(pStr, '0', tmp);
                pStr += tmp;
            }
        }
        if (wm->TypeVar == int16Frmt)
            sprintf(str, Upr, &strtmp[0], (f32)abs((s16)wm->s32.Value)/AntiLg(Dot));
        else
            sprintf(str, Upr, &strtmp[0], (f32)abs(wm->s32.Value)/AntiLg(Dot));
        PosChar = CalcPosCharInt(VesDig, str);
        if (CntMig > TimeMig) {
            CntMig = 0;
            Mig ^= 1;
        }
        else CntMig++;
        if (Mig == 0 && KeyPressed == 0){
            str[PosChar] = ' ';
        }


        GrafBffClear();
        //memset(str, 0, 17);
        //==========================================================
        //Значение параметра
        //==========================================================
        sprintf(&strtmp[0], "%6s %s", str, &MasEdIzmTek[EdIzm][0]);
        drawParamEdit(&strtmp[0]);
        if (gIsNvsaOld)
        {
            //==========================================================
            //Название параметра
            //==========================================================
            getNamePrm(BaseHOF, &HOFOld, &DPDrive->DPOld, str);
            StrOutFont(0, 0, STR_INV, str, &fontNormal, leftAlign );
            //==========================================================
            //Единицы - нет
            //==========================================================
            sprintf(str, "[%d,%d]", DPDrive->DPOld.IDGrp, DPDrive->DPOld.IDPrm);
            StrOutFont(0, 144, INV, str, &fontNormal, leftAlign );
        }
        else
        {
            //==========================================================
            //Название параметра
            //==========================================================
            getNamePrm(BaseHOF, &HOF, &DPDrive->DP, str);
            StrOutFont(0, 0, STR_INV, str, &fontNormal, leftAlign );
            //==========================================================
            //Единицы - нет
            //==========================================================
            sprintf(str, "[%d,%d]", DPDrive->DP.IDGrp, DPDrive->DP.IDPrm);
            StrOutFont(0, 144, INV, str, &fontNormal, leftAlign );
        }
        //==========================================================
        //Min и Max
        //==========================================================
        if (wm->TypeVar == int16Frmt) {
            sprintf(&strtmp[0], "%s  %.*f", GetStringTitle(MessageTitul_MIN), Dot,(f32)(s16)wm->s32.LowLim/AntiLg(Dot));
        }
        else{
            sprintf(&strtmp[0], "%s  %.*f", GetStringTitle(MessageTitul_MIN), Dot,(f32)wm->s32.LowLim/AntiLg(Dot));
        }

        drawParamEditLowLim(&strtmp[0]);

        if(wm->TypeVar == int16Frmt){
            sprintf(&strtmp[0], "%s %.*f", GetStringTitle(MessageTitul_MAX), Dot,(f32)(s16)wm->s32.HighLim/AntiLg(Dot));
        }
        else{
            sprintf(&strtmp[0], "%s %.*f", GetStringTitle(MessageTitul_MAX), Dot,(f32)wm->s32.HighLim/AntiLg(Dot));
        }

        drawParamEditHighLim(&strtmp[0]);
        //==========================================================
        //Вывод на экран
        //==========================================================
        GrfOut();
        //==========================================================
        //Вывод индикации статуса
        //==========================================================
        WordStatusForLed();
    }// While(1)
}


//===================================================================
//Редактирование 16-ричных параметров
//===================================================================
u32 HexToStrEdit(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str)
{
    u32                 PosChar; 
    u32                 NumDig;                 //номер выбранного разряда числа
    u32                 VesDig;                 //вес выбранного разряда числа
    u32                 MaxLimDig;              //макс. к-во разрядов числа
    s32                 ValPrev;                //значение параметра перед редактированием
    keyName_type        Key;                    //нажатая клавиша
    char                strtmp[20];             //строка-временный буфер
    u8                  Mig;                    //флаг мигания курсора
    const char          *Upr;                   //строка форматирования параметра 
    u8                  EdIzm;                  //индекс единиц измерения
    u8                  CntMig = 0;             //таймер мигания курсора
    u8                  KeyPressed;
    u8                  err;
    HeadOfFile_type     HOF;
    HeadOfFileOld_type  HOFOld;

    Mig = 1;
    ValPrev = wm->s32.Value;
    EdIzm = (u8)wm->StepEdIsm & 0xFF;           

    Upr = "0X%04X";
    NumDig = 0;

    MaxLimDig = MaxDigHex(wm->s32.HighLim);
           
    memset(&strtmp[0], 0, sizeof(strtmp));
    
    if (gIsNvsaOld)
    {
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&HOFOld, BaseHOF, sizeof(HeadOfFileOld_type));
        #else            
        memcpy(&HOFOld, (void *)((u32)BaseHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFileOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                                 
        #endif
    }
    else
    {
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&HOF, BaseHOF, sizeof(HeadOfFile_type));
        #else            
        memcpy(&HOF, (void *)((u32)BaseHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                                 
        #endif
    }
    
    //рабочий цикл редактора
    while(1){
        ResetWD(tsk_lcd);         //сброс watchdog-таймера
        OSSemPend(SemMenu, SYS_FOREVER, &err );
        
        //обработка нажатий клавиш пульта
        Key = WaitKey(&KeyPressed);        
        switch (Key) {
            case KEY_UP:
                if(wm->s32.Value + VesDig <= wm->s32.HighLim) wm->s32.Value += VesDig;
                break;
                
            case KEY_DOWN:
                if(wm->s32.Value - (s32)VesDig >= wm->s32.LowLim) wm->s32.Value -= VesDig;
                break;
                
            case KEY_LEFT:
                if (NumDig < MaxLimDig - 1) {
                    NumDig++;
                }
                break;
                
            case KEY_RIGHT:
                if (NumDig > 0) {
                    NumDig --;
                }
                break;
                
            case KEY_ENTER:
                return 1;
                
            case KEY_ESC:
                wm->s32.Value = ValPrev;
                return 1;
        }
        
        VesDig = (u32)(1 << NumDig * 4);
        
        //форматированный вывод параметра в строку
        sprintf(str, Upr, wm->s32.Value);
        
        //мигание
        PosChar = CalcPosCharInt(NumDig, str);
        if (CntMig > TimeMig) {
            CntMig = 0;
            Mig ^= 1;
        }
        else CntMig++;
        if (Mig == 0 && KeyPressed == 0){
            str[PosChar] = ' ';
        }

        GrafBffClear();
        //==========================================================
        //Значение параметра
        //==========================================================
        sprintf(&strtmp[0], "%6s %s", str, &MasEdIzmTek[EdIzm][0]);
        drawParamEdit(&strtmp[0]);
        if (gIsNvsaOld)
        {
            //==========================================================
            //Название параметра
            //==========================================================
            getNamePrm(BaseHOF, &HOFOld, &DPDrive->DPOld, str);
            StrOutFont(0, 0, STR_INV, str, &fontNormal, leftAlign );
            //==========================================================
            //Единицы - нет
            //==========================================================
            sprintf(str, "[%d,%d]", DPDrive->DPOld.IDGrp, DPDrive->DPOld.IDPrm);
            StrOutFont(0, 144, INV, str, &fontNormal, leftAlign );
        }
        else
        {
            //==========================================================
            //Название параметра
            //==========================================================
            getNamePrm(BaseHOF, &HOF, &DPDrive->DP, str);
            StrOutFont(0, 0, STR_INV, str, &fontNormal, leftAlign );
            //==========================================================
            //Единицы - нет
            //==========================================================
            sprintf(str, "[%d,%d]", DPDrive->DP.IDGrp, DPDrive->DP.IDPrm);
            StrOutFont(0, 144, INV, str, &fontNormal, leftAlign );
        }
        //==========================================================
        //Min и Max
        //==========================================================
        sprintf(&strtmp[0], "%s 0X%04X", GetStringTitle(MessageTitul_MIN), wm->s32.LowLim);
        drawParamEditLowLim(&strtmp[0]);

        sprintf(&strtmp[0], "%s 0X%04X", GetStringTitle(MessageTitul_MAX), wm->s32.HighLim);
        drawParamEditHighLim(&strtmp[0]);
        //==========================================================
        //Вывод на экран
        //==========================================================
        GrfOut();
        //==========================================================
        //Вывод индикации статуса
        //==========================================================
        WordStatusForLed();
    }// While(1)
}


//====================================================================
// Редактирование текстовых параметров
//====================================================================
u32 TextToStrEdit(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str)
{
    u32                 ValPrev;
    keyName_type        Key;
    u8                  CntMig = 0;
    u8                  Mig = 0;
    u8                  KeyPressed;
    char                StrTmp[NUM_SHOW_TXT_PRM][17];
    char                strtmp[17];
    u8                  err;
    u16                 Cursor;
    u8                  i;
    u32                 Ypos;
    invers_type         inv;
    HeadOfFile_type     HOF;
    HeadOfFileOld_type     HOFOld;


    //StrTmp[16] = 0;
    memset(&str[0], 0, 20);
    
    if (wm->s32.Value > wm->s32.HighLim)
    {
        wm->s32.Value = wm->s32.HighLim;
        ValPrev = wm->s32.LowLim;
    }
    else if (wm->s32.Value < wm->s32.LowLim)
    {
        wm->s32.Value = wm->s32.LowLim;
        ValPrev = wm->s32.LowLim;
    }
    else
    {
        ValPrev = wm->s32.Value;
    }
    if (gIsNvsaOld)
    {
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&HOFOld, BaseHOF, sizeof(HeadOfFileOld_type));
        #else 
        memcpy(&HOFOld, (void *)((u32)BaseHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFileOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                                 
        #endif
    }
    else
    {
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&HOF, BaseHOF, sizeof(HeadOfFile_type));
        #else            
        memcpy(&HOF, (void *)((u32)BaseHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                                 
        #endif
    }
    if (wm->s32.Value < (wm->s32.LowLim + NUM_SHOW_TXT_PRM))
    {
        for (i = wm->s32.LowLim; i < (NUM_SHOW_TXT_PRM + wm->s32.LowLim); i++)
        {
            if (i > wm->s32.HighLim)
            {
                memset(&StrTmp[i - wm->s32.LowLim][0], 0, 17);
            }
            else
            {
                ReadValueTextPrm(BaseHOF, &HOF, DPDrive, &StrTmp[i - wm->s32.LowLim][0], i);
            }
        }
        Cursor = wm->s32.Value - wm->s32.LowLim;
    }
    else
    {
        for (i = wm->s32.Value - (NUM_SHOW_TXT_PRM - 1); i < (wm->s32.Value + 1); i++)
        {
            ReadValueTextPrm(BaseHOF, &HOF, DPDrive, &StrTmp[i -(wm->s32.Value - (NUM_SHOW_TXT_PRM - 1))][0], i);
        }
        Cursor = NUM_SHOW_TXT_PRM - 1;
    }
    inv = STR_INV;
    while(1){
        ResetWD(tsk_lcd);
        OSSemPend(SemMenu, SYS_FOREVER, &err );
        Key = WaitKey(&KeyPressed);
        if (wm->s32.Value > wm->s32.HighLim)
        {
            wm->s32.Value = wm->s32.HighLim;
        }

        switch (Key) {
        case KEY_UP:
            if (wm->s32.Value > wm->s32.LowLim) 
            {   
                wm->s32.Value--; 
                if (Cursor > 0 )//< NUM_SHOW_TXT_PRM - 1)
                {
                    Cursor--;
                }
                else
                {
                    for (i = wm->s32.Value; i < (NUM_SHOW_TXT_PRM + wm->s32.Value); i++)
                    {
                        ReadValueTextPrm(BaseHOF, &HOF, DPDrive, &StrTmp[i - wm->s32.Value][0], i);
                    }
                }
            }
            else    { wm->s32.Value = wm->s32.LowLim; }
            CntMig = 0;
            Mig = 1;
            inv = STR_INV;
            break;
        case KEY_DOWN:
            wm->s32.Value++;
            if (wm->s32.Value > wm->s32.HighLim) wm->s32.Value = wm->s32.HighLim;
            else
            {
                if (Cursor < NUM_SHOW_TXT_PRM - 1)
                {
                    Cursor++;
                }
                else
                {
                    for (i = (wm->s32.Value - (NUM_SHOW_TXT_PRM - 1)); i <  (wm->s32.Value + 1); i++)
                    {
                            ReadValueTextPrm(BaseHOF, &HOF, DPDrive, &StrTmp[i - (wm->s32.Value - (NUM_SHOW_TXT_PRM - 1))][0], i);
                    }
                }
            }
            CntMig = 0;
            Mig = 1;
            inv = STR_INV;
            break;

        case KEY_ENTER:           
          return 1;

        case KEY_ESC:
            wm->s32.Value = ValPrev;
            return 1;

        default:
            break;
        }

        GrafBffClear();

        if (CntMig > TimeMig) 
        {
            CntMig = 0;
            Mig ^= 1;
        }
        else
        { 
            CntMig++; 
        }
//==========================================================
//Значение параметра
//==========================================================
        for (i = 0, Ypos = 3; i < NUM_SHOW_TXT_PRM; i++) 
        {
            if (i == Cursor)
            {
                sprintf(&strtmp[0], "%s", &StrTmp[i][0]);
                StrOutFont(0, Ypos += INCY, inv, &strtmp[0], &fontNormal, leftAlign );
            }
            else
            {
                sprintf(&strtmp[0], "%s", &StrTmp[i][0]);
                StrOutFont(0, Ypos += INCY, NO_INV, &strtmp[0], &fontNormal, leftAlign );
            }
        }
//==========================================================
//Название параметра
//==========================================================
        if (gIsNvsaOld)
        {
            getNamePrm(BaseHOF, &HOFOld, &DPDrive->DPOld, str);
        }
        else
        {
            getNamePrm(BaseHOF, &HOF, &DPDrive->DP, str);
        }
        StrOutFont(0, 0, STR_INV, str, &fontNormal, leftAlign );
//==========================================================
//Выдиление области значений
//==========================================================
        DrawLine(0, 17, 148, 17, 1, 3, 3);
        DrawLine(0, 132, 148, 132, 1, 3, 3);
//==========================================================
//Указатели наличия параметров за приделани видемой области
//==========================================================
        if ((wm->s32.Value - Cursor) != wm->s32.LowLim)
        {
            DrawIcon16x16(75, 135, upSmall);
        }
        if ((wm->s32.HighLim - wm->s32.Value + Cursor) > NUM_SHOW_TXT_PRM - 1)
        {
            DrawIcon16x16(65, 135, downSmall);
        }
//==========================================================
//Номер группы и номер параметра
//==========================================================
        sprintf(str, "[%d,%d]", DPDrive->DP.IDGrp, DPDrive->DP.IDPrm);
        StrOutFont(0, 144, INV, str, &fontNormal, leftAlign );
//==========================================================
//Вывод на экран
//==========================================================
        GrfOut();
//==========================================================
//Вывод индикации статуса
//==========================================================
        WordStatusForLed();
    }
}

//====================================================================
// Редактирование Часов Минут или Минут Секунд или Месяц день
//====================================================================
/**************************************/
/*День месяц 366 -> 31 12             */
/**************************************/
s16 DayMonthToFormat(s16 DayMonth)
{
    u16 Month, Day;

    for (Month = 1; Month <= 13; Month++){
        if (monthSumEx[Month] >= DayMonth){
            Month--;
            break;
        }
    }
    Day = DayMonth - monthSumEx[Month];
    return Day * 100 + Month;     // Partial Date Value
}
/**************************************/
/*Формат 31 12 -> 366                 */
/**************************************/
s16 FormatToDayMonth(s16 Format)
{
    u16     Day;
    u16     Month;

    Day = Format / 100;
    Month = Format % 100;

    if (Month > 12) Month = 12;
    if (Month < 1)  Month = 1;
    if (Day > DayInMonth[Month]) Day = DayInMonth[Month];
    if (Day < 1) Day = 1;

    return monthSumEx[Month] + Day;
}
/**************************************/
/*Часы минуты 1439 -> 23 59           */
/**************************************/
s16 HourMinToFormat(s16 HourMin)
{
    u16     Hour, Min;

    Hour = HourMin / 60;
    Min = HourMin % 60;
    return Hour * 100 + Min;
}
/**************************************/
/*Формат 23 59 -> 1439                */
/**************************************/
s16 FormatToHourMin(s16 Format)
{
    s16     Hour;
    s16     Min;

    Hour = Format / 100;
    Min = Format % 100;

    if (Hour < 0) 
    {
        Hour = 0;
        Min = 0;
    }
    if(Min > 69) 
    {
        Min = 60 - (100 - Min);
    }
    else if (Min > 59)
    {
        Min = 0;
        Hour++;
        if (Hour > 23) 
        {
            Hour = 23;
            Min = 59;
        }
    }
    if (Hour > 23) 
    {
        Hour = 23;
        Min = 59;
    }
    if (Min < 0){
        Min = 59;
	    Hour--;
        if (Hour < 0) 
        {
            Hour = 0;
            Min = 0;
        }
    }
    return Hour * 60 + Min;
}
/**************************************/
/*Минуты секнды 3481 -> 59 59         */
/**************************************/
s16 MinSecToFormat(s16 MinSec)
{
    u16     Min, Sec;

    Min = MinSec / 60;
    Sec = MinSec % 60;

    return Min * 100 + Sec;
}
/**************************************/
/*Формат 59 59 -> 3481                */
/**************************************/
s16 FormatToMinSec(s16 Format)
{
    s16 Min;
    s16 Sec;

    Min = Format / 100;
    Sec = Format % 100;

    if (Sec > 59) Sec = 59;
    if (Sec < 0) Sec = 0;
    if (Min > 59) Min = 59;
    if (Min < 0) Min = 0;

    return Min * 60 + Sec;
}
/**************************************/
/*Ссылки 9999 -> 99 99         */
/**************************************/
s16 RefToFormat(s16 Ref)
{
    return Ref;
}
/**************************************/
/*Формат 99 99 -> 9999                */
/**************************************/
s16 FormatToRef(s16 Format)
{
    s16     Grp;
    s16     Prm;

    Grp = Format / 100;
    Prm = Format % 100;

    if (Prm > 99) Prm = 99;
    if (Prm < 0) Prm = 0;
    if (Grp > 99) Grp = 99;
    if (Grp < 0) Grp = 0;

    return Grp * 100 + Prm;
}
//===================================================================
//===================================================================
inline u16 GeteHigh(u16 format){
    return format / 100;
}
//===================================================================
//===================================================================
inline u16 GeteLow(u16 format){
    return format - ((format / 100) * 100);
}
//===================================================================
//===================================================================
u32 HMorMSorMDToStrEdit(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str)
{
    char                *uprStr;
    u32                 valPrev;
    u32                 vesDig, maxLimDig;
    u16                 format, high, low, formatLim;
    u16                 posCursor;
    keyName_type        key;
    u8                  cntMig, mig;
    u8                  KeyPressed;
    char                strtmp[16];
    u8                  err;
    HeadOfFile_type     HOF;
    HeadOfFileOld_type  HOFOld;
    s16(*formatToDigit)(s16 format);
    s16(*digitToFormat)(s16 digit);

    if (wm->TypeVar == HourMinFrmt) {
        formatToDigit = &FormatToHourMin;
        digitToFormat = &HourMinToFormat;
        uprStr = "%.2d:%.2d";
    }
    else if (wm->TypeVar == MinSecFrmt) {
        formatToDigit = &FormatToMinSec;
        digitToFormat = &MinSecToFormat;
        uprStr = "%.2d:%.2d";
    }
    else if (wm->TypeVar == MonDayFrmt) {
        formatToDigit = &FormatToDayMonth;
        digitToFormat = &DayMonthToFormat;
        uprStr = "%.2d-%.2d";
    }
    else if(wm->TypeVar == refFormat){
        formatToDigit = &FormatToRef;
        digitToFormat = &RefToFormat;
        uprStr = "%.2d:%.2d";
    }
    valPrev = wm->s32.Value;

    mig = 0;
    vesDig = 0;
    maxLimDig = 3;
    format = digitToFormat((s16)wm->s32.Value);

    cntMig = 0;
    if (gIsNvsaOld)
    {
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&HOFOld, BaseHOF, sizeof(HeadOfFileOld_type));
        #else            
        memcpy(&HOFOld, (void *)((u32)BaseHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFileOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                                 
        #endif
    }
    else
    {
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&HOF, BaseHOF, sizeof(HeadOfFile_type));
        #else            
        memcpy(&HOF, (void *)((u32)BaseHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                                 
        #endif
    }
    while(1){
        ResetWD(tsk_lcd);  //Зарядим WatchDog чтоб не перегрузился пульт
        OSSemPend(SemMenu, SYS_FOREVER, &err );
        key = WaitKey(&KeyPressed);

        switch (key) {
            case KEY_UP:
                format += (u32)AntiLg(vesDig);
                wm->s32.Value = formatToDigit(format);
                if (wm->s32.Value > wm->s32.HighLim) {
                    wm->s32.Value = wm->s32.HighLim;
                }
                break;
            case KEY_DOWN:
                format -= (u32)AntiLg(vesDig);
                wm->s32.Value = formatToDigit(format);
                if (wm->s32.Value < wm->s32.LowLim) {
                    wm->s32.Value = wm->s32.LowLim;
                }
                break;
            case KEY_LEFT:
                if (vesDig < maxLimDig) {
                    vesDig++;
                }
                break;
            case KEY_RIGHT:
                if (vesDig > 0) {
                    vesDig--;
                }
                break;
            case KEY_ENTER:
                wm->s32.Value = formatToDigit(format);
                return 1;
            case KEY_ESC:
                wm->s32.Value = valPrev;
                return 1;
        }
        GrafBffClear();
        format = digitToFormat((s16)wm->s32.Value);
        high = GeteHigh(format);
        low = GeteLow(format);
        sprintf(str, uprStr, high, low);

        posCursor = CalcPosCharInt(vesDig, str);

        if (cntMig > TimeMig) {
            cntMig = 0;
            mig ^= 1;
        }
        else cntMig++;

        if (mig == 0 && KeyPressed == 0) {
            str[posCursor] = ' ';
        }
        //==========================================================
        //Значение параметра
        //==========================================================
        drawParamEdit(str);
        //==========================================================
        //Название параметра
        //==========================================================
        if (gIsNvsaOld)
        {
            getNamePrm(BaseHOF, &HOFOld, &DPDrive->DPOld, str);
        }
        else
        {
            getNamePrm(BaseHOF, &HOF, &DPDrive->DP, str);
        }
        StrOutFont(0, 0, STR_INV, str, &fontNormal, leftAlign );
        //==========================================================
        //Min и Max
        //==========================================================
        formatLim = digitToFormat((s16)wm->s32.HighLim);
        high = GeteHigh(formatLim);
        low = GeteLow(formatLim);
        sprintf(str, "%s ", GetStringTitle(MessageTitul_MAX));
        sprintf(&strtmp[0], uprStr, high, low);
        strcat(str, &strtmp[0]);
        drawParamEditHighLim(str);

        formatLim = digitToFormat((s16)wm->s32.LowLim);
        high = GeteHigh(formatLim);
        low = GeteLow(formatLim);
        sprintf(str, "%s ", GetStringTitle(MessageTitul_MIN));
        sprintf(&strtmp[0], uprStr, high, low);
        strcat(str, &strtmp[0]);
        drawParamEditLowLim(str);
        //==========================================================
        //Номер группы и номер параметра
        //==========================================================
        if(gIsNvsaOld){
            sprintf(str, "[%d,%d]", DPDrive->DPOld.IDGrp, DPDrive->DPOld.IDPrm);
        }
        else{
            sprintf(str, "[%d,%d]", DPDrive->DP.IDGrp, DPDrive->DP.IDPrm);
        }
        
        StrOutFont(0, 144, INV, str, &fontNormal, leftAlign );

        GrfOut();
        //==========================================================
        //Вывод индикации статуса
        //==========================================================
        WordStatusForLed();
    }
}
//====================================================================
// Просмотрщики редактируемых параметров
//
//
//====================================================================
//====================================================================
// Просмотр редактируемых чисел с плавающей точкой
//====================================================================
void FloatViewEdit(wm_type *wm, char *str)
{

    //sprintf(&Str[0], "%7.f", wm->f.Value);
    //sprintf(&Str[0], "%7.f", wm->f.Value);
}
//------------------------------------------------------------------------------
//Прорисовка строки со значением параметра
//------------------------------------------------------------------------------
void drawParamEdit(char *str)
{
     StrOutFont(0, 60, NO_INV, str, &fontMiddle, leftAlign );
}
//------------------------------------------------------------------------------
//Прорисовка строки со нижним пределом параметра
//------------------------------------------------------------------------------
void drawParamEditLowLim(char *str)
{
    StrOutFont(0, 100, NO_INV, str, &fontNormal, leftAlign );
}
//------------------------------------------------------------------------------
//Прорисовка строки со верхним пределом параметра
//------------------------------------------------------------------------------
void drawParamEditHighLim(char *str)
{
     StrOutFont(0, 120, NO_INV, str, &fontNormal, leftAlign );
}
/*****************************************************
Выполнение заданной функции, номерок ф-ии хранится при
редактировании
******************************************************/
void ExecFunc(wm_type *wm)
{
    void (*ExecFuncParam1)(void);
    u16         NumFunc;

    NumFunc = (wm->Flags >> 24) & 0xff;
    if (NumFunc < sizeof(ExecFuncParam) / sizeof(ExecFuncParam[0])){
        ExecFuncParam1 = ExecFuncParam[NumFunc];
        if (ExecFuncParam1 != NULL) {
            MakeOpis(wm, WRITE_UST, TYPE_PULT);
            ExecFuncParam1();
        }
    }
}
// **************************************************************************
//  очистка графического буфера
// **************************************************************************
void GrafBffClear(void)
{
    memset( GrafBff, 0, NBYTES * XPIXELS );
}
// **************************************************************************



// **************************************************************************
// **************************************************************************
// **************************************************************************
// **************************************************************************
                #include "MenuPrmTostr.c"
// **************************************************************************
// **************************************************************************
// **************************************************************************
// **************************************************************************
