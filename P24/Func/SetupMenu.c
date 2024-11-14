/************************** (C) Copyright 2013 Triol ***************************
* File Name          : SetupMenu.c
* Author             : Kats Eyvhen
* TS Idea            : 2013.04.08
* TS Version         : 2013.04.08
* Description        : Function for setup mode menu
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "globalIncludes.h"
#include <stdlib.h>
#include "MenuStatDrv.h"
#include "Menu.h"

/* Private define ------------------------------------------------------------*/
HandlerMenuSetup_struct HandlerMenuSetup;
extern HandlerMenuPultNew_struct_type   HandlerMenuPultNew_struct;
extern inline void SetModePrevMenu(void);
extern const char noValidData[];
extern uint8_t gIsNvsaOld;

/* Private macro -------------------------------------------------------------*/



/* Private types -------------------------------------------------------------*/



/* Private variables ---------------------------------------------------------*/



/* Private function prototypes -----------------------------------------------*/



/* Private functions ---------------------------------------------------------*/
//===================================================================
//  Обработчик меню мастера настройки
//===================================================================
void HandlerMenuMasterSet(u16 Key)
{
    DescriptorMenu_type                 DM;
    DescriptorMenuOld_type              DMOld;
    HeadOfFile_type                     HOF;
    HeadOfFileOld_type                  HOFOld;
    static struct{
        u16         cntPrm;
        ModeMenuSetup_type         Mode;
    }MenuSetup = {0};
    u32                                 netAdr;
    u32                                 cntPrm;
    Full_DescriptorPrm_type             *pntDPDriveTmp;
    u16                                 tmp, status;
    u16         				        lenStr;
    static u16                          FirstItr = 1;

    netAdr = HandlerMenuSetup.netAdr;

    if (HandlerMenuSetup.getIndex == 0) 
    {
        if (gIsNvsaOld)
        {
            //========================
            //Чтение заголовка файла
            //========================
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&HOFOld, HandlerMenuSetup.BaseHOF, sizeof(HeadOfFileOld_type));
            #else            
            memcpy(&HOFOld, (void *)((u32)HandlerMenuSetup.BaseHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFileOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
            #endif
            //========================
            //Чтение описателя группы
            //========================
            if (HandlerMenuSetup.BaseDM)
            {
                #ifdef BLOCK_STM_MEMORY
                AT45RD(&DMOld, HandlerMenuSetup.BaseDM, sizeof(DescriptorMenuOld_type));
                #else            
                memcpy(&DMOld, (void *)((u32)HandlerMenuSetup.BaseDM + STM32_FLASH_OFFSET), sizeof(DescriptorMenuOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
                #endif
            }
            else
            {
                memcpy(&DMOld, &HandlerMenuSetup.DMOld, sizeof(DescriptorMenuOld_type));
            }
            //========================
            //Поиск параметров и инициализация массива указателей на DP DPGlobal
            //========================
            cntPrm = FindVisibleParam(&HOFOld,&DMOld,HandlerMenuSetup.BaseHOF, 0);
            MenuSetup.cntPrm = cntPrm - 1; //Без 0-го параметра
            if (cntPrm < 2) 
            {
                SetModePrevMenu();
                DeInitAllDP();
                return;
            }
            //========================
            //Описатель 1-го параметра (ссылка на текущий параметр)
            //========================
            InitDP(&MDPDrive.DPDrive[0], HandlerMenuSetup.BaseHOF, DPGlobalOld[0], netAdr);
            MDPDrive.DPDrive[0].NumParam = 0;
            InitDP(&MDPDrive.DPDrive[1], HandlerMenuSetup.BaseHOF, DPGlobalOld[1], netAdr);
            MDPDrive.DPDrive[1].NumParam = 1;
            memset(&MDPDrive.DPDrive[2], 0, sizeof(Full_DescriptorPrm_type));
            memset(&MDPDrive.DPDrive[3], 0, sizeof(Full_DescriptorPrm_type));
            HandlerMenuSetup.getIndex = 1;
        }
        else
        {
            //========================
            //Чтение заголовка файла
            //========================
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&HOF, HandlerMenuSetup.BaseHOF, sizeof(HeadOfFile_type));
            #else            
            memcpy(&HOF, (void *)((u32)HandlerMenuSetup.BaseHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
            #endif
            //========================
            //Чтение описателя группы
            //========================
            if (HandlerMenuSetup.BaseDM)
            {
                #ifdef BLOCK_STM_MEMORY
                AT45RD(&DM, HandlerMenuSetup.BaseDM, sizeof(DescriptorMenu_type));
                #else            
                memcpy(&DM, (void *)((u32)HandlerMenuSetup.BaseDM + STM32_FLASH_OFFSET), sizeof(DescriptorMenu_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                           
                #endif
            }
            else
            {
                memcpy(&DM, &HandlerMenuSetup.DM, sizeof(DescriptorMenu_type));
            }
            //========================
            //Поиск параметров и инициализация массива указателей на DP DPGlobal
            //========================
            cntPrm = FindVisibleParam(&HOF,&DM,HandlerMenuSetup.BaseHOF, 0);
            MenuSetup.cntPrm = cntPrm - 1; //Без 0-го параметра
            if (cntPrm < 2) 
            {
                SetModePrevMenu();
                DeInitAllDP();
                //memset(&MenuJrn, 0, sizeof(MenuJrn));
                return;
            }
            //========================
            //Описатель 1-го параметра (ссылка на текущий параметр)
            //========================
            InitDP(&MDPDrive.DPDrive[0], HandlerMenuSetup.BaseHOF, DPGlobal[0], netAdr);
            MDPDrive.DPDrive[0].NumParam = 0;
            InitDP(&MDPDrive.DPDrive[1], HandlerMenuSetup.BaseHOF, DPGlobal[1], netAdr);
            MDPDrive.DPDrive[1].NumParam = 1;
            memset(&MDPDrive.DPDrive[2], 0, sizeof(Full_DescriptorPrm_type));
            memset(&MDPDrive.DPDrive[3], 0, sizeof(Full_DescriptorPrm_type));
            HandlerMenuSetup.getIndex = 1;
        }
    }

    //Обработка кнопок
    switch (Key) 
    {
        case KEY_RIGHT:
            MenuSetup.Mode = ModeMenuSetup_EDIT;
            break;
        case KEY_LEFT:
                FirstItr = 1;
                SetModePrevMenu();
                memset(&MDPDrive.DPDrive[0], 0, sizeof(Full_DescriptorPrm_type));
                memset(&MDPDrive.DPDrive[1], 0, sizeof(Full_DescriptorPrm_type));
                memset(&MenuSetup, 0, sizeof(MenuSetup));
                return;
            break;
        case KEY_UP:
            if (MDPDrive.DPDrive[1].ValidData != 0)//MODE_ID_JRN)
            {
                if (MDPDrive.DPDrive[1].wm.s32.Value > 0)
                {
                    MDPDrive.DPDrive[1].wm.s32.Value--;
                    ModbusMakeOpis(&MDPDrive.DPDrive[1], WRITE_UST);
                    MDPDrive.DPDrive[1].Ignore             = 1;
                    MDPDrive.DPDrive[1].ValidData          = 0;
                    MDPDrive.DPDrive[1].ValidDataFirst     = 0;
                    if (gIsNvsaOld)
                    {
                        InitDP(&MDPDrive.DPDrive[0], HandlerMenuSetup.BaseHOF, DPGlobalOld[0], netAdr);
                    }
                    else
                    {
                        InitDP(&MDPDrive.DPDrive[0], HandlerMenuSetup.BaseHOF, DPGlobal[0], netAdr);
                    }
                }
            }
            break;
        case KEY_DOWN:
            if (MDPDrive.DPDrive[1].ValidData != 0)//MODE_ID_JRN)
            {
                MDPDrive.DPDrive[1].wm.s32.Value++;
                ModbusMakeOpis(&MDPDrive.DPDrive[1], WRITE_UST);
                MDPDrive.DPDrive[1].Ignore             = 1;
                MDPDrive.DPDrive[1].ValidData          = 0;
                MDPDrive.DPDrive[1].ValidDataFirst     = 0;
                if (gIsNvsaOld)
                {
                    InitDP(&MDPDrive.DPDrive[0], HandlerMenuSetup.BaseHOF, DPGlobalOld[0], netAdr);
                }
                else
                {
                    InitDP(&MDPDrive.DPDrive[0], HandlerMenuSetup.BaseHOF, DPGlobal[0], netAdr);
                }
            }
            break;
        case KEY_ENTER:
            MenuSetup.Mode = ModeMenuSetup_EDIT;
            break;
        case KEY_ESC:
            if (MenuSetup.Mode == ModeMenuSetup_MENU)
            {
                if (MDPDrive.DPDrive[0].ValidData != 0)//MODE_ID_JRN)
                {
                    //lenStr = getHelp(HandlerMenuSetup.BaseHOF, &StrOut[0], MDPDrive.DPDrive[0].DP.OfsHlp);
                    if (gIsNvsaOld)
                    {         
                        lenStr = getHelpPrm(&MDPDrive.DPDrive[0], &StrOut[0]);
                    }
                    else
                    {
                        lenStr = getHelp(HandlerMenuSetup.BaseHOF, &StrOut[0], MDPDrive.DPDrive[0].DP.OfsHlp);
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
            }
            break;
        case KEY_FUNC:
                FirstItr = 1;
                SetModePrevMenu();
                memset(&MDPDrive.DPDrive[0], 0, sizeof(Full_DescriptorPrm_type));
                memset(&MDPDrive.DPDrive[1], 0, sizeof(Full_DescriptorPrm_type));
                memset(&MenuSetup, 0, sizeof(MenuSetup));
                return;
            break;
        default:
            break;
    }
    GrafBffClear();
    if (MenuSetup.Mode == ModeMenuSetup_MENU)
    {
        ParamStartOneShotReadAT45_ForSetup(HandlerMenuSetup.BaseHOF, &MDPDrive.DPDrive[0], &MDPDrive.DPDrive[1]);
    }
    else if (MenuSetup.Mode == ModeMenuSetup_EDIT) 
    {
            pntDPDriveTmp = &MDPDrive.DPDrive[0];
            tmp = 1;
                if (MDPDrive.DPDrive[NUM_STATUS].ValidData == 0 || pntDPDriveTmp->ValidData == 0 )
                {
                	tmp = 0;
                }
                else
                {
                    status = MDPDrive.DPDrive[NUM_STATUS].wm.s32.Value;
                    if (gIsNvsaOld)
                    {
                        if (pntDPDriveTmp->DPOld.FlgPrm.EditType == NO_EDIT){
                            MessagePopUp(GetStringMess(MessageMess_PARAM_NOT_EDIT));
                            tmp = 0;
                        }
                        else if (pntDPDriveTmp->DPOld.FlgPrm.EditType == STOP_EDIT) {
                            if (getStatusWorkAll(status) || getStatusCharge(status)) {
                                MessagePopUp(GetStringMess(MessageMess_PARAM_EDIT_STOP));
                                tmp = 0;
                            }
                        }
                    }
                    else
                    {
                        if (pntDPDriveTmp->DP.FlgPrm.EditType == NO_EDIT){
                            MessagePopUp(GetStringMess(MessageMess_PARAM_NOT_EDIT));
                            tmp = 0;
                        }
                        else if (pntDPDriveTmp->DP.FlgPrm.EditType == STOP_EDIT) {
                            if (getStatusWorkAll(status) || getStatusCharge(status)) {
                                MessagePopUp(GetStringMess(MessageMess_PARAM_EDIT_STOP));
                                tmp = 0;
                            }
                        }
                    }
                }
            if (tmp == 1) 
            {
                ParamEdit(HandlerMenuSetup.BaseHOF, pntDPDriveTmp);
                ReqEtalonPassword();                                                    // Опросить привод и прочитать пароли
                Password();  
            }
            MenuSetup.Mode = ModeMenuSetup_MENU;
            if (MDPDrive.DPDrive[1].ValidData != 0)//MODE_ID_JRN)
            {
                MDPDrive.DPDrive[1].wm.s32.Value++;
                ModbusMakeOpis(&MDPDrive.DPDrive[1], WRITE_UST);
                MDPDrive.DPDrive[1].Ignore             = 1;
                MDPDrive.DPDrive[1].ValidData          = 0;
                MDPDrive.DPDrive[1].ValidDataFirst     = 0;
                if (gIsNvsaOld)
                {
                    InitDP(&MDPDrive.DPDrive[0], HandlerMenuSetup.BaseHOF, DPGlobalOld[0], netAdr);
                }
                else
                {
                    InitDP(&MDPDrive.DPDrive[0], HandlerMenuSetup.BaseHOF, DPGlobal[0], netAdr);
                }
            }
    }
    // Разбор запросов к приводу
    RazborRx();
    // Присваеваем 1 при первом вхождении
    if (gIsNvsaOld)
    {
        if (((PrmFrmt_type)MDPDrive.DPDrive[1].DPOld.FlgPrm.TypeVar != refFormat) && FirstItr)
        {
            MDPDrive.DPDrive[1].wm.s32.Value = 1;
            ModbusMakeOpis(&MDPDrive.DPDrive[1], WRITE_UST);
            MDPDrive.DPDrive[1].ValidDataFirst = 0;
            InitDP(&MDPDrive.DPDrive[0], HandlerMenuSetup.BaseHOF, DPGlobalOld[0], netAdr);
            FirstItr = 0;
        }
    }
    else
    {
        if (((PrmFrmt_type)MDPDrive.DPDrive[1].DP.FlgPrm.TypeVar != refFormat) && FirstItr)
        {
            MDPDrive.DPDrive[1].wm.s32.Value = 1;
            ModbusMakeOpis(&MDPDrive.DPDrive[1], WRITE_UST);
            MDPDrive.DPDrive[1].ValidDataFirst = 0;
            InitDP(&MDPDrive.DPDrive[0], HandlerMenuSetup.BaseHOF, DPGlobal[0], netAdr);
            FirstItr = 0;
        }
    }
    GrfOut();
}

//===================================================================
// Нахождение дескриптора по значению ссылочного параметра
//===================================================================
inline FindDescriptor_type FindDescriptor(Full_DescriptorPrm_type *DPDrive, Status_type *StatusRef, void *HOF, void *pHOF, u8 NetAdr)
{
    DescriptorMenu_type     DM;
    DescriptorMenuOld_type  DMOld;
    DescriptorPrm_type      DP;
    DescriptorPrmOld_type   DPOld;
    FindDescriptor_type     Result;
    HeadOfFile_type         *pHOFNew;
    HeadOfFileOld_type      *pHOFOld;

    Result = FindDescriptor_FAIL;
    if (gIsNvsaOld)
    {
        pHOFOld = HOF;
        if (FindGroup(pHOFOld, &DMOld, pHOF, StatusRef->NumGrp))
        {
            if(DPDrive->BaseDP = FindParam(pHOFOld, &DMOld, &DPOld, pHOF, StatusRef->NumPrm))
            {
                DPDrive->AdrNet = NetAdr;
                DPDrive->BaseHOF = pHOF;
                memcpy(&DPDrive->DPOld, &DPOld, sizeof(DescriptorPrmOld_type));
                DPDrive->NumParam = 0;
                DPDrive->ValidData = 0;
                DPDrive->ValidDataFirst = 0;
                DPDrive->Ignore = 1;
                DPDrive->msgReqDrvIn.Busy = 0;
                Result = FindDescriptor_OK;
            }
        }
    }
    else
    {
        pHOFNew = HOF;
        if (FindGroup(pHOFNew, &DM, pHOF, StatusRef->NumGrp))
        {
            if(DPDrive->BaseDP = FindParam(pHOFNew, &DM, &DP, pHOF, StatusRef->NumPrm))
            {
                DPDrive->AdrNet = NetAdr;
                DPDrive->BaseHOF = pHOF;
                memcpy(&DPDrive->DP, &DP, sizeof(DescriptorPrm_type));
                DPDrive->NumParam = 0;
                DPDrive->ValidData = 0;
                DPDrive->ValidDataFirst = 0;
                DPDrive->Ignore = 1;
                DPDrive->msgReqDrvIn.Busy = 0;
                Result = FindDescriptor_OK;
            }
        }
    }
    return Result;
}
//===================================================================
//  Обработчик запроса значения параметра
//===================================================================
Drv_err_type RequestModbusOpisPrmDirect(Full_DescriptorPrm_type *DPDrive, u8 Type)
{
    Drv_err_type            drvErr;
    u8                      *pBuf;
    ModbusOpisPrm4byte_type ModbusOpisPrm4byte;
    ModbusOpisPrm2byte_type ModbusOpisPrm2byte;
    wm_type                 *wm;
    DescriptorPrm_type      DP;
    void                    *AdrDP;
    
    DPDrive->msgReqDrvIn.AdrNet   = DPDrive->AdrNet;
    if (Type == EXT)
    {
        DPDrive->msgReqDrvIn.AdrData  = 0x8000 + DPDrive->wm.AdrPrm;
    }
    else
    {
        DPDrive->msgReqDrvIn.AdrData  = DPDrive->wm.AdrPrm;
    }

    if (DPDrive->SzReq % 2 == 0)    DPDrive->msgReqDrvIn.Sz = DPDrive->SzReq >> 1;
    else                            DPDrive->msgReqDrvIn.Sz = (DPDrive->SzReq >> 1) + 1;
    DPDrive->msgReqDrvIn.Func     = READ_H_REGS;
    DPDrive->msgReqDrvIn.Buf      = DPDrive->BufParam;
    DPDrive->msgReqDrvIn.SzBuf    = sizeof(DPDrive->BufParam);
    DPDrive->msgReqDrvIn.Buf     =  DPDrive->BufParam;
    DPDrive->msgReqDrvIn.NumParam = DPDrive->NumParam;


#ifndef _PROJECT_IAR_ 
    DPDrive->msgReqDrvIn.countByteRead = DPDrive->msgReqDrvIn.Sz*2 + 5;  // Ожидаем по колич данных
#endif
    
    while(1)
    {
        drvErr = ReQuestDrv(QueueDrvIn, &DPDrive->msgReqDrvIn);
        if( drvErr == Ok_err )
        {
            pBuf = DPDrive->BufParam;
            DPDrive->ValidData = 1;
            DPDrive->ValidDataFirst = 1;

            wm = &DPDrive->wm;
            if (DPDrive->DP.FlgPrm.LimChange || DPDrive->DP.FlgPrm.FormatChange || DPDrive->DP.FlgPrm.NumBase)
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
                    AdrDP = (void *)((DescriptorPrm_type *)((u32)DPDrive->BaseDP + sizeof(DescriptorPrm_type)));
                    
                    #ifdef BLOCK_STM_MEMORY
                    AT45RD(&DP, AdrDP, sizeof(DescriptorPrm_type));
                    #else            
                    memcpy(&DP, (void *)((u32)AdrDP + STM32_FLASH_OFFSET), sizeof(DescriptorPrm_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                               
                    #endif

                    wm->s32.LowLim  = (DPDrive->DP.LowLim & 0xffff) + ((u32)DP.LowLim << 16);//Старшая часть пределов
                    wm->s32.HighLim = (DPDrive->DP.HighLim & 0xffff) + ((u32)DP.HighLim << 16);
                }
                else
                {                                                   //2-х байтный параметр
                    wm->s32.Value       = (u16)swapU16(*(u16 *)pBuf);
                    wm->s32.LowLim      = (u16)DPDrive->DP.LowLim;
                    wm->s32.HighLim      = (u16)DPDrive->DP.HighLim;
                }
            }
            return drvErr;
        }
    }
}

//===================================================================
// Проводим замену дескриптора для ссылочного параметра
//===================================================================
void ChangeDescriptorPrm(Full_DescriptorPrm_type *DPDrive, void *pHOF, u8 NetAdr)
{
    Status_type StatusRef;
    Full_DescriptorPrm_type DPDriveTemp;
    HeadOfFile_type HOF;
    
    StatusRef.NumGrp = DPDrive->wm.s32.Value >> 7;
    StatusRef.NumPrm = DPDrive->wm.s32.Value & 0x007F;
    
    #ifdef BLOCK_STM_MEMORY
    AT45RD(&HOF, pHOF, sizeof(HeadOfFile_type));
    #else            
    memcpy(&HOF, (void *)((u32)pHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса
    #endif
    
    if (FindDescriptor(&DPDriveTemp, &StatusRef, &HOF, pHOF, NetAdr))
    {
        InitDP(DPDrive,
                pHOF,
                DPDriveTemp.BaseDP,
                NetAdr);
    }
}

//===================================================================
// Отоброжение окна текущего параметра для мастера настройки 
//===================================================================
void ParamStartOneShotReadAT45_ForSetup(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, Full_DescriptorPrm_type *DPDriveNum)
{
    u32                     Ypos;
    u16                     j;
    wm_type                 *wm, wmem;
    OS_Q_DATA               QData;
    char                    strtmp[17];
//    u16                     status;
    const u8                *pntIconEdit;
    static u16              StatusDaleyValid = 0;
    static u16              StatusDaleyValid2 = 0;
    static u16              PrmDaleyValid[ MAX_STR_OTOBR_PRM ] = { 0, 0, 0, 0};
    HeadOfFile_type         HOF;
    HeadOfFileOld_type      HOFOld;


    strtmp[16]  = 0;
    wm = &wmem;
    Ypos = 0;
    if (gIsNvsaOld)
    {
        if (DPDrive->DPOld.FlgPrm.TypeVar == refFormat || (DPDrive->wm.TypeVar == refFormat))
        {
            sprintf(&StrOut[0], "[-,-]");
            StrOutFont(0, 144, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //Вывод внизу номера группы и параметра
            sprintf(&StrOut[0], "%s%s", &noValidData[0], &noValidData[0]);
            StrOutFont(MENUX, Ypos += INCY*4, NO_INV, &StrOut[0], &fontNormal, leftAlign ); // вывод прочерков
            sprintf(&StrOut[0], "%15s", &noValidData[0]);
            StrOutFont(0, Ypos += INCY, NO_INV, &StrOut[0], &fontNormal, leftAlign );
        }
        else
        {
            sprintf(&StrOut[0], "[%d,%d]",
                    DPDrive->DPOld.IDGrp,
                    DPDrive->DPOld.IDPrm);
            StrOutFont(0, 144, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //Вывод внизу номера группы и параметра

            switch( DPDrive->DPOld.FlgPrm.EditType )
            {  //Обработка статусов доступа
                case NO_EDIT:       pntIconEdit = InfoPrm;      break;
                case STOP_EDIT:     pntIconEdit = EditStopPrm;  break;
                default:            pntIconEdit = EditPrm;      break;
            }
            DrawIcon16x16(136, 144, pntIconEdit );
            
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&HOFOld, BaseHOF, sizeof(HeadOfFileOld_type));
            #else            
            memcpy(&HOFOld, (void *)((u32)BaseHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFileOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                                   
            #endif

            getNamePrm(BaseHOF, &HOFOld, &DPDrive->DPOld, &strtmp[0]);
            sprintf(&StrOut[0], "%s", &strtmp[0]);
            StrOutFont(MENUX, Ypos += INCY*4, NO_INV, &StrOut[0], &fontNormal, leftAlign ); 	//Вывод параметра
            if (DPDrive->ValidData == 1 && (DPDrive->ValidDataFirst == 1))
            {
                wm = &DPDrive->wm;
                ParamToStr(BaseHOF, DPDrive, wm, &StrOut[0]);
                sprintf(&strtmp[0], "%15s", &StrOut[0]);
                StrOutFont(0, Ypos += INCY, NO_INV,    &strtmp[0], &fontNormal, leftAlign );
                PrmDaleyValid[0] = 0;
            }
            else
            {
                if ((PrmDaleyValid[0] < 4) && (DPDrive->ValidDataFirst == 1))
                {
                    wm = &DPDrive->wm;
                    ParamToStr(BaseHOF, DPDrive, wm, &StrOut[0]);
                    sprintf(&strtmp[0], "%15s", &StrOut[0]);
                    StrOutFont(0, Ypos += INCY, NO_INV,    &strtmp[0], &fontNormal, leftAlign );    
                }
                else
                {
                    sprintf(&StrOut[0], "%15s", &noValidData[0]);
                    StrOutFont(0, Ypos += INCY, NO_INV, &StrOut[0], &fontNormal, leftAlign );
                }
                PrmDaleyValid[0]++;
            }
        }
        Ypos -= INCY;
        DrawLine(0, Ypos - 2, 160, Ypos - 2, 1, 3, 3);        				    //Рисование линии перед вывода параметра
        DrawLine(0, Ypos + 34, 160, Ypos + 34, 1, 3, 3);        				//Рисование линии после вывода параметра
    }
    else
    {
        if (DPDrive->DP.FlgPrm.TypeVar == refFormat || (DPDrive->wm.TypeVar == refFormat))
        {
            sprintf(&StrOut[0], "[-,-]");
            StrOutFont(0, 144, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //Вывод внизу номера группы и параметра
            sprintf(&StrOut[0], "%s%s", &noValidData[0], &noValidData[0]);
            StrOutFont(MENUX, Ypos += INCY*4, NO_INV, &StrOut[0], &fontNormal, leftAlign ); // вывод прочерков
            sprintf(&StrOut[0], "%15s", &noValidData[0]);
            StrOutFont(0, Ypos += INCY, NO_INV, &StrOut[0], &fontNormal, leftAlign );
        }
        else
        {
            sprintf(&StrOut[0], "[%d,%d]",
                    DPDrive->DP.IDGrp,
                    DPDrive->DP.IDPrm);
            StrOutFont(0, 144, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //Вывод внизу номера группы и параметра

            switch( DPDrive->DP.FlgPrm.EditType )
            {  //Обработка статусов доступа
                case NO_EDIT:       pntIconEdit = InfoPrm;      break;
                case STOP_EDIT:     pntIconEdit = EditStopPrm;  break;
                default:            pntIconEdit = EditPrm;      break;
            }
            DrawIcon16x16(136, 144, pntIconEdit );
            
            #ifdef BLOCK_STM_MEMORY
            AT45RD(&HOF, BaseHOF, sizeof(HeadOfFile_type));
            #else            
            memcpy(&HOF, (void *)((u32)BaseHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                                               
            #endif

            //memcpy(&strtmp[0], &DPDrive->DP.Name, sizeof(DPDrive->DP.Name));
            getNamePrm(BaseHOF, &HOF, &DPDrive->DP, &strtmp[0]);
            sprintf(&StrOut[0], "%s", &strtmp[0]);
            StrOutFont(MENUX, Ypos += INCY*4, NO_INV, &StrOut[0], &fontNormal, leftAlign ); 	//Вывод параметра
            if (DPDrive->ValidData == 1 && (DPDrive->ValidDataFirst == 1))
            {
                wm = &DPDrive->wm;
                ParamToStr(BaseHOF, DPDrive, wm, &StrOut[0]);
                sprintf(&strtmp[0], "%15s", &StrOut[0]);
                StrOutFont(0, Ypos += INCY, NO_INV,    &strtmp[0], &fontNormal, leftAlign );
                PrmDaleyValid[0] = 0;
            }
            else
            {
                if ((PrmDaleyValid[0] < 4) && (DPDrive->ValidDataFirst == 1))
                {
                    wm = &DPDrive->wm;
                    ParamToStr(BaseHOF, DPDrive, wm, &StrOut[0]);
                    sprintf(&strtmp[0], "%15s", &StrOut[0]);
                    StrOutFont(0, Ypos += INCY, NO_INV,    &strtmp[0], &fontNormal, leftAlign );    
                }
                else
                {
                    sprintf(&StrOut[0], "%15s", &noValidData[0]);
                    StrOutFont(0, Ypos += INCY, NO_INV, &StrOut[0], &fontNormal, leftAlign );
                }
                PrmDaleyValid[0]++;
            }
        }
        Ypos -= INCY;
        DrawLine(0, Ypos - 2, 160, Ypos - 2, 1, 3, 3);        				    //Рисование линии перед вывода параметра
        DrawLine(0, Ypos + 34, 160, Ypos + 34, 1, 3, 3);        				//Рисование линии после вывода параметра
    }
    //*************************************************************
    // Вывод текущего номера параметра
    //*************************************************************       
            if (DPDriveNum->ValidData == 1 && (DPDriveNum->ValidDataFirst == 1))
            {
                wm = &DPDriveNum->wm;
                ParamToStr(BaseHOF, DPDriveNum, wm, &StrOut[0]);
                sprintf(&strtmp[0], "  %s %s",GetStringTitle(MessageTitul_PARAM), &StrOut[0]);
                StrOutFont(MENUX, INCY, NO_INV, &strtmp[0], &fontNormal, leftAlign ); 	//Вывод параметра
                PrmDaleyValid[1] = 0;
            }
            else
            {
                if ((PrmDaleyValid[1] < 4) && (DPDriveNum->ValidDataFirst == 1))
                {
                    wm = &DPDriveNum->wm;
                    ParamToStr(BaseHOF, DPDriveNum, wm, &StrOut[0]);
                    sprintf(&strtmp[0], "  %s %s", GetStringTitle(MessageTitul_PARAM), &StrOut[0]);
                    //StrOutFont(0, Ypos += INCY, NO_INV, &strtmp[0], &fontNormal, leftAlign );
                    StrOutFont(0, INCY, NO_INV,    &strtmp[0], &fontNormal, leftAlign );    
                }
                else
                {
                    sprintf(&StrOut[0], "  %s %s", GetStringTitle(MessageTitul_PARAM), "-");
                    //StrOutFont(0, Ypos += INCY, NO_INV, &StrOut[0], &fontNormal, leftAlign );
                    StrOutFont(0, INCY, NO_INV, &StrOut[0], &fontNormal, leftAlign );
                }
                PrmDaleyValid[1]++;
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
            parseWordStatus(MDPDrive.DPDrive[NUM_STATUS].AdrNet, StatusDrv);
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
            for (j = 0; j < 2; j++) 
            {
                ModbusMakeOpis(&MDPDrive.DPDrive[j], READ_UST);
            }
            ModbusMakeOpis(&MDPDrive.DPDrive[NUM_STATUS], READ_UST);
            ModbusMakeOpis(&MDPDrive.DPDrive[NUM_STATUS_2], READ_UST);
        }      
}

//===================================================================
// Отоброжение окна поиска по номеру параметра 
//===================================================================
u32 RefToStrEdit(Status_type *StatusRef)
{
    u32                 PosChar, VesDig;
    u32                 MaxLimDig;
    keyName_type        Key;
    char                strtmp[20];
    u8                  Mig;
    u8                  CntMig = 0;
    u8                  KeyPressed;
    u8                  err;
    s16                 ValGrp, ValPrm;
    s16                 LowLim, HighLim;
    char                *str;

    str = &StrOut[0];
    Mig = 1;
    ValGrp = 0;
    ValPrm = 0;

    //wm->s32.LowLim = -1000;
    VesDig = 0;
    LowLim = 0;
    HighLim = 99;
    MaxLimDig = 4;
    strtmp[0] = 0;

    while(1){
      ResetWD(tsk_lcd);
        OSSemPend(SemMenu, SYS_FOREVER, &err );
        Key = WaitKey(&KeyPressed);

        switch (Key) {
            case KEY_UP:
                if (VesDig < 2)
                {
                    ValPrm += (u16)AntiLg(VesDig);
                    if ((ValPrm > HighLim) || (ValPrm < (ValPrm - (u16)AntiLg(VesDig)))) 
                    {
                        ValPrm = HighLim;
                    }
                }
                else
                {
                    ValGrp += (u16)AntiLg(VesDig-2);
                    if ((ValGrp > HighLim) || (ValGrp < (ValGrp - (u16)AntiLg(VesDig-2)))) 
                    {
                        ValGrp = HighLim;
                    }
                }
                break;
            case KEY_DOWN:
                if (VesDig < 2)
                {
                    ValPrm -= (u16)AntiLg(VesDig);
                    if ((ValPrm < LowLim) || (ValPrm > (s16)(ValPrm + (u16)AntiLg(VesDig)))) 
                    {
                        ValPrm = LowLim;
                    }
                }
                else
                {
                    ValGrp -= (u16)AntiLg(VesDig-2);
                    if ((ValGrp < LowLim) || (ValGrp > (s16)(ValGrp + (u16)AntiLg(VesDig-2)))) 
                    {
                        ValGrp = LowLim;
                    }
                }
                break;
            case KEY_LEFT:
                if (VesDig < MaxLimDig - 1) 
                {
                    VesDig++;
                }
                break;
            case KEY_RIGHT:
                if (VesDig > 0) 
                {
                    VesDig --;
                }
                break;
            case KEY_ENTER:
                StatusRef->NumGrp = ValGrp;
                StatusRef->NumPrm = ValPrm;
                return 1;
            case KEY_FUNC:
                StatusRef->NumGrp = ValGrp;
                StatusRef->NumPrm = ValPrm;
                return 1;
            case KEY_ESC:
                StatusRef->NumGrp = 0;
                StatusRef->NumPrm = 0;
                return 0;
        }
        memset(&strtmp[0], 0, sizeof(strtmp));
        sprintf(str, "%02d:%02d", ValGrp, ValPrm);

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
        sprintf(&strtmp[0], "  %6s", str);
        StrOutFont(0, 60, NO_INV, &strtmp[0], &fontBig, leftAlign );
        //==========================================================
        //Название параметра
        //==========================================================
        memcpy(str, GetStringTitle(MessageTitul_ENTER_PARAM), 16);

        StrOutFont(0, 0, STR_INV, str, &fontNormal, leftAlign );
        //==========================================================
        //Min и Max
        //==========================================================
        sprintf(&strtmp[0], "%s  %02d:%02d", GetStringTitle(MessageTitul_MIN), LowLim, LowLim);
        StrOutFont(0, 120, NO_INV, &strtmp[0], &fontNormal, leftAlign );
        
        sprintf(&strtmp[0], "%s %02d:%02d", GetStringTitle(MessageTitul_MAX), HighLim, HighLim);
        StrOutFont(0, 140, NO_INV, &strtmp[0], &fontNormal, leftAlign );
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
// Нахождение дескриптора по значению ссылочного параметра с проверкой на видимость
//===================================================================
FindDescriptor_type FindDescriptorWhithCheck(Full_DescriptorPrm_type *DPDrive, Status_type *StatusRef, void *HOF, void *pHOF, u8 NetAdr, u16 NumLnk)
{
    DescriptorMenu_type     DM;
    DescriptorPrm_type      DP;
    DescriptorMenuOld_type  DMOld;
    DescriptorPrmOld_type   DPOld;
    HeadOfFileOld_type      *pHOFOld;
    HeadOfFile_type         *pHOFNew;
    FindDescriptor_type     Result;
    //void                	*AdrParam;
    void                	*AdrFunct;
    FileFunctional_type 	Functional;
    u16                 	tmp;

    Result = FindDescriptor_FAIL;
    if (NumLnk > MAXDRIVES_CUT - 1) 
    {
        return Result;
    }
    AdrFunct = &nv.OpisDrv[NumLnk].Functional;
    FM25RD(&Functional, AdrFunct, sizeof(Functional));
    tmp = 0;
    
    if (gIsNvsaOld)
    {
        pHOFOld = HOF;
        if (FindGroup(pHOFOld, &DMOld, pHOF, StatusRef->NumGrp))
        {
            if (DMOld.FlgMenu.TypeMenu == TypeMenu_MMASTER)
            {
                return Result;
            }
            if(DPDrive->BaseDP = FindParam(pHOFOld, &DMOld, &DPOld, pHOF, StatusRef->NumPrm))
            {    
                if (DPDrive->BaseDP && DPOld.FlgPrm.TypeVar != sec2Frmt) {
                    if ((DPOld.Functional & (Functional.curFunct.functMask >> 8)) != 0) {
                        if (DPOld.FlgPrm.MasterVis == 1 && DPOld.FlgPrm.TexnVis == 1 && DPOld.FlgPrm.UserVis == 1){
                            tmp = 1;
                        }
                        else if (DPOld.FlgPrm.UserVis == 1 && LevelAccess){
                            tmp = 1;
                        }
                        else if(DPOld.FlgPrm.MasterVis  == 1 && LevelAccess == MASTER_PASSW ||
                                DPOld.FlgPrm.TexnVis    == 1 && LevelAccess == TEXN_PASSW){
                            tmp = 1;
                        }
                        else if (DMOld.FlgMenu.TypeMenu == 4){                            //Журнал
                            tmp = 1;
                        }
                    }
                }
                if (tmp != 0) 
                {
                    DPDrive->AdrNet = NetAdr;
                    DPDrive->BaseHOF = pHOF;
                    memcpy(&DPDrive->DPOld, &DPOld, sizeof(DescriptorPrmOld_type));
                    DPDrive->NumParam = 0;
                    DPDrive->ValidData = 0;
                    DPDrive->ValidDataFirst = 0;
                    DPDrive->Ignore = 1;
                    DPDrive->msgReqDrvIn.Busy = 0;
                    Result = FindDescriptor_OK;
                }
            }
        }
    }
    else
    {
        pHOFNew = HOF;
        if (FindGroup(pHOFNew, &DM, pHOF, StatusRef->NumGrp))
        {
            if (DM.FlgMenu.TypeMenu == TypeMenu_MMASTER)
            {
                return Result;
            }
            if(DPDrive->BaseDP = FindParam(pHOFNew, &DM, &DP, pHOF, StatusRef->NumPrm))
            {    
               // AdrParam = GetOpisParam(HOF, &DM, &DP, pHOF, FLASH_OPIS, StatusRef->NumPrm);
                if (/*AdrParam*/DPDrive->BaseDP && DP.FlgPrm.TypeVar != sec2Frmt) {
                    if ((DP.Functional & (Functional.curFunct.functMask >> 8)) != 0) {
                        if (DP.FlgPrm.MasterVis == 1 && DP.FlgPrm.TexnVis == 1 && DP.FlgPrm.UserVis == 1){
                            tmp = 1;
                        }
                        else if (DP.FlgPrm.UserVis == 1 && LevelAccess){
                            tmp = 1;
                        }
                        else if(DP.FlgPrm.MasterVis  == 1 && LevelAccess == MASTER_PASSW ||
                                DP.FlgPrm.TexnVis    == 1 && LevelAccess == TEXN_PASSW){
                            tmp = 1;
                        }
                        else if (DM.FlgMenu.TypeMenu == 4){                            //Журнал
                            tmp = 1;
                        }
                    }
                }
                if (tmp != 0) 
                {
                    DPDrive->AdrNet = NetAdr;
                    DPDrive->BaseHOF = pHOF;
                    memcpy(&DPDrive->DP, &DP, sizeof(DescriptorPrm_type));
                    DPDrive->NumParam = 0;
                    DPDrive->ValidData = 0;
                    DPDrive->ValidDataFirst = 0;
                    DPDrive->Ignore = 1;
                    DPDrive->msgReqDrvIn.Busy = 0;
                    Result = FindDescriptor_OK;
                }
            }
        }
    }
    return Result;  // Проверить результат возврата
}

//===================================================================
// Нахождение меню с искомым параметром
//===================================================================
u16 FindMenuWhithPrm(Full_DescriptorPrm_type  *DPDrive, void *BaseHOF, u16 NumMenu, TypeVisible_type TypeVisible, u8 NetAdr, u16 NumLnk)
{
    u16 i, tmp, FirstGrp;
    u16 CountDM;
    u16 DMNumHOF[MAX_NUM_MENU];
    u16 CountPos[MAX_LEVEL_MENU];
    u16 AdrMenuTemp;        // Адрес меню
    u16 NumPrm;
    DescriptorMenu_type     DM;
    DescriptorMenuOld_type  DMOld;
    DescriptorPrm_type      DP;
    DescriptorPrmOld_type   DPOld;
    
    FirstGrp = 1;
    SetRootMenu(&HandlerMenuPultNew_struct);
    AdrMenuTemp = HandlerMenuPultNew_struct.AdrLevelMenu;
    if (gIsNvsaOld)
    {
        HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOFOld, BaseHOF, NumLnk, AdrMenuTemp, TypeVisible);
        for (i = 0; i < HandlerMenuPultNew_struct.CntMP; i++)
        {
            InitDM(&DMDriveOld[i], i);  
            DMNumHOF[i] = DMNumGlobalHOF[i];
        }
    }
    else
    {
        HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOF, BaseHOF, NumLnk, AdrMenuTemp, TypeVisible);
        for (i = 0; i < HandlerMenuPultNew_struct.CntMP; i++)
        {
            InitDM(&DMDrive[i], i);  
            DMNumHOF[i] = DMNumGlobalHOF[i];
        }
    }

    for (CountDM = CountPos[ HandlerMenuPultNew_struct.Level ] = HandlerMenuPultNew_struct.CntMenu; CountDM > 0; CountDM--)
    {
        if (CountPos[ HandlerMenuPultNew_struct.Level ])
        {
            CountPos[ HandlerMenuPultNew_struct.Level ]--;
        }
        else
        {
                if (HandlerMenuPultNew_struct.Level)
                {
                    HandlerMenuPultNew_struct.Level--;
                    HandlerMenuPultNew_struct.AdrLevelMenuFirst = HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].AdrLevel;
                }
                if (HandlerMenuPultNew_struct.Level)
                {
                    if (CountPos[ HandlerMenuPultNew_struct.Level ])
                    {
                        CountPos[ HandlerMenuPultNew_struct.Level ]--;
                    }
                    else
                    {  
                        do
                        {
                            HandlerMenuPultNew_struct.Level--;
                            HandlerMenuPultNew_struct.AdrLevelMenuFirst = HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].AdrLevel;
                        }
                        while (!CountPos[ HandlerMenuPultNew_struct.Level ]);
                        CountPos[ HandlerMenuPultNew_struct.Level ]--;
                    }
                }
        }
        if (!HandlerMenuPultNew_struct.Level)
        {
            HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].AdrLevel = ROOT_MENU;
            if ((CountPos[ HandlerMenuPultNew_struct.Level ] - 1) < MAX_STR_OTOBR_MENU ||
                (CountPos[ HandlerMenuPultNew_struct.Level ] == (HandlerMenuPultNew_struct.CntMenu - 1) && 
                 CountPos[ HandlerMenuPultNew_struct.Level ] < MAX_STR_OTOBR_MENU))
            {
                if (FirstGrp)
                {
                    if ((((DMDrive[CountDM - 1].FlgMenu.TypeMenu == TypeMenu_MGRP_GRPS) && !gIsNvsaOld) ||
                         ((DMDriveOld[CountDM - 1].FlgMenu.TypeMenu == TypeMenu_MGRP_GRPS) && gIsNvsaOld))
                        && FirstGrp && CountPos[ HandlerMenuPultNew_struct.Level ] == (HandlerMenuPultNew_struct.CntMenu - 1))
                    {
                        FirstGrp = 0;
                    }
                    HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPosCursor = CountPos[ HandlerMenuPultNew_struct.Level ];
                    HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPos = CountPos[ HandlerMenuPultNew_struct.Level ];  
                }
                else
                {
                    HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPosCursor = CountPos[ HandlerMenuPultNew_struct.Level ] - 1;
                    HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPos = CountPos[ HandlerMenuPultNew_struct.Level ] - 1;  
                }
            }
            else
            {
                if (CountPos[ HandlerMenuPultNew_struct.Level ] == (HandlerMenuPultNew_struct.CntMenu - 1))
                {
                    HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPosCursor = MAX_STR_OTOBR_MENU - 1;
                    HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPos = CountPos[ HandlerMenuPultNew_struct.Level ];  
                }
                else
                {
                    HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPosCursor = MAX_STR_OTOBR_MENU - 1;
                    HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPos = CountPos[ HandlerMenuPultNew_struct.Level ] - 1;
                }
            }
        }
        else
        {
            if ((CountPos[ HandlerMenuPultNew_struct.Level ]) < MAX_STR_OTOBR_MENU)
            {
                HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPosCursor = CountPos[ HandlerMenuPultNew_struct.Level ] ;
                HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPos = CountPos[ HandlerMenuPultNew_struct.Level ];  
            }
            else
            {
                HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPosCursor = MAX_STR_OTOBR_MENU - 1;
                HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPos = CountPos[ HandlerMenuPultNew_struct.Level ];  
            }
            HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].AdrLevel = HandlerMenuPultNew_struct.AdrLevelMenuFirst;
        }
        if (((DMDrive[CountDM - 1].FlgMenu.TypeMenu == TypeMenu_MGRP_GRPS) && !gIsNvsaOld) || 
          ((DMDriveOld[CountDM - 1].FlgMenu.TypeMenu == TypeMenu_MGRP_GRPS) && gIsNvsaOld))
        {
            HandlerMenuPultNew_struct.AdrLevelMenuFirst = DMNumHOF[CountDM - 1];
            AdrMenuTemp = HandlerMenuPultNew_struct.AdrLevelMenuFirst;
            HandlerMenuPultNew_struct.Level++;
            if (gIsNvsaOld)
            {
                HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOFOld, BaseHOF, NumLnk, AdrMenuTemp, TypeVisible);
                for (i = 0; i < HandlerMenuPultNew_struct.CntMP; i++)
                {
                    InitDM(&DMDriveOld[CountDM + i - 1], i);  
                    DMNumHOF[CountDM + i - 1] = DMNumGlobalHOF[i];
                }
            }
            else
            {
                HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntMenu = FindVisibleMenu(&HandlerMenuPultNew_struct.HOF, BaseHOF, NumLnk, AdrMenuTemp, TypeVisible);
                for (i = 0; i < HandlerMenuPultNew_struct.CntMP; i++)
                {
                    InitDM(&DMDrive[CountDM + i - 1], i);  
                    DMNumHOF[CountDM + i - 1] = DMNumGlobalHOF[i];
                }
            }
            CountDM += HandlerMenuPultNew_struct.CntMP;
            CountPos[ HandlerMenuPultNew_struct.Level ] = HandlerMenuPultNew_struct.CntMP;
        }
        else
        {
            NumPrm = 0;
            if (gIsNvsaOld)
            {
                HandlerMenuPultNew_struct.CntParam = FindVisibleParamWhithPrm(&HandlerMenuPultNew_struct.HOFOld, &DMDriveOld[CountDM - 1], BaseHOF, NumLnk, DPDrive, &NumPrm);         
            }
            else
            {
                HandlerMenuPultNew_struct.CntParam = FindVisibleParamWhithPrm(&HandlerMenuPultNew_struct.HOF, &DMDrive[CountDM - 1], BaseHOF, NumLnk, DPDrive, &NumPrm);         
            }
            if (HandlerMenuPultNew_struct.CntParam)
            {
                if (NumPrm)
                {
                    if ((NumPrm - 1) < MAX_STR_OTOBR_PRM)
                    {
                        tmp = 0;
                    }
                    else
                    {
                        tmp = NumPrm - MAX_STR_OTOBR_PRM;
                    }
                    if (gIsNvsaOld)
                    {
                        HandlerMenuPultNew_struct.DMOld = &DMDriveOld[CountDM - 1];
                        for (i = tmp; i < MAX_STR_OTOBR_PRM + tmp && i < HandlerMenuPultNew_struct.CntParam + tmp; i++) 
                        {	//Начальное наполнение дескрипторов меню
                            InitDP(&MDPDrive.DPDrive[i - tmp], BaseHOF, DPGlobalOld[i], NetAdr);
                        }
                        //------------------------------------------------------
                        //Считываем описатели на дополнительные параметры
                        //------------------------------------------------------
                        if (FindGroup(&HandlerMenuPultNew_struct.HOFOld, &DMOld, BaseHOF, GRP_STATUS))
                        {
                            if(MDPDrive.DPDrive[NUM_STATUS].BaseDP = FindParam(&HandlerMenuPultNew_struct.HOFOld, &DMOld, &DPOld, BaseHOF, PRM_STATUS))
                            {
                                    MDPDrive.DPDrive[NUM_STATUS].AdrNet 	= NetAdr;
                                    MDPDrive.DPDrive[NUM_STATUS].BaseHOF 	= BaseHOF;
                                    InitDP(&MDPDrive.DPDrive[NUM_STATUS], BaseHOF, MDPDrive.DPDrive[NUM_STATUS].BaseDP, NetAdr);
                            }
                            if(MDPDrive.DPDrive[NUM_STATUS_2].BaseDP = FindParam(&HandlerMenuPultNew_struct.HOFOld, &DMOld, &DPOld, BaseHOF, PRM_STATUS_2))
                            {
                                MDPDrive.DPDrive[NUM_STATUS_2].AdrNet 	= NetAdr;
                                MDPDrive.DPDrive[NUM_STATUS_2].BaseHOF 	= BaseHOF;
                                InitDP(&MDPDrive.DPDrive[NUM_STATUS_2], BaseHOF, MDPDrive.DPDrive[NUM_STATUS_2].BaseDP, NetAdr);
                            }
                        }
                    }
                    else
                    {
                        HandlerMenuPultNew_struct.DM = &DMDrive[CountDM - 1];
                        for (i = tmp; i < MAX_STR_OTOBR_PRM + tmp && i < HandlerMenuPultNew_struct.CntParam + tmp; i++) 
                        {	//Начальное наполнение дескрипторов меню
                            InitDP(&MDPDrive.DPDrive[i - tmp], BaseHOF, DPGlobal[i], NetAdr);
                        }
                        //------------------------------------------------------
                        //Считываем описатели на дополнительные параметры
                        //------------------------------------------------------
                        if (FindGroup(&HandlerMenuPultNew_struct.HOF, &DM, BaseHOF, GRP_STATUS))
                        {
                            if(MDPDrive.DPDrive[NUM_STATUS].BaseDP = FindParam(&HandlerMenuPultNew_struct.HOF, &DM, &DP, BaseHOF, PRM_STATUS))
                            {
                                    MDPDrive.DPDrive[NUM_STATUS].AdrNet 	= NetAdr;
                                    MDPDrive.DPDrive[NUM_STATUS].BaseHOF 	= BaseHOF;
                                    InitDP(&MDPDrive.DPDrive[NUM_STATUS], BaseHOF, MDPDrive.DPDrive[NUM_STATUS].BaseDP, NetAdr);
                            }
                            if(MDPDrive.DPDrive[NUM_STATUS_2].BaseDP = FindParam(&HandlerMenuPultNew_struct.HOF, &DM, &DP, BaseHOF, PRM_STATUS_2))
                            {
                                MDPDrive.DPDrive[NUM_STATUS_2].AdrNet 	= NetAdr;
                                MDPDrive.DPDrive[NUM_STATUS_2].BaseHOF 	= BaseHOF;
                                InitDP(&MDPDrive.DPDrive[NUM_STATUS_2], BaseHOF, MDPDrive.DPDrive[NUM_STATUS_2].BaseDP, NetAdr);
                            }
                        }
                    }
                    for (i = 0 ; i < MAX_REQ_PRM; i++) 
                    {  //инициализация номеров параметров для разбора посылок
                        MDPDrive.DPDrive[i].NumParam = i;
                    }
                    HandlerMenuPultNew_struct.PosCursorPrev = HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPosCursor;
                    HandlerMenuPultNew_struct.PosPrev = HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].LevPos;
                    HandlerMenuPultNew_struct.AdrLevelMenuFirst = HandlerMenuPultNew_struct.OpisLevel[ HandlerMenuPultNew_struct.Level ].AdrLevel;
                    if (gIsNvsaOld)
                    {
                        if(DMDriveOld[CountDM - 1].FlgMenu.TypeMenu == TypeMenu_MJRN) 
                        {
                          //Просмотр журналов
                            HandlerMenuJrn_struct.netAdr = NetAdr;
                            memcpy(&HandlerMenuJrn_struct.DMOld, &DMDriveOld[CountDM - 1], sizeof(DescriptorMenuOld_type));
                            //HandlerMenuJrn_struct.DM = DMDrive[CountDM - 1]; //Базовый адрес группы
                            HandlerMenuJrn_struct.BaseHOF = BaseHOF;
                            HandlerMenuJrn_struct.BaseDM = NULL;
                            SetModeMenu(MODE_HandlerMenuJrn);
                            //return 2;
                        }
                        // Если текущее меню настройки
                        else if(DMDriveOld[i].FlgMenu.TypeMenu == TypeMenu_MMASTER) 
                        {   //Просмотр журналов
                            HandlerMenuSetup.netAdr = NetAdr;
                            HandlerMenuSetup.BaseHOF = BaseHOF;
                            HandlerMenuSetup.BaseDM = NULL;
                            SetModeMenu(MODE_HandlerMenuSetup);
                        }

                    }
                    else
                    {
                        if(DMDrive[CountDM - 1].FlgMenu.TypeMenu == TypeMenu_MJRN) 
                        {
                          //Просмотр журналов
                            HandlerMenuJrn_struct.netAdr = NetAdr;
                            memcpy(&HandlerMenuJrn_struct.DM, &DMDrive[CountDM - 1], sizeof(DescriptorMenu_type));
                            //HandlerMenuJrn_struct.DM = DMDrive[CountDM - 1]; //Базовый адрес группы
                            HandlerMenuJrn_struct.BaseHOF = BaseHOF;
                            HandlerMenuJrn_struct.BaseDM = NULL;
                            SetModeMenu(MODE_HandlerMenuJrn);
                            //return 2;
                        }
                        // Если текущее меню настройки
                        else if(DMDrive[i].FlgMenu.TypeMenu == TypeMenu_MMASTER) 
                        {   //Просмотр журналов
                            HandlerMenuSetup.netAdr = NetAdr;
                            HandlerMenuSetup.BaseHOF = BaseHOF;
                            HandlerMenuSetup.BaseDM = NULL;
                            SetModeMenu(MODE_HandlerMenuSetup);
                        }
                    }
                    HandlerMenuPultNew_struct.Mode = MODE_PRM;
                    if ((NumPrm - 1) < MAX_STR_OTOBR_PRM)
                    {
                        HandlerMenuPultNew_struct.PosBegin = 0;
                        HandlerMenuPultNew_struct.PosCursor = NumPrm - 1;
                        HandlerMenuPultNew_struct.Pos = NumPrm - 1;
                        HandlerMenuPultNew_struct.pntDPMenu = &MDPDrive.DPDrive[0];                                            //Инициализация циклического указателя на меню параметров
                        HandlerMenuPultNew_struct.pntDPReq = &MDPDrive.DPDrive[0];
                    }
                    else
                    {
                        HandlerMenuPultNew_struct.PosBegin = 0;
                        HandlerMenuPultNew_struct.PosCursor = MAX_STR_OTOBR_PRM - 1;
                        HandlerMenuPultNew_struct.Pos = NumPrm - 1;
                        HandlerMenuPultNew_struct.pntDPMenu = &MDPDrive.DPDrive[0];                                            //Инициализация циклического указателя на меню параметров
                        HandlerMenuPultNew_struct.pntDPReq = &MDPDrive.DPDrive[0];//MAX_STR_OTOBR_PRM - 1];
                    }
                    HandlerMenuPultNew_struct.CntMP = HandlerMenuPultNew_struct.CntParam;
                    return 1;
                }
            }
        }
    }
    return 0;
}

//===================================================================
// Нахождение параметра в текущем меню параметров
//===================================================================
// **************************************************************************
//  Поиск видимых п-ров и складывание указателей на  в глобальных массив п-ров
//  Входные данные:
//  - ОЗУ указатель на вычитанных HOF;
//  - адрес во флеши HOF, если это меню привода
//  - тип - 0-описатель меню пульта, 1-описатель меню привода (во флеши)
// **************************************************************************
u32 FindVisibleParamWhithPrm(void *HOF, void *DM, void *BaseHOF, u16 NumLnk, Full_DescriptorPrm_type  *DPDrive, u16 * NumPrm)
{
    DescriptorPrm_type  	DP;
    DescriptorPrmOld_type   DPOld;
    u32                 	CntParam;
    void                	*AdrParam;
    void                	*AdrFunct;
    FileFunctional_type 	Functional;
    u16                 	i, tmp;
    HeadOfFileOld_type      *pHOFOld;
    HeadOfFile_type         *pHOF;
    DescriptorMenuOld_type  *pDMOld;
    DescriptorMenu_type     *pDM;

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
            if (tmp != 0) {
                tmp = 0;
                DPGlobalOld[CntParam] = AdrParam;
                CntParam ++;
                if (!memcmp(&DPOld, &DPDrive->DPOld, sizeof(DescriptorPrmOld_type)))
                {
                    *NumPrm = CntParam;
                }
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
            if (tmp != 0) {
                tmp = 0;
                DPGlobal[CntParam] = AdrParam;
                CntParam ++;
                if (!memcmp(&DP, &DPDrive->DP, sizeof(DescriptorPrm_type)))
                {
                    *NumPrm = CntParam;
                }
            }
        }
    }      
    return CntParam;
}
/************************** (C) Copyright 2013 Triol **************************/