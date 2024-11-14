/**********************************************
 *                                            *
 *    (c) Copyright Triol Corporation 2010    *
 *                                            *
 * File name   : Prog.c                       *
 * Description : Модуль работы                *
 *        с параметрами привода               *
 *                                            *
 * Date        : 29.03.2010                   *
 * Author      : Shatyorny A.V.               *
 * Modified    : Apostolov S.V.               *
 *                                            *
 **********************************************/
#include "win32.h"
#include "DrvOpisStruct.h"    
#include "Monitor.h"
#include "Menu.h"
#include "syscall.h"
#include "OS.h"
#include "Text.h"
#include "DGP.h"  
#include "Prog.h"  
#include "File.h"
#include "Hard.h"
#include "RdkText.h"
#include "FormHot.h"
#include "PuPrm.h"
#include "Pasword.h"
#include "Sint.h"
#include "Big.h"


// ******   ОПРЕДЕЛЕНИЯ uC-OS II   ******
#ifdef _PROJECT_IAR_
#include <os_cpu.h>
#include "os_cfg_r.h"
#include <ucos_ii.h>
#endif
#include "globdefs.h"  
#include "globdata.h"

void DrvNamePrg(id_type *pntId, mp_type *pntMp, int EnterExit);  //=0/1 Exit/Enter
// ****************************************************************************  
#define  retPrintf  (pStringLcd_typedef) ret

#define  ret_0        0
#define  ret_1        1
#define  UtimPrgPsk_N 2
#define  AdrPrgPsk_N  3
#define  DMWr_N       4
#define  uTimWr_N     5

// ****************************************************************************  
const pFuncPskPrg_typedef TablPrgFunc[] = {
    NULLPsk   ,    //0
    retPsk    ,    //1
    UtimPrgPsk,    //2
    AdrPrgPsk ,    //3
    DMWr      ,    //4
    uTimWr         //5
};
// ****************************************************************************  
//  таблица обработки форматов 
//   - адрес подпрограммы вывода параметра в меню
//   - адрес таблицы клавиатуры
//   - адрес подпрограммы вывода параметра в большом виде
//   - адрес стартовой подпрограммы редактирования.
//   - адрес финишной подпрограммы редактирования.
//   - max значение positZnm
// ****************************************************************************  
/*const FmtOpis_type FmtOpisTbl[] = 
{ //Printf         key       Big       Psk          wr
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    },   //nonFrmt     0
  {NtxtPrintf,  Int32_Key, PoliBig , ret_1       , ret_1   , 0    },   //NtxtFrmt    1     // номер текста
  {Hex32Printf, Int32_Key, Hex32Big, ret_1       , ret_1   , 4    },   //Hex16Frmt   2     // unsigned short числа 
  {Int32Printf, Int32_Key, Int32Big, ret_1       , ret_1   , 4    },   //int16Frmt   3     // short числа 
  {HMPrintf,    Utime_Key, PoliBig , ret_1       , ret_1   , 3    },   //HourMinFrmt 4     // время: минуты, часы
  {HMPrintf,    Utime_Key, PoliBig , ret_1       , ret_1   , 3    },   //MinSecFrmt  5     // время: минуты, секунды
  {retPrintf,   Int32_Key, retBig  , ret_1       , ret_1   , 0    },   //floatFrmt   6     // float числа 
  {Int32Printf, Int32_Key, Int32Big, ret_1       , ret_1   , 7    },   //int32Frmt   7     // int числа 
  {HMPrintf,    Utime_Key, PoliBig , ret_1       , ret_1   , 3    },   //HmsFrmt     8     // время: часы, минуты, секунды
  {UTimPrintf,  Utime_Key, PoliBig , UtimPrgPsk_N, uTimWr_N, 0xFF },   //UTimeFrmt   9     // линейное время от 1января 2001года
  {RefPrintf,   Int32_Key, RefBig  , ret_1       , ret_1   , 4    },   //refFormat   10    // ссылки 
  {HMPrintf,    Utime_Key, PoliBig , ret_1       , ret_1   , 3    },   //MonDayFrmt  11    // дата: месяц, день
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    },   //nonFrmt     12 
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    },   //nonFrmt     13 
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    },   //nonFrmt     14
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    },   //nonFrmt     15
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    },   //nonFrmt     16
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    },   //nonFrmt     17
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    },   //nonFrmt     18
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    },   //nonFrmt     19
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    },   //nonFrmt     20
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    },   //nonFrmt     21
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    },   //nonFrmt     22
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    },   //nonFrmt     23
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    },   //nonFrmt     24
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    },   //nonFrmt     25
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    },   //nonFrmt     26
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    },   //nonFrmt     27
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    },   //nonFrmt     28
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    },   //nonFrmt     29
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    },   //nonFrmt     30
  {retPrintf,   Int32_Key, retBig  , ret_0       , ret_0   , 0    } }; //nonFrmt     31*/
                                                
