/**********************************************
 *                                            *
 *    (c) Copyright Triol Corporation 2010    *
 *                                            *
 * File name   : OS.h                         *
 * Description : Определения и структуры      *
 *                                            *
 * Date        : 29.03.2010                   *
 * Author      : Shatyorny A.V.               *
 * Modified    : Apostolov S.V.               *
 *                                            *
 **********************************************/
#ifndef  OS_H
#define OS_H

#include "Menu.h"

//#include "Calc.h"
//#include "Hard.h"
//#include "nonVolatile.h"
//#include "Text.h"    //объявление структур
//#include "DGP.h"
//#include "Menu.h"
//#include "File.h"

#ifndef NULL
#define  NULL  ((void *)0)
#endif

// ************************************************************************************************************/
/*
typedef struct
{ unsigned int cpsr_save      ;
  unsigned int restart_adr    ;
  unsigned int save_r0_r14[15];
  unsigned int cmp_stop       ;
  unsigned int IntClearMask   ;
  unsigned char flag          ;
  unsigned char Npcb          ;
  unsigned char NeventWait    ;
  unsigned char al_os         ; } PCB_type;
  */
// ************************************************************************************************************/
/*
typedef struct{
    unsigned int maskWaitProc;
    unsigned int MaskProcRcb ;
}RCB_type;
*/
// ************************************************************************************************************/
/*
typedef struct
{ PCB_type      pcb[16]  ;      //process  control bloc
  unsigned char ecb[32]  ;      //event   control bloc
  RCB_type      rcb[32]  ;      //resourse  control bloc
  unsigned int G_mask    ;        //mask ready proc
  unsigned int AdrPcbTek ;      //
  unsigned int NprocTek  ;      //
  unsigned int BitProcTek;
  unsigned int SysTimer  ;
  unsigned int ErrorMask ;
  unsigned int FlagProc  ;      //Flagi megdu processami
 } osmem_type;
 */
// **************************************************************************
/*
typedef struct
{ unsigned short marker;
  unsigned short Var   ;
  id_type IdMnu        ; } marker_type;
*/

// **************************************************************************
typedef struct{ 
    union{ 
        unsigned char   tmpSc[4]  ;
        unsigned int    tmpScanCode; 
    } un;
    unsigned int    scanCode;
    unsigned int    tecKey;
    unsigned char   m_10mS;
    unsigned char   m_100mS;
    unsigned char   BegFl;
    unsigned char   Fase; 
} opk_type;
// **************************************************************************
/*
typedef struct{
  unsigned int Crc        ; // контрольная сумма файла
  unsigned int Sign       ; // сигнатура
  unsigned short DescrSize;
  unsigned short GrpStart ;
  unsigned short PrmStart ;
  unsigned short KolvGrp  ;
  char NetAdr             ;
  char res[3]             ; 
} HeaderOpDrv_type;
*/
// **************************************************************************
//заведу параметр с такими битами
#define  promtInstBit  0
#define  inversGrafBit 1
#define  titPrmVarBit  2    //Что отображать в верхней строке. Группу или статус

// **************************************************************************

typedef int (*pFuncWrPrg_typedef)(void);
typedef int (*pFuncPskPrg_typedef)(id_type *,mp_type *);
#define  NULLPsk   (pFuncPskPrg_typedef )0
#define  KolvUtoch 16

typedef struct
{ const pFuncKey_typedef *keySel; // указатель на таблицу обработчиков клавиатуры
  pFuncLcd_typedef lcdSel;        // указатель на рисовалку экрана
  pFuncLcd_typedef lcdTitSel;     // указатель на рисовалку титула в режиме меню
  const pFuncKey_typedef *keySelSave;//сохранение keySel при редактировании
  pFuncLcd_typedef lcdSelSave;    // сохранение lcdSel при редактировании
  const pFuncKey_typedef *BigKeySelSave;//сохранение keySel в однострочном режиме
  pFuncLcd_typedef BigLcdSelSave; // сохранение lcdSel в однострочном режиме
  const pFuncKey_typedef *HelpKeySelSave;//сохранение keySel в Help
  pFuncLcd_typedef HelpLcdSelSave;// сохранение lcdSel в Help
  pFuncLcd_typedef lcdTitSelSave; // сохранение рисовалки титула в режиме меню
  pFuncWrPrg_typedef WrProg;      // указатель на функцию записи при программировании
  id_type pntIdSave;              // при редактировании текста сохраняем
  //const Lang_type *Lang;          // указатель на тексты данного языка
  marker_type *pntMark;           // указатель на текущую маркер структуру
  unsigned int FlagKey;           // флаги
  unsigned int InstalMsk;         // маска проинсталлированных приводов
  mp_type *pntMp;                 // указатель на маркированную mp структуру pntMp = mp[utochn]
  id_type *pntId;                 // указатель на маркированный Id
  int AdcKontr;
  int AdcTermo;
  int integrKontr;
  unsigned char KolvMenu ;  // количество пунктов меню
  unsigned char beg_delay;  // счетчик задержки бега по удержанию кнопки
  unsigned char utochn   ;  // текущий уровень меню
  unsigned char chKey    ;
//---
  char utochnAdrSave    ;   // сохранение utochn при редактировании адреса
  char posRdk           ;   // позиция в редактируемой строке
  char NetAdrRdGal      ;   // адрес привода с прочитанными галками
  unsigned char NStatLed;
//---
  unsigned char m_znm           ; // номер мигающего знакоместа
  unsigned char kolMigZnm       ; // количество мигающих знакомест
  unsigned char positZnm        ; // позиция в редактируемой переменной
  char maxPosit                 ; // максимальное значение позиции
  unsigned short StatWordMark   ;
  unsigned short OfsTxtStat     ;
  char TxtValStat[NCHARS]       ;
  char CurrentTitle[NCHARS]     ;
  char ZerTxtStat               ;
  char NetAdrMark               ;
  unsigned short Kontrast       ;    // контрастность экрана пульта
  unsigned short StatWord_status;
  unsigned short curHot         ;    // адрес текущего горячего параметра
  unsigned short netDrives      ;    // число приводов в сети
  //unsigned char netDrives       ;    // число приводов в сети
  unsigned char nHots           ;    // размер списка "горячие параметры"
  unsigned char nLasts          ;    // размер списка "последние коррекции"
  unsigned short dostupPu       ;    // код доступа
  unsigned short LangPrm        ;    // код языка общения
  unsigned short ReInstal       ;    // флаг переинсталляции привода
  unsigned short ReRdVal        ;    // флаг вычитки заводских уставок
  unsigned short ReSetNet       ;    // флаг настройки сети
  unsigned short AddDrive       ;    // флаг добавления привода
  unsigned short SintReRd       ;    // флаг вычитки синтетических параметров
  unsigned short ModbSped       ;    // скорость Modbus (дист. пульт)
  unsigned short RepitModbus    ;    // число повторов Modbus (дист. пульт)
  unsigned short MestnDis       ;    // 0 - местный пульт, 1 - дистанционный
  unsigned short VersPo         ;    // версия ПО пульта
  unsigned short CrcPo          ;    // контрольная сумма ПО пульта
  unsigned short FictPasw       ;    // признак "пароль забыт"
  short MaxLasts                ;    // макс. размер массива последних коррекций
  //short ReHide                  ;
  //short SetHide                 ;
  short TimeMess                ;    // длительность краткого сообщения
  short kontr                   ;    // отфильтр. код ацп контрастности
  short termo                   ;    // температура *10  (20.0°С)
  marker_type marker[KolvUtoch] ;
  id_type id[100]               ; } mk_type;
