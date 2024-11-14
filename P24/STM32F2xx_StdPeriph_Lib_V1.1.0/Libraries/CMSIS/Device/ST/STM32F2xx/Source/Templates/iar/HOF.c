// UMKA

typedef struct{
    unsigned int        FileCrc;                                //����������� ����� �� ����	                  4	
    unsigned int        Sign;                                   //��������� ������� ����������	                  4	
    unsigned short int  SizeOfFileDescr;                          //������ ����� ����� ��������� ���������� � ������. 2	
    unsigned short int  MenuStart;                                //����� ������ ���������� ����	                  2	������������ ������ �����
    unsigned short int  PrmStart;                                  //����� ������ ���������� ����������	          2	������������ ������ �����
    unsigned short int  QuantityMenu;                              //���������� ���������� ����	                  2	
    unsigned short int  QuantityPrm;                            //���������� ���������� ����������	          2	
    //unsigned short int  rsrvd;                                  //��� ������������
    unsigned char       VendorName[32];                            //������ � ������������� �������������	          32	
    unsigned char       DeviceName[32];                         //������ � ������������� �������	                  32
    unsigned int		DeviceType;                                //��� ������� �� ������������ �����	          2	
    unsigned short int  TopOfJrnFlt;                            //����� ��������� ��������� ������� ������� ��������� ���������.	2	��������� �� ����� ������ � �������
    unsigned short int  NumOfRecFlt;                            //����� ��������� � ������� ���������� ���������� ������� � ������� ��������� ���������	2	��������� �� ���������� ������� � �������
    unsigned short int  TopOfJrnAl;                                //����� ��������� ��������� ������� ������� ��������������.	2	��������� �� ����� ������ � �������
    unsigned short int  NumOfRecAl;                                //����� ��������� � ������� ���������� ���������� ������� � ������� ��������������	2	��������� �� ���������� ������� � �������
    unsigned short int  AdrListPrmJrn;                            //����� ������ ���������� �������.	2	� ������ ���������� ������ ���������� � ������ �������. 
    unsigned short int  NumPrmInJrn;                            //���������� ���������� � ������ �� ���� AdrListPrmJrn 	2	
	unsigned short int  QuantityUst;	
	unsigned short int  JrnStart;
	unsigned short int  QuantityJrn;
    unsigned char       Res[20];                                //������ 1	128 - ����� �����	
//    unsigned char       Res[18];                                //������ 1	128 - ����� �����	
    unsigned int        HeaderCrc;                              //����������� ����� �� ���������	4	������ ������ ��������� 128 ���� � ����� HeaderCrc
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
