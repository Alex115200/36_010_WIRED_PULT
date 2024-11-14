/**********************************************
 *                                            *
 *    (c) Copyright Triol Corporation 2010    *
 *                                            *
 * File name   : OS_asm_c.h                   *
 * Description : Основные определения для     *
 *     аппаратной части                       *
 *                                            *
 * Date        : 29.03.2010                   *
 * Author      : Shatyorny A.V.               *
 * Modified    : Apostolov S.V.               *
 *                                            *
 **********************************************/
#ifndef  OS_ASM_C_H
#define OS_ASM_C_H

// ****************************************************************************

#define  SzStackCommon 4        //128*4
#define  Fcpu          (14745600*4)    //11059200*5
#define  Fvpb          Fcpu      //Fcpu     

#define  bitTimer0      20
#define  SysTik        (Fvpb/1000)    //Fvpb/Time_uS  
#define  KeyTik        (Fvpb/1000)    //Fvpb/Time_uS  
#define  i2cSpeed      100000  //400000

#define RES160
//#define FONT 86
//#define FONT 108

#ifdef RES160
#ifdef FONT
#if FONT == 86
// ПАРАМЕТРЫ ДИСПЛЕЯ 160*160 ПРИ ШРИФТЕ 8*6
#define  NLINES     20
#define  NCHARS     26
#define  XCPIXELS   6
#define  YCPIXELS   8
#endif              // FONT == 86
#if FONT == 108
// ПАРАМЕТРЫ ДИСПЛЕЯ 160*160 ПРИ ШРИФТЕ 10*8
#define  NLINES     16
#define  NCHARS     20
#define  XCPIXELS   8
#define  YCPIXELS   10
#endif              // FONT == 108
#else               // ndef FONT
// ПАРАМЕТРЫ ДИСПЛЕЯ 160*160 ПРИ ШРИФТЕ 16*10
#define  NLINES     10
#define  NCHARS     16
#define  XCPIXELS   10
#define  YCPIXELS   16
#endif              // def FONT
#define  VALBEG     2
#define  VALSIZE    (NCHARS-VALBEG)

// ПАРАМЕТРЫ ЭМУЛИРОВАННОГО ДИСПЛЕЯ 64*128 ПРИ ШРИФТЕ 8*6
#else               // ndef RES160
#define  NLINES     8
#define  NCHARS     21
#define  XPIXELS    128
#define  YPIXELS    64
#define  XCPIXELS   6
#define  YCPIXELS   8
#endif              // def RES160

#define  NCHARS2      (NCHARS*2)
#define  NCHARS3      (NCHARS*3)
#define  XPIXELS2     (XPIXELS*2)
#define  XPIXELS3     (XPIXELS*3)
#define  MENULINES    (NLINES-2)     // полное число строк меню на экране   
#define  LASTMENULINE (MENULINES-1)  // номер последней экранной строки меню
#define  FLASHBYTES   528            // размер страницы АТ45 в байтах
#define  FLASHWORDS   (FLASHBYTES/2) // размер страницы АТ45 в short словах
#define  FLASHPORTION (FLASHBYTES/6) // размер порции чтения/записи АТ45 в short словах
#define  HEADSIZE     128            // размер заголовка FileInstal в байтах

#define  TEXTBUFSIZE  (NCHARS*NLINES)
#define  GRAFBUFSIZE  (YPIXELS*NBYTES)

#define HOTPARAMS     32        // число элементов горячего списка
#define LASTPARAMS    32        // число элементов списка последниx хоррекций
#define MAXPULTPARAMS 32        // max число параметров пульта
#define MAXPASSSIZE   8         // max длина пароля в пульте
#define MENUPUNKTS    11        // тек число пунктов меню
#define PULTPARAMS    16        // тек число параметров пульта
#define MAXSINTPARAMS 16        // max число синтетических параметров
#define MAXBIGSYMBOLS 13        // max число символов в большом тексте
#define MAXGROUPS     100       // max число групп
#define MAXPARAMS     100       // max число параметров
#define MAXNETDRIVES  255       // max число сетевых адресов приводов

#define ROOTLINES     7         // число пунктов главного меню

#define LOCALNET       1        // сетевой адрес привода (для местного подключения)

#define PUSKCOMMAND  0xFF00
#define STOPCOMMAND  0x0000

// ****************************************************************************

#define    KolvoProc    16
#define    KovoEvent    32
#define    KovoResourse 32

#define    event_complet  (1<<0)    
#define    event_sful    (1<<1)    //event Successful
#define    abort_fos    (1<<2)    //process prervali
#define    event_ok    (event_complet|event_sful)

// ****************************************************************************
//KEY      KEY      KEY      KEY      KEY      KEY      KEY
// ****************************************************************************
#define  d_10mS        16    //10
#define  d_100mS        2    //10
#define  firstDelay      1
// ****************************************************************************
//Flagi_mo
#define  mastDost      0
#define  tehnDost      1
#define  userDost      2
#define  fmo_net       (1<<2)    //режим присутствует если привод сетевой
//dostupPm = 0        master
//dostupPt = 1        tehn
//dostupPu = 2        user
#define  rootM        (1<<3)    //корневой режим
#define  nastrM       (1<<4)    //подрежимы настройки пульта

// ****************************************************************************
#define  nvI2c   0xA2      //flash memory
#define  timI2c  0xD0
// ****************************************************************************
                                   //FlagKey  
