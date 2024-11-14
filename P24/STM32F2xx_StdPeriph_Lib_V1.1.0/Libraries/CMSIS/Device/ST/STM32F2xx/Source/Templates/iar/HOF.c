// UMKA

typedef struct{
    unsigned int        FileCrc;                                //Контрольная сумма на файл	                  4	
    unsigned int        Sign;                                   //Сигнатура системы параметров	                  4	
    unsigned short int  SizeOfFileDescr;                          //Размер всего файла описателя параметров в словах. 2	
    unsigned short int  MenuStart;                                //Адрес начала описателей меню	                  2	Относительно начала файла
    unsigned short int  PrmStart;                                  //Адрес начала описателей параметров	          2	Относительно начала файла
    unsigned short int  QuantityMenu;                              //Количество описателей меню	                  2	
    unsigned short int  QuantityPrm;                            //Количество описателей параметров	          2	
    //unsigned short int  rsrvd;                                  //для выравнивания
    unsigned char       VendorName[32];                            //Строка с наименованием производителя	          32	
    unsigned char       DeviceName[32];                         //Строка с наименованием изделия	                  32
    unsigned int		DeviceType;                                //Тип привода по номенклатуре ТРИОЛ	          2	
    unsigned short int  TopOfJrnFlt;                            //Адрес параметра указателя вершины журнала аварийных остановов.	2	Указывает на номер записи в журнале
    unsigned short int  NumOfRecFlt;                            //Адрес параметра в котором содержится количество записей в журнале аварийных остановов	2	Указывает на количество записей в журнале
    unsigned short int  TopOfJrnAl;                                //Адрес параметра указателя вершины журнала предупреждений.	2	Указывает на номер записи в журнале
    unsigned short int  NumOfRecAl;                                //Адрес параметра в котором содержится количество записей в журнале предупреждений	2	Указывает на количество записей в журнале
    unsigned short int  AdrListPrmJrn;                            //Адрес списка параметров журнала.	2	В списке приводятся номера параметров в записи журнала. 
    unsigned short int  NumPrmInJrn;                            //Количество параметров в списке по полю AdrListPrmJrn 	2	
	unsigned short int  QuantityUst;	
	unsigned short int  JrnStart;
	unsigned short int  QuantityJrn;
    unsigned char       Res[20];                                //Резерв 1	128 - сумма полей	
//    unsigned char       Res[18];                                //Резерв 1	128 - сумма полей	
    unsigned int        HeaderCrc;                              //Контрольная сумма на заголовок	4	Полный размер структуры 128 байт с полем HeaderCrc
}HeadOfFile_type;

// PU24


__packed typedef struct{
    unsigned int        FileCrc;                    //Eiio?ieuiay noiia ia oaee	                  4	
    unsigned int        Sign;                       //Neaiaoo?a nenoaiu ia?aiao?ia	                  4	
    unsigned short int  SizeOfFileDescr;            //?acia? anaai oaeea iienaoaey ia?aiao?ia a neiaao. 2	
    unsigned short int  MenuStart;                  //Aa?an ia?aea iienaoaeae iai?	                  2	Ioiineoaeuii ia?aea oaeea
    unsigned short int  PrmStart;                   //Aa?an ia?aea iienaoaeae ia?aiao?ia	          2	Ioiineoaeuii ia?aea oaeea
    unsigned short int  QuantityMenu;               //Eiee?anoai iienaoaeae iai?	                  2	
    unsigned short int  QuantityPrm;                //Eiee?anoai iienaoaeae ia?aiao?ia	          2	
    unsigned char       VendorName[32];             //No?iea n iaeiaiiaaieai i?iecaiaeoaey	          32	
    unsigned char       DeviceName[32];             //No?iea n iaeiaiiaaieai ecaaeey	                  32    ..82
    unsigned int        DeviceType;                 //Oei i?eaiaa ii iiiaieeaoo?a O?EIE	          4	
    unsigned short int  TopOfJrnFlt;                //Aa?an ia?aiao?a oeacaoaey aa?oeiu ?o?iaea aaa?eeiuo inoaiiaia.	2	Oeacuaaao ia iiia? caiene a ?o?iaea
    unsigned short int  NumOfRecFlt;                //Aa?an ia?aiao?a a eioi?ii niaa??eony eiee?anoai caienae a ?o?iaea aaa?eeiuo inoaiiaia	2	Oeacuaaao ia eiee?anoai caienae a ?o?iaea
    unsigned short int  TopOfJrnAl;                 //Aa?an ia?aiao?a oeacaoaey aa?oeiu ?o?iaea i?aaoi?a?aaiee.	2	Oeacuaaao ia iiia? caiene a ?o?iaea
    unsigned short int  NumOfRecAl;                 //Aa?an ia?aiao?a a eioi?ii niaa??eony eiee?anoai caienae a ?o?iaea i?aaoi?a?aaiee	2	Oeacuaaao ia eiee?anoai caienae a ?o?iaea
    unsigned short int  AdrListPrmJrn;              //Aa?an nienea ia?aiao?ia ?o?iaea.	2	A nienea i?eaiayony iiia?a ia?aiao?ia a caiene ?o?iaea.
    unsigned short int  NumPrmInJrn;                //Eiee?anoai ia?aiao?ia a nienea ii iie? AdrListPrmJrn 	2	
    unsigned short int  QuantityUst;                //Eiee?a?anoai iai?
    unsigned char       Res[24];                    //?aca?a 1	128 - noiia iieae	
    unsigned int        HeaderCrc;                  //Eiio?ieuiay noiia ia caaieiaie	4	Iieiue ?acia? no?oeoo?u 128 aaeo n iieai HeaderCrc
}HeadOfFile_type;
