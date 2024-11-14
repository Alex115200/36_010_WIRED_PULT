/************************** (C) Copyright 2013 Triol ***************************
* File Name          : LevelMenu.c
* Author             : Kats Eyvhen
* TS Idea            : 2013.03.15
* TS Version         : 2013.03.15
* Description        : Function for level mode menu
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "globalIncludes.h"
#include <stdlib.h>
#include "TskDrv.h"
#include "MicroProg.h"


/* Private define ------------------------------------------------------------*/
const char povtor[] = "Повторить установку привода?";


/* Private macro -------------------------------------------------------------*/



/* Private types -------------------------------------------------------------*/



/* Private variables ---------------------------------------------------------*/
extern HandlerMenuPultNew_struct_type   HandlerMenuPultNew_struct;
extern DescriptorMenu_type              *DMGlobal[MAX_NUM_MENU];             
extern DescriptorMenu_type              DMDrive[MAX_NUM_MENU];
extern uint8_t                          gIsNvsaOld;

/* Private function prototypes -----------------------------------------------*/



/* Private functions ---------------------------------------------------------*/

// **************************************************************************
//  Поиск видимых меню с текущими параметрами и меню групп, а также
// складывание указателей на меню в глобальных массив
//  Входные данные:
//  - ОЗУ указатель на вычитанных HOF;
//  - адрес во флеши HOF, если это меню привода
//  - тип - 0-описатель меню пульта, 1-описатель меню привода (во флеши)
//  - Номер HOF (Lnk);
//  - номер меню
//  - адрес опесателя меню в HOF
// **************************************************************************
u32 FindVisibleMenu_Direct(void *HOF, void *BaseHOF, u16 NumLnk, u16 NumMenu, u16 AdrMenu, TypeVisible_type TypeVisible, u16 * CountMenuInto)
{
    DescriptorMenu_type     DM;
    DescriptorMenuOld_type  DMOld;
    u32                     CntMenu = 0;
    u32                     CntParam;
    u16                     j;
    u16                     AdrTemp;
    u16                     CountMenuIntoTemp;
    HeadOfFile_type         *pHOF;
    HeadOfFileOld_type      *pHOFOld;

    if (gIsNvsaOld)
    {
        pHOFOld = HOF;

        if (NumMenu > pHOFOld->QuantityMenu)
        {
            GetOpisMenu(pHOFOld, &DMOld, BaseHOF, NumMenu);
        }
        else
        {
            GetOpisMenuGrp(pHOFOld, &DMOld, BaseHOF, NumMenu, AdrMenu, &AdrTemp);
            NumMenu = AdrTemp;
        }  
        if(AdrMenu) 
        {
            if (DMOld.FlgMenu.TypeMenu == TypeMenu_MGRP_GRPS)
            {  
                // Определяем номер вложеного меню
                CntParam = 0;
                CountMenuIntoTemp = 0;
                for (j = NumMenu + 1; j < (DMOld.QuantityPrmMenu + NumMenu + 1); j++) 
                {
                    CntParam += FindVisibleMenu_Direct(pHOFOld, BaseHOF, NumLnk, j + CountMenuIntoTemp, DMOld.AdrNprm, TypeVisible, &CountMenuIntoTemp);
                    (*CountMenuInto)++;
                }
                (*CountMenuInto) += CountMenuIntoTemp;
                if (CntParam) 
                {
                    CntMenu++;
                }
            }  
            else if (DMOld.FlgMenu.TypeMenu == TypeMenu_MJRN)
            {
                CntMenu++;
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
                //CntParam = FindVisibleParam_Direct(pHOFOld, &DMOld, BaseHOF, NumLnk, TypeVisible);
                if (CntParam) 
                {
                    CntMenu++;;
                }
            }
        }
    }
    else
    {
        pHOF = HOF;

        if (NumMenu > pHOF->QuantityMenu)
        {
            GetOpisMenu(pHOF, &DM, BaseHOF, NumMenu);
        }
        else
        {
            GetOpisMenuGrp(pHOF, &DM, BaseHOF, NumMenu, AdrMenu, &AdrTemp);
            NumMenu = AdrTemp;
        }  
        if(AdrMenu) 
        {
            if (DM.FlgMenu.TypeMenu == TypeMenu_MGRP_GRPS)
            {  
                // Определяем номер вложеного меню
                CntParam = 0;
                CountMenuIntoTemp = 0;
                for (j = NumMenu + 1; j < (DM.QuantityPrmMenu + NumMenu + 1); j++) 
                {
                    CntParam += FindVisibleMenu_Direct(pHOF, BaseHOF, NumLnk, j + CountMenuIntoTemp, DM.AdrNprm, TypeVisible, &CountMenuIntoTemp);
                    (*CountMenuInto)++;
                }
                (*CountMenuInto) += CountMenuIntoTemp;
                if (CntParam) 
                {
                    CntMenu++;
                }
            }  
            else if (DM.FlgMenu.TypeMenu == TypeMenu_MJRN)
            {
                CntMenu++;
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
                //CntParam = FindVisibleParam_Direct(pHOF, &DM, BaseHOF, NumLnk, TypeVisible);
                if (CntParam) 
                {
                    CntMenu++;;
                }
            }
        }
    }
    return CntMenu;
}
                    
