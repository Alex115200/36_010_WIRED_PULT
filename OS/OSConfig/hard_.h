#ifndef INIT_H
#define INIT_H

// ��������� ������������� �������
#define FOSC     12000000L         // ������� ���������� ���������� � ��
#define VALUEOFM 25/3              //  ������������� PLLCFG_bit.MSEL=3
#define CCLK     (FOSC*VALUEOFM)   // CCLK =   100.000 ���  58,9824 ���
//#define VALUEOFM 5                 //  ������������� PLLCFG_bit.MSEL=3
//#define CCLK     (FOSC*VALUEOFM)   // CCLK =   60.000 ���  58,9824 ���
#define PCLK     CCLK
#define PLCK     0x0400            // PLOCK    0x0400

#endif