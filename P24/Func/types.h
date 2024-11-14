#ifndef TYPES_H
#define TYPES_H


typedef enum PrmFrmt{
    sec2Frmt            ,//0   // старшие два байта 4 байтного параметра  2pice
    NtxtFrmt            ,//1   // номер текста                    1 s16
    Hex16Frmt           ,//2   // как int16Frmt                   2 s16
    int16Frmt           ,//3   // знаковое числовое значение 16 бит     2 s16
    HourMinFrmt         ,//4   // время:минуты, часы            2 s16
    MinSecFrmt          ,//5   // время:минуты, часы            2 s16
    floatFrmt           ,//6   // числовое значение в формате float     4 float
    int32Frmt           ,//7   // знаковое числовое значение 32 бит     4 s32
    HmsFrmt             ,//8   // часы:минуты:секунды               4 s32
    UTimeFrmt           ,//9   // линейное время по Unix          4 s32
    refFormat           ,//10  // ссылка:группа, параметр                   2 s16
    MonDayFrmt          ,//11  // дата: день:месяц               2 s16
    uns16Frmt           ,//12  // беззнаковое числовое значение 16 бит     2 s16
    uns32Frmt           ,//13  // беззнаковое числовое значение 32 бит     4 u32
    kolvFormat           //14  // количество форматов в приводе
}PrmFrmt_type;



//Коды ошибок в приводе
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
      ErrPrmBusy        ,       // 12  Параметр еще в обработке
      ErrQueueFull      ,       // 13  Очередь от драйвера забита

      ErrDebug = 99     ,       // 99  Очередь от драйвера забита
}Drv_err_type;


//Структура ответа из уровня драйвера
/*typedef struct{
    u8                  AdrNet;         // Сетевой адрес запроса
    u8                  Func;           // Функция
    u8                  Sz;             // Объем возвращаемых данных
    u8                  NumParam;       // Номер параметра
    u16                 AdrData;        // Адрес данных
    u8                  *Buf;           // Указатель на буфер данных
    Drv_err_type        Err;            // Ошибка связи
    struct{
        u8              Busy        :1; //
    };
}MsgReqDrvOut_type;*/

//Структура запроса на уровень драйвера
typedef struct{
    u8                  AdrNet;         // Сетевой адрес запроса
    u8                  Func;           // Функция
    u8                  Sz;             // Объем запрашиваемых данных
    //u8                  TypeReq;      // Тип данных
    u8                  NumParam;       // Номер параметра

    u32                 AdrData;        // Адрес данных
    u16                 Param;          // Дополнительные параметры
    u16                 SzBuf;          // Размер буфера

    u8                  *Buf;           // Указатель на буфер обмена
#ifdef _PROJECT_IAR_
    u8                  SzOut;          // Объем вычитанных данных в байтах
#else
    u32                 SzOut;          // Объем вычитанных данных в байтах
#endif // _PROJECT_IAR_

    Drv_err_type        Err;            // Ошибка связи
    u8                  cntRepeat;      // Количество повторов запроса

    //    Full_DescriptorPrm_type *DPDrive;
    //    MsgReqDrvOut_type   *MsgOut;       // Указатель на сообщение ответа
    struct{
    //  u8              ValidData   :1;
        u8              CleanQueue  :1;
    };

#ifndef _PROJECT_IAR_
        u32             countByteWrite; // Количество данных для передачи по порту
        u32             countByteRead;  // Ожидаемое количество данных в ответе от привода
#endif // _PROJECT_IAR_

    u32              Busy;
}MsgReqDrvIn_type;

typedef enum{
    KbdStartStop_OFF,
    KbdStartStop_ON
}KbdStartStop_type;

//Структура запроса на уровень задачи клавиатуры
typedef struct{
    KbdStartStop_type                  KbdStateStartStop;         // Сетевой адрес запроса
}MsgReqKbdIn_type;

struct wmStruct;
typedef int (*pFuncMosAf_typedef)(struct wmStruct *,s16);   //указатель на функцию

typedef struct{
        u16         AdrVal      ; //адрес значения (СОНОДМ)
        s16         LowLim      ; //значение нижнего предела или номер переменной в микропрограмме где лежит значение
        s16         HighLim     ; //значение нижнего предела или номер переменной в микропрограмме где лежит значение
        u16         StepEdIsm   ; //
        u32         Flags     ; //
        u16         LedName[4]    ; //имя для светодиодного пульта
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
        u16         LedName[4]    ; //имя для светодиодного пульта
                struct{
                    u16             enablePrmChange :1; // Флаг для блокировки записи параметров задания
                };
        };
//********************************************************
typedef struct wmStruct wm_type;
//********************************************************
//===================================================================
// Описатель шрифта для вывода на дисплей
//===================================================================
typedef struct{
    u8 width;           // Ширина шрифта в пикселях в массиве шрифта
    u8 height;          // Высота шрифта в пикселях в массиве шрифта
    u8 narrowOffset;    // Ширина символов '.' ',' ':' ';' при выводе на дисплей
    u8 normalOffset;    // Ширина всех остальных символов  при выводе на дисплей
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
    u16     functMask;                      //Маска функционала
}Functional_type;
//===================================================================

typedef struct{
    Functional_type     curFunct;
    u16                 crc;
}FileFunctional_type;
//===================================================================

#endif

