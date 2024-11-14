#ifndef ICON_H
#define ICON_H

extern const unsigned char errorIcon [];
extern const unsigned char error_iIcon [];
extern const unsigned char avarIcon [];
extern const unsigned char avar_iIcon [];
extern const unsigned char stopIcon [];
extern const unsigned char stop_iIcon [];
extern const unsigned char localIcon [];
extern const unsigned char remoteIcon [];
extern const unsigned char razgon1Icon [];
extern const unsigned char razgon2Icon [];
extern const unsigned char razgon3Icon [];
extern const unsigned char razgon1reverseIcon [];
extern const unsigned char razgon2reverseIcon [];
extern const unsigned char razgon3reverseIcon [];
extern const unsigned char readyIcon [];
extern const u8 SyshkaIcon [4][32];
extern const u8 PickUpIcon [4][32];
extern const u8 stopByDCIcon [5][32];
extern const u8 PickUpIcon1[5][64];

extern const u8 ZadanieIcon [3][32];
extern const u8 ZadanieReverseIcon[3][32];

extern const u8 RazgonIcon [4][32];
extern const u8 RazgonReverseIcon[4][32];
extern const unsigned char chargeIcon[4][32];

extern const unsigned char EditPrm [];
extern const unsigned char EditStopPrm [];
extern const unsigned char InfoPrm [];

extern const unsigned char down1 [];
extern const unsigned char down2 [];
extern const unsigned char up1 [];
extern const unsigned char up2 [];
extern const unsigned char left1 [];
extern const unsigned char left2 [];
extern const unsigned char right1 [];
extern const unsigned char right2 [];
extern const unsigned char anyKey1 [];
extern const unsigned char anyKey2 [];
extern const unsigned char targetKey [];
extern const unsigned char downSmall [];
extern const unsigned char upSmall [];



void DrawIcon16x16(u16 x, u16 y, const void *Icon);
void DrawIcon32x16(u16 x, u16 y, const void *Icon);

#endif
