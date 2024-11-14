//=============================================================================
//              �������� ��� ��45 � 25LC256
//=============================================================================

#include "globalIncludes.h"

//=============================================================================
//                            ----������� ��45----
//===============================================================================*/
// �� ��������� �������� GPIO_ResetBits/GPIO_SetBits �� GPIO�->BSRRH/GPIO�->BSRRH
// �������� ��������
#define spiAT45En();     GPIO_ResetBits(GPIOC, GPIO_Pin_7);
#define spiAT45Dis();    GPIO_SetBits(GPIOC, GPIO_Pin_7);


#define VIEW_AT45_REQUEST_ERROR
// ****************************************************************************
//      ������� ������ � ������ ����� � SPI � ���� � ���������
// ****************************************************************************
#define WRspiByteWaite(pSpi, data, vr)      \
{                                           \
    pSPI->DR = (data);                      \
    while ( ((pSpi->SR & RXNE) == 0)) {;}   \
    (vr) = pSPI->DR;                        \
}
// ****************************************************************************
#define RDspiByteWaite(pSpi, data)          \
{                                           \
    pSPI->DR = 0;                           \
    while ( ((pSpi->SR & RXNE) == 0)) {;}   \
    (data) = pSPI->DR;                      \
}

//===================================================================
void Error_AT_Request ()
{
    //==========================================================
    // ������� ����� ������
    //==========================================================
    GrafBffClear();
    //====================================================================
    // ������� ���������� � ������
    //====================================================================
    sprintf(&StrOut[0], "������ AT45!");
    StrOutFont(0, 0, NO_INV, &StrOut[0], &fontNormal, leftAlign );    //����� ����� ������ ������ � ���������
    //==========================================================
    // ����� �� �����
    //==========================================================
    GrfOut();
}

//===================================================================
// ������� ����������� ������ ������� �������� ������ � �������� �����
// �������� � ������ ��45
//===================================================================
u8 spflWr( void *adrSpfl, void *AdrInData, u16 size )
{

#ifdef VIEW_AT45_REQUEST_ERROR  
   // ���������� ������ - ��� ������� ��������� � AT45
    Error_AT_Request(); 
    while (1)
    {}
#endif

    
    u8                  err;
    u32                 adrSpflRest, tmp;
    u16                 sizePortion;    // ������ ������ ��� ������

    if(size == 0 ) { return 2; }
    err = 0;

    OSSemPend( SPISem, SYS_FOREVER, &err);

        do{
            tmp = (u32)adrSpfl % AT45sizePage;
            adrSpflRest = (u32)adrSpfl - tmp + AT45sizePage;
            sizePortion = AT45sizePage - tmp;

            if( sizePortion > size ){
                sizePortion = size;
            }
            err = spflWrPg( adrSpfl, AdrInData, sizePortion );

            adrSpfl    = (void*)adrSpflRest;
            AdrInData  = (void*)((u32)AdrInData + sizePortion);
            size       -= sizePortion;
        }while(size != 0);

    DMA_Cmd(DMA1_Stream3, DISABLE);
    DMA_Cmd(DMA1_Stream4, DISABLE);
    OSSemPost( SPISem );
    return err;
}
//===================================================================
//  ������ �� Flash AT45DB161D
//  =>* AdrSpfl - ����� Flash
//      AdrLpc  - ����� RAM
//      size    - ����� ������
// *=> ��������� �������� (1 - ������ �������� ��������, 2 - ������ ����������
// ��������, 0 - ��� ������)

// ������� ���������� ������� ����� �������� (������ ��������) ��� ��������
// ����� ������� ��������. �������� ������ ��������������
// � ���������� ��� ������� ����
// ****************************************************************************
at45status_type         at45status;

