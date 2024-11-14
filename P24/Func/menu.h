/**********************************************
 *                                            *
 *    (c) Copyright _Triol Corporation 2010    *
 *                                            *
 * File name   : Menu.h                       *
 * Description : Определения и структуры меню *
 *                                            *
 * Date        : 				              *
 * Author      : 				              *
 * Modified    : 				              *
 *                                            *
 **********************************************/
#ifndef     Menu_H
#define     Menu_H

// ******   ОПРЕДЕЛЕНИЯ uC-OS II   ******

// Определение, подключающее меню сохраненное на внутреннюю флеш
//#define TEST_MENU
// Определяем функционал для моделирования
#define TYPE_FUNCTIONAL     0x0100  // для насоса
// Определяем максимальное количество уровней меню
#define MAX_LEVEL_MENU      10
// Маркер для обозночения корневого меню
#define ROOT_MENU           0xFFFF
// Время индикации сообщения в сек.
#define T_MSG               3
// Количество отображаемых значений текстовых параметров
#define NUM_SHOW_TXT_PRM    7

#define MAXDRIVES           14 
#define MAXDRIVES_CUT       1 //ограничим 1 приводом 27.03.2013

#define MODE_MENU   0
#define MODE_PRM    1
#define MODE_EDIT   2

#define MODE_HandlerMainMenuPult                        0
#define MODE_HandlerMenuPultAndDrive                    1
#define MODE_HandlerMenuChangeDriveForMenuPult          2
#define MODE_HandlerMenuPultNewWithDrive                3
#define MODE_HandlerMessage                             4

#define MODE_HandlerMenuStatusReplace                   5
#define MODE_HandlerMenuStatus                          6
#define MODE_HandlerMenuChangeDriveForMenuStatusReplace 7
#define MODE_HandlerMenuJrn                             8
#define MODE_HandlerMenuSetup                           9


extern char StrOut[256];

int OSWait(int CmpStop, int NEvent);



#define GRAFBUFSIZE (YPIXELS*NBYTES)
extern u8 VideoRAM[YPIXELS*NBYTES];
#define GrafBff VideoRAM

// **************************************************************************
//                      Определения
// **************************************************************************
//                      Константы
// **************************************************************************
#define UnixConst               ((u32)(11323L*24L*3600L))     //2001-1970

#define MAX_STR_OTOBR_MENU      8
#define MAX_STR_OTOBR_PRM       4
#define ADDITIONAL_PRM          4
#define MAX_REQ_PRM             NUM_STATUS + 2 //MAX_STR_OTOBR_PRM + ADDITIONAL_PRM

#define MAX_STR_OTOBR_JRN       2

#define     COUNT_STATUS        20

#define NUM_STATUS              COUNT_STATUS + 1
#define NUM_STATUS_2            COUNT_STATUS + 2

#define  LINEadr             	(1uL<<14)     /* Признак линейного адреса */

#define ADDR_STATUS     		((44<<7) + 0)
#define GRP_STATUS      		44
#define PRM_STATUS      		0
#define PRM_STATUS_2      		2

#define TimeMig                 3          /* Время в осчетах задачи дисплея ~100 мс */
#define TimeMigSTATUS           500        /* Время в осчетах задачи дисплея ~500 мс */

#define MAX_NUM_MENU            100
#define MAX_NUM_PRM             100

// **************************************************************************
//                  Типы
// **************************************************************************
// *********************
// Типы меню
typedef enum{
    TypeVisible_ALL,
    TypeVisible_TEK,
}TypeVisible_type;

typedef union{
    f32 		f32Prm;
    u32 		u32Prm;
    s32 		s32Prm;
}PrmPult_type;

