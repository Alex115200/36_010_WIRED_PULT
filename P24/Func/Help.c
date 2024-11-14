/**********************************************
 *                                            *
 *    (c) Copyright Triol Corporation 2010    *
 *                                            *
 * File name   : Help.c                       *
 * Description : Модуль работы с подсказками  *
 *                                            *
 * Date        : 29.03.2010                   *
 * Author      :                              *
 * Modified    :                              *
 *                                            *
 **********************************************/
#include "win32.h"
#include "DrvOpisStruct.h"    
#include "Menu.h"
#include "OS.h"
#include "Text.h"
#include "File.h"    
#include "string.h"
#include "CommonBff.h"
#include "Help.h"
#include "Monitor.h"
#include "Sint.h"

// **************************************************************************
//  нажатие кнопки "СПРАВКА/ОТМЕНА"
// **************************************************************************
void HelpPush(void)
{
/*id_type idTMP;
unsigned short tmp[2];
int res;

idTMP = *mk.pntId;
if (idTMP.ym == Help_ym)    
{ exitMenu();                      // выход из отображения подсказки
  return; }
else                               // вход в отображение подсказки
{ switch (idTMP.ym)
  { case Mode_ym:
      tmp[0] = strlen(LangMainMenuHelp[idTMP.as2][mk.LangPrm]);
      sprintf((char *)HelpBff, "%s", LangMainMenuHelp[idTMP.as2][mk.LangPrm]);
      break;
    case PrmPuBig_ym:
    case PrmPu_ym:
      tmp[0] = strlen(LangPrmPuHelp[idTMP.as2][mk.LangPrm]);
      sprintf((char *)HelpBff, "%s", LangPrmPuHelp[idTMP.as2][mk.LangPrm]);
      break;
    case Sint1_ym:
      tmp[0] = strlen(Sint1Help[mk.LangPrm]);
      sprintf((char *)HelpBff, "%s", Sint1Help[mk.LangPrm]);
      break;
    case Sint2_ym: break;
    case Sint_ym: return;
    case PrmBig_ym: 
    case Prm_ym: 
      res = GetHelpPrmDrv(&idTMP, tmp);
      if (res)
      { LcdMess(lang.MessNPrg[res], NCHARS2);
        return; }  
      break;
    case GrpAdr_ym: 
    case Grp_ym: 
      res = GetHelpGrpDrv(&idTMP, tmp);
      if (res)
      { LcdMess(lang.MessNPrg[res], NCHARS2);
        return; }  
      break;
    default: 
      tmp[0] = strlen(NoHelp[mk.LangPrm]);
      sprintf((char *)HelpBff, "%s", NoHelp[mk.LangPrm]);
      break; } }
idTMP.ym = HelpList_ym;
idTMP.as2 = tmp[0];      //размер статьи
mk.id[mk.pntMark->Var] = idTMP;
Mov_mdgp();
enterMenu();*/
}

// **************************************************************************
// формирование текста подсказки
//  =>* pntId - указатель на идентификатор подсказки
//  *=> число строк текста подсказки
// **************************************************************************
int HelpPsk(const id_type *pntId)
{
/*int i, sz, index, kolvStr;
id_type *pntTMPId;

sz = pntId->as2;
kolvStr = (sz + (NCHARS-1)) / NCHARS;
index = 0;
pntTMPId = mk.id;
for (i=0; i<kolvStr; i++)
{ pntTMPId->ym = Help_ym;
  if ((index+NBYTES) < sz) pntTMPId->a1 = NCHARS;    //количество символов в строке
  else pntTMPId->a1 = sz - NCHARS * i;
  pntTMPId->as2 = index;            // индекс начального символа строки
  pntTMPId++;
  index += NCHARS; }
mk.lcdTitSel = HelpTit;*/
//return kolvStr;
	return 1;
}


// **************************************************************************
// подсказка о группе
//  =>* pntId - указатель на идентификатор группы
//      tmp   - массив с характеристиками подсказки
//        tmp[0]  длина строки подсказки (0 - подсказки нет)
//        tmp[1]  адрес строки подсказки
//  *=> признак завершения (okWr_cerr - OK, иначе ошибка)
// **************************************************************************
int GetHelpGrpDrv(const id_type *pntId, unsigned short *tmp)
{
/*int i;
unsigned short tmp1[3];
unsigned int otkuda;
opGrp_type opGrpTmp;

if (GetHeaderOpDrv(tmp1, pntId->a1, ((int)&opDrv.GrpStart)/2, 3) == 0)
 return nCon_cerr; //адрес таблицы описателей групп
for (i=0; i<tmp1[2]; i++)
{ if (pntId->as2 == mk.id[i].as2) 
  { otkuda = tmp1[0] + i * sizeof(opGrp_type);
    break; } }
if (RdHwMem(&opGrpTmp, pntId->a1, otkuda, (sizeof(opGrp_type)/2)) == 0)
 return nCon_cerr; //описатель группы 
if (opGrpTmp.HelpLen == 0) return Help_cerr;
tmp[0] = opGrpTmp.HelpLen;
if (opGrpTmp.HelpLen > szHelpBff) tmp[0] = szHelpBff;
tmp[1] = opGrpTmp.HelpOfs;
if (RdHwMem(shHelpBff, pntId->a1, opGrpTmp.HelpOfs, (tmp[0]+1)/2) == 0)
 return nCon_cerr; //читаем статью
return okWr_cerr;*/return 0;
}

// **************************************************************************
// подсказка о параметре
//  =>* pntId - указатель на идентификатор параметра
//      tmp   - массив с характеристиками подсказки
//        tmp[0]  длина строки подсказки (0 - подсказки нет)
//        tmp[1]  адрес строки подсказки
//  *=> признак завершения (okWr_cerr - OK, иначе ошибка)
// **************************************************************************
int GetHelpPrmDrv(const id_type *pntId, unsigned short *tmp)
{
/*if (RdHwMem(tmp, pntId->a1, (int)(pntId->as2 + (unsigned char *)&opPrm.HelpLen), 2) == 0)
//if (RdHwFile(tmp, pntId->a1, FileHelpPrm, (int)(pntId->as2 + (unsigned char *)&opPrm.HelpLen), 2) == 0)
 return nCon_cerr; //читаем адрес и sz 
if (tmp[0] == 0) return Help_cerr;
if (tmp[0] > szHelpBff) tmp[0] = szHelpBff;
if (RdHwMem(shHelpBff, pntId->a1, tmp[1], (tmp[0]+1)/2) == 0)
 return nCon_cerr; //читаем статью
return okWr_cerr;*/
  return 0;
}

// **************************************************************************
