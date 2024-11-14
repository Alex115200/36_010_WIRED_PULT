#ifndef DbgTim_H
#define DbgTim_H



#define QNT_MEAS_EVENT      10


#define BLoff( )                                            \
{                                                           \
    GPIOB->BSRRH = ( 1 << 0 );  /* —бросить ногу B0 */      \
    GPIOB->MODER &= ~0x03 ;                                 \
    GPIOB->MODER |=  0x01 ;                                 \
}


#define BLon( )                                             \
{                                                           \
    GPIOB->MODER &= ~0x03 ;                                 \
    GPIOB->MODER |=  0x02 ;                                 \
}

typedef struct{
    u32     measTim;
    u32     measMin;
    u32     measMax;
    u32     beginTime;
}MeasTimeDbg_type;

extern MeasTimeDbg_type MeasTimeDbg[QNT_MEAS_EVENT];

void TIM5_Debug_Init(void);
void BeginMeasEvent(u8 numEvent);
void EndMeasEvent(u8 numEvent);

#define DEBUG_P24       1

#if DEBUG_P24 > 0
    void debugStop(void);
#else
    #define debugStop() ;
#endif // DEBUG_P24

#endif