//**************************************************************************
// Структура Head Of Language
//**************************************************************************
#pragma pack(push)  // сохран. тек. значение выравнивания в стеке прекомпилера
#pragma pack(1)     // устан. значение выравнивания в 1 байт
typedef struct{
    u32                 SectSize;           //Размер секции
    u8                  Name[16];            //Название языка на русском языке
    u8                  NameLocal[16];       //Название языка на поддерживаемом языке
    u8                  Acronym[8];         //Сокращенное название языка
    u8                  CodPageAnsi[16];     //Код страницы ansi для знакогенератора
    u32                 PntTxtPrm;          //Cсылка на начало таблицы текстов параметров
    u32                 PntTxtGrp;          //Cсылка на начало таблицы текстов групп
    u32                 PntTxtMGrp;         //Cсылка на начало таблицы текстов мегагрупп
    u32                 PntTxtTxt;          //Cсылка на начало таблицы текстов текстовых параметров
    u32                 PntTxtEdIzm;        //Cсылка на начало таблицы текстов единиц измерения
    u32                 PntTxtHelp;
    u32                 PntTxtMess;
    u32                 PntMessPultMass;
    u16                 QuantityHelp;       //Длина списка Help 
    u8                  Res[2];
}HeadOfLanguage_type;

typedef struct{
    u32                 PntString;          //Размер секции
    u16                 LenString;          //Название языка на русском языке
}DescriptorStr_type;

typedef struct{
    u8                  IDMessPult;
    u16                 OfsText;            //Смищение относительно начала таблицы
    u8                  QuantityMessPult;   //Название языка на русском языке
}DescriptorMessPult_type;
#pragma pack(pop) // востанавливаем выравнивание.

typedef struct{
    PrmPult_type  *Param;
    PrmPult_type  Min;
    PrmPult_type  Max;
    char          **txtEnum;
    u32           Type;
}PrmMenuOpis_type;

// Типы меню
typedef enum{
    TypeMenu_RESERV,
    TypeMenu_MGRP_GRPS,
    TypeMenu_MPRM,
    TypeMenu_MSPEED_ACCESS,
    TypeMenu_MJRN,
    TypeMenu_MGRP_JRN,
    TypeMenu_MJRN_RECORD,
    TypeMenu_MMASTER,
}TypeMenu_type;
//Таблица 4 Структура флагов описателя меню
typedef struct{
    u16     Parole1Vis      :1;
    u16     Parole2Vis      :1;
    u16     Parole3Vis      :1;
    u16     TypeMenu        :3;
}FlagOpisMenu_type;

#pragma pack(push)  // сохран. тек. значение выравнивания в стеке прекомпилера
#pragma pack(1)     // устан. значение выравнивания в 1 байт

//Таблица 3 Структура описателя меню
typedef struct{
    u8                  IDGrp;              //Идентификатор группы
    u8                  QuantityPrmMenu;    //Количество параметров в меню.
    u32                 AdrNprm;            //Адрес номеров параметров. Указатель на начало массива с номерами параметров для текущего меню. 
    FlagOpisMenu_type   FlgMenu;            //Флаги меню. Структура битовых полей.
    //char                Name[16];         //
    u32                 OfsName;            //Указатель на строку меню
    //u16                 LenHlp;             //
    u16                 OfsHlp;             //Индекс в массиве хелпов
    u8                  res[6];             //
}DescriptorMenu_type;

#pragma pack(pop) // востанавливаем выравнивание.

//Таблица 6 Структура флагов описателя параметров
typedef struct{
    u32     TypeVar         :5;
    u32     MasterVis       :1;
    u32     MasterChange    :1;
    u32     TexnVis         :1;
    u32     TexnChange      :1;
    u32     UserVis         :1;
    u32     UserChange      :1;
    u32     Power           :3;
    u32     EditType        :3;
    u32     nonVolatile     :1;
    u32     LimChange       :1;
    u32     FormatChange    :1;
    u32     NumBase         :4;
    u32     reserved        :8;
//    u32     Func            :1;
}FlagOpisParam_type;

#pragma pack(push)  // сохран. тек. значение выравнивания в стеке прекомпилера
#pragma pack(1)     // устан. значение выравнивания в 1 байт
//Таблица 5 Структура описателя параметра
typedef struct{
    u8                      IDGrp;              //Идентификатор группы
    u8                      IDPrm;              //Идентификатор параметра
    u16                     Unit;               //Код физической величины и функция преобразования.
    FlagOpisParam_type      FlgPrm;             //Указатель на флаги параметра
    s16                     LowLim;             //нижний предел изменения параметра
    s16                     HighLim;            //Верхний предел изменения параметра
    u32                     OfsTxt;             //Указатель на массив строк для текстовых параметров
    u16                     DefaultSet;         //Значение по умолчанию
    u32                     OfsName;            //Указатель на строку с названием параметра.
    //char                    Name[16];           //Название параметра
    //u16                     LenHlp;
    u16                     OfsHlp;             //Индекс в массиве хелпов
    u16                     Functional;         //Принадлежность параметра к определенному функционалу
    u16                     AdrVal;             //    
    u16                     Visible;            //                            
    char                    res[2];
}DescriptorPrm_type;

