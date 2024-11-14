#ifndef LED_H
#define LED_H

typedef union{
        u32     all;
        u16     word[2];
        struct{
			u16		fict1		:15;	// Фиктивное поле
            u16		R           :1;     // PA15
			// ======
			u16		fict2		:4;		// Фиктивное поле
            u16		L           :1;     // PC4
			u16		fict3		:5;		// Фиктивное поле
            u16     Breakdown   :1;     // PC10
            u16		Ready       :1;     // PC11
            u16     Work        :1;		// PC12
        }/*bit*/;
}LEDState_type;

extern LEDState_type LEDState;

// Работа
#define ledWorkOn(led)    { led.Work = 1; }
#define ledWorkOff(led)   { led.Work = 0; }
// Готов
#define ledReadyOn(led)    { led.Ready = 1; }
#define ledReadyOff(led)   { led.Ready = 0; }
// Авария
#define ledFaultOn(led)    { led.Breakdown = 1; }
#define ledFaultOff(led)   { led.Breakdown = 0; }
// Местный
#define ledLocalOn(led)    { led.L = 1; }
#define ledLocalOff(led)   { led.L = 0; }
// АСУ/ДУ
#define ledRemotOn(led)    { led.R = 1; }
#define ledRemotOff(led)   { led.R = 0; }

// Выключить все
#define ledOff(led)        { led.all = 0; }

// Включить все
#define ledOn(led)         { led.all = ( (1UL<<15)+ (1UL<<(4+16)) + (1UL<<(10+16)) + (1UL<<(11+16)) + (1UL<<(12+16)) ) ; }


#ifdef _PROJECT_IAR_
/*
	// Работа
	#define ledWorkOn()    { GPIOC->BSRRH = GPIO_BSRR_BS_12; }
	#define ledWorkOff()   { GPIOC->BSRRL = GPIO_BSRR_BS_12; }
	// Готов
	#define ledReadyOn()    { GPIOC->BSRRH = GPIO_BSRR_BS_11; }
	#define ledReadyOff()   { GPIOC->BSRRL = GPIO_BSRR_BS_11; }
	// Авария
	#define ledFaultOn()    { GPIOC->BSRRH = GPIO_BSRR_BS_10; }
	#define ledFaultOff()   { GPIOC->BSRRL = GPIO_BSRR_BS_10; }
	// Местный
	#define ledLocalOn()    { GPIOC->BSRRH = GPIO_BSRR_BS_4; }
	#define ledLocalOff()   { GPIOC->BSRRL = GPIO_BSRR_BS_4; }
	// АСУ/ДУ
	#define ledRemotOn()    { GPIOA->BSRRH = GPIO_BSRR_BS_15; }
	#define ledRemotOff()   { GPIOA->BSRRL = GPIO_BSRR_BS_15; }
	
	// Выключить все
	#define ledOff()                                                                                \
	{                                                                                               \
	    GPIOC->BSRRL = ( GPIO_BSRR_BS_10 | GPIO_BSRR_BS_11 | GPIO_BSRR_BS_12 | GPIO_BSRR_BS_4 );    \
	    GPIOA->BSRRL =   GPIO_BSRR_BS_15;                                                           \
	}
	
	// Включить все
	#define ledOn()                                                                                 \
	{                                                                                               \
	    GPIOC->BSRRH = ( GPIO_BSRR_BS_10 | GPIO_BSRR_BS_11 | GPIO_BSRR_BS_12 | GPIO_BSRR_BS_4 );    \
	    GPIOA->BSRRH =   GPIO_BSRR_BS_15;                                                           \
	}
*/
	void initLED(void);
#else
    #define initLED()
#endif	// _PROJECT_IAR_


void testLed(void);
void forceLED(void);


#endif // LED_H
