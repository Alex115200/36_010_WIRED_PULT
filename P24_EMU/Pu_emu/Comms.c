//#include "win32.h"
//#include "modbus.h"
//#include "logfile.h"
#include "windows.h"

#include "emustubs.h"

#include "globalIncludes.h"



//====================================================================
// Передача и прием сообщения по порту
//====================================================================
errUART_type txRxUart(MsgReqDrvIn_type *pntMsgIn, char *pBufOut, char *pBufIn )
{
    ReceivePacket( &comPortSetup, pBufIn, pntMsgIn->countByteRead, &pntMsgIn->SzOut );   // прочистить порт


    SendPacket( &comPortSetup, pBufOut, pntMsgIn->countByteWrite );                      // Передать запрос

    ReceivePacket( &comPortSetup, pBufIn, pntMsgIn->countByteRead, &pntMsgIn->SzOut );   // Ожидаем ответ

    if( pntMsgIn->SzOut == 0 )
        { return tymeOutErr; }
    else if( pntMsgIn->SzOut != pntMsgIn->countByteRead )
        { return rxErr; }
    else
        { return noErr; }
}
//====================================================================
//HANDLE 	CommHandle 		= INVALID_HANDLE_VALUE;
int 	ModbusLogFlags 	= 0;
//====================================================================
// 					Открытие порта 
//====================================================================
BOOL OpenPort(comPortSetup_type *set )
{
	HANDLE 			h; 
	COMMTIMEOUTS 	tw;
	DCB 			dcb;
    int             time;

    char            portNameLong[16] = {'\\' , '\\' ,'.','\\', 0 } ;
	
    if(set->comIsInit == FALSE ){
        return FALSE;
    }

    strcat(portNameLong, set->portName);

//    h = CreateFile( portNameLong        /*set->portName*/ ,
    h = CreateFile( set->portName ,
					GENERIC_READ | GENERIC_WRITE,
					0,    /* comm devices must be opened w/exclusive-access */
					NULL, /* no security attrs */
					OPEN_EXISTING, /* comm devices must use OPEN_EXISTING */
					0,    /* overlapped I/O */
					NULL
					);  /* hTemplate must be NULL for comm devices */

	if (h == INVALID_HANDLE_VALUE){
		return FALSE;
	}

	if (GetCommTimeouts( h, &tw) == 0) {
		CloseHandle(h);
		return FALSE;
	}

    time = 1000/ set->BaudRate;
    if(time == 0) time = 1;

//    time = 2;
#if 0
    http://vsokovikov.narod.ru/New_MSDN_API/index_msdn.htm
#endif

    tw.ReadIntervalTimeout          = time;    //(40000 / set->BaudRate) + 60;  //Modbus RTU mode max. timeout (ms)
    tw.ReadTotalTimeoutMultiplier   = time;    //time;   // 10000 / set->BaudRate;
	tw.ReadTotalTimeoutConstant     = 15;       //300;
	tw.WriteTotalTimeoutMultiplier  = 0;
	tw.WriteTotalTimeoutConstant    = 0;

	if (SetCommTimeouts(h, &tw) == 0){
		CloseHandle(h);
		return FALSE;
	}

	if (GetCommState(h,&dcb) == 0) {
		CloseHandle(h);
		return FALSE;
	}
	
	dcb.fBinary         = 1;
	dcb.fDtrControl     = 0;
	dcb.fRtsControl     = RTS_CONTROL_ENABLE;
	dcb.ByteSize        = 8;
	dcb.Parity          = NOPARITY;
    dcb.StopBits        = set->nStopBit;
    dcb.BaudRate        = set->BaudRate;

	if (SetCommState(h, &dcb) == 0){
		CloseHandle(h);
		return FALSE;
	}

	set->CommHandle = h;
	return TRUE;
}
//====================================================================
// 						Закрытие порта
//====================================================================
BOOL ClosePort(comPortSetup_type *set)
{
	BOOL res = FALSE;
	if( set->CommHandle != INVALID_HANDLE_VALUE ){
	    res 		        = CloseHandle(set->CommHandle);
	    set->CommHandle 	= INVALID_HANDLE_VALUE;
    }
    return res;
}
//====================================================================
// 						Передача по потру
//====================================================================
int SendPacket( comPortSetup_type *set, const void *buf, int size)
{
	int 		countByteWrite;

	if( WriteFile( set->CommHandle, buf, size, &countByteWrite, NULL ) == 0 ){
		return 0;
	}
	return countByteWrite;
}
//====================================================================
//						Прием по порту
//====================================================================
BOOL ReceivePacket( comPortSetup_type *set, void *buf, int maxsize, int *cbr )
{
	if ( ReadFile(set->CommHandle, buf, maxsize, cbr, NULL ) == 0){
		*cbr = 0;
		return FALSE; 
	}
	
	return TRUE;
}
//====================================================================