// **************************************************************************
/*const pFuncKey_typedef Int32_Key[] = 
{ ret       ,  // RUN
  ret       ,  // STOP
  leftInt32 ,  // LEFT 
  upInt32   ,  // UP   
  ret       ,  // FUNC 
  progMenu  ,  // ENTER
  rightInt32,  // RIGHT
  exitPrg   ,  // ESC  
  downInt32 ,  // DOWN 
  ret       }; //nonCode*/
// **************************************************************************
/*const pFuncKey_typedef Bsq8_Key[] =
{ ret       ,    //nonCode
  upBsq8    ,    // UP   
  downBsq8  ,    // DOWN 
  leftInt32 ,    // LEFT 
  rightInt32,    // RIGHT
  progMenu  ,    // ENTER
  exitPrg   ,    // ESC  
  ret       };   // FUNC */
// **************************************************************************
/*const pFuncKey_typedef Utime_Key[] =
{ ret       ,  // RUN
  ret       ,  // STOP
  leftInt32 ,  // LEFT 
  upUtime   ,  // UP   
  ret       ,  // FUNC 
  progMenu  ,  // ENTER
  rightInt32,  // RIGHT
  exitPrg   ,  // ESC  
  downUtime ,  // DOWN 
  ret       }; //nonCode*/
// **************************************************************************
/*const pFuncKey_typedef DatMn_Key[] =
{ ret       ,  // RUN
  ret       ,  // STOP
  leftInt32 ,  // LEFT 
  upDm      ,  // UP   
  ret       ,  // FUNC 
  progMenu  ,  // ENTER
  rightInt32,  // RIGHT
  exitPrg   ,  // ESC  
  downDm    ,  // DOWN 
  ret       }; //nonCode*/
// **************************************************************************
//GALKI    GALKI    GALKI    GALKI    GALKI    GALKI    GALKI
// **************************************************************************
/*const pFuncKey_typedef GalPrg_Key[] =
{ ret      ,   // RUN
  ret      ,   // STOP
  exitGal ,    // LEFT 
  upMenu  ,    // UP   
  progMenu,    // FUNC 
  enterGal,    // ENTER
  enterGal,    // RIGHT
  exitGal ,    // ESC  
  downMenu,    // DOWN 
  ret     };   //nonCode*/
// ****************************************************************************  

//static const int adTim[6] = {1,10,60,600,60*60,60*60*10};
//static const int addDecim[10]={1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};

// ****************************************************************************
//  выход из режима редактирования параметра без сохранения изменений
// ****************************************************************************
void exitPrg(void)
{
progCmm(0);
}

// ****************************************************************************
//  вход в режим редактирования параметра или выход из режима с записью изменений
// ****************************************************************************
void progMenu(void)
{
progCmm(1);
}

