#ifndef     _BOARDCONFIG_
#define     _BOARDCONFIG_


#define     szEEPROMmem     (32768)
#define     szPageEEPROM    (64)    // В байтах

#define     SysTime100ms    99     /* Время пробуждения задачи LCD */

#define YPIXELS 160
#define XPIXELS 160
#define NBYTES  (XPIXELS/8) // блин при рефакторинге все что связано с отрисовкой нужно поместить сюда!!!


#endif      // _BOARDCONFIG_