// **************************************************************************
//  Расчет номера меню по его адресу 
//  Входные данные:
//  - ОЗУ указатель на вычитанных HOF;
//  - адрес во флеши HOF, если это меню привода
//  - тип - 0-описатель меню пульта, 1-описатель меню привода (во флеши)
//  - адрес опесателя меню в HOF
// **************************************************************************
// **************************************************************************
//  Вычитка текущего меню в группе пульта и привода 
// **************************************************************************
void *GetOpisMenuGrp(void *HOF, void *DM, void *BaseHOF, u16 NumMenu, u16 AdrMenuStart, u16 *AdrMenuHOF)
{
    void 			        *Adr;
    u16  			        OffSNprmAT45;
    DescriptorMenu_type     *pDM;
    DescriptorMenuOld_type  *pDMOld;
    HeadOfFile_type         *pHOF;
    HeadOfFileOld_type      *pHOFOld;

    if (gIsNvsaOld)
    {
        pHOFOld = HOF;
        pDMOld = DM;
        Adr = (u16*)((u32)BaseHOF + (u32)(AdrMenuStart << 1));
        
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&OffSNprmAT45, Adr, sizeof(u16));
        #else            
        memcpy(&OffSNprmAT45, (void *)((u32)Adr + STM32_FLASH_OFFSET), sizeof(u16)); //  *!* Загрузка из памяти STM по сдвигу адреса           
        #endif
        
        *AdrMenuHOF = OffSNprmAT45;
        Adr = (void *)((DescriptorMenuOld_type *)((u32)BaseHOF + (u32)(pHOFOld->MenuStart << 1)) + OffSNprmAT45);
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
        Adr = (u16*)((u32)BaseHOF + (u32)(AdrMenuStart << 1));
        #ifdef BLOCK_STM_MEMORY
        AT45RD(&OffSNprmAT45, Adr, sizeof(u16));
        #else            
        memcpy(&OffSNprmAT45, (void *)((u32)Adr + STM32_FLASH_OFFSET), sizeof(u16)); //  *!* Загрузка из памяти STM по сдвигу адреса           
        #endif
        
        *AdrMenuHOF = OffSNprmAT45;
        Adr = (void *)((DescriptorMenu_type *)((u32)BaseHOF + (u32)(pHOF->MenuStart << 1)) + OffSNprmAT45);
        
        #ifdef BLOCK_STM_MEMORY
        AT45RD(pDM, Adr, sizeof(DescriptorMenu_type));
        #else            
        memcpy(pDM, (void *)((u32)Adr + STM32_FLASH_OFFSET), sizeof(DescriptorMenu_type)); //  *!* Загрузка из памяти STM по сдвигу адреса                   
        #endif
    }
    return Adr;
}

// **************************************************************************
//  Поиск видимых п-ров и складывание указателей на  в глобальных массив п-ров
//  Входные данные:
//  - ОЗУ указатель на вычитанных HOF;
//  - адрес во флеши HOF, если это меню привода
//  - тип - 0-описатель меню пульта, 1-описатель меню привода (во флеши)
// **************************************************************************
u32 FindVisibleParam_Direct(void *HOF, void *DM, void *BaseHOF, u16 NumLnk, TypeVisible_type TypeVisible)
{
    DescriptorPrm_type  	DP;
    DescriptorPrmOld_type  	DPOld;
    u32                 	CntParam;
    void                	*AdrParam;
    void                	*AdrFunct;
    FileFunctional_type 	Functional;
    u16                 	i, tmp;
    DescriptorMenu_type     *pDM;
    DescriptorMenuOld_type  *pDMOld;
    HeadOfFile_type         *pHOF;
    HeadOfFileOld_type      *pHOFOld;

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
        for (i = 0, CntParam = 0; i < pDMOld->QuantityPrmMenu && i < MAX_NUM_MENU; i++) 
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
                        else if ((pDMOld->FlgMenu.TypeMenu == TypeMenu_MJRN) && (TypeVisible == TypeVisible_ALL))
                        {                            //Журнал
                            tmp = 1;
                        }
                }
            }
            if (tmp != 0) 
            {
                tmp = 0;
                CntParam ++;
            }
        }
    }
    else
    {
        pHOF = HOF;
        pDM = DM;
        for (i = 0, CntParam = 0; i < pDM->QuantityPrmMenu && i < MAX_NUM_MENU; i++) 
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
                        else if ((pDM->FlgMenu.TypeMenu == TypeMenu_MJRN) && (TypeVisible == TypeVisible_ALL))
                        {                            //Журнал
                            tmp = 1;
                        }
                }
            }
            if (tmp != 0) 
            {
                tmp = 0;
                CntParam ++;
            }
        }
    }
    return CntParam;
}

// **************************************************************************
//  Установка начальных значений для корневого меню
// **************************************************************************
void SetRootMenu(HandlerMenuPultNew_struct_type *MenuPult)
{
    MenuPult->AdrLevelMenu = ROOT_MENU; 
    MenuPult->AdrLevelMenuFirst = ROOT_MENU; 
    MenuPult->Level = 0;
    MenuPult->pntDPMenu = &MDPDrive.DPDrive[0];                                            //Инициализация циклического указателя на меню параметров
    MenuPult->pntDPReq = &MDPDrive.DPDrive[0];

    MenuPult->PosBeginPrev = HandlerMenuPultNew_struct.PosBegin;
    MenuPult->PosCursorPrev = HandlerMenuPultNew_struct.PosCursor;
    MenuPult->PosPrev = HandlerMenuPultNew_struct.Pos;

    MenuPult->Mode = MODE_MENU;
    MenuPult->PosBegin = 0;
    MenuPult->PosCursor = 0;
    MenuPult->Pos = 0;
    MenuPult->CntMP = HandlerMenuPultNew_struct.CntParam;
}

