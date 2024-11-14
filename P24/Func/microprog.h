#ifndef MICROPROG_H
#define MICROPROG_H

//===================================================================
/**************************************************************************/
#define READ_MCU_BLOCK 0x50  // Блок читаемых данных из ПЧ при прямой загрузке в FLASH

#define FLASH_OPIS_START_ADDR   ADDR_FLASH_SECTOR_10   /* Start @ of user Flash area */
#define FLASH_OPIS_END_ADDR     ADDR_FLASH_SECTOR_11   /* End @ of user Flash area */

// *!* пока адресацию выполнить так, а после удаления всех упоминаний AT45RD стартовый адрес в HandlerMenuStatus_struct.LnkNetFDP.Lnk - поставить равным FLASH_OPIS_START_ADDR
// *!* и сбросить STM32_FLASH_OFFSET в ноль, либо убрать все его упоминания.
#define STM32_FLASH_OFFSET (FLASH_OPIS_START_ADDR-sizeof(DescrLoad_type)) // смещение адресов для FOP при переходе от адресации AT45 к адресам STM32


/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

//===================================================================
#define NO_PASSW        0
#define USER_PASSW      1
#define TEXN_PASSW      2
#define MASTER_PASSW    3

#define ADDR_ETALON_USER    ((43<<7) + 1)
#define ADDR_ETALON_TEXN    ((43<<7) + 3)
#define ADDR_ETALON_MASTER  ((43<<7) + 5)

#define ADDR_USER    ((43<<7) + 0)
#define ADDR_TEXN    ((43<<7) + 2)
#define ADDR_MASTER  ((43<<7) + 4)

#define GRP_PASSWORD            43
#define PRM_PASSWORD_USER       0
#define PRM_PASSWORD_TEXN       2
#define PRM_PASSWORD_MASTER     4

#define ADDR_LANGUAGE    ((50<<7) + 0)
//===================================================================
// Флаги ошибок загрузки Flash памяти STM
typedef enum{
    LoadError_ERROR,
    LoadError_ERROR_CONNECTION,
    LoadError_OK,
}LoadError_type;
//===================================================================

extern u32 LevelAccess;

//===================================================================
//===================================================================
void ResetPassword(void);
void Microprog(void * pHOF);
u8 ReadLnk(LnkNetFDP_type *pLnkNetFDP, u8 Num);
void InstallDrv(void);
void Password(void);

//Стирание флеши контроллера
LoadError_type EraseMCUFlash(void);
// Возвращает номер сектора флэш-памяти STM
uint32_t GetSector(uint32_t Address);
// Запись массива в Flash-память STM
LoadError_type WriteMCUFlash( void *pFlashAddr, void *pData, u16 size );

#endif