// ****************************************************************************
//  распределитель обработчиков редактирования значений
//       EnterExit - флаг сохранения изменений (1 - да, 0 - нет)
// ****************************************************************************
void progCmm (int EnterExit)
{
    switch (mk.pntId->ym){
        case Mode_ym:
            switch (mk.pntId->a1){
                case EndList_md: AdrPrgWr(mk.pntId, mk.pntMp); 
                break; 
            } 
            break;
        case PrmBig_ym:  //WARNING
        case Prm_ym: 
            PrmRdk(mk.pntId, mk.pntMp, EnterExit); 
            break;

        case DrvGal_ym: 
        case GrpGal_ym: 
        case PrmGal_ym: 
            DgpGalPrg(mk.pntId, mk.pntMp, EnterExit); 
            break;
        case SintAdr_ym:
        case PrmPuAdr_ym:                      
        case PrmAdr_ym: 
            AdrPrgWr(mk.pntId, mk.pntMp); 
            break;    //Заходим только при выходе
        case Drv_ym: 
            DrvNamePrg(mk.pntId, mk.pntMp, EnterExit); 
            break;
        case Fhot_ym: 
            FhotAdrPrg(mk.pntId, mk.pntMp, EnterExit); 
            break;
        case Flast_ym: 
            FlastAdrPrg(mk.pntId, mk.pntMp, EnterExit); 
            break;
        case PrmPuBig_ym:  //WARNING
        case PrmPu_ym: 
            PrmRdk(mk.pntId, mk.pntMp, EnterExit); 
            break;
        case Sint2_ym: 
            /*Sint2Prg(mk.pntId, mk.pntMp, EnterExit);*/ 
            break;
        case SintBig_ym:  //WARNING
        case Sint_ym: 
            /*SintPrg(mk.pntId, mk.pntMp, EnterExit);*/ 
            break;
        case Mpsw_ym: 
            /*MpswPrg(mk.pntId, mk.pntMp, EnterExit);*/ 
            break;
    }
}
// ****************************************************************************
//   редактирование параметра пульта или привода
//   =>* pntId   - указатель на 
//       pntMp   - указатель на 
//       EnterExit - флаг сохранения изменений (1 - да, 0 - нет)
// ****************************************************************************
void PrmRdk(id_type *pntId, mp_type *pntMp, int EnterExit)    //=0/1 Exit/Enter
{
/*int tmp;
const FmtOpis_type *pntFOp;
long long val, hLim;

pntFOp = &FmtOpisTbl[pntMp->format];      //указатель на описатель формата
if ((mk.FlagKey & f_Rdk) == 0)
{ // обработка входа в режим редактирования параметра
  if ((tmp = RdkPrmControl(pntMp, pntId, 1)))
  { LcdMess(lang.MessNPrg[tmp], NCHARS2);   // не можем редактировать
    return; }
  else                                      // начнем редактирование
  { mk.curHot = 0xFFFF;
    if (StartPrgPrmPu_cmm(pntId,pntMp) == 0) return;  //стартовая функция пульта
    if ((TablPrgFunc[(int)pntFOp->StartPrgFmt_N](pntId,pntMp)) == 0) return;
    mk.keySelSave = mk.keySel;
    mk.lcdSelSave = mk.lcdSel;
    if ((mk.pntMp->format == refFormat) && (mk.pntMp->u.Znach != 9999)) 
     mk.curHot = (unsigned short)mk.pntMp->u.Znach;
    if (pntFOp->maxPosit != 0xFF) 
    { mk.maxPosit = pntFOp->maxPosit + 1;
      if ((mk.pntMp->format == int32Frmt) || (mk.pntMp->format == int16Frmt))
       hLim = (long long)mk.pntMp->s.HighLim;
      else hLim = (long long)mk.pntMp->u.HighLim;
      do
      { val = 1;
        for (tmp=mk.maxPosit; tmp>0; tmp--) val *= 10;
        if (val > hLim) mk.maxPosit--; }
      while (val > hLim); }
    mk.keySel = pntFOp->keySel;
    pntMp->Status |= ActiveStr_smp;
    mk.FlagKey |= f_Rdk;         // флаг "мы в режиме редактирования"
    mk.positZnm = 0; } }
else 
{ // обработка выхода из режима редактирования параметра
  pntMp->Status &= ~ActiveStr_smp;
  mk.FlagKey &= ~f_Rdk;
  mk.kolMigZnm = 0;
  mk.keySel = mk.keySelSave;
  mk.lcdSel = mk.lcdSelSave;
  if (EnterExit)
  { if (FinishPrgPrmPu_cmm(pntId, pntMp) == 0)  return;  // финишная функция пульта
    if ((TablPrgFunc[(int)pntFOp->FinishPrgFmt_N](pntId,pntMp)) == 0) return;
    tmp = WrValProg(pntId, pntMp);
    if (tmp != okWr_cerr) LcdMess(lang.MessNPrg[tmp], NCHARS2); }   //не можем записать
  else if ((pntId->ym == PrmPu_ym) && (pntId->as2 == KONTRAST)) Kontrast(mk.Kontrast); }*/
}

// ****************************************************************************
//  запись измененного значения редактируемого параметра
//   =>* pntId   - указатель на 
//       pntMp   - указатель на 
// ****************************************************************************
int WrValProg(id_type *pntId, mp_type *pntMp)
{
return 0;
}

