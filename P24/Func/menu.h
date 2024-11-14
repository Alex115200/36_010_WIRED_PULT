/**********************************************
 *                                            *
 *    (c) Copyright _Triol Corporation 2010    *
 *                                            *
 * File name   : Menu.h                       *
 * Description : ����������� � ��������� ���� *
 *                                            *
 * Date        : 				              *
 * Author      : 				              *
 * Modified    : 				              *
 *                                            *
 **********************************************/
#ifndef     Menu_H
#define     Menu_H

// ******   ����������� uC-OS II   ******

// �����������, ������������ ���� ����������� �� ���������� ����
//#define TEST_MENU
// ���������� ���������� ��� �������������
#define TYPE_FUNCTIONAL     0x0100  // ��� ������
// ���������� ������������ ���������� ������� ����
#define MAX_LEVEL_MENU      10
// ������ ��� ����������� ��������� ����
#define ROOT_MENU           0xFFFF
// ����� ��������� ��������� � ���.
#define T_MSG               3
// ���������� ������������ �������� ��������� ����������
#define NUM_SHOW_TXT_PRM    7

#define MAXDRIVES           14 
#define MAXDRIVES_CUT       1 //��������� 1 �������� 27.03.2013

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
//                      �����������
// **************************************************************************
//                      ���������
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

#define  LINEadr             	(1uL<<14)     /* ������� ��������� ������ */

#define ADDR_STATUS     		((44<<7) + 0)
#define GRP_STATUS      		44
#define PRM_STATUS      		0
#define PRM_STATUS_2      		2

#define TimeMig                 3          /* ����� � ������� ������ ������� ~100 �� */
#define TimeMigSTATUS           500        /* ����� � ������� ������ ������� ~500 �� */

#define MAX_NUM_MENU            100
#define MAX_NUM_PRM             100

// **************************************************************************
//                  ����
// **************************************************************************
// *********************
// ���� ����
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
// ��������� Head Of Language
//**************************************************************************
#pragma pack(push)  // ������. ���. �������� ������������ � ����� ������������
#pragma pack(1)     // �����. �������� ������������ � 1 ����
typedef struct{
    u32                 SectSize;           //������ ������
    u8                  Name[16];            //�������� ����� �� ������� �����
    u8                  NameLocal[16];       //�������� ����� �� �������������� �����
    u8                  Acronym[8];         //����������� �������� �����
    u8                  CodPageAnsi[16];     //��� �������� ansi ��� ���������������
    u32                 PntTxtPrm;          //C����� �� ������ ������� ������� ����������
    u32                 PntTxtGrp;          //C����� �� ������ ������� ������� �����
    u32                 PntTxtMGrp;         //C����� �� ������ ������� ������� ���������
    u32                 PntTxtTxt;          //C����� �� ������ ������� ������� ��������� ����������
    u32                 PntTxtEdIzm;        //C����� �� ������ ������� ������� ������ ���������
    u32                 PntTxtHelp;
    u32                 PntTxtMess;
    u32                 PntMessPultMass;
    u16                 QuantityHelp;       //����� ������ Help 
    u8                  Res[2];
}HeadOfLanguage_type;

typedef struct{
    u32                 PntString;          //������ ������
    u16                 LenString;          //�������� ����� �� ������� �����
}DescriptorStr_type;

typedef struct{
    u8                  IDMessPult;
    u16                 OfsText;            //�������� ������������ ������ �������
    u8                  QuantityMessPult;   //�������� ����� �� ������� �����
}DescriptorMessPult_type;
#pragma pack(pop) // �������������� ������������.

typedef struct{
    PrmPult_type  *Param;
    PrmPult_type  Min;
    PrmPult_type  Max;
    char          **txtEnum;
    u32           Type;
}PrmMenuOpis_type;

// ���� ����
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
//������� 4 ��������� ������ ��������� ����
typedef struct{
    u16     Parole1Vis      :1;
    u16     Parole2Vis      :1;
    u16     Parole3Vis      :1;
    u16     TypeMenu        :3;
}FlagOpisMenu_type;

#pragma pack(push)  // ������. ���. �������� ������������ � ����� ������������
#pragma pack(1)     // �����. �������� ������������ � 1 ����

