#ifndef     OSdebug_H
#define     OSdebug_H

void  OS_CPU_SysTickInit(INT32U  cnts);   // Заглушка для VS
void EndMeasEvent(u8 numEvent);
void BeginMeasEvent(u8 numEvent);


void LCD_init_hw(void);
void LCD_init(void);


#endif  // OSdebug_H