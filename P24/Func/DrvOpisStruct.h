/**********************************************
 *                                            *
 *    (c) Copyright _Triol Corporation 2010   *
 *                                            *
 * File name   : DrvOpisStruct.h              *
 * Description : ����������� � ���������      *
 *     ��� �������                            *
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

#pragma pack(push)  // ������. ���. �������� ������������ � ����� ������������
#pragma pack(1)     // �����. �������� ������������ � 1 ����

typedef struct{
    unsigned int        FileCrc;                                //����������� ����� �� ����	                  4	
    unsigned int        Sign;                                   //��������� ������� ����������	                  4	
    unsigned int        SizeOfFileDescr;                        //������ ����� ����� ��������� ���������� � ������. 4	
    unsigned char       MaxLengthName;                          //������������ ���������� �������� � ������������ ���������, ������ 1
    unsigned char       MaxLengthMess;                          //������������ ���������� �������� � ������������ �������           1
    unsigned char       MaxLengthHelp;                          //������������ ���������� �������� � ������������ Help              1
    unsigned char       MaxLengthEdIzm;                         //������������ ���������� �������� � ������������ ������� ��������� 1
    unsigned int        MenuStart;                              //����� ������ ���������� ����	                    4	������������ ������ �����
    unsigned int        PrmStart;                               //����� ������ ���������� ����������	            4	������������ ������ �����
    unsigned short int  QuantityMenu;                           //���������� ���������� ����	                    2	
    unsigned short int  QuantityPrm;                            //���������� ���������� ����������	                2	
    //unsigned short int  rsrvd;                                //��� ������������
    unsigned char       VendorName[32];                         //������ � ������������� �������������	            32	
    unsigned char       DeviceName[32];                         //������ � ������������� �������	                32
    unsigned int		DeviceType;                             //��� ������� �� ������������ �����	                4	
    unsigned short int  TopOfJrnFlt;                            //����� ��������� ��������� ������� ������� ��������� ���������.	2	��������� �� ����� ������ � �������
    unsigned short int  NumOfRecFlt;                            //����� ��������� � ������� ���������� ���������� ������� � ������� ��������� ���������	2	��������� �� ���������� ������� � �������
    unsigned short int  TopOfJrnAl;                             //����� ��������� ��������� ������� ������� ��������������.	2	��������� �� ����� ������ � �������
    unsigned short int  NumOfRecAl;                             //����� ��������� � ������� ���������� ���������� ������� � ������� ��������������	2	��������� �� ���������� ������� � �������
    unsigned short int  AdrListPrmJrn;                          //����� ������ ���������� �������.	2	� ������ ���������� ������ ���������� � ������ �������.
    unsigned short int  NumPrmInJrn;                            //���������� ���������� � ������ �� ���� AdrListPrmJrn 	2	
	unsigned short int  QuantityUst;	                        //���������� �������        2
	unsigned int        JrnStart;                               //����� ������ ����������   4
	unsigned short int  QuantityJrn;                            //���������� ��������
    unsigned short int  QuantityFildJrn;                        //���������� ���������� ����� ���������� �������
    unsigned int        MessPultStart;
    unsigned short int  QuantityMessPult;
    unsigned int        Pnt1LngTxt;                             //������ �� ������ ��������� ���������� ������� �����   4
    unsigned int        QuantityLng;                            //����������  �������������� ������   4
    unsigned char       Res[14];                                 //������ 1	128 - ����� �����	
    unsigned int        HeaderCrc;                              //����������� ����� �� ���������	4	������ ������ ��������� 128 ���� � ����� HeaderCrc
}HeadOfFile_type;

#pragma pack(pop) // �������������� ������������.

// ������ ��������� ��������� ���
#pragma pack(push)  // ������. ���. �������� ������������ � ����� ������������
#pragma pack(1)     // �����. �������� ������������ � 1 ����

typedef struct{
    unsigned int        FileCrc;                                //����������� ����� �� ����	                  4	
    unsigned int        Sign;                                   //��������� ������� ����������	                  4	
    unsigned short int  SizeOfFileDescr;                        //������ ����� ����� ��������� ���������� � ������. 2	
    unsigned short int  MenuStart;                              //����� ������ ���������� ����	                  2	������������ ������ �����
    unsigned short int  PrmStart;                               //����� ������ ���������� ����������	          2	������������ ������ �����
    unsigned short int  QuantityMenu;                           //���������� ���������� ����	                  2	
    unsigned short int  QuantityPrm;                            //���������� ���������� ����������	          2	
    //unsigned short int  rsrvd;                                //��� ������������
    unsigned char       VendorName[32];                         //������ � ������������� �������������	          32	
    unsigned char       DeviceName[32];                         //������ � ������������� �������	                  32
    unsigned int		DeviceType;                             //��� ������� �� ������������ �����	          2	
    unsigned short int  TopOfJrnFlt;                            //����� ��������� ��������� ������� ������� ��������� ���������.	2	��������� �� ����� ������ � �������
    unsigned short int  NumOfRecFlt;                            //����� ��������� � ������� ���������� ���������� ������� � ������� ��������� ���������	2	��������� �� ���������� ������� � �������
    unsigned short int  TopOfJrnAl;                             //����� ��������� ��������� ������� ������� ��������������.	2	��������� �� ����� ������ � �������
    unsigned short int  NumOfRecAl;                             //����� ��������� � ������� ���������� ���������� ������� � ������� ��������������	2	��������� �� ���������� ������� � �������
    unsigned short int  AdrListPrmJrn;                          //����� ������ ���������� �������.	2	� ������ ���������� ������ ���������� � ������ �������.
    unsigned short int  NumPrmInJrn;                            //���������� ���������� � ������ �� ���� AdrListPrmJrn 	2	
	unsigned short int  QuantityUst;	
	unsigned short int  JrnStart;
	unsigned short int  QuantityJrn;
    unsigned char       Res[20];                                //������ 1	128 - ����� �����	
//    unsigned char       Res[18];                               //������ 1	128 - ����� �����	
    unsigned int        HeaderCrc;                              //����������� ����� �� ���������	4	������ ������ ��������� 128 ���� � ����� HeaderCrc
}HeadOfFileOld_type;

#pragma pack(pop) // �������������� ������������.
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
