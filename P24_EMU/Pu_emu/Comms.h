#include "windows.h"
#include "emustubs.h"

#ifndef 	COMMS_H
#define		COMMS_H

typedef enum{
    noErr   = 0,
    tymeOutErr,
    rxErr,
}errUART_type;

errUART_type txRxUart(MsgReqDrvIn_type *pntMsgIn, char *pBufOut, char *pBufIn );

bool OpenPort  ( comPortSetup_type * set);
int ClosePort ( comPortSetup_type * );
int SendPacket( comPortSetup_type *, const void *, int );
int ReceivePacket( comPortSetup_type *, void *, int , int * );

#endif	  //COMMS_H