#ifndef   KBD_H
#define   KBD_H

//===================================================================
//              Скан коды кнопок пульта
//===================================================================
typedef enum{
        NON_KEY			,	// Не нажата ни одна клавиша

        KEY_FUNC        ,   // К1
        KEY_UP          ,
        KEY_DOWN        ,

        KEY_ESC         ,   // К2
        KEY_STOP        ,
        KEY_START       ,

        KEY_RIGHT       ,   // К3
        KEY_LEFT        ,
        KEY_ENTER       ,

        NUM_of_KEY          // Количество клавиш всего в наличии
}keyName_type;
//===================================================================
//===================================================================
/*                  Константы                                      */
//===================================================================
#ifdef _PROJECT_IAR_
    #define     KBD_delay_TYME  1   // Время вызова программного потока опросс
    #define     DEBOUNCEtime    20      /* Время подавления дребезга клавиш */
#else
    #define     KBD_delay_TYME  1
    #define     DEBOUNCEtime    1      /* Время подавления дребезга клавиш */
#endif

//#define     DEBOUNCEtime    20      /* Время подавления дребезга клавиш */
#define     kTyme0	        (80 /KBD_delay_TYME) //(250 /KBD_delay_TYME)    /* Время ожидания от нажатия до первого изменение значения */
#define     kTyme1	        (50 /KBD_delay_TYME) //(150 /KBD_delay_TYME)    /* Время изменения параметра на первой скорости */
#define     kTyme2	        (23 /KBD_delay_TYME) //(70  /KBD_delay_TYME)    /* Время изменения параметра на второй скорости */
#define	    kCnt1	        3       /* Количество изменений на первой скорости */
//===================================================================
//              описатель сообщения о нажатии клавиши
//===================================================================
typedef struct{
        keyName_type        Code;           // Код нажатой клавиши
        u8                  keyPressed;     // Бит нажатя клавиш UP DOWN
}MsgKbd_type;
//===================================================================
//              Состояние клавиши
//===================================================================
typedef enum{
        kOFF,       // Отпущена
        kONv0,      // Зажата на 0 скорости
        kONv1,      // Зажата на 1 скорости
        kONv2,      // Зажата на 2 скорости
}stateAutoKey_type;
//===================================================================
//      Описатель клавиши UP DOWN при длительном нажатии
//===================================================================
typedef struct{
    u16  	            TimerV0;
    u16  	            TimerV1;
    u16  	            TimerV2;
    u16		            cnt;
    u8		            maskVgen;		// Маска сгенерированных событий
    stateAutoKey_type   state;
}autoRepeatStateKey_type;
//===================================================================
//       Описатель кнопки для подавления дребезга
//===================================================================
typedef struct{
    u8  	TimeDrebezg;
    u8      State;
}StateKbd_type;
//===================================================================
//              Тип с состоянием кнопок
//===================================================================
typedef union{
        u16     all;
        struct{
            u16     NON_KEY :1;     // Фиктивное поле

            u16		FUNC    :1;     // К1
            u16		UP      :1;
            u16		DOWN    :1;

            u16		CANCEL  :1;     // К2
            u16		START   :1;
            u16     STOP    :1;

            u16		RIGHT   :1;     // К3
            u16		LEFT    :1;
            u16		ENTER   :1;

            u16		DEBUG_key :1;
        }bit;
}keyState_type;
//======================================================================
// Структура для сохранения состояния клавиш с учетом автоповтора
//======================================================================
typedef union {
    u32     all;
    struct{
        keyState_type   keyState;
        u8              autoMask;
    };
}currStateKey_type;
//===================================================================
// макросы для управления светодиодами
// я использую макросы определения типа выходного канала!!! обьясняю - так как
// во избежании выхода из строя порта настроеного на выход(при засветке светодиодов
// при замкнутых двух кнопках в ряду один порт может быть в низком состоянии другой в высоком)
// поэтому гашение светодиода выполняю Z состоянием
//===================================================================
#define LED_FAULT          GPIO_MODER_MODER11_0
#define LED_RUN            GPIO_MODER_MODER12_0
#define LED_READY          GPIO_MODER_MODER13_0
#define LED_LOC            GPIO_MODER_MODER14_0
#define LED_RMT            GPIO_MODER_MODER15_0

// засвечиваем группой
#define LED_ON(leds)  {do{extern unsigned int LedStateReg; LedStateReg |= leds;}while(0);}
// тушим группой
#define LED_OFF(leds) {do{extern unsigned int LedStateReg; LedStateReg &= ~(leds);}while(0);}
//===================================================================
//      это уйдет в драйвер Дисплея!!!
//===================================================================
#ifdef _PROJECT_IAR_
#define LCD_BACK_LIGHT_SET() {do {u16 period, pulse;                                                 \
                                  extern u8 Lcd_driver_init;                                         \
                                  extern u16 brightness1;                                            \
                                  extern unsigned char Sobitie_ot_polzovatelya;                      \
                                  extern unsigned char Sobitie_ot_polzovatelya_;                     \
                                  if (Lcd_driver_init){                                              \
                                  period = (SystemCoreClock / 20000 ) - 1;                           \
                                  if (brightness1 < 1000) brightness1++; else {if (Sobitie_ot_polzovatelya_ < 2) Sobitie_ot_polzovatelya_++; else Sobitie_ot_polzovatelya_ = 1; Sobitie_ot_polzovatelya = Sobitie_ot_polzovatelya_; brightness1 = 0;}          \
                                  pulse = (uint16_t) (((uint32_t) brightness1 * (period - 1)) / 1000);\
                                  TIM3->CCR3 = pulse;}\
                                 }while(0);}
#else
#define LCD_BACK_LIGHT_SET()
#endif
//===================================================================
//              Глобальные переменные
//===================================================================

extern StateKbd_type  StateKbd[ NUM_of_KEY + 1 ];

//===================================================================
//                      Прототипы
//===================================================================
__noreturn void TaskKBD(void *pData);

//void SwitchToLedControl(void);
//void SwitchToReadKey(void);
//void InitKeyboardDriver(void);

void SET_KEY_D1(void);
void SET_KEY_D2(void);

#ifdef _PROJECT_IAR_
    void initKeyMK(void);
    #define initKey()   initKeyMK()

    keyState_type readKeyMK(void);
    #define readKey()   readKeyMK()
    
#else
    #define initKey()
    keyState_type readKeyPC(void);
    #define readKey() readKeyPC()
#endif



keyName_type findFirstKey( keyState_type  *keyState );
u8 autoRepeatKey( u16 state, autoRepeatStateKey_type *ARSKey );
keyState_type  filtrKey( keyState_type  *keyState, StateKbd_type	*stateKbd );

keyName_type WaitKey(u8 *keyPressed );

//===================================================================
#endif  // KBD_H
