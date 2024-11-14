#ifndef  initOSfile_h
#define  initOSfile_h

//======================================================================
// Выделение стека для каждой программной задачи
//======================================================================

#define     STKDRVSIZE          256
#define 	STKKBDSIZE          256
#define 	STKLCDSIZE          2048
#define 	STK_CONTROL_SIZE    256

extern 		__no_init OS_STK KBDStack[STKKBDSIZE];
extern 		__no_init OS_STK DRVStack[STKDRVSIZE];
extern 		__no_init OS_STK LCDStack[STKLCDSIZE];
extern 		__no_init OS_STK ControlTaskStack[STK_CONTROL_SIZE];

//======================================================================

#endif  // initOSfile_h
