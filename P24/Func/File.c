/**********************************************
 *                                            *
 *    (c) Copyright Triol Corporation 2010    *
 *                                            *
 * File name   : File.c                       *
 * Description : Модуль инсталляции привода   *
 *                                            *
 * Date        : 29.03.2010                   *
 * Author      :                              *
 * Modified    :                              *
 *                                            *
 **********************************************/
#include "include.h"

void *adrInMem(u16 file, u32 adr);
inline u32 checkAdrInFile(u16 file, u32 adr, u16 size);


const FATtable_type FATtable[] = {
    0                           , 0                          , FILE_In_ERR    , FILE_ReadOnly, // 0
    (u32)&spfl.FileDescr[0]     , sizeof(spfl.FileDescr)     , FILE_In_Flash  , FILE_RdWr    , // 1
    (u32)&spfl.FileDescr[1]     , sizeof(spfl.FileDescr)     , FILE_In_Flash  , FILE_RdWr    , // 2
    (u32)&spfl.FileDescr[2]     , sizeof(spfl.FileDescr)     , FILE_In_Flash  , FILE_RdWr    , // 3
    (u32)&spfl.FileDescr[3]     , sizeof(spfl.FileDescr)     , FILE_In_Flash  , FILE_RdWr    , // 4
    (u32)&spfl.FileDescr[4]     , sizeof(spfl.FileDescr)     , FILE_In_Flash  , FILE_RdWr    , // 5
    (u32)&spfl.FileDescr[5]     , sizeof(spfl.FileDescr)     , FILE_In_Flash  , FILE_RdWr    , // 6
    (u32)&spfl.FileDescr[6]     , sizeof(spfl.FileDescr)     , FILE_In_Flash  , FILE_RdWr    , // 7
    (u32)&spfl.FileDescr[7]     , sizeof(spfl.FileDescr)     , FILE_In_Flash  , FILE_RdWr    , // 8
    (u32)&spfl.FileDescr[8]     , sizeof(spfl.FileDescr)     , FILE_In_Flash  , FILE_RdWr    , // 9
    (u32)&spfl.FileDescr[9]     , sizeof(spfl.FileDescr)     , FILE_In_Flash  , FILE_RdWr    , // 10
    (u32)&spfl.FileDescr[10]    , sizeof(spfl.FileDescr)     , FILE_In_Flash  , FILE_RdWr    , // 11
    (u32)&spfl.FileDescr[11]    , sizeof(spfl.FileDescr)     , FILE_In_Flash  , FILE_RdWr    , // 12
    (u32)&spfl.FileDescr[12]    , sizeof(spfl.FileDescr)     , FILE_In_Flash  , FILE_RdWr    , // 13
    (u32)&spfl.FileDescr[13]    , sizeof(spfl.FileDescr)     , FILE_In_Flash  , FILE_RdWr    , // 14
    (u32)&spfl.FileDescr[14]    , sizeof(spfl.FileDescr)     , FILE_In_Flash  , FILE_RdWr    , // 15
    (u32)&spfl.FileDescr[15]    , sizeof(spfl.FileDescr)     , FILE_In_Flash  , FILE_RdWr    , // 16
    0                           , 0                          , FILE_In_ERR    , FILE_ReadOnly, // 17
    0                           , 0                          , FILE_In_ERR    , FILE_ReadOnly, // 18
    0                           , 0                          , FILE_In_ERR    , FILE_ReadOnly, // 19
    0                           , 0                          , FILE_In_ERR    , FILE_ReadOnly, // 20
    0                           , 0                          , FILE_In_ERR    , FILE_ReadOnly, // 21
    0                           , 0                          , FILE_In_ERR    , FILE_ReadOnly, // 22
    0                           , 0                          , FILE_In_ERR    , FILE_ReadOnly, // 23
    0                           , 0                          , FILE_In_ERR    , FILE_ReadOnly, // 24
    0                           , 0                          , FILE_In_ERR    , FILE_ReadOnly, // 25
    0                           , 0                          , FILE_In_ERR    , FILE_ReadOnly, // 26
    0                           , 0                          , FILE_In_ERR    , FILE_ReadOnly, // 27
    0                           , 0                          , FILE_In_ERR    , FILE_ReadOnly, // 28
    0                           , 0                          , FILE_In_ERR    , FILE_ReadOnly, // 29
    0                           , 0                          , FILE_In_ERR    , FILE_ReadOnly, // 30


    /*(u32)&nv.File.LnkNetFDP[0]    , sizeof(nv.File.LnkNetFDP), FILE_In_Fram   , FILE_RdWr    , // 31 !!!Не меняять расположение
    (u32)&nv.File.LnkNetFDP[1]    , sizeof(nv.File.LnkNetFDP), FILE_In_Fram   , FILE_RdWr    , // 32
    (u32)&nv.File.LnkNetFDP[2]    , sizeof(nv.File.LnkNetFDP), FILE_In_Fram   , FILE_RdWr    , // 33
    (u32)&nv.File.LnkNetFDP[3]    , sizeof(nv.File.LnkNetFDP), FILE_In_Fram   , FILE_RdWr    , // 34
    (u32)&nv.File.LnkNetFDP[4]    , sizeof(nv.File.LnkNetFDP), FILE_In_Fram   , FILE_RdWr    , // 35
    (u32)&nv.File.LnkNetFDP[5]    , sizeof(nv.File.LnkNetFDP), FILE_In_Fram   , FILE_RdWr    , // 36
    (u32)&nv.File.LnkNetFDP[6]    , sizeof(nv.File.LnkNetFDP), FILE_In_Fram   , FILE_RdWr    , // 37
    (u32)&nv.File.LnkNetFDP[7]    , sizeof(nv.File.LnkNetFDP), FILE_In_Fram   , FILE_RdWr    , // 38
    (u32)&nv.File.LnkNetFDP[8]    , sizeof(nv.File.LnkNetFDP), FILE_In_Fram   , FILE_RdWr    , // 39
    (u32)&nv.File.LnkNetFDP[9]    , sizeof(nv.File.LnkNetFDP), FILE_In_Fram   , FILE_RdWr    , // 40
    (u32)&nv.File.LnkNetFDP[10]   , sizeof(nv.File.LnkNetFDP), FILE_In_Fram   , FILE_RdWr    , // 41
    (u32)&nv.File.LnkNetFDP[11]   , sizeof(nv.File.LnkNetFDP), FILE_In_Fram   , FILE_RdWr    , // 42
    (u32)&nv.File.LnkNetFDP[12]   , sizeof(nv.File.LnkNetFDP), FILE_In_Fram   , FILE_RdWr    , // 43
    (u32)&nv.File.LnkNetFDP[13]   , sizeof(nv.File.LnkNetFDP), FILE_In_Fram   , FILE_RdWr    , // 44
    (u32)&nv.File.LnkNetFDP[14]   , sizeof(nv.File.LnkNetFDP), FILE_In_Fram   , FILE_RdWr    , // 45
    (u32)&nv.File.LnkNetFDP[15]   , sizeof(nv.File.LnkNetFDP), FILE_In_Fram   , FILE_RdWr    , // 46*/
};

