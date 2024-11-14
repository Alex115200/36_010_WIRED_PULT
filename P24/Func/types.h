#ifndef TYPES_H
#define TYPES_H


typedef enum PrmFrmt{
    sec2Frmt            ,//0   // ������� ��� ����� 4 �������� ���������  2pice
    NtxtFrmt            ,//1   // ����� ������                    1 s16
    Hex16Frmt           ,//2   // ��� int16Frmt                   2 s16
    int16Frmt           ,//3   // �������� �������� �������� 16 ���     2 s16
    HourMinFrmt         ,//4   // �����:������, ����            2 s16
    MinSecFrmt          ,//5   // �����:������, ����            2 s16
    floatFrmt           ,//6   // �������� �������� � ������� float     4 float
    int32Frmt           ,//7   // �������� �������� �������� 32 ���     4 s32
    HmsFrmt             ,//8   // ����:������:�������               4 s32
    UTimeFrmt           ,//9   // �������� ����� �� Unix          4 s32
    refFormat           ,//10  // ������:������, ��������                   2 s16
    MonDayFrmt          ,//11  // ����: ����:�����               2 s16
    uns16Frmt           ,//12  // ����������� �������� �������� 16 ���     2 s16
    uns32Frmt           ,//13  // ����������� �������� �������� 32 ���     4 u32
    kolvFormat           //14  // ���������� �������� � �������
}PrmFrmt_type;



//���� ������ � �������
typedef enum {
      Ok_err            ,       // 0
      IllegalFunc_err   ,       // 1
      BadAdr_err        ,       // 2
      Lim_err           ,       // 3
      NonRdk_err        ,       // 4

      SysPrmInitBad_err ,       // 5
      ErrPostMB         ,       // 6
      ErrPendMB         ,       // 7
      ErrSizeQ          ,       // 8
      ErrSzMB           ,       // 9
      ErrKsMB           ,       // 10
      ErrReqMB          ,       // 11
      ErrPrmBusy        ,       // 12  �������� ��� � ���������
      ErrQueueFull      ,       // 13  ������� �� �������� ������

      ErrDebug = 99     ,       // 99  ������� �� �������� ������
}Drv_err_type;


//��������� ������ �� ������ ��������
/*typedef struct{
    u8                  AdrNet;         // ������� ����� �������
    u8                  Func;           // �������
    u8                  Sz;             // ����� ������������ ������
    u8                  NumParam;       // ����� ���������
    u16                 AdrData;        // ����� ������
    u8                  *Buf;           // ��������� �� ����� ������
    Drv_err_type        Err;            // ������ �����
    struct{
        u8              Busy        :1; //
    };
}MsgReqDrvOut_type;*/

//��������� ������� �� ������� ��������
typedef struct{
    u8                  AdrNet;         // ������� ����� �������
    u8                  Func;           // �������
    u8                  Sz;             // ����� ������������� ������
    //u8                  TypeReq;      // ��� ������
    u8                  NumParam;       // ����� ���������

    u32                 AdrData;        // ����� ������
    u16                 Param;          // �������������� ���������
    u16                 SzBuf;          // ������ ������

    u8                  *Buf;           // ��������� �� ����� ������
#ifdef _PROJECT_IAR_
    u8                  SzOut;          // ����� ���������� ������ � ������
#else
    u32                 SzOut;          // ����� ���������� ������ � ������
#endif // _PROJECT_IAR_

    Drv_err_type        Err;            // ������ �����
    u8                  cntRepeat;      // ���������� �������� �������

    //    Full_DescriptorPrm_type *DPDrive;
    //    MsgReqDrvOut_type   *MsgOut;       // ��������� �� ��������� ������
    struct{
    //  u8              ValidData   :1;
        u8              CleanQueue  :1;
    };

#ifndef _PROJECT_IAR_
        u32             countByteWrite; // ���������� ������ ��� �������� �� �����
        u32             countByteRead;  // ��������� ���������� ������ � ������ �� �������
#endif // _PROJECT_IAR_

    u32              Busy;
}MsgReqDrvIn_type;

typedef enum{
    KbdStartStop_OFF,
    KbdStartStop_ON
}KbdStartStop_type;

//��������� ������� �� ������� ������ ����������
typedef struct{
    KbdStartStop_type                  KbdStateStartStop;         // ������� ����� �������
}MsgReqKbdIn_type;

struct wmStruct;
typedef int (*pFuncMosAf_typedef)(struct wmStruct *,s16);   //��������� �� �������

typedef struct{
        u16         AdrVal      ; //����� �������� (������)
        s16         LowLim      ; //�������� ������� ������� ��� ����� ���������� � �������������� ��� ����� ��������
        s16         HighLim     ; //�������� ������� ������� ��� ����� ���������� � �������������� ��� ����� ��������
        u16         StepEdIsm   ; //
        u32         Flags     ; //
        u16         LedName[4]    ; //��� ��� ������������� ������
}OpisPrm_type;

struct wmStruct{
        u32         Flags     ; //
        u16         StepEdIsm   ; //
        u16         AdrPrm      ; //
        union{
          struct{
               s32  Value     ;
               s32  LowLim      ;
               s32  HighLim     ;
              }s32;
          struct{
               _iq  Value     ;
               _iq  LowLim      ;
               _iq  HighLim     ;
              }iq;
          struct{
               float  Value     ;
               float  LowLim      ;
               float  HighLim     ;
              }f;
        };

        const OpisPrm_type  *oPnt     ;
        void        *valPnt     ;
        pFuncMosAf_typedef  mosaf     ;
        u16         kolvShort   ; //
        u16         NumBase     ;
        u16         Val2byte    ;
//        u16         TypeVar     ;
    PrmFrmt_type    TypeVar     ;
        u16         LedName[4]    ; //��� ��� ������������� ������
                struct{
                    u16             enablePrmChange :1; // ���� ��� ���������� ������ ���������� �������
                };
        };
//********************************************************
typedef struct wmStruct wm_type;
//********************************************************
//===================================================================
// ��������� ������ ��� ������ �� �������
//===================================================================
typedef struct{
    u8 width;           // ������ ������ � �������� � ������� ������
    u8 height;          // ������ ������ � �������� � ������� ������
    u8 narrowOffset;    // ������ �������� '.' ',' ':' ';' ��� ������ �� �������
    u8 normalOffset;    // ������ ���� ��������� ��������  ��� ������ �� �������
}propertyFont_type;

//================================
typedef struct{
    propertyFont_type   propertyFont;
    const u8            *ptrFont;
}font_type;
//===================================================================

typedef __packed  struct{
    u8              NetAdr;
    void            *Lnk;
    u16             Crc;
}LnkNetFDP_type;
//===================================================================

typedef struct{
    u16     functMask;                      //����� �����������
}Functional_type;
//===================================================================

typedef struct{
    Functional_type     curFunct;
    u16                 crc;
}FileFunctional_type;
//===================================================================

#endif

