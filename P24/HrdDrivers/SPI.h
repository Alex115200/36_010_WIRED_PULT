#ifndef  Spi_H
#define  Spi_H

//===================================================================
extern OS_EVENT *AT45Sem;
//===================================================================
/*  ћаска на последние биты адресации по странице пам€ти в EEPROM  */
//===================================================================
#define MASKpageEEPROM	((u32)( ~(szPageEEPROM-1) )	)
//===================================================================
//          ќпределение битов в слове состо€ни€ SPI
//===================================================================
#define RXNE    (1<<0)
#define TXE     (1<<1)
#define BSY     (1<<7)
//===================================================================
// ќпределение команд дл€ 25LC256
//===================================================================
typedef enum{
	FramWrStatus     = 0x01,
	FramWrMem        = 0x02,
	FramRdMem        = 0x03,
	FramDisWrite     = 0x04,
	FramRdStatus     = 0x05,
	FramEnWrite      = 0x06,
}cmdEEPROM_type;
//===================================================================
typedef union{
    u8      all;
    struct{
        u8      PAGE_SIZE   :1;
        u8      PROTECT     :1;
        u8      rez         :4;
        u8      COMP        :1;
        u8      RDY_BUSY    :1;
    }bit;
}at45status_type;
//===================================================================
// € бы JDEC вычитал гденибудь по стару и раздуплил что за флеша
// набил бы структурку и работал бы не заморачива€сь 161D, 642 или еще кака€нить
#define AT45sizePage                1056//528

typedef enum{
		AT45cmdRdAr                  = 0xE8UL,
		AT45cmdWrThroughPage1NE      = 0x88UL,
		AT45cmdWrThroughPage1WE      = 0x83UL,
		AT45cmdWrBuf1                = 0x84UL,
		AT45cmdErPage                = 0x81UL,
		AT45cmdErBlock               = 0x50UL,
		AT45cmdRdSigDevice           = 0x9FUL,
		AT45cmdP1toBuff              = 0x53UL,
		AT45cmdCmpBuf1               = 0x60UL,
		AT45cmdStatusRead            = 0xD7UL,
}cmdAT45_type;
//===================================================================
//                      ѕрототипы функций
//===================================================================

void spiInitAT45_HARD(void);
void spiInitAT45(void);
void at45cmdWr(u8 cmd, void *AdrSpfl);
inline u8 RdStatusAT45(void);

u8 spflWr(void *AdrSpfl, void *AdrInData, u16 size);
u8 spflWrPg(void *AdrSpfl, void *AdrLpc, u16 size);
u8 spflRd(void *AdrLpc, void *AdrSpfl, u16 size);



u8 FM25Read(void *AdrLpc, void *AdrFRAM, u16 size);
u8 FM25Write(void *AdrFRAM, void *AdrLpc, u16 size);
u8 FM25Fill(void *AdrFRAM, u8 data, u16 size);
void spiInitFRAM_HARD(void);
void spiInitFRAM(void);

#endif  // Spi_H