// ****************************************************************************
//  стартовая программа редактирования параметра пульта
//   =>* pntId   - указатель на 
//       pntMp   - указатель на 
// ****************************************************************************
int StartPrgPrmPu_cmm(const id_type *pntId, mp_type *pntMp)
{
/*if ((pntId->ym != PrmPu_ym) && (pntId->ym != PrmPuBig_ym)) return 1;
switch (TablPuOp[pntId->as2].StartPrgPrmPu_N)        //номер пусковой функции пульта
{ case 0: return 1;                    //None
  case 1: return PaswChange(pntId, pntMp);        //Pasword
  default: return  0; }*/return  0;
}

// ****************************************************************************
//  финишная программа редактирования параметра пульта
//   =>* pntId   - указатель на 
//       pntMp   - указатель на 
// ****************************************************************************
int FinishPrgPrmPu_cmm(const id_type *pntId, mp_type *pntMp)
{
/*if ((pntId->ym != PrmPu_ym) && (pntId->ym != PrmPuBig_ym)) return 1;
return TablPuOp[pntId->as2].FinishPrgPrmPu(pntId,pntMp);    //номер пусковой функции пульта*/
  return 0;
}

// ****************************************************************************
int RdkPrmControl(mp_type *pntMp, id_type *pntId, int rdLim)
{
/*Can8b_type Bff8;
int kolvFormat, FlgPrm;
  
FlgPrm = pntMp->FlgPrm;
if (((pntId->ym == Prm_ym) || (pntId->ym == PrmBig_ym) || 
    (pntId->ym == PrmPu_ym) || (pntId->ym == PrmPuBig_ym)) == 0)
 return okWr_cerr;
kolvFormat = sizeof(FmtOpisTbl) / sizeof(FmtOpis_type);    //количество форматов в пульте
if ((pntId->ym == Prm_ym) || (pntId->ym == PrmBig_ym))
{ kolvFormat = 32;                            //у привода 32 формата
  if ((pntMp->Status & mskPrg_smp) != ((1<<NameStat) | (1<<ValStat)))
   return nCon_cerr;  //нет связи
  FlgPrm = RdFlagPrmDrv(pntId);
  if (FlgPrm == -1)  return nCon_cerr;  //нет связи
  if ((FlgPrm & (1<<F_LimCh)) && (rdLim))
  { if (RdLimPrm(&Bff8, pntId->a1, FlgPrm & TYPE__MASK) == 0)
     return nCon_cerr;
    pntMp->s.LowLim  = Bff8.LowLim;
    pntMp->s.HighLim = Bff8.HighLim; } }
if ((FlgPrm & TYPE__MASK) >= kolvFormat) return FmtErr_cerr;  //ошибка формата
if (FmtOpisTbl[pntMp->format].StartPrgFmt_N == ret_0) return FmtErr_cerr;  //ошибка формата
if (((FlgPrm & (1<<F_tehWr)) == 0) && mk.dostupPu) return Master_cerr; //редактируется произв
if ((FlgPrm & EDITEDMASK) == 0) return okWr_cerr;  //редактируется
if ((FlgPrm & EDITEDMASK) ==  (1<<F_rdk)) return InfPrm_cerr;  //не редактируется
if (((FlgPrm & EDITEDMASK) == (2<<F_rdk)) && (mk.StatWordMark & ReadyRef_sw)) return Stop_cerr;    //редактируется в останове
if (((FlgPrm & EDITEDMASK) == (3<<F_rdk)) && ((mk.StatWordMark & jmpr_sw) == 0)) return Jmpr_cerr;    //редактируется с перемычкой*/
return okWr_cerr;
}

// ****************************************************************************
int RdFlagPrmDrv(const id_type *pntId)
{
/*unsigned short tmp;
  
if (RdHwMem(&tmp, pntId->a1, pntId->as2, 1) == 0) return -1;  //описатель параметра */
return 1;
}

// **************************************************************************
int AdrPrgPsk(id_type *pntId, mp_type *pntMp)
{
/*id_type  idTMP;
  
idTMP.ym = DrvAdr_ym;
idTMP.a1 = pntId->a1;
mk.utochnAdrSave = mk.utochn;
mk.id[mk.pntMark->Var] = idTMP;
enterMenu();*/
return 0;      //делаем вид что в редактирование не входим
}

