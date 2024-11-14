
#include "globalIncludes.h"

#include "TestScreen.h"

typedef		int	(*pDebFunc_type)( keyName_type    Key );

extern char MasEdIzmTek[65][8];


//====================================================================
//====================================================================
const pDebFunc_type 	debFunc[] = {
    drawScale_1,
    drawScale_2,
    drawScale_3,
	HandlerMenuStatus_TS1,
	HandlerMenuStatus_TS3,
	HandlerMenuStatus_TS2,
    testContrast,
};
//====================================================================
//====================================================================
void TestScreen(void)
{
    keyName_type    Key;

static    int				deb = 0;
    u8              err;

	int i = 0;

    while(deb == 0){
	    OSSemPend(SemMenu, SYS_FOREVER, &err );
	    Key = WaitKey(NULL);
        if(Key == KEY_FUNC ) return;


	    i+= debFunc[i]( Key );

        if(i == 100) return;
        if( i >=  sizeof(debFunc)/sizeof(pDebFunc_type)){
	    	i = 0;
	    }
        else if (i<0)   
            i = sizeof(debFunc)/sizeof(pDebFunc_type);
	}
}
//====================================================================
const  s32  dyUp  = -15;
const  s32  dyMid = -5;
const  s32  dyDwn =  5;
//====================================================================
int HandlerMenuStatus_TS1(keyName_type Key)
{ 
    u16                     yPos;
    char                    strtmp[17];
    u32                     xPos;
    u8                      err;
    statusDRV_tipe          Status;

	while(1){ 
	    OSSemPend(SemMenu, SYS_FOREVER, &err );
	    Key = WaitKey(NULL);

	    switch(Key){
	        case KEY_UP:    return  (-1);
	        case KEY_DOWN:	return     1;
	        case KEY_FUNC:	return   100;
	        default:        break;
	    }
	    GrafBffClear();

        // Верхний параметр
        yPos = 45;
        sprintf(&strtmp[0], "%s", "123.5" );
        xPos = StrOutFont(120, yPos, NO_INV, &strtmp[0], &fontArial22, rightAlign );
        sprintf(&strtmp[0], "%s", &MasEdIzmTek[HzRasm][0]);
        StrOutFont(xPos, yPos+13, NO_INV, &strtmp[0], &fontNormal, leftAlign );//вычисление смещения для единиц измерения, 25 - ширина символа
//	    StrOutFont( 50, 36+dyUp, NO_INV, "380 В", &fontCourierNew16B );

        yPos = 20;
        sprintf(&strtmp[0], "Част.на вых.ПЧ  ");    //Ххаст >> Част
        StrOutFont(0, yPos, INV, &strtmp[0], &fontNormal, leftAlign );


        // Нижний параметр
        yPos = 107;
        sprintf(&strtmp[0], "%s", "1470" );
        xPos = StrOutFont(120, yPos, NO_INV, &strtmp[0], &fontArial22, rightAlign );
        sprintf(&strtmp[0], "%s", &MasEdIzmTek[obMinRsm][0]);
        StrOutFont(xPos, yPos+13, NO_INV, &strtmp[0], &fontNormal, leftAlign );//вычисление смещения для единиц измерения, 25 - ширина символа
//	    StrOutFont(x, 120+dyDwn, NO_INV, "1470 Об/Мин", &fontCourierNew16B);

        yPos = 86;
//      sprintf(&strtmp[0], "Част.на вых.ПЧ  ");
        sprintf(&strtmp[0], "Скорость двигат.");
        StrOutFont(0, yPos, INV, &strtmp[0], &fontNormal, leftAlign );


      //DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 width, u16 stroke1, u16 stroke2);
        DrawLine(    0,      16,    159,     16,         1,           0,           0);
        DrawLine(    0,  160-16,    159, 160-16,         1,           0,           0);

//        statusDRV_tipe      Status;
        Status.all = 0;
        Status.bit.localRemote  = 1;
        Status.bit.ready        = 1;
        parseWordStatus( 205, Status);
        // Подвал
        sprintf(&strtmp[0], "А 205" );
        StrOutFont(0, 160-16, NO_INV, &strtmp[0], &fontNormal, leftAlign );

        GrfOut();
	}
}
//===================================================================
int HandlerMenuStatus_TS3(keyName_type Key)
{ 
    u16                     yPos;
    char                    strtmp[17];
    u32                     xPos;
    u8                      err;

	while(1){ 
	    OSSemPend(SemMenu, SYS_FOREVER, &err );
	    Key = WaitKey(NULL);

	    switch(Key){
	        case KEY_UP:    return  (-1);
	        case KEY_DOWN:	return    1;
	        case KEY_FUNC:	return   100;
            
	        default:        break;
	    }
	    GrafBffClear();

        // Верхний параметр
        yPos = 45;
        sprintf(&strtmp[0], "%s", "123.5" );
        xPos = StrOutFont(120, yPos, NO_INV, &strtmp[0], &fontArial22, rightAlign );
        sprintf(&strtmp[0], "%s", &MasEdIzmTek[HzRasm][0]);
        StrOutFont(xPos, yPos+13, NO_INV, &strtmp[0], &fontNormal, leftAlign );//вычисление смещения для единиц измерения, 25 - ширина символа
//	    StrOutFont( 50, 36+dyUp, NO_INV, "380 В", &fontCourierNew16B );

        yPos = 20;
        sprintf(&strtmp[0], "Част.на вых.ПЧ  ");
        StrOutFont(0, yPos, NO_INV, &strtmp[0], &fontNormal, leftAlign );


        // Нижний параметр
        yPos = 107;
        sprintf(&strtmp[0], "%s", "1470" );
        xPos = StrOutFont(120, yPos, NO_INV, &strtmp[0], &fontArial22, rightAlign );
        sprintf(&strtmp[0], "%s", &MasEdIzmTek[obMinRsm][0]);
        StrOutFont(xPos, yPos+13, NO_INV, &strtmp[0], &fontNormal, leftAlign );//вычисление смещения для единиц измерения, 25 - ширина символа
//	    StrOutFont(x, 120+dyDwn, NO_INV, "1470 Об/Мин", &fontCourierNew16B);

        yPos = 86;
//      sprintf(&strtmp[0], "Част.на вых.ПЧ  ");
        sprintf(&strtmp[0], "Скорость двигат.");
        StrOutFont(0, yPos, NO_INV, &strtmp[0], &fontNormal, leftAlign );


      //DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 width, u16 stroke1, u16 stroke2);
        DrawLine(    0,      16,    159,     16,         1,           0,           0);
        DrawLine(    0,  160-16,    159, 160-16,         1,           0,           0);

        DrawLine(    0,      83,    159,     83,         1,           0,           0);

        {
            statusDRV_tipe      Status;
            Status.all = 0;
            Status.bit.localRemote  = 0;
            Status.bit.breakdown    = 1;
            parseWordStatus( 205, Status);
        }
        // Подвал
        sprintf(&strtmp[0], "А 205" );
        StrOutFont(0, 160-16, NO_INV, &strtmp[0], &fontNormal, leftAlign );

        GrfOut();
	}
}
//===================================================================
int HandlerMenuStatus_TS2(keyName_type Key)
{
    char                    strtmp[17];
    u32                     xPos;
    u8                      err;

	while(1){ 
	    OSSemPend(SemMenu, SYS_FOREVER, &err );
	    Key = WaitKey(NULL);

	    switch(Key){
	        case KEY_UP:    return  (-1);
	        case KEY_DOWN:	return    1;
	        case KEY_FUNC:	return   100;
	        default:        break;
	    }
	    GrafBffClear();
        // Верхний параметр
        sprintf(&strtmp[0], "%s", "380" );
        xPos = StrOutFont(120, 36+dyUp, NO_INV, &strtmp[0], &fontArial22, rightAlign );
        sprintf(&strtmp[0], "%s", &MasEdIzmTek[VRasm][0]);
        StrOutFont(xPos, 50+dyUp, NO_INV, &strtmp[0], &fontNormal, leftAlign );//вычисление смещения для единиц измерения, 25 - ширина символа
//	    StrOutFont( 50, 36+dyUp, NO_INV, "380 В", &fontCourierNew16B );

        //Центральный параметр
        sprintf(&strtmp[0], "%s", "123.5" );
        xPos = StrOutFont(120, 65+dyMid, NO_INV, &strtmp[0], &fontArial22, rightAlign );
        sprintf(&strtmp[0], "%s", &MasEdIzmTek[HzRasm][0]);
        StrOutFont(xPos, 78+dyMid, NO_INV, &strtmp[0], &fontNormal, leftAlign );//вычисление смещения для единиц измерения, 25 - ширина символа

        // Нижний параметр
//	    x = 0;

        sprintf(&strtmp[0], "%s", "1470" );
        xPos = StrOutFont(120, 100+dyDwn, NO_INV, &strtmp[0], &fontArial22, rightAlign );
        sprintf(&strtmp[0], "%s", &MasEdIzmTek[obMinRsm][0]);
        StrOutFont(xPos, 113+dyDwn, NO_INV, &strtmp[0], &fontNormal, leftAlign );//вычисление смещения для единиц измерения, 25 - ширина символа
//	    StrOutFont(x, 120+dyDwn, NO_INV, "1470 Об/Мин", &fontCourierNew16B);

      //DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 width, u16 stroke1, u16 stroke2);
        DrawLine(    0,      16,    159,     16,         1,           0,           0);
        DrawLine(    0,  160-16,    159, 160-16,         1,           0,           0);

        {
            statusDRV_tipe      Status;
            Status.all = 0;
            Status.bit.run  = 1;
            Status.bit.speedup = 1;
            parseWordStatus( 205, Status);
        }
        // Подвал
        sprintf(&strtmp[0], "А 205" );
        StrOutFont(0, 160-16, NO_INV, &strtmp[0], &fontNormal, leftAlign );

        GrfOut();
	}
}
//===================================================================
//===================================================================
typedef struct{
    f32         data;
    f32         min;
    f32         max;
    s32         xPos1;
    s32         xPos2;
    s32         yPos;
    s32         dot;
    u8          format;
  //  alignment_type      alignment;
}scaleData_type;
//====================================================================
void drawScale(scaleData_type *scaleData);

