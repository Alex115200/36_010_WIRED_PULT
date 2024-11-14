#ifndef __MANCHESTER_H__
#define __MANCHESTER_H__

inline u8 Manchester_Decode(unsigned short encoded_data);
void ManchesterDecode(void *OutPtr, void *InPtr, u16 size );
void ManchesterEncode(void *OutPtr, void *InPtr, u16 size );

#endif
