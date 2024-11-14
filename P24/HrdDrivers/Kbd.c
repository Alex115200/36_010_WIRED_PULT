
#include "globalIncludes.h"

StateKbd_type  StateKbd[ NUM_of_KEY + 1 ];


// ==================================================================
//          Разобраться и выкинуть в другое место
//          Разобраться и выкинуть в другое место
//          Разобраться и выкинуть в другое место
//          Разобраться и выкинуть в другое место
// ==================================================================

unsigned char Sobitie_ot_polzovatelya = 0;
unsigned char Sobitie_ot_polzovatelya_ = 1;
unsigned int LedStateReg = 0;

#ifdef DYNAMIC_UPDATE
  unsigned int LesStateShiftDinamicUpdate = 0xC0000000;
#endif
//===================================================================
//          Разобраться и выкинуть в другое место
//          Разобраться и выкинуть в другое место
//          Разобраться и выкинуть в другое место
//===================================================================
//===================================================================
//===================================================================


//===================================================================
// Функция просчитывает состояние клавиши с учетом антидребезга
// Получает указатель на описатель клавиши и возвращает состояние c
// учетом фильтрации
//===================================================================
#ifdef _PROJECT_IAR_
  keyState_type  filtrKey( keyState_type  *keyState, StateKbd_type	*stateKbd )
{
	u32						maskKey;			// Маска клавиш
	static keyState_type	keyStateFiltr;		// Маска отфильтрованных клавиш

	maskKey = 0x001;
	
	for( maskKey = 0x02 ;  maskKey <= ( 1<< NUM_of_KEY ) ; maskKey <<= 1 ){	// 0х01 пропускаем

		stateKbd++;								// 0 - пропускаем

		if( keyState->all & maskKey ){			// Клавиша нажата
			if(stateKbd->TimeDrebezg <= DEBOUNCEtime ){
				stateKbd->TimeDrebezg++;
			}
			else{
				keyStateFiltr.all |= maskKey;   // Установить бит нажатой клавиши
			}
		}
		else{									// Клавиша отпущена
			if(stateKbd->TimeDrebezg > 0 ){
				stateKbd->TimeDrebezg--;
			}
			else{
				keyStateFiltr.all &= ~maskKey;  // C,hjcbnm бит нажатой клавиши
			}
		}
	}

    return  keyStateFiltr;
}

#else
  keyState_type  filtrKey( keyState_type  *keyStateLoc, StateKbd_type	*stateKbd )
{
    return          *keyStateLoc;
}
#endif

//===================================================================
//              Поиск первой нажатой клавиши
//===================================================================
keyName_type findFirstKey( keyState_type  *keyState )
{
    keyName_type            firstKey;
    u16                     maskKey;

    maskKey = 0x01;
    for(firstKey = NON_KEY; firstKey < NUM_of_KEY; firstKey++ ){
        if( (keyState->all & maskKey) != 0  ){
            break;  // Уже нашли
        }
        maskKey <<= 1;
    }

    if(firstKey == NUM_of_KEY){
        firstKey = NON_KEY;
    }

    return firstKey;
}
//===================================================================
// Функция генерирует автоповтор при длительном нажатии клавиши
//===================================================================
u8 autoRepeatKey( u16 state, autoRepeatStateKey_type *ARSKey )
{
    if(state == 0){
        ARSKey->state = kOFF;       // Прочистить состояние структуры
    }

    switch(ARSKey->state){
        case kOFF:
    		ARSKey->TimerV0 	= 0;
			ARSKey->maskVgen	= 0;
            ARSKey->state 	= kONv0;
            break;

        case kONv0:
        	if(ARSKey->TimerV0 < kTyme0 ){
        		ARSKey->TimerV0++;
        	}
        	else{
	    		ARSKey->cnt		= 0;
	    		ARSKey->TimerV1 = 0;
        		ARSKey->state 	= kONv1;
        	}
            break;

        case kONv1:
			if(ARSKey->TimerV1 < kTyme1){
				ARSKey->TimerV1++;
			}
			else{
				if(ARSKey->cnt < kCnt1 ){
					ARSKey->cnt++;
					ARSKey->TimerV1 	= 0;
					ARSKey->maskVgen	= (1<<0);
				}
				else{
					ARSKey->cnt			= 0;
					ARSKey->TimerV2 	= 0;
					ARSKey->state 		= kONv2;
				}
			}
            break;

        case kONv2:
			if(ARSKey->TimerV2 < kTyme2){
				ARSKey->TimerV2++;
			}
			else{
				ARSKey->TimerV2 	= 0;
				ARSKey->maskVgen	= (1<<1);
			}
            break;

        default:
        	ARSKey->state = kOFF;
            break;
    }

    return ARSKey->maskVgen;
}
//===================================================================
// Короткая задержка для опроса клавиатуры пока не установится сигнал на входе
//===================================================================
void waiteKBDSet(void)
{
    int time;
    for( time=10;  time; time-- );  // Подождем пока устаканится
}
//===================================================================
#ifdef _PROJECT_IAR_
void waiteKBDnop(void)
{
    asm(" nop");
    asm(" nop");
}
#endif
//===================================================================
// Функция снятия состояния клавиатуры
//===================================================================
#ifdef _PROJECT_IAR_

