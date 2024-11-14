
//#include "uart.h"
#include "globalIncludes.h"


extern AllPrm_Type *AllPrmPnt;
extern unsigned short SizeRX;
//*************************************************************
//*************************************************************
#include "uart.h"
#include "Manchester.h"
#include "modbus.h"
#include "string.h"
#include "crc.h"

// настройки драйвера
#define USING_UART1         
#define UART1_USE_DMA      

// ноги UART1
#define UART1_GPIO_TX		GPIO_Pin_6
#define UART1_GPIO_RX		GPIO_Pin_7
#define UART1_GPIO		GPIOB

// перекидывание ног если надо(смотреть по даташиту там есть таблица допустимых конфигураций)
#define UART1_GPIO_SOURCE_TX	GPIO_PinSource6
#define UART1_GPIO_SOURCE_RX	GPIO_PinSource7

// специфика архитектуры STM от семейства к семейству регистры могут менятся поэтому дефайн целиком вызов функции
#define UART1_GPIO_CLK_EN()     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE)
#define UART1_CLK_EN()	        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE)
#define UART1_DMA_EN()          RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE)

// тоже специфика STM от семейства к семейству каналы могут отличатся
// в любом случае в коде инициализации могут быть изменения внимательно читаем и проверяем
// Эти дефайны привязаны к порту UART 1
#define UART1_DMA_CHANNEL_RX    DMA_Channel_4
#define UART1_DMA_CHANNEL_TX    DMA_Channel_4
#define UART1_RX_DMA		    DMA2_Stream5
#define UART1_TX_DMA		    DMA2_Stream7

// внутренние обьекты
void rt_hw_serial_isr( portUart_type *dev );

#ifdef USING_UART1
//===================================================================
// собственно экземпляр драйвера уарт
//===================================================================
u8 UartBufRx[UART1_BUFFER_SIZE];
u8 UartBufTx[UART1_TX_BUFFER_SIZE];

portUart_type uart1 =
{
	USART1,						   	// USART_TypeDef      				*reg;
	UART1_DMA_CHANNEL_RX,          	// uint32_t           				dma_ch_rx;
	UART1_DMA_CHANNEL_TX,          	// uint32_t           				dma_ch_tx;
	UART1_RX_DMA,                  	// DMA_Stream_TypeDef 				*dma_rx;
	UART1_TX_DMA,                  	// DMA_Stream_TypeDef 				*dma_tx;
	UartBufRx,                     	// u8                 				*bufRx;
	UartBufTx,                     	// u8                 				*bufTx;
	sizeof(UartBufRx),             	// u16                				size_buf;
	UART1_TX_BUFFER_SIZE,         	// u16                				tx_size;
	0,								// u16                				rx_size;                                	
	RX_READY,                       // stateUart_type     				state;
	WaiteIdleEnd,                  	// stateUartTransaction_type		stateTA;
    MINC_DISABLE,                   // u32                              minc;
    time9(10.0),                    // u16                              timeWaiteRx;    // Время ожидания ответа
};
#endif  // USING_UART1

