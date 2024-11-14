//#include <win32.h>
#include "modbus.h"
#include "windows.h"

// def SWAPPED_CRC - ModBus standard crc
// undef SWAPPED_CRC - Triol bug crc

#define SWAPPED_CRC

int InvertWord(int w) {return ((w&0xff)<<8) |( (w>>8) & 0xff); }
int InvertMem(unsigned short *data, int size); //size in 16-bit words

unsigned char PackBuf[256];
//extern HANDLE CommHandle;

int CheckSum(unsigned char *data, int size);

BOOL ModbusFunction(int level, int netaddr, int fnum, void *req,
       int reqsize, void *resp, int respsize)
{
int cs,cbr;

#ifndef _PROJECT_IAR_
if (CommHandle == INVALID_HANDLE_VALUE) 
{ SetLastError(MODBUS_ENOTINIT);
  return FALSE; }
#endif  
if ((reqsize > 250) || (respsize > 250)) 
{ SetLastError(MODBUS_EMAXBUF);
  return FALSE; }
PackBuf[0] = netaddr;
PackBuf[1] = fnum;
if (req) memcpy(PackBuf+2, req, reqsize);
cs = CheckSum(PackBuf, reqsize+2);
PackBuf[reqsize+2] = cs >> 8;
PackBuf[reqsize+3] = cs & 0xff;
#ifdef _PROJECT_IAR_
// инициировать прерывание по передаче PackBuf, reqsize
//  и подождать ответа
#else
if (ModbusSendPacket(level, PackBuf, reqsize+4) == 0) return FALSE;
if (ModbusReceivePacket(level, PackBuf, respsize+4, &cbr) == 0) return FALSE;
#endif	
if (resp && (cbr > respsize+4))          // Overflow check
{ SetLastError(MODBUS_EOVERFLOW);
  return FALSE; }
if (cbr == 0)   // Check for empty reply (timeout)
{ SetLastError(MODBUS_ENOREPLY);
  return FALSE; }
if (cbr < 4)   // Reply must be 4 bytes min
{ SetLastError(MODBUS_EINVALIDREPLY);
  return FALSE; }
cs = PackBuf[cbr-1] + (PackBuf[cbr-2] << 8);    // Checksum check
if (cs != CheckSum(PackBuf, cbr-2)) 
{ SetLastError(MODBUS_ECHECKSUM);
  //wsprintf(estr, "CS err: %x<>%x ", cs, CheckSum(PackBuf, cbr-2));
  //MessageBox(NULL, estr, "Modbus.dll", MB_OK);
  return FALSE; }
if ((PackBuf[0] != netaddr) && (((PackBuf[1]^fnum) & 0x7f) != 0)) 
{ SetLastError(MODBUS_EINVALIDREPLY);
  return FALSE;  }
if (PackBuf[1] != fnum) 
{ SetLastError(MODBUS_EREFUSED);
  return FALSE; }
if (resp) memcpy(resp, PackBuf+2, cbr-4);
return TRUE;
}

//---------------------------------------------------------
int CheckSum(unsigned char *data, int size)
{
int i,j,t;
BOOL bit;

t = 0xffff;
for (i=0; i<size; i++) 
{ t ^= data[i];
  for (j=0; j<8; j++) {
    bit = t & 1;
    t >>= 1;
    t &= 0x7fff;
    if (bit) t ^= 0xa001; } }
#ifdef SWAPPED_CRC
  return ((t&0xff)<<8) | ((t&0xff00)>>8);
#else
  return t;
#endif
}

int InvertMem(unsigned short *data, int size) //size in 16-bit words
{
int i;

for (i=0; i<size; i++) data[i] = InvertWord(data[i]);
return TRUE;
}