// ****************************************************************************
int AdrPrgWr(id_type *pntIdIn, mp_type *pntMpIn)
{
/*int tmp;

id_type idTMPIn = *pntIdIn;
mk.utochn = mk.utochnAdrSave+1;
exitMenu();
mk.pntMp->u.Znach = idTMPIn.as2;
if (idTMPIn.ym == PrmAdr_ym) idTMPIn.ym = Prm_ym;
if (idTMPIn.ym == PrmPuAdr_ym) idTMPIn.ym = PrmPu_ym;
if (idTMPIn.ym == SintAdr_ym)  idTMPIn.ym = Sint_ym;
switch (mk.pntId->ym)
{ case Prm_ym: 
    tmp = WrValue(mk.pntId->a1, mk.pntMp->u.Znach, uns16Frmt);
    //tmp = WrValue(mk.pntId->a1, mk.pntId->as2, mk.pntMp->u.Znach, uns16Frmt);
    if (tmp) LcdMess(lang.MessNPrg[tmp], NCHARS2);    //не можем записать
    break;
  case Sint2_ym: 
    mk.pntMp->id = idTMPIn;
    WrSint2();      
    break;
  case Fhot_ym:    
    mk.utochn++;
    exitMenu(); 
    break;
  case Flast_ym:    
    mk.utochn++;
    exitMenu(); break; }*/
return 1;
}

// ****************************************************************************
int UtimPrgPsk(id_type *pntId, mp_type *pntMp)
{
/*time_type *tpnt = (time_type *)pntMp->TxtValue;
mk.maxPosit = 4;
if ((pntId->as2 & 1) == 0)    //четный номер - время
{ mk.maxPosit = 5;
  tpnt->second = 0;
  tpnt->minute = 0;
  tpnt->hour = 0;
  pntMp->u.LowLim = smhdmy_to_ch(tpnt);    //WARNING 
  tpnt->second = 59;
  tpnt->minute = 59;
  tpnt->hour = 23;
  pntMp->u.HighLim = smhdmy_to_ch(tpnt); }    //WARNING*/
return 1;
}

// **************************************************************************
int DMWr(id_type *pntId, mp_type *pntMp)
{
/*time_type *tpnt = (time_type *)pntMp->TxtValue;
pntMp->u.Znach = (tpnt->month<<8) | tpnt->date;*/
return 1;
}

// **************************************************************************
int uTimWr(id_type *pntId, mp_type *pntMp)
{
/*Can8b_type Bff8;
time_type *tpnt = (time_type *)pntMp->TxtValue;
time_type tpnt2;
if (pntId->as2 & 1)                        //дата
{ if (RdValue(&Bff8, mk.NetAdrMark, UTimeFrmt) == 0) 
//{ if (RdValue(&Bff8, mk.NetAdrMark, pntId->as2, UTimeFrmt) == 0) 
  { LcdMess(lang.MessNPrg[nCon_cerr], NCHARS2);          //нет связи
    return 0; };
  ch_to_smhdmy(&tpnt2, (unsigned int)Bff8.Value);
  tpnt->second = tpnt2.second;
  tpnt->minute = tpnt2.minute;
  tpnt->hour = tpnt2.hour;
  pntMp->u.Znach = smhdmy_to_ch(tpnt); }*/
return 1;
}

// **************************************************************************
void udUtime(int add)
{
/*if (((mk.pntId->as2&1) == 0) | (mk.pntMp->format == HourMinFrmt))    //четный номер - время
{ ud32(adTim,add); }*/
}

// **************************************************************************
void udNpol(const DatOp_type *datPnt, int add)
{
/*int val;  
unsigned char *rval;                    //редактируемый байт

time_type *tpnt = (time_type *)mk.pntMp->TxtValue;    //текст значения используем как буфер времени
datPnt += mk.positZnm;                    //описатель
rval = ((unsigned char *)tpnt + datPnt->ofsRval);        //к чему добавлять
val = datPnt->addVal;                    //что добавлять
if (add)
{ val = *rval + val;
  if (val > datPnt->HighLim) val = datPnt->HighLim; }
else 
{ val = (int)*rval - val;
  if (val < datPnt->LowLim) val = datPnt->LowLim; }
*rval = val;*/
}

