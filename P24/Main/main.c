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
// <<<<<<<  Сосновчик Д.М. >>>>>>>  
// 1. Для возврата работы с внешней памятью AT45DB642 добавить
// #define BLOCK_STM_MEMORY
// 2. Для подмены параметров Bootlaoder'a в PULT_BOOT FRAM, для нормальной отладки ПО PULT,    
// надо добавить SUBSTITUTE_BOOTLOADER, тогда контрольная проверка CheckSum будет проходить нормально.
// Без этого объявления, при отладке каждый запуск PULT_BOOT будет пытаться загрузить программу из ПЧ,
// т.к. проверка CheckSum будет с ошибкой.
// 3. Добавлена информация в начало программы о версии ПО. Внимание!!! Изменены адреса расположеня реальной программы.
// **************************************************************************


#include "globalIncludes.h"


//======================================================================
//			      Собственно	main()
//======================================================================
void DummyFunc (void);
void initOS(void);
void RCC_Init(void);

__noreturn
void main(void) 
{  
    #ifdef VERSION_SECTION
    // Если используется вспомогательная секция с версией ПО, необходимо по месту заменить MSP и таблицу прерываний
    // Установка реальных адресов MSP и таблицы прерываний
    __set_MSP( *(uint32_t*)(0x08010100) );
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x10100); // поправили адрес таблички прерываний    
    #endif
    
    // Дальнейшая инициализация программы
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
    rt_hw_usart_init();             // инициализация драйвера Uart
    
    spiInitAT45_HARD();
    spiInitFRAM_HARD();
    DMAInitForSpi();
    TIM5_Debug_Init();          // Вообще то отладочный таймер
    
    InitWD(); // Запуск Watchdoga, перед запуском OS, иначе задачи начнут запускать WDT с меньшим временем жизни
    
    initOS();
    
    // Вызов пустой подпрограммы для активации секции versionsection
    DummyFunc (); // Вынужденные обращения для активации переменных    
}
//======================================================================

//======================================================================
//       Инициализация тактирования модулей процессора
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
    IWDG->KR=0X5555;//Ключ для доступа к таймеру
    IWDG->PR=4; //Обновление предделителя
    IWDG->RLR=4000; //Загружаем регистр перезагрузки, счет идет вниз от этого значения
    IWDG->KR=0XAAAA;//перезагрузка
    IWDG->KR=0XCCCC;//Запуск в работу
}

void ResetWD(tsk_IDtype id){
  
  // !*!*! SOSNOVCHIK - Штатный перезапуск WDT заблокирован.
  // Теперь каждый таск может выполнять перезапуск WDT. Проверяем как это скажется на стабильности работы пульта
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
    IWDG->RLR=4000; //Загружаем регистр перезагрузки, счет идет вниз от этого значения
    IWDG->KR=0XAAAA;//перезагрузка
    IWDG->KR=0XCCCC;//Запуск в работу
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
// ***********   СЕКЦИЯ versionsection    ******************************
//======================================================================
// Дополнительная секция памяти, необходимая для корректного внесения версии прошивки в 8,9,10,11 байты от начала программы.
// Размер секции 256 байт - 0x100
// Загрузка программы осуществляется по адресу 0x8010000 - как и в старых версиях программы. Но рабочая программа начинается с адреса 0x8010100
// Для нормальной работы в файле icf должен быть задан начальный адрес ROM = 0x08010100;
// Для нормальной работы в файле icf должен быть задан начальный адрес .intvec = 0x08010100;
// Так же необходимо добавить информацию в icf файл о размерах секции - versionsection и задать и разместить её в памяти:
// define symbol __ICFEDIT_region_VER_start__ = 0x08010000;
// define symbol __ICFEDIT_region_VER_end__   = 0x080100FF;
// define region VER_region   = mem:[from __ICFEDIT_region_VER_start__   to __ICFEDIT_region_VER_end__];
// place in VER_region   { readonly section .versionsection };

// Определеям Reset_Handler в явном виде, он потребуется для перехода
void Reset_Handler (void);

// Собственно дополнительная секция, вставленная по размещению памяти ранее, чем основная часть программы
#pragma section = ".versionsection"
#pragma data_alignment=4

// Размещаем переменные в след. порядке, с указанием static const - иначе они не попадут в bin или hex файл.
// 1. 0x8010000 -  Указатель на временное значение Main Stack Pointer - без него программа не запускается
static const u32 MainStackPointer @ 0x8010000 = 0x2000A000; // MSP
// 2. 0x8010004 - Адрес обработчика сброса из оригинальной таблицы программы (тот же самый что и в 0x8010104)
static const u32 ResetHandlerAddr @ 0x8010004 = (u32)Reset_Handler; // Reset_Handler
// 3. 0x8010008 - младшие два байта - версия программы пульта, старшие два байта - сигнатура программы.
static const u32 PrgVersion @ 0x8010008 = (u32)(GRP_VERSION_PO_PU24E*1000+SUB_VERSION_PO_PU24E) + (u32)((u32)(MRK_VERSION_PO_PU24E) << 16);
// 4. 0x8010010 - Стринг указывающий что имеет место заголовок с версией программы
static const char VersionStr[] @ 0x8010010 = "+VERSION HEADER+";
// Вспомогательная подпрограмма для активации секции versionsection
void DummyFunc (void) @ ".versionsection"
{      
    u32 temp = (u32)(&MainStackPointer);  // Вынужденные обращения для активации переменных
    temp = ResetHandlerAddr;
    temp = PrgVersion;
    temp = VersionStr[0];

}
// ============= конец секции versionsection ===========================
//======================================================================
#else
    #warning "ВНИМАНИЕ!!! ОТКЛЮЧЕНА СЕКЦИЯ ВЕРСИИ ПРОГРАММЫ!!! ЗАГРУЗКА ДАННОЙ ВЕРСИИ В ПЧ НЕ БУДЕТ КОРРЕКТНО ЗАПУСКАТЬСЯ!!!"
#endif


// =========================== Конец файла ===========================

/////  http://supermcu.googlecode.com/svn/trunk/LPC17XX/Micrium/Software/uCOS-II/
///// http://www.2beanet.com/ucos/Source/index.html
// =========================== Конец файла ===========================