#pragma pack(pop) // востанавливаем выравнивание.

//Таблица 1a Структура полей возвращаемых при чтении по запросу с адресом 0х8000 + адрес регистра
typedef __packed  struct{
    s16                     Value;
    u16                     StepEdIsm;
    s16                     LowLim;
    s16                     HighLim;
    u32                     Flags;
}ModbusOpisPrm2byte_type;

typedef __packed  struct{
    s32                     Value;
    u16                     StepEdIsm;
//    u16                     zer;
    s32                     LowLim;
    s32                     HighLim;
    u32                     Flags;
}ModbusOpisPrm4byte_type;


//#pragma pack(push)  // сохран. тек. значение выравнивания в стеке прекомпилера
//#pragma pack(1)     // устан. значение выравнивания в 1 байт

typedef __packed struct{
    u8                  IDGrp;
    u8                  QuantityPrmMenu;
    u16                 AdrNprm;
    FlagOpisMenu_type   FlgMenu;
    char                Name[16];
    u16                 LenHlp;
    u16                 OfsHlp;
    u8                  res[6];
}DescriptorMenuOld_type;

typedef __packed struct{
    u8                      IDGrp;
    u8                      IDPrm;
    u16                     Unit;
    FlagOpisParam_type      FlgPrm;             //Указатель на флаги параметра
    s16                     LowLim;
    s16                     HighLim;
    u16                     OfsTxt;
    u16                     DefaultSet;
    char                    Name[16];           //Название параметра
    u16                     LenHlp;
    u16                     OfsHlp;
    u16                     Functional;
    char                    res[6];
}DescriptorPrmOld_type;
//#pragma pack(pop) // востанавливаем выравнивание.


typedef struct{
    void *              BaseHOF;
    void *              BaseDP;
    DescriptorPrm_type  DP;
    wm_type             wm;
    MsgReqDrvIn_type    msgReqDrvIn;
    //MsgReqDrvOut_type   MsgReqDrvOut;
    u32                 Ready;
    u32                 NumParam;
    u16                 AdrParam;
    u8                  BufParam[20];
    u8                  AdrNet;
    u8                  SzReq;
    __packed struct {
        u8      Ignore          : 1;         //игнорировать данные, не выставлять валидность
        u8      ValidData       : 1;         //Валидные данные по модбасу
        u8      ValidDataFirst  : 1;         //Валидные данные при первом обращении к данным
        u8      ValidStruct     : 1;         //Структура с правильными параметрами
        u8      reserved        : 4;
    };
    DescriptorPrmOld_type DPOld;
}Full_DescriptorPrm_type;
//=================

typedef struct{
    Full_DescriptorPrm_type  DPDrive[MAX_REQ_PRM + 1];
}Full_MenuDescriptorPrm_type;

extern Full_MenuDescriptorPrm_type MDPDrive;

//=================

typedef enum{
    MSG_MODE_YESNO,
    MSG_MODE_POPUP,
    MSG_MODE_THROUGH,
    MSG_MODE_CANCEL
}MsgMode_enum;

typedef struct{
    const char    *Msg;
    struct{
        //u32 KeyYes  :1;         //Поддержка кнопки Да
        //u32 Through :1;         //Прохождение функции минуя нажатие кнопок
        MsgMode_enum     Mode;
    };
}Message_type;

//Продумать разруливание моего, неоднородного меню указателями!!!!!!!!!!!!!!!!!!
struct _ParamMenu_type{
    u16                             Key;
    u16                             Type;
    u16                             NetAdr;
    void                            *BaseHOF;
    const struct _ParamMenu_type    *pParamMenuPrev;
    void                            (*const pFuncNextMenu)(struct _ParamMenu_type *pParamMenuPrev);
};

typedef struct _ParamMenu_type ParamMenu_type;

//=================

typedef struct _menu_type{
    char                        *txt;
    void                        (*CallMenu)(void);
}Menu_type;

extern Menu_type MainMenu[];

