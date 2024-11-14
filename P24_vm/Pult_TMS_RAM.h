#ifndef Pult_RAM_h
#define Pult_RAM_h
 
typedef enum{ 
  DD1            =  0          //AND-2
}nameFuncExe_type; 
 
  #define defB_0                             16
  #define defB_1                             17
  #define defB_NET00016                      18
  #define defB_NET00017                      19
  #define defNet_Fict_bit                    20
 
 typedef struct {
               u16  Net_Fict1_bit                 : 16 ;   //--
               u16  B_0                           : 1  ;   //16
               u16  B_1                           : 1  ;   //17
               u16  NET00016                      : 1  ;   //18
               u16  NET00017                      : 1  ;   //19
               u16  Net_Fict_bit                  : 12  ;   //20
/* 0x0002*/    float              Fict_Float;
/* 0x0004*/    s32                NET00008;
/* 0x0006*/    s32                Fict_S32;
/* 0x0008*/    s16                ADD_DRIVE;
/* 0x0009*/    s16                ADD_DRIVE_ADR;
/* 0x000A*/    s16                ADR_DRV_UST_READ;
/* 0x000B*/    s16                ADR_DRV_UST_WRITE;
/* 0x000C*/    s16                ADR_FOR_LOCAL;
/* 0x000D*/    s16                EN_UST_READ;
/* 0x000E*/    s16                EN_UST_WRITE;
/* 0x000F*/    s16                ETALON_MASTER;
/* 0x0010*/    s16                ETALON_TEXN;
/* 0x0011*/    s16                ETALON_USER;
/* 0x0012*/    s16                KONTRAST;
/* 0x0013*/    s16                NET;
/* 0x0014*/    s16                NET00000;
/* 0x0015*/    s16                NET00010;
/* 0x0016*/    s16                NUM_UST_DRV_WRITE;
/* 0x0017*/    s16                NUM_UST_PULT_WRITE;
/* 0x0018*/    s16                PASSW_MASTER;
/* 0x0019*/    s16                PASSW_TEXN;
/* 0x001A*/    s16                PASSW_USER;
/* 0x001B*/    s16                SPEED;
/* 0x001C*/    s16                T_MSG;
/* 0x001D*/    s16                USTDRIVES;
/* 0x001E*/    s16                Fict_S16_1;
/* 0x001F*/    s16                Fict_S16_2;
/* 0x0020*/    s16                Fict_S16_3;
 
// распределение kolvState
 
 } AllPrm_Type;
// 34
#endif //Pult_RAM_h
