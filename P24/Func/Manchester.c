
#include "globalIncludes.h"

//====================================================================
//              ������ ��� ����������� ����������
//====================================================================
const u8 ManchesterBitCode[16] = {
    0x55, 0x56, 0x59, 0x5A,
    0x65, 0x66, 0x69, 0x6A,
    0x95, 0x96, 0x99, 0x9A,
    0xA5, 0xA6, 0xA9, 0xAA
};
//===================================================================
const u8 mTblDecode[256] = {
    #include "manTblDecode.h"
};
//===================================================================
#define  Manchester_Decode( data ) ((u8)(mTblDecode[(u8)(data)] + (mTblDecode[(u8)((data)>>8)] << 4 )))
//====================================================================

//====================================================================
//  ��������������� ������� � ������
//====================================================================
void ManchesterDecode(void *OutPtr, void *InPtr, u16 size )
{
    u8         *inPtr;
    u8          *outPtr;

    inPtr   = (u8*)InPtr;
    outPtr  = (u8*)OutPtr;

    for( ; size; size--){              // ��������� ����� �������
        *outPtr++ = *inPtr++;
    }
}
//====================================================================
inline u16 Manchester_Encode( u8 data )
{
    return ( (ManchesterBitCode[data>>4]<<8) | ManchesterBitCode[data&0x0f] );
}
//====================================================================
//  ��������������� ������� � ������
//====================================================================
void ManchesterEncode(void *OutPtr, void *InPtr, u16 size )
{
    u8          *inPtr;
    u8         *outPtr;

    inPtr   = (u8*)InPtr;
    outPtr  = (u8*)OutPtr; //!��, ��� ����� ������������

    for( ; size; size--){              // ��������� ����� �������
        *outPtr++ = *inPtr++;
    }
}
//====================================================================
