#ifndef MakeOpis_H
#define MakeOpis_H

//===================================================================
#define TYPE_PULT  		0
#define TYPE_DRIVE  	1

#define SzParamPult     100

#define kolvShort_RAM_MICROPROG   4096

#define LINEadr             (1uL<<14)   /* ������� ��������� ������ ���������*/
#define EXT_OPIS_PRM_ADR    (1uL<<15)   /* ������� ������������ ���������    */
//===================================================================
typedef u16 *(* pFuncMicro_type)(const u16 *, u32);
//===================================================================

extern const        u16 MicroProgTabl[];
extern const        u16 PultProgTabl[];

extern u32              RamArea[SzParamPult];
extern void             *FileOpisParam;
extern const            pFuncMicro_type  TblMicroProgFunc[];
//===================================================================
//��������� ������� � *.dll �������
//===================================================================
typedef struct{
        char        		*SymbolName   	;	// ��� ��������  "MUX8"
        char        		*Prefix     	;	// �������   "DD"
        const char      	*typePin    	;	// ��������� �� ������� ���������
        pFuncMicro_type   	FuncPnt     	;	// ��������� �� �������
        u16         		kolvPin     	;	// ���������� ����� � ������������ �������
        u16         		sizeofState   	;	// ������ ��� ������������ �������� ��� �������� ���������� ���������
        u16         		sizeofNVState 	;	// ����� ����������������� ������ ������������ ��������
        u16         		Exe       		;	// =1 ���� �������� �����������
        u16         		sizeofInputStruct;  // ������ ������� ���������
        u16         		al        		; 	// �� ������������
        }OpisFuncMicroProg_type;
//********************************************************
#define OpisFuncBlock(SymbolName, Prefix, typePin, FuncPnt, kolvPin, sizeofState, sizeofNVState, Exe) {SymbolName, Prefix, typePin, (pFuncMicro_type)FuncPnt, kolvPin, sizeofState, sizeofNVState, Exe}
#define OpisFuncBlockNew(SymbolName, Prefix, typePin, FuncPnt, kolvPin, sizeofState, sizeofNVState, Exe, sizeofInputStruct) {SymbolName, Prefix, typePin, (pFuncMicro_type)FuncPnt, kolvPin, sizeofState, sizeofNVState, Exe, sizeofInputStruct}
//********************************************************
typedef struct{
        u16         PntNumOpisPrm ;
        u16         kolvPrmInGrp  ;
        u16         LedName[4]    ; //��� ��� ������������� ������
        }OpisGrp_type;
//********************************************************
//��������� ��������������. ����� �� ���������� ����� ��. �� �������� ������� ��������
//����    - �������� ������������ ������ ��������������
//������  - �������� ������������ ������ ������� ������ ��������������
typedef struct{
        u32         CrcMicroprog  ; //����������� ����� ��������������
        u16         SignatCoreMicro ; //��������� ��������� ���� ��� ������ � ���������������
        u16         szFlash     ; //������ ���� ���� ��������� � ������������������ ������
//��������� �� ������ � ������� ��� �������������� (����������� �� 4 �����)
        u16         kolvRam_All   ; //����� ���� ��� ��������� ��� �������������� � char
        u16         kolvNvRam_All ; //����� ���� NV ��� ��������� ��� �������������� � char

        u16         Pnt_float_Sect  ; //��������� �� float ������ (������)
        u16         Pnt_s32_Sect  ; //��������� �� s32 ������ (������)
        u16         Pnt_s16_Sect  ; //��������� �� s16 ������ (������)

        u16         Sz_bit_Sect   ; //������ bit ������ (������)
//
        u16         PntRam_Mb_0x05  ; //��������� �� ������ ���������� 0x05 ������� Modbus
        u16         kolv_Mb_0x05  ; //���������� ��������

        u16         PntConst_float  ; //������ const float (����)
        u16         kolvConst_float ; //���������� const float

        u16         PntConst_s32  ; //������ const s32 (����)
        u16         kolvConst_s32 ; //���������� const s32

        u16         PntConst_s16  ; //������ const s16 (����)
        u16         kolvConst_s16 ; //���������� const s16

        u16         Reserv[16]    ; //

        u16         PntOpisGrp    ; //��������� �� ��������� �����
        u16         kolvGrp     ; //���������� ���������� �����

        u16         PntOpisUst    ; //��������� ������� (����)
        u16         kolvUst     ; //���������� �������

        u16         PntOpisTek    ; //��������� ������� ���������� (����)
        u16         kolvTek     ; //���������� ������� ����������

        u16         PntMicroProg  ; //��������� �� ���� �������������� (����)
        u16         kolvFuncMicroProg;  //���������� ������� � ��������������

        u16         PntPwrValUst  ; //�������� ������� ����� �������� (����)
//---�����
        u16         CRC16_NV    ; //����������� ����� ����� ����������������� ������ (���������)

        u16         PntModbus_0x2B  ; //��������� �� ���� ������� 0x2B
        u16         kolvModbus_0x2B ; //������ ���� 0x2B

      }MicroProg_type;

