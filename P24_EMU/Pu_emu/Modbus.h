/* Header for modbus.dll */
#ifndef __MODBUS_H
#define __MODBUS_H

#include <windows.h>
#include "err_id.h"

extern int ModbusLogFlags;
/* Log flags */
#define MODBUS_LOG_SYS    	0x01
#define MODBUS_LOG_LOW    	0x02
#define MODBUS_LOG_RDVAL  	0x04
#define MODBUS_LOG_RDVALSE  0x08
#define MODBUS_LOG_RDLIM  	0x10
#define MODBUS_LOG_RDFILE  	0x20
#define MODBUS_LOG_WRVAL  	0x40
#define MODBUS_LOG_WRFILE  	0x80
#define MODBUS_LOG_LOWTMP  	0x100


// Low-level port functions
BOOL  ModbusOpenPort(const char * PortName, int BaudRate);
BOOL  ModbusClosePort(void);
BOOL  ModbusSendPacket(int level, const void *buf, int size);
BOOL  ModbusReceivePacket(int level, void *buf, int maxsize, int *cbr);
// Modbus functions
BOOL   ModbusFunction(int level, int netaddr, int fnum, void *req, int reqsize,
      void *resp, int respsize);

#endif
