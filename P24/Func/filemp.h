
//

#ifndef     FILEMP_H
#define     FILEMP_H

#include "globalIncludes.h"
#include "fixedUst.h"

//

#define FILE_MP_FLASH_BYTES      9 * 1024    // Reserved space for MPFile in flash memory here
#define FILE_MP_FLASH_OFFSET     140 * 1024  // Maximal size of the FOP in flash memory
#define FILE_MP_READ_BYTES       128         // Read this number of bytes for one time
#define FILE_MP_WRITE_BYTES      0x4          // Write this number of bytes for one time
#define FILE_MP_HEAD_BYTES       128
#define FILE_MP_SIGN             0x0002
#define NUM_ATTEMPTS             100

#define FILE_MP_LOAD_MODE        1
#define FILE_MP_STORE_MODE       2

#define FILEMP_LINK_FULL_SIGN    49

//

//
void DataRotate(u8 *data, s32 size);

//
void ShowFileMPProgress(u16 Per, u16 FileMPNum, u16 Mode);

//
u8 SaveFileMPFromAT24(u8 FileMPNum, 
                      u8 SlotNum);

//
u8 SaveFileMPToAT24(u8 SlotNum,
                    u8 FileMPNum);

//
void UpdateMPFiles(u16 GrpPrm);
                    
#endif     // FILE_MP