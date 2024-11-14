/**********************************************
 *                                            *
 *    (c) Copyright _Triol Corporation 2010   *
 *                                            *
 * File name   : DrvOpisStruct.h              *
 * Description : Определения и структуры      *
 *     для привода                            *
 *                                            *
 * Date        : 29.03.2010                   *
 * Author      :                              *
 * Modified    :                              *
 *                                            *
 **********************************************/

#ifndef     DrvOpisStruct_H
#define     DrvOpisStruct_H

#include "global_types.h"

// ****************************************************************************

#pragma pack(push)  // сохран. тек. значение выравнивания в стеке прекомпилера
#pragma pack(1)     // устан. значение выравнивания в 1 байт

typedef struct{
    unsigned int        FileCrc;                                //Контрольная сумма на файл	                  4	
    unsigned int        Sign;                                   //Сигнатура системы параметров	                  4	
    unsigned int        SizeOfFileDescr;                        //Размер всего файла описателя параметров в словах. 4	
    unsigned char       MaxLengthName;                          //Максимальное количество символов в наименовании параметра, группы 1
    unsigned char       MaxLengthMess;                          //Максимальное количество символов в наименовании события           1
    unsigned char       MaxLengthHelp;                          //Максимальное количество символов в наименовании Help              1
    unsigned char       MaxLengthEdIzm;                         //Максимальное количество символов в наименовании единицы измерения 1
    unsigned int        MenuStart;                              //Адрес начала описателей меню	                    4	Относительно начала файла
    unsigned int        PrmStart;                               //Адрес начала описателей параметров	            4	Относительно начала файла
    unsigned short int  QuantityMenu;                           //Количество описателей меню	                    2	
    unsigned short int  QuantityPrm;                            //Количество описателей параметров	                2	
    //unsigned short int  rsrvd;                                //для выравнивания
    unsigned char       VendorName[32];                         //Строка с наименованием производителя	            32	
    unsigned char       DeviceName[32];                         //Строка с наименованием изделия	                32
    unsigned int		DeviceType;                             //Тип привода по номенклатуре ТРИОЛ	                4	
    unsigned short int  TopOfJrnFlt;                            //Адрес параметра указателя вершины журнала аварийных остановов.	2	Указывает на номер записи в журнале
    unsigned short int  NumOfRecFlt;                            //Адрес параметра в котором содержится количество записей в журнале аварийных остановов	2	Указывает на количество записей в журнале
    unsigned short int  TopOfJrnAl;                             //Адрес параметра указателя вершины журнала предупреждений.	2	Указывает на номер записи в журнале
    unsigned short int  NumOfRecAl;                             //Адрес параметра в котором содержится количество записей в журнале предупреждений	2	Указывает на количество записей в журнале
    unsigned short int  AdrListPrmJrn;                          //Адрес списка параметров журнала.	2	В списке приводятся номера параметров в записи журнала.
    unsigned short int  NumPrmInJrn;                            //Количество параметров в списке по полю AdrListPrmJrn 	2	
	unsigned short int  QuantityUst;	                        //Количество уставок        2
	unsigned int        JrnStart;                               //Адрес начала описателей   4
	unsigned short int  QuantityJrn;                            //Количество журналов
    unsigned short int  QuantityFildJrn;                        //Количество описателей полей журнальных записей
    unsigned int        MessPultStart;
    unsigned short int  QuantityMessPult;
    unsigned int        Pnt1LngTxt;                             //Ссылка на начало описателя параметров первого языка   4
    unsigned int        QuantityLng;                            //Количество  поддерживаемых языков   4
    unsigned char       Res[14];                                 //Резерв 1	128 - сумма полей	
    unsigned int        HeaderCrc;                              //Контрольная сумма на заголовок	4	Полный размер структуры 128 байт с полем HeaderCrc
}HeadOfFile_type;

#pragma pack(pop) // востанавливаем выравнивание.

// Старая структура заголовка ФОП
#pragma pack(push)  // сохран. тек. значение выравнивания в стеке прекомпилера
#pragma pack(1)     // устан. значение выравнивания в 1 байт

typedef struct{
    unsigned int        FileCrc;                                //Контрольная сумма на файл	                  4	
    unsigned int        Sign;                                   //Сигнатура системы параметров	                  4	
    unsigned short int  SizeOfFileDescr;                        //Размер всего файла описателя параметров в словах. 2	
    unsigned short int  MenuStart;                              //Адрес начала описателей меню	                  2	Относительно начала файла
    unsigned short int  PrmStart;                               //Адрес начала описателей параметров	          2	Относительно начала файла
    unsigned short int  QuantityMenu;                           //Количество описателей меню	                  2	
    unsigned short int  QuantityPrm;                            //Количество описателей параметров	          2	
    //unsigned short int  rsrvd;                                //для выравнивания
    unsigned char       VendorName[32];                         //Строка с наименованием производителя	          32	
    unsigned char       DeviceName[32];                         //Строка с наименованием изделия	                  32
    unsigned int		DeviceType;                             //Тип привода по номенклатуре ТРИОЛ	          2	
    unsigned short int  TopOfJrnFlt;                            //Адрес параметра указателя вершины журнала аварийных остановов.	2	Указывает на номер записи в журнале
    unsigned short int  NumOfRecFlt;                            //Адрес параметра в котором содержится количество записей в журнале аварийных остановов	2	Указывает на количество записей в журнале
    unsigned short int  TopOfJrnAl;                             //Адрес параметра указателя вершины журнала предупреждений.	2	Указывает на номер записи в журнале
    unsigned short int  NumOfRecAl;                             //Адрес параметра в котором содержится количество записей в журнале предупреждений	2	Указывает на количество записей в журнале
    unsigned short int  AdrListPrmJrn;                          //Адрес списка параметров журнала.	2	В списке приводятся номера параметров в записи журнала.
    unsigned short int  NumPrmInJrn;                            //Количество параметров в списке по полю AdrListPrmJrn 	2	
	unsigned short int  QuantityUst;	
	unsigned short int  JrnStart;
	unsigned short int  QuantityJrn;
    unsigned char       Res[20];                                //Резерв 1	128 - сумма полей	
//    unsigned char       Res[18];                               //Резерв 1	128 - сумма полей	
    unsigned int        HeaderCrc;                              //Контрольная сумма на заголовок	4	Полный размер структуры 128 байт с полем HeaderCrc
}HeadOfFileOld_type;

#pragma pack(pop) // востанавливаем выравнивание.
// ****************************************************************************

// ****************************************************************************

extern HeadOfFile_type HeadOfFile;

// ****************************************************************************
#define  szNameDrv     20

typedef struct{
    unsigned char Name[szNameDrv];
} FileID_type;

#define    Id_File  (*(FileID_type*)0)
// ****************************************************************************

#endif // DrvOpisStruct_H
