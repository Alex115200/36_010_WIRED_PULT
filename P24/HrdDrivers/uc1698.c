
#include "globalIncludes.h"
#include "TskDrv.h"
#include "fixedUst.h"

u16 brightness = 500;
u16 brightness1 = 0;
u8 Lcd_driver_init = 0;


#define LCD_READ_ENABLE()  {do{ GPIO_SetBits(GPIOC, GPIO_Pin_3);\
                                GPIO_ResetBits(GPIOC, GPIO_Pin_2);}while(0);}

#define LCD_WRITE_ENABLE() {do{ GPIO_SetBits(GPIOC, GPIO_Pin_2);\
                                GPIO_ResetBits(GPIOC, GPIO_Pin_3);}while(0);}

#define LCD_READ_WRITE_DISABLE() {do{ GPIO_SetBits(GPIOC, GPIO_Pin_2);\
                                      GPIO_SetBits(GPIOC, GPIO_Pin_3);}while(0);}


#define LCD_CS_ENABLE()         GPIOC->BSRRH = GPIO_Pin_0//GPIO_ResetBits(GPIOC, GPIO_Pin_0)

#define LCD_CS_DISABLE()        GPIOC->BSRRL = GPIO_Pin_0//GPIO_SetBits(GPIOC, GPIO_Pin_0)

#define LCD_COMMAND_ENABLE()    GPIOC->BSRRH = GPIO_Pin_1//GPIO_ResetBits(GPIOC, GPIO_Pin_1)

#define LCD_DATA_ENABLE()       GPIOC->BSRRL = GPIO_Pin_1//GPIO_SetBits(GPIOC, GPIO_Pin_1)

//#define LCD_WRITE_DATA(comm) {do{GPIO_ResetBits(GPIOA, 0x00FF);\
//                                 GPIO_SetBits(GPIOA, (comm&0x00FF));}while(0);}
#define LCD_WRITE_DATA(comm) {GPIOA->BSRRH = 0x00FF; GPIOA->BSRRL = (comm&0x00FF);}

#define LCD_BACK_LIGHT_ON()  {do{}while(0);}
#define LCD_BACK_LIGHT_OFF() {do{}while(0);}