//=================

typedef enum {
    testNone,
    testDisp,
    testKeys,
    testAT45,
    testFRAM,
    testModBus
}NumTest_type;

typedef struct {
    char                        *txt;
    NumTest_type                (*CallMenuTest)(void);
}TestMenu_type;

//=================

/*typedef struct {
}*/

/*typedef struct{
    u16 typeJrn;
}HandlerMenuJrn_type;*/

typedef struct {            // 34 слова
    u16 indexRec;
    u16 quantRec;
    u16 numRec;
    u16 sizeRec;
    u16 rsrvd;
    u16 Crc;
}Index_type;


typedef struct{
    u16 jrnData[MAX_STR_OTOBR_JRN][60];
}JrnData_type;

// Структура для работы с мастером настройки
typedef struct{
    //u16     	typeJrn;
    u16     	        netAdr;
    void    	        *BaseHOF;
    void    	        *BaseDM;
    DescriptorMenu_type DM;
    DescriptorMenu_type DMOld;
    u16                 getIndex;
}HandlerMenuSetup_struct;

typedef struct{
    u16     	typeJrn;
    u16     	netAdr;
    void    	*BaseHOF;
    void    	*BaseDM;
    DescriptorMenu_type DM;
    DescriptorMenu_type DMOld;
}HandlerMenuJrn_struct_type;
// ***************************************************************************
//                          Прототипы функций
// ***************************************************************************

__noreturn void HandlerMenuPult(void);
//===================================================================
//  Обработчик меню параметров пульта и параметров привода
//===================================================================
void HandlerMenuPultAndDrive(u16 Key, u8 NetAdr, u16 Type, void *HofAT45);
void HandlerMainMenuPult(u16 Key);

keyName_type WaitKey(u8 *keyPressed );

u32 MessagePopUp(const char *const str);
u32 MessageCancel(const char *const str);
u32 MessageThrough(const char *const str);
u32 MessageYesNo(const char *const str);

//u32 HandlerMessage(Message_type *Msg);
u16 Format_Message(const char *const Msg, u16 beginLine);
/*
inline void setCrashIndicator(void);
inline void clrCrashIndicator(void);
inline void setReadyIndicator(void);
inline void clrReadyIndicator(void);
inline void setWorkIndicator(void);
inline void clrWorkIndicator(void);
*/
/*==========================================================

==========================================================*/

//#ifdef  Menu_PRIVAT

//===============================================
//Определения для функции CallHandleParam
//===============================================
#define PARAM_NONE          0   //Ничего не делать
#define PARAM_OUT           1   //Неверные данные для обработки параметра
#define PARAM_WRITE         2   //Параметр записать после редактирования
//===============================================


typedef enum{
    FLASH_OPIS = 0,
    LPC_OPIS   = 1
}typeOpis_enum;

#define HANDLE_VIEW_PARAM           0
#define HANDLE_EDIT_PARAM           1
#define HANDLE_VIEW_EDIT_PARAM      2

#define INCY                16
#define MENUX               0
#define PARAMX              16

//Определение типа редактируемости
#define NO_EDIT             0
#define STOP_EDIT           1
#define ALWAYS_EDIT         2

//#define NUM_STATUS          5
extern AllPrm_Type *AllPrmPnt;
extern u16 GetModeKey(void);

typedef struct {
        u16       al;
        u16       second;
        u16       minute;
        u16       hour  ;
        u16       day   ;
        u16       date  ;
        u16       month ;
        u16       year  ;
}time_type;

//===============================================
// Тип статуса ПЧ
//===============================================
typedef union{ 
    u16     all;
    struct{
    u16 numAvar     :8; // Номер текущей аварии

    u16 localRemote :1; // МУ (0) - активный местный канал управления
                        // ДУ (1) - активный дистанционный канал управления
    u16 fNegative   :1; // F+ (0) - выходная частота имеет положительный знак, либо равна нулю
                        // F- (1) - выходная частота имеет отрицательный знак;
    u16 rundown     :1; // нет (0) - нет частотного торможения двигателя в настоящий момент;
                        // F--(1) - происходит частотное торможение двигателя в настоящий момент
    u16 speedup     :1; // нет (0) - нет разгона двигателя в настоящий момент;
                        // F++ (1) - происходит разгон двигателя
    u16 ready       :1; // Гот (0) - нет готовности к пуску ПЧ (ПЧ в состоянии аварии, либо в работе);
                        // Гот (1) - ПЧ готов к пуску;
    u16 breakdown   :1; // Авар. (0) - безаварийное состояние ПЧ;
                        // Авар. (1) - аварийное состояние ПЧ;
    u16 charge      :1; // Заряд (0) - не происходит заряд звена постоянного тока;
                        // Заряд (1) - происходит заряд звена постоянного тока;
    u16 run         :1; // ШИМ (0) - на выходе ПЧ нет напряжения;
                        // ШИМ (1) - на выходе ПЧ есть напряжение.
    }bit;
}statusDRV_tipe;

