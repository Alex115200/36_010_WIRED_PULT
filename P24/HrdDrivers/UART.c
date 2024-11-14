//#include "include.h"
//#include "uart.h"
#include "types.h"
#include "Pult_RAM.h"
#include "ucos_ii.h"

#include "OS_Obj.h"

/*
#include "irq.h"

#define MODE_UART_NONE  0
#define MODE_UART_TX    1
#define MODE_UART_RX    2

#define PCLK_UART       25000000
#define DIV             1
#define MUL             2
#define DIVF            1.0
#define MULF            2.0
*/
extern AllPrm_Type *AllPrmPnt;

extern unsigned short SizeRX;

/*
extern OS_EVENT *OutDRVSem;

__no_init UART_type UART3  @adrUART3;

inline void ClrRxBuf(UartMsg_type *UartMsg)
{
    volatile unsigned char tmp;
    tmp = UartMsg->UART->RBR;
    UartMsg->UART->FCRbit.RFR = 1;
}

//====================================================================
//       ��������� ����� ��� ����������� ���������
// =>* u - ��������� �� ��������� �����
//====================================================================
void UARTinit(UartMsg_type *UartMsg)
{
    unsigned int temp;

    temp = UartMsg->UART->IIR;
    temp = UartMsg->UART->LSR;


    //UartMsg->time35t = ByteTimeUart[UartMsg->BaudRate];
#ifndef TEST_P24
    UartMsg->time35t = ByteTimeUart[sizeof(ByteTimeUart) / sizeof(ByteTimeUart[0]) - 1];      //����� �� �������� ������ ������
#else
    UartMsg->time35t = 0;      //����� �� �������� ������ ������
#endif
    UartMsg->cnt = 0;


    temp = (u32)BdrtUart[UartMsg->BaudRate];

    UartMsg->UART->TERbit.TXEN = 0;
    UartMsg->UART->FDRbit.DIVADDVAL = DIV;
    UartMsg->UART->FDRbit.MULVAL = MUL;
//    UartMsg->UART->FDRbit = 0x00000021;
    UartMsg->UART->LCR = DLAB;                       // ��������� ������ � ��������� �������� UART
    UartMsg->UART->DLL = temp & 0x000000FF;          // �� ���� �������� ������
    UartMsg->UART->DLM = (temp >> 8) & 0x000000FF;   // �� ���� �������� ������
    UartMsg->UART->LCR |= WLS_8b | SBS;              // ����� ������� 8 ����� + 2 �������� ����
    UartMsg->UART->LCR &= ~DLAB;                     // ��������� ��������� � ������� UART
    UartMsg->UART->FCR = FCRFE | RFR | TFR           // ��������� ����� FIFO � ������� �������� N ��������
     | RTLS_level;                          // �������� �����a FIFO
    //UartMsg->UART->IER  = RDAIE | THREIE;            // ��������� ���������� �� ������ ������ � �����
    UartMsg->UART->IERbit.THREIE = 1;
    UartMsg->UART->IERbit.RXLSIE = 1;
    NVIC_IntEnable(NVIC_UART3);

}
//====================================================================
// �������� ������
//====================================================================
#ifndef TEST_P24
void CheckTimeOutMB(UartMsg_type *UartMsg)
{
    if (UartMsg->Mode == MODE_UART_TX){                    //�������� ������
        if(UartMsg->UART->LSRbit.TEMT){
            UARTin();
            UartMsg->Mode = MODE_UART_RX;                   //������� �� ����� ������
            ClrRxBuf(UartMsg);
            UartMsg->UART->IERbit.RDAIE = 1;     //��������� ���������� �� ������
        }
    }
    else if (UartMsg->Mode == MODE_UART_RX){
        if (UartMsg->UART->LSRbit.DR != 0)         //���-�� ���� � ������, �� ������� �������
            return;
        UartMsg->cnt ++;

        if (UartMsg->cnt > UartMsg->time35t){     //�������
            UartMsg->cnt = UartMsg->time35t;
            UartMsg->Mode = MODE_UART_NONE;
            UartMsg->UART->IERbit.RDAIE = 0;     //��������� ���������� �� ������
            UartMsg->UART->FCRbit.RFR = 1;   //���������� ����� ����
            OSSemPost(UARTSem);
        }
    }
//    FIO1SET = 1 << 25;
}
#else
void CheckTimeOutMB(UartMsg_type *UartMsg)
{
    INT8U err;
    if (UartMsg->Mode == MODE_UART_TX){                    //�������� ������
        if(UartMsg->UART->LSRbit.TEMT){
            UARTin();
            UartMsg->Mode = MODE_UART_RX;                   //������� �� ����� ������
            ClrRxBuf(UartMsg);
            UartMsg->UART->IERbit.RDAIE = 1;     //��������� ���������� �� ������
            OSSemPost(UARTSem);
        }
    }
    else if (UartMsg->Mode == MODE_UART_NONE){
        if (UartMsg->time35t == 0){
            return;
        }
        if (UartMsg->UART->LSRbit.DR != 0)         //���-�� ���� � ������, �� ������� �������
            return;
        UartMsg->cnt ++;

        if (UartMsg->cnt > UartMsg->time35t){     //�������
            UartMsg->pntBuf = UartMsg->Buf = &DataBuf[0];     //������� ��������� �� ������ ������
            UartMsg->time35t = 0;
            UartMsg->cnt = UartMsg->time35t;

            UartMsg->Mode = MODE_UART_NONE;
            UartMsg->UART->IERbit.RDAIE = 1;     //��������� ���������� �� ������
            UartMsg->UART->FCRbit.RFR = 1;   //���������� ����� ����
            OSSemSet (UARTSemRx, 0, &err); //�������� �������
            OSSemPost(UARTSemRx);
        }
    }
    else{

    }
//    FIO1SET = 1 << 25;
}
#endif
//====================================================================
// �������� ������
//====================================================================
UartMsg_type   UartMsg;

void TxUart(UartMsg_type *UartMsg)
{

    u16 i;

//    if (UartMsg->Sz == 0) return;
    UARTout();
    //pnt = UartMsg.pntBuf;//UartMsg->Buf;

    for (i = 0; i < UMAXLEN && i < UartMsg->Sz; i ++){
       UartMsg->UART->THR = *UartMsg->pntBuf++;  // ��������� FIFO
    }
    UartMsg->Sz -= i;

    UartMsg->UART->TER = 0x80;      // start transmit
    UartMsg->Mode = 1;              //����� �������� ������

}
//====================================================================
// ���������� ���������� �� UART3
//====================================================================
#define IID_RLS     3
#define IID_RDA     2
#define IID_CTI     6
#define IID_THRE    1
#ifndef TEST_P24
void UART3_ISR_Handler(void)
{

    volatile unsigned char tmp;
    unsigned int TypeInt;

    OSIntNesting ++;

    TypeInt = UartMsg.UART->IIRbit.IID;

    if(TypeInt == IID_THRE){                                //������� ����
        tmp = UartMsg.UART->IIR;
        if(UartMsg.Sz) TxUart(&UartMsg);
        else UartMsg.pntBuf = UartMsg.Buf;                  //��������� �������������� ��������� �� ������ ������
    }
    else if(TypeInt == IID_RDA || TypeInt == IID_CTI){

        UartMsg.time35t = ByteTimeUart[UartMsg.BaudRate];   //��������� ��������
        UartMsg.cnt = 0;

        if(UartMsg.Mode == MODE_UART_TX) ClrRxBuf(&UartMsg);           //����� ��������, ������ ����� ������???
        else{                                               //����� ��������
            while(UartMsg.UART->LSRbit.DR != 0){
                *UartMsg.pntBuf++ = UartMsg.UART->RBR;
                if(UartMsg.pntBuf > UartMsg.Buf + UartMsg.SzBuf - 1)
                    UartMsg.pntBuf = UartMsg.Buf;
                UartMsg.Sz = UartMsg.pntBuf - UartMsg.Buf;
            }
            if (TypeInt == IID_CTI){
                UartMsg.Mode = MODE_UART_NONE;
                UartMsg.UART->IERbit.RDAIE = 0; //��������� ���������� �� ������
                OSSemPost(UARTSem);
            }
        }
    }
    else if (TypeInt == IID_RLS){               //������ �����
        tmp = UartMsg.UART->LSR;                //������ ����������
        ClrRxBuf(&UartMsg);                     //������ �������� �����
        UartMsg.Sz = 0;                         //�������� ���������� �������� ����
        UartMsg.time35t =
        ByteTimeUart[sizeof(ByteTimeUart) /
        sizeof(ByteTimeUart[0]) - 1];           //���� ��������� ��������� ������
    }
    NVIC_ClrPend(NVIC_UART3);
    OSIntExit();
}
#else
void UART3_ISR_Handler(void)
{

    volatile unsigned char tmp;
    unsigned int TypeInt;
    INT8U err;

    OSIntNesting ++;

    TypeInt = UartMsg.UART->IIRbit.IID;

    if(TypeInt == IID_THRE){                                //������� ����
        tmp = UartMsg.UART->IIR;
        if(UartMsg.Sz) TxUart(&UartMsg);
        else UartMsg.pntBuf = UartMsg.Buf;                  //��������� �������������� ��������� �� ������ ������
    }
    else if(TypeInt == IID_RDA || TypeInt == IID_CTI){

        UartMsg.cnt = 0;

        if(UartMsg.Mode == MODE_UART_TX) ClrRxBuf(&UartMsg);           //����� ��������, ������ ����� ������???
        else{                                               //����� ��������
            while(UartMsg.UART->LSRbit.DR != 0){
                *UartMsg.pntBuf++ = UartMsg.UART->RBR;
                if(UartMsg.pntBuf > UartMsg.Buf + UartMsg.SzBuf - 1)
                    UartMsg.pntBuf = UartMsg.Buf;
                UartMsg.Sz = UartMsg.pntBuf - UartMsg.Buf;
            }
            if (TypeInt == IID_CTI){
                UartMsg.pntBuf = UartMsg.Buf = &DataBuf[0];     //������� ��������� �� ������ ������
                UartMsg.Mode = MODE_UART_NONE;
                UartMsg.UART->IERbit.RDAIE = 1; //!!!��������� ���������� �� ������
                OSSemSet (UARTSemRx, 0, &err); //�������� �������
                OSSemPost(UARTSemRx);
            }
            else{
                UartMsg.time35t = ByteTimeUart[UartMsg.BaudRate];   //��������� ��������
            }
        }
    }
    else if (TypeInt == IID_RLS){               //������ �����
        tmp = UartMsg.UART->LSR;                //������ ����������
        ClrRxBuf(&UartMsg);                     //������ �������� �����
        UartMsg.Sz = 0;                         //�������� ���������� �������� ����
        UartMsg.time35t = 0;
        //ByteTimeUart[sizeof(ByteTimeUart) /
        //sizeof(ByteTimeUart[0]) - 1];           //���� ��������� ��������� ������
    }
    NVIC_ClrPend(NVIC_UART3);
    OSIntExit();
}
#endif
u32 SetUartSpeed(u32 speed)
{
    if (speed > GetNumUartSpeed() - 1) {
        return 1;
    }
    UartMsg.BaudRate = speed;
    return 0;
}

u32 GetUartSpeed(void)
{
    return UartMsg.BaudRate;
}
//��������� �������� ������� ������ � ����
void SetUartUstSpeed(void)
{
    UartMsg.BaudRate = AllPrmPnt->SPEED;
}

u32 GetNumUartSpeed(void)
{
    return sizeof(BdrtUart) / sizeof(BdrtUart[0]);
}

//======================================================================
// ������ � ����������� ��� �������� UART
const unsigned short BdrtUart[10] =
{
  (u16)(((PCLK_UART * 16) / (1200       * (1 + DIVF / MULF))) / 256),  //0
  (u16)(((PCLK_UART * 16) / (2400       * (1 + DIVF / MULF))) / 256),  //1
  (u16)(((PCLK_UART * 16) / (4800       * (1 + DIVF / MULF))) / 256),  //2
  (u16)(((PCLK_UART * 16) / (9600       * (1 + DIVF / MULF))) / 256),  //3
  (u16)(((PCLK_UART * 16) / (14400      * (1 + DIVF / MULF))) / 256),  //4
  (u16)(((PCLK_UART * 16) / (19200      * (1 + DIVF / MULF))) / 256),  //4
  (u16)(((PCLK_UART * 16) / (38400      * (1 + DIVF / MULF))) / 256),  //5
  (u16)((((PCLK_UART * 16) / (56000      * (1 + DIVF / MULF))) / 256)),  //6
  (u16)((((PCLK_UART * 16) / (57600      * (1 + DIVF / MULF))) / 256)),  //7
//  (u16)(((PCLK_UART * 16) / (76800      * (1 + DIVF / MULF))) / 256),  //7
  (u16)(((PCLK_UART * 16) / (115200     * (1 + DIVF / MULF))) / 256),  //8
//  (u16)(((PCLK_UART * 16) / (230400     * (1 + DIVF / MULF))) / 256),  //9
//  (u16)(((PCLK_UART * 16) / (460800     * (1 + DIVF / MULF))) / 256),  //10
//  (u16)(((PCLK_UART * 16) / (921600     * (1 + DIVF / MULF))) / 256),  //11
//  (u16)(((PCLK_UART * 16) / (1843200    * (1 + DIVF / MULF))) / 256),  //12
};

const char *const BdrtName[10] = {
    "1200",
    "2400",
    "4800",
    "9600",//14400
    "14400",
    "19200",
    "38400",
    "56000",
    "57600",
//    "76800",
    "115200",
    //"230400",
    //"460800",
    //"921600",
    //"1843200",
};
//const unsigned short BdrtUart[13] =
//{ (CCLK + ((16 * 1200)    / 2)) / (16 * 1000)   ,   // 0
//  (CCLK + ((16 * 2400)    / 2)) / (16 * 2400)   ,   // 1
//  (CCLK + ((16 * 4800)    / 2)) / (16 * 4800)   ,   // 2
//  (CCLK + ((16 * 9600)    / 2)) / (16 * 9600)   ,   // 3
//  (CCLK + ((16 * 19200)   / 2)) / (16 * 19200)  ,   // 4
//  (CCLK + ((16 * 38400)   / 2)) / (16 * 38400)  ,   // 5
//  (CCLK + ((16 * 57600)   / 2)) / (16 * 57600)  ,   // 6
//  (CCLK + ((16 * 76800)   / 2)) / (16 * 76800)  ,   // 7
//  (CCLK + ((16 * 115200)  / 2)) / (16 * 115200) ,   // 8
//  (CCLK + ((16 * 230400)  / 2)) / (16 * 230400) ,   // 9
//  (CCLK + ((16 * 460800)  / 2)) / (16 * 460800) ,   // 10
//  (CCLK + ((16 * 921600)  / 2)) / (16 * 921600) ,   // 11
//  (CCLK + ((16 * 1843200) / 2)) / (16 * 1843200) }; // 12

#define CONST_TIME_OUT  10000.F
const u32 ByteTimeUart[11] =
{
    (u16)(1.0/1200.0 *      10 * 3.5 * CONST_TIME_OUT),  //0
    (u16)(1.0/2400.0 *      10 * 3.5 * CONST_TIME_OUT),  //1
    (u16)(1.0/4800.0 *      10 * 3.5 * CONST_TIME_OUT),  //2
    (u16)(1.0/9600.0 *      10 * 3.5 * CONST_TIME_OUT),  //3
    (u16)(1.0/14400.0 *     10 * 3.5 * CONST_TIME_OUT),  //3//
    (u16)(1.0/19200.0 *     10 * 3.5 * CONST_TIME_OUT),  //4
    (u16)(1.0/38400.0 *     10 * 3.5 * CONST_TIME_OUT),  //5
    (u16)(1.0/56000.0 *     10 * 3.5 * CONST_TIME_OUT),  //7
    (u16)((1.0/57600.0 *     10 * 3.5 * CONST_TIME_OUT) + 10),  //8
    //(u16)(1.0/76800.0 *     10 * 3.5 * CONST_TIME_OUT),  //7
    (u16)(1.0/115200.0 *    10 * 3.5 * CONST_TIME_OUT),  //9
    //(u16)(1.0/230400.0 *    10 * 3.5 * CONST_TIME_OUT),  //9
    //(u16)(1.0/460800.0 *    10 * 3.5 * CONST_TIME_OUT),  //10
    //(u16)(1.0/921600.0 *    10 * 3.5 * CONST_TIME_OUT),  //11
    //(u16)(1.0/184320.0 *    10 * 3.5 * CONST_TIME_OUT),  //12
    5000                                                  //����� �������� ������ ����� ��������� ��������

};

//const u32 ByteTimeUart[14] =
//{
//    (CONST_TIME_OUT * 10 * (CCLK / 1200    )) / (PCLK / MKS100)  , // 0
//    (CONST_TIME_OUT * 10 * (CCLK / 2400    )) / (PCLK / MKS100)  , // 1
//    (CONST_TIME_OUT * 10 * (CCLK / 4800    )) / (PCLK / MKS100)  , // 2
//    (CONST_TIME_OUT * 10 * (CCLK / 9600    )) / (PCLK / MKS100)  , // 3
//    (CONST_TIME_OUT * 10 * (CCLK / 19200   )) / (PCLK / MKS100)  , // 4
//    (CONST_TIME_OUT * 10 * (CCLK / 38400   )) / (PCLK / MKS100)  , // 5
//    (CONST_TIME_OUT * 10 * (CCLK / 57600   )) / (PCLK / MKS100)  , // 6
//    (CONST_TIME_OUT * 10 * (CCLK / 76800   )) / (PCLK / MKS100)  , // 7
//    (CONST_TIME_OUT * 10 * (CCLK / 115200  )) / (PCLK / MKS100)  , // 8
//    (CONST_TIME_OUT * 10 * (CCLK / 230400  )) / (PCLK / MKS100)  , // 9
//    (CONST_TIME_OUT * 10 * (CCLK / 460800  )) / (PCLK / MKS100)  , // 10
//    (CONST_TIME_OUT * 10 * (CCLK / 921800  )) / (PCLK / MKS100)  , // 11
//    (CONST_TIME_OUT * 10 * (CCLK / 1843200 )) / (PCLK / MKS100)  , // 12
//    5000                                                  //����� �������� ������ ����� ��������� ��������
//
//};
*/