u8  err;
u8 spflWrPg(void *AdrSpfl, void *AdrInData, u16 size)
{
//    u8                    err     = 0;
//    u32                     SzCur;
//    u32                     NPStart, NPEnd;
//    SPI_TypeDef             *pSPI;
//    at45status_type         at45status;

//    pSPI = SPI2;

    spiInitAT45();
/*
    NPStart = (u32)AdrSpfl / AT45sizePage;  		//����� ��������� ��������
    NPEnd =   ((u32)AdrSpfl + size - 1) / (AT45sizePage); //����� �������� ��������

    if(NPStart == NPEnd){
        SzCur = size;
    }
    else{
        SzCur = AT45sizePage - ((u32)AdrSpfl % AT45sizePage);
    }
*/
    spiAT45En();
    at45cmdWr(AT45cmdP1toBuff, AdrSpfl);            //������� ������ �������� � �����
    spiAT45Dis();
    OSSemPend(AT45Sem, 2, &err);                    //������� ������� �� �������� ����������
    if( err != OS_ERR_NONE ){
        return 1;
    }

    spiAT45En();
    at45cmdWr(AT45cmdWrBuf1, AdrSpfl);              // ������� ������ � ����� ����� �������� ������
    setDmaForAT45_Wr(AdrInData, size);

    OSSemPend(AT45Sem, 10, &err);                   // ������� ������� �� DMA
    if(err != OS_ERR_NONE){
        spiAT45Dis();
        return 1;
    }
//    while ( ((pSPI->SR & BSY) != 0)){;}             // ������� ���� ��� ������� �������
//    while ( ((pSPI->SR & BSY) != 0)){;}
    spiAT45Dis();

    spiAT45En();
    at45cmdWr(AT45cmdWrThroughPage1WE, AdrSpfl);    // ������� ������ ������ 1 � ������
    spiAT45Dis();
    OSSemPend(AT45Sem, 45, &err);                   // ������� ������� �� �������� ����������
    if(err != OS_ERR_NONE){
        return 1;
    }

    spiAT45En();
    at45cmdWr(AT45cmdCmpBuf1, AdrSpfl);             //��������� ������ 1 � ���������� ������ ��� ���������� � ������
    spiAT45Dis();
    OSSemPend(AT45Sem, 2, &err);                    //������� ������� �� �������� ����������
    if (err != OS_ERR_NONE){
        return 1;
    }

    at45status.all = RdStatusAT45();
    if ( at45status.bit.COMP != 0 ){
        return 2;                                   // ������ ��������� ������
    }

    return 0;
}
// ****************************************************************************
//  ������ ������� AT45DB161D
// *=> ���������� ������
// ****************************************************************************
inline
u8 RdStatusAT45(void)
{
	volatile u8            	vr;
    SPI_TypeDef             *pSPI;

    pSPI = SPI2;

    spiAT45En();
    vr = pSPI->DR;      vr = pSPI->DR;
    WRspiByteWaite(pSPI, AT45cmdStatusRead, vr);
    RDspiByteWaite(pSPI, vr);

    spiAT45Dis();

    return (u8)vr;
}
// ****************************************************************************
//  ������ �� Flash AT45DB161D
//  =>* AdrSpfl - ����� Flash
//      AdrLpc  - ����� RAM
//      size    - ����� ������
// *=> ��������� �������� (1 - ������ �������� ��������, 0 - ��� ������)
// ****************************************************************************
u8 spflRd(void *AdrLpc, void *AdrSpfl, u16 size)
{
  
#ifdef VIEW_AT45_REQUEST_ERROR
   // ���������� ������ - ��� ������� ��������� � AT45
    Error_AT_Request(); 
    while (1)
    {}
#endif
  
    u8 err = 0;

    OSSemPend( SPISem, SYS_FOREVER, &err);

        spiInitAT45();

        spiAT45En();
        at45cmdWr(AT45cmdRdAr, AdrSpfl);
        setDmaForAT45_Rd( AdrLpc, size );
        //������� ������� �� DMA
BeginMeasEvent(1);
        OSSemPend(AT45Sem, 100, &err);
        if (err != OS_ERR_NONE){
            err = 1;
        }

        spiAT45Dis();
        DMA_Cmd(DMA1_Stream3, DISABLE);
        DMA_Cmd(DMA1_Stream4, DISABLE);

    OSSemPost( SPISem );
    return err;
}
// ****************************************************************************
//  ������ ������� � Flash AT45DB161D
//  =>* cmd     - �������
//      AdrSpfl - ����� �����
// ****************************************************************************
void at45cmdWr(u8 cmd, void *AdrSpfl)
{
    volatile u16            vr;
    SPI_TypeDef             *pSPI;
    u16                     page, byte;

    pSPI = SPI2;

    page = (u32)AdrSpfl / AT45sizePage;
    byte = (u32)AdrSpfl % AT45sizePage;

    if((AT45cmdRdSigDevice == cmd) ||
       (AT45cmdStatusRead  == cmd)){

        WRspiByteWaite(pSPI, cmd, vr);
    }
    else if((AT45cmdWrBuf1              == cmd) ||
            (AT45cmdP1toBuff            == cmd) ||
            (AT45cmdErPage              == cmd) ||
            (AT45cmdErBlock             == cmd) ||
            (AT45cmdWrThroughPage1NE    == cmd) ||
            (AT45cmdCmpBuf1             == cmd) ||
            (AT45cmdWrThroughPage1WE    == cmd) ){
        WRspiByteWaite(pSPI, cmd, vr);							        //  ��� ����� 161
        WRspiByteWaite(pSPI, ( page >> 5 ), vr);					    //  ( page >> 6 );                       	
        WRspiByteWaite(pSPI, (( page << 3 ) | (( byte >> 8 ) & 7 )), vr);//  ( (page << 2 ) | ((byte >> 8) & 3) )
        WRspiByteWaite(pSPI, ( byte & 0xFF ), vr);                      //  ( byte & 0xff );
        }
    else if( AT45cmdRdAr == cmd ){
        WRspiByteWaite(pSPI, cmd, vr);							        //  ��� ����� 161
        WRspiByteWaite(pSPI, ( page >> 5 ), vr);					    //  ( page >> 6 );                       	
        WRspiByteWaite(pSPI, (( page << 3 ) | (( byte >> 8 ) & 7 )), vr);//  ( (page << 2 ) | ((byte >> 8) & 3) )
        WRspiByteWaite(pSPI, ( byte & 0xFF ), vr);                      //  ( byte & 0xff );
        WRspiByteWaite(pSPI, 0xFF, vr);         // �������������� ����� ��� �������
        WRspiByteWaite(pSPI, 0xFF, vr);
        WRspiByteWaite(pSPI, 0xFF, vr);
        WRspiByteWaite(pSPI, 0xFF, vr);
    }
    else{    // ���������� �������
        spiAT45Dis();   // ������� ���� �� ������ ������
    }
}
// ***************************************************************************
//  ������������� ���, ���������� (����� ��� ����� SPI ��� ��45)
// ***************************************************************************
void spiInitAT45_HARD(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13,  GPIO_AF_SPI2);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    spiAT45Dis();

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    EXTI_InitTypeDef EXTI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource8);

    EXTI_InitStructure.EXTI_Line = EXTI_Line8;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
