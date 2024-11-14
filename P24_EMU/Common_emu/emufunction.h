
#ifndef __EmuFunction__
#define __EmuFunction__

typedef struct
{
  u16      SR;         /*!< USART Status register,                   Address offset: 0x00 */
  u16      RESERVED0;  /*!< Reserved, 0x02                                                */
  u16      DR;         /*!< USART Data register,                     Address offset: 0x04 */
  u16      RESERVED1;  /*!< Reserved, 0x06                                                */
  u16      BRR;        /*!< USART Baud rate register,                Address offset: 0x08 */
  u16      RESERVED2;  /*!< Reserved, 0x0A                                                */
  u16      CR1;        /*!< USART Control register 1,                Address offset: 0x0C */
  u16      RESERVED3;  /*!< Reserved, 0x0E                                                */
  u16      CR2;        /*!< USART Control register 2,                Address offset: 0x10 */
  u16      RESERVED4;  /*!< Reserved, 0x12                                                */
  u16      CR3;        /*!< USART Control register 3,                Address offset: 0x14 */
  u16      RESERVED5;  /*!< Reserved, 0x16                                                */
  u16      GTPR;       /*!< USART Guard time and prescaler register, Address offset: 0x18 */
  u16      RESERVED6;  /*!< Reserved, 0x1A                                                */
} USART_TypeDef_;

u32    dmaRx, dmaTx;            

struct stm32_uart_device{
    USART_TypeDef      *reg;
    u32                dma_ch_rx;
    u32                dma_ch_tx;
    u32                *dma_rx; 
    u32                *dma_tx;
    u8                 *buf;
    u16                size_buf;
    u16                tx_size;
    u16                rx_size;
    u8                 state;
};


//===================================================================
// собственно экземпляр драйвера уарт
//===================================================================
// флаги состояния UART
#define RX_IDLE_READY     0x01
#define RX_IDLE_COMPLETE  0x02
#define TX_READY          0x03
#define TX_COMPLETE       0x04
#define UART_ERROR        0x80 // маска ошибки 

//#define UART1_BUFFER_SIZE       512

u8 Uart1Buf[UART1_BUFFER_SIZE];

USART_TypeDef   USART1;
/*
struct stm32_uart_device uart1 =
{
	    &USART1,
        0,                  // UART1_DMA_CHANNEL_RX,
        0,                  // UART1_DMA_CHANNEL_TX,  
        0,                  // UART1_RX_DMA,
        0,                  // UART1_TX_DMA,
        Uart1Buf,
        sizeof(Uart1Buf),
        16,
        0,
        RX_IDLE_READY
};*/
//===================================================================

#endif