//********************************************************
typedef struct{
    const MicroProg_type  	*PntMicroProg 		; //
    const OpisGrp_type    	*PntOpisGrp   		; //
    const OpisPrm_type    	*PntOpisUst   		; //
    const OpisPrm_type    	*PntOpisTek   		; //
    u16           			*PntBase    		; //
    u32           			ramPnt      		; //��������� �� ��� �������������� ����������� � u32
    const u16     			*CodeMicroProg  	; //
    u16           			kolvGrpOpis   		;
    u16           			kolvPrmOpis   		;

    s16           			Base[16]    		;
    s16           			kolvState   		;
    u32           			cntCall     		;
    u32           			cntCallStop   		; //��������� �������
    u16           			NumFuncStop   		; //��������� �������
    u16           			NumFunc     		;
    u16           			kolvFunc    		; //���������� ������� � ��������������
}MPOpis_type;
//********************************************************
typedef struct{
    u16   val;                // ������� ���������
    u16   mask;
    u16   state;              // ��������� �������
    u16   rezerv;             // ������ ��� ������������
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
*/
//**************************************************************************
enum    EdIzm {
    bezRasm             ,//0     //
    mVRasm              ,//1     //��
    VRasm               ,//2     //�
    kVRasm              ,//3     //��
    mARasm              ,//4     //��
    ARasm               ,//5     //�
    kARasm              ,//6     //��
    HzRasm              ,//7     //��
    kHzRasm             ,//8     //���
    VARasm              ,//9     //��
    kVARasm             ,//10    //���
    WRasm               ,//11    //��
    kWRasm              ,//12    //���
    mOmRasm             ,//13    //���
    OmRasm              ,//14    //��
    kOmRasm             ,//15    //���
    MGOmRasm            ,//16    //�O�
    mkHLRasm            ,//17    //����
    mHLRasm             ,//18    //���
    mkSRasm             ,//19    //���
    mSRasm              ,//20    //��
    SRasm               ,//21    //���
    minRasm             ,//22    //���
    HourRasm            ,//23    //�
    mmRasm              ,//24    //��
    cmRasm              ,//25    //��
    dmRasm              ,//26    //��
    mRasm               ,//27    //�
    obMinRsm            ,//28    //��/���
    CelsRasm            ,//29    //��
    ProcRasm            ,//30    //%
    mKvRasm             ,//31    //�2
    mKubRasm            ,//32    //�3
    mKubPminRasm        ,//33    //�2/���
    mKubPhRasm          ,//34    //�3/�
    opisRasm            ,//35    //����
    atmRasm             ,//36    //���
    litrRasm            ,//37    //�
    gRasm               ,//38    //g
    HzSRasm             ,//39    //��/�
    VSRasm              ,//40    //�/�
    ProcSRasm           ,//41    //%/�
    kmRasm              ,//42    //��
    m_S2Rasm            ,//43    //�/�2
    kPaRasm             ,//44    //���
    MPaRasm             ,//45    //���
    kg_cm2Rasm          ,//46    //��/��2
    psiRasm             ,//47    //psi
    FarengRasm          ,//48    //�F
    kubSecRasm          ,//49    //���/�
    kW_hRasm            ,//50    //���/�
    MW_hRasm            ,//51    //���/�
    VpKm_hRasm          ,//52    //�/��
    MWar_hRasm          ,//53    //����/�
    rad_hRasm           ,//54    //���
    rad_sec_hRasm       ,//55    //���/�
    rad_sec2_hRasm      ,//56    //���/�2
    kg_m2_hRasm         ,//57    //��/�2
    Vb_hRasm            ,//58    //��
    Nm_hRasm            ,//59    //��
    grad_hRasm          ,//60    //����
    Hn_hRasm            ,//61    //��
    _1_sec_hRasm        ,//62    //1/�
    kg_Rasm             ,//63    //��
    mm2_Rasm            ,//64    //��2
    kg_m3Rasm           ,//65    //��/�3
    bar_Rasm            ,//66    //���
    Vb_Rasm             ,//67    //��
    bit_Rasm            ,//68    //���
    kolvoEdIsm           //69
};
//********************************************************
//              ��������� �������
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
