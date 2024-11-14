#ifndef INIT_H
#define INIT_H

//  онстанты инициализации системы
#define FOSC     12000000L         // „астота кварцевого резонатора в √ц
#define VALUEOFM 25/3              //  соответствует PLLCFG_bit.MSEL=3
#define CCLK     (FOSC*VALUEOFM)   // CCLK =   100.000 ћ√ц  58,9824 ћ√ц
//#define VALUEOFM 5                 //  соответствует PLLCFG_bit.MSEL=3
//#define CCLK     (FOSC*VALUEOFM)   // CCLK =   60.000 ћ√ц  58,9824 ћ√ц
#define PCLK     CCLK
#define PLCK     0x0400            // PLOCK    0x0400

#endif