//*************************************************************
//*************************************************************
#include "uart.h"
//#include "packet.h"
#include "Manchester.h"
#include "modbus.h"
#include "string.h"
#include "crc.h"
#include "globdefs.h"

// ��������� ��������
#define USING_UART1

#define UART1_BUFFER_SIZE 256
#define UART1_TX_SIZE (16*2)
#define UART1_RX_SIZE 256

#define UART1_USE_DMA

// ���� UART1
#define UART1_GPIO_TX		GPIO_Pin_6
#define UART1_GPIO_RX		GPIO_Pin_7
#define UART1_GPIO		GPIOB

// ������������� ��� ���� ����(�������� �� �������� ��� ���� ������� ���������� ������������)
#define UART1_GPIO_SOURCE_TX	GPIO_PinSource6
#define UART1_GPIO_SOURCE_RX	GPIO_PinSource7

// ��������� ����������� STM �� ��������� � ��������� �������� ����� ������� ������� ������ ������� ����� �������
#define UART1_GPIO_CLK_EN()     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE)
#define UART1_CLK_EN()	        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE)
#define UART1_DMA_EN()          RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE)

// ���� ��������� STM �� ��������� � ��������� ������ ����� ���������
// � ����� ������ � ���� ������������� ����� ���� ��������� ����������� ������ � ���������
// ��� ������� ��������� � ����� UART 1
#define UART1_DMA_CHANNEL_RX    DMA_Channel_4
#define UART1_DMA_CHANNEL_TX    DMA_Channel_4
#define UART1_RX_DMA		    DMA2_Stream5
#define UART1_TX_DMA		    DMA2_Stream7