//===============================================
// Тип статуса ПЧ
//===============================================
typedef union{
    u16     all;
    struct{
    u16 breakdown   :1; // Авар. (0) - безаварийное состояние ПЧ;
                        // Авар. (1) - аварийное состояние ПЧ;
    u16 ready       :1; // Гот (0) - нет готовности к пуску ПЧ (ПЧ в состоянии аварии, либо в работе);
                        // Гот (1) - ПЧ готов к пуску;
    u16 run         :1; // ШИМ (0) - на выходе ПЧ нет напряжения;
                        // ШИМ (1) - на выходе ПЧ есть напряжение.
    u16 fNegative   :1; // F+ (0) - выходная частота имеет положительный знак, либо равна нулю
                        // F- (1) - выходная частота имеет отрицательный знак;
    u16 rundown     :1; // нет (0) - нет частотного торможения двигателя в настоящий момент;
                        // F--(1) - происходит частотное торможение двигателя в настоящий момент
    u16 speedup     :1; // нет (0) - нет разгона двигателя в настоящий момент;
                        // F++ (1) - происходит разгон двигателя
    u16 charge      :1; // Заряд (0) - не происходит заряд звена постоянного тока;
                        // Заряд (1) - происходит заряд звена постоянного тока;
    u16 stopByDC    :1; // Нет (0) - на выходе ПЧ нет постоянного тока торможения;
                        // Стоп (1) - на выходе ПЧ есть постояный ток торможения.
    u16 stopAll     :1; // Нет (0) - ПЧ не в останове;
                        // Стоп (1) - ПЧ в останове.
    u16 pickUp      :1; // Нет (0) - нет подхвата;
                        // Есть (1) - подхват.
    u16 autoTune    :1; // Нет (0) - нет автонастройки;
                        // Есть (1) - автонастойка.
    u16 fazirovka   :1; // CBA (0) - фазировка на входе;
                        // ABC (1) - фазировка на входе.
    u16 stopKey     :1; // Нет (0) - кнопка СТОП не назата;
                        // Стоп (1) - кнопка нажата.
    u16 chanalAuto  :1; // Нет (0) - автоматического выбора канала;
                        // Авто (1) - выбор канала.
    u16 autoMode    :1; // Нет (0) - ПЧ не в останове;
                        // Стоп (1) - ПЧ в останове.
    u16 syshka      :1; // Нет (0) - Происходит сушка;
                        // Есть (1) - Не происходит сушка.
    }bit;
}statusDRV_2_type;

// **************************************************************************
// Структура опесателя уровня меню
typedef struct{
    u16 						LevPos;
    u16 						LevPosPrev;
    u16 						LevPosBegin;
    u16 						LevPosBeginPrev;
    u16 						LevPosCursor;
    u16 						LevPosCursorPrev;
    u32 						CntMenu;
    u32 						LevCntMenu;
    u16                         AdrLevel;
    u16                         AdrLevelPrev;
}OpisLevel_type;

