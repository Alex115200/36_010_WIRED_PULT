/************************** (C) Copyright 2013 Triol ***************************
* File Name          : SetupMenu.h
* Author             : Kats Eyvhen
* TS Idea            : 2013.04.08
* TS Version         : 2013.04.08
* Description        : Function for setup mode menu
*******************************************************************************/

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef SETUP_MENU_H
#define SETUP_MENU_H

#define MASK4BYTE       ((1<<floatFrmt)|(1<<int32Frmt)|(1<<HmsFrmt)|(1<<UTimeFrmt)|(1<<uns32Frmt) )

#define SIMPLE          0       // простой параметр
#define EXT             1       // сложный параметр

/* Includes ------------------------------------------------------------------*/



/* Exported define -----------------------------------------------------------*/
extern u16                              DMNumGlobalHOF[MAX_NUM_MENU];
extern DescriptorMenu_type              *DMGlobal[MAX_NUM_MENU];    
extern DescriptorPrm_type               *DPGlobal[MAX_NUM_PRM];
extern DescriptorMenu_type              DMDrive[MAX_NUM_MENU];
extern DescriptorMenuOld_type           *DMGlobalOld[MAX_NUM_MENU];             
extern DescriptorPrmOld_type            *DPGlobalOld[MAX_NUM_PRM];
extern DescriptorMenuOld_type           DMDriveOld[MAX_NUM_MENU];
extern const char                       *UprStrF[];
extern Full_DescriptorPrm_type          DPStatus;
extern Full_MenuDescriptorPrm_type      MDPDrive;
extern Full_DescriptorPrm_type          DPDriveStatus;
extern HandlerMenuJrn_struct_type       HandlerMenuJrn_struct;

// Результат поиска дескриптора
typedef enum{
    FindDescriptor_FAIL,
    FindDescriptor_OK,
}FindDescriptor_type;

typedef enum{
    ModeMenuSetup_MENU,
    ModeMenuSetup_EDIT,
}ModeMenuSetup_type;
/* Exported macro ------------------------------------------------------------*/



/* Exported types ------------------------------------------------------------*/



/* Exported constants --------------------------------------------------------*/



/* Exported variables --------------------------------------------------------*/



/* Exported function prototypes ----------------------------------------------*/



/* Exported functions --------------------------------------------------------*/
//===================================================================
//  Обработчик меню мастера настройки
//===================================================================
void HandlerMenuMasterSet(u16 Key);
//===================================================================
// Нахождение дескриптора по значению ссылочного параметра
//===================================================================
inline FindDescriptor_type FindDescriptor(Full_DescriptorPrm_type *DPDrive, Status_type *StatusRef, void *HOF, void *pHOF, u8 NetAdr);
//===================================================================
//  Формирование запроса на параметра         
//===================================================================
Drv_err_type RequestModbusOpisPrmDirect(Full_DescriptorPrm_type *DPDrive, u8 Type);
//===================================================================
// Проводим замену дескриптора для ссылочного параметра
//===================================================================
void ChangeDescriptorPrm(Full_DescriptorPrm_type *DPDrive, void *pHOF, u8 NetAdr);
//===================================================================
// Отоброжение окна текущего параметра для мастера настройки 
//===================================================================
void ParamStartOneShotReadAT45_ForSetup(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, Full_DescriptorPrm_type *DPDriveNum);
//===================================================================
// Отоброжение окна поиска по номеру параметра 
//===================================================================
u32 RefToStrEdit(Status_type *StatusRef);
//===================================================================
// Нахождение дескриптора по значению ссылочного параметра с проверкой на видимость
//===================================================================
FindDescriptor_type FindDescriptorWhithCheck(Full_DescriptorPrm_type *DPDrive, Status_type *StatusRef, void *HOF, void *pHOF, u8 NetAdr, u16 NumLnk);
//===================================================================
// Нахождение меню с искомым параметром
//===================================================================
u16 FindMenuWhithPrm(Full_DescriptorPrm_type  *DPDrive, void *BaseHOF, u16 NumMenu, TypeVisible_type TypeVisible, u8 NetAdr, u16 NumLnk);
//===================================================================
// Нахождение параметра в текущем меню параметров
//===================================================================
//===================================================================
u32 FindVisibleParamWhithPrm(void* HOF, void *DM, void *BaseHOF, u16 NumLnk, Full_DescriptorPrm_type  *DPDrive, u16 * NumPrm);

#endif // SETUP_MENU_H

/************************** (C) Copyright 2013 Triol **************************/