// ���������� �������
void rt_hw_serial_isr( portUart_type *dev );

#ifdef USING_UART1
//===================================================================
// ���������� ��������� �������� ����
//===================================================================
u8 UartBufRx[UART1_BUFFER_SIZE];
u8 UartBufTx[UART1_BUFFER_SIZE];

portUart_type uart1 =
{
	USART1,
        UART1_DMA_CHANNEL_RX,
        UART1_DMA_CHANNEL_TX,
        UART1_RX_DMA,
        UART1_TX_DMA,
        UartBufRx,
        UartBufTx,
        sizeof(UartBufRx),
        UART1_TX_SIZE,
        NULL,
        RX_IDLE_READY
};
#endif

//===================================================================
// ��� �������� ��������� � �������� �������� ����� �������������
// �� ������������
// ��������� ��������� �������� ������� ������������ ������
//===================================================================
static void RCC_Configuration(void)
{
#ifdef USING_UART1
    // Enable USART1 and GPIOx clocks
    UART1_GPIO_CLK_EN();
    UART1_CLK_EN();
#ifdef UART1_USE_DMA
    // Enable DMA for USART1
    UART1_DMA_EN();
#endif
#endif
}
//===================================================================
// ��������� GPIO
//===================================================================
static void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

#ifdef USING_UART1
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;

    GPIO_InitStruct.GPIO_Pin = UART1_GPIO_TX | UART1_GPIO_RX;
    GPIO_Init(UART1_GPIO,&GPIO_InitStruct);

    // Connect Pin to AF
    GPIO_PinAFConfig(UART1_GPIO, UART1_GPIO_SOURCE_TX, GPIO_AF_USART1);
    GPIO_PinAFConfig(UART1_GPIO, UART1_GPIO_SOURCE_RX, GPIO_AF_USART1);