//===================================================================
//===================================================================
u8 fileRd(u8 *Bff, u16 file, u32 adr, u16 size)
{
    u16                 memType;
    void                *Adr;
    u8                  err;

    // Проверка на допустимый диапазон файлов
    if(size == 0)                                           return 0;
    if((memType = checkAdrInFile(file, adr, size)) == 0 )   return 0;


    Adr = adrInMem(file, adr);
    if(memType == FILE_In_Flash){
        err = AT45RD(Bff, Adr, size);
    }
    else if((memType == FILE_In_Fram)){
        err = FM25RD(Bff, Adr, size);
    }

    return err;

}
//===================================================================
//===================================================================
u8 fileWr(u8 *Bff, u16 file, u32 adr, u16 size)
{
    u16                 memType;
    void                *Adr;
    u8                  err;

    // Проверка на допустимый диапазон файлов
    if(size == 0)                                           return 0;
    if((memType = checkAdrInFile(file, adr, size)) == 0 )   return 0;


    Adr = adrInMem(file, adr);
    if(memType == FILE_In_Flash){
        err = AT45WR(Bff, Adr, size);
    }
    else if((memType == FILE_In_Fram)){
        err = FM25WR(Bff, Adr, size);
    }

    return err;

}
/*============================================================================*/
// Проверка запроса на допустимый адрес по номеру файла адресу и количеству регистров
/*============================================================================*/
inline u32 checkAdrInFile(u16 file, u32 adr, u16 size)
{
    if(file < sizeof(FATtable) / sizeof(FATtable_type)){
        if((adr + size) > FATtable[file].size )     return 0;
        else                                        return FATtable[file].alloc;
    }
    else                                            return 0;
}
/*============================================================================*/
//                 Вычисление адреса по номеру файла
/*============================================================================*/
void *adrInMem(u16 file, u32 adr)
{
    return (void*)(FATtable[file].adres + adr);
}















