//===================================================================
// для удобства понимания я разделил основные части инициализации
// на подпрограммы
// включение источника тактовой частоты периферийных блоков
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
#endif  // USING_UART1
}
//===================================================================
// настройка GPIO
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
#endif  // USING_UART1
}
//===================================================================
// Настройка НВиииК
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
#endif  // USING_UART1
}
//===================================================================
// Настройка ДМА
//===================================================================
u8      dumpRx;     // Свалка для данных при чтении через DMA при запитке

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
    // эти поля всеравно надо заполнить а то как бог пошлет
    DMA_InitStructure.DMA_FIFOThreshold         = DMA_FIFOThreshold_1QuarterFull;
    DMA_InitStructure.DMA_MemoryBurst           = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst       = DMA_PeripheralBurst_Single;
    DMA_InitStructure.DMA_BufferSize            = UART1_BUFFER_SIZE;
    DMA_InitStructure.DMA_Memory0BaseAddr       = (u32)dumpRx;                  // Пишем пока на свалку
    DMA_InitStructure.DMA_MemoryInc             = DMA_PeripheralInc_Disable;    // Пишем пока на свалку

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
    // эти поля всеравно надо заполнить а то как бог пошлет
    DMA_InitStructure.DMA_FIFOThreshold         = DMA_FIFOThreshold_1QuarterFull;
    DMA_InitStructure.DMA_MemoryBurst           = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst       = DMA_PeripheralBurst_Single;

    DMA_InitStructure.DMA_BufferSize            = UART1_TX_BUFFER_SIZE;

    DMA_InitStructure.DMA_Memory0BaseAddr       = (u32)dev->bufTx;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;

    DMA_Init(dev->dma_tx, &DMA_InitStructure);
}
#endif
//===================================================================
// Вот она функция инициализации UART
// можно вызывать где угодно
//===================================================================
void TIM9MicroInit(void);

