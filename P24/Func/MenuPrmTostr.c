
// **************************************************************************
//  Преобразование параметра в строку
// **************************************************************************
void ParamToStr(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str)
{
    CallHandleParam(BaseHOF, DPDrive, wm, HANDLE_VIEW_PARAM);
}
// **************************************************************************
//  
// **************************************************************************
void FloatToStr(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *Str)
{
    u8 			Dot, Edism;

    Dot = wm->StepEdIsm >> 8;
    Edism = wm->StepEdIsm & 0xFF;
    sprintf(&Str[0], "%.*f%s", Dot, wm->f.Value, &MasEdIzmTek[Edism][0]);
}
// **************************************************************************
//  
// **************************************************************************
void UIntToStr(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *Str)
{
    f32         FVal;
    u32         Step;
    u32         i, dot;
    s32         Val;

    Val = wm->s32.Value;
    if (wm->TypeVar == uns16Frmt || wm->TypeVar == uns32Frmt)
        Val = abs(Val);
    dot = wm->StepEdIsm >> 8;

    Step = 1;
    for (i = 0; i < dot; i++)
        Step *= 10;
    i = wm->StepEdIsm & 0xFF;
    if (wm->TypeVar == int16Frmt) {
        FVal = (f32)((double)(s16)Val / Step);
    }
    else{
        FVal = (f32)((double)Val / Step);
    }
    sprintf(&Str[0], "%.*f%s", dot, (f32)FVal, &MasEdIzmTek[i][0]);
}
// **************************************************************************
//  
// **************************************************************************
void TxtToStr(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str)
{
    void                *Adr;  
    HeadOfLanguage_type HOL;
    HeadOfFile_type     HOF;
    char 			    strtmp[17];
    
    if (gIsNvsaOld)
    {
        Adr = (void *)((u32)DPDrive->BaseHOF + DPDrive->DPOld.OfsTxt + wm->s32.Value * 16);
//        AT45RD(&strtmp[0], Adr, 16);
        memcpy(&strtmp[0], (void *)((u32)Adr + STM32_FLASH_OFFSET), 16); //  *!* Загрузка из памяти STM по сдвигу адреса           
        sprintf(str, "%s", &strtmp[0]);
    }
    else
    {
//        AT45RD(&HOF, BaseHOF, sizeof(HeadOfFile_type));
        memcpy(&HOF, (void *)((u32)BaseHOF + STM32_FLASH_OFFSET), sizeof(HeadOfFile_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
        if ((DPDrive->DP.OfsTxt != 0xFFFF)) //|| (FDP->DP.LenHlp != 0))
        {
            Adr = getPntForLng(BaseHOF, &HOF);// + (FDP->OfsName << 1));
//            AT45RD(&HOL, Adr, sizeof(HeadOfLanguage_type));
            memcpy(&HOL, (void *)((u32)Adr + STM32_FLASH_OFFSET), sizeof(HeadOfLanguage_type)); //  *!* Загрузка из памяти STM по сдвигу адреса           
            Adr = (void *)((u32)Adr + (HOL.PntTxtTxt << 1) + (DPDrive->DP.OfsTxt << 1) + wm->s32.Value * 16);
            //Adr = (void *)((u32)DPDrive->BaseHOF + DPDrive->DP.OfsTxt + NumPrm * 16);
//            AT45RD(str, Adr, 16);
            memcpy(str, (void *)((u32)Adr + STM32_FLASH_OFFSET), 16); //  *!* Загрузка из памяти STM по сдвигу адреса                       
        }
        else
        {
            str[0] = 0;
        }
    }
}
// **************************************************************************
//  
// **************************************************************************
void HexToStr(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str)
{
    u16 Val;
    Val = (u16)wm->s32.Value;
    sprintf(str, "%#.4x", Val);
}
// **************************************************************************
//  
// **************************************************************************
void DayMonToStr(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str)
{
    u16 		DayMonth, Mon, Day;

    DayMonth = (u16)wm->s32.Value;

    for (Mon = 1; Mon <= 13; Mon ++){
        if (monthSumEx[Mon] >= DayMonth){
            Mon --;
            break;
        }
    }
    Day = DayMonth - monthSumEx[Mon];
    sprintf(str, "%d-%s", Day, NameMonth[Mon]);
}
// **************************************************************************
//  
// **************************************************************************
void HourMinToStr(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str)
{
    u16 		HourMin, Hour, Min;

    HourMin = (u16)wm->s32.Value;
    Hour 	= HourMin / 60;
    Min 	= HourMin % 60;
    sprintf(str, "%.2d:%.2d", Hour, Min);
}
// **************************************************************************
//  
// **************************************************************************
void RefToStr(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str)
{
    u16 		GrpPrm, Grp, Prm;

    GrpPrm 	= (u16)wm->s32.Value;
    Grp 	= GrpPrm >> 7;
    Prm 	= GrpPrm - (Grp << 7);
    sprintf(str, "%.2d:%.2d", Grp, Prm);
}
// **************************************************************************
//  
// **************************************************************************
void MinSecToStr(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str)
{
    u16 		MinSec, Min, Sec;

    MinSec 	= (u16)wm->s32.Value;
    Min 	= MinSec / 60;
    Sec 	= MinSec % 60;
    sprintf(str, "%d:%d", Min, Sec);
}
// **************************************************************************
//  
// **************************************************************************
void UnixToStr(void *BaseHOF, Full_DescriptorPrm_type *DPDrive, wm_type *wm, char *str)
{
    u32 		UnixTime;
    time_type 	Time;

    UnixTime = wm->s32.Value;
    ch_to_smhdmy(&Time, UnixTime);
    sprintf(str, "%.2d%s%.2d %.2d:%.2d:%.2d", Time.date, NameMonth[Time.month], Time.year, Time.hour, Time.minute, Time.second);
}