#endif
}
//===================================================================
// ��������� ������
//===================================================================
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

#ifdef USING_UART1
    // Enable the USART1 Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;//configMAX_SYSCALL_INTERRUPT_PRIORITY; //??? configLIBRARY_LOWEST_INTERRUPT_PRIORITY; //
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        //???
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
}
//===================================================================
// ��������� ���
//===================================================================
u8      dumpRx;     // ������ ��� ������ ��� ������ ����� DMA ��� �������

#ifdef UART1_USE_DMA
static void DMA_Configuration(portUart_type *dev)
{
    DMA_InitTypeDef DMA_InitStructure;

    DMA_ClearFlag(dev->dma_rx, DMA_FLAG_TCIF5 | DMA_FLAG_TEIF5);
    DMA_ClearFlag(dev->dma_tx, DMA_FLAG_TCIF7 | DMA_FLAG_TEIF7);

    // RX Stream configuration
    DMA_Cmd(dev->dma_rx, DISABLE);
    DMA_InitStructure.DMA_Channel               = dev->dma_ch_rx;
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (u32)&dev->reg->DR;
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Circular;//DMA_Mode_Normal;
    DMA_InitStructure.DMA_FIFOMode              = DMA_FIFOMode_Disable;
    // ��� ���� �������� ���� ��������� � �� ��� ��� ������
    DMA_InitStructure.DMA_FIFOThreshold         = DMA_FIFOThreshold_1QuarterFull;
    DMA_InitStructure.DMA_MemoryBurst           = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst       = DMA_PeripheralBurst_Single;
    DMA_InitStructure.DMA_BufferSize            = UART1_BUFFER_SIZE;
    DMA_InitStructure.DMA_Memory0BaseAddr       = (u32)dumpRx;                  // ����� ���� �� ������
    DMA_InitStructure.DMA_MemoryInc             = DMA_PeripheralInc_Disable;    // ����� ���� �� ������

    DMA_Init(dev->dma_rx, &DMA_InitStructure);
    DMA_Cmd(dev->dma_rx, ENABLE);

    // TX Stream configuration
    DMA_Cmd(dev->dma_tx, DISABLE);
    DMA_InitStructure.DMA_Channel               = dev->dma_ch_tx;
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (u32)&dev->reg->DR;
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_Medium;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Normal;
    DMA_InitStructure.DMA_FIFOMode              = DMA_FIFOMode_Disable;
    // ��� ���� �������� ���� ��������� � �� ��� ��� ������
    DMA_InitStructure.DMA_FIFOThreshold         = DMA_FIFOThreshold_1QuarterFull;
    DMA_InitStructure.DMA_MemoryBurst           = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst       = DMA_PeripheralBurst_Single;

    DMA_InitStructure.DMA_BufferSize            = UART1_TX_SIZE;

    DMA_InitStructure.DMA_Memory0BaseAddr       = (u32)dev->bufTx;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;

    DMA_Init(dev->dma_tx, &DMA_InitStructure);

    //DMA_Cmd(dev->dma_tx, ENABLE);
}
#endif
//===================================================================
// ��� ��� ������� ������������� UART
// ����� �������� ��� ������
//===================================================================
void rt_hw_usart_init(void)
{
    USART_InitTypeDef USART_InitStructure;

    RCC_Configuration();
    GPIO_Configuration();
    NVIC_Configuration();

    TIM7init();

    // uart init
#ifdef USING_UART1
    USART_DeInit(uart1.reg);
    USART_InitStructure.USART_BaudRate            = 230400;//115200; // ����� ���� ������� � ��������
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(uart1.reg, &USART_InitStructure);

#ifdef UART1_USE_DMA
    DMA_Configuration(&uart1);
#endif

    uart1.reg->SR = 0xFFFF;

    // Enable USART1, enable interrupt
    uart1.reg->CR1 = USART_CR1_RE     | // Reciever Enable
                     USART_CR1_UE     | // Uart Enable
                     USART_CR1_IDLEIE | // Uart Idle Interrupt Enable
                     USART_CR1_PEIE;    // Parity Error Interrupt Enable

    // ��������� ����� ����� DMA
    uart1.reg->CR3 = USART_CR3_EIE | USART_CR3_DMAR;

    //USART_Cmd(uart1.reg, ENABLE);
    // enable interrupt
    //USART_ITConfig(uart1.reg, USART_IT_IDLE, ENABLE);
    //USART_ClearFlag(uart1.reg,USART_FLAG_TXE);
    //USART_DMACmd(uart1.reg, USART_DMAReq_Rx, ENABLE);
#endif
}
//===================================================================
// �������� ������������ ���, � ����� ������ ����� ���
//===================================================================
void USART_DMA_Start_RX(portUart_type *uart)
{
//    struct stm32_uart_device *uart;
//    configASSERT(dev != RT_NULL);
//    uart = (struct stm32_uart_device*)dev->user_data;

    // ��������� ������ ���������, ���������� ��������� ������ � UART
    uart->reg->CR1 |= USART_CR1_RE     | /* Reciever Enable */               \
                      USART_CR1_UE     | /* Uart Enable */                   \
                      USART_CR1_RXNEIE | /**/                                \
                      USART_CR1_IDLEIE;    /* Uart Idle Interrupt Enable  */

    // ��������� �������� ����� DMA, ��������� ���������� �� ������� � ��������� ����� USART1 ����� DMA
    uart->reg->CR3 = USART_CR3_EIE | USART_CR3_DMAR;

    // ���������� DMA
    DMA_Cmd(uart->dma_rx, DISABLE);
    // ��������� �������� ��������� ������
    // ����� �������� ���������������� ������ � �������� DMA ������
    // ����� ����� ������


    uart->dma_rx->M0AR = (u32)uart->bufRx;
    uart->dma_rx->CR &= (~(1<<10));    // MINC = 0
    uart->dma_rx->CR |= uart->minc;

    uart->dma_rx->NDTR = uart->size_buf;  // � ����������� ������ ���� �������� ���������
                                          // ����� ���������� ������ �������� ��������
                                          // ��������� �� ����� � �������� ���������
                                          // ������� �� �� 1 ������ ������� ������
    DMA_ClearFlag(uart->dma_rx, DMA_FLAG_TCIF5 | DMA_FLAG_TEIF5); // �������� ����� ���������� �� ��������� ������
    DMA_Cmd(uart->dma_rx, ENABLE);
}
//===================================================================
// �������� ������������ ���, � ����� �������� ����� ���
//===================================================================
void USART_DMA_Start_TX( portUart_type *uart )
{
//    struct stm32_uart_device *uart;
//    configASSERT(dev != RT_NULL);
//    uart = (struct stm32_uart_device*)dev->user_data;

    // ���������� DMA � ��������� �� �������� �����
    uart->dma_tx->CR &= ~(uint32_t)DMA_SxCR_EN;

    uart->dma_tx->NDTR = uart->tx_size;

    DMA_ClearFlag(uart->dma_tx, DMA_FLAG_TCIF7 | DMA_FLAG_TEIF7); // �������� ����� ���������� �� ��������� ������
    uart->dma_tx->CR |= (uint32_t)DMA_SxCR_EN;

    // ��������� ���������� �� ������� UART � ��������� �������� ����� DMA
    uart->reg->CR3 = USART_CR3_DMAT;

    // ��������� ������ �����������, ���������� ��������� �������� � UART
    uart->reg->CR1 = USART_CR1_TCIE   | // Tranciever Interrupt Complete Enable
                     USART_CR1_UE     | // Uart Enable
                     USART_CR1_TE;      // Tranciever Enable
}
//===================================================================
// ��� ����������� ���������� ���� ��� ������������ �������
// �������� ����������� � ��� ��� ����� �� �������� ������������
// ������ ����� ����������� ������� IDLE, ����� ��������� ��������� ��������
// ������ ����� ���������� ��������� �������� ������� ��� �������� �������
// ���������� ���������� ��������� ��������� �� ������ TaskUart
//===================================================================
inline
void RxStop(void)
{

}

