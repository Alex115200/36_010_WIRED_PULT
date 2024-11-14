#ifndef Monitor_H
#define Monitor_H

typedef enum{
    NO_INV     = 0,
    INV        = 1,
    STR_INV    = 2,
}invers_type;

typedef enum{
    leftAlign   = 0,
    centrAlign  = 1, 
    rightAlign  = 2
}textAlign_type ;

// ************************************************************************************************************
int RdKey(void);
//int WaitEnterExit(void);
void GrafBffClear(void);

void DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 width, u16 stroke1, u16 stroke2);
void ScrollBarVert(u16 MaxElem, u16 MaxVisElem, u16 SizeElem, u16 Pos, u16 x, u16 y);
void DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
void ProgressBar(u16 x, u16 y, u16 len, u16 Proc);

u32 StrOutFont(s32 xPos, s32 yPos, invers_type inv, char *str, font_type *fontPtr, textAlign_type align);
void SymOut(u32 xPos, u32 yPos, char sym, u8 Inv, font_type *fontPtr);

// ************************************************************************************************************
#endif