// ***************************************************************************
//  ������������� SPI ��� ��45
// ***************************************************************************
const SPI_InitTypeDef spiInitForAT45 = {
	SPI_Direction_2Lines_FullDuplex,  		// SPI_Direction;
  	SPI_Mode_Master,                		// SPI_Mode;
  	SPI_DataSize_8b,                		// SPI_DataSize;
  	SPI_CPHA_2Edge,                 		// SPI_CPOL;
  	SPI_CPOL_High,                  		// SPI_CPHA;
  	SPI_NSS_Soft,                   		// SPI_NSS;
  	SPI_BaudRatePrescaler_2,        		// SPI_BaudRatePrescaler;
  	SPI_FirstBit_MSB,               		// SPI_FirstBit;
  	7       								// SPI_CRCPolynomial;
};
// ***************************************************************************
// ***************************************************************************
void spiInitAT45(void)
{
    SPI_I2S_DeInit(SPI2);
    SPI_Init( SPI2, (SPI_InitTypeDef*)&spiInitForAT45 );
    SPI_Cmd(SPI2, ENABLE);
    SPI_CalculateCRC(SPI2, DISABLE);
}
// ***************************************************************************
//  ���������� ���������� �������� BYSY AT45
// ***************************************************************************
void EXTI9_5_IRQHandler(void)
{
    OSIntEnter();

        if(EXTI_GetITStatus(EXTI_Line8) != RESET){
            OSSemPost(AT45Sem);
            EXTI_ClearITPendingBit(EXTI_Line8);
        }
        //EXTI_ClearITPendingBit(EXTI_Line0);

    OSIntExit();
}
//=============================================================================
//=============================================================================
//                 ----������� FRAM/EEPROM----
//=============================================================================
//=============================================================================

