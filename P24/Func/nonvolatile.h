/**********************************************
 *                                            *
 *    (c) Copyright _Triol Corporation 2010    *
 *                                            *
 * File name   : nonVolatile.h                *
 * Description : Определения для              *
 *     энергонезависимой памяти               *
 *                                            *
 * Date        : 29.03.2010                   *
 * Author      :                              *
 * Modified    :                              *
 *                                            *
 **********************************************/
#ifndef     nonVolatile_H
#define     nonVolatile_H

#ifndef NUM_FILEMP_SLOTS
#define NUM_FILEMP_SLOTS 5
#endif

//==============================================================================

//==============================================================================
//Описатель AT45
//==============================================================================
typedef struct{
    u8              DescrPrm[0x80000];
}Descr_type;

// Дискриптор файла прошивки
typedef struct{
    u8              DescrLoad[0x80000];
}DescrLoad_type;

typedef struct {
    u16     Crc;
    u16     MicroSignatura;
    u16     kolvUst;
    u8      rsrvd[122];
    u16     masUstDrv[4096];
}ustDrv_type;

typedef struct{
    ustDrv_type         ustDrv;
    HeadOfFile_type     HOF;
    u16                 nomI;           //Номинальный ток
    FileFunctional_type Functional;
}SettingsDrv_type;

typedef struct{
    DescrLoad_type      FileDescrLoad;
    Descr_type          FileDescr[MAXDRIVES];
    SettingsDrv_type    FileSettingsDrv[3];
    u16                 Wait;
}spfl_type;
// **************************************************************************

#define  spfl     (*(spfl_type*)0)

// **************************************************************************

//==============================================================================
//                      Описатель FRAM
//==============================================================================
#define     FileDescrParam      MAX_DRIVE
#define     LNKFileDescrParam   MAX_DRIVE
#define     N_Files             FileDescrParam + LNKFileDescrParam

typedef struct{
    u8 NumGrp;
    u8 NumPrm;
}Status_type;

typedef struct{
    u8  tmp;
    //LnkNetFDP_type  LnkNetFDP[MAXDRIVES];
}fram_file_type;

typedef struct {
    LnkNetFDP_type      LnkNetFDP;
    Status_type         StatusRef[COUNT_STATUS];               //Адреса параметров для статусного меню
    FileFunctional_type Functional;
    u16                 Lang;
}OpisDrv_type;

#pragma pack(push)
#pragma pack(1)

typedef struct 
{
    u8 IsFullSign;
    u16 FuncMaskCrc;
    u16 FileCrc;
    void *FlashAddr;
    
    u16 LinkCrc;
} FileMPLink;

#pragma pack(pop)

#define         FLAG_NONE       0
#define         FLAG_FILE_MCU   1
#define         FLAG_FILE_AT45  2
#define         FLAG_ERASE_MCU  4
#define         FLAG_WRITE_MCU  8
#define         FLAG_WRITE_AT45 16
#define         FLAG_READY      32

// Структура загаловка
typedef struct{
    u32 VersiaPO;
    u32 Signatura;
    u32 SizeFile;
    u32 CrcFile;
}HeadOfLoadFile_type; 

typedef union
{
    u32                 ALL;
    struct{
        u32     FILE_MCU    :1;
        u32     FILE_AT45   :1;
        u32     ERASE_MCU   :1;
        u32     WRITE_MCU   :1;
        u32     WRITE_AT45  :1;
        u32     READY       :1;
        u32     REZ         :26;
    }bit;
}FlagLoader_type;

typedef enum
{
    StateLoader_READY       = FLAG_READY,
    StateLoader_FILE_MCU    = FLAG_FILE_MCU,
    StateLoader_ERASE_MCU   = FLAG_ERASE_MCU,
    StateLoader_WRITE_MCU   = FLAG_WRITE_MCU,
    StateLoader_WRITE_AT45  = FLAG_WRITE_AT45,
}StateLoader_type;

typedef struct
{
    HeadOfLoadFile_type HOLF;
    HeadOfLoadFile_type HOLFShadow;
    FlagLoader_type     FlagLoader;
}Bootloader_type;

typedef struct{
    Bootloader_type     Bootloader;
    u32                 ParamPult[SzParamPult];
    u16                 MemSignatura;
    u16                 MicroSignatura;

    fram_file_type      File;
    OpisDrv_type        OpisDrv[MAXDRIVES];
    //  LnkNetFDP_type      LnkNetFDP[MAXDRIVES];
    //  FileFunctional_type Functional[MAXDRIVES];
    u16                 Wait;
    FileMPLink          FileMPLinks[NUM_FILEMP_SLOTS];
    
    SettingsDrv_type    FileSettingsDrv[3];     // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    u16                 crc;                    //контрольная сумма	
}nv_type;
// **************************************************************************

#define  nv     (*(nv_type*)0)
// **************************************************************************

#endif  // nonVolatile_H
