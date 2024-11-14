//===================================================================

//===================================================================
#include "globalIncludes.h"

//****************************************************************************
#define  mask4byteFormat ((1<<floatFrmt)|(1<<int32Frmt)|(1<<HmsFrmt)|(1<<UTimeFrmt)|(1<<uns32Frmt) )
//****************************************************************************
#define   AllOk_merr        		0
#define   Illegal_Function_merr 	1
#define   Illegal_DataAdr_merr  	2
#define   Illegal_DataVal_merr  	3
#define   Failure_merr      		4
#define   SLAVE_DEVICE_BUSY_merr	6
//****************************************************************************
//флаги в описателе параметра
#define TypeVar_fprm        	0U    //5бит  //format
#define MasterVis_fprm      	5U    //1бит
#define MasterChange_fprm   	6U    //1бит
#define TexnVis_fprm        	7U    //1бит
#define TexnChange_fprm     	8U    //1бит
#define UserVis_fprm        	9U    //1бит
#define UserChange_fprm     	10U   //1бит
#define Power_fprm          	11U   //3бит  // олич.отображаемых символов после дес€тичной точки дл€ числовых значений
#define EditType_fprm       	14U   //3бита
#define nonVolatile_fprm    	17U   //1бит
#define LimChange_fprm      	18U   //1бит
#define FormatChange_fprm   	19U   //1бит
#define BaseNumber_fprm     	20U   //4бит
#define FuncNumber_fprm     	24U   //4бит

//#define Next_fprm           28U   //1бит

#define TypeVar_fprm_mask     0x0000001F
#define Access_fprm_mask      0x000007E0
#define EditType_fprm_mask      0x3
#define BaseNumber_fprm_mask    0xF
#define FuncNumber_fprm_mask    0xF // ѕосле сдвига вправо
//****************************************************************************
// Ќомера дл€ баз в массиве баз
#define NUM_FREQ_BASE           1
#define NUM_VOLT_BASE           2
#define NUM_CUR_BASE            3
#define NUM_POWER_BASE          4
#define NUM_1000_BASE           5
#define NUM_HZ_BASE             6
#define NUM_BASE_REF_PID        7

//EditType_fprm
#define Non_RdkPrm          	0
#define Stop_RdkPrm         	1
#define Always_RdkPrm       	2
//****************************************************************************
#define mask4byteFormat       ((1<<floatFrmt)|(1<<int32Frmt)|(1<<HmsFrmt)|(1<<UTimeFrmt)|(1<<uns32Frmt) )
//****************************************************************************
//**************************************************************************
#define FLOAT_PIN_TYPE        		0
#define S32_PIN_TYPE        		1
#define S16_PIN_TYPE        		2
#define BIT_PIN_TYPE        		3
#define TXT_PIN_TYPE        		4
#define MASK_PIN_TYPE               0x7

#define INPUT_PIN_MODE          	(0<<4)
#define OUTPUT_PIN_MODE         	(1<<4)
#define PASSIVE_PIN_MODE        	(2<<4)
#define INDEPEND_2_FLAG           	(1<<6)
#define INDEPEND_FLAG           	(1<<7)
#define MASK_PIN_MODE               (0x3<<3)
//**************************************************************************
extern const u16 PultProgTabl[];
extern uint8_t gIsNvsaOld;
//********************************************************
MPOpis_type		                MPOpis;
AllPrm_Type	                    *AllPrmPnt;
//void                            *FileOpisParam = (void *)&PultProgTabl[0];
void                            *FileOpisParam = (void *)PultProgTabl;
u32                             RamArea[SzParamPult];

//********************************************************