//===================================================================
//===================================================================
void LCD_init_hw(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    //SPI_InitTypeDef         SPI_InitStructure;

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef       TIM_OCInitStructure;
    uint16_t                period;
    uint16_t                pulse;

    // Enable SPI Periph clock
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;

    // Connect Pin to AF
    //GPIO_PinAFConfig(FM25_SPI_GPIO, GPIO_PinSource13, GPIO_AF_SPI2);
    //GPIO_PinAFConfig(FM25_SPI_GPIO, GPIO_PinSource14, GPIO_AF_SPI2);
    //GPIO_PinAFConfig(FM25_SPI_GPIO, GPIO_PinSource15, GPIO_AF_SPI2);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);

    //Setup GPIO LCD Data
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;

    // Connect Pin to AF
    //GPIO_PinAFConfig(FM25_SPI_GPIO, GPIO_PinSource13, GPIO_AF_SPI2);
    //GPIO_PinAFConfig(FM25_SPI_GPIO, GPIO_PinSource14, GPIO_AF_SPI2);
    //GPIO_PinAFConfig(FM25_SPI_GPIO, GPIO_PinSource15, GPIO_AF_SPI2);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);

    //setup backlight pwm
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;

    //Connect Pin to AF
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_0);

    //init timer3 ch3

    // Compute the value for the ARR register to have a period of 20 KHz
    period = (SystemCoreClock / 20000 ) - 1;	

    // Compute the CCR3 value to generate a PWN signal with 50% duty cycle
    pulse = (uint16_t) (((uint32_t) 5 * (period - 1)) / 100);

    // TIM1 clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);

    // Timer Base configuration
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = period;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // Channel 3 output configuration
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = pulse;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OC3Init(TIM3, &TIM_OCInitStructure); 
    // TIM3 counter enable
    //TIM_Cmd(TIM3, ENABLE);

    // TIM3 Main Output Enable
    //TIM_CtrlPWMOutputs(TIM3, ENABLE);
    Lcd_driver_init = 1;
}
//===================================================================
//===================================================================
void LCD_write_command(u8 comm)
{
    LCD_CS_ENABLE();
    LCD_COMMAND_ENABLE();
    LCD_WRITE_ENABLE();
    LCD_WRITE_DATA(comm);
    LCD_READ_WRITE_DISABLE();
    LCD_CS_DISABLE();
}
//===================================================================
//===================================================================
void LCD_write_data(u8 data)
{
    LCD_CS_ENABLE();
    LCD_DATA_ENABLE();
    LCD_WRITE_ENABLE();
    LCD_WRITE_DATA(data);
    LCD_READ_WRITE_DISABLE();
    LCD_CS_DISABLE();
}
//===================================================================
//===================================================================
void SetWindows(int x,int y,int w,int h)
{
    LCD_write_command(UC1698_SetWC0);  //wpc0:column
    LCD_write_command(x);              //start from 130
    LCD_write_command(UC1698_SetWC1);  //wpc1
    LCD_write_command(x+w);            //end:272

    LCD_write_command(UC1698_SetWR0);  //wpp0:row
    LCD_write_command(y);              //start from 0
    LCD_write_command(UC1698_SetWR1);  //wpp1
    LCD_write_command(y+h);            //end 160
}
//===================================================================
//===================================================================
void LCD_init(void)
{
    OSTimeDly(100);
        LCD_write_command(UC1698_SetRST);
    OSTimeDly(5);
        LCD_write_command(UC1698_SetRST);
    OSTimeDly(5);

    #if 0
     //power control
     LCD_write_command(UC1698_SetBR+1);     //Bias Ratio:0:5,1:10,2:11,3:12
     LCD_write_command(UC1698_SetPM);       //electronic potentionmeter
     LCD_write_command(255);
     LCD_write_command(UC1698_SetDE+0x06);  //display enable

     LCD_write_command(UC1698_SetPC+UC1698_LcdCapLarge+UC1698_VlcdInter);   //power control set as internal power
     LCD_write_command(UC1698_SetTC+UC1698_TC05);   //set temperate compensation as 0%

     //display control
     LCD_write_command(UC1698_SetAPO+1);    //all pixel on
     LCD_write_command(UC1698_SetINV+0);    //inverse display off

     //lcd control
     LCD_write_command(UC1698_SetMAP);      //MX & MY disable
     LCD_write_command(UC1698_SetLR+0x03);  //line rate 15.2klps
     LCD_write_command(UC1698_SetCP+1);     //rgb-rgb
     LCD_write_command(UC1698_SetCM+1);     //4k color mode
     LCD_write_command(UC1698_SetPDC+0);    //12:partial display control disable


     //n-line inversion
     LCD_write_command(UC1698_SetNIV);
     LCD_write_command(0x10);               //enable NIV

     //com scan fuction
     LCD_write_command(UC1698_SetCS+0x02);  //enable FRC,disable PWM,LRM

     //window
     LCD_write_command(UC1698_SetWC0);      //wpc0:column
     LCD_write_command(0x25);               //start from 130
     LCD_write_command(UC1698_SetWC1);      //wpc1
     LCD_write_command(0x5A);               //end:272

     LCD_write_command(UC1698_SetWR0);      //wpp0:row
     LCD_write_command(0x00);               //start from 0
     LCD_write_command(UC1698_SetWR1);      //wpp1
     LCD_write_command(0x9F);               //end 160

     LCD_write_command(UC1698_SetWMODE);    //inside mode

     LCD_write_command(UC1698_SetRAC+1);    //RAM control

     LCD_write_command(UC1698_SetDE+0x02);  //display on,select on/off mode.Green Enhance mode disable

     //scroll line
     LCD_write_command(UC1698_SetSLL+0);    //low bit of scroll line
     LCD_write_command(UC1698_SetSLM+0);    //high bit of scroll line
     LCD_write_command(UC1698_SetMAP+0x04); //enable MY.enable FLT and FLB
     LCD_write_command(UC1698_SetFL);       //14:FLT,FLB set
     LCD_write_command(0x00);

     //partial display
     LCD_write_command(UC1698_SetPDC);      //12,set partial display control:off
     LCD_write_command(UC1698_SetCEND);     //com end
     LCD_write_command(0x9f);               //160
     LCD_write_command(UC1698_SetDST);      //display start
     LCD_write_command(0);                  //0
     LCD_write_command(UC1698_SetDEND);     //display end
     LCD_write_command(159);                //160

    #endif

    #if 1
     //power control
     LCD_write_command(UC1698_SetBR+UC1698_Bias_10);     //Bias Ratio:0:5,1:10,2:11,3:12
     LCD_write_command(UC1698_SetPM);                    //electronic potentionmeter
     LCD_write_command(0xc8/*224*/);
    // LCD_write_command(UC1698_SetDE+0x06);    //display enable

     LCD_write_command(UC1698_SetPC+UC1698_LcdCapLarge+UC1698_VlcdInter);   //power control set as internal power
     LCD_write_command(UC1698_SetTC+UC1698_TC00);   //set temperate compensation as 0%

    //display control
    //LCD_write_command(UC1698_SetAPO+0);  //all pixel on
    // LCD_write_command(UC1698_SetINV+0);  //inverse display off


     //lcd control
     LCD_write_command(UC1698_SetMAP+0x04);     //MX & MY disable
    // LCD_write_command(UC1698_SetLR+0x03);    //line rate 15.2klps
     LCD_write_command(UC1698_SetCP+1);     //rgb-rgb
     LCD_write_command(UC1698_SetCM+1);     //1-4k color mode 2-64k color mode
    // LCD_write_command(UC1698_SetPDC+0);  //12:partial display control disable


     //n-line inversion
    // LCD_write_command(UC1698_SetNIV);
    // LCD_write_command(0x10);             //enable NIV

     //com scan fuction
    // LCD_write_command(UC1698_SetCS+0x02);    //enable FRC,disable PWM,LRM

     //window
    // LCD_write_command(UC1698_SetWC0);        //wpc0:column
    // LCD_write_command(0x25);             //start from 112
    // LCD_write_command(UC1698_SetWC1);        //wpc1
    // LCD_write_command(0x5A);             //end:272

    // LCD_write_command(UC1698_SetWR0);        //wpp0:row
    // LCD_write_command(0x00);             //start from 0
    // LCD_write_command(UC1698_SetWR1);        //wpp1
    // LCD_write_command(0x9F);             //end 160

    // LCD_write_command(UC1698_SetWMODE);  //inside mode

    // LCD_write_command(UC1698_SetRAC+1);  //RAM control
     LCD_write_command(UC1698_SetDE+/*0x07*/0x05);  //display on,select on/off mode.Green Enhance mode disable

     SetWindows(0x25,0,0x35,160);
     //scroll line
    // LCD_write_command(UC1698_SetSLL+0);  //low bit of scroll line
    // LCD_write_command(UC1698_SetSLM+0);  //high bit of scroll line
    // LCD_write_command(UC1698_SetMAP+0x04);   //enable MY.enable FLT and FLB
    // LCD_write_command(UC1698_SetFL);     //14:FLT,FLB set
    // LCD_write_command(0x00);

     //partial display
    // LCD_write_command(UC1698_SetPDC);        //set partial display control:off
    // LCD_write_command(UC1698_SetCEND);       //com end
    // LCD_write_command(0x9f);             //160
    // LCD_write_command(UC1698_SetDST);        //display starting com
    // LCD_write_command(0);                    //0
    // LCD_write_command(UC1698_SetDEND);       //display ending com
    // LCD_write_command(159);              //160
    #endif
}
//===================================================================
//===================================================================
u8 VideoRAM[YPIXELS*XPIXELS/8];

#define Uc1698_WriteData LCD_write_data
#define Uc1698_SetReg    LCD_write_command

//void GrfOut(void)
//===================================================================
//===================================================================
void Uc1698_Update(void)
{
    int i,j;
    char pdata;
    u8 * pVRAM = VideoRAM;
    u8 tmp;

    Uc1698_SetReg(UC1698_SetCAL+0x05);
    Uc1698_SetReg(UC1698_SetCAM+0x02);
    Uc1698_SetReg(UC1698_SetRAL+0x00);
    Uc1698_SetReg(UC1698_SetRAM+0x00);

    for(j=0;j<160;j++){
        for(i=0;i<160/8;i++){
            if (j == 13 && i == 9){
                j++;
                j--;
            }
            tmp = *pVRAM++;
            pdata = 0;
            if(tmp & 0x80) pdata =  0xf0;
            if(tmp & 0x40) pdata |= 0x0f;
            Uc1698_WriteData(pdata);
            pdata = 0;
            if(tmp & 0x20) pdata =  0xf0;
            if(tmp & 0x10) pdata |= 0x0f;
            Uc1698_WriteData(pdata);
            pdata = 0;
            if(tmp & 0x08) pdata =  0xf0;
            if(tmp & 0x04) pdata |= 0x0f;
            Uc1698_WriteData(pdata);
            pdata = 0;
            if(tmp & 0x02) pdata =  0xf0;
            if(tmp & 0x01) pdata |= 0x0f;
            Uc1698_WriteData(pdata);
        }
        Uc1698_WriteData(0);
    }
}
// ****************************************************************************
// Выключение обновления экрана
// ****************************************************************************
u32 EnUpdate = 1;
u32 EnLcd = 1;

void UpdateEn(void)
{
    EnLcd = 1;
}
// ****************************************************************************
// Включение обновления экрана
// ****************************************************************************
void UpdateDis(void)
{
    EnLcd = 0;
}
//===================================================================
//===================================================================
u32 IsUpdateEnabled(void)
{
    return EnLcd;
}
//===================================================================
//              Регулирование контраста дисплея
//===================================================================
void setContrast( void )
{
    static u8       ContrastLCD;
    u16             contrast;
    
    if( ContrastLCD != AllPrmPnt->KONTRAST ){
        contrast = 150 + AllPrmPnt->KONTRAST;
        LCD_write_command(UC1698_SetPM);  //electronic potentionmeter
        LCD_write_command( contrast );
        ContrastLCD = AllPrmPnt->KONTRAST;
    }
}

//
void UpdateContrast(u16 GrpPrm)
{
    if(GrpPrm == CONTRAST_GRPPRM)
    {
        AllPrmPnt->KONTRAST = ReqS16Prm(CONTRAST_GRPPRM);
        setContrast();
    }
}

//=============  End of file  =======================================