#define  f_Rdk       (1<<2)        //=1 редактируем
#define  f_overUp    (1<<3)        //=1 при редактировании букв
#define  f_invers    (1<<4)        //=1 инвертируем
#define  f_DataChang (1<<5)        //=1 перерисуем экран
#define  f_inmenu    (1<<6)        //=1 находимся в меню

// ****************************************************************************
//SPFL      SPFL      SPFL      SPFL      SPFL      
// ****************************************************************************

// ****************************************************************************
//            Number Event
// ****************************************************************************
#define  SysTim_event     0
#define  Timer1_event     1
#define  Uart0_event      2
#define  Uart1_event      3
#define  Pwm0_event       4
#define  I2c_event        5
#define  Spi0_event       6
#define  Spi1_event       7
#define  Rtc_event        8
#define  Eint0_event      9
#define  Eint1_event      10
#define  Eint2_event      11
#define  Eint3_event      12
#define  Adc_event        13
#define  CanTx1_event     14
#define  CanTx2_event     15
#define  CanRx1_event     16
#define  CanRx2_event     17                      
#define  mS1_event        18
#define  Can1Ok_event     19
#define  TmsWr_event      20
#define  ModbusFunc_event 21
#define  SoftIrq_event    22
#define  CalcPrmDrv_event 23
#define  chKey_event      24
#define  Freq_event       25
#define  CanWr0_event     26
//#define  Cmd_event        27
#define  End_event        27

// ****************************************************************************
//FlagProc
// ****************************************************************************
#define  flKey      (1<<2)
// ****************************************************************************
//            Number Process
// ****************************************************************************

#define  FonProc     0
#define  procname1   1
#define  MonitorProc 2
#define  mS1_Proc    3
#define  procname4   4
#define  procname5   5
#define  procname6   6
#define  procname7   7
#define  procname8   8
#define  procname9   9
#define  procname10  10
#define  procname11  11
#define  procname12  12
#define  procname13  13
#define  procname14  14
#define  procname15  15

// ****************************************************************************
//    System call numbers
// ****************************************************************************

#define  MUTEX_swc        0
#define  WAIT_swc         1
#define  UNMUTEX_swc      2
#define  POSTEVENT_swc    3    //Old
#define  CREATE_swc       4
#define  DESTROY_swc      5
#define  POSTPROC_swc     6    //Old
#define  FATALERR_swc     7
#define  setf_swc         8
#define  clrf_swc         9
#define  setb_swc         10
#define  clrb_swc         11
#define  bl_cli_swc       12
#define  POSTEVENTNew_swc 13
#define  POSTPROCNew_swc  14
#define  PERIOD_swc       15
#define  DifOsFlag_swc    16
#define  CheckFlClr_swc   17
#define  CheckFlSet_swc   18

// ****************************************************************************
//            Number Resourse
// ****************************************************************************
#define  Uart0_res      0
#define  ModbusFunc_res 1
#define  LargeStack_res 2
#define  Timer1_res     3
#define  Spi1_res       4
#define  TmsWr_res      5
#define  i2c_res        6

// ****************************************************************************
//    ERROR
// ****************************************************************************

#define    progError    0

// ****************************************************************************

#define    probelZnac     127    // значок пробела
#define    log_0          128    // 0x80
#define    log_1          129    // 0x81
#define    front          130    // 0x82
#define    spad           131    // 0x83
#define    prDark         132    // 0x84       
#define    prLight        133    // 0x85
#define    prStart        134    // 0x86
#define    prEnd          135    // 0x87
#define    darkSym        136    // 0x88 символ чёрный фон
#define    strelkaRight   137    // 0x89
#define    strelkaLeft    138    // 0x9A
#define    strelkaUp      139    // 0x8B
#define    strelkaDw      140    // 0x8C
#define    rightRdy       141    // 0x8D стрелочка вправо с ограничением  
#define    leftRdy        142    // 0x8E стрелочка влево с ограничением  
#define    upRdy          143    // 0x8F стрелочка вверх с ограничением
#define    dwRdy          144    // 0x90 стрелочка вверх с ограничением
#define    strelkaRightTr 145    // 0x91 стрелочка вправо треугольничком
#define    strelkaLeftTr  146    // 0x92 стрелочка влево треугольничком
#define    darkFon        147    // 0x93 темный фон 
#define    i_plus         148    // 0x94 
#define    infPrm         149    // 0x95 информационный
#define    delta          150    // 0x96 дельта
#define    kvadrat        151    // 0x97 квадрат (2 сверху) 
#define    kub            152    // 0x98 куб (3 сверху) 
#define    galka_a        153    // 0x99 
#define    galka_b        154    // 0x9A 
#define    Ngalka_a       155    // 0x9B 
#define    Ngalka_b       156    // 0x9C 
#define    error_1a       157    // 0x9D 
#define    error_1b       158    // 0x9E 
#define    error_2a       159    // 0x9F 
#define    error_2b       160    // 0xA0 
#define    error_3a       161    // 0xA1 
#define    error_3b       162    // 0xA2 
#define    redPrm         163    // 0xA3   редактируемый
#define    fi             164    // 0xA4 
#define    Mnbyte         165    // 0xA5 подчёркивание сверху (старший байт)
#define    summa          166    // 0xA6 сумма
#define    pl_minus       167    // 0xA7 (плюс минус) 
#define    u0_d0          168    // 0xA8
#define    u1_d0          169    // 0xA9
#define    u0_d1          170    // 0xAA
#define    u1_d1          171    // 0xAB
                          
#define    gradus         176    // 0xB0 градус

// ****************************************************************************
#endif
