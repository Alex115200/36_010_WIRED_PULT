#ifndef MakeOpis_H
#define MakeOpis_H

//===================================================================
#define TYPE_PULT  		0
#define TYPE_DRIVE  	1

#define SzParamPult     100

#define kolvShort_RAM_MICROPROG   4096

#define LINEadr             (1uL<<14)   /* Признак линейного адреса параметра*/
#define EXT_OPIS_PRM_ADR    (1uL<<15)   /* Признак расширенного описателя    */
//===================================================================
typedef u16 *(* pFuncMicro_type)(const u16 *, u32);
//===================================================================

extern const        u16 MicroProgTabl[];
extern const        u16 PultProgTabl[];

extern u32              RamArea[SzParamPult];
extern void             *FileOpisParam;
extern const            pFuncMicro_type  TblMicroProgFunc[];
//===================================================================
//описатель функции в *.dll таблице
//===================================================================
typedef struct{
        char        		*SymbolName   	;	// Имя элемента  "MUX8"
        char        		*Prefix     	;	// Префикс   "DD"
        const char      	*typePin    	;	// указатель на таблицу прототипа
        pFuncMicro_type   	FuncPnt     	;	// указатель на функцию
        u16         		kolvPin     	;	// количество ножек у графического символа
        u16         		sizeofState   	;	// размер ОЗУ используемый функцией для хранения внутренних состояний
        u16         		sizeofNVState 	;	// объём энергонезависимой памяти используемый функцией
        u16         		Exe       		;	// =1 блок является исполняемым
        u16         		sizeofInputStruct;  // размер входной структуры
        u16         		al        		; 	// не используется
        }OpisFuncMicroProg_type;
//********************************************************
#define OpisFuncBlock(SymbolName, Prefix, typePin, FuncPnt, kolvPin, sizeofState, sizeofNVState, Exe) {SymbolName, Prefix, typePin, (pFuncMicro_type)FuncPnt, kolvPin, sizeofState, sizeofNVState, Exe}
#define OpisFuncBlockNew(SymbolName, Prefix, typePin, FuncPnt, kolvPin, sizeofState, sizeofNVState, Exe, sizeofInputStruct) {SymbolName, Prefix, typePin, (pFuncMicro_type)FuncPnt, kolvPin, sizeofState, sizeofNVState, Exe, sizeofInputStruct}
//********************************************************
typedef struct{
        u16         PntNumOpisPrm ;
        u16         kolvPrmInGrp  ;
        u16         LedName[4]    ; //имя для светодиодного пульта
        }OpisGrp_type;
//********************************************************
//Структура микропрограммы. Лежит во внутренней флеше МК. Не содержит текстов названий
//СОНМ    - смещение относительно начала микропрограммы
//СОНОДМ  - смещение относительно начала области данных микропрограммы
typedef struct{
        u32         CrcMicroprog  ; //контрольная сумма микропрограммы
        u16         SignatCoreMicro ; //Требуемая сигнатура ядра для работы с микропрограммой
        u16         szFlash     ; //Размер всей этой структуры с закоментированными полями
//указатели на секции в области ОЗУ микропрограммы (выровненные по 4 байта)
        u16         kolvRam_All   ; //общий оъём ОЗУ требуемый для микропрограммы в char
        u16         kolvNvRam_All ; //общий оъём NV ОЗУ требуемый для микропрограммы в char

        u16         Pnt_float_Sect  ; //Указатель на float секцию (СОНОДМ)
        u16         Pnt_s32_Sect  ; //Указатель на s32 секцию (СОНОДМ)
        u16         Pnt_s16_Sect  ; //Указатель на s16 секцию (СОНОДМ)

        u16         Sz_bit_Sect   ; //размер bit секции (СОНОДМ)
//
        u16         PntRam_Mb_0x05  ; //указатель на массив переменных 0x05 команды Modbus
        u16         kolv_Mb_0x05  ; //количество структур

        u16         PntConst_float  ; //секция const float (СОНМ)
        u16         kolvConst_float ; //количество const float

        u16         PntConst_s32  ; //секция const s32 (СОНМ)
        u16         kolvConst_s32 ; //количество const s32

        u16         PntConst_s16  ; //секция const s16 (СОНМ)
        u16         kolvConst_s16 ; //количество const s16

        u16         Reserv[16]    ; //

        u16         PntOpisGrp    ; //указатель на описатели групп
        u16         kolvGrp     ; //количество описателей групп

        u16         PntOpisUst    ; //описатели усатвок (СОНМ)
        u16         kolvUst     ; //количество уставок

        u16         PntOpisTek    ; //описатели текущих параметров (СОНМ)
        u16         kolvTek     ; //количество текущих параметров

        u16         PntMicroProg  ; //указатель на тело микропрограммы (СОНМ)
        u16         kolvFuncMicroProg;  //количество функций в микропрограмме

        u16         PntPwrValUst  ; //значения уставок после прошивки (СОНМ)
//---новое
        u16         CRC16_NV    ; //контрольная сумма файла энергонезависимой памяти (сигнатура)

        u16         PntModbus_0x2B  ; //указатель на тело команды 0x2B
        u16         kolvModbus_0x2B ; //размер тела 0x2B

      }MicroProg_type;

