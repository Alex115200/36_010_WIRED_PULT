
#include "globalIncludes.h"

const u8 Arial22[225][120] = {
#ifdef LAN_TUR
    #include "Arial22_tur.c"
#else
    #include "Arial22.c"
#endif
};

//===================================================================
//===================================================================
const u8 gFontNormal[225][32] = {
#ifdef LAN_TUR
    #include "CourierNew12_tur.c"
#else
    #include "CourierNew12.c"
#endif    
//    #include "Arial10B.c"
//    #include "CourierNew12B.c"
//    #include "BookManOldStyle10B.c"
};

font_type fontNormal = {
    12,
    16,
    4,
    10,
    &gFontNormal[0][0]
};
//===================================================================
//===================================================================
const u8 gFontBig[225][160] = {
    #include "LucidaConsole30B.c"
};
/*
font_type fontBig = {
    25,
    40,
    12,
    15,
    &gFontBig[0][0]
};
*/
font_type fontBig = {
    30,
    30,
    6,
    16,
    &Arial22[0][0]
};

//===================================================================
//===================================================================
const u8 gFontMiddle[225][46] = {
#ifdef LAN_TUR
    #include "CourierNew16B_tur.c"
#else
    #include "CourierNew16B.c"
#endif  

};

font_type fontMiddle = {
    16,
    23,
    8,
    14,
   &gFontMiddle[0][0]
};
//===================================================================
//              В этом шрифте доступны только цифры
//===================================================================
const u8 gFontMini[26][8] = {
    #include "Arial8digit.c"
};

font_type fontMini = {
    8,
    8,
    2,
    7,
   &gFontMini[0][0]
};
//===================================================================
//===================================================================

font_type fontArial22 = {
    30,
    30,
    6,
    16,
    &Arial22[0][0]
};
//===================================================================
