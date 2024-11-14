
#include "globalIncludes.h"

//====================================================================
//              Массив для кодирования манчестера
//====================================================================
const u8 ManchesterBitCode[16] = {
    0x55, 0x56, 0x59, 0x5A,
    0x65, 0x66, 0x69, 0x6A,
    0x95, 0x96, 0x99, 0x9A,
    0xA5, 0xA6, 0xA9, 0xAA
};
//====================================================================
// Декодирование из кода манчестер в бинарный код
//====================================================================
inline u8 Manchester_Decode(unsigned short encoded_data)
{
	u8			decoded_data;
	u16			mask;

	decoded_data    = 0;
	encoded_data   &= 0x5555;		// Убрать лишние биты из манчестера
	mask            = 0x4000;
	
	do{
		decoded_data <<= 1;
		if( (encoded_data & mask) == 0 ){
			decoded_data |= 0x01 ;
		}
		mask >>= 2;
	}while( mask != 0x0000 );

	return (u8)decoded_data;
}
//====================================================================
//  Перекодирование массива в массив
//====================================================================
void ManchesterDecode(void *OutPtr, void *InPtr, u16 size )
{
    u16         *inPtr;
    u8          *outPtr;

    inPtr   = (u16*)InPtr;
    outPtr  = (u8*)OutPtr;

    for( ; size; size--){              // Формируем пакет запроса
        *outPtr++ = Manchester_Decode(*inPtr);
        inPtr++;
    }
}
//====================================================================
inline u16 Manchester_Encode( u8 data )
{
    return ( (ManchesterBitCode[data>>4]<<8) | ManchesterBitCode[data&0x0f] );
}
//====================================================================
//  Перекодирование массива в массив
//====================================================================
void ManchesterEncode(void *OutPtr, void *InPtr, u16 size )
{
    u8          *inPtr;
    u16         *outPtr;

    inPtr   = (u8*)InPtr;
    outPtr  = (u16*)OutPtr;

    for( ; size; size--){              // Формируем пакет запроса
        *outPtr++ = Manchester_Encode(*inPtr);
        inPtr++;
    }
}
//====================================================================