//********************************************************
Drv_err_type MOspecial(wm_type   *wm, s16 Rd)
{
    wm = wm;
    Rd = Rd;
    return Ok_err;
}
//****************************************************************************
/*
void RecalcBase(wm_type *wm, s16 Base, s16 Rd)
{
    _iq   tmp;

    if(Rd){
        wm->s32.Value   = _IQrmpyI32int(wm->iq.Value, Base);
    }
    else{
        tmp = (_iq)(((s64)abs(wm->s32.Value)<<GLOBAL_Q)+(Base>>1))/Base;
        if(wm->s32.Value <0)  tmp = -tmp;
        wm->iq.Value = tmp;
    }
}
*/
//**************************************************************************
void TestMakeOpis(void)
{
/*    wm_type     wmem, *wm;
    s16       Kolv, Adr, tmp;

    wm = &wmem;

    for(Kolv=0; Kolv<20 ; Kolv++){
        wm->AdrPrm = LINEadr+3;
        wm->Val2byte = Kolv;
        tmp = ModbusMakeOpis(wm, 0);
        tmp = ModbusMakeOpis(wm, 1);
    }

    for(Kolv = 75, Adr = LINEadr; Kolv>0; Kolv--, Adr++){
        wm->AdrPrm = Adr;
        tmp = ModbusMakeOpis(wm, 1);
        if(tmp) for(;;);
    }*/
}
//**************************************************************************
Drv_err_type GetFlagsPrm(wm_type *wm)
{
    MPOpis_type         *ePnt;
    const OpisPrm_type  *oPnt;
    const OpisGrp_type  *gPnt;
    u16                 NopisPrm, NopisGrp, *NNNPnt , adr, tmp;

    ePnt = &MPOpis;

    wm->mosaf = NULL;
    if(wm->AdrPrm & LINEadr){
        adr = wm->AdrPrm & ~LINEadr;        // —бросить бит признака адреса
        if(adr >= ePnt->kolvPrmOpis)         return  BadAdr_err;
        oPnt = &ePnt->PntOpisUst[adr];
    }
    else{
        NopisPrm = GetNumPrmFromAdrPrm(wm->AdrPrm);
        NopisGrp = GetNumGrpFromAdrPrm(wm->AdrPrm);

        if(NopisGrp >= ePnt->kolvGrpOpis)   return  BadAdr_err;
        gPnt = &ePnt->PntOpisGrp[NopisGrp];

        NNNPnt = AddAdr(ePnt->PntMicroProg, gPnt->PntNumOpisPrm);   //указатель на описатель параметров
        tmp    = NNNPnt[NopisPrm];
        if(tmp == 0xFFFF)          return  BadAdr_err;
        oPnt = AddAdr(ePnt->PntMicroProg,  tmp);                  //указатель на описатель параметра
        if(NopisPrm >= gPnt->kolvPrmInGrp)  return  BadAdr_err;

        adr = (u16)(((u32)oPnt) - ((u32)(ePnt->PntOpisUst))) /sizeof(OpisPrm_type);
        adr |= LINEadr;
        wm->AdrPrm = adr;
    }

    wm->Flags       = oPnt->Flags;
    wm->NumBase   = (u16)(wm->Flags >> BaseNumber_fprm) & BaseNumber_fprm_mask;
    wm->valPnt    = AddAdr(ePnt->ramPnt, oPnt->AdrVal);
    wm->TypeVar     = (PrmFrmt_type)((wm->Flags>>TypeVar_fprm)&TypeVar_fprm_mask);       //
    wm->kolvShort   = ((1<<wm->TypeVar) & mask4byteFormat) ? 2:1;
    wm->oPnt        = oPnt;
    return  Ok_err;
}
//****************************************************************************

