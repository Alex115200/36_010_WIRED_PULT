#include <windows.h>
#include <stdio.h>
#include "modbus.h"
#include "logfile.h"

//#define _CRT_SECURE_NO_WARNINGS /* убрать предупреждения про sprintf */

HANDLE hlog=INVALID_HANDLE_VALUE;

int WriteHexBuf(const unsigned char * buf, int len)
{
  int i,j,cb,cbw;
  char c,str[255];

  for (i=0;i<=len;i+=16)
  {
    str[0]=0;
    cb=0;
    for (j=i;j<i+16;j++)
    {
      if (j<len) cb+=sprintf(str+cb/*,255*/,"%02X ",buf[j]);
      else cb+=sprintf(str+cb/*,255*/,"   ");
  }
    cb+=sprintf(str+cb,/*255,*/"    ");
    for (j=i;j<i+16;j++)
    {
      c=buf[j];
      if (c<32) c='.';
      if (j<len) str[cb++]=c;
      else str[cb++]=' ';
  }
    cb+=sprintf(str+cb,/*255,*/"\r\n");

    WriteFile(hlog,str,cb,&cbw,NULL);
 }
  WriteFile(hlog,"\r\n",2,&cbw,NULL);
  return 1;
}

int OpenLog()
{
  if (hlog!=INVALID_HANDLE_VALUE) return 1;
  hlog=CreateFile("modbus.log", GENERIC_WRITE, FILE_SHARE_READ, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    LogEvent(MODBUS_LOG_SYS, "Logging started");
    return 1;
}

int CloseLog()
{
  if (hlog==INVALID_HANDLE_VALUE) return 1;
    LogEvent(MODBUS_LOG_SYS, "Logging stopped");
  CloseHandle(hlog);
  return 1;
}

int CheckLog(int Level)
{
  if (ModbusLogFlags)
    OpenLog();
  else
    CloseLog();

  return (Level & ModbusLogFlags);
}

int LogEvent(int Level, const char * Event)
{
  char str[255];
  int cb;

  if (!CheckLog(Level)) return 1;
  WriteFile(hlog,str,sprintf_s(str,255,"Event: %s\r\n",Event),&cb,NULL);
  return 1;
}

int LogNum1(int Level, const char * Event, int n1)
{
  char str[255];
  int cb;

  if (!CheckLog(Level)) return 1;
  WriteFile(hlog,str,sprintf_s(str,255,"Event: %s %d\r\n",Event, n1),&cb,NULL);
  return 1;
}


int LogPacket(int Level, const char * Event, const void * buf, int len, int needlen)
{
  char str[255];
  int cb;

  if (!CheckLog(Level)) return 1;
  WriteFile(hlog,str,sprintf_s(str,255,"%s: block %d bytes (need %d)\r\n",Event,len,needlen),&cb,NULL);
  WriteHexBuf(buf,len);
  return 1;
}

