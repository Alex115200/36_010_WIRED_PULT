#ifndef HARD_H
#define HARD_H

/* TIMER match control register */
enum 
{ MR0INT  = 1<<0,
  MR0RES  = 1<<1,
  MR0STOP = 1<<2,
  MR1INT  = 1<<3,
  MR1RES  = 1<<4,
  MR1STOP = 1<<5,
  MR2INT  = 1<<6,
  MR2RES  = 1<<7,
  MR2STOP = 1<<8,
  MR3INT  = 1<<9,
  MR3RES  = 1<<10,
  MR3STOP = 1<<11, };// __mcr_bits;

// TIMER1
typedef struct
{ __REG32   IR  ;    // 0xXXXXX000
  __REG32   TCR ;    // 0xXXXXX004
  __REG32   TC  ;    // 0xXXXXX008
  __REG32   PR  ;    // 0xXXXXX00c
  __REG32   PC  ;    // 0xXXXXX010
  __REG32   MCR ;    // 0xXXXXX014
  __REG32   MR0 ;    // 0xXXXXX018
  __REG32   MR1 ;    // 0xXXXXX01C
  __REG32   MR2 ;    // 0xXXXXX020
  __REG32   MR3 ;    // 0xXXXXX024
  __REG32   CCR ;    // 0xXXXXX028
  __REG32   CR0 ;    // 0xXXXXX02C
  __REG32   CR1 ;    // 0xXXXXX030
  __REG32   CR2 ;    // 0xXXXXX034
  __REG32   CR3 ;    // 0xXXXXX038
  __REG32   EMR ;    // 0xXXXXX03c
 } Ttimer;

#define  T0adr    (Ttimer*)&T0IR   //  0xE0004000
#define  T1adr    (Ttimer*)&T1IR   //  0xE0008000

typedef struct
{ __REG32   CR0   ;  // 0xXXXXX000
  __REG32   CR1   ;  // 0xXXXXX004
  __REG32   DR    ;  // 0xXXXXX008
  __REG32   SR    ;  // 0xXXXXX00c
  __REG32   CPSR  ;  // 0xXXXXX010
  __REG32   IMSC  ;  // 0xXXXXX014
  __REG32   RIS   ;  // 0xXXXXX018
  __REG32   MIS   ;  // 0xXXXXX01C
  __REG32   ICR   ;  // 0xXXXXX020
  __REG32   DMACR ;  // 0xXXXXX024
 } Tssp;

#define  SSP0adr  (Tssp*)&SSP0CR0   //  0x40088000
#define  SSP1adr  (Tssp*)&SSP1CR0   //  0x40030000

enum 
{ MR0 = 1,
  MR1 = 1<<1,
  MR2 = 1<<2,
  MR3 = 1<<3,
  CR0 = 1<<4,
  CR1 = 1<<5,
  CR2 = 1<<6,
  CR3 = 1<<7 };

// GPIO
typedef struct
{ volatile INT32U IO0PIN;     // 0x2009C014
  volatile INT32U IO0SET;     // 0x2009C018
  volatile INT32U IO0DIR;     // 0x2009C000
  volatile INT32U IO0CLR;     // 0x2009C01C
  volatile INT32U IO1PIN;     // 0x2009C034
  volatile INT32U IO1SET;     // 0x2009C038
  volatile INT32U IO1DIR;     // 0x2009C020
  volatile INT32U IO1CLR;     // 0x2009C03C
  volatile INT32U IO2PIN;     // 0x2009C054
  volatile INT32U IO2SET;     // 0x2009C058
  volatile INT32U IO2DIR;     // 0x2009C040
  volatile INT32U IO2CLR;     // 0x2009C05C
  volatile INT32U IO4PIN;     // 0x2009C094
  volatile INT32U IO4SET;     // 0x2009C098
  volatile INT32U IO4DIR;     // 0x2009C080
  volatile INT32U IO4CLR;     // 0x2009C09C
 } Tio;

#define ioAdr (Tio*)&FIO0PIN

#define  cs_spfl_pin  (1<<6)

// Константы инициализации системы
#define FOSC     12000000L         // Частота кварцевого резонатора в Гц
#define VALUEOFM 5                 //  соответствует PLLCFG_bit.MSEL=3
#define CCLK     (FOSC*VALUEOFM)   // CCLK =   60.000 МГц  58,9824 МГц
#define PCLK     CCLK
#define PLOCK    0x0400

#define  FLASHBYTES 528
#define  FLASHWORDS 528/2

#endif