// **************************************************************************
void ud32(const int *ad_Val, int add)
{
/*long long adVal, Val, HLim, LLim;

adVal = (long long)ad_Val[mk.positZnm];
if ((mk.pntMp->format == int32Frmt) || (mk.pntMp->format == int16Frmt))
{ HLim = (long long)mk.pntMp->s.HighLim;
  LLim = (long long)mk.pntMp->s.LowLim;
  Val = (long long)mk.pntMp->s.Znach; }
else 
{ HLim = (long long)mk.pntMp->u.HighLim;
  LLim = (long long)mk.pntMp->u.LowLim;
  Val = (long long)mk.pntMp->u.Znach; }
if (add)
{ Val = Val + adVal;
  if (Val > HLim) 
  { if (mk.pntMp->format == NtxtFrmt) Val = LLim;
    else Val = HLim; } }
else 
{ Val = Val - adVal;
  if (Val < LLim) 
  { if (mk.pntMp->format == NtxtFrmt) Val = HLim;
    else Val = LLim; } }
mk.pntMp->s.Znach = (int)Val;
if ((mk.pntId->ym == PrmPu_ym) && (mk.pntId->as2 == KONTRAST)) Kontrast(Val);*/
}

// **************************************************************************
void udBsq8(int add)
{
/*if (add) mk.pntMp->u.Znach |= (1<<mk.positZnm);
else mk.pntMp->u.Znach &= ~(1<<mk.positZnm);*/
}

// **************************************************************************
void downUtime(void)
{
//udUtime(0);
}

// **************************************************************************
void upUtime(void)
{
//udUtime(1);
}

// **************************************************************************
void downBsq8(void)
{
//udBsq8(0);
}

// **************************************************************************
void upBsq8(void)
{
//udBsq8(1);
}

// **************************************************************************
void downInt32(void)
{
//ud32(addDecim, 0);
}

// **************************************************************************
void upInt32(void)
{
//ud32(addDecim, 1);
}

// **************************************************************************
void upDm(void)
{
}

// **************************************************************************
void downDm(void)
{
}

// **************************************************************************
void leftInt32(void)
{
//if (mk.positZnm < mk.maxPosit) mk.positZnm++;
}

// **************************************************************************
void rightInt32(void)
{
//if (mk.positZnm > 0) mk.positZnm--;
}

// **************************************************************************
int RdGalGrpPrm(int NetAdr)
{
/*if (mk.NetAdrRdGal == NetAdr) return 1;
if (AT45Read(&mskEn, (int)&spfl.MaskPrm[NetAdr], sizeof(mskEn_type)) == 0) return 0;
mk.NetAdrRdGal = NetAdr;*/
return 1;
}

// **************************************************************************
void enterGal(void)
{
/*if (mk.pntId->ym == DrvGal_ym) RdGalGrpPrm(mk.pntId->a1);
enterMenu();*/
}
// **************************************************************************
void exitGal(void)
{
/*exitMenu();
SaveGal();*/
}

// **************************************************************************
void SaveGal(void)
{
/*int NetAdr, tmp;
unsigned char wf[MAXDRIVES];

if ((mk.pntId->ym == Mode_ym) && mk.MestnDis)    // выходим из режима скрытия.
{ for (NetAdr=1; NetAdr<MAXDRIVES; NetAdr++)
  { if (mskEnDrv[NetAdr>>3] & (1<<(NetAdr&7))) wf[NetAdr] = 0x5A;
    else wf[NetAdr] = 0xFF; }
  FM25Write((int )&nv.VisDrv, wf, MAXDRIVES); }
if (mk.MestnDis == 0) NetAdr = LOCALNET;
else NetAdr = mk.pntId->a1;
tmp = ((mk.pntId->ym == Mode_ym) && (mk.MestnDis == 0));
if ((mk.pntId->ym == DrvGal_ym) || tmp)
{ AT45Erase((int)(&spfl.MaskPrm[NetAdr])/FLASHBYTES, 1);
  AT45Write((int)&spfl.MaskPrm[NetAdr], &mskEn, sizeof(mskEn_type)); }
if (mk.pntId->ym == Mode_ym) mk.keySel = mk.keySelSave;*/
}

// **************************************************************************
void DgpGalPrg(id_type *pntId, mp_type *pntMp, int EnterExit)  //=0/1 Exit/Enter
{
/*char *AdrTmp;
int Nbit;

if (pntId->ym == DrvGal_ym)
{ Nbit = pntId->a1;
  AdrTmp = (char *)mskEnDrv; }
else 
{ Nbit = pntId->as2;
  if (pntId->ym == GrpGal_ym) AdrTmp = mskEn.mgrp;
  else AdrTmp = mskEn.mprm; }
AdrTmp[Nbit>>3] ^= (1<<(Nbit&7));*/
}

// ****************************************************************************
int retPsk( id_type *pntId, mp_type *pntMp)
{
  return 1;
}

// ****************************************************************************