#define spiFRAMEn();     GPIO_ResetBits(GPIOC, GPIO_Pin_6);
#define spiFRAMDis();    GPIO_SetBits(GPIOC, GPIO_Pin_6);

// ****************************************************************************
//  ������ �� Flash FM25L256B
//  =>* AdrFlash - ����� ������
//      AdrLpc   - ����� ����
//      sz       - ������� ������
// *=> ��������� ������ (1 - OK, 0 - error)
// ****************************************************************************
u8 FM25Read(void *AdrLpc, void *AdrFRAM, u16 size)
{
    volatile u16            vr;
    SPI_TypeDef             *pSPI;
    u8                      *Buf;
    u8                      err;

    Buf = AdrLpc;

    OSSemPend( SPISem, SYS_FOREVER, &err);

        spiInitFRAM();
        spiFRAMDis();
        spiFRAMEn();

        pSPI = SPI2;
        vr = pSPI->DR;          				// �������� ������
        vr = pSPI->DR;          				// �������� ������

        pSPI->DR = FramRdMem;   				// �������
        while ( ((SPI2->SR & RXNE) == 0)) {;}
        vr = pSPI->DR;          				// �������� ������
        pSPI->DR = (u8)((u32)AdrFRAM >> 8); 	// �����
        while ( ((SPI2->SR & RXNE) == 0)) {;}
        vr = pSPI->DR;          				// �������� ������
        pSPI->DR = (u8)((u32)AdrFRAM);
        while ( ((SPI2->SR & RXNE) == 0)) {;}
        vr = pSPI->DR;          				// �������� ������

        for( ;size!=0; size--){
            SPI2->DR = 0x00;    				// ��������� ��������/������
            while((( SPI2->SR & RXNE )==0)){;}
            *Buf++ = pSPI->DR;
        }while(size > 0); 						//�������� ����������� ������

        spiFRAMDis();

    OSSemPost( SPISem );
    return 0;
}
//===================================================================
//              ������� ������ �� FRAM
//===================================================================
u8 FM25Write(void *AdrFRAM, void *AdrLpc, u16 size)
{
    SPI_TypeDef             *pSPI;
    u32						pageBorder, adrCurr;
    volatile u16            vr;
    u8						*Buf;
    u8                      err;

	if(size == 0) { return 0; }

        OSSemPend( SPISem, SYS_FOREVER, &err);

        pSPI 		= SPI2;
        adrCurr 	= (u32)AdrFRAM;
        pageBorder  = (u32)AdrFRAM;                  	// ������������������� �����
        Buf 		= (u8*)AdrLpc;

        spiInitFRAM();

        vr = pSPI->DR;          						// �������� ������
        vr = pSPI->DR;          						// �������� ������

        do{    											// ���� �� �������� �� ��������� ������
            spiFRAMEn();								// �������������� ������ ��� ������
            pSPI->DR = FramEnWrite;
            while ( ((SPI2->SR & RXNE) == 0)) {;}
            spiFRAMDis();

            vr = pSPI->DR;          					// �������� ������
            vr = pSPI->DR;          					// �������� ������

            pageBorder += szPageEEPROM;
            pageBorder &= MASKpageEEPROM;
            if( pageBorder > (u32)AdrFRAM + size){
                pageBorder = (u32)AdrFRAM + size;
            }

            spiFRAMEn();								// ������� ������

            pSPI->DR = FramWrMem;
            while ( ((SPI2->SR & RXNE) == 0)) {;}
            vr = pSPI->DR;          					// �������� ������

            pSPI->DR = (u8)((u32)adrCurr >> 8); 		// �����
            while ( ((SPI2->SR & RXNE) == 0)) {;}
            vr = pSPI->DR;

            pSPI->DR = (u8)((u32)adrCurr);
            while ( ((SPI2->SR & RXNE) == 0)) {;}
            vr = pSPI->DR;

            do{											// ���� �� �������� ������ � ������
                pSPI->DR = *Buf++;
                adrCurr++;
                while ( ((SPI2->SR & RXNE) == 0)) {;}
                vr = pSPI->DR;
            }while( adrCurr < pageBorder );

            spiFRAMDis(); 								// ������������ �������� � ������
            OSTimeDly(6);

        }while( adrCurr < ( (u32)AdrFRAM + size ) );

    OSSemPost( SPISem );

    return 0;
}


