#include "windows.h"
#include "global_types.h"

#ifndef 	EMUSTUBS_H
#define		EMUSTUBS_H


//===================================================================
//          Параметры настройки Com порта из Exe
//===================================================================
typedef struct{
    char            portName[16];           // Имя порта
    u32             BaudRate;               // Скорость обмена по порту
    u32             nStopBit;               // Количество стоп битов
    u32             waiteTime;              // Время ожидания ответа по порту
    u32             parity;

    int             comIsInit;
    HANDLE 	        CommHandle;             // Дескриптор порта
}comPortSetup_type;

extern comPortSetup_type       comPortSetup;

void wrFlash( void *adrFlash, void *buf, int size );
void rdFlash( void *buf, void *adrFlash, int size );


#endif EMUSTUBS_H