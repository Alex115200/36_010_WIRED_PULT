/**********************************************
 *                                            *
 *    (c) Copyright Triol Corporation 2012    *
 *                                            *
 * File name   : emustubs.c                   *
 * Description : Модуль файловой эмуляции     *
 *    внешней памяти и привода                *
 *                                            *
 * Date        :                              *
 * Author      :                              *
 * Modified    :                              *
 *                                            *
 **********************************************/

#define _WIN32_WINNT  0x0501
#include    <windows.h>
#include    <winbase.h>
#include    <mmsystem.h>

#include    <stdio.h>
#include    <stdlib.h>
#include    <assert.h>


#include "globalIncludes.h"

#include "OS.h"
#include "syscall.h"


u8 UartBufRx[UART1_BUFFER_SIZE];

//***************************************************************************


int spflWr(void *AdrSpfl, void *AdrLpc, unsigned short skolko);

//**************************************************************************

typedef     u8     graphBufer_type[YPIXELS*XPIXELS/8];
//***********************************************************************
typedef void (__stdcall *voidcb_type)(void);
extern keyState_type       	KeyState; 
extern LEDState_type		lampState;
void __stdcall OS_init(void);
void __stdcall EmuClose(void);


comPortSetup_type       comPortSetup;
//===================================================================
// Обменная структура между DLL и EXE
//===================================================================
typedef struct{ 
    int                 Size;           // На всякий случай размер
    voidcb_type         RunProc;        // Процедура запуска операционки
    voidcb_type         Cleanup;        // Останов операционки и уборка мусора

    keyState_type       *pKeyState;     // Клавиатура из EXE
    LEDState_type		*pLampState;    // Индикаторы в  EXE
    graphBufer_type     *pGraphBufer;   // Буфер дисплея в EXE 
    comPortSetup_type   *pComPortSetup; // Настройки СОМ порта из ЕХЕ
} IFaceGUI_type;

IFaceGUI_type   IFaceGUI;
//**************************************************************************
FILE *es_fspi = NULL;
FILE *es_fi2c = NULL;

HANDLE KeyEvent = INVALID_HANDLE_VALUE;
//****************************************************************************

//***********************************************************************
extern u8 VideoRAM_Copy[YPIXELS*XPIXELS/8];

int __declspec(dllexport) __stdcall InitDllIFace(void)
{
    IFaceGUI.Size           = sizeof(IFaceGUI);
    IFaceGUI.RunProc        = OS_init; // EmuRunProcess;
    IFaceGUI.Cleanup        = EmuClose;

    IFaceGUI.pKeyState      = &KeyState;
    IFaceGUI.pLampState		= &LEDState;
    IFaceGUI.pGraphBufer    = &VideoRAM_Copy;
    IFaceGUI.pComPortSetup  = &comPortSetup;

    IFaceGUI.pComPortSetup->comIsInit  = FALSE;
    IFaceGUI.pComPortSetup->CommHandle = INVALID_HANDLE_VALUE;

    return (int)&IFaceGUI;
}
//***********************************************************************

/*
unsigned int RandomGen(void)
{
    return (unsigned int)((rand() << 16) | rand());
}
*/
//****************************************************************************
//===================================================================
//===================================================================
//===================================================================
//              Образ флэши в ОЗУ 
//===================================================================
#define     szFlash     (1056 * 8192)   /* Размер флэши в байтах */

static  spfl_type  flashMirror ;


int         isFlashChanged;      // Признак сохранить в файл при закрытии
//===================================================================
//  ЭМУЛЯЦИЯ ФЛЭШ-ПАМЯТИ БОЛЬШОГО ОБЪЕМА  AT45DB161D  
//  - РАБОТА С ФАЙЛОМ "SPFL.BIN"
//===================================================================