/**
 * @fn  - FM25Fill 
 * @brief � ������� ���������� FRAM ����������� �������
 * @param AdrLpc - ��������� ����� FRAM
 * @param data - ������ ��� ����������
 * @param size - ������ ������
 * @return -
 */
u8 FM25Fill(void *AdrFRAM, u8 data, u16 size)
{
    SPI_TypeDef             *pSPI;
    u32			    pageBorder, adrCurr;
    volatile u16            vr;
    u8                      err;

    if(size == 0) { return 0; }

    OSSemPend( SPISem, SYS_FOREVER, &err);

    pSPI 		= SPI2;
    adrCurr 	= (u32)AdrFRAM;
    pageBorder  = (u32)AdrFRAM;                  	// ������������������� �����

    spiInitFRAM();

    vr = pSPI->DR;          						// �������� ������
    vr = pSPI->DR;          						// �������� ������

    do{    											// ���� �� �������� �� ��������� ������
        spiFRAMEn();								// �������������� ������ ��� ������
        pSPI->DR = FramEnWrite;
        while ( ((SPI2->SR & RXNE) == 0)) {;}
        spiFRAMDis();

        vr = pSPI->DR;          					// �������� ������
        vr = pSPI->DR;          					// �������� ������

        pageBorder += szPageEEPROM;
        pageBorder &= MASKpageEEPROM;
        if( pageBorder > (u32)AdrFRAM + size){
            pageBorder = (u32)AdrFRAM + size;
        }

        spiFRAMEn();								// ������� ������

        pSPI->DR = FramWrMem;
        while ( ((SPI2->SR & RXNE) == 0)) {;}
        vr = pSPI->DR;          					// �������� ������

        pSPI->DR = (u8)((u32)adrCurr >> 8); 		// �����
        while ( ((SPI2->SR & RXNE) == 0)) {;}
        vr = pSPI->DR;

        pSPI->DR = (u8)((u32)adrCurr);
        while ( ((SPI2->SR & RXNE) == 0)) {;}
        vr = pSPI->DR;

        do{											// ���� �� �������� ������ � ������
            pSPI->DR = data;
            adrCurr++;
            while ( ((SPI2->SR & RXNE) == 0)) {;}
            vr = pSPI->DR;
        }while( adrCurr < pageBorder );

        spiFRAMDis(); 								// ������������ �������� � ������
        OSTimeDly(6);

    }while( adrCurr < ( (u32)AdrFRAM + size ) );

    OSSemPost( SPISem );

    return 0;
}


// ***************************************************************************
//          ������������� SPI ��� FRAM
// ***************************************************************************
const SPI_InitTypeDef   spiInitFor25LC256 = {
	SPI_Direction_2Lines_FullDuplex,  		// SPI_Direction;
  	SPI_Mode_Master,                		// SPI_Mode;
  	SPI_DataSize_8b,                		// SPI_DataSize;
  	SPI_CPHA_1Edge,                 		// SPI_CPOL;
  	SPI_CPOL_Low,                  		    // SPI_CPHA;
  	SPI_NSS_Soft,                   		// SPI_NSS;
  	SPI_BaudRatePrescaler_8,        		// SPI_BaudRatePrescaler;  // Fpclk/8 = 30/8 = 3.75 Mbit
  	SPI_FirstBit_MSB,               		// SPI_FirstBit;
  	7       								// SPI_CRCPolynomial;
};

void spiInitFRAM(void)
{
    SPI_Cmd (SPI2, DISABLE);
    SPI_Init(SPI2, (SPI_InitTypeDef*)&spiInitFor25LC256);
    SPI_Cmd (SPI2, ENABLE);
    SPI_CalculateCRC(SPI2, DISABLE);
}
// ***************************************************************************
//  ������������� ��� � ����� ���� ����� SPI ��� FRAM
// ***************************************************************************
void spiInitFRAM_HARD(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // CS pin
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    spiFRAMDis();
}
//===================  End of file  =================================