//������� 3 ��������� ��������� ����
typedef struct{
    u8                  IDGrp;              //������������� ������
    u8                  QuantityPrmMenu;    //���������� ���������� � ����.
    u32                 AdrNprm;            //����� ������� ����������. ��������� �� ������ ������� � �������� ���������� ��� �������� ����. 
    FlagOpisMenu_type   FlgMenu;            //����� ����. ��������� ������� �����.
    //char                Name[16];         //
    u32                 OfsName;            //��������� �� ������ ����
    //u16                 LenHlp;             //
    u16                 OfsHlp;             //������ � ������� ������
    u8                  res[6];             //
}DescriptorMenu_type;

#pragma pack(pop) // �������������� ������������.

//������� 6 ��������� ������ ��������� ����������
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

#pragma pack(push)  // ������. ���. �������� ������������ � ����� ������������
#pragma pack(1)     // �����. �������� ������������ � 1 ����
//������� 5 ��������� ��������� ���������
typedef struct{
    u8                      IDGrp;              //������������� ������
    u8                      IDPrm;              //������������� ���������
    u16                     Unit;               //��� ���������� �������� � ������� ��������������.
    FlagOpisParam_type      FlgPrm;             //��������� �� ����� ���������
    s16                     LowLim;             //������ ������ ��������� ���������
    s16                     HighLim;            //������� ������ ��������� ���������
    u32                     OfsTxt;             //��������� �� ������ ����� ��� ��������� ����������
    u16                     DefaultSet;         //�������� �� ���������
    u32                     OfsName;            //��������� �� ������ � ��������� ���������.
    //char                    Name[16];           //�������� ���������
    //u16                     LenHlp;
    u16                     OfsHlp;             //������ � ������� ������
    u16                     Functional;         //�������������� ��������� � ������������� �����������
    u16                     AdrVal;             //    
    u16                     Visible;            //                            
    char                    res[2];
}DescriptorPrm_type;

#pragma pack(pop) // �������������� ������������.

//������� 1a ��������� ����� ������������ ��� ������ �� ������� � ������� 0�8000 + ����� ��������
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


//#pragma pack(push)  // ������. ���. �������� ������������ � ����� ������������
//#pragma pack(1)     // �����. �������� ������������ � 1 ����

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
    FlagOpisParam_type      FlgPrm;             //��������� �� ����� ���������
    s16                     LowLim;
    s16                     HighLim;
    u16                     OfsTxt;
    u16                     DefaultSet;
    char                    Name[16];           //�������� ���������
    u16                     LenHlp;
    u16                     OfsHlp;
    u16                     Functional;
    char                    res[6];
}DescriptorPrmOld_type;
//#pragma pack(pop) // �������������� ������������.


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
        u8      Ignore          : 1;         //������������ ������, �� ���������� ����������
        u8      ValidData       : 1;         //�������� ������ �� �������
        u8      ValidDataFirst  : 1;         //�������� ������ ��� ������ ��������� � ������
        u8      ValidStruct     : 1;         //��������� � ����������� �����������
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
        //u32 KeyYes  :1;         //��������� ������ ��
        //u32 Through :1;         //����������� ������� ����� ������� ������
        MsgMode_enum     Mode;
    };
}Message_type;

//��������� ������������ �����, ������������� ���� �����������!!!!!!!!!!!!!!!!!!
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