/*const unsigned int segAdrFl[3][3] = {
    {0                                   ,   0,                        0             },
    {(unsigned int)&spfl.FileDescr[0]    ,   sizeof(spfl.FileDescr),   FileInAt45    },
    {(unsigned int)&nv.FileParamPult     ,   sizeof(nv.FileParamPult), FileInFram    }
};*/

// **************************************************************************
//  функция файлового чтения в приводе
// **************************************************************************


// **************************************************************************
//  функция файлового чтения в контроллере
// **************************************************************************
/*char fileRd(unsigned char *Bff, unsigned char file, unsigned int adr, unsigned short int size)
{

}*/


// **************************************************************************
//  функция файлового чтения
//   (привод инсталлирован - читается из флэши, иначе прямо из привода)
// =>* kuda     - указатель на массив для прочитанных данных
//     NetAdr   - сетевой адрес
//     otkudaHw - смещение откуда читать
//     skolkoHw - сколько short-слов читать
// *=> результат чтения (1 - OK, 0 - ошибка)
// **************************************************************************

int RdHwMem(void *kuda, unsigned char NetAdr, unsigned short otkudaHw, unsigned short skolkoHw)
{
/*if (otkudaHw == 0xFFFF) return 0;             // ошибка
int adr = (int)(otkudaHw+NetAdr*SzOpDrvSpfl);
if (mk.InstalMsk & (1 << NetAdr)) return AT45Read(kuda, adr, skolkoHw*2);
#ifdef EMUL_6869
RdHwFileH(kuda, (int)otkudaHw, skolkoHw);
return 1;
#else
return RdHwFile(kuda,NetAdr, FileInstal, (int)(otkudaHw/2), skolkoHw);
#endif*/return 0;
}

// **************************************************************************
// чтение заголовка привода HeadOfFile
// =>* kuda     - указатель на массив для прочитанных данных
//     NetAdr   - сетевой адрес
//     otkudaHw - смещение откуда читать
//     skolkoHw - сколько short-слов читать
// *=> результат чтения (1 - OK, 0 - ошибка)
// **************************************************************************
int GetHeaderOpDrv(void *kuda,
          unsigned char NetAdr,
          unsigned short otkudaHw,    //hw адреса
          unsigned short skolkoHw)
{
/*if (skolkoHw == 0) return 0;
if (HeaderOpDrv.NetAdr != NetAdr)
{ if (RdHwMem(&HeaderOpDrv, NetAdr, ((int)&opDrv.Crc)/2, 8) == 0) return 0;
  HeaderOpDrv.NetAdr = NetAdr; }     //Crc, GrpStart, PrmStart, KolvGrp
HwCopy(kuda, (unsigned short *)(&HeaderOpDrv)+otkudaHw, skolkoHw);
return 1;*/return 0;
}

// ****************************************************************************
//  проверка инсталлированности приводов
//   Подпрограмма предлагает инсталлировать привод,
//    если он включен, но не инсталлирован.
//   Инсталляция привода - вычитка его файла описателей параметров
//    и запись прочитанных данных в большую флэшу.
//   Если привод был инсталлирован (при инсталляции в mk.InstalMsk
//    взводится соответствующий бит), файл описателей параметров
//    вычитывается из соответствующего места большой флэши.
// ****************************************************************************
void checkInstal(void)
{
/*unsigned short nDgp = 0;
unsigned char wf[MAXDRIVES];

mk.InstalMsk = 0;
if (mk.MestnDis == 0)
{ DgpList[0] = LOCALNET;
  DgpList[1] = 0xFFFF;
  nDgp = 1;
  promtInstDrv(0); }
else
{ nDgp = 0;
  FM25Read((int)&nv.NetDrv, wf, MAXDRIVES);   // сведения о приводах в сети
  //idTmp.ym = Drv_ym;           // дистанционный
  while (wf[nDgp] != 0xFF)
  { //idTmp.a1 = wf[net];
    //if (DgpBird(&idTmp, 0))
    DgpList[nDgp] = (unsigned short)wf[nDgp];
    promtInstDrv(nDgp);
    nDgp++; }
  DgpList[nDgp] = 0xFFFF; }
CurGroup = CurParam = 0;*/
}