void rt_hw_serial_isr( portUart_type *uart)
{
//    struct stm32_uart_device *uart;
    volatile u8             temp;   // ������� ��� ��������� ������ ������
    u16                     cntRxData;

//    device_copy = dev;
//    configASSERT(dev != RT_NULL);
//    uart = (struct stm32_uart_device*)dev->user_data;

    // ***********************
    // �������� ������ � ������
    if (uart->reg->SR & (USART_SR_PE | USART_SR_FE |   // Parity Error Framing Error
                         USART_SR_NE | USART_SR_ORE)){ // Noise Error Flag  Overrun Error Flag

        temp = uart->reg->DR;
        uart->reg->SR = ~USART_SR_TC; // �������� ����� ����������, Uart_size_RX ������� ��� temp
        temp = uart->reg->DR;
        temp = uart->reg->DR;
        startRxIdle(uart);
    }

    if(uart->reg->SR & USART_SR_IDLE){      //  ��������� ������� �� �����
        uart->reg->CR1 &= ~(USART_CR1_RE | USART_CR1_IDLEIE | USART_CR1_PEIE);
        DMA_Cmd(uart->dma_rx, DISABLE);     // ��������� ������ ������

        if(uart->stateTA == WaiteIdleEnd){  // ���� �����
            if( uart->state == TX_READY ){  // ���� ���� �������
                uart->stateTA = WaiteRxEnd;

                USART_DMA_Start_TX(uart);   // ��������� ����������
            }
            else{
                startRxIdle(uart);
            }
        }
        else{                               // ���� ������
            cntRxData = (uart->size_buf) - uart->dma_rx->NDTR; // ���������� �������� ���� �������� �������
            uart->rx_size = cntRxData;
            startRxIdle(uart);
            OSMutexPost(UARTMutex);             // ��������� � ������
            TIMstop(TIM7);
        }
    }
    else if(uart->reg->SR & USART_SR_TC)    // ��������� ���������� �� �����������
    {
        // �������� ��������
        // ���� ���������� ������������ �� ������ � ������� DR

        uart->state = TX_COMPLETE;          // ��������� ������

        uart->reg->CR3 &= ~USART_CR3_DMAT;
        uart->reg->CR1 &= ~USART_CR1_TCIE;

        startRxReceive(uart);               // ���� ������
        TIMstart(TIM7);                     // �������� ������ �� ������ �� ������
    }
}