//===================================================================
// Открытие файла и копирование в ОЗУ. Если файла нет, то форматирование 
// ОЗУ.
//===================================================================
int openFlash(void)
{
    errno_t     err;
    FILE        *flash;

    err = fopen_s(&flash, "spfl.bin", "r"); 

    if( err ){
        memset( &flashMirror, 0xFF, sizeof(flashMirror));    // Форматируем память
        if(flash != NULL){
            fclose(flash);
        }
        isFlashChanged = 1;      // Признак сохранить в файл при закрытии
        return 1;
    }
    else{
        fread( &flashMirror, sizeof(char), sizeof(flashMirror), flash );
        if(flash != NULL){
            fclose(flash);
        }
        isFlashChanged = 0;
        return 0;
    }
}
//===================================================================
// При закрытии программы сохраняет образ флэши в файл
//===================================================================
int closeFlash(void)
{
    errno_t     err;
    FILE        *flash = NULL;

    u32         sz = sizeof(flashMirror);

    if(isFlashChanged){
        err = fopen_s( &flash, "spfl.bin", "w+b" );
        if(!err){
            fwrite( &flashMirror, 1, sz, flash );
            fflush(es_fspi);
            if(flash != NULL){
                fclose(flash);
            }
            return 0;
        }
    }
    return 1;
}
//===================================================================
//===================================================================
void wrFlash( void *adrFlash, void *buf, int size )
{
    u64    adrF; 
    
    if( ((u64)adrFlash >= 0) && ((u64)adrFlash<= 0x80) ){
        size++; size--;
    }

    adrF = (u64)(&flashMirror) + (u64)adrFlash; // Привести adrFlash к адресу во флэши

    memcpy((void*)adrF, buf, size );
}
//===================================================================
// 
//#define  nv     (*(nv_type*)0)

//===================================================================
void rdFlash( void *buf, void *adrFlash, int size )
{
    u64    adrF; 
    
    adrF = (u64)(&flashMirror) + (u64)adrFlash; // Привести adrFlash к адресу во флэши
    memcpy( buf, (char*)adrF,  size );
}
//**************************************************************************
//  ЭМУЛЯЦИЯ ФЛЭШ-ПАМЯТИ БОЛЬШОГО ОБЪЕМА  AT45DB161D  
//  - РАБОТА С ФАЙЛОМ "SPFL.BIN"
//**************************************************************************
// стирание страниц
//  =>* StartPage - номер начальной страницы
//      KolvPage  - количество страниц (подряд)
//  *=> 1 - OK, 0 - error
//****************************************************************************
int SpflErase(int StartPage, int KolvPage)
{
unsigned char bff[FLASHBYTES];

int adr = StartPage * FLASHBYTES;
memset(bff, 0xFF, FLASHBYTES);
for (; KolvPage>0; KolvPage--)
{ if (spflWr((void *)adr, bff, FLASHBYTES) == 0) return 0;
  adr += FLASHBYTES; }
return 1;
}

//****************************************************************************
// открытие файла "spfl.bin"
//  *=> 1 - OK, 0 - error
//**************************************************************************
int es_OpenSpfl(void)
{
    int         i, buf;
    errno_t     err;

    if (es_fspi){
        return 1;
    }

    err = fopen_s(&es_fspi, "spfl.bin", "r+b");
    if (es_fspi == NULL){ 
        err = fopen_s(&es_fspi, "spfl.bin", "w+b");
        if (es_fspi == NULL){ 
            return 0;
        }
        
        buf = 0xFF;
        for ( i=0; i < FLASHBYTES*4096/4; i++ ){ 
            fwrite(&buf, 4, 1, es_fspi); 
        }
    }
    return es_fspi ? 1 : 0;
}

//**************************************************************************
// закрытие файла "spfl.bin"
//  *=> 1 - OK, 0 - error
//**************************************************************************
int es_CloseSpfl(void)
{
    if (es_fspi == NULL){ 
        return 0;
    }
    fclose(es_fspi);
    return 1;
}

//**************************************************************************
// чтение из файла "spfl.bin"
//  =>* buf  - адрес буфера памяти
//      adr  - смещение в файле
//      size - сколько байтов читать
//  *=> 1 - OK, 0 - error (файл не открыт)
//**************************************************************************
int es_ReadSpfl(void *buf, void* adr, int size)
{
    if (es_OpenSpfl() == 0){ 
        return 0;
    }
    fseek(es_fspi, (long)adr, SEEK_SET);
    fread( buf, size, 1, es_fspi );

    return 1;
}

