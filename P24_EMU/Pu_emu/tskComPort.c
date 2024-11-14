
#include "globalIncludes.h"




void TaskComPort(void *pdata)
{
//    int             res;
    char            buf[256];
    
    int            err = 0 ;
    int			   countByteRecieve;
    
static int cnt = 0;
    
    memset(buf, 0x55, sizeof(buf) );

    while(1){
		while(err == 0){			// ќжидаем открыти€ порта
		    OSTimeDly(200);
		    err = OpenPort( &comPortSetup ) ;
		}

        while( comPortSetup.comIsInit != 0 ){
            OSTimeDly(1000);
            SendPacket( &comPortSetup, buf, 10 );
            ReceivePacket( &comPortSetup, buf, 10, &countByteRecieve );
            if( countByteRecieve !=0 ){
                cnt++;
            }
        }        

        ClosePort(&comPortSetup);
        err = 0;
    }
}