//********************************************************
Drv_err_type ModbusMakeOpis(Full_DescriptorPrm_type *DPDrive, u16 Rd)
{

    Drv_err_type            code = Ok_err;
    u8                      *pBuf;
    wm_type                 *wm;
    u8                      Flag = 0;   //ќпредел€ет тип параметра

    DPDrive->ValidData = 0;
    if (gIsNvsaOld)
    {
        DPDrive->wm.AdrPrm          = DPDrive->AdrParam = (DPDrive->DPOld.IDGrp << 7) + DPDrive->DPOld.IDPrm;
        DPDrive->wm.Flags           = *(u32*)&DPDrive->DPOld.FlgPrm;
        DPDrive->wm.TypeVar         = (PrmFrmt_type)DPDrive->DPOld.FlgPrm.TypeVar;
        DPDrive->wm.kolvShort       = ((1<<DPDrive->wm.TypeVar) & mask4byteFormat) ? 2:1;
        DPDrive->wm.s32.LowLim      &= (0xFFFF0000 || DPDrive->DPOld.LowLim);
        DPDrive->wm.s32.HighLim     &= (0xFFFF0000 || DPDrive->DPOld.HighLim);
        if (!DPDrive->DPOld.FlgPrm.LimChange && !DPDrive->DPOld.FlgPrm.FormatChange && !DPDrive->DPOld.FlgPrm.NumBase)
            DPDrive->wm.StepEdIsm       = DPDrive->DPOld.Unit + (DPDrive->DPOld.FlgPrm.Power << 8);
        if (DPDrive->DPOld.FlgPrm.LimChange || DPDrive->DPOld.FlgPrm.FormatChange || DPDrive->DPOld.FlgPrm.NumBase)
        {
            Flag = 1;
        }
        else
        {
            Flag = 0;
        }
    }
    else
    {
        DPDrive->wm.AdrPrm          = DPDrive->AdrParam = (DPDrive->DP.IDGrp << 7) + DPDrive->DP.IDPrm;
        DPDrive->wm.Flags           = *(u32*)&DPDrive->DP.FlgPrm;
        DPDrive->wm.TypeVar         = (PrmFrmt_type)DPDrive->DP.FlgPrm.TypeVar;
        DPDrive->wm.kolvShort       = ((1<<DPDrive->wm.TypeVar) & mask4byteFormat) ? 2:1;
        DPDrive->wm.s32.LowLim      &= (0xFFFF0000 || DPDrive->DP.LowLim);
        DPDrive->wm.s32.HighLim     &= (0xFFFF0000 || DPDrive->DP.HighLim);
        if (!DPDrive->DP.FlgPrm.LimChange && !DPDrive->DP.FlgPrm.FormatChange && !DPDrive->DP.FlgPrm.NumBase)
            DPDrive->wm.StepEdIsm       = DPDrive->DP.Unit + (DPDrive->DP.FlgPrm.Power << 8);
        if (DPDrive->DP.FlgPrm.LimChange || DPDrive->DP.FlgPrm.FormatChange || DPDrive->DP.FlgPrm.NumBase)
        {
            Flag = 1;
        }
        else
        {
            Flag = 0;
        }
    }


        if(Rd == READ_UST) 
        {//ѕосылка запроса в драйвер
            if (Flag)
            {   // —ложный параметр
                if (DPDrive->wm.kolvShort == 2)
                {
                    DPDrive->SzReq = sizeof(ModbusOpisPrm4byte_type);
                    code = RequestModbusOpisPrmExt(DPDrive);
                }
                else
                {
                    DPDrive->SzReq = sizeof(ModbusOpisPrm2byte_type);
                    RequestModbusOpisPrmExt(DPDrive);
                }
            }
            else
            {                                                                   // ѕростой параметр
                if (DPDrive->wm.kolvShort == 2)
                {
                    DPDrive->SzReq = sizeof(u32);
                    RequestModbusOpisPrm(DPDrive);
                }
                else
                {
                    DPDrive->SzReq = sizeof(u16);
                    RequestModbusOpisPrm(DPDrive);
                }
            }
        }
        else if (Rd == WRITE_UST){//«апись
            pBuf = DPDrive->BufParam;
            wm = &DPDrive->wm;
            if(wm->kolvShort == 2) {
                *pBuf++ = (u8)((wm->s32.Value>>8) & 0xFF);   // текуща€ величина
                *pBuf++ = (u8)(wm->s32.Value & 0xFF);
                *pBuf++ = (u8)((wm->s32.Value>>24) & 0xFF);
                *pBuf++ = (u8)((wm->s32.Value>>16) & 0xFF);
                DPDrive->SzReq = sizeof(wm->s32.Value);
            }
            else{
                *pBuf++ = (u8)((wm->s32.Value>>8) & 0xFF);   // текуща€ величина
                *pBuf++ = (u8)(wm->s32.Value & 0xFF);
                DPDrive->SzReq = sizeof(wm->s32.Value) >> 1;
            }
            code = WritePrm(DPDrive);
        }
        else if (Rd == DO_NONE_UST)
        {
            if (gIsNvsaOld)
            {
                DPDrive->wm.StepEdIsm = DPDrive->DPOld.Unit + (DPDrive->DPOld.FlgPrm.Power << 8);
            }
            else
            {
                DPDrive->wm.StepEdIsm = DPDrive->DP.Unit + (DPDrive->DP.FlgPrm.Power << 8);
            }
        }
    return  code;
}
//****************************************************************************
//#else   // _PROJECT_IAR_
#if 0
Drv_err_type ModbusMakeOpis(Full_DescriptorPrm_type *DPDrive, u16 Rd)
{
    return 0;
}
#endif  // _PROJECT_IAR_
//****************************************************************************
Drv_err_type MakeOpis(wm_type *wm, s16 Rd, u16 Type)
{
    MPOpis_type         *ePnt;
    const OpisPrm_type  *oPnt;
    Drv_err_type        code;
    u16                 rdkAtr;
    u32                 Flags;
    //s64         LL;
    //_iq         iq_Val;
    if (Type == TYPE_PULT){
        ePnt = &MPOpis;
        code = GetFlagsPrm(wm);
        if(code != Ok_err)  return  code;
        oPnt = wm->oPnt;
        memcpy(wm->LedName, oPnt->LedName, sizeof(oPnt->LedName));
        Flags = wm->Flags;
        rdkAtr          = (u16)((Flags>>EditType_fprm)&EditType_fprm_mask); //
        if(Flags & (1UL<<FormatChange_fprm))    wm->StepEdIsm = *(s16 *)AddAdr(ePnt->ramPnt, oPnt->StepEdIsm);
        else                                  wm->StepEdIsm = oPnt->StepEdIsm;

        if(wm->TypeVar == sec2Frmt)     return  BadAdr_err;
        if(wm->kolvShort == 2){     //параметр 4 байтный
            wm->s32.LowLim  = ((u32)oPnt->LowLim&0xFFFF) |(((u32)oPnt[1].LowLim ) <<16);
            wm->s32.HighLim = ((u32)oPnt->HighLim&0xFFFF)|(((u32)oPnt[1].HighLim) <<16);
            if(Rd){
                wm->s32.Value = *(s32 *)(wm->valPnt);
                if(code = MOspecial(wm, Rd))   return  code;
            }
            else{
                if(rdkAtr == Non_RdkPrm)    return  NonRdk_err;
                //if((rdkAtr == Stop_RdkPrm)&&(!DrvIsStop())) return  NonRdk_err;
                if(wm->TypeVar == floatFrmt){
                    if(wm->f.Value < wm->f.LowLim)      return  Lim_err;
                    if(wm->f.Value > wm->f.HighLim)     return  Lim_err;
                }
                else{
                    if(wm->s32.Value < wm->s32.LowLim)  return  Lim_err;
                    if(wm->s32.Value > wm->s32.HighLim) return  Lim_err;
                }
                if(code = MOspecial(wm, Rd))   return  code;
                *(s32 *)(wm->valPnt) = wm->s32.Value;
                if(wm->mosaf != NULL)    wm->mosaf(wm, Rd);
                if(Flags & (1UL<<nonVolatile_fprm)){
                    FM25WR((void *)((wm->AdrPrm & ~LINEadr)*sizeof(u32) + (u32)(&nv.ParamPult[0])), &wm->s32.Value, sizeof(s32));     //сохранение в nvRam
                }
            }
        }//wm->Flags
        else{       //параметр 2 байтный
            if(wm->NumBase == 0){   //
                if(Rd){//я добавил!!!!!!
                    if ((wm->Flags & TypeVar_fprm_mask) == uns16Frmt){
                        wm->s32.Value = *(u16 *)(wm->valPnt);
                    }
                    else{
                        wm->s32.Value = *(s16 *)(wm->valPnt);
                    }
                }
                if(Flags & (1UL<<LimChange_fprm)){      //переменные пределы
                    if ((wm->Flags & TypeVar_fprm_mask) == uns16Frmt){
                        wm->s32.LowLim  = *(u16 *)AddAdr(ePnt->ramPnt, oPnt->LowLim);
                        wm->s32.HighLim = *(u16 *)AddAdr(ePnt->ramPnt, oPnt->HighLim);
                    }
                    else{
                        wm->s32.LowLim  = *(s16 *)AddAdr(ePnt->ramPnt, oPnt->LowLim);
                        wm->s32.HighLim = *(s16 *)AddAdr(ePnt->ramPnt, oPnt->HighLim);
                    }
                }
                else{
                    if ((wm->Flags & TypeVar_fprm_mask) == uns16Frmt){
                        wm->s32.LowLim  = (u16)oPnt->LowLim;
                        wm->s32.HighLim = (u16)oPnt->HighLim;
                    }
                    else{
                        wm->s32.LowLim  = oPnt->LowLim;
                        wm->s32.HighLim = oPnt->HighLim;
                    }
                }
            }
            else{ //не нулева€ база
                //Base = ePnt->Base[wm->NumBase];
                if(Rd){
                    wm->iq.Value = *(_iq *)(wm->valPnt);
                    //wm->s32.Value   = _IQrmpyI32int(wm->iq.Value, Base);
                }

                if(Flags & (1UL<<LimChange_fprm)){      //переменные пределы
                    wm->iq.LowLim  = *(_iq *)AddAdr(ePnt->ramPnt, oPnt->LowLim);
                    wm->iq.HighLim  = *(_iq *)AddAdr(ePnt->ramPnt, oPnt->HighLim);
                }
                else{
                    //wm->iq.LowLim  = _IQ12toIQ(oPnt->LowLim);
                    //wm->iq.HighLim = _IQ12toIQ(oPnt->HighLim);
                }
                //wm->s32.LowLim  = _IQrmpyI32int(wm->iq.LowLim,  Base);
                //wm->s32.HighLim = _IQrmpyI32int(wm->iq.HighLim, Base);
            }
            if(Rd){
                if(code = MOspecial(wm, Rd))   return  code;
            }
            else{
                if(rdkAtr == Non_RdkPrm)    return  NonRdk_err;
                //if((rdkAtr == Stop_RdkPrm)&&(!DrvIsStop())) return  NonRdk_err;
                if((wm->s32.Value < wm->s32.LowLim)||(wm->s32.Value > wm->s32.HighLim))  return  Lim_err;

                if(wm->NumBase == 0){
                    if(code = MOspecial(wm, Rd))   return  code;
                    *(s16 *)(wm->valPnt) = (s16)wm->s32.Value;
                    if(wm->mosaf != NULL)    wm->mosaf(wm, Rd);
                }
                else{           //¬ пульте такого не будет
/*                    LL = (((s64)abs(wm->s32.Value)<<GLOBAL_Q)+(Base>>1))/Base;
                    if((LL >> 32) != 0)   return  Lim_err;
                    iq_Val = (_iq)LL;



                    if(wm->s32.Value <0)  iq_Val = -iq_Val;
                    wm->iq.Value = iq_Val;

                    if(code = MOspecial(wm, Rd))   return  code;
                    *(_iq *)(wm->valPnt) = wm->iq.Value;
                    if(wm->mosaf != NULL)    wm->mosaf(wm, Rd);*/
                }
                if(Flags & (1UL<<nonVolatile_fprm)){     //сохранение в nvRam
                    FM25WR((void *)((wm->AdrPrm & ~LINEadr)*sizeof(u32) + (u32)(&nv.ParamPult[0])), &wm->s32.Value, sizeof(s32));
                }
            }
        }
    }
    else{//Type == TYPE_DRIVE

    }
    return  Ok_err;
}
//===================================================================
//===================================================================
u16 swapU16(u16 Val)
{
    u16 tmp = Val;
    Val = ((tmp << 8) & 0xFF00) +  (tmp >> 8);

    return Val;
}
//===================================================================
//===================================================================
u32 swapU32(u32 Val)
{
    u32 tmp = Val;
    Val = ((tmp >> 8) & 0xFF0000) +((tmp << 8) & 0xFF000000) +((tmp >> 8) & 0xFF) +((tmp << 8) & 0xFF00);

    return Val;
}
//===================================================================
//===================================================================
int MicroProgWrCoil(s16 Adr, s16 val)
{
    MPOpis_type                 *ePnt;
    const MicroProg_type        *mPnt;
    StateFuncMODBUS_0X05_type   *sPnt;
    u16                         tmp;

    ePnt = &MPOpis;
    mPnt = ePnt->PntMicroProg;
    if(Adr >= (mPnt->kolv_Mb_0x05 <<3)) return Illegal_DataAdr_merr;  //err DataAdr
    sPnt = (StateFuncMODBUS_0X05_type *)((u32)ePnt->ramPnt + mPnt->PntRam_Mb_0x05);
    sPnt = &sPnt[Adr>>3];
    tmp = 1<<(Adr&0x7);
    if(val){
        if(sPnt->mask & tmp){
            sPnt->val |= tmp;      // ”становить бит установки
        }
        else{
            return  2;                          //err   Failure
        }
    }
    else{
        if( sPnt->mask & (tmp<<8)){
            sPnt->val |= (tmp<<8);// ”становить бит сброса
        }
        else{
            return  2;                          //err   Failure
        }
    }

    return  0;
}
//===================================================================
//===================================================================
void StartExtMem(void)
{
    u16                     usTmp1, usTmp2;
    u32                     Zer[64], *u32Pnt;
    s16                     i;
    MicroProg_type          *pntM;

    pntM = (MicroProg_type *)MicroProgTabl;

    FM25RD(&usTmp1, &nv.MemSignatura, sizeof(u16));
    FM25RD(&usTmp2, &nv.MicroSignatura, sizeof(u16));

    if((usTmp1 != pntM->SignatCoreMicro)||(usTmp2 != MPOpis.PntMicroProg->CRC16_NV)){
//        memset( Zer, 0, sizeof(Zer) );
//        for( i=0, u32Pnt = &nv.ParamPult[0]; i < szEEPROMmem/sizeof(Zer); i++,u32Pnt++ ){
//            FM25WR(u32Pnt, &Zer, 64 );
//        }
        Ust_Copy(PWR_to_RAM);
        Ust_Copy(RAM_to_NV);

        usTmp1 = pntM->SignatCoreMicro;
        usTmp2 = MPOpis.PntMicroProg->CRC16_NV;
        FM25WR( &nv.MemSignatura, &usTmp1,   sizeof(u16));
        FM25WR( &nv.MicroSignatura, &usTmp2, sizeof(u16));

    }

    Ust_Copy(NV_to_RAM);
}
//****************************************************************************
int Ust_Copy(UstCopy_enum   Ust_src_to_dest)
{
    const OpisPrm_type  *oPnt;
    u16                 Adr, *u16Pnt, kolv, f4byte;
    s32         Val, *s32Pnt;
    wm_type       wmem;

    u16Pnt = (u16 *)(MPOpis.PntMicroProg->PntPwrValUst + (u32)MPOpis.PntMicroProg);
    kolv = MPOpis.PntMicroProg->kolvUst;
      oPnt = MPOpis.PntOpisUst;
    s32Pnt = (s32 *)(&nv.ParamPult[0]);
    f4byte = 0;
    for(Adr= LINEadr; kolv > 0; kolv--, Adr++, oPnt++, u16Pnt++){
        if(f4byte){
            f4byte = 0;
            continue;
        }
        wmem.AdrPrm = Adr;
        GetFlagsPrm(&wmem);
        if(Ust_src_to_dest == PWR_to_RAM){
            if(wmem.kolvShort == 1){
                if(wmem.NumBase == 0) *(s16 *)wmem.valPnt = *u16Pnt;
                //else          *(_iq *)wmem.valPnt = _IQ12toIQ(*u16Pnt);
            }
            else{
                *(s32 *)wmem.valPnt = (s32)u16Pnt[0] | ((s32)u16Pnt[1] <<16);
                f4byte = 1;
            }
        }
        else if(Ust_src_to_dest == RAM_to_NV){
            if((wmem.kolvShort == 1)&&(wmem.NumBase == 0))  Val = *(s16 *)wmem.valPnt;
            else                      Val = *(s32 *)wmem.valPnt;
                FM25WR(s32Pnt++, &Val, sizeof(u32));
        }
        else{   //NV_to_RAM
            FM25RD(&Val, s32Pnt++, sizeof(u32));
            if(wmem.TypeVar == floatFrmt){
                *(s32 *)wmem.valPnt = Val;
                f4byte = 1;
            }
            else if((wmem.kolvShort == 1)&&(wmem.NumBase == 0)) *(s16 *)wmem.valPnt = (s16)Val;
            else{
                *(s32 *)wmem.valPnt = Val;
                f4byte = 1;
            }
        }
    }
    return  0;
}
//****************************************************************************
//****************************************************************************
u32 MicroProg_Init(u32 ramPnt, const MicroProg_type *prgPnt)
{
    void      *PntFlash, *PntRam;
    MPOpis_type   *ePnt;

    ePnt = &MPOpis;
    memset((char *)ePnt, 0, sizeof(MPOpis_type));
    if(GetCrc((unsigned char *)&prgPnt->SignatCoreMicro, (prgPnt->szFlash - sizeof(s32))) != prgPnt->CrcMicroprog){
        memset((char *)ePnt, 0, sizeof(MPOpis_type));
        return 0;
    }

    ePnt->PntMicroProg      = prgPnt;
    ePnt->PntOpisGrp    = (const OpisGrp_type   *)((u32)prgPnt + prgPnt->PntOpisGrp);
    ePnt->PntOpisUst    = (const OpisPrm_type   *)((u32)prgPnt + prgPnt->PntOpisUst);
    ePnt->PntOpisTek    = (const OpisPrm_type   *)((u32)prgPnt + prgPnt->PntOpisTek);
    ePnt->CodeMicroProg   = (const u16      *)((u32)prgPnt + prgPnt->PntMicroProg);
    ePnt->kolvGrpOpis   = prgPnt->kolvGrp;
    ePnt->kolvPrmOpis   = prgPnt->kolvUst + prgPnt->kolvTek;

    ePnt->cntCallStop     = 0xFFFFFFFF;
    ePnt->NumFuncStop     = 0xFFFF;
    ePnt->kolvFunc      = 6000;     //WARNING

    memset((char *)ramPnt, 0, prgPnt->kolvRam_All);             //протирание всей выделенной пам€ти
    AllPrmPnt = (AllPrm_Type *)ramPnt;
    ramPnt    = ramPnt + prgPnt->Sz_bit_Sect;
    ePnt->ramPnt= ramPnt;
    PntRam    = AddAdr(ramPnt, prgPnt->Pnt_float_Sect);         //инициализаци€ float const
    PntFlash  = AddAdr(prgPnt, prgPnt->PntConst_float);
    memcpy(PntRam, PntFlash, prgPnt->kolvConst_float * sizeof(float));

    PntRam    = AddAdr(ramPnt, prgPnt->Pnt_s32_Sect);           //инициализаци€ s32 const
    PntFlash  = AddAdr(prgPnt, prgPnt->PntConst_s32);
    memcpy(PntRam, PntFlash, prgPnt->kolvConst_s32 * sizeof(s32));

    PntRam    = AddAdr(ramPnt, prgPnt->Pnt_s16_Sect);           //инициализаци€ s16 const
    PntFlash  = AddAdr(prgPnt, prgPnt->PntConst_s16);
    memcpy(PntRam, PntFlash, prgPnt->kolvConst_s16 * sizeof(s16));

    //setBitMicro(ramPnt, 0x8001);                              //устанавливаем первый бит в секции битовых переменных
    //debugCopy();
    StartExtMem();

    /*ePnt->Base[NUM_FREQ_BASE  ] = 800;
    ePnt->Base[NUM_VOLT_BASE  ] = 380;
    ePnt->Base[NUM_CUR_BASE   ] = 10000;
    ePnt->Base[NUM_POWER_BASE ] = 1000;
    ePnt->Base[NUM_1000_BASE  ] = 1000;
    ePnt->Base[NUM_HZ_BASE      ] = 800;
    ePnt->Base[NUM_BASE_REF_PID ] = 1000;*/
    //TestMakeOpis();

    return  ramPnt;
}
//****************************************************************************
//****************************************************************************
////u16 KolvoShort(DescriptorPrm_type *DP)
////{
////    if (DP->FlgPrm.TypeVar == floatFrmt || DP->FlgPrm.TypeVar == int32Frmt || DP->FlgPrm.TypeVar == HmsFrmt ||
////        DP->FlgPrm.TypeVar == UTimeFrmt || DP->FlgPrm.TypeVar == uns32Frmt)
////        return 2;
////    else return 1;
////}
//****************************************************************************
