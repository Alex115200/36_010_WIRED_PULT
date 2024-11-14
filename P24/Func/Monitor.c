
#include "globalIncludes.h"

#define  NumSymbInStr   16

//**************************************************************************
//          Вывод строки шрифтом указанныи в fontPtr
//**************************************************************************
u32 StrOutFont( s32 xPos, s32 yPos, invers_type inv, char *str, font_type *fontPtr, textAlign_type align )
{
    s32                 i, len, space, x;
    char                tmpStr[20];

    tmpStr[19] = 0;

    if (inv == STR_INV){
        len = sprintf(&tmpStr[0],   "%s" , str );
        space = (NumSymbInStr  - 1) - len;

        sprintf(&tmpStr[len], "%*c", space, ' ' );
        len += space;
    }
    else{
        len = strlen(str);
        tmpStr[len] = 0;
        strcpy(tmpStr, str);
    }

	switch(align){          // Переопределение координат x для центровки
        case leftAlign:     // Ни чего не делаем
            break;

        case centrAlign:
            x = 0;
            for (i = 0; i < len; i++){  // Вычислить конец строки
                if ((tmpStr[i] == ',' || tmpStr[i] == ':' || tmpStr[i] == '.') && fontPtr != &fontNormal)
                    { x += fontPtr->propertyFont.narrowOffset; }
                else
                    { x += fontPtr->propertyFont.normalOffset; }
            }

            xPos = xPos - x/2;// + fontPtr->propertyFont.normalOffset/2 ;
            if(xPos<0)  					{ xPos = 0; }               // Ограничение влево
            else if( xPos + x > XPIXELS )   { xPos = YPIXELS - x; }     // Ограничение вправо

            break;

        case rightAlign:
            x = 0;
            for (i = 0; i < len; i++){  // Вычислить конец строки
                if ((tmpStr[i] == ',' || tmpStr[i] == ':' || tmpStr[i] == '.') && fontPtr != &fontNormal)
					{ x += fontPtr->propertyFont.narrowOffset; }
                else
                    { x += fontPtr->propertyFont.normalOffset; }
            }
            xPos -= x;
            break;

        default:		break;
	}

    for ( i = 0; i < len; i++ )
    {
        if ((tmpStr[i] == ',' || tmpStr[i] == ':' || tmpStr[i] == '.') && fontPtr != &fontNormal) 
        { 
            SymOut(xPos, yPos, tmpStr[i], inv, fontPtr);
            xPos += fontPtr->propertyFont.narrowOffset; 
        }
        else if (tmpStr[i] == '\n' || tmpStr[i] == '\r')
        {
            break;
        }
        else
        { 
            SymOut(xPos, yPos, tmpStr[i], inv, fontPtr);
            xPos += fontPtr->propertyFont.normalOffset; 
        }
    }
    return xPos;
}
//===================================================================
//===================================================================
void SymOut(u32 xPos, u32 yPos, char sym, u8 Inv, font_type *fontPtr)
{
    u32             xOfs;
    const u8        *symPnt;
    u8              symVal;
    u8              *cPnt;
    u8              font[8];
    u8              ofs, ofsPrev, i, j;
    u8              tmp, emptySym;
    u8              Nbyte, Nbit, fontWidthInBytes, fontHeightInBytes;
    u16             bit;
    u32             offsFont;
    u32             maskForInv;

    if (fontPtr->propertyFont.width % 8 == 0)
        { fontWidthInBytes = fontPtr->propertyFont.width / 8; }
    else
    	{ fontWidthInBytes = fontPtr->propertyFont.width / 8 + 1; }

    fontHeightInBytes = fontPtr->propertyFont.height;

    offsFont = fontWidthInBytes * fontHeightInBytes * (sym - 0x20); //Поиск графики символа

    symPnt = fontPtr->ptrFont + offsFont;
    cPnt = &GrafBff[(xPos / 8) + yPos * (YPIXELS / 8)];             //Вычисление указателя в буфере по заданным координатам
    xOfs = xPos & 7;                                                //Смещение в битах

    ofsPrev = 0;                                                    //Гипотетическое смещение шрифта относительно левого края
    ofs = fontWidthInBytes * 8 + 1;
    emptySym = 0;
    for (j = 0; j < fontHeightInBytes; j++) {
        for (i = 0; i < fontWidthInBytes; i++) {                    //Сканируем строки
            tmp = *symPnt++;
            if (tmp == 0){
                ofsPrev += 8;
                continue; 
            }
            else{
                emptySym = 1;
                bit = 0x80;
                while(bit > 0x01) {
                    if (bit & tmp){
                        symPnt = symPnt + 1 - i;
                        i = 5;
                        break;
                    }
                    ofsPrev++;
                    bit = bit >> 1;
                }
            }
        }
        if(ofsPrev < ofs) ofs = ofsPrev;
        ofsPrev = 0;
    }
    if ( ( ofs == fontPtr->propertyFont.width + 1 ) || ( emptySym == 0) ) 
    	{ ofs = 0; }
    symPnt = fontPtr->ptrFont + offsFont;

    maskForInv = 0xFF << (8 - (fontPtr->propertyFont.normalOffset % 8));

    for(i = 0; i < fontHeightInBytes; i++, cPnt += YPIXELS / 8, symPnt += fontWidthInBytes){
        Nbyte = ofs / 8;                                                //Номер байта куда сместить шрифт
        Nbit = ofs % 8;
        for (j = 0; Nbyte < fontWidthInBytes; j++, Nbyte++) {
            symVal = symPnt[Nbyte];
            if (Inv){
                symVal = ~symVal;
            }
            font[j] = symVal << Nbit;
            if (Nbyte < fontWidthInBytes - 1) {
                symVal = symPnt[Nbyte + 1];
                bit = (8 - Nbit);
                if (Inv) {
                    symVal = ~symVal;
                }
                font[j] |= (symVal >> (8 - Nbit));
            }

            if (Inv && Nbyte == fontWidthInBytes - 1) {
                font[j] &= maskForInv;
            }

            cPnt[j] |= font[j] >> xOfs;
            cPnt[j + 1] |= (font[j] << (8 - xOfs));
//            GrfOut();
        }
    }
}
//===================================================================
//===================================================================
void DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 width, u16 stroke1, u16 stroke2)
{
    u16 Nbyte, Nbit;
    //u16 CntX, CntY;
    u16 Xcur, Ycur;
    u16 EndX, EndY, BeginX, BeginY;
//	u16 EndX1, EndY1, BeginX1, BeginY1;
    u16 stroke11, stroke22;
    s16 IncX, IncY;
    f32 K;

    EndX = x2;
    BeginX = x1;
    EndY = y2;
    BeginY = y1;
    if (x1 <= x2)   IncX = 1;
    else            IncX = -1;
    if (y1 <= y2)   IncY = 1;
    else            IncY = -1;

    if (x1 <= x2) {
        BeginX  = x1;
        EndX    = x2;
    }
    else{
        BeginX  = x2;
        EndX    = x1;
    }
    if (y1 <= y2) {
        BeginY  = y1;
        EndY    = y2;
    }
    else{
        BeginY  = y2;
        EndY    = y1;
    }
    IncY = 1;
    IncX = 1;

    Xcur = BeginX;
    Ycur = BeginY;
	if ((f32)(EndX - BeginX) != 0)
            K = (f32)((f32)(EndY - BeginY) / (f32)(EndX - BeginX));
	else{
            K = (f32)((f32)(EndY - BeginY) / 0.00001);
            /*if (EndY < BeginY || EndX < BeginX) K = -9999999;
            else K = 9999999;*/
	}

    stroke11 = stroke22 = 0;
    do{
        do{
            Nbyte = Ycur * NBYTES + Xcur / 8;
            Nbit  = Xcur % 8;
            if (stroke1 == 0 || stroke2 == 0) {
                GrafBff[Nbyte] |= 1 << (7 - Nbit);
            }
            else if (stroke11++ < stroke1) {
                GrafBff[Nbyte] |= 1 << (7 - Nbit);
				stroke22 = 0;
            }
            else if (stroke22++ >= stroke2 - 1) {
                stroke11 = 0;
            }

            if (Xcur > BeginX || Xcur < EndX) {
                Xcur = (s16)Xcur + IncX;
            }
            if (Ycur > BeginY || Ycur < EndY) {
                Ycur = (s16)Ycur + IncY;
            }
            if ((Ycur <= BeginY || Ycur > EndY) && (Xcur <= BeginX || Xcur > EndX)) {
                break;
            }

            if (Xcur >= XPIXELS) break;
            if (Ycur >= YPIXELS) break;

        }while(1);
        if (width > 0) width--;
        if (K > 1) {
            Xcur = ++BeginX;
            Ycur = BeginY;
        }
        else{
            Xcur = BeginX;
            Ycur = ++BeginY;
        }
    }while (width);
}
//===================================================================
//===================================================================
void ScrollBarVert(u16 MaxElem, u16 MaxVisElem, u16 SizeElem, u16 Pos, u16 x, u16 y)
{
    u16         SizeStrip;
    u16         y1, y2;
    f32         Shag;

    Shag    = (f32)SizeElem * (f32)(MaxVisElem) / (f32)(MaxElem);
    if (Shag < 1) Shag = 1;

    SizeStrip   = (u16)Shag;
    y2          = SizeElem * MaxVisElem + y;
    DrawLine(x - 1, y, x - 1, y2, 2, 2, 2);

    y1          = (u16)((Shag * (f32)Pos + (f32)y));
    y2          = y1 + SizeStrip;
    DrawLine(x - 2, y1, x - 2, y2, 5, 0, 0);
}
//===================================================================
// Рисование прямоугольника на дисплее
//===================================================================
void DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
    DrawLine(x1, y1, x2, y1, 1, 0, 0); //1-я горизонтальная линия
    DrawLine(x1, y2, x2, y2, 1, 0, 0); //2-я горизонтальная линия
    DrawLine(x1, y1, x1, y2, 1, 0, 0); //1-я вертикальная линия
    DrawLine(x2, y1, x2, y2, 1, 0, 0); //2-я вертикальная линия
}
//===================================================================

//===================================================================
void ProgressBar(u16 x, u16 y, u16 len, u16 Proc)
{
    u16         LenProg;
    u16         x1, x2, y1, y2;

    if (x > 2) x1 = x - 2;
    else x1 = 0;

    if (y > 2) y1 = y - 2;
    else y1 = 0;

    if (len < XPIXELS) x2 = x1 + len + 4;
    else x2 = XPIXELS;

    y2 = y + 10;
    DrawRectangle(x1, y1, x2, y2);

    LenProg = (u16)(((f32)Proc / 100)* (f32)len);
    x1 += 2;
    x2 = x1 + LenProg;
    y1 += 2;
    y2 = y1;
    DrawLine(x1, y1, x2, y2, 9, 0, 0);
}
//===================================================================