//********************************************************
typedef struct{
    const MicroProg_type  	*PntMicroProg 		; //
    const OpisGrp_type    	*PntOpisGrp   		; //
    const OpisPrm_type    	*PntOpisUst   		; //
    const OpisPrm_type    	*PntOpisTek   		; //
    u16           			*PntBase    		; //
    u32           			ramPnt      		; //указатель на ОЗУ микропрограммы приведенный к u32
    const u16     			*CodeMicroProg  	; //
    u16           			kolvGrpOpis   		;
    u16           			kolvPrmOpis   		;

    s16           			Base[16]    		;
    s16           			kolvState   		;
    u32           			cntCall     		;
    u32           			cntCallStop   		; //пошаговая отладка
    u16           			NumFuncStop   		; //пошаговая отладка
    u16           			NumFunc     		;
    u16           			kolvFunc    		; //количество функций в микропрограмме
}MPOpis_type;
//********************************************************
typedef struct{
    u16   val;                // Текущее состояние
    u16   mask;
    u16   state;              // Состояние катушек
    u16   rezerv;             // Резерв для выравнивания
}StateFuncMODBUS_0X05_type;
//********************************************************
//********************************************************
#define GetNumGrpFromAdrPrm(AdrPrm)     ((AdrPrm>>7)&0x7F)
#define GetNumPrmFromAdrPrm(AdrPrm)     (AdrPrm&0x7F)
#define DoAdrPrm(Ngrp, Nprm)            ((Nprm&0x7F)|((Ngrp&0x7F)<<7))

#define AddAdr(arg1, arg2)              ((void *)((u32)arg1 + (u32)arg2))
//********************************************************
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX/
#ifdef  DEFINE_CommonMicro_GLOBAL
    #define CommonMicro_GLOBAL
#else
    #define CommonMicro_GLOBAL extern
#endif
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX/

    CommonMicro_GLOBAL  MPOpis_type     MPOpis;
//    CommonMicro_GLOBAL  u16             flagIsFirstCall;
//    CommonMicro_GLOBAL  u16             CrcProgFlash;

//********************************************************
typedef enum{
    NV_to_RAM,
    RAM_to_NV,
    PWR_to_RAM
}UstCopy_enum;
//****************************************************************************