// ****************************************************************************
// предложение инсталляции привода с предпроверкой
//  Подпрограмма выполняет следующие действия в зависимости от результата
//   проверки инсталлированности привода:
//   привод не инсталлирован и не ответил: возврат 0
//   привод не инсталлирован и ответил: предложение инсталляции
//   привод инсталлирован: установка бита в mk.InstalMsk
//  =>* anet - индекс в списке сетевых приводов
//  *=> результат предложения (1 - OK, 0 - отказ)
// ****************************************************************************
int promtInstDrv(int anet)
{

/*if (mk.InstalMsk & (1<<anet)) return 1; // флаг "привод инсталлирован" установлен

switch (checkInstalDrv(anet))
{ case InstOff_cid:                // инсталлирован и не ответил
  case InstOn_cid:                 // инсталлирован и ответил
    mk.InstalMsk |= (1 << anet);
    return 1;
  case nInstOn_cid:               // не инсталлирован и ответил
    txtLcdClear();
    GrafBffClear();
    msnprintf(txtLcd+NCHARS, NCHARS2, mk.Lang->InstalDevice, DgpList[anet]);
    memcpy(&txtLcd[NCHARS3], &(mk.Lang->EnterExit_txt), NCHARS2);
    // запросить разрешение на инсталляцию и запустить процедуру, если "да"
    if (WaitEnterExit() == KEY_ENTER) return Instal(anet);
    LcdMess(lang.InstDevOtkaz, NCHARS2);    // ответ "нет"
  default: return 0; }             // не инсталлирован и не ответил*/return 0;
}

// ****************************************************************************
// проверка инсталлированности привода
//  =>* anet - адрес в списке сетевых приводов
//  *=> результат проверки
//       0 nInstOff_cid  Не инсталлирован и не ответил
//       1 nInstOn_cid  Не инсталлирован и ответил
//       2 InstOff_cid  Инсталлирован и не ответил
//       3 InstOn_cid  Инсталлирован и ответил
// ****************************************************************************
int checkInstalDrv(int anet)
{
/*#ifndef EMUL_6869
int net = DgpList[anet];
#endif
unsigned int CrcDrv = 0xFFFFFFFFUL;
// прочесть КС файла привода из привода net
#ifdef EMUL_6869
RdHwFileH((unsigned short *)&CrcDrv, ((int)(&opDrv.Crc))/2, 2);
// сравнить КС из привода с КС из опер. флэши
//  совпала - признак выхода "привод ответил и инсталлирован"
if (rintFlash((int)&nv.CrcOpis[anet], FM256_FLASH) == CrcDrv) return InstOn_cid;
//  не совпала - записать в опер. флэшу флаг "привод не инсталлирован"
wbyteFlash((int)&nv.InsDrv[anet], 0xFF, FM256_FLASH);
// признак выхода "привод ответил, но не инсталлирован"
return nInstOn_cid;
#else
if (RdHwFile((unsigned short *)&CrcDrv, net, FileInstal, ((int)&opDrv.Crc)/2, 2))
{ // сравнить КС из привода с КС из опер. флэши
  //  совпала - признак выхода "привод ответил и инсталлирован"
  if (rintFlash((int)&nv.CrcOpis[anet], FM256_FLASH) == CrcDrv) return InstOn_cid;
  //  не совпала - записать в опер. флэшу флаг "привод не инсталлирован"
  wbyteFlash((int)&nv.InsDrv[anet], 0xFF, FM256_FLASH);
  // признак выхода "привод ответил, но не инсталлирован"
  return nInstOn_cid; }
// посмотреть в опер. флэше флаг инсталляции и если он не 0xFF,
//  признак выхода "привод не ответил, но инсталлирован и должен быть в сети"
if (rbyteFlash((int)&nv.InsDrv[anet], FM256_FLASH) == net) return InstOff_cid;
// признак выхода "привод не ответил и не инсталлирован"
return nInstOff_cid;
#endif*/return 0;
}

