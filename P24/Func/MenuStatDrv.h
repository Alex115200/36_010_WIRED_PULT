/*
//===================================================================
// Файл содержит функции обработки состояния привода и вывода этой
// информации на дисплей
//===================================================================
*/
//===================================================================
inline u32 getStatusLocal(u16 status)
{
    return (status & 0x0100) ? 1 : 0;
}
//===================================================================
inline u32 getStatusRunDown(u16 status) //Выбег
{
//    return (status & 0xFE00) ? 1 : 0;
    if ((status & 0xFE00) == 0x0000) return 1;
    else return 0;
}
//===================================================================
inline u32 getStatusOutputWork(u16 status)
{
    if ((status & 0x8e00) == 0x8000) return 1;
    else return 0;
}
//===================================================================
inline u32 getStatusOutputWorkReverse(u16 status)
{
    if ((status & 0x8e00) == 0x8200) return 1;
    else return 0;
}
//===================================================================
inline u32 getStatusSpeedUp(u16 status)
{
    if ((status & 0x8400) == 0x8400) return 1;
    else return 0;
//    return (status & 0x8400) ? 1 : 0;
}
//===================================================================
inline u32 getStatusSpeedUpReverse(u16 status)
{
    if ((status & 0x8600) == 0x8600) return 1;
    else return 0;
//    return (status & 0x8600) ? 1 : 0;
}
//===================================================================
inline u32 getStatusBrake(u16 status)
{
    if ((status & 0x8800) == 0x8800) return 1;
    else return 0;
//    return (status & 0x8800) ? 1 : 0;
}
//===================================================================
inline u32 getStatusBrakeReverse(u16 status)
{
    if ((status & 0x8A00) == 0x8A00) return 1;
    else return 0;
//    return (status & 0x8800) ? 1 : 0;
}
//===================================================================
inline u32 getStatusReady(u16 status)
{
    return (status & 0x1000) ? 1 : 0;
}
//===================================================================
inline u32 getStatusCrash(u16 status)
{
    return (status & 0x2000) ? 1 : 0;
}
//===================================================================
inline u32 getStatusWorkAll(u16 status)
{
    return (status & 0x8000) ? 1 : 0;
}
//===================================================================
inline u32 getStatusCharge(u16 status)
{
    return (status & 0x4000) ? 1 : 0;
}
//===================================================================