//********************************************************
/*
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
*/
//**************************************************************************
enum    EdIzm {
    bezRasm             ,//0     //
    mVRasm              ,//1     //мВ
    VRasm               ,//2     //В
    kVRasm              ,//3     //кВ
    mARasm              ,//4     //мА
    ARasm               ,//5     //А
    kARasm              ,//6     //кА
    HzRasm              ,//7     //Гц
    kHzRasm             ,//8     //кГц
    VARasm              ,//9     //ВА
    kVARasm             ,//10    //кВА
    WRasm               ,//11    //Вт
    kWRasm              ,//12    //кВт
    mOmRasm             ,//13    //мОм
    OmRasm              ,//14    //Ом
    kOmRasm             ,//15    //кОм
    MGOmRasm            ,//16    //МOм
    mkHLRasm            ,//17    //мкГн
    mHLRasm             ,//18    //мГн
    mkSRasm             ,//19    //мкс
    mSRasm              ,//20    //мс
    SRasm               ,//21    //сек
    minRasm             ,//22    //мин
    HourRasm            ,//23    //ч
    mmRasm              ,//24    //мм
    cmRasm              ,//25    //см
    dmRasm              ,//26    //дм
    mRasm               ,//27    //м
    obMinRsm            ,//28    //об/мин
    CelsRasm            ,//29    //°С
    ProcRasm            ,//30    //%
    mKvRasm             ,//31    //м2
    mKubRasm            ,//32    //м3
    mKubPminRasm        ,//33    //м2/мин
    mKubPhRasm          ,//34    //м3/ч
    opisRasm            ,//35    //опис
    atmRasm             ,//36    //атм
    litrRasm            ,//37    //л
    gRasm               ,//38    //g
    HzSRasm             ,//39    //Гц/с
    VSRasm              ,//40    //В/с
    ProcSRasm           ,//41    //%/с
    kmRasm              ,//42    //км
    m_S2Rasm            ,//43    //м/с2
    kPaRasm             ,//44    //кПа
    MPaRasm             ,//45    //МПа
    kg_cm2Rasm          ,//46    //кг/см2
    psiRasm             ,//47    //psi
    FarengRasm          ,//48    //°F
    kubSecRasm          ,//49    //куб/с
    kW_hRasm            ,//50    //кВт/ч
    MW_hRasm            ,//51    //МВт/ч
    VpKm_hRasm          ,//52    //В/км
    MWar_hRasm          ,//53    //Мвар/ч
    rad_hRasm           ,//54    //рад
    rad_sec_hRasm       ,//55    //рад/с
    rad_sec2_hRasm      ,//56    //рад/с2
    kg_m2_hRasm         ,//57    //кг/м2
    Vb_hRasm            ,//58    //Вб
    Nm_hRasm            ,//59    //Нм
    grad_hRasm          ,//60    //град
    Hn_hRasm            ,//61    //Гн
    _1_sec_hRasm        ,//62    //1/с
    kg_Rasm             ,//63    //кг
    mm2_Rasm            ,//64    //мм2
    kg_m3Rasm           ,//65    //кг/м3
    bar_Rasm            ,//66    //бар
    Vb_Rasm             ,//67    //Вб
    bit_Rasm            ,//68    //бит
    kolvoEdIsm           //69
};
//********************************************************
//              Прототипы функций
//********************************************************

int MicroProgWrCoil(s16 Adr, s16 val);
//********************************************************
u32 MicroProg_Init(u32 ramPnt, const MicroProg_type *mPnt);
//void CoreMicroProg(u32 ramPnt, MPOpis_type  *mPnt);
//********************************************************

Drv_err_type ModbusMakeOpis(Full_DescriptorPrm_type *DPDrive, u16 Rd);
Drv_err_type GetFlagsPrm(wm_type *wm);
Drv_err_type MakeOpis(wm_type *wm, s16 Rd, u16 Type);
Drv_err_type MOspecial(wm_type *wm, s16 Rd);
void StartExtMem(void);
int Ust_Copy(UstCopy_enum Ust_src_to_dest);
////u16 KolvoShort(DescriptorPrm_type *DP);

u16 swapU16(u16 Val);
u32 swapU32(u32 Val);
//********************************************************
#endif