// ************************************************************************************************************/
typedef struct
{ unsigned char second ;
  unsigned char minute ;
  unsigned char hour   ;
  unsigned char day    ;
  unsigned char date   ;
  unsigned char month  ;
  unsigned char year   ;
  unsigned char al_time;
  unsigned int Time_ch ; } memTime_type;
// **************************************************************************
typedef struct
{ unsigned int NetPntr_iap  ;
  unsigned short CrcOpis_iap;
  unsigned short SzOpis_iap ;
  unsigned short PrgVar_iap ;
  unsigned short DevType_iap;
  unsigned int NetAdr_iap   ; } memInstal_type;
// **************************************************************************
typedef struct
{ unsigned char NameMode_mo[27];
  unsigned char Flagi_mo       ;
  unsigned int PuskSr_mo       ; } memModOpis_type;
// **************************************************************************
typedef struct
{ char mgrp[32]                ;             // маска групп
  char mprm[256]               ;             // маска параметров
  char align[FLASHBYTES-32-256]; } MaskPrm_type;
// ****************************************************************************
typedef struct
{ char mgrp[32] ;             // маска групп
  char mprm[256];             // маска параметров
  } mskEn_type;
// ****************************************************************************
/*#define  SzOpDrvSpfl    (FLASHBYTES*125)    //размер одного описателя в spfl
typedef struct
{ union
  { struct
    { MaskPrm_type MaskPrm[32]; };
    char reserv[SzOpDrvSpfl]; };
  char opDrvSpfl[31][SzOpDrvSpfl]; } spfl_type;*/

// **************************************************************************
// #define spfl (*(spfl_type*)0)
// **************************************************************************
typedef void (*pFuncSpi_typedef)(void);            //подфункции spi
// **************************************************************************
typedef struct
{ unsigned char *AdrLpc  ;
  short skolko           ;
  unsigned char WriteFlag; } memspi_type;
// **************************************************************************

#ifdef  DEFINE_MAIN_GLOBAL
#define MAIN_GLOBAL
#else
#define MAIN_GLOBAL extern
#endif

MAIN_GLOBAL memspi_type memspi;

MAIN_GLOBAL volatile osmem_type osmem;
MAIN_GLOBAL unsigned short UseStackSize[16];
MAIN_GLOBAL volatile unsigned int CommonStack[SzStackCommon];  //SzStackCommon   in   OS_asm_c.h
//MAIN_GLOBAL unsigned char GrafBff[YPIXELS*NBYTES*2];

MAIN_GLOBAL char txtLcd[TEXTBUFSIZE];
MAIN_GLOBAL mk_type mk;
MAIN_GLOBAL HeaderOpDrv_type HeaderOpDrv;
MAIN_GLOBAL opk_type opk;
MAIN_GLOBAL mp_type mp[MENULINES];

MAIN_GLOBAL volatile memInstal_type memInstal;
//MAIN_GLOBAL volatile SintCalc_type SintCalc;

MAIN_GLOBAL unsigned int iapmem[5];
MAIN_GLOBAL unsigned int iapmem2[5];
MAIN_GLOBAL mskEn_type mskEn;          // буфер маски отображения групп параметров
MAIN_GLOBAL unsigned char mskEnDrv[4]; // буфер маски отображения приводов

MAIN_GLOBAL unsigned short kontr;
MAIN_GLOBAL char FlagCanReInit;

MAIN_GLOBAL unsigned int CntLutErr;
MAIN_GLOBAL unsigned int *pntErr;
MAIN_GLOBAL unsigned int Err[128];

extern const char AsciiCode14[256][24];
extern unsigned short Ascii_14[256][16];
extern const unsigned short AsciiWindows_16[256][16];


// ************************************************************************************************************/
#endif