// ****************************************************************************
//  Инсталляция привода
//  =>* anet - индекс в списке сетевых приводов
// *=> результат предложения (1 - OK, 0 - ошибка)
// ****************************************************************************
int Instal(int anet)
{
/*int i;
int skolkoW, skolkoB, szLeft;
unsigned int CrcDrv;              // КС файла (из привода)
unsigned short szOpis;            // длина файла (из привода)
unsigned short spiBffTmp[FLASHWORDS];

char *kuda = (void *)(anet * SzOpDrvSpfl);
unsigned char *otkuda = 0;
int net = DgpList[anet];
memset(spiBffTmp, 0, FLASHBYTES);
// прочесть КС файла
#ifdef EMUL_6869
RdHwFileH((unsigned short *)&CrcDrv, (int)(&opDrv.Crc), 2);
// прочесть длину файла
RdHwFileH(&szOpis, (int)(&opDrv.DescrSize), 1);
#else
if (RdHwFile((unsigned short *)&CrcDrv, net, FileInstal, (int)(&opDrv.Crc), 2) == 0)
 return ErrInstal();    //Crc
// прочесть длину файла
if (RdHwFile(&szOpis, net, FileInstal, ((int)(&opDrv.DescrSize))/2, 1) == 0)
 return ErrInstal();    //szOpis, VidDev
#endif
mk.InstalMsk &= ~(1 << anet);
InstalLcd(net, szOpis, 0);                  // начальный прогресс-бар
// очистить место в большой флэши
AT45Erase((anet*SzOpDrvSpfl)/FLASHBYTES, SzOpDrvSpfl/FLASHBYTES);
// перенести в большую флэшу систему параметров из привода
for (szLeft=szOpis; szLeft; szLeft-=skolkoW)
{ if (szLeft > FLASHWORDS) skolkoW = FLASHWORDS;
  else skolkoW = szLeft;
  skolkoB = skolkoW << 1;
  InstalLcd(net, szOpis, szOpis-szLeft);    // прогресс-бар в динамике
  // прочесть параметры
#ifdef EMUL_6869
  RdHwFileH(spiBffTmp, (int)otkuda, skolkoW);
  otkuda += skolkoB;
#else
  if (RdHwFile(spiBffTmp, net, FileInstal, (int)otkuda, skolkoW) == 0)
   return ErrInstal();
  otkuda += skolkoW;
#endif
  if (AT45Write((int)kuda, spiBffTmp, skolkoB) == 0) return ErrInstal();
  kuda += skolkoB; }
if (szOpis)
{ i = GetCrcSpfl((SzOpDrvSpfl*anet+HEADSIZE),(szOpis*2-HEADSIZE));
  if (i == CrcDrv)
  { wbyteFlash((int )&nv.InsDrv[anet], 0x5A, FM256_FLASH);  // признак "инсталлирован"
    wintFlash((int )&nv.CrcOpis[anet], CrcDrv, FM256_FLASH);// КС привода
    mk.InstalMsk |= (1<<anet);                              // бит привода в маске инст. приводов
    LcdMess(lang.InstDevOk, NCHARS2);                       // Инсталляция успешно завершена
    return 1; } }
return ErrInstal();*/return 0;
}

// ****************************************************************************
//  Сообщение об ошибке инсталляции привода
// *=> 0 (признак ошибки)
// ****************************************************************************
int ErrInstal(void)
{
/*LcdMess(lang.InstDevBad, NCHARS2);
return 0;*/return 0;
}

