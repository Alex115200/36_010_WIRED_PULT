// В файле собраны функции заглушки и объекты 

#include "globalIncludes.h"

//===================================================================
// собственно экземпляр драйвера уарт
//===================================================================
u8 UartBufRx[UART1_BUFFER_SIZE];
u8 UartBufTx[UART1_TX_BUFFER_SIZE];

portUart_type uart1 =
{
	NULL,//USART1,						   	// USART_TypeDef      				*reg;
	0,//UART1_DMA_CHANNEL_RX,          	// uint32_t           				dma_ch_rx;
	0,//UART1_DMA_CHANNEL_TX,          	// uint32_t           				dma_ch_tx;
	NULL,//UART1_RX_DMA,                  	// DMA_Stream_TypeDef 				*dma_rx;
	NULL,//UART1_TX_DMA,                  	// DMA_Stream_TypeDef 				*dma_tx;
	UartBufRx,                     	// u8                 				*bufRx;
	UartBufTx,                     	// u8                 				*bufTx;
	sizeof(UartBufRx),             	// u16                				size_buf;
	UART1_TX_BUFFER_SIZE,         	// u16                				tx_size;
	0,								// u16                				rx_size;                                	
	RX_READY,                       // stateUart_type     				state;
	WaiteIdleEnd,                  	// stateUartTransaction_type		stateTA;
    MINC_DISABLE,                   // u32                              minc;
    time9((u16)10.0),                    // u16                              timeWaiteRx;    // Время ожидания ответа
};
//===================================================================
u8 VideoRAM[YPIXELS*XPIXELS/8];
u8 VideoRAM_Copy[YPIXELS*XPIXELS/8];

unsigned char DataBuf[256];        // Буфер уарта
//===================================================================
int OSWait(int CmpStop, int NEvent);
//===================================================================
//void Kontrast(void) {}
//===================================================================
/*
int MakeMenu(const id_type *pntId)
{
    return 0;
}
*/
//===================================================================
// Заглушка для функции экрана
//===================================================================
/*
u16  EnLcd;
void UpdateEn(void)
{
    EnLcd = 1;
}*/
// =================================================================
// Включение обновления экрана
// =================================================================
/*
void UpdateDis(void)
{
    EnLcd = 0;
}*/
//===================================================================
void USART_DMA_Start_RX(portUart_type *uart){}
//===================================================================
void USART_DMA_Start_TX(portUart_type *uart){}
//===================================================================
f32 KoefKontrast(u16 codeAdc)
{
    return 1.0f;
}
//===================================================================
u16 GetCodeADC(void)
{
    return 0;
}
//===================================================================

//===================================================================
unsigned char _Manchester_Decode(unsigned short encoded_data)
{
    return (u8)encoded_data;
}
//===================================================================
unsigned short _Manchester_Encode(unsigned char data)
{
    return data;
}
//===================================================================


