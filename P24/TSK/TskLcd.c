
#include "globalIncludes.h"

//===================================================================
//  ������ ��������� ���� � ������� � ��������� ����������
//===================================================================
#include "TestScreen.h"

__noreturn void TaskLCD(void *pdata)
{
    u8 err;

    
    //memcpy(VideoRAM, Triol, sizeof(VideoRAM));  // ����������� �������� � �����
    //memcpy(VideoRAM, Screen1, sizeof(VideoRAM));  // ����������� �������� � �����
    LCD_init_hw();                  // ���������� ���� ��� ������� � ������������������� ��������� � ����� ����������
    initLED();                      // ������������������� ����������
    
    LCD_init();                     // ���������� ���� ��� ���������� ��������
    
    //=============================================
#ifdef SUBSTITUTE_BOOTLOADER
    // ������� ���������� ��������� � ����������, ����� ��������� �������� ��������� �� ��� JTAG'a
    Bootloader_type     Bootloader;
    // ��������� CRC ��������� 
    u32 StartAddr = ADDR_FLASH_SECTOR_4;
    u32 crcSize = (ADDR_FLASH_SECTOR_7 - ADDR_FLASH_SECTOR_4);
    u16 crcPrg = GetCrc((void *)(StartAddr), crcSize);
    
    FM25RD(&Bootloader, &nv.Bootloader, sizeof(Bootloader)); // ������ ������ Bootloader'a
    Bootloader.HOLF.Signatura = 2;
    Bootloader.HOLF.VersiaPO = 36013;
    Bootloader.HOLF.CrcFile = crcPrg;
    Bootloader.HOLF.SizeFile = crcSize;
    Bootloader.FlagLoader.ALL = StateLoader_READY;
    memcpy(&Bootloader.HOLFShadow, &Bootloader.HOLF, sizeof(Bootloader.HOLFShadow));
    FM25WR(&nv.Bootloader, &Bootloader, sizeof(Bootloader));
#endif
//=============================================
  
        
/*    
     // ����������� �������� ������ FRAM ��� ������������ ������ ������ ������
    u8 flag = 0;
    if (flag)
    {
        u8 FileData1[128];
        void *pAddr = 0;
        u8 FileData2[128] = {0};
        memset ((void *)FileData1, 0xFF, sizeof(FileData1));
    
        for (u8 j=0; j<200; j++)
        {
           FM25WR(pAddr, &FileData1, sizeof(FileData1)); // ������ � FM25    
           FM25RD(&FileData2, pAddr, sizeof(FileData2)); // ������ �� FM25
           pAddr = (void *)((u32)pAddr + sizeof(FileData1));
        }
    }
*/    
    
    //GrfOut();                       // ��������� �������� �� �������
    //Anime();                        
    //OSTimeDly(1000);              // ������� �������� 2 ���
    //memcpy(VideoRAM, Screen2, sizeof(VideoRAM));  // ����������� �������� � �����
    //GrfOut();
    //OSTimeDly(1000);              // ������� �������� 2 ���

    MicroProg_Init((u32)RamArea, (const MicroProg_type *)MicroProgTabl);
    
    AllPrmPnt->KONTRAST = DEFAULT_CONTRAST;
    setContrast();                  // ��������� ��������� ���������
    
    
    //Anime(); 
    //Snake();
    //Version();
    
    OSTaskCreate(TaskControl, (void *)0, &ControlTaskStack[STK_CONTROL_SIZE - 1], 8);  // �������� ������ Control
    #if OS_TASK_NAME_EN > 0
        OSTaskNameSet(7, "ControlTask", &err);
    #endif

//TestScreen();		// WARNING
        
    OSTaskCreate(TaskDRV, (void *)0, &DRVStack[STKDRVSIZE - 1], 5);  // �������� ������ DRV
    #if OS_TASK_NAME_EN > 0u
        OSTaskNameSet(4, "DRV Task", &err);
    #endif
    Test_with_NVSA();      
    Anime(); 
    //Snake();

    Version();
   
    //InitWD();
    
    HandlerMenuPult();              // ��������� ����. ���� ������ ���. ������ ������ �� ������������
}
//===================================================================
//                          ������
//===================================================================

/*********************************************
 * ������ �������������� �������             *
 *  =>* AdrNet - ������� ����� �������       *
 *********************************************/
unsigned char InstallDrive(void)
{
/*    unsigned char AdrNet = 1;
    unsigned int  AdrData = 528;
    unsigned int Sz;
    unsigned int sz;*/

//    FM25Read(&nv.FATtableDescr[0], &FATtable[0], sizeof(FATtable[0]) * MAX_DRIVE);

/*        if(Sz = RdDriveIden(AdrNet)){                       //������� ����� �� �������
            if(Sz = RdFileDescriptorParam(AdrNet)){
                //AT45Write((void *)AdrData, &DataBuf[0], Sz);
//                while(1){
//                    AT45Write((void *)AdrData, &DataBuf[0], 10);
                //AT45Read(&DataBuf[0], (void *)AdrData, Sz);
                    AT45Read(&DataBuf[0], (void *)AdrData, 1);
//                }

                Sz = HeadOfFile.SizeOfFileDescr;

                do{
                    if(Sz > 100){
                        sz = 100;
                        Sz -= 100;
                    }
                    else{
                        sz = Sz;
                        Sz = 0;
                    }

                    RdDriveAll(AdrNet, AdrData, sz, READ_FLASH);
                    AT45Write((void *)AdrData, &DataBuf[0], Sz);
                    AT45Read(&DataBuf[0], (void *)AdrData, Sz);

                    AdrData += sz;
                }while(Sz != 0);
            }
        }
        */
        return 1;
}

int putchar(int value)
{
    //str11[indexPut++] = (char)value;
    return value;
}
//===================================================================