void rt_hw_usart_init(void)
{
    USART_InitTypeDef USART_InitStructure;

    RCC_Configuration();
    GPIO_Configuration();
    NVIC_Configuration();
    TIM9MicroInit();

    // uart init
#ifdef USING_UART1
    USART_DeInit(uart1.reg);
    USART_InitStructure.USART_BaudRate            = 230400uL; //115200; // позже надо вынести в параметр
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

    // Разрешаем прием через DMA
    uart1.reg->CR3 = USART_CR3_EIE | USART_CR3_DMAR;

    //USART_Cmd(uart1.reg, ENABLE);
    // enable interrupt
    //USART_ITConfig(uart1.reg, USART_IT_IDLE, ENABLE);
    //USART_ClearFlag(uart1.reg,USART_FLAG_TXE);
    //USART_DMACmd(uart1.reg, USART_DMAReq_Rx, ENABLE);

#endif  // USING_UART1
}
//===================================================================
// Функкция сбрасывающяя ДМА, и старт приема через ДМА
//===================================================================
void USART_DMA_Start_RX(portUart_type *uart)
{
//    struct stm32_uart_device *uart;
//    configASSERT(dev != RT_NULL);
//    uart = (struct stm32_uart_device*)dev->user_data;

    // Разрешаем работу приемника, прерывания окончания приема и UART
    uart->reg->CR1 |= USART_CR1_RE     | /* Reciever Enable */               \
                      USART_CR1_UE     | /* Uart Enable */                   \
                      USART_CR1_RXNEIE | /**/                                \
                      USART_CR1_IDLEIE;    /* Uart Idle Interrupt Enable  */

    // Запрещаем передачу через DMA, Разрешаем прерывание по ошибкам и разрешить прием USART1 через DMA
    uart->reg->CR3 = USART_CR3_EIE | USART_CR3_DMAR;

    // сбрасываем DMA
    DMA_Cmd(uart->dma_rx, DISABLE);
    // заполнять огромную структуру нестал
    // можно получить непосредственный доступ к регистру DMA потока
    // через адрес потока


    uart->dma_rx->M0AR = (u32)uart->bufRx;
    uart->dma_rx->CR &= (~(1uL << 10));    // MINC = 0
    uart->dma_rx->CR |= uart->minc;

    uart->dma_rx->NDTR = uart->size_buf;  // в циклическом режиме этот параметр указывает
                                          // после достижения какого значения сбросить
                                          // указатель на буфер в исходное состояние
                                          // поэтому он на 1 больше размера буфера
    DMA_ClearFlag(uart->dma_rx, DMA_FLAG_TCIF5 | DMA_FLAG_TEIF5); // сбросить флаги прерывания по пересылке данных
    DMA_Cmd(uart->dma_rx, ENABLE);
}
//===================================================================
// Функкция сбрасывающяя ДМА, и старт передачи через ДМА
//===================================================================
void USART_DMA_Start_TX( portUart_type *uart )
{
//    struct stm32_uart_device *uart;
//    configASSERT(dev != RT_NULL);
//    uart = (struct stm32_uart_device*)dev->user_data;

    // сбрасываем DMA и запускаем на передачу канал
    uart->dma_tx->CR &= ~(uint32_t)DMA_SxCR_EN;

    uart->dma_tx->NDTR = uart->tx_size;

    DMA_ClearFlag(uart->dma_tx, DMA_FLAG_TCIF7 | DMA_FLAG_TEIF7); // сбросить флаги прерывания по пересылке данных
    uart->dma_tx->CR |= (uint32_t)DMA_SxCR_EN;

    // запрещаем прерывания по ошибкам UART и разрешаем передачу через DMA
    uart->reg->CR3 = USART_CR3_DMAT;

    // Разрешаем работу передатчика, прерывания окончания передачи и UART
    uart->reg->CR1 = USART_CR1_TCIE   | // Tranciever Interrupt Complete Enable
                     USART_CR1_UE     | // Uart Enable
                     USART_CR1_TE;      // Tranciever Enable
}
//===================================================================
// Это обработчиик прерываний УАРТ для индукционной запитки
// основная особенность в том что выход на передачу производится
// только после наступления события IDLE, иначе получится встречная передача
// помним пульт необходимо постоянно заряжать посылая ему зарядные посылки
// обработчик прерывания полностью разгружен на задаче TaskUart
//===================================================================
void rt_hw_serial_isr( portUart_type *uart)
{
//    struct stm32_uart_device *uart;
    volatile u8             temp;   // Регистр для холостого чтения данных
    u16                     cntRxData;

//    device_copy = dev;
//    configASSERT(dev != RT_NULL);
//    uart = (struct stm32_uart_device*)dev->user_data;

    // ***********************
    // возникли ошибки в канале
    if (uart->reg->SR & (USART_SR_PE | USART_SR_FE |   // Parity Error Framing Error
                         USART_SR_NE | USART_SR_ORE)){ // Noise Error Flag  Overrun Error Flag

        temp = uart->reg->DR;
        uart->reg->SR = ~USART_SR_TC; // Сбросить флаги прерываний, Uart_size_RX использ как temp
        temp = uart->reg->DR;
        temp = uart->reg->DR;
        startRxIdle(uart);
    }

    if(uart->reg->SR & USART_SR_IDLE && (uart->reg->CR1 & USART_CR1_RE )){      //  Обработка простоя на линии
        uart->reg->CR1 &= ~(USART_CR1_RE | USART_CR1_IDLEIE | USART_CR1_PEIE);
        DMA_Cmd(uart->dma_rx, DISABLE);     // запретить работу потока
        temp = uart->reg->DR;               // Сбросить прерывание по порту

        if(uart->stateTA == WaiteIdleEnd){  // Ждем паузы
            if( uart->state == TX_READY ){  // Есть чего послать
                uart->stateTA = WaiteRxEnd;
                USART_DMA_Start_TX(uart);   // Запускаем передатчик

                TIMuartWdStart(time9( uart->timeWaiteRx ));
            }
            else{
                startRxIdle(uart);
            }
        }
        else{                               // Ждем ответа
            TIMuartWdSop();                 // Ответ пришел остановим таймер

            cntRxData = (uart->size_buf) - uart->dma_rx->NDTR; // количество принятых байт полезной посылки
            uart->rx_size = cntRxData;
            startRxIdle(uart);
            uart1.state = RX_READY;
            OSSemPost(UARTSem);             // Сообщение в задачу
        }
    }

    if( (uart->reg->SR & USART_SR_TC) && (uart->reg->CR1 & USART_CR1_TCIE ) ){    // обработка прерывания от передатчика
        // передача окончена флаг прерывания сбрасывается по записи в регистр DR
        uart->state = TX_COMPLETE;          // Прочищаем запрос

        uart->reg->SR  = 0x00;              // Прочистить все флаги

        uart->reg->CR3 &= ~USART_CR3_DMAT;
        uart->reg->CR1 &= ~USART_CR1_TCIE;
        uart->reg->CR1 |= USART_CR1_RE;

//        uart->reg->SR  &= (~USART_SR_TC);

        startRxReceive(uart);               // Ждем ответа
    }
}

