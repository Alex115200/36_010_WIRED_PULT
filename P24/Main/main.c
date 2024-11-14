/*************************************************************************
 *
 *    (c) Copyright _Triol Corporation 2010
 *
 *    File name   : main.c
 *    Description : Define main module
 *
 *    1. Date        :
 *       Author      :
 *       Description :
 *
 **************************************************************************/

// **************************************************************************
// <<<<<<<  ��������� �.�. >>>>>>>  
// 1. ��� �������� ������ � ������� ������� AT45DB642 ��������
// #define BLOCK_STM_MEMORY
// 2. ��� ������� ���������� Bootlaoder'a � PULT_BOOT FRAM, ��� ���������� ������� �� PULT,    
// ���� �������� SUBSTITUTE_BOOTLOADER, ����� ����������� �������� CheckSum ����� ��������� ���������.
// ��� ����� ����������, ��� ������� ������ ������ PULT_BOOT ����� �������� ��������� ��������� �� ��,
// �.�. �������� CheckSum ����� � �������.
// 3. ��������� ���������� � ������ ��������� � ������ ��. ��������!!! �������� ������ ����������� �������� ���������.
// **************************************************************************


#include "globalIncludes.h"


//======================================================================
//			      ����������	main()
//======================================================================
void DummyFunc (void);
void initOS(void);
void RCC_Init(void);

__noreturn
void main(void) 
{  
    #ifdef VERSION_SECTION
    // ���� ������������ ��������������� ������ � ������� ��, ���������� �� ����� �������� MSP � ������� ����������
    // ��������� �������� ������� MSP � ������� ����������
    __set_MSP( *(uint32_t*)(0x08010100) );
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x10100); // ��������� ����� �������� ����������    
    #endif
    
    // ���������� ������������� ���������
    RCC_Init();
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
        /* Get BOR Option Bytes */
    if((FLASH_OB_GetBOR() & 0x0C) != OB_BOR_LEVEL1) 
    {
        /* Unlocks the option bytes block access */
        FLASH_OB_Unlock();

        /* Select the desired V(BOR) Level -------------------------------------*/
        FLASH_OB_BORConfig(OB_BOR_LEVEL1); 

        /* Launch the option byte loading */
        FLASH_OB_Launch();

        /* Locks the option bytes block access */
        FLASH_OB_Lock();
    }
    
    //PWR->CR = 0xF0;
    rt_hw_usart_init();             // ������������� �������� Uart
    
    spiInitAT45_HARD();
    spiInitFRAM_HARD();
    DMAInitForSpi();
    TIM5_Debug_Init();          // ������ �� ���������� ������
    
    InitWD(); // ������ Watchdoga, ����� �������� OS, ����� ������ ������ ��������� WDT � ������� �������� �����
    
    initOS();
    
    // ����� ������ ������������ ��� ��������� ������ versionsection
    DummyFunc (); // ����������� ��������� ��� ��������� ����������    
}
//======================================================================

//======================================================================
//       ������������� ������������ ������� ����������
//======================================================================
#ifdef _PROJECT_IAR_
void RCC_Init(void)
{
    RCC_AHB1PeriphClockCmd(
    	RCC_AHB1Periph_GPIOA |
    	RCC_AHB1Periph_GPIOB |
    	RCC_AHB1Periph_GPIOC |
    	RCC_AHB1Periph_GPIOD |
    	RCC_APB1Periph_SPI2  |
    	RCC_APB1Periph_TIM3  |
    	RCC_APB1Periph_TIM5  |
    	RCC_AHB1Periph_DMA1 , ENABLE); 
}

void InitWD(void)
{
    IWDG->KR=0X5555;//���� ��� ������� � �������
    IWDG->PR=4; //���������� ������������
    IWDG->RLR=4000; //��������� ������� ������������, ���� ���� ���� �� ����� ��������
    IWDG->KR=0XAAAA;//������������
    IWDG->KR=0XCCCC;//������ � ������
}