typedef struct {            // 34 �����
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

// ��������� ��� ������ � �������� ���������
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
//                          ��������� �������
// ***************************************************************************

__noreturn void HandlerMenuPult(void);
//===================================================================
//  ���������� ���� ���������� ������ � ���������� �������
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
//����������� ��� ������� CallHandleParam
//===============================================
#define PARAM_NONE          0   //������ �� ������
#define PARAM_OUT           1   //�������� ������ ��� ��������� ���������
#define PARAM_WRITE         2   //�������� �������� ����� ��������������
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

//����������� ���� ���������������
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
// ��� ������� ��
//===============================================
typedef union{ 
    u16     all;
    struct{
    u16 numAvar     :8; // ����� ������� ������

    u16 localRemote :1; // �� (0) - �������� ������� ����� ����������
                        // �� (1) - �������� ������������� ����� ����������
    u16 fNegative   :1; // F+ (0) - �������� ������� ����� ������������� ����, ���� ����� ����
                        // F- (1) - �������� ������� ����� ������������� ����;
    u16 rundown     :1; // ��� (0) - ��� ���������� ���������� ��������� � ��������� ������;
                        // F--(1) - ���������� ��������� ���������� ��������� � ��������� ������
    u16 speedup     :1; // ��� (0) - ��� ������� ��������� � ��������� ������;
                        // F++ (1) - ���������� ������ ���������
    u16 ready       :1; // ��� (0) - ��� ���������� � ����� �� (�� � ��������� ������, ���� � ������);
                        // ��� (1) - �� ����� � �����;
    u16 breakdown   :1; // ����. (0) - ������������ ��������� ��;
                        // ����. (1) - ��������� ��������� ��;
    u16 charge      :1; // ����� (0) - �� ���������� ����� ����� ����������� ����;
                        // ����� (1) - ���������� ����� ����� ����������� ����;
    u16 run         :1; // ��� (0) - �� ������ �� ��� ����������;
                        // ��� (1) - �� ������ �� ���� ����������.
    }bit;
}statusDRV_tipe;

//===============================================
// ��� ������� ��
//===============================================
typedef union{
    u16     all;
    struct{
    u16 breakdown   :1; // ����. (0) - ������������ ��������� ��;
                        // ����. (1) - ��������� ��������� ��;
    u16 ready       :1; // ��� (0) - ��� ���������� � ����� �� (�� � ��������� ������, ���� � ������);
                        // ��� (1) - �� ����� � �����;
    u16 run         :1; // ��� (0) - �� ������ �� ��� ����������;
                        // ��� (1) - �� ������ �� ���� ����������.
    u16 fNegative   :1; // F+ (0) - �������� ������� ����� ������������� ����, ���� ����� ����
                        // F- (1) - �������� ������� ����� ������������� ����;
    u16 rundown     :1; // ��� (0) - ��� ���������� ���������� ��������� � ��������� ������;
                        // F--(1) - ���������� ��������� ���������� ��������� � ��������� ������
    u16 speedup     :1; // ��� (0) - ��� ������� ��������� � ��������� ������;
                        // F++ (1) - ���������� ������ ���������
    u16 charge      :1; // ����� (0) - �� ���������� ����� ����� ����������� ����;
                        // ����� (1) - ���������� ����� ����� ����������� ����;
    u16 stopByDC    :1; // ��� (0) - �� ������ �� ��� ����������� ���� ����������;
                        // ���� (1) - �� ������ �� ���� ��������� ��� ����������.
    u16 stopAll     :1; // ��� (0) - �� �� � ��������;
                        // ���� (1) - �� � ��������.
    u16 pickUp      :1; // ��� (0) - ��� ��������;
                        // ���� (1) - �������.
    u16 autoTune    :1; // ��� (0) - ��� �������������;
                        // ���� (1) - ������������.
    u16 fazirovka   :1; // CBA (0) - ��������� �� �����;
                        // ABC (1) - ��������� �� �����.
    u16 stopKey     :1; // ��� (0) - ������ ���� �� ������;
                        // ���� (1) - ������ ������.
    u16 chanalAuto  :1; // ��� (0) - ��������������� ������ ������;
                        // ���� (1) - ����� ������.
    u16 autoMode    :1; // ��� (0) - �� �� � ��������;
                        // ���� (1) - �� � ��������.
    u16 syshka      :1; // ��� (0) - ���������� �����;
                        // ���� (1) - �� ���������� �����.
    }bit;
}statusDRV_2_type;

// **************************************************************************
// ��������� ��������� ������ ����
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
    // ���� ����������� � �������������� ���������
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
    u32                         GetOpisStatus;  //�������� ��� ���������� ������� ��� �������� ����� �������� � ����
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
//������������� ������� ����
//- ������� ����
//- ���� �������
//- ���� ������
//- ���� ������ ������� �� ��� �������� ������
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
//������� ������ ���������� ���� �
//���������� ����������
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
//  ������������� ��� ������������� �������������
// **************************************************************************
void InitDPWhithoutIgnor(Full_DescriptorPrm_type *DPDrive, void *BaseHOF, void *BaseDP, u16 Type, u8 AdrNet);
void DeInitAllDP(void);

//===============================================
//������� ������������ ����������
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
//������� ���������
//===============================================
u32 FloatToStrEdit(void *BaseHOF, Full_DescriptorPrm_type *AdrFDP, wm_type *wm, char *str);
u32 UIntToStrEdit(void *BaseHOF, Full_DescriptorPrm_type *AdrFDP, wm_type *wm, char *str);
u32 HexToStrEdit(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str);
u32 TextToStrEdit(void *BaseHOF, Full_DescriptorPrm_type *AdrFDP, wm_type *wm, char *Str);
u32 HMorMSorMDToStrEdit(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str);
//===============================================
//������� ������������ ������������� ����������
//===============================================
u32 TestLimIntFrmtMin(const PrmMenuOpis_type *pntPrm, u32 PosDig);
u32 TestLimIntFrmtMax(const PrmMenuOpis_type *pntPrm, u32 PosDig);
void EditInt16Frmt(const PrmMenuOpis_type *pntPrm);
void EditFloatFrmt(const PrmMenuOpis_type *pntPrm);

void FloatViewEdit(wm_type *wm, char *str);
//===============================================
//������� ������ ��������� � ������
//===============================================
void *FindParam(void *HOF, void *DM, void *DP, void *BaseHOF, u8 NumParam);
void *FindGroup(void *HOF, void *DM, void *BaseHOF, u8 NumGroup);
/*======================================
    ���� ��������� ������
======================================*/
/*======================================
    ��������� ������
======================================*/

/*======================================
    ������� ������
======================================*/
//���������� ������ 
u32 getHelp(void *BaseHOF, char *str, u16 OfsHlp);

void RazborRx_mk(void);
void ch_to_smhdmy(time_type *tpnt, u32 chislo);
/*======================================
    ������ � �������� �������� ����
======================================*/
void SetModeMenu(u32 mode);
u32 GetModeMenu(void);
//===============================================
void GrfOut(void);
//===============================================
//===================================================================
// ���������� ���������� ����� �� � ���������� ������ ����������
//===================================================================
void WordStatusForLed(void);
//===================================================================
// ������ ���������� ����� �� � ���������� ������ ����������
//===================================================================
void parseWordStatusForLed(u16 Adr, statusDRV_tipe status);
void parseWordStatus2(u16 Adr, statusDRV_2_type status);
void InitDM(void *DMDriveDirect, u16 NumItem);
// ������� ������ �������� ���������� ���������
inline void ReadValueTextPrm(void *BaseHOF, void *HOF, Full_DescriptorPrm_type *DPDrive, char * Str, s32 NumPrm);
void InitAllDM(u16 cntItem);

u32 CalcPosCharInt(u32 VesDig, char *str);
u16 TestValidDrv(u8 netAdr, u16 numCurOpis);

// **************************************************************************
//  			����� �������� ��� ���������
// **************************************************************************
u32 getNamePrm(void *BaseHOF, void *HOF, void *FDP, char *str);
// **************************************************************************
//  			����� �������� ��� ������
// **************************************************************************
u32 getNameMenu(void *BaseHOF, void *HOF, void *FDM, char *str);
// **************************************************************************
//  			����� ������ ��� ���������
// **************************************************************************
u32 getHelpPrm(Full_DescriptorPrm_type *FDP, char *str);
// **************************************************************************
//  			����� �������� ��� ��������� �������
// **************************************************************************
void *getPntForLng(void *BaseHOF, HeadOfFile_type *HOF);
// **************************************************************************
//  			������������� ������ ���������
// **************************************************************************
void initEdIzm(void *BaseHOF);
// **************************************************************************
//  			������������� ������� ������
// **************************************************************************
void  initMessPult(void *BaseHOF);

extern DescriptorMenuOld_type             *DMGlobalOld[MAX_NUM_MENU];             
extern DescriptorPrmOld_type              *DPGlobalOld[MAX_NUM_PRM];
extern DescriptorMenuOld_type             DMDriveOld[MAX_NUM_MENU];



#endif