//====================================================================
int drawScale_1( keyName_type Key )
{
    u8                  err;
    scaleData_type      scaleData;
    char                strtmp[17];
//    u32                 xPos;
    f32                 data1,data2,data3 ;

    data1 = 10.0f;
    data2 = 1.0f;
    data3 = 100.0f;

    while(1){ 
	    OSSemPend(SemMenu, SYS_FOREVER, &err );
	    Key = WaitKey(NULL);

	    switch(Key){
	        case KEY_UP:    return  (-1);
	        case KEY_DOWN:	return    1;
	        case KEY_FUNC:	return   100;

            case KEY_RIGHT:	
                data1 += 5.0f;
                data2 += 0.15f;
                data3 += 50.0f;
                break;
	        case KEY_LEFT:
                data1 -= 5.0f;
                data2 -= 0.15f;
                data3 -= 100.0f;
                break;
            default:        break;
	    }
	    GrafBffClear();

        scaleData.xPos1 = 21;
        scaleData.xPos2 = 139;
/*
        sprintf(&strtmp[0], "%.*f", scaleData.dot, scaleData.data );
        xPos = StrOutFont(55, 30, NO_INV, &strtmp[0], &fontArial22, leftAlign );
        sprintf(&strtmp[0], "%s", MasEdIzm[VRasm]);
        StrOutFont(xPos, 30+13, NO_INV, &strtmp[0], &fontNormal, leftAlign );
*/
        scaleData.yPos = 33;

        scaleData.data = data1;
        scaleData.min = 0.0f;
        scaleData.max = 600.f;
        scaleData.dot = 0;
            drawScale( &scaleData );
        
        scaleData.yPos = 76;
        scaleData.data = data2;

        scaleData.min = 0.0f;
        scaleData.max = 5.00f;
        scaleData.dot = 2;
            drawScale( &scaleData );

        scaleData.yPos = 119;
        scaleData.data = data3;
        scaleData.min = 0.0f;
        scaleData.max = 32767.f;
            scaleData.dot = 0;

        drawScale( &scaleData );


        DrawLine(    0,      16,    159,     16,         1,           0,           0);
        DrawLine(    0,  160-16,    159, 160-16,         1,           0,           0);

        {
            static statusDRV_tipe      Status = {0};
//            Status.all = 0;

            Status.bit.run   = 1;

            if(Key == KEY_STOP){
                Status.bit.fNegative = 1;
            }
            if(Key == KEY_START){
                Status.bit.fNegative = 0;
            }

            if(Key == KEY_ENTER){
                Status.bit.rundown = 0;
                Status.bit.speedup = 1;
            }
            if(Key == KEY_ESC){
                Status.bit.rundown = 1;
                Status.bit.speedup = 0;
            }

            Status.bit.fNegative = 1;
            parseWordStatus( 205, Status);
        }
        // Подвал
        sprintf(&strtmp[0], "А 205" );
        StrOutFont(0, 160-16, NO_INV, &strtmp[0], &fontNormal, leftAlign );

        GrfOut();
	}
}
//====================================================================
//====================================================================
int drawScale_2( keyName_type Key )
{
    u8                  err;
    scaleData_type      scaleData;
    char                strtmp[17];
    u32                 xPos;
    f32                 data1,data2;

    int yPos;

    data1 = 10.0f;
    data2 = 1.0f;

    while(1){ 
	    OSSemPend(SemMenu, SYS_FOREVER, &err );
	    Key = WaitKey(NULL);

	    switch(Key){
	        case KEY_UP:    return  (-1);
	        case KEY_DOWN:	return    1;
	        case KEY_FUNC:	return   100;

            case KEY_RIGHT:	
                data1 += 1111.1f;
                data2 += 0.15f;
                break;
	        case KEY_LEFT:
                data1 -= 1111.1f;
                data2 -= 0.15f;
                break;
            default:        break;
	    }
	    GrafBffClear();

        scaleData.xPos1 = 10;
        scaleData.xPos2 = 150;

        scaleData.data = data1;
        scaleData.min = 0.0f;
        scaleData.max = 600.f;
        scaleData.dot = 1;

        yPos = 20;
        sprintf(&strtmp[0], "Част.на вых.ПЧ  ");
        StrOutFont(0, yPos, INV, &strtmp[0], &fontNormal, leftAlign );

        sprintf(&strtmp[0], "%.*f", scaleData.dot, data1 );
        xPos = StrOutFont(120, 40, NO_INV, &strtmp[0], &fontArial22, rightAlign );
        sprintf(&strtmp[0], "%s", &MasEdIzmTek[HzRasm][0]);
        StrOutFont(xPos, 40+13, NO_INV, &strtmp[0], &fontNormal, leftAlign );

        yPos = 80;
        sprintf(&strtmp[0], "Давление в сист ");
        StrOutFont(0, yPos, INV, &strtmp[0], &fontNormal, leftAlign );
        
        scaleData.yPos = 119;
        scaleData.data = data2;

        scaleData.min = 0.0f;
        scaleData.max = 5.00f;
        scaleData.dot = 2;
            drawScale( &scaleData );


        DrawLine(    0,      16,    159,     16,         1,           0,           0);
        DrawLine(    0,  160-16,    159, 160-16,         1,           0,           0);

        {
            static statusDRV_tipe      Status = {0};

            if(Key == KEY_STOP){
                Status.bit.run = 0;
            }
            if(Key == KEY_START){
                Status.bit.run = 1;
            }

            if(Key == KEY_ENTER){
                Status.bit.fNegative = 0;
            }
            if(Key == KEY_ESC){
                Status.bit.fNegative = 1;
            }
            parseWordStatus( 205, Status);
        }
        // Подвал
        sprintf(&strtmp[0], "А 205" );
        StrOutFont(0, 160-16, NO_INV, &strtmp[0], &fontNormal, leftAlign );

        GrfOut();
	}
}
//====================================================================
//====================================================================
int drawScale_3( keyName_type Key )
{
    u8                  err;
    scaleData_type      scaleData;
    char                strtmp[17];
    u32                 xPos;
    f32                 data1,data2,data3 ;

    data1 = 10.0f;
    data2 = 1.0f;
    data3 = 100.0f;

    while(1){ 
	    OSSemPend(SemMenu, SYS_FOREVER, &err );
	    Key = WaitKey(NULL);

	    switch(Key){
	        case KEY_UP:    return  (-1);
	        case KEY_DOWN:	return    1;
	        case KEY_FUNC:	return   100;

            case KEY_RIGHT:	
                data1 += 5.0f;
                data2 += 0.15f;
                data3 += 50.0f;
                break;
	        case KEY_LEFT:
                data1 -= 5.0f;
                data2 -= 0.15f;
                data3 -= 100.0f;
                break;
            default:        break;
	    }
	    GrafBffClear();

        scaleData.xPos1 = 10;
        scaleData.xPos2 = 150;

        // Верхний параметр
        sprintf(&strtmp[0], "%s", "380" );
        xPos = StrOutFont(120, 36+dyUp, NO_INV, &strtmp[0], &fontArial22, rightAlign );
        sprintf(&strtmp[0], "%s", &MasEdIzmTek[VRasm][0]);
        StrOutFont(xPos, 50+dyUp, NO_INV, &strtmp[0], &fontNormal, leftAlign );//вычисление смещения для единиц измерения, 25 - ширина символа
//	    StrOutFont( 50, 36+dyUp, NO_INV, "380 В", &fontCourierNew16B );

        //Центральный параметр
        sprintf(&strtmp[0], "%s", "123.5" );
        xPos = StrOutFont(120, 65+dyMid, NO_INV, &strtmp[0], &fontArial22, rightAlign );
        sprintf(&strtmp[0], "%s", &MasEdIzmTek[HzRasm][0]);
        StrOutFont(xPos, 78+dyMid, NO_INV, &strtmp[0], &fontNormal, leftAlign );//вычисление смещения для единиц измерения, 25 - ширина символа
        scaleData.yPos = 33;


        scaleData.yPos = 119;
        scaleData.data = data3;
        scaleData.min = 0.0f;
        scaleData.max = 1500.f;
            scaleData.dot = 0;

        drawScale( &scaleData );


        DrawLine(    0,      16,    159,     16,         1,           0,           0);
        DrawLine(    0,  160-16,    159, 160-16,         1,           0,           0);

        {
            statusDRV_tipe      Status;
            Status.all = 0;
            Status.bit.charge  = 1;
//            Status.bit.breakdown    = 1;
            parseWordStatus( 205, Status);
        }
        // Подвал
        sprintf(&strtmp[0], "А 205" );
        StrOutFont(0, 160-16, NO_INV, &strtmp[0], &fontNormal, leftAlign );

        GrfOut();
	}
}
//====================================================================
//====================================================================
void drawScale( scaleData_type *s )
{
    s32             x, x2;
    s32             len;
    char            strtmp[17];

    DrawLine( s->xPos1,   s->yPos,  s->xPos2,   s->yPos, 2, 0, 0);      // Горизонтальная линия

    DrawLine(  s->xPos1,   s->yPos,  s->xPos1, s->yPos+10, 1, 0, 0);    // Левая вертикальная
    sprintf( strtmp, "%.*f", s->dot, s->min);
    StrOutFont(s->xPos1, s->yPos+13, NO_INV, strtmp, &fontMini, centrAlign);

    DrawLine(  s->xPos2,   s->yPos,  s->xPos2, s->yPos+10, 1, 0, 0);    // Правая вертикальная
    sprintf( strtmp, "%.*f", s->dot, s->max);
    StrOutFont(s->xPos2, s->yPos+13, NO_INV, strtmp, &fontMini, centrAlign);

    x = (s->xPos2 - s->xPos1)/2 + s->xPos1;                             // Центральная вертикальная
    DrawLine( x , s->yPos, x, s->yPos+10, 1, 0, 0);
    sprintf( strtmp, "%.*f", s->dot, (s->max - s->min)/2);
    StrOutFont( x , s->yPos+13, NO_INV, strtmp, &fontMini, centrAlign);

    x = (s->xPos2 - s->xPos1)/4 + s->xPos1;                             // Левая вторая вертикальная
    DrawLine( x , s->yPos, x, s->yPos+5,  1, 0, 0);

    x = (s->xPos2 - s->xPos1)/4*3 + s->xPos1;                           // Правая вторая вертикальная
    DrawLine( x , s->yPos, x, s->yPos+5,  1, 0, 0);

    len = (s32)((s->xPos2 - s->xPos1)/(s->max - s->min) * s->data);     // Бегунок шкалы
    x2 = s->xPos1 + len; 

    if      ( x2 >  s->xPos2 )   x2 = s->xPos2;
    else if ( x2 <= s->xPos1 )   x2 = s->xPos1;

    DrawLine( s->xPos1,   s->yPos-11,  x2 ,   s->yPos-11, 10, 0, 0);


}
//===================================================================
//===================================================================
#ifdef _PROJECT_IAR_

const u8 dContrast = 5;
void setContrast( void );

int testContrast( keyName_type Key )
{
    u8                  err;
    static u8       contr = 0x80;
    while(1){ 
	    OSSemPend(SemMenu, SYS_FOREVER, &err );
	    Key = WaitKey(NULL);

	    switch(Key){
	        case KEY_UP:    return  (-1);
	        case KEY_DOWN:	return    1;
	        case KEY_FUNC:	return   100;

            case KEY_RIGHT:	
                contr += dContrast;
                break;
	        case KEY_LEFT:
                contr -= dContrast;
                break;
            default:        break;
	    }

        setContrast( );
    }
}
#else
int testContrast( keyName_type Key )
{
    return 1;
}
#endif
//===================================================================
//===================================================================