#ifdef USING_UART1
//===================================================================
//===================================================================
inline
void startRxIdle( portUart_type *uart)
{

    uart->stateTA = WaiteIdleEnd;

    uart->bufRx = &dumpRx;              // �������������� �����
    uart->minc  = MINC_DISABLE;         // MINC = 0

    USART_DMA_Start_RX(uart);       // ��������� ��������
}
//===================================================================
//===================================================================
inline
void startRxReceive( portUart_type *uart)
{
    uart->stateTA = WaiteRxEnd;

    uart->bufRx = UartBufRx;       // �������������� �����
    uart->minc  = MINC_ENABLE;      // MINC = 1

    USART_DMA_Start_RX(uart);       // ��������� ��������
}
//===================================================================
void USART1_IRQHandler(void)
{
    OSIntEnter ();
        rt_hw_serial_isr(&uart1);
    OSIntExit();
}
#endif
//===================================================================

//===================================================================
// ������������� ������� 7 ��� ������� ��������� ���������� ��� ������
// ����� ������������ �������
//===================================================================
void TIM7init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_TIM7_STOP; // TIM7_STOP
    RCC->APB1ENR |= (1<<5);   // TIM7EN �������� ������������ ������ �������

    // ������������� NVIC
    NVIC_InitStructure.NVIC_IRQChannel                      = TIM7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 15;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    // ������������� �������
    TIM7->PSC = 29;                     // 1/30 ������������
    TIM7->ARR = (0xffff - 12500);       // 12.5 ��
    TIM7->CNT = 0;                      // ���������� ������
    TIM7->CR1 = (0x09);                 // ����� ������ ��������
    TIM7->DIER = 0x01;                  // ��������� ������� �� ����������
}
//===================================================================
// ���������� ���������� �� ������� ��� ������� ��������� ����������
// �� ���������� ������ �� �����
//===================================================================
void TIM7_IRQHandler(void)
{
    uart1.state = RX_ERR;           // ������ ������
//    uart1.stateTA = WaiteIdleEnd;
    startRxIdle(&uart1);
    TIM7->SR = 0;                   // �������� ����������
    OSMutexPost(UARTMutex);         // ��������� � ������
    return;
}
//===================================================================
//===================================================================
//===================================================================
//===================================================================
//===================================================================
//===================================================================
// �������� ������������ ���, � ����� ������ ����� ���
//===================================================================
void USART_DMA_Start_RX_old( portUart_type *uart )
{
//    struct stm32_uart_device *uart;
//    configASSERT(dev != RT_NULL);
//    uart = (struct stm32_uart_device*)dev->user_data;

    // ��������� ������ ���������, ���������� ��������� ������ � UART
    uart->reg->CR1 = USART_CR1_RE     | // Reciever Enable
                     USART_CR1_TE     |
                     USART_CR1_UE     | // Uart Enable
                     USART_CR1_IDLEIE | // Parity Error Interrupt Enable
                     USART_CR1_PEIE;    // Uart Idle Interrupt Enable

    // ��������� �������� ����� DMA, ��������� ���������� �� ������� � ��������� ����� USART1 ����� DMA
    uart->reg->CR3 = USART_CR3_EIE | USART_CR3_DMAR;

    // ���������� DMA
    DMA_Cmd(uart->dma_rx, DISABLE);
    // ��������� �������� ��������� ������
    // ����� �������� ���������������� ������ � �������� DMA ������
    // ����� ����� ������
    uart->dma_rx->NDTR = uart->size_buf;  // � ����������� ������ ���� �������� ���������
                                          // ����� ���������� ������ �������� ��������
                                          // ��������� �� ����� � �������� ���������
                                          // ������� �� �� 1 ������ ������� ������
    DMA_ClearFlag(uart->dma_rx, DMA_FLAG_TCIF5 | DMA_FLAG_TEIF5); // �������� ����� ���������� �� ��������� ������
    DMA_Cmd(uart->dma_rx, ENABLE);
}
//===================================================================
// �������� ������������ ���, � ����� �������� ����� ���
//===================================================================
void USART_DMA_Start_TX_old( portUart_type *uart )
{
//    struct stm32_uart_device *uart;
//    configASSERT(dev != RT_NULL);
//    uart = (struct stm32_uart_device*)dev->user_data;

    // ���������� DMA � ��������� �� �������� �����
    uart->dma_tx->CR &= ~(uint32_t)DMA_SxCR_EN;//DMA_Cmd(uart->dma_tx, DISABLE);
    // ��������� �������� ��������� ������
    // ����� �������� ���������������� ������ � �������� DMA ������
    // ����� ����� ������
    uart->dma_tx->NDTR = uart->tx_size;   // �������� ��������� ������� ���� �������� �� ������
                                          // !!! ����� ������� ����� �� �����������
    DMA_ClearFlag(uart->dma_tx, DMA_FLAG_TCIF7 | DMA_FLAG_TEIF7); // �������� ����� ���������� �� ��������� ������
    uart->dma_tx->CR |= (uint32_t)DMA_SxCR_EN;//DMA_Cmd(uart->dma_tx, ENABLE);

    // ��������� ���������� �� ������� UART � ��������� �������� ����� DMA
    uart->reg->CR3 = USART_CR3_DMAT;

    // ��������� ������ �����������, ���������� ��������� �������� � UART
    uart->reg->CR1 = USART_CR1_TCIE   | // Tranciever Interrupt Complete Enable
                     USART_CR1_UE     | // Uart Enable
                     USART_CR1_TE;      // Tranciever Enable
}

//=============  End of file  =======================================