//**************************************************************************
// запись в файл "spfl.bin"
//  =>* buf  - адрес буфера памяти
//      adr  - смещение в файле
//      size - сколько байтов читать
//  *=> 1 - OK, 0 - error (файл не открыт)
//**************************************************************************
int es_WriteSpfl(void *buf, void *adr, int size)
{
    if (es_OpenSpfl() == 0) 
        return 0;
    fseek(es_fspi, (long)adr, SEEK_SET);
    fwrite(buf, size, 1, es_fspi);
    fflush(es_fspi);
    
    return 1;
}

//**************************************************************************
// чтение массива из файла "spfl.bin"
//  =>* AdrLpc  - адрес буфера памяти
//      AdrSpfl - смещение в файле
//      skolko  - сколько байтов читать
//  *=> 1 - OK, 0 - error (файл не открыт)
//****************************************************************************
int spflRd(void *AdrLpc, void *AdrSpfl, unsigned short skolko)
{
    if (es_OpenSpfl() == 0) return 0;
    es_ReadSpfl(AdrLpc, AdrSpfl, skolko);

    return 1;
}

//**************************************************************************
// запись массива в файл "spfl.bin"
//  =>* AdrLpc  - адрес буфера памяти
//      AdrSpfl - смещение в файле
//      skolko  - сколько байтов читать
//  *=> 1 - OK, 0 - error (файл не открыт)
//**************************************************************************
int spflWr(void *AdrSpfl, void *AdrLpc, unsigned short skolko)
{
    if (es_OpenSpfl() == 0) return 0;
    es_WriteSpfl(AdrLpc, AdrSpfl, skolko);
    return 1;
}

//**************************************************************************
// чтение байта из файла "spfl.bin"
//  =>* adr - смещение в файле
//  *=> прочитанное значение
//**************************************************************************
unsigned char ldrb_spfl(unsigned char *adr)
{
unsigned char val;

spflRd(&val, adr, 1);
return val;
}

//**************************************************************************
//  ЭМУЛЯЦИЯ ОПЕРАТИВНОЙ ФЛЭШ-ПАМЯТИ   FM25L256B
//  - РАБОТА С ФАЙЛОМ "I2C.BIN"
//**************************************************************************
// открытие файла "i2c.bin"
//  *=> 1 - OK, 0 - error
//**************************************************************************
int es_OpenI2c(void)
{
int i, buf;
errno_t e;

if (es_fi2c) return 1;
e = fopen_s(&es_fi2c, "i2c.bin", "r+b");
if (es_fi2c == 0) 
{ e = fopen_s(&es_fi2c, "i2c.bin", "w+b");
  if (es_fi2c == 0) return 0;
  buf = 0;
  for (i=0; i<32768/4; i++) fwrite(&buf, 4, 1, es_fi2c); }
return es_fi2c ? 1 : 0;
}

//**************************************************************************
// закрытие файла "i2c.bin"
//  *=> 1 - OK, 0 - error
//**************************************************************************
int es_CloseI2c(void)
{
if (es_fi2c == 0) return 0;
fclose(es_fi2c);
return 1;
}

//**************************************************************************
// чтение из файла "i2c.bin"
//  =>* buf  - адрес буфера памяти
//      adr  - смещение в файле
//      size - сколько байтов читать
//  *=> 1 - OK, 0 - error (файл не открыт)
//**************************************************************************
int es_ReadI2c(void *buf, void *adr, int size)
{
    if (es_OpenI2c() == 0)                      return 0;
    if (fseek(es_fi2c, (long)adr, SEEK_SET))    return 0;
    if (fread(buf, size, 1, es_fi2c) == 0)      return 0;
    return 1;
}

//**************************************************************************
// запись в файл "i2c.bin"
//  =>* buf  - адрес буфера памяти
//      adr  - смещение в файле
//      size - сколько байтов читать
//  *=> 1 - OK, 0 - error (файл не открыт)
//**************************************************************************
int es_WriteI2c(void *buf, void* adr, int size)
{
    if (es_OpenI2c() == 0)                      return 0;
    if (fseek(es_fi2c, (long) adr, SEEK_SET))   return 0;
    if (fwrite(buf, size, 1, es_fi2c) == 0)     return 0;
    fflush(es_fi2c);

    return 1;
}

