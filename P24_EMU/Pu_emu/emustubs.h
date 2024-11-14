#include "windows.h"
#include "global_types.h"

#ifndef 	EMUSTUBS_H
#define		EMUSTUBS_H


//===================================================================
//          ��������� ��������� Com ����� �� Exe
//===================================================================
typedef struct{
    char            portName[16];           // ��� �����
    u32             BaudRate;               // �������� ������ �� �����
    u32             nStopBit;               // ���������� ���� �����
    u32             waiteTime;              // ����� �������� ������ �� �����
    u32             parity;

    int             comIsInit;
    HANDLE 	        CommHandle;             // ���������� �����
}comPortSetup_type;

extern comPortSetup_type       comPortSetup;

void wrFlash( void *adrFlash, void *buf, int size );
void rdFlash( void *buf, void *adrFlash, int size );


#endif EMUSTUBS_H