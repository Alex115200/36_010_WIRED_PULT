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
/* 0x0004*/    float              Fict_Float;
/* 0x0008*/    s32                NET00008;
/* 0x000C*/    s32                Fict_S32;
/* 0x0010*/    s16                ADD_DRIVE;
/* 0x0012*/    s16                ADD_DRIVE_ADR;
/* 0x0014*/    s16                ADR_DRV_UST_READ;
/* 0x0016*/    s16                ADR_DRV_UST_WRITE;
/* 0x0018*/    s16                ADR_FOR_LOCAL;
/* 0x001A*/    s16                EN_UST_READ;
/* 0x001C*/    s16                EN_UST_WRITE;
/* 0x001E*/    s16                ETALON_MASTER;
/* 0x0020*/    s16                ETALON_TEXN;
/* 0x0022*/    s16                ETALON_USER;
/* 0x0024*/    s16                KONTRAST;
/* 0x0026*/    s16                NET;
/* 0x0028*/    s16                NET00000;
/* 0x002A*/    s16                NET00010;
/* 0x002C*/    s16                NUM_UST_DRV_WRITE;
/* 0x002E*/    s16                NUM_UST_PULT_WRITE;
/* 0x0030*/    s16                PASSW_MASTER;
/* 0x0032*/    s16                PASSW_TEXN;
/* 0x0034*/    s16                PASSW_USER;
/* 0x0036*/    s16                SPEED;
/* 0x0038*/    s16                T_MSG;
/* 0x003A*/    s16                USTDRIVES;
/* 0x003C*/    s16                Fict_S16_1;
/* 0x003E*/    s16                Fict_S16_2;
/* 0x0040*/    s16                Fict_S16_3;
 
// распределение kolvState
 
 } AllPrm_Type;
// 68
#endif //Pult_RAM_h
