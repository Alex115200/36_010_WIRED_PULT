
#include "globalIncludes.h"

StateKbd_type  StateKbd[ NUM_of_KEY + 1 ];


// ==================================================================
//          ����������� � �������� � ������ �����
//          ����������� � �������� � ������ �����
//          ����������� � �������� � ������ �����
//          ����������� � �������� � ������ �����
// ==================================================================

unsigned char Sobitie_ot_polzovatelya = 0;
unsigned char Sobitie_ot_polzovatelya_ = 1;
unsigned int LedStateReg = 0;

#ifdef DYNAMIC_UPDATE
  unsigned int LesStateShiftDinamicUpdate = 0xC0000000;
#endif
//===================================================================
//          ����������� � �������� � ������ �����
//          ����������� � �������� � ������ �����
//          ����������� � �������� � ������ �����
//===================================================================
//===================================================================
//===================================================================


//===================================================================
// ������� ������������ ��������� ������� � ������ ������������
// �������� ��������� �� ��������� ������� � ���������� ��������� c
// ������ ����������
//===================================================================
#ifdef _PROJECT_IAR_
  keyState_type  filtrKey( keyState_type  *keyState, StateKbd_type	*stateKbd )
{
	u32						maskKey;			// ����� ������
	static keyState_type	keyStateFiltr;		// ����� ��������������� ������

	maskKey = 0x001;
	
	for( maskKey = 0x02 ;  maskKey <= ( 1<< NUM_of_KEY ) ; maskKey <<= 1 ){	// 0�01 ����������

		stateKbd++;								// 0 - ����������

		if( keyState->all & maskKey ){			// ������� ������
			if(stateKbd->TimeDrebezg <= DEBOUNCEtime ){
				stateKbd->TimeDrebezg++;
			}
			else{
				keyStateFiltr.all |= maskKey;   // ���������� ��� ������� �������
			}
		}
		else{									// ������� ��������
			if(stateKbd->TimeDrebezg > 0 ){
				stateKbd->TimeDrebezg--;
			}
			else{
				keyStateFiltr.all &= ~maskKey;  // C,hjcbnm ��� ������� �������
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
//              ����� ������ ������� �������
//===================================================================
keyName_type findFirstKey( keyState_type  *keyState )
{
    keyName_type            firstKey;
    u16                     maskKey;

    maskKey = 0x01;
    for(firstKey = NON_KEY; firstKey < NUM_of_KEY; firstKey++ ){
        if( (keyState->all & maskKey) != 0  ){
            break;  // ��� �����
        }
        maskKey <<= 1;
    }

    if(firstKey == NUM_of_KEY){
        firstKey = NON_KEY;
    }

    return firstKey;
}
//===================================================================
// ������� ���������� ���������� ��� ���������� ������� �������
//===================================================================
u8 autoRepeatKey( u16 state, autoRepeatStateKey_type *ARSKey )
{
    if(state == 0){
        ARSKey->state = kOFF;       // ���������� ��������� ���������
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
// �������� �������� ��� ������ ���������� ���� �� ����������� ������ �� �����
//===================================================================
void waiteKBDSet(void)
{
    int time;
    for( time=10;  time; time-- );  // �������� ���� �����������
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
// ������� ������ ��������� ����������
//===================================================================
#ifdef _PROJECT_IAR_

#define kbdMaskRd (GPIO_IDR_IDR_13 | GPIO_IDR_IDR_14 | GPIO_IDR_IDR_15 )
keyState_type readKeyMK(void)
{
    keyState_type           keyStateL;

	GPIOB->BSRRL = ( GPIO_BSRR_BS_10 | GPIO_BSRR_BS_11 | GPIO_BSRR_BS_3);   // ���������� ���� � ������� ��������� ��� ���������
	GPIOB->BSRRH = GPIO_BSRR_BS_11 ;	                                    // ���������� ������ �1
	//waiteKBDSet();
        OSTimeDly(1);
    keyStateL.all = ((GPIOC->IDR & kbdMaskRd) >> 12 );                      // ����� ������

	GPIOB->BSRRL = ( GPIO_BSRR_BS_10 | GPIO_BSRR_BS_11 | GPIO_BSRR_BS_3);	// ���������� ���� � ������� ��������� ��� ���������
	GPIOB->BSRRH = GPIO_BSRR_BS_10 ;	                                    // ���������� ������ �2
	//waiteKBDSet();
        OSTimeDly(1);
    keyStateL.all |=((GPIOC->IDR & kbdMaskRd) >> (13-4) );                  // ����� ������

	GPIOB->BSRRL = ( GPIO_BSRR_BS_10 | GPIO_BSRR_BS_11 | GPIO_BSRR_BS_3);	// ���������� ���� � ������� ��������� ��� ���������
	GPIOB->BSRRH = GPIO_BSRR_BS_3 ;	                                        // ���������� ������ �2
	//waiteKBDSet();
        OSTimeDly(1);
    keyStateL.all |=((GPIOC->IDR & kbdMaskRd) >> (13-7) );                  // ����� ������

	GPIOB->BSRRL = ( GPIO_BSRR_BS_10 | GPIO_BSRR_BS_11 | GPIO_BSRR_BS_3);	// ������ ��������� ������

    keyStateL.all =  ~keyStateL.all & 0x03FE;
    return  keyStateL;
}
//===================================================================
// ��������� ������������� ���������� ���������� ���� ��� �� ������ ���������
//===================================================================
void initKeyMK(void)
{
	RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

	// ��������� ����� ��� ��������� ����������
	GPIOB->BSRRL = ( GPIO_BSRR_BS_10 | GPIO_BSRR_BS_11 | GPIO_BSRR_BS_3);				// ���������� ���� � ������� ��������� ��� ���������
	
	GPIOB->OTYPER |= (GPIO_OTYPER_OT_10 | GPIO_OTYPER_OT_11 | GPIO_OTYPER_OT_3 );		// ��� ���� ���������
	GPIOB->MODER &= ~(GPIO_MODER_MODER10 | GPIO_MODER_MODER11 | GPIO_MODER_MODER3);		// ���������� ����������
	GPIOB->MODER |= GPIO_MODER_MODER10_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER3_0;	// ��������� ��� ������
	
	GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR10   | GPIO_OSPEEDER_OSPEEDR11   | GPIO_OSPEEDER_OSPEEDR3 );
	GPIOB->OSPEEDR |=  (GPIO_OSPEEDER_OSPEEDR10_1 | GPIO_OSPEEDER_OSPEEDR11_1 | GPIO_OSPEEDER_OSPEEDR3_1);	// �������� 50 ���
	
	// ��������� ����� ��� ������� ��������� ����������
	GPIOC->MODER &= ~(GPIO_MODER_MODER13 | GPIO_MODER_MODER14 | GPIO_MODER_MODER15 );	// ���������� ���������� (�������� �������)

	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR13   | GPIO_OSPEEDER_OSPEEDR14   | GPIO_OSPEEDER_OSPEEDR15  );
	GPIOC->OSPEEDR |=  (GPIO_OSPEEDER_OSPEEDR13_1 | GPIO_OSPEEDER_OSPEEDR14_1 | GPIO_OSPEEDER_OSPEEDR15_1);	// �������� 50 ���
}
#endif
//===================================================================
// ������� �������� ������� �������. ���������� �� ������ LCD
//===================================================================
keyName_type WaitKey(u8 *keyPressed )
{
    u8                      err;
    MsgKbd_type             *pntMsgKbd, MsgKbd;

    err = OSMboxPost(MBoxKbdReq, &MsgKbd);                  // ������� ������ � ������ ����������
    pntMsgKbd = OSMboxPend(MBoxKbdResp, SYS_FOREVER, &err); // ������� ������

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
//======== ������ = ������ = ������ =================================
//======== ������ = ������ = ������ =================================
//===================================================================
// ������������� ����� ���������� ����������� �� ���������� ������������
// ��-�� ��������� � ������������ �������� ���������������, ������ ��
// ������������� ��������� ����� ��� ������� ������ �� ����� ���������� ������������(��. ����� ������ � ����������)
// ��� ���������� ��������� ������ ������ �������� ������������ ����������������
// ���������� ���������� ��� ������������� StdPeripheralLibrary
//===================================================================
#ifdef _PROJECT_IAR_
void SwitchToLedControl(void)
{
    // ����� ��� ��� ��������� ����� D1, D2 �� ���� ���������� ��������� �����������
    // ������� GPIO � ������� ���������� ����� D1,D2(����� ������ �� �����)
    GPIOB->BSRRH |= GPIO_BSRR_BS_10 | GPIO_BSRR_BS_11;

    // ���������� D1, D2 � Z ���������
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
    // �������� �� ����� ��������
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR10 | GPIO_PUPDR_PUPDR11);
    GPIOB->PUPDR |= (/*GPIO_PUPDR_PUPDR10_1 |*/ /*GPIO_PUPDR_PUPDR10_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     /*GPIO_PUPDR_PUPDR11_1 |*/ /*GPIO_PUPDR_PUPDR11_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     0);

    // �������� ���������� ������ �����������
    GPIOC->BSRRL |= GPIO_BSRR_BS_5;

    // ��� �������������� ��������� �������� ��������� ����� �� ������ ���������
    // �� ����� ���������� ������������ ����� ��� ������� ������ � ������
    // ���������� ������������ �������������� ����� �����/Z-���������

    // ���������� K1-K5 � ������������ � ���������� �������� ��������� ����������� ������� ���������
    // � ���� �������� � �������� ������������ �� �����. ������� ���������� ����� ����� ���������� ��������� MODER
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
    // ������ Push-Pull
    // ����� Don`t care
    GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_11 | GPIO_OTYPER_OT_12 | GPIO_OTYPER_OT_13 | GPIO_OTYPER_OT_14 | GPIO_OTYPER_OT_15);
    GPIOC->OTYPER |= (//GPIO_OTYPER_OT_11  | // 0 - PUSHPULL / 1 - OD
                      //GPIO_OTYPER_OT_12  | // 0 - PUSHPULL / 1 - OD
                      //GPIO_OTYPER_OT_13  | // 0 - PUSHPULL / 1 - OD
                      //GPIO_OTYPER_OT_14  | // 0 - PUSHPULL / 1 - OD
                      //GPIO_OTYPER_OT_15  | // 0 - PUSHPULL / 1 - OD
                      0);
    // ������ ����� ������ �� ��������
    // ����� Don`t care
    GPIOC->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR11 | GPIO_OSPEEDER_OSPEEDR12 | GPIO_OSPEEDER_OSPEEDR13 | GPIO_OSPEEDER_OSPEEDR14 | GPIO_OSPEEDER_OSPEEDR15);
    GPIOC->OSPEEDR |= (//GPIO_OSPEEDER_OSPEEDR11_1  | GPIO_OSPEEDER_OSPEEDR11_0 // ����� ������
                      //GPIO_OSPEEDER_OSPEEDR12_1  | GPIO_OSPEEDER_OSPEEDR12_0 //  -//-
                      //GPIO_OSPEEDER_OSPEEDR13_1  | GPIO_OSPEEDER_OSPEEDR13_0 //
                      //GPIO_OSPEEDER_OSPEEDR14_1  | GPIO_OSPEEDER_OSPEEDR14_0 //
                      //GPIO_OSPEEDER_OSPEEDR15_1  | GPIO_OSPEEDER_OSPEEDR15_0 //
                      0);

    // � ��� ������� ����� �������� �� ����� ���������
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR11 | GPIO_PUPDR_PUPDR12 | GPIO_PUPDR_PUPDR13 | GPIO_PUPDR_PUPDR14 | GPIO_PUPDR_PUPDR15);
    GPIOC->PUPDR |= (GPIO_PUPDR_PUPDR11_1 |/* GPIO_PUPDR_PUPDR11_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR12_1 |/* GPIO_PUPDR_PUPDR12_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR13_1 |/* GPIO_PUPDR_PUPDR13_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR14_1 |/* GPIO_PUPDR_PUPDR14_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR15_1 |/* GPIO_PUPDR_PUPDR15_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     0);

    // ��� �������������� ��������� �������� ��������� ����� �� ������ ���������
    // �� ����� ���������� ������������ ����� ��� ������� ������ � ������,
    // ���������� ������������ �������������� ����� �����/Z-���������
    // ������� ������� ��������� ������ ��� ���� ��������������� ��������� ����� ����� �������� ���������
    GPIOC->ODR |= (GPIO_ODR_ODR_11 | GPIO_ODR_ODR_12 | GPIO_ODR_ODR_13 | GPIO_ODR_ODR_14 | GPIO_ODR_ODR_15);
}


//===================================================================
// ���� ������� ������ ���������� ���������� �� ������ D1 D2
// (��� ������� ��� ��������� ����� ������� ������������ ��� ������� ������ 2-� ������)
// ��� ���� ������ �� �������� ������� ��. ����� ����������

// ������������� ����� ���������� ����������� �� ������ ������
// ������ ���-�� ��� � � ���������� �������(��. ����� ������ � ����������)
//===================================================================
void SwitchToReadKey(void)
{
    // �������� ����� ����� ����������� �� �����
    GPIOC->BSRRH |= GPIO_BSRR_BS_5;

    // ���������� K1-K5 �� ���� ���� 10 �������� ������ � K1 ������ ��������� �� �����
    GPIOC->MODER &= ~(GPIO_MODER_MODER10 | GPIO_MODER_MODER11 | GPIO_MODER_MODER12 | GPIO_MODER_MODER13  | GPIO_MODER_MODER14 | GPIO_MODER_MODER15);
    GPIOC->MODER |=  (//GPIO_MODER_MODER10_1 |   GPIO_MODER_MODER10_0 | // Input
                      //GPIO_MODER_MODER11_1 |   GPIO_MODER_MODER11_0 | // Input
                      //GPIO_MODER_MODER12_1 |   GPIO_MODER_MODER12_0 | // Input
                      //GPIO_MODER_MODER13_1 |   GPIO_MODER_MODER13_0 | // Input
                      //GPIO_MODER_MODER14_1 |   GPIO_MODER_MODER14_0 | // Input
                      //GPIO_MODER_MODER15_1 |   GPIO_MODER_MODER15_0 | // Input
                      0);
    // Dont care!!! ������� ��� �����������
    GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_10 | GPIO_OTYPER_OT_11 | GPIO_OTYPER_OT_12 | GPIO_OTYPER_OT_13 | GPIO_OTYPER_OT_14 | GPIO_OTYPER_OT_15);
    GPIOC->OTYPER |= (//GPIO_OTYPER_OT_10  | // 0 - PUSHPULL / 1 - OD
                      //GPIO_OTYPER_OT_11  | // 0 - PUSHPULL / 1 - OD
                      //GPIO_OTYPER_OT_12  | // 0 - PUSHPULL / 1 - OD
                      //GPIO_OTYPER_OT_13  | // 0 - PUSHPULL / 1 - OD
                      //GPIO_OTYPER_OT_14  | // 0 - PUSHPULL / 1 - OD
                      //GPIO_OTYPER_OT_15  | // 0 - PUSHPULL / 1 - OD
                      0);
    // Dont care!!! ������� ��� �����������
    GPIOC->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR10 | GPIO_OSPEEDER_OSPEEDR11 | GPIO_OSPEEDER_OSPEEDR12 | GPIO_OSPEEDER_OSPEEDR13 | GPIO_OSPEEDER_OSPEEDR14 | GPIO_OSPEEDER_OSPEEDR15);
    GPIOC->OSPEEDR |= (//GPIO_OSPEEDER_OSPEEDR10_1  | GPIO_OSPEEDER_OSPEEDR10_0 //
                       //GPIO_OSPEEDER_OSPEEDR11_1  | GPIO_OSPEEDER_OSPEEDR11_0 //
                       //GPIO_OSPEEDER_OSPEEDR12_1  | GPIO_OSPEEDER_OSPEEDR12_0 //
                       //GPIO_OSPEEDER_OSPEEDR13_1  | GPIO_OSPEEDER_OSPEEDR13_0 //
                       //GPIO_OSPEEDER_OSPEEDR14_1  | GPIO_OSPEEDER_OSPEEDR14_0 //
                       //GPIO_OSPEEDER_OSPEEDR15_1  | GPIO_OSPEEDER_OSPEEDR15_0 //
                       0);
    // ����� ����� K1 - K5 � �����
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR10 | GPIO_PUPDR_PUPDR11 | GPIO_PUPDR_PUPDR12 | GPIO_PUPDR_PUPDR13 | GPIO_PUPDR_PUPDR14 | GPIO_PUPDR_PUPDR15);
    GPIOC->PUPDR |= (GPIO_PUPDR_PUPDR10_1 | /*GPIO_PUPDR_PUPDR11_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR11_1 | /*GPIO_PUPDR_PUPDR11_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR12_1 | /*GPIO_PUPDR_PUPDR12_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR13_1 | /*GPIO_PUPDR_PUPDR13_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR14_1 | /*GPIO_PUPDR_PUPDR14_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR15_1 | /*GPIO_PUPDR_PUPDR15_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     0);

    // D1, D2 �� ����� �������� ������ ������� ��� ��� ��� ���������� �� ���� �������
    // ������ ���� ������ ������ � �� ��������� ��������� D1 �� D2 ����� ��� �����
    // (�� ����� � ��� �� ������ K1-5, �� ���� ������ ������ D1-2),
    // ������������ ����� � 1 ������������� � Z ����������(�����������
    // � �������� ���������� ������ D1, D2)
}
//===================================================================
// ���������� ������������� ������� ��� ������� ���������
// �� �������� ��� ���������� ��������� ����������� ����� ������� ����������
// ���������� ��������� ����� �������� ��������� � ���� �������
// ����� ����� ������� ��������� ��������� ����������� �������� �� ������� ��������� �����
// D1 �� ������� ���������� ��������� ������ K1-5
//===================================================================
void SET_KEY_D1(void)
{
    u8 i;

    // ����� ��� ��� ��������� ����� D2 �� ���� ���������� ��������� �����������
    // ������� GPIO � �������� ���������� ����� D2(����� ����� �� �����)
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
    GPIOB->OSPEEDR |= (//GPIO_OSPEEDER_OSPEEDR10_1  | GPIO_OSPEEDER_OSPEEDR10_0 // ����� ������
                       //GPIO_OSPEEDER_OSPEEDR11_1  | GPIO_OSPEEDER_OSPEEDR11_0 // Dont care
                      0);
    // ��� ����� D1, D2 ����� ��������� � �����(������� ����� ��� Floating)
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR10 | GPIO_PUPDR_PUPDR11);
    GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR10_1 | /* GPIO_PUPDR_PUPDR10_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR11_1 | /* GPIO_PUPDR_PUPDR11_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     0);
    // ������� � ���� ��� ������(��� ����� ��� ��� �� ����� �������� �� �������...)
    //GPIOB->BSRRH |= GPIO_BSRR_BS_10 | GPIO_BSRR_BS_11;

    // ��������� ������� ������� �� ����� D1
    GPIOB->BSRRL |= GPIO_BSRR_BS_10;

    // �������� ���� ���������� ������� ���������� ��������
    for(i = 0; i < 100; i++);
}
//===================================================================
// ���������� ��� D2 ��. ���������� �������
//===================================================================
void SET_KEY_D2(void)
{
    u8 i;

    // ����� ��� ��� ��������� ����� D1 �� ���� ���������� ��������� �����������
    // ������� GPIO � �������� ���������� ����� D1(����� ����� �� �����)
    GPIOB->BSRRH |= GPIO_BSRR_BS_10;

    // ������ ��� ������� �� ���� � ����� ������ �� ����� D2
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
                       //GPIO_OSPEEDER_OSPEEDR11_1  | GPIO_OSPEEDER_OSPEEDR11_0 // ����� ������
                      0);
    // ��� ����� D1, D2 ����� ��������� � �����(������� ����� ��� Floating)
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR10 | GPIO_PUPDR_PUPDR11);
    GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR10_1 | /* GPIO_PUPDR_PUPDR10_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     GPIO_PUPDR_PUPDR11_1 | /* GPIO_PUPDR_PUPDR11_0 |*/ // 00-Z/01-PullUP/10-PullDOWN
                     0);
    // ���� ��� (����� ����� � ���� � ����� ��������� � 1) ���������� �������
    // ������� � ���� ��� ������(��� ����� ��� ��� �� ����� �������� �� �������...)
    //GPIOB->BSRRH |= GPIO_BSRR_BS_10 | GPIO_BSRR_BS_11;

    // ��������� ������� ������� �� ����� D2
    GPIOB->BSRRL |= GPIO_BSRR_BS_11;

    // �������� ���� ���������� ������� ���������� ��������
    for(i = 0; i < 100; i++);
}

//===================================================================
// ������������� �������� ����������,
// ��������� ������ ������� �� �������� �� ���� ���������,
// ������ ������������� �������, �������� �������
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