typedef struct{
    u16 						Pos;
    u16 						PosPrev;
    u16 						PosBegin;
    u16 						PosBeginPrev;
    u16 						PosCursor;
    u16 						PosCursorPrev;
    // Поля относящиеся к многоуровневой структуре
    u16                         Level;
    u16                         AdrLevelMenuFirst;
    u16                         AdrLevelMenuFirstPrev;
    u16                         AdrLevelMenu;
    u16                         AdrLevelMenuPrev;
    OpisLevel_type              OpisLevel[MAX_LEVEL_MENU];
    //--------------------------------------------
    u16 						Mode;
    u32 						CntMenu;
    u32 						CntParam;
    u32 						CntMP;
    u32 						GetOpisMenu;
    u32                         GetOpisStatus;  //Ввведена для обновления статуса при переходе между статусом и меню
    DescriptorMenu_type 		*DM;
    HeadOfFile_type 			HOF;
    Full_DescriptorPrm_type 	*pntDPMenu;
    Full_DescriptorPrm_type 	*pntDPReq;
    HeadOfFileOld_type          HOFOld;
    DescriptorMenuOld_type      *DMOld;
}HandlerMenuPultNew_struct_type;


typedef struct{
    LnkNetFDP_type 	LnkNetFDP;
    void 			*AdrFRAM[ MAXDRIVES ];
    u32 			Ypos;
    u16 			Key;
    u16 			Pos;
    u16 			PosBegin;
    u16 			PosCursor;
    u8 				MaxDrv;
    u8 				NetAdr[MAXDRIVES];
}HandlerMenuChangeDrive_struct_type;
// ***************************************************************************
//===============================================
void ch_to_smhdmy(time_type *tpnt, u32 chislo);
void ExecFunc(wm_type *wm);

void drawParamEdit(char *str);
void drawParamEditLowLim(char *str);
void drawParamEditHighLim(char *str);

u32 CallHandleParam(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, u16 TypeHandle);
//===============================================
//Нестандартные функции меню
//- главное меню
//- меню привода
//- меню пульта
//- меню выбора привода по его сетевому адресу
//===============================================
void CallMenuPult(void);
void CallMenuDrive(void);
void CallMenuStatusReplace(void);
void CallMenuKeyTest(void);


void HandlerMenuChangeDrive(u16 Key);
void HandlerMenuDrives(void *HofAT45);
u32 HandlerMessage(Message_type *Msg);
void HandlerMenuStatusReplace(u16 Key);
void HandlerMenuJrn(u16 Key);
void HandlerMenuTestKey(u16 Key);

void HandlerMenuStatus(u16 Key);

void MenuToGraf(const Menu_type *pntMenu, u8 Pos, u8 LenMenu);
void MenuToGraf    (const Menu_type *pntMenu, u8 Pos, u8 LenMenu);
void MenuParamToGraf(const Menu_type *pntMenu, char *Str);

void parseWordStatus(u16 Adr, statusDRV_tipe StatusDrv);

inline void clrAllIndicator(void);
//===============================================
//Функции чтения описателей меню и
//описателей параметров
//===============================================
void *GetOpisMenu(void *HOF, void *DM, void *BaseHOF, u16 NumMenu);
void *GetOpisParam(void *HOF, void *DM, void *DP, void *BaseHOF, u16 NumPrm);
void MenuStart(void *HOF, u16 Type, u16 PosBegin, u16 PosCursor, u16 SizeMenu);
void ParamStart(void *HOF, u16 Type, u16 PosBegin, u16 PosCursor, u16 SizeParam);
void MenuStartOneShotReadAT45(void *BaseHOF, u16 PosCursor, u16 Pos, u16 SizeMenu);
void ParamStartOneShotReadAT45(void *BaseHOF, u16 PosCursor, u16 Pos, u16 SizeParam, u16 Type);
void ParamEdit(void *BaseHOF, Full_DescriptorPrm_type *DPDrive);
u32 FindVisibleMenu(void *HOF, void *AdrHOF, u16 NumLnk, u16 NumMenu, TypeVisible_type TypeVisible);
u32 FindVisibleParam(void *HOF, void *DM, void *BaseHOF, u16 NumLnk);
u32 FindVisibleTekParam(void *HOF, void *DM, void *BaseHOF, u16 NumLnk);

void InitDP(Full_DescriptorPrm_type *DPDrive, void *BaseHOF, void *BaseDP, u8 AdrNet);
// **************************************************************************
//  инициализация без игнорирования игнорирование
// **************************************************************************
void InitDPWhithoutIgnor(Full_DescriptorPrm_type *DPDrive, void *BaseHOF, void *BaseDP, u16 Type, u8 AdrNet);
void DeInitAllDP(void);