//**************************************************************************
// чтение массива из файла "i2c.bin"
//  =>* kuda   - адрес буфера памяти
//      otkuda - смещение в файле
//      skolko - сколько байтов читать
//  *=> 1 - OK, 0 - error (файл не открыт)
//**************************************************************************
int i2cRd(void *kuda, void *otkuda, unsigned short skolko)
{
    return es_ReadI2c(kuda, otkuda, skolko);
}

//**************************************************************************
// запись массива в файл "i2c.bin"
//  =>* kuda   - смещение в файле
//      otkuda - адрес буфера памяти
//      skolko - сколько байтов читать
//  *=> 1 - OK, 0 - error (файл не открыт)
//**************************************************************************
int i2cWr(void *kuda, void *otkuda, unsigned short skolko)
{
return es_WriteI2c(otkuda, kuda, skolko);
}

//**************************************************************************
// чтение байта из файла "i2c.bin"
//  =>* adr - смещение в файле
//  *=> прочитанное значение
//**************************************************************************
unsigned char ldrb_i2c(unsigned char *adr)
{
    unsigned char val = 0;

    es_ReadI2c(&val, adr, 1);

    return val;
}

//**************************************************************************
// чтение short-слова из файла "i2c.bin"
//  =>* adr - смещение в файле
//  *=> прочитанное значение
//**************************************************************************
unsigned short ldrh_i2c(unsigned short *adr)
{
    unsigned short val = 0;

    es_ReadI2c(&val, adr, 2);

    return val;
}

//**************************************************************************
// чтение int-слова из файла "i2c.bin"
//  =>* adr - смещение в файле
//  *=> прочитанное значение
//**************************************************************************
unsigned int ldr_i2c(unsigned int *adr)
{
    int val = 0;

    es_ReadI2c(&val, adr, 4);

    return val;
}

//**************************************************************************
// запись байта в файл "i2c.bin"
//  =>* adr  - смещение в файле
//      Data - значение для записи
//  *=> 1 - OK, 0 - error (файл не открыт)
//**************************************************************************
int strb_i2c(unsigned char *adr, unsigned char Data)
{
    return es_WriteI2c(&Data, adr, 1);
}

//**************************************************************************
// запись short-слова в файл "i2c.bin"
//  =>* adr  - смещение в файле
//      Data - значение для записи
//  *=> 1 - OK, 0 - error (файл не открыт)
//**************************************************************************
int strh_i2c(unsigned short *adr, unsigned short Data)
{
    return es_WriteI2c(&Data, adr, 2);
}

//**************************************************************************
// запись int-слова в файл "i2c.bin"
//  =>* adr  - смещение в файле
//      Data - значение для записи
//  *=> 1 - OK, 0 - error (файл не открыт)
//**************************************************************************
int str_i2c(unsigned int *adr, unsigned int Data)
{
    return es_WriteI2c(&Data, adr, 4);
}

//**************************************************************************
//****************************************************************************
int OSWait(int CmpStop, int NEvent)
{
    int res; 

    if (NEvent == chKey_event){ 
        res = WaitForSingleObject(KeyEvent, CmpStop);
        if (res == WAIT_OBJECT_0) return 3;
        else return 0;
    }
    else{
        Sleep(CmpStop);
        return 0;
    }
}
//**************************************************************************
//***********************************************************************
extern HANDLE       hScheduleThread, hInterruptThread;

extern volatile HANDLE  hTaskThread[OS_LOWEST_PRIO + 2];		//Map uCOS tasks to WIN32 threads

//===================================================================
//              Закрытие трэдов в uCOS
//===================================================================
void __stdcall EmuClose(void)
{
    int     i;

    for (i=0; i < OS_LOWEST_PRIO + 2; i++){
        if (hTaskThread[i]){
            TerminateThread(hTaskThread[i], 0);     // Закрыть трэды с задачами
	        CloseHandle(hTaskThread[i]);
	    }
        hTaskThread[i]=NULL;
	}
    TerminateThread( hScheduleThread, 0 );          // Закрыть служебные трэды операционки
    TerminateThread( hInterruptThread, 0 );
    
    ClosePort( &comPortSetup );                     // Закрыть сом порт
    closeFlash();                                   // Закрыть флэш и записать в файл
}
//***********************************************************************
