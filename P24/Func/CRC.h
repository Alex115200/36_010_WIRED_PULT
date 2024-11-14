
#ifndef CRC_H
#define CRC_H

#include "globalIncludes.h"

//

u16 GetCrc(void *Bff, u32 szBff);
u16 CrcPice(u8 *Bff, u32 szBff, u16 CrcPrev);
u16 CrcPortion(unsigned short *Bff, unsigned short szBff, unsigned short PrevCrc);
void UpdateCRC(void);

#endif  // CRC_H