void ResetWD(tsk_IDtype id){
  
  // !*!*! SOSNOVCHIK - ������� ���������� WDT ������������.
  // ������ ������ ���� ����� ��������� ���������� WDT. ��������� ��� ��� �������� �� ������������ ������ ������
  InitWD ();
   
/*  
  static char buferokk[4];
  char i; 
  
  switch(id){
    case tsk_lcd: buferokk[0] = tsk_lcd;
      break;
    case tsk_drv: buferokk[1] = tsk_drv;
      break;
    case tsk_kbd: buferokk[2] = tsk_kbd;
      break;
    case tsk_cont: buferokk[3] = tsk_cont;
      break;
  }
  
  if(buferokk[0] + buferokk[1] + buferokk[2] + buferokk[3] == 10){
    for(i = 0; i<4; i++){
      buferokk[i] = 0;
    }
    IWDG->KR = 0x5555;
    IWDG->RLR=4000; //��������� ������� ������������, ���� ���� ���� �� ����� ��������
    IWDG->KR=0XAAAA;//������������
    IWDG->KR=0XCCCC;//������ � ������
  }
*/
  
}

void ResetAllWD(void){
  ResetWD(tsk_lcd);
  ResetWD(tsk_drv);
  ResetWD(tsk_kbd);
  ResetWD(tsk_cont);
}

#else
void RCC_Init(void)
{
}
#endif


#ifdef VERSION_SECTION
//======================================================================
// ***********   ������ versionsection    ******************************
//======================================================================
// �������������� ������ ������, ����������� ��� ����������� �������� ������ �������� � 8,9,10,11 ����� �� ������ ���������.
// ������ ������ 256 ���� - 0x100
// �������� ��������� �������������� �� ������ 0x8010000 - ��� � � ������ ������� ���������. �� ������� ��������� ���������� � ������ 0x8010100
// ��� ���������� ������ � ����� icf ������ ���� ����� ��������� ����� ROM = 0x08010100;
// ��� ���������� ������ � ����� icf ������ ���� ����� ��������� ����� .intvec = 0x08010100;
// ��� �� ���������� �������� ���������� � icf ���� � �������� ������ - versionsection � ������ � ���������� � � ������:
// define symbol __ICFEDIT_region_VER_start__ = 0x08010000;
// define symbol __ICFEDIT_region_VER_end__   = 0x080100FF;
// define region VER_region   = mem:[from __ICFEDIT_region_VER_start__   to __ICFEDIT_region_VER_end__];
// place in VER_region   { readonly section .versionsection };

// ���������� Reset_Handler � ����� ����, �� ����������� ��� ��������
void Reset_Handler (void);

// ���������� �������������� ������, ����������� �� ���������� ������ �����, ��� �������� ����� ���������
#pragma section = ".versionsection"
#pragma data_alignment=4

// ��������� ���������� � ����. �������, � ��������� static const - ����� ��� �� ������� � bin ��� hex ����.
// 1. 0x8010000 -  ��������� �� ��������� �������� Main Stack Pointer - ��� ���� ��������� �� �����������
static const u32 MainStackPointer @ 0x8010000 = 0x2000A000; // MSP
// 2. 0x8010004 - ����� ����������� ������ �� ������������ ������� ��������� (��� �� ����� ��� � � 0x8010104)
static const u32 ResetHandlerAddr @ 0x8010004 = (u32)Reset_Handler; // Reset_Handler
// 3. 0x8010008 - ������� ��� ����� - ������ ��������� ������, ������� ��� ����� - ��������� ���������.
static const u32 PrgVersion @ 0x8010008 = (u32)(GRP_VERSION_PO_PU24E*1000+SUB_VERSION_PO_PU24E) + (u32)((u32)(MRK_VERSION_PO_PU24E) << 16);
// 4. 0x8010010 - ������ ����������� ��� ����� ����� ��������� � ������� ���������
static const char VersionStr[] @ 0x8010010 = "+VERSION HEADER+";
// ��������������� ������������ ��� ��������� ������ versionsection
void DummyFunc (void) @ ".versionsection"
{      
    u32 temp = (u32)(&MainStackPointer);  // ����������� ��������� ��� ��������� ����������
    temp = ResetHandlerAddr;
    temp = PrgVersion;
    temp = VersionStr[0];

}
// ============= ����� ������ versionsection ===========================
//======================================================================
#else
    #warning "��������!!! ��������� ������ ������ ���������!!! �������� ������ ������ � �� �� ����� ��������� �����������!!!"
#endif


// =========================== ����� ����� ===========================

/////  http://supermcu.googlecode.com/svn/trunk/LPC17XX/Micrium/Software/uCOS-II/
///// http://www.2beanet.com/ucos/Source/index.html
// =========================== ����� ����� ===========================