#if 0
typedef struct _DCB {
    DWORD DCBlength;      			/* sizeof(DCB)                     		*/
    DWORD BaudRate;       			/* Baudrate at which running       		*/
    DWORD fBinary			:1;     /* Binary Mode (skip EOF check)    		*/
    DWORD fParity			:1;     /* Enable parity checking          		*/
    DWORD fOutxCtsFlow		:1; 	/* CTS handshaking on output       		*/
    DWORD fOutxDsrFlow		:1; 	/* DSR handshaking on output       		*/
    DWORD fDtrControl		:2;  	/* DTR Flow control                		*/
    DWORD fDsrSensitivity	:1; 	/* DSR Sensitivity              		*/
    DWORD fTXContinueOnXoff	:1; 	/* Continue TX when Xoff sent 			*/
    DWORD fOutX				:1;     /* Enable output X-ON/X-OFF        		*/
    DWORD fInX				:1;     /* Enable input X-ON/X-OFF         		*/
    DWORD fErrorChar		:1;  	/* Enable Err Replacement          		*/
    DWORD fNull				:1;     /* Enable Null stripping           		*/
    DWORD fRtsControl		:2;  	/* Rts Flow control                		*/
    DWORD fAbortOnError		:1; 	/* Abort all reads and writes on Error 	*/
    DWORD fDummy2			:17;  	/* Reserved                        		*/
    WORD wReserved;       			/* Not currently used              		*/
    WORD XonLim;          			/* Transmit X-ON threshold         		*/
    WORD XoffLim;         			/* Transmit X-OFF threshold        		*/
    BYTE ByteSize;        			/* Number of bits/byte, 4-8        		*/
    BYTE Parity;          			/* 0-4=None,Odd,Even,Mark,Space    		*/
    BYTE StopBits;        			/* 0,1,2 = 1, 1.5, 2               		*/
    char XonChar;         			/* Tx and Rx X-ON character        		*/
    char XoffChar;        			/* Tx and Rx X-OFF character       		*/
    char ErrorChar;       			/* Error replacement char          		*/
    char EofChar;         			/* End of Input character          		*/
    char EvtChar;         			/* Received Event character        		*/
    WORD wReserved1;      			/* Fill for now.                    	*/
} DCB, *LPDCB;



typedef struct _COMMTIMEOUTS {
    DWORD ReadIntervalTimeout;          /* Maximum time between read chars. */
    DWORD ReadTotalTimeoutMultiplier;   /* Multiplier of characters.        */
    DWORD ReadTotalTimeoutConstant;     /* Constant in milliseconds.        */
    DWORD WriteTotalTimeoutMultiplier;  /* Multiplier of characters.        */
    DWORD WriteTotalTimeoutConstant;    /* Constant in milliseconds.        */
} COMMTIMEOUTS,*LPCOMMTIMEOUTS;