//===============================================
//Функции просмотрщики параметров
//===============================================
void IntToStr(s32 Val, u16 dot, char *Str);
void UIntToStr(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str);
void TxtToStr(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str);
void HexToStr(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str);
void FloatToStr(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str);
void ParamToStr(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str);
void DayMonToStr(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str);
void HourMinToStr(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str);
void MinSecToStr(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str);
void UnixToStr(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str);
void RefToStr(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str);
//===============================================
//Функции редакторы
//===============================================
u32 FloatToStrEdit(void *BaseHOF, Full_DescriptorPrm_type *AdrFDP, wm_type *wm, char *str);
u32 UIntToStrEdit(void *BaseHOF, Full_DescriptorPrm_type *AdrFDP, wm_type *wm, char *str);
u32 HexToStrEdit(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str);
u32 TextToStrEdit(void *BaseHOF, Full_DescriptorPrm_type *AdrFDP, wm_type *wm, char *Str);
u32 HMorMSorMDToStrEdit(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str);
//===============================================
//Функции просмотрщики редактируемых параметров
//===============================================
u32 TestLimIntFrmtMin(const PrmMenuOpis_type *pntPrm, u32 PosDig);
u32 TestLimIntFrmtMax(const PrmMenuOpis_type *pntPrm, u32 PosDig);
void EditInt16Frmt(const PrmMenuOpis_type *pntPrm);
void EditFloatFrmt(const PrmMenuOpis_type *pntPrm);

void FloatViewEdit(wm_type *wm, char *str);
//===============================================
//Функции поиска параметра и группы
//===============================================
void *FindParam(void *HOF, void *DM, void *DP, void *BaseHOF, u8 NumParam);
void *FindGroup(void *HOF, void *DM, void *BaseHOF, u8 NumGroup);
/*======================================
    Меню настройки пульта
======================================*/
/*======================================
    Параметры пульта
======================================*/

/*======================================
    Функции помощи
======================================*/
//Вычиткание помощи 
u32 getHelp(void *BaseHOF, char *str, u16 OfsHlp);

void RazborRx_mk(void);
void ch_to_smhdmy(time_type *tpnt, u32 chislo);
/*======================================
    Работа с текущими режимами меню
======================================*/
void SetModeMenu(u32 mode);
u32 GetModeMenu(void);
//===============================================
void GrfOut(void);
//===============================================
//===================================================================
// Считывание статусного слова ПЧ и управление только индикацией
//===================================================================
void WordStatusForLed(void);
//===================================================================
// Разбор статусного слова ПЧ и управление только индикацией
//===================================================================
void parseWordStatusForLed(u16 Adr, statusDRV_tipe status);
void parseWordStatus2(u16 Adr, statusDRV_2_type status);
void InitDM(void *DMDriveDirect, u16 NumItem);
// Функция чтения значения текстового параметра
inline void ReadValueTextPrm(void *BaseHOF, void *HOF, Full_DescriptorPrm_type *DPDrive, char * Str, s32 NumPrm);
void InitAllDM(u16 cntItem);

u32 CalcPosCharInt(u32 VesDig, char *str);
u16 TestValidDrv(u8 netAdr, u16 numCurOpis);

// **************************************************************************
//  			Поиск названия для параметра
// **************************************************************************
u32 getNamePrm(void *BaseHOF, void *HOF, void *FDP, char *str);
// **************************************************************************
//  			Поиск названия для группы
// **************************************************************************
u32 getNameMenu(void *BaseHOF, void *HOF, void *FDM, char *str);
// **************************************************************************
//  			Поиск помощи для параметра
// **************************************************************************
u32 getHelpPrm(Full_DescriptorPrm_type *FDP, char *str);
// **************************************************************************
//  			Поиск смещения для языкового сектора
// **************************************************************************
void *getPntForLng(void *BaseHOF, HeadOfFile_type *HOF);
// **************************************************************************
//  			Инициализация единиц измерения
// **************************************************************************
void initEdIzm(void *BaseHOF);
// **************************************************************************
//  			Инициализация текстов пульта
// **************************************************************************
void  initMessPult(void *BaseHOF);

extern DescriptorMenuOld_type             *DMGlobalOld[MAX_NUM_MENU];             
extern DescriptorPrmOld_type              *DPGlobalOld[MAX_NUM_PRM];
extern DescriptorMenuOld_type             DMDriveOld[MAX_NUM_MENU];



#endif
