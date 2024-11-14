#ifndef _ANYFUNC_H
#define _ANYFUNC_H

#ifdef _PROJECT_IAR_

    #define AT45WR(AdrAT45, AdrLPC, SZ)     spflWr(AdrAT45, AdrLPC, SZ)
    #define AT45RD(AdrLPC, AdrAT45, SZ)     spflRd(AdrLPC, AdrAT45, SZ)


    #define FM25WR(AdrFM25, AdrLPC, SZ)     FM25Write(AdrFM25, AdrLPC, SZ); UpdateCRC()
    #define FM25RD(AdrLPC, AdrFM25, SZ)     FM25Read(AdrLPC, AdrFM25, SZ)

    #define RazborRx()     RazborRx_mk()

#else

    int es_WriteSpfl(void *buf, void *adr, int size);
    int es_ReadSpfl( void *buf, void *adr, int size);

    int es_WriteI2c(void *buf, void *adr, int size);
    int es_ReadI2c(void *buf, void *adr, int size);


//    #define AT45WR(AdrAT45, AdrLPC, SZ)     es_WriteSpfl(AdrAT45, AdrLPC, SZ);
//    #define AT45RD(AdrLPC, AdrAT45, SZ)     es_ReadSpfl(AdrLPC, AdrAT45, SZ);

    #define AT45WR(AdrAT45, AdrLPC, SZ)     wrFlash( AdrAT45, AdrLPC, SZ )
    #define AT45RD(AdrLPC, AdrAT45, SZ)     rdFlash( AdrLPC, AdrAT45, SZ )

    #define FM25WR(AdrFM25, AdrLPC, SZ)     es_WriteI2c(AdrLPC, AdrFM25, SZ);
    #define FM25RD(AdrLPC, AdrFM25, SZ)     es_ReadI2c(AdrLPC, AdrFM25, SZ);

    #define RazborRx()     RazborRx_mk()


#if DEBUG_P24 > 0
    void debugStop(void);
#else
    #define debugStop() ;
#endif // DEBUG_P24

#endif  // _PROJECT_IAR_
#endif  // _ANYFUNC_H