#define kbdMaskRd (GPIO_IDR_IDR_13 | GPIO_IDR_IDR_14 | GPIO_IDR_IDR_15 )
keyState_type readKeyMK(void)
{
    keyState_type           keyStateL;

	GPIOB->BSRRL = ( GPIO_BSRR_BS_10 | GPIO_BSRR_BS_11 | GPIO_BSRR_BS_3);   // Установить ноги в высокое состояние как начальное
	GPIOB->BSRRH = GPIO_BSRR_BS_11 ;	                                    // Подсветить низким К1
	//waiteKBDSet();
        OSTimeDly(1);
    keyStateL.all = ((GPIOC->IDR & kbdMaskRd) >> 12 );                      // Снять данные

	GPIOB->BSRRL = ( GPIO_BSRR_BS_10 | GPIO_BSRR_BS_11 | GPIO_BSRR_BS_3);	// Установить ноги в высокое состояние как начальное
	GPIOB->BSRRH = GPIO_BSRR_BS_10 ;	                                    // Подсветить низким К2
	//waiteKBDSet();
        OSTimeDly(1);
    keyStateL.all |=((GPIOC->IDR & kbdMaskRd) >> (13-4) );                  // Снять данные

	GPIOB->BSRRL = ( GPIO_BSRR_BS_10 | GPIO_BSRR_BS_11 | GPIO_BSRR_BS_3);	// Установить ноги в высокое состояние как начальное
	GPIOB->BSRRH = GPIO_BSRR_BS_3 ;	                                        // Подсветить низким К2
	//waiteKBDSet();
        OSTimeDly(1);
    keyStateL.all |=((GPIOC->IDR & kbdMaskRd) >> (13-7) );                  // Снять данные

	GPIOB->BSRRL = ( GPIO_BSRR_BS_10 | GPIO_BSRR_BS_11 | GPIO_BSRR_BS_3);	// Убрать подсветку клавиш

    keyStateL.all =  ~keyStateL.all & 0x03FE;
    return  keyStateL;
}
//===================================================================
// Начальная инициализация клавиатуры проводится один раз по старту программы
//===================================================================
void initKeyMK(void)
{
	RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

	// Настройка порта для подсветки клавиатуры
	GPIOB->BSRRL = ( GPIO_BSRR_BS_10 | GPIO_BSRR_BS_11 | GPIO_BSRR_BS_3);				// Установить ноги в высокое состояние как начальное
	
	GPIOB->OTYPER |= (GPIO_OTYPER_OT_10 | GPIO_OTYPER_OT_11 | GPIO_OTYPER_OT_3 );		// Как ОТКР КОЛЛЕКТОР
	GPIOB->MODER &= ~(GPIO_MODER_MODER10 | GPIO_MODER_MODER11 | GPIO_MODER_MODER3);		// Прочистить назначение
	GPIOB->MODER |= GPIO_MODER_MODER10_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER3_0;	// Назначить как выходы
	
	GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR10   | GPIO_OSPEEDER_OSPEEDR11   | GPIO_OSPEEDER_OSPEEDR3 );
	GPIOB->OSPEEDR |=  (GPIO_OSPEEDER_OSPEEDR10_1 | GPIO_OSPEEDER_OSPEEDR11_1 | GPIO_OSPEEDER_OSPEEDR3_1);	// скорость 50 МГц
	
	// Настройка порта для вычитки состояния клавиатуры
	GPIOC->MODER &= ~(GPIO_MODER_MODER13 | GPIO_MODER_MODER14 | GPIO_MODER_MODER15 );	// Прочистить назначение (назначит входами)

	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR13   | GPIO_OSPEEDER_OSPEEDR14   | GPIO_OSPEEDER_OSPEEDR15  );
	GPIOC->OSPEEDR |=  (GPIO_OSPEEDER_OSPEEDR13_1 | GPIO_OSPEEDER_OSPEEDR14_1 | GPIO_OSPEEDER_OSPEEDR15_1);	// скорость 50 МГц
}
#endif
//===================================================================
// Функция ожидания нажатия клавиши. Вызывается из потока LCD
//===================================================================
keyName_type WaitKey(u8 *keyPressed )
{
    u8                      err;
    MsgKbd_type             *pntMsgKbd, MsgKbd;

    err = OSMboxPost(MBoxKbdReq, &MsgKbd);                  // Послать запрос в задачу клавиатуры
    pntMsgKbd = OSMboxPend(MBoxKbdResp, SYS_FOREVER, &err); // Ожидаем ответа

    EndMeasEvent(1);
    BeginMeasEvent(1);

    if (err == OS_ERR_NONE) {
        if(keyPressed != NULL){
            *keyPressed = pntMsgKbd->keyPressed;
        }
        return pntMsgKbd->Code;
    }
    else{
        if(keyPressed != NULL){
            *keyPressed = pntMsgKbd->keyPressed;
        }
        *keyPressed = 0;
        return NON_KEY;
    }
}
//===================================================================
//======== Подвал = Подвал = Подвал =================================
//======== Подвал = Подвал = Подвал =================================
//===================================================================
// перенастройка порта управления клавиатурой на управление светодиодами
// из-за упрощения в схемотехнике алгоритм предусматривает, защиту от
// нежелательных состояний порта при нажатии кнопок во время управления светодиодами(см. схему пульта и клавиатуры)
// для наглядного понимания логики работы драйвера используется непосредственное
// управление регистрами без использования StdPeripheralLibrary
//===================================================================
#ifdef _PROJECT_IAR_
void SwitchToLedControl(void)
{
    // перед тем как поставить линии D1, D2 на вход желательно разрядить собственные
    // емкости GPIO к которым подключены линии D1,D2(садим выходы на землю)
    GPIOB->BSRRH |= GPIO_BSRR_BS_10 | GPIO_BSRR_BS_11;

    // переключаю D1, D2 в Z состояние
    GPIOB->MODER &= ~(GPIO_MODER_MODER10 | GPIO_MODER_MODER11);
    GPIOB->MODER |=  (//GPIO_MODER_MODER10_1 |   GPIO_MODER_MODER10_0 | // Input
                      //GPIO_MODER_MODER11_1 |   GPIO_MODER_MODER11_0 | // Input
                      0);
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_10 | GPIO_OTYPER_OT_11);
    GPIOB->OTYPER |= (//GPIO_OTYPER_OT_10  | // Dont care
                      //GPIO_OTYPER_OT_11  | // Dont care
                      0);
    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR10 | GPIO_OSPEEDER_OSPEEDR11);
    GPIOB->OSPEEDR |= (//GPIO_OSPEEDER_OSPEEDR10_1  | GPIO_OSPEEDER_OSPEEDR10_0 // Dont care
                       //GPIO_OSPEEDER_OSPEEDR11_1  | GPIO_OSPEEDER_OSPEEDR11_0 // Dont care
                      0);
    // подтяжки на землю отключаю
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR10 | GPIO_PUPDR_PUPDR11);
    GPIOB->PUPDR |= (/*GPIO_PUPDR_PUPDR10_1 |*/ /*GPIO_PUPDR_PUPDR10_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     /*GPIO_PUPDR_PUPDR11_1 |*/ /*GPIO_PUPDR_PUPDR11_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     0);

    // открываю транзистор итания светодиодов
    GPIOC->BSRRL |= GPIO_BSRR_BS_5;

    // для предотвращения попадания высокого состояния порта на низкое состояние
    // во время управления светодиодами через две нажатые кнопки в группе
    // управление светодиодами осуществляется через выход/Z-состояние

    // переключаю K1-K5 в соответствии с параметром хранящим состояние светодиодов которые обьявлены
    // в виде макросов и являются псевдонимами на соотв. каждому светодиоду маску битов управления регистром MODER
    GPIOC->MODER &= ~(GPIO_MODER_MODER11 | GPIO_MODER_MODER12 | GPIO_MODER_MODER13  | GPIO_MODER_MODER14 | GPIO_MODER_MODER15);
    //GPIOC->MODER |=  (/*GPIO_MODER_MODER11_1 |*/   GPIO_MODER_MODER11_0 | // Output
    //                  /*GPIO_MODER_MODER12_1 |*/   GPIO_MODER_MODER12_0 | // Output
    //                  /*GPIO_MODER_MODER13_1 |*/   GPIO_MODER_MODER13_0 | // Output
    //                  /*GPIO_MODER_MODER14_1 |*/   GPIO_MODER_MODER14_0 | // Output
    //                  /*GPIO_MODER_MODER15_1 |*/   GPIO_MODER_MODER15_0 | // Output
    //                  0);
#ifdef DYNAMIC_UPDATE
    GPIOC->MODER |= (LedStateReg & LesStateShiftDinamicUpdate);
    if (LesStateShiftDinamicUpdate != 0x00C00000) LesStateShiftDinamicUpdate >>= 2;
    else LesStateShiftDinamicUpdate = 0xC0000000;
#else
    GPIOC->MODER |= LedStateReg;
#endif
    // выходы Push-Pull
    // входы Don`t care
    GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_11 | GPIO_OTYPER_OT_12 | GPIO_OTYPER_OT_13 | GPIO_OTYPER_OT_14 | GPIO_OTYPER_OT_15);
    GPIOC->OTYPER |= (//GPIO_OTYPER_OT_11  | // 0 - PUSHPULL / 1 - OD
                      //GPIO_OTYPER_OT_12  | // 0 - PUSHPULL / 1 - OD
                      //GPIO_OTYPER_OT_13  | // 0 - PUSHPULL / 1 - OD
                      //GPIO_OTYPER_OT_14  | // 0 - PUSHPULL / 1 - OD
                      //GPIO_OTYPER_OT_15  | // 0 - PUSHPULL / 1 - OD
                      0);
    // выходы самые низкие по скорости
    // входы Don`t care
    GPIOC->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR11 | GPIO_OSPEEDER_OSPEEDR12 | GPIO_OSPEEDER_OSPEEDR13 | GPIO_OSPEEDER_OSPEEDR14 | GPIO_OSPEEDER_OSPEEDR15);
    GPIOC->OSPEEDR |= (//GPIO_OSPEEDER_OSPEEDR11_1  | GPIO_OSPEEDER_OSPEEDR11_0 // Самая низкая
                      //GPIO_OSPEEDER_OSPEEDR12_1  | GPIO_OSPEEDER_OSPEEDR12_0 //  -//-
                      //GPIO_OSPEEDER_OSPEEDR13_1  | GPIO_OSPEEDER_OSPEEDR13_0 //
                      //GPIO_OSPEEDER_OSPEEDR14_1  | GPIO_OSPEEDER_OSPEEDR14_0 //
                      //GPIO_OSPEEDER_OSPEEDR15_1  | GPIO_OSPEEDER_OSPEEDR15_0 //
                      0);

    // а вот пуллапы можно оставить на землю нестрашно
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR11 | GPIO_PUPDR_PUPDR12 | GPIO_PUPDR_PUPDR13 | GPIO_PUPDR_PUPDR14 | GPIO_PUPDR_PUPDR15);
    GPIOC->PUPDR |= (GPIO_PUPDR_PUPDR11_1 |/* GPIO_PUPDR_PUPDR11_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR12_1 |/* GPIO_PUPDR_PUPDR12_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR13_1 |/* GPIO_PUPDR_PUPDR13_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR14_1 |/* GPIO_PUPDR_PUPDR14_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR15_1 |/* GPIO_PUPDR_PUPDR15_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     0);

    // для предотвращения попадания высокого состояния порта на низкое состояние
    // во время управления светодиодами через две нажатые кнопки в группе,
    // управление светодиодами осуществляется через выход/Z-состояние
    // поэтому регистр состояния выхода для всех пятисветодиодов постоянно будет равен высокому состоянию
    GPIOC->ODR |= (GPIO_ODR_ODR_11 | GPIO_ODR_ODR_12 | GPIO_ODR_ODR_13 | GPIO_ODR_ODR_14 | GPIO_ODR_ODR_15);
}


//===================================================================
// если выборка чтения клавиатуры происходит по линиям D1 D2
// (при отладке был обнаружен эфект ложного срабатывания при нажатии больше 2-х кнопок)
// ЭТО ЕСЛИ КНОПКИ НЕ СНАБЖЕНЫ ДИОДАМИ см. схему клавиатуры

// перенастройка порта управления клавиатурой на чтение кнопок
// подход тот-же что и в предыдущей функции(см. схему пульта и клавиатуры)
//===================================================================
void SwitchToReadKey(void)
{
    // отключаю общий катод светодиодов от земли
    GPIOC->BSRRH |= GPIO_BSRR_BS_5;

    // переключаю K1-K5 на вход Нога 10 работала вместе с K1 решили подтянуть на землю
    GPIOC->MODER &= ~(GPIO_MODER_MODER10 | GPIO_MODER_MODER11 | GPIO_MODER_MODER12 | GPIO_MODER_MODER13  | GPIO_MODER_MODER14 | GPIO_MODER_MODER15);
    GPIOC->MODER |=  (//GPIO_MODER_MODER10_1 |   GPIO_MODER_MODER10_0 | // Input
                      //GPIO_MODER_MODER11_1 |   GPIO_MODER_MODER11_0 | // Input
                      //GPIO_MODER_MODER12_1 |   GPIO_MODER_MODER12_0 | // Input
                      //GPIO_MODER_MODER13_1 |   GPIO_MODER_MODER13_0 | // Input
                      //GPIO_MODER_MODER14_1 |   GPIO_MODER_MODER14_0 | // Input
                      //GPIO_MODER_MODER15_1 |   GPIO_MODER_MODER15_0 | // Input
                      0);
    // Dont care!!! оставил для наглядности
    GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_10 | GPIO_OTYPER_OT_11 | GPIO_OTYPER_OT_12 | GPIO_OTYPER_OT_13 | GPIO_OTYPER_OT_14 | GPIO_OTYPER_OT_15);
    GPIOC->OTYPER |= (//GPIO_OTYPER_OT_10  | // 0 - PUSHPULL / 1 - OD
                      //GPIO_OTYPER_OT_11  | // 0 - PUSHPULL / 1 - OD
                      //GPIO_OTYPER_OT_12  | // 0 - PUSHPULL / 1 - OD
                      //GPIO_OTYPER_OT_13  | // 0 - PUSHPULL / 1 - OD
                      //GPIO_OTYPER_OT_14  | // 0 - PUSHPULL / 1 - OD
                      //GPIO_OTYPER_OT_15  | // 0 - PUSHPULL / 1 - OD
                      0);
    // Dont care!!! оставил для наглядности
    GPIOC->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR10 | GPIO_OSPEEDER_OSPEEDR11 | GPIO_OSPEEDER_OSPEEDR12 | GPIO_OSPEEDER_OSPEEDR13 | GPIO_OSPEEDER_OSPEEDR14 | GPIO_OSPEEDER_OSPEEDR15);
    GPIOC->OSPEEDR |= (//GPIO_OSPEEDER_OSPEEDR10_1  | GPIO_OSPEEDER_OSPEEDR10_0 //
                       //GPIO_OSPEEDER_OSPEEDR11_1  | GPIO_OSPEEDER_OSPEEDR11_0 //
                       //GPIO_OSPEEDER_OSPEEDR12_1  | GPIO_OSPEEDER_OSPEEDR12_0 //
                       //GPIO_OSPEEDER_OSPEEDR13_1  | GPIO_OSPEEDER_OSPEEDR13_0 //
                       //GPIO_OSPEEDER_OSPEEDR14_1  | GPIO_OSPEEDER_OSPEEDR14_0 //
                       //GPIO_OSPEEDER_OSPEEDR15_1  | GPIO_OSPEEDER_OSPEEDR15_0 //
                       0);
    // тянем входы K1 - K5 к земле
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR10 | GPIO_PUPDR_PUPDR11 | GPIO_PUPDR_PUPDR12 | GPIO_PUPDR_PUPDR13 | GPIO_PUPDR_PUPDR14 | GPIO_PUPDR_PUPDR15);
    GPIOC->PUPDR |= (GPIO_PUPDR_PUPDR10_1 | /*GPIO_PUPDR_PUPDR11_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR11_1 | /*GPIO_PUPDR_PUPDR11_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR12_1 | /*GPIO_PUPDR_PUPDR12_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR13_1 | /*GPIO_PUPDR_PUPDR13_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR14_1 | /*GPIO_PUPDR_PUPDR14_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR15_1 | /*GPIO_PUPDR_PUPDR15_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     0);

    // D1, D2 на выход включать вместе нестоит так как при управлении на этих выводах
    // должны быть разные уровни и во избежании замыкания D1 на D2 через две кноки
    // (на одном и том же канале K1-5, но двух разных линиях D1-2),
    // переключение порта в 1 комбинируется с Z состоянием(реализовано
    // в функциях управления ногами D1, D2)
}
//===================================================================
// изначально планировалось сделать эти функции макросами
// но учитывая что корректное изменение направления порта требует перезаписи
// нескольких регистров порта пришлось выполнить в виде функций
// также вызов функций позволяет выполнить необходимые задержки от момента включения линии
// D1 до момента считывания состояния кнопок K1-5
//===================================================================
void SET_KEY_D1(void)
{
    u8 i;

    // перед тем как поставить линию D2 на вход желательно разрядить собственную
    // емкость GPIO к которому подключена линия D2(садим выход на землю)
    GPIOB->BSRRH |= GPIO_BSRR_BS_11;

    GPIOB->MODER &= ~(GPIO_MODER_MODER10 | GPIO_MODER_MODER11);
    GPIOB->MODER |=  (/*GPIO_MODER_MODER10_1 |*/   GPIO_MODER_MODER10_0 | // Output
                      /*GPIO_MODER_MODER11_1 |   GPIO_MODER_MODER11_0 |*/ // Input
                      0);
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_10 | GPIO_OTYPER_OT_11);
    GPIOB->OTYPER |= (//GPIO_OTYPER_OT_10  | // 0 - PUSHPULL / 1 - OD
                      //GPIO_OTYPER_OT_11  | // Dont care
                      0);
    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR10 | GPIO_OSPEEDER_OSPEEDR11);
    GPIOB->OSPEEDR |= (//GPIO_OSPEEDER_OSPEEDR10_1  | GPIO_OSPEEDER_OSPEEDR10_0 // самая низкая
                       //GPIO_OSPEEDER_OSPEEDR11_1  | GPIO_OSPEEDER_OSPEEDR11_0 // Dont care
                      0);
    // обе линии D1, D2 решил подтянуть к земле(всетаки лучше чем Floating)
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR10 | GPIO_PUPDR_PUPDR11);
    GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR10_1 | /* GPIO_PUPDR_PUPDR10_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR11_1 | /* GPIO_PUPDR_PUPDR11_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     0);
    // сбросим в ноль оба канала(для входа это уже не имеет значения но всетаки...)
    //GPIOB->BSRRH |= GPIO_BSRR_BS_10 | GPIO_BSRR_BS_11;

    // установим высокий уровень на линии D1
    GPIOB->BSRRL |= GPIO_BSRR_BS_10;

    // подождем пока закончатся быстрые переходные процессы
    for(i = 0; i < 100; i++);
}
//===================================================================
// аналогично для D2 см. предыдущую функцию
//===================================================================
void SET_KEY_D2(void)
{
    u8 i;

    // перед тем как поставить линию D1 на вход желательно разрядить собственную
    // емкость GPIO к которому подключена линия D1(садим выход на землю)
    GPIOB->BSRRH |= GPIO_BSRR_BS_10;

    // теперь оба включаю на вход а потом ставлю на выход D2
    GPIOB->MODER &= ~(GPIO_MODER_MODER10 | GPIO_MODER_MODER11);
    GPIOB->MODER |=  (/*GPIO_MODER_MODER10_1 |   GPIO_MODER_MODER10_0 |*/ // Input
                      /*GPIO_MODER_MODER11_1 |*/   GPIO_MODER_MODER11_0 | // Output
                      0);
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_10 | GPIO_OTYPER_OT_11);
    GPIOB->OTYPER |= (//GPIO_OTYPER_OT_10  | // Dont care
                      //GPIO_OTYPER_OT_11  | // 0 - PUSHPULL / 1 - OD
                      0);
    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR10 | GPIO_OSPEEDER_OSPEEDR11);
    GPIOB->OSPEEDR |= (//GPIO_OSPEEDER_OSPEEDR10_1  | GPIO_OSPEEDER_OSPEEDR10_0 // Dont care
                       //GPIO_OSPEEDER_OSPEEDR11_1  | GPIO_OSPEEDER_OSPEEDR11_0 // самая низкая
                      0);
    // обе линии D1, D2 решил подтянуть к земле(всетаки лучше чем Floating)
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR10 | GPIO_PUPDR_PUPDR11);
    GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR10_1 | /* GPIO_PUPDR_PUPDR10_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR11_1 | /* GPIO_PUPDR_PUPDR11_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     0);
    // этот код (сброс обоих в ноль и сразу установка в 1) производил выбросы
    // сбросим в ноль оба канала(для входа это уже не имеет значения но всетаки...)
    //GPIOB->BSRRH |= GPIO_BSRR_BS_10 | GPIO_BSRR_BS_11;

    // установим высокий уровень на линии D2
    GPIOB->BSRRL |= GPIO_BSRR_BS_11;

    // подождем пока закончатся быстрые переходные процессы
    for(i = 0; i < 100; i++);
}

//===================================================================
// инициализация драйвера клавиатуры,
// настройка портов которые не меняются по ходу алгоритма,
// запуск периодической функции, создание очереди
//===================================================================
void InitKeyboardDriver(void)
{
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN |
                     RCC_AHB1ENR_GPIOCEN);

    GPIOC->MODER &= ~(GPIO_MODER_MODER5);
    GPIOC->MODER |=  (/*GPIO_MODER_MODER5_1 |*/   GPIO_MODER_MODER5_0 | // Input
                      0);
    GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_5);
    GPIOC->OTYPER |= (//GPIO_OTYPER_OT_5  | // 0 - PUSHPULL / 1 - OD
                      0);
    GPIOC->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR5);
    GPIOC->OSPEEDR |= (//GPIO_OSPEEDER_OSPEEDR5_1  | GPIO_OSPEEDER_OSPEEDR5_0 //
                      0);
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR5);
    GPIOC->PUPDR |= (//GPIO_PUPDR_PUPDR5_1 | GPIO_PUPDR_PUPDR5_0 | // 00-Z/01-PullUP/10-PullDOWN
                     0);
}
#endif
//===================================================================
//=============  End of file  =======================================