typedef struct _COMMCONFIG {
    DWORD   dwSize;                 /* Size of the entire struct                */
    WORD    wVersion;               /* version of the structure                 */
    WORD    wReserved;              /* alignment                                */
    DCB     dcb;                    /* device control block                     */
    DWORD   dwProviderSubType;      /* ordinal value for identifying
                                       provider-defined data structure format   */
    DWORD   dwProviderOffset;       /* Specifies the offset of provider specific
                                       data field in bytes from the start       */
    DWORD   dwProviderSize;         /* size of the provider-specific data field */
    WCHAR   wcProviderData[1];      /* provider-specific data                   */
} COMMCONFIG,*LPCOMMCONFIG;

#endif

#if 0
//==================
//Не уверен, что существует родной класс MFC для работы с COM портом... Я бы посоветовал воспользоваться функциями WinAPI.

//Для получения списка имеющихся в системе COM портов в том числе и виртуальных можно воспользоваться считыванием информации из реестра. С функциями API это выглядит так:

HKEY hKey;
LONG lResult;

DWORD typeValue;
TCHAR data[MAX_PATH] = TEXT("aa");
CHAR dataChar[MAX_PATH] = "aa";
DWORD MaxData = sizeof(data);

DWORD BufferSize = TOTALBYTES;
PPERF_DATA_BLOCK PerfData = (PPERF_DATA_BLOCK) malloc( BufferSize );

TCHAR achValue[MAX_VALUE_NAME]; 
DWORD cchValue = MAX_VALUE_NAME; 


TCHAR  achClass[MAX_PATH] = TEXT(""); // buffer for class name 
DWORD  cchClassName = MAX_PATH; // size of class string 
DWORD  cValues;       // number of values for key 
DWORD  cchMaxValue;     // longest value name 
DWORD  cbMaxValueData;    // longest value data 
DWORD  cbSecurityDescriptor; // size of security descriptor 
FILETIME ftLastWriteTime;   // last write time 

lResult  = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), NULL, KEY_READ, &hKey);
if (lResult != ERROR_SUCCESS) 
{
	if (lResult == ERROR_FILE_NOT_FOUND) {
		MessageBoxEx(hwnd, TEXT("Ключ реестра не найден"), TEXT("Ошибка"), MB_ICONERROR| MB_OK, NULL);
		return false;
	} 
	else {
		MessageBoxEx(hwnd, TEXT("Ошибка открытия ключа реестра"), TEXT("Ошибка"), MB_ICONERROR| MB_OK, NULL);
		return false;
	}
}

// Get the class name and the value count. 
lResult = RegQueryInfoKey(
	hKey,          // key handle 
	achClass,        // buffer for class name 
	&cchClassName,      // size of class string 
	NULL,          // reserved 
	NULL,//&cSubKeys,        // number of subkeys 
	NULL,//&cbMaxSubKey,      // longest subkey size 
	NULL,//&cchMaxClass,      // longest class string 
	&cValues,        // number of values for this key 
	&cchMaxValue,      // longest value name 
	&cbMaxValueData,     // longest value data 
	&cbSecurityDescriptor,  // security descriptor 
	&ftLastWriteTime);    // last write time 

if (cValues) 
{
	for (unsigned int i=0, lResult=ERROR_SUCCESS; i<cValues; i++) 
	{ 
		cchValue = MAX_VALUE_NAME; 
		achValue[0] = '\0'; 
		lResult = RegEnumValue(hKey, i, 
				achValue, 
				&cchValue, 
				NULL, 
				&typeValue,
				(BYTE*)data,
				&MaxData);
		if (lResult == ERROR_SUCCESS ) 
		{ 
			//В переменной data имеем имя COM порта
			//...
		} 
	}
}

RegCloseKey(hKey);



//После того, как имя COM порта получено, дальнейшая работа с ним происходит как с файлом. Но для начала нам надо его настроить

HANDLE Port;   //Дескриптор COM-порта
//Открываем COM-порт
Port = CreateFile(data, //Имя COM-порта
           GENERIC_READ|GENERIC_WRITE, 
           FILE_SHARE_READ, 
           NULL,
           OPEN_EXISTING, 
           FILE_ATTRIBUTE_NORMAL, 
           NULL);