#ifdef USING_UART1
//===================================================================
//===================================================================
inline void TIMuartWdStart( u16 timeStop )
{
    TIM9->ARR = timeStop;
    TIM_Cmd(TIM9, ENABLE);                      // Включить таймер
//BLoff();
    TIM9->DIER = 1;                             // Разрешить прерывания
}
//===================================================================
//===================================================================
inline void TIMuartWdSop(void)
{
    TIM_Cmd(TIM9, DISABLE);                     // Выключить таймер
//BLon();
    TIM9->DIER = 0;                             // Запретить прерывания
    TIM9->EGR = 1;                              // Обновить таймер прочистить все в том числе и прескаллер
    TIM_ClearITPendingBit(TIM9, TIM_IT_Update); // Сбросить запрос на прерывание
}
//===================================================================
//===================================================================
inline void startRxIdle( portUart_type *uart)
{

    uart->stateTA = WaiteIdleEnd;

    uart->bufRx = &dumpRx;              // Переопределить адрес
    uart->minc  = MINC_DISABLE;         // MINC = 0

    USART_DMA_Start_RX(uart);       // Запускаем приемник
}
//===================================================================
//===================================================================
inline void startRxReceive( portUart_type *uart)
{
    uart->stateTA = WaiteRxEnd;

    uart->bufRx = UartBufRx;       // Переопределить адрес
    uart->minc  = MINC_ENABLE;      // MINC = 1

    USART_DMA_Start_RX(uart);       // Запускаем приемник
}
//===================================================================
void USART1_IRQHandler(void)
{
    OSIntEnter ();
        rt_hw_serial_isr(&uart1);
    OSIntExit();
}
#endif  // USING_UART1
//===================================================================
// Инициализация таймера для функции WD для UART по не приходу ответа
//===================================================================
void TIM9MicroInit(void)
{
    NVIC_InitTypeDef                NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef         TIM_TimeBaseStructure;

    NVIC_InitStructure.NVIC_IRQChannel                      = TIM1_BRK_TIM9_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 15;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
    DBGMCU_APB1PeriphConfig(DBGMCU_TIM9_STOP, ENABLE);

    TIM_TimeBaseStructure.TIM_Prescaler       = (tim9PSinit-1);//  60000 * 4;//4мс
    TIM_TimeBaseStructure.TIM_Period          = time9( 4.5 );

    TIM_TimeBaseStructure.TIM_ClockDivision   = 0;
    TIM_TimeBaseStructure.TIM_CounterMode     = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);

    DBGMCU_APB2PeriphConfig(DBGMCU_TIM9_STOP, ENABLE);

    TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);
    TIM_ClearITPendingBit(TIM9, TIM_IT_Update);
    TIM_ITConfig(TIM9, TIM_IT_Update, ENABLE);

    TIM_SelectOnePulseMode(TIM9, TIM_OPMode_Single);
}
//===================================================================
// Обработчик прерывания по таймеру для отсчета защитного интервалла
// по отсктствию ответа по порту
//===================================================================
void TIM1_BRK_TIM9_IRQHandler(void)
{
//EndMeasEvent(0);
//BeginMeasEvent(0);

    OSIntEnter ();

        TIMuartWdSop();
        startRxIdle(&uart1);

        uart1.state = RX_TIME_ERR;      // Ошибка приема
        OSSemPost(UARTSem);             // Сообщение в задачу

    OSIntExit();

    return;
}
//===================================================================
//=============  End of file  =======================================

