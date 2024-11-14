#ifndef UART_H
#define UART_H

#ifdef _PROJECT_IAR_
    #include "stm32f2xx_dma.h"
#endif

//====================================================================

#define UART1_BUFFER_SIZE  (256 * 2)    // Протокол Модбаса * манчестер
#define UART1_TX_BUFFER_SIZE 32

extern u8 UartBufRx[UART1_BUFFER_SIZE];
extern u8 UartBufTx[UART1_TX_BUFFER_SIZE];

//====================================================================
extern u8      dumpRx;  // Свалка для данных при чтении через DMA при запитке
//====================================================================
//          Состояние порта UART по обмену данных
//====================================================================
typedef enum{
    NON_TA       = 0x00,
	RX_READY     = 0x01,
	TX_READY,
	TX_COMPLETE,
    RX_TIME_ERR,
}stateUart_type;
//====================================================================
//          Состояние драйвера UART
//====================================================================
typedef enum{
    WaiteIdleEnd,
    WaiteRxEnd,
}stateUartTransaction_type;
//====================================================================
#define MINC_ENABLE         (1<<10)
#define MINC_DISABLE        (0<<10)
//====================================================================
//#define TIMstart(TIM)     TIM->CR1 = (0x09);
//#define TIMstop(TIM)     TIM->CR1 = (0x00);

#define tim9PSinit      1200      /* 120МГц / 120 = 10мкс */
#define time9(ms)   (ms * 100)  /* Преобразование в мс */
//====================================================================

#ifndef _PROJECT_IAR_

typedef struct
{
    u16 SR;               /*!< USART Status register,                   Address offset: 0x00 */
    u16      RESERVED0;   /*!< Reserved, 0x02                                                */
    u16 DR;               /*!< USART Data register,                     Address offset: 0x04 */
    u16      RESERVED1;   /*!< Reserved, 0x06                                                */
    u16 BRR;              /*!< USART Baud rate register,                Address offset: 0x08 */
    u16      RESERVED2;   /*!< Reserved, 0x0A                                                */
    u16 CR1;              /*!< USART Control register 1,                Address offset: 0x0C */
    u16      RESERVED3;   /*!< Reserved, 0x0E                                                */
    u16 CR2;              /*!< USART Control register 2,                Address offset: 0x10 */
    u16      RESERVED4;   /*!< Reserved, 0x12                                                */
    u16 CR3;              /*!< USART Control register 3,                Address offset: 0x14 */
    u16      RESERVED5;   /*!< Reserved, 0x16                                                */
    u16 GTPR;             /*!< USART Guard time and prescaler register, Address offset: 0x18 */
    u16      RESERVED6;   /*!< Reserved, 0x1A                                                */
} USART_TypeDef;

typedef struct
{
    u32 		CR;     	/*!< DMA stream x configuration register      */
    u32 		NDTR;   	/*!< DMA stream x number of data register     */
    u32 		PAR;    	/*!< DMA stream x peripheral address register */
    u32 		M0AR;   	/*!< DMA stream x memory 0 address register   */
    u32 		M1AR;   	/*!< DMA stream x memory 1 address register   */
    u32 		FCR;    	/*!< DMA stream x FIFO control register       */
} DMA_Stream_TypeDef;

#endif // _PROJECT_IAR_

typedef struct{
    USART_TypeDef      				*reg;
    uint32_t           				dma_ch_rx;
    uint32_t           				dma_ch_tx;
    DMA_Stream_TypeDef 				*dma_rx;
    DMA_Stream_TypeDef 				*dma_tx;
    u8                 				*bufRx;
    u8                 				*bufTx;
    u16                				size_buf;
    u16                				tx_size;
    u16                				rx_size;
    stateUart_type     				state;
    stateUartTransaction_type		stateTA;
    u32                             minc;
    u16                             timeWaiteRx;    // Время ожидания ответа
}portUart_type;

extern portUart_type uart1;
//===================================================================
//                      Прототипы
//===================================================================

void USART_DMA_Start_RX(portUart_type *dev); // Функкция сбрасывающяя ДМА, и старт приема через ДМА
void USART_DMA_Start_TX(portUart_type *dev); // Функкция сбрасывающяя ДМА, и старт передачи через ДМА

void rt_hw_usart_init(void);
inline void startRxIdle(portUart_type *uart);
inline void startRxReceive(portUart_type *uart);

inline void TIMuartWdStart( u16 );
inline void TIMuartWdSop(void);

#endif // UART_H