// **************************************************************************
//  Востоновления приведущего состояния отоброжения на экране
//  Входные данные:
//  - Количество пунктов меню;
//  - Позиция относительно начала массива;
//  - позиция относительно начала экрана.
// **************************************************************************
void GetPosPrev(u32 CntMP, u16 Pos, u16 PosCursor)
{
    u16 CountVisibleMenu;       // Количество выводимых на экран меню
    u16 StartForFirstMenu;      // Позиция в масиве меню с которого выводим значения на экран
    u16 FirstMenuForScreen;     // Позиция на экране для вывода первого меню
    u16 CountMenu;
    
    // Определяем количесво строк (названий меню) выводимых на экран
    if (CntMP < MAX_STR_OTOBR_MENU) 
        CountVisibleMenu = HandlerMenuPultNew_struct.CntMP;
    else 
        CountVisibleMenu = MAX_STR_OTOBR_MENU;
    // Если курсор больше позиции
    if (PosCursor < Pos)
    {
        StartForFirstMenu = Pos - PosCursor;        // Позиция в масиве меню с которого выводим значения на экран
    }
    else
    {
        StartForFirstMenu = PosCursor - Pos;        // Позиция в масиве меню с которого выводим значения на экран
    }
    FirstMenuForScreen = CntMP - StartForFirstMenu; // Позиция на экране для вывода первого меню
    if (gIsNvsaOld)
    {
        if (FirstMenuForScreen < MAX_STR_OTOBR_MENU)
        {
            for (CountMenu = 0; CountMenu < FirstMenuForScreen; CountMenu++)
            {
                #ifdef BLOCK_STM_MEMORY
                AT45RD(&DMDriveOld[CountMenu], DMGlobalOld[StartForFirstMenu + CountMenu], sizeof(DescriptorMenuOld_type));
                #else            
                memcpy(&DMDriveOld[CountMenu], (void *)((u32)DMGlobalOld[StartForFirstMenu + CountMenu] + STM32_FLASH_OFFSET), sizeof(DescriptorMenuOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
                #endif
            }
            for (CountMenu = FirstMenuForScreen; CountMenu < CountVisibleMenu; CountMenu++)
            {
                #ifdef BLOCK_STM_MEMORY
                AT45RD(&DMDriveOld[CountMenu], DMGlobalOld[CountMenu - FirstMenuForScreen], sizeof(DescriptorMenuOld_type));
                #else            
                memcpy(&DMDriveOld[CountMenu], (void *)((u32)DMGlobalOld[CountMenu - FirstMenuForScreen] + STM32_FLASH_OFFSET), sizeof(DescriptorMenuOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
                #endif
            }
        }
        else
        {
            for (CountMenu = 0; CountMenu < CountVisibleMenu; CountMenu++)
            {
                #ifdef BLOCK_STM_MEMORY
                AT45RD(&DMDriveOld[CountMenu], DMGlobalOld[StartForFirstMenu + CountMenu], sizeof(DescriptorMenuOld_type));
                #else            
                memcpy(&DMDriveOld[CountMenu], (void *)((u32)DMGlobalOld[StartForFirstMenu + CountMenu] + STM32_FLASH_OFFSET), sizeof(DescriptorMenuOld_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
                #endif
            }
        }
    }
    else
    {
        if (FirstMenuForScreen < MAX_STR_OTOBR_MENU)
        {
            for (CountMenu = 0; CountMenu < FirstMenuForScreen; CountMenu++)
            {
                #ifdef BLOCK_STM_MEMORY
                AT45RD(&DMDrive[CountMenu], DMGlobal[StartForFirstMenu + CountMenu], sizeof(DescriptorMenu_type));
                #else            
                memcpy(&DMDrive[CountMenu], (void *)((u32)DMGlobal[StartForFirstMenu + CountMenu] + STM32_FLASH_OFFSET), sizeof(DescriptorMenu_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
                #endif
            }
            for (CountMenu = FirstMenuForScreen; CountMenu < CountVisibleMenu; CountMenu++)
            {
                #ifdef BLOCK_STM_MEMORY
                AT45RD(&DMDrive[CountMenu], DMGlobal[CountMenu - FirstMenuForScreen], sizeof(DescriptorMenu_type));
                #else            
                memcpy(&DMDrive[CountMenu], (void *)((u32)DMGlobal[CountMenu - FirstMenuForScreen] + STM32_FLASH_OFFSET), sizeof(DescriptorMenu_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
                #endif
            }
        }
        else
        {
            for (CountMenu = 0; CountMenu < CountVisibleMenu; CountMenu++)
            {
                #ifdef BLOCK_STM_MEMORY
                AT45RD(&DMDrive[CountMenu], DMGlobal[StartForFirstMenu + CountMenu], sizeof(DescriptorMenu_type));
                #else            
                memcpy(&DMDrive[CountMenu], (void *)((u32)DMGlobal[StartForFirstMenu + CountMenu] + STM32_FLASH_OFFSET), sizeof(DescriptorMenu_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
                #endif
            }
        }
    }
}
// Вывод анимации
void Anime(void)
{
    u8 i;

    for (i = 0; i < NUM_SCREENS; i++)
    {
        memcpy(VideoRAM, Screens[i], sizeof(VideoRAM));  // Прописываем картинку в буфер
        GrfOut();
        OSTimeDly(80);              // Покажем заставку 2 сек
    }
    OSTimeDly(450);              // Покажем заставку 2 сек
    for (i = NUM_SCREENS; i > 0; i--)
    {
        memcpy(VideoRAM, Screens[i - 1], sizeof(VideoRAM));  // Прописываем картинку в буфер
        GrfOut();
        OSTimeDly(80);              // Покажем заставку 2 сек
    }
    GrafBffClear();
    GrfOut();
}

//************************************
// Установка описателя меню привода
//************************************
void SetHandlerMenuDrive(HandlerMenuChangeDrive_struct_type * HandlerMenuDrv)
{
    u32             i;

    for (i = 0; i < MAXDRIVES_CUT; i++) 
    {
        if (ReadLnk(&HandlerMenuDrv->LnkNetFDP, i)) 
        {
            HandlerMenuDrv->AdrFRAM[i] = HandlerMenuDrv->LnkNetFDP.Lnk;
            HandlerMenuDrv->NetAdr[i] = HandlerMenuDrv->LnkNetFDP.NetAdr;
            HandlerMenuDrv->MaxDrv = i + 1;
        }
        else if (i == 0)
        {
            if (MessageYesNo(povtor))
            {
                AllPrmPnt->ADD_DRIVE = 1;
                InstallDrv();
            }
            SetModeMenu(MODE_HandlerMenuPultNewWithDrive);
            return;
        }
        else break;
    }
}

//====================================================================
//  Игра змейка
//====================================================================
void Snake(void)
{
    keyName_type        Key;
    u8                  err;
    
    //=======================
    SnakePos_type       LenSnake[MAX_POS]; 
    SnakePos_type       Apple;
    u16                 CountPos;
    u16                 CountItr;
    Cursor_type         CursorDirect = Cursor_LEFT;
    u8                  countTime = 0;
    SnakePos_type       TempPos;
    u16                 Point = 4; // балы
    u8                  first = 1;
    u8                  flagPress;
    
    // Инициализация
    for (CountPos = 0; CountPos < MAX_POS; CountPos++)
    {
        LenSnake[CountPos].SnakePos_HOR = 0xFF;
        LenSnake[CountPos].SnakePos_VER = 0xFF;
    }
    LenSnake[0].SnakePos_HOR = 8;
    LenSnake[0].SnakePos_VER = 10;
    LenSnake[1].SnakePos_HOR = 9;
    LenSnake[1].SnakePos_VER = 10;
    LenSnake[2].SnakePos_HOR = 10;
    LenSnake[2].SnakePos_VER = 10;
    LenSnake[3].SnakePos_HOR = 11;
    LenSnake[3].SnakePos_VER = 10;
    
    Apple.SnakePos_HOR = 5;
    Apple.SnakePos_VER = 8;

    while(1)
    {
        OSSemPend(SemMenu, SYS_FOREVER, &err );
        Key = WaitKey(NULL);

        switch (Key) 
        {
            case KEY_UP:
                switch (CursorDirect){
                case Cursor_RIGHT:
                    if (flagPress)
                    {
                        CursorDirect = Cursor_UP;
                        flagPress = 0;
                    }
                    break;
                case Cursor_LEFT:
                    if (flagPress)
                    {
                        CursorDirect = Cursor_UP;
                        flagPress = 0;
                    }
                    break;
                case Cursor_UP:
                    CursorDirect = Cursor_UP;
                    break;
                case Cursor_DOWN:
                    break;
                default:
                    break;
                }
                break;
            case KEY_DOWN:
                switch (CursorDirect){
                case Cursor_RIGHT:
                    if (flagPress)
                    {
                        CursorDirect = Cursor_DOWN;
                        flagPress = 0;
                    }
                    break;
                case Cursor_LEFT:
                    if (flagPress)
                    {
                        CursorDirect = Cursor_DOWN;
                        flagPress = 0;
                    }
                    break;
                case Cursor_UP:
                    break;
                case Cursor_DOWN:
                    CursorDirect = Cursor_DOWN;
                    break;
                default:
                    break;
                }
                break;
            case KEY_LEFT:
                switch (CursorDirect){
                case Cursor_RIGHT:
                    break;
                case Cursor_LEFT:
                    CursorDirect = Cursor_LEFT;
                    break;
                case Cursor_UP:
                    if (flagPress)
                    {
                        CursorDirect = Cursor_LEFT;
                        flagPress = 0;
                    }
                    break;
                case Cursor_DOWN:
                    if (flagPress)
                    {
                        CursorDirect = Cursor_LEFT;
                        flagPress = 0;
                    }
                    break;
                default:
                    break;
                }
                break;
            case KEY_RIGHT:
                switch (CursorDirect){
                case Cursor_RIGHT:
                    CursorDirect = Cursor_RIGHT;
                    break;
                case Cursor_LEFT:
                    break;
                case Cursor_UP:
                    if (flagPress)
                    {
                        CursorDirect = Cursor_RIGHT;
                        flagPress = 0;
                    }
                    break;
                case Cursor_DOWN:
                    if (flagPress)
                    {
                        CursorDirect = Cursor_RIGHT;
                        flagPress = 0;
                    }
                    break;
                default:
                    break;
                }
                break;
            case KEY_ENTER:
                if (first)
                {
                    first = 0;
                }
                break;
            case KEY_ESC:
                if (!first)
                {
                    sprintf(&StrOut[0],"%s %d %s", "Вы набрали:    \n", Point*10 - 40, " Очков");
                    MessageCancel( StrOut );
                }
                return;
        default:
          break;
        }
        if (first)
        {
            return;
        }
        if (countTime < COUNT_ITR)
        {
            countTime++;   
        }
        else
        {
            countTime = 0;
            flagPress = 1;
            switch (CursorDirect)
            {
            case Cursor_RIGHT:
                if (LenSnake[0].SnakePos_HOR < MAX_HOR_POS - 1)
                {
                    TempPos.SnakePos_HOR = LenSnake[0].SnakePos_HOR + 1;  
                }
                else
                {
                    TempPos.SnakePos_HOR = MIN_HOR_POS;
                }
                TempPos.SnakePos_VER = LenSnake[0].SnakePos_VER;
                break;
            case Cursor_LEFT:
                if (LenSnake[0].SnakePos_HOR > MIN_HOR_POS)
                {
                    TempPos.SnakePos_HOR = LenSnake[0].SnakePos_HOR - 1;  
                }
                else
                {
                    TempPos.SnakePos_HOR = MAX_HOR_POS - 1;
                }
                TempPos.SnakePos_VER = LenSnake[0].SnakePos_VER;
                break;
            case Cursor_UP:
                TempPos.SnakePos_HOR = LenSnake[0].SnakePos_HOR;
                if (LenSnake[0].SnakePos_VER > MIN_VER_POS)
                {
                    TempPos.SnakePos_VER = LenSnake[0].SnakePos_VER - 1;  
                }
                else
                {
                    TempPos.SnakePos_VER = MAX_VER_POS - 1;
                }
                break;
            case Cursor_DOWN:
                TempPos.SnakePos_HOR = LenSnake[0].SnakePos_HOR;
                if (LenSnake[0].SnakePos_VER < MAX_VER_POS - 1)
                {
                    TempPos.SnakePos_VER = LenSnake[0].SnakePos_VER + 1;  
                }
                else
                {
                    TempPos.SnakePos_VER = MIN_VER_POS;
                }
                break;
            default:
                break;
            }
            if ((TempPos.SnakePos_VER == Apple.SnakePos_VER) && (TempPos.SnakePos_HOR == Apple.SnakePos_HOR))
            {
                for (CountPos = Point; CountPos > 0; CountPos--)
                {
                    LenSnake[CountPos].SnakePos_HOR = LenSnake[CountPos - 1].SnakePos_HOR;
                    LenSnake[CountPos].SnakePos_VER = LenSnake[CountPos - 1].SnakePos_VER;
                    if ((TempPos.SnakePos_VER == LenSnake[CountPos].SnakePos_VER) && (TempPos.SnakePos_HOR == LenSnake[CountPos].SnakePos_HOR))
                    {
                        sprintf(&StrOut[0],"%s %d %s", "Вы набрали:    \n", Point*10 - 40, " Очков");
                        MessageCancel( StrOut );
                        return;
                    }
                }
                LenSnake[0].SnakePos_HOR = TempPos.SnakePos_HOR;
                LenSnake[0].SnakePos_VER = TempPos.SnakePos_VER;
                do
                {
                    CountItr = 0;
                    Apple.SnakePos_VER = rand()%MAX_VER_POS;
                    Apple.SnakePos_HOR = rand()%MAX_HOR_POS;
                    for (CountPos = 0; CountPos < Point; CountPos++)
                    {
                        if ((Apple.SnakePos_VER == LenSnake[CountPos].SnakePos_VER) && (Apple.SnakePos_HOR == LenSnake[CountPos].SnakePos_HOR))
                        {
                            CountItr = 1;
                        }
                    }
                }
                while(CountItr == 1);
                Point++;
            }     
            else
            {
                for (CountPos = Point - 1; CountPos > 0; CountPos--)
                {
                    LenSnake[CountPos].SnakePos_HOR = LenSnake[CountPos - 1].SnakePos_HOR;
                    LenSnake[CountPos].SnakePos_VER = LenSnake[CountPos - 1].SnakePos_VER;
                    if ((TempPos.SnakePos_VER == LenSnake[CountPos].SnakePos_VER) && (TempPos.SnakePos_HOR == LenSnake[CountPos].SnakePos_HOR))
                    {
                        sprintf(&StrOut[0],"%s %d %s", "Вы набрали:    \n", Point*10 - 40, " Очков");
                        MessageCancel( StrOut );
                        return;
                    }
                }
                LenSnake[0].SnakePos_HOR = TempPos.SnakePos_HOR;
                LenSnake[0].SnakePos_VER = TempPos.SnakePos_VER;
            }
        }
        GrafBffClear();
        //====================================================================
        //  Рисуем змейку и "яблоко"
        //====================================================================
        DrawSnake(&LenSnake[0]);
        if ((Apple.SnakePos_VER < MAX_VER_POS) && (Apple.SnakePos_HOR < MAX_HOR_POS) &&
            (countTime & 2))
        {
            DrawAppleTrak(Apple);
        }
        //==========================================================
        //Вывод на экран
        //==========================================================
        GrfOut();
    }// While(1)
}

//====================================================================
//  Рисуем змейку и "яблоко"
//====================================================================
void DrawSnake(SnakePos_type * LenSnake)
{
    u16     CountPos;

    for (CountPos = 0;; CountPos++)
    {
         if ((LenSnake[CountPos].SnakePos_VER < MAX_VER_POS) && (LenSnake[CountPos].SnakePos_HOR < MAX_HOR_POS))
         {
             DrawSnakeTrak(LenSnake[CountPos]);
         }
         else
         {
             break;
         }
    }
}

//====================================================================
//  Рисуем звено змейки
//====================================================================
void DrawSnakeTrak(SnakePos_type SnakeTrak)
{
    u8      Trak[8] = {0xFF, 0x81, 0xBD, 0xBD, 0xBD, 0xBD, 0x81, 0xFF};
    u8      *cPnt;
    u16     CountPos;

    cPnt = &GrafBff[SnakeTrak.SnakePos_HOR + SnakeTrak.SnakePos_VER * YPIXELS];
    for (CountPos = 0; CountPos < 8; CountPos++)
    {
        cPnt[CountPos * (YPIXELS / 8)] = Trak[CountPos];
    }
}

//====================================================================
//  Рисуем "Яблоко"
//====================================================================
void DrawAppleTrak(SnakePos_type AppleTrak)
{
    //u8      Trak[8] = {0x81, 0x42, 0x24, 0x3C, 0x3C, 0x24, 0x42, 0x81};
    u8      Trak[8] = {0xFF, 0x81, 0xBD, 0xBD, 0xBD, 0xBD, 0x81, 0xFF};
    u8      *cPnt;
    u16     CountPos;

    cPnt = &GrafBff[AppleTrak.SnakePos_HOR + AppleTrak.SnakePos_VER * YPIXELS];
    for (CountPos = 0; CountPos < 8; CountPos++)
    {
        cPnt[CountPos * (YPIXELS / 8)] = Trak[CountPos];
    }
}

//====================================================================
//  Тест качества связи с NVSA
//====================================================================
#define COUNT_REQ       2
#define COUNT_SIZE      5000
//=============================

void Test_with_NVSA(void){
  
    keyName_type        Key;
    u8                  err;
    char                StrOut[100];

    u32                 Sz, sz, SzALL;
    u16                 procInst;
    void                *CurAdrAT45;
    void                *AdrInFile;
    u16                 i;
    u8                  netAdr;
    Drv_err_type        code;
    MsgReqKbdIn_type    MsgReqKbd;
    s32 numErrs = 0;
    u32                 NumberErr = 0;
    u32                 NumberOk = 0;
    u32                 NumberWrErr = 0;
    u32                 NumberWrOk = 0;
    u32                 NumberRdErr = 0;
    u32                 NumberRdOk = 0;
    char                Wr[] = "Запись"; 
    char                Rd[] = "Чтение";

    //procNet = 0.f;
    //procInst = 0;
    netAdr = 1;
    MsgReqKbd.KbdStateStartStop = KbdStartStop_OFF;
    OSQPost(QueueDrvInStartStop, &MsgReqKbd); 
    
    OSSemPend(SemMenu, SYS_FOREVER, &err );
    Key = WaitKey(NULL);
    
    if(Key == KEY_LEFT){
      
      sprintf(&StrOut[0], "  To start");
      StrOutFont(0, 0, NO_INV, &StrOut[0], &fontNormal, centrAlign); 
      sprintf(&StrOut[0], "  connection test");
      StrOutFont(0, 16, NO_INV, &StrOut[0], &fontNormal, leftAlign); 
      sprintf(&StrOut[0], " press ENTER");
      StrOutFont(0, 32, NO_INV, &StrOut[0], &fontNormal, leftAlign); 
      
      sprintf(&StrOut[0], "   To exit");
      StrOutFont(0, 48, NO_INV, &StrOut[0], &fontNormal, leftAlign );
      sprintf(&StrOut[0], " press CANCEL");
      StrOutFont(0, 64, NO_INV, &StrOut[0], &fontNormal, leftAlign );
      
      GrfOut();
      
      while(Key != KEY_ESC){
        
        Key = WaitKey(NULL);
        
        if(Key == KEY_ENTER){
          GrafBffClear();
          GrfOut();
             
//****************************************************************************//
//****************** Собственно, сам тест связи ******************************//
//****************************************************************************//
          while(1){
            AdrInFile = 0;     //Адрес внутри файла для привода
            CurAdrAT45 = 0;
            SzALL = Sz = COUNT_SIZE;
            do
            {
                     GrafBffClear();
                if (procInst != 100 - (Sz * 100 / SzALL))
                {
                    procInst = 100 - (Sz * 100 / SzALL);
                    //InstallDrvProgress((u16)procNet, procInst, cntFind, cntInst, netAdr, 1);
                }
                sprintf(&StrOut[0], "%s 0xAA:%d%%", Wr, (u16)procInst);
                StrOutFont(0, 0, NO_INV, &StrOut[0], &fontNormal, leftAlign);
                sprintf(&StrOut[0], "Rd all:%d",NumberRdErr + NumberRdOk);
                StrOutFont(0, 32, NO_INV, &StrOut[0], &fontNormal, leftAlign );
                sprintf(&StrOut[0], "Rd Err:%d", NumberRdErr);
                StrOutFont(0, 32 + INCY*1, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //Вывод внизу номера группы и параметра
                sprintf(&StrOut[0], "Wr all:%d",NumberWrErr + NumberWrOk);
                StrOutFont(0, 32 + INCY*3, NO_INV, &StrOut[0], &fontNormal, leftAlign );
                sprintf(&StrOut[0], "Wr Err:%d", NumberWrErr);
                StrOutFont(0, 32 + INCY*4, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //Вывод внизу номера группы и параметра
                sprintf(&StrOut[0], "Err:%d", NumberErr);
                StrOutFont(0, 32 + INCY*6, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //Вывод внизу номера группы и параметра
                //==========================================================
                // Вывод на экран
                //==========================================================
                GrfOut();
                        if (Sz > COUNT_REQ)
                        {      //в словах
                            sz = COUNT_REQ;
                            Sz -= COUNT_REQ;
                        }
                        else
                        {
                            sz = Sz;
                            Sz = 0;
                        }
                        for (i = 0; i < COUNT_REQ<<1; i++)
                        {
                            DataBuf[i] = 0xAA;
                        }
                        SzALL += sz;
                        code = WriteFileToDrv(&MsgReqDrvIn[MSG_ID], netAdr, AdrInFile, sz<<1, fileLoad, &DataBuf[0], sizeof(DataBuf));
                        if (code == Ok_err)
                        {
                            NumberWrOk += COUNT_REQ << 1;
                            AdrInFile = (void *)((u32)AdrInFile + sz);
                            CurAdrAT45 = (void *)((u32)CurAdrAT45 + sz * 2);
                            numErrs = 0;    
                        }
                        else
                        {
                            NumberWrErr++;
                            numErrs++;
                            if(numErrs < 100)
                            {
                                code = Ok_err;
                                Sz += COUNT_REQ;
                                numErrs++;
                            }
                            else
                            {
                                break;
                            }
                        }
                        OSTimeDly(40);
            }while (Sz != 0);
            
            AdrInFile = 0;     //Адрес внутри файла для привода
            CurAdrAT45 = 0;
            SzALL = Sz = COUNT_SIZE;
            if (!numErrs)
            do
            {
                   GrafBffClear();
                if (procInst != 100 - (Sz * 100 / SzALL))
                {
                    procInst = 100 - (Sz * 100 / SzALL);
                    //InstallDrvProgress((u16)procNet, procInst, cntFind, cntInst, netAdr, 1);
                }
                sprintf(&StrOut[0], "%s 0xAA:%d%%", Rd, (u16)procInst);
                StrOutFont(0, 0, NO_INV, &StrOut[0], &fontNormal, leftAlign);
                sprintf(&StrOut[0], "Rd all:%d",NumberRdErr + NumberRdOk);
                StrOutFont(0, 32, NO_INV, &StrOut[0], &fontNormal, leftAlign );
                sprintf(&StrOut[0], "Rd Err:%d", NumberRdErr);
                StrOutFont(0, 32 + INCY*1, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //Вывод внизу номера группы и параметра
                sprintf(&StrOut[0], "Wr all:%d",NumberWrErr + NumberWrOk);
                StrOutFont(0, 32 + INCY*3, NO_INV, &StrOut[0], &fontNormal, leftAlign );
                sprintf(&StrOut[0], "Wr Err:%d", NumberWrErr);
                StrOutFont(0, 32 + INCY*4, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //Вывод внизу номера группы и параметра
                sprintf(&StrOut[0], "Err:%d", NumberErr);
                StrOutFont(0, 32 + INCY*6, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //Вывод внизу номера группы и параметра
                //==========================================================
                // Вывод на экран
                //==========================================================
                GrfOut();
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
                        code = ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], netAdr, AdrInFile, sz<<1, fileLoad, &DataBuf[0], sizeof(DataBuf));
                        if (code == Ok_err)
                        {
                            NumberOk++;
                            AdrInFile = (void *)((u32)AdrInFile + sz);
                            CurAdrAT45 = (void *)((u32)CurAdrAT45 + sz * 2);
                            numErrs = 0;
                            for (i = 0; i < sz<<1; i++)
                            {
                                 if (DataBuf[i] != 0xAA) 
                                 {
                                    numErrs++;
                                 }
                            }
                            if (numErrs)
                            {
                                NumberRdErr += numErrs;
                                NumberRdOk += (sz << 1) - numErrs;
                            }
                            else
                            {
                                NumberRdOk += sz<<1;
                            }
                            numErrs = 0;
                        }
                        else
                        {
                            NumberErr++;
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
            
            AdrInFile = 0;     //Адрес внутри файла для привода
            CurAdrAT45 = 0;
            SzALL = Sz = COUNT_SIZE;
            do
            {
                   GrafBffClear();
                if (procInst != 100 - (Sz * 100 / SzALL))
                {
                    procInst = 100 - (Sz * 100 / SzALL);
                    //InstallDrvProgress((u16)procNet, procInst, cntFind, cntInst, netAdr, 1);
                }
                sprintf(&StrOut[0], "%s 0x55:%d%%", Wr, (u16)procInst);
                StrOutFont(0, 0, NO_INV, &StrOut[0], &fontNormal, leftAlign);
                sprintf(&StrOut[0], "Rd all:%d",NumberRdErr + NumberRdOk);
                StrOutFont(0, 32, NO_INV, &StrOut[0], &fontNormal, leftAlign );
                sprintf(&StrOut[0], "Rd Err:%d", NumberRdErr);
                StrOutFont(0, 32 + INCY*1, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //Вывод внизу номера группы и параметра
                sprintf(&StrOut[0], "Wr all:%d",NumberWrErr + NumberWrOk);
                StrOutFont(0, 32 + INCY*3, NO_INV, &StrOut[0], &fontNormal, leftAlign );
                sprintf(&StrOut[0], "Wr Err:%d", NumberWrErr);
                StrOutFont(0, 32 + INCY*4, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //Вывод внизу номера группы и параметра
                sprintf(&StrOut[0], "Err:%d", NumberErr);
                StrOutFont(0, 32 + INCY*6, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //Вывод внизу номера группы и параметра
                //==========================================================
                // Вывод на экран
                //==========================================================
                GrfOut();
                        if (Sz > COUNT_REQ)
                        {      //в словах
                            sz = COUNT_REQ;
                            Sz -= COUNT_REQ;
                        }
                        else
                        {
                            sz = Sz;
                            Sz = 0;
                        }
                        SzALL += sz;
                        for (i = 0; i < COUNT_REQ<<1; i++)
                        {
                            DataBuf[i] = 0x55;
                        }
                        code = WriteFileToDrv(&MsgReqDrvIn[MSG_ID], netAdr, AdrInFile, sz<<1, fileLoad, &DataBuf[0], sizeof(DataBuf));
                        if (code == Ok_err)
                        {
                            NumberWrOk += COUNT_REQ << 1;
                            AdrInFile = (void *)((u32)AdrInFile + sz);
                            CurAdrAT45 = (void *)((u32)CurAdrAT45 + sz * 2);
                            numErrs = 0;
                        }
                        else
                        {
                            NumberWrErr++;
                            numErrs++;
                            if(numErrs < 100)
                            {
                                code = Ok_err;
                                Sz += COUNT_REQ;
                                numErrs++;
                            }
                            else
                            {
                                break;
                            }
                        }
                        OSTimeDly(40);
            }while (Sz != 0);
            
            AdrInFile = 0;     //Адрес внутри файла для привода
            CurAdrAT45 = 0;
            SzALL = Sz = COUNT_SIZE;
            if (!numErrs)
            do
            {
                   GrafBffClear();
                if (procInst != 100 - (Sz * 100 / SzALL))
                {
                    procInst = 100 - (Sz * 100 / SzALL);
                    //InstallDrvProgress((u16)procNet, procInst, cntFind, cntInst, netAdr, 1);
                }
                sprintf(&StrOut[0], "%s 0x55:%d%%", Rd, (u16)procInst);
                StrOutFont(0, 0, NO_INV, &StrOut[0], &fontNormal, leftAlign);
                sprintf(&StrOut[0], "Rd all:%d",NumberRdErr + NumberRdOk);
                StrOutFont(0, 32, NO_INV, &StrOut[0], &fontNormal, leftAlign );
                sprintf(&StrOut[0], "Rd Err:%d", NumberRdErr);
                StrOutFont(0, 32 + INCY*1, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //Вывод внизу номера группы и параметра
                sprintf(&StrOut[0], "Wr all:%d",NumberWrErr + NumberWrOk);
                StrOutFont(0, 32 + INCY*3, NO_INV, &StrOut[0], &fontNormal, leftAlign );
                sprintf(&StrOut[0], "Wr Err:%d", NumberWrErr);
                StrOutFont(0, 32 + INCY*4, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //Вывод внизу номера группы и параметра
                sprintf(&StrOut[0], "Err:%d", NumberErr);
                StrOutFont(0, 32 + INCY*6, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //Вывод внизу номера группы и параметра
                //==========================================================
                // Вывод на экран
                //==========================================================
                GrfOut();
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
                        code = ReadFileFromDrv(&MsgReqDrvIn[MSG_ID], netAdr, AdrInFile, sz<<1, fileLoad, &DataBuf[0], sizeof(DataBuf));
                        if (code == Ok_err)
                        {
                            NumberOk++;
                            AdrInFile = (void *)((u32)AdrInFile + sz);
                            CurAdrAT45 = (void *)((u32)CurAdrAT45 + sz * 2);
                            numErrs = 0;
                            for (i = 0; i < sz<<1; i++)
                            {
                                 if (DataBuf[i] != 0x55) 
                                 {
                                    numErrs++;
                                 }
                            }
                            if (numErrs)
                            {
                                NumberRdErr += numErrs;
                                NumberRdOk += (sz<<1) - numErrs;
                            }
                            else
                            {
                                NumberRdOk += sz<<1;
                            }
                            numErrs = 0;
                        }
                        else
                        {
                            NumberErr++;
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
        }
//****************************************************************************//
//****************** Собственно, конец теста связи ***************************//
//****************************************************************************//
        /*
            while(1){
              sprintf(&StrOut[0], "  Тест окончен  ");
              StrOutFont(0, 32 + INCY*7, NO_INV, &StrOut[0], &fontNormal, leftAlign);           
              GrfOut();
            }
        */
        }    
      }
    }
}


/************************** (C) Copyright 2013 Triol **************************/