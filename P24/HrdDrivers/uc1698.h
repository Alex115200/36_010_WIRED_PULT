#ifndef __UC1698_H__
#define __UC1698_H__

//===================================================================
//              Команды контроллера UC1698
//===================================================================
#define UC1698_SetCAL		0x00	//set column address lsb Set CA[3:0] [3..0]
#define UC1698_SetCAM		0x10	//set column address msb Set CA[6:4] [2..0]
#define UC1698_SetTC		0x24	//set temp. compensation Set TC[1:0] [1..0]
#define UC1698_SetPC		0x28	//set power control      Set PC[1:0] [1..0]
#define UC1698_SetAPC		0x30	//set adv. program control,**do not using.for testing.
                                        //                       Set APC[R][7..0] [0]second byte[7..0]
#define UC1698_SetSLL		0x40	//set scroll line lsb    Set SL[3:0] [3..0]
#define UC1698_SetSLM		0x50	//set scroll line msb    Set SL[7:4] [3..0]
#define UC1698_SetRAL		0x60	//set row address lsb    Set RA[3:0] [3..0]
#define UC1698_SetRAM		0x70	//set row address msb    Set RA[7:4] [3..0]
#define UC1698_SetPM		0x81	//set v-bias potentiometer
#define UC1698_SetPDC		0x84	//set partial display control
#define UC1698_SetRAC		0x88	//set ram address control
#define UC1698_SetFL		0x90	//set fixed lines
#define UC1698_SetLR		0xa0	//set line rate
#define UC1698_SetAPO		0xa4	//set all-pixel on
#define UC1698_SetINV		0xa6	//set inverse display
#define UC1698_SetDE		0xa8	//set display enable
#define UC1698_SetMAP		0xc0	//set lcd maping control
#define UC1698_SetNIV		0xc8	//set n-line inversion
#define UC1698_SetCP		0xd0	//set color pattern
#define UC1698_SetCM		0xd4	//set color mode
#define UC1698_SetCS		0xd8	//set com scan funtion
#define UC1698_SetRST		0xe2	//set system reset
#define UC1698_SetNOP		0xe3	//no operation
#define UC1698_SetBR		0xe8	//set lcd bias ratio
#define UC1698_SetCEND		0xf1	//set com end
#define UC1698_SetDST		0xf2	//set partial display start
#define UC1698_SetDEND		0xf3	//set partial display end
#define UC1698_SetWC0		0xf4	//set window program starting column address
#define UC1698_SetWR0		0xf5	//set window program starting row address
#define UC1698_SetWC1		0xf6	//set window program ending column address
#define UC1698_SetWR1		0xf7	//set window program ending row address
#define UC1698_SetWMODE		0xf8	//set window program mode
#define UC1698_SetMTPC		0xb8	//set MTP operation control
#define UC1698_SetMTPWM		0xb9	//set MTP Write Mask

//
#define UC1698_Bias_5		0
#define UC1698_Bias_10		1
#define UC1698_Bias_11		2
#define UC1698_Bias_12		3

#define UC1698_LcdCapSmall	0x00
#define UC1698_LcdCapLarge	0x01
#define UC1698_VlcdExter	0x00
#define UC1698_VlcdInter	0x02

#define UC1698_TC00	0	//v-bias temp compensation coefficient -0.00%/co
#define UC1698_TC05	1	//v-bias temp compensation coefficient -0.05%/co
#define UC1698_TC15	2	//v-bias temp compensation coefficient -0.15%/co
#define UC1698_TC25	3	//v-bias temp compensation coefficient -0.25%/co


//#define GRAFBUFSIZE (YPIXELS*NBYTES)
extern u8 VideoRAM[YPIXELS*NBYTES];
//#define GrafBff VideoRAM
//
extern char const pic[];
//===================================================================
//                          Прототипы
//===================================================================
void LCD_init_hw(void);
void LCD_init(void);
void Uc1698_Update(void);
void GrfOut(void);

void UpdateDis(void);
void UpdateEn(void);
u32 IsUpdateEnabled(void);
void setContrast( void );

void UpdateContrast(u16 GrpPrm);
//===================================================================
//          Псевдонимы для работы кода старого проекта
//===================================================================
#define GrfOut          Uc1698_Update
#define LcdInit         LCD_init
#define LcdDis          UpdateDis
#define LcdEn           UpdateEn
#define GetStateLcdEn   IsUpdateEnabled
//===================================================================

#endif // __UC1698_H__