if (Port == INVALID_HANDLE_VALUE) 
{
	MessageBox(NULL, TEXT("Невозможно открыть последовательный порт"), TEXT("Error"), MB_OK);
	break;
}

//Получаем состояние управляющей структуры COM-порта, 
//если не удалось выводим сообщение об ошибке и выходим из 
//обработчика
COMMCONFIG comm;
GetCommState(Port, &comm.dcb);

//Настраиваем управляющую структуру COM-порта
comm.dcb.ByteSize = 8;        //Размер байта
comm.dcb.Parity = NOPARITY;    //Паритет отключен
comm.dcb.StopBits = ONESTOPBIT;//Один стоповый бит
//Можно также воспользоваться стандартным диалоговым
//окном настройки управляющей структуры COM-порта:
CommConfigDialog(data,NULL,&comm);

//Применяем настроенную структуру к COM-порту, если не 
//удалось выводим сообщение об ошибке 
if(!SetCommState(Port, &comm.dcb)) 
{
	MessageBox(NULL, TEXT("Невозможно сконфигурировать последовательный порт"), TEXT("Error"), MB_OK);
	CloseHandle(Port);
	break;
}

//Получаем текущие настройки тайм-аутов COM-порта
COMMTIMEOUTS commTimeouts;
GetCommTimeouts(Port, &commTimeouts);

//Перенастраиваем тайм-ауты: 
//Максимальный интервал чтения в миллисекундах между 
//двумя принимаемыми символами
commTimeouts.ReadIntervalTimeout = 100;
//Константа в миллисекундах используемая для вычисления 
//полного тайм-аута операции чтения
commTimeouts.ReadTotalTimeoutConstant = 300;
//Множитель используемый для вычисления полного тайм-аута
//операции чтения в миллисекундах
commTimeouts.ReadTotalTimeoutMultiplier = 50;
//Полный максимальный тайм-аут операции чтения 
//вычисляется следующим образом //ReadTotalTimeoutConstant + (ReadIntervalTimeout * количество считываемых байт)

//Устанавливаем тайм-ауты для COM-порта
if(!SetCommTimeouts(Port,&commTimeouts))
{
	MessageBox(NULL, TEXT("Невозможно настроить тайм-ауты последовательного порта"), TEXT("Error"), MB_OK);
	CloseHandle(Port);
	break;
}


Передача данных по COM-порту выглядит следующим образом:

DWORD feedback;
LONG lResult; //Помещаем сюда количество данных, которые
// необходимо передать
if((!WriteFile(Port, &data[0], lResult*sizeof(data[0]), &feedback, 0) || feedback != lResult*sizeof(data[i]))) 
{
	CloseHandle(Port);
	Port = INVALID_HANDLE_VALUE;
}


При передаче мы проверяем количество переданных байт, и вообще удалось ли выполнить передачу.

Чтение данных можно реализовать следующим образом (лучше это делать по таймеру):

DWORD feedback = 0;
int i=0;
TCHAR ct;
//Попытка чтения первого символа хранящегося в COM-порте
ReadFile(Port, &ct, sizeof(ct), &feedback, NULL);
if(feedback>0)
{
//Если чтение удалось продолжаем чтение пока не встретится символ #13
	do
	{
		data[i] = ct;
		ReadFile(Port, &ct, sizeof(ct), &feedback, NULL);
		i++;
	}
	while(feedback>0);
	data[i] = '\0';
}


//И закрываем порт после работы:

if(Port != INVALID_HANDLE_VALUE)
{    //Если порт открыт
	CloseHandle(Port);
}

//==========================

http://msdn.microsoft.com/ru-ru/library/windows/desktop/aa363257.aspx
http://msdn.microsoft.com/ru-ru/library/windows/desktop/aa363194.aspx
	
#endif