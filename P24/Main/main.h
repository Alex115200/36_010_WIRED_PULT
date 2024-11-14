#ifndef  Main_H
#define  Main_H

typedef enum{
tsk_lcd  = 1,
tsk_drv,
tsk_kbd,
tsk_cont
}tsk_IDtype;

void InitWD(void);
void ResetWD(tsk_IDtype id);
void ResetAllWD(void);

// **************************************
#define DEBUG 0

#endif  // Main_H
