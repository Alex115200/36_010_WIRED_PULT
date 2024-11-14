
#include "globalIncludes.h"

LEDState_type 		LEDState;

#ifdef _PROJECT_IAR_
//===================================================================
//          ������������� ����������� �� ������
//===================================================================
void initLED(void)
{
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN);

    GPIOC->BSRRL = ( GPIO_BSRR_BS_10 | GPIO_BSRR_BS_11 | GPIO_BSRR_BS_12 | GPIO_BSRR_BS_4 );// ���������� ���� � ������� ��������� ��� ���������
    GPIOA->BSRRL = ( GPIO_BSRR_BS_15 );

	GPIOC->OTYPER |= (GPIO_OTYPER_OT_10 | GPIO_OTYPER_OT_11 | GPIO_OTYPER_OT_12 | GPIO_OTYPER_OT_4 );	    // ��� ���� ���������
	GPIOC->MODER &= ~(GPIO_MODER_MODER10 | GPIO_MODER_MODER11 | GPIO_MODER_MODER12 | GPIO_MODER_MODER4);    // ���������� ����������
	GPIOC->MODER |= (GPIO_MODER_MODER10_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER12_0 | GPIO_MODER_MODER4_0);	// ��������� ��� ������


    GPIOA->PUPDR  &= ~(1<<15);                  // ��� ��������

    GPIOA->OTYPER |= (GPIO_OTYPER_OT_15 );	    // ��� ���� ���������
	GPIOA->MODER  &= ~(GPIO_MODER_MODER15 );    // ���������� ����������
	GPIOA->MODER  |= GPIO_MODER_MODER15_0;	    // ��������� ��� ������

}
//===================================================================
// 				���������� ��������� � ������
//===================================================================
void forceLED(void)
{
    GPIOA->BSRRL = (0x1uL<<15);	            // ���������� ��������� ���
    GPIOA->BSRRH = LEDState.word[0];	    // ������� �����

    GPIOC->BSRRL = (0x7uL<<10)+(0x1uL<<4);	// ���������� ��������� ��� 
    GPIOC->BSRRH = LEDState.word[1];	    // ������� �����
        
}
//===================================================================
#else  // _PROJECT_IAR_
    void forceLED(void) { ; }
#endif  // _PROJECT_IAR_
//===================================================================
// 				���� ��� ���������
//===================================================================
void testLed(void)
{
	ledWorkOn(LEDState) ;	// ������
    forceLED();
	OSTimeDly(1000);
	ledWorkOff(LEDState);
    forceLED();
	OSTimeDly(1000);

	ledReadyOn(LEDState) ;// �����
    forceLED();
	OSTimeDly(1000);
	ledReadyOff(LEDState);
    forceLED();
	OSTimeDly(1000);

	ledFaultOn(LEDState) ;// ������
    forceLED();
	OSTimeDly(1000);
	ledFaultOff(LEDState);
    forceLED();
	OSTimeDly(1000);

	ledLocalOn(LEDState) ;// �������
    forceLED();
	OSTimeDly(1000);
	ledLocalOff(LEDState);
    forceLED();
	OSTimeDly(1000);

	ledRemotOn(LEDState);// ���/��
    forceLED();
	OSTimeDly(1000);
	ledRemotOff(LEDState);
    forceLED();
	OSTimeDly(1000);

	ledOff(LEDState);
    forceLED();
	OSTimeDly(1000);
	ledOn(LEDState);	// �������� ���
    forceLED();
	OSTimeDly(1000);
}