// ****************************************************************************
void InstalLcd(char NetAdr, int Kolvo, int tek)
{
/*const char ProgBar[NCHARS] =
{ '0',                 // 1
  ' ',                 // 2
  prStart,             // 3
  prLight,             // 4
  prLight,             // 5
  prLight,             // 6
  prLight,             // 7
  prLight,             // 8
  prLight,             // 9
  prLight,             // 10
  prLight,             // 11
  prEnd,               // 12
  ' ',                 // 13
  '1',                 // 14
  '0',                 // 15
  '0' };               // 16

unsigned int mytek, myadr;
unsigned char i, mybyte;
unsigned int adr = NBYTES * (6 * 8 + 2) + 2;
unsigned int fulltek = 10 * 10 * tek;
fulltek /= Kolvo;
txtLcdClear();
if (NetAdr) msnprintf(txtLcd+NCHARS, NCHARS, mk.Lang->InstalProgres, NetAdr);
else ByteCopy((unsigned char *)&txtLcd+NCHARS, (const unsigned char *)mk.Lang->InquiryProgres, NCHARS);
ByteCopy((unsigned char *)&txtLcd+NCHARS3, (const unsigned char *)ProgBar, NCHARS);
GrafBffClear();                 // очистка графического буфера
Sym84To512(0);
while (fulltek)
{ if (fulltek > 8) mytek = 8;
  else mytek = fulltek;
  mybyte = 0x00;
  switch (mytek)
  { case 8: mybyte |= 0x01;
    case 7: mybyte |= 0x02;
    case 6: mybyte |= 0x04;
    case 5: mybyte |= 0x08;
    case 4: mybyte |= 0x10;
    case 3: mybyte |= 0x20;
    case 2: mybyte |= 0x40;
    case 1: mybyte |= 0x80; }
  myadr = adr;
  GrafBff[myadr] = 0xFF;
  myadr += NBYTES;
  for (i=1; i<11; i++)
  { GrafBff[myadr] = mybyte;
    myadr += NBYTES; }
  GrafBff[myadr] = 0xFF;
  fulltek -= mytek;
  if (fulltek) adr++; }
GrfOut();                                  //вывод на экран*/
}

// ****************************************************************************
//  опрос приводов
//   =>* net - фиктивный параметр
// ****************************************************************************
int Inquiry(int net)
{
/*int i, res;
unsigned char wf[MAXDRIVES];
Can8b_type CanBuf;

int k = 0;
if (mk.MestnDis == 0) wf[k++] = LOCALNET;// "местный привод виден"
else
{ InstalLcd(0, MAXNETDRIVES, 0);         // начальный прогресс-бар
  for (i=1; i<MAXNETDRIVES; i++)
  { res = RdValue(&CanBuf, i, nonFrmt);
    if (res) wf[k++] = (unsigned char)i; // "привод есть в сети и виден"
    if (i & 0x02) InstalLcd(0, MAXNETDRIVES, i);    // прогресс-бар в динамике
    if (k == MAXDRIVES) break; } }
mk.netDrives = k;
wshortFlash((int )&nv.NetDrives, mk.netDrives, FM256_FLASH);
//wbyteFlash((int )&nv.NetDrives, mk.netDrives, FM256_FLASH);
while (k < MAXDRIVES) wf[k++] = 0xFF;
FM25Write((int)&nv.NetDrv, wf, MAXDRIVES);
FM25Write((int)&nv.VisDrv, wf, MAXDRIVES);
return 1;*/return 0;
}

// ****************************************************************************
//  подсчет контрольной суммы в большой флэши
//   =>* Bff   - адрес массива данных флэши
//       SzBff - размер массива данных в байтах
//   *=> полученная контрольная сумма
// ****************************************************************************
int GetCrcSpfl(int Bff, int SzBff)
{
/*unsigned short StartCrc, kolvTmp;
int szLeft, otkudaTmp;
unsigned char tmpBff[FLASHBYTES];

otkudaTmp = Bff;
StartCrc = 0xFFFF;
kolvTmp = 0;
for (szLeft=SzBff; szLeft; szLeft-=kolvTmp)
{ if (szLeft > FLASHBYTES) kolvTmp = FLASHBYTES;
  else kolvTmp = szLeft;
  AT45Read(tmpBff, otkudaTmp, kolvTmp);
  otkudaTmp += kolvTmp;
  StartCrc = CrcPice(tmpBff, kolvTmp, StartCrc); }
return StartCrc;*/return 0;
}

// ****************************************************************************
