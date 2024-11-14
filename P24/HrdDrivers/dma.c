/*****************************************************************************

******************************************************************************/
#include "globalIncludes.h"

extern OS_EVENT *AT45Sem;

u8 dummyDMA;			// ������� ��� ������ ������ ��� ������ � ��������

//===================================================================
//      ���������� ���������� �� ��������� ������ SPI
//===================================================================
void DMA1_Stream3_IRQHandler(void)
{
    OSIntEnter();

        DMA_ClearITPendingBit( DMA1_Stream3, DMA_IT_TCIF3 | DMA_IT_TEIF3);

        SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, DISABLE);
        SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, DISABLE);

        DMA_Cmd(DMA1_Stream3, DISABLE);
        DMA_Cmd(DMA1_Stream4, DISABLE);

        OSSemPost(AT45Sem);
    OSIntExit();
}
/******************************************************************************
Function name:   DMA_Init

Descriptions: ������������� DMA ��� ������ � SPI.
				�������� ���� ��� ��� ������ ���������

** parameters:
** Returned value:
**
******************************************************************************/
void DMAInitForSpi()
{
    DMA_InitTypeDef 					DMA_InitStructure;
    NVIC_InitTypeDef 					NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    DMA_ClearFlag(DMA1_Stream3, DMA_IT_TCIF3 | DMA_IT_TEIF3);
    DMA_ClearFlag(DMA1_Stream4, DMA_IT_TCIF4 | DMA_IT_TEIF4);


    // TX channel configuration
    DMA_Cmd(DMA1_Stream4, DISABLE);
    DMA_InitStructure.DMA_Channel 				= DMA_Channel_0;
    DMA_InitStructure.DMA_PeripheralBaseAddr 	= (u32)&SPI2->DR;
    DMA_InitStructure.DMA_DIR 					= DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Priority 				= DMA_Priority_Medium;
    DMA_InitStructure.DMA_Mode 					= DMA_Mode_Normal;
    DMA_InitStructure.DMA_FIFOMode 				= DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold 		= DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst 			= DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst 		= DMA_PeripheralBurst_Single;

    DMA_InitStructure.DMA_BufferSize 			= 0;				// ��� ��������� ������������� ����� ����� 0
    DMA_InitStructure.DMA_Memory0BaseAddr 		= (u32)&dummyDMA;
    DMA_InitStructure.DMA_MemoryInc 			= DMA_MemoryInc_Disable;

    DMA_Init(DMA1_Stream4, &DMA_InitStructure);
    DMA_Cmd(DMA1_Stream4, ENABLE);

    // RX channel configuration
    DMA_InitStructure.DMA_DIR                	= DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_Priority           	= DMA_Priority_VeryHigh;

    DMA_Init(DMA1_Stream3, &DMA_InitStructure);
    DMA_Cmd(DMA1_Stream3, ENABLE);

	// ������������� ���������� �� DMA
    NVIC_InitStructure.NVIC_IRQChannel 					 = DMA1_Stream3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0f;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority 		 = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd 				 = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
//===================================================================
// ����������������� ��
//===================================================================
// DMA1_Stream3 �����
// DMA1_Stream4 ��������

void setDmaForAT45_Rd(void *Buf, u16 sz)
{
	DMA1_Stream3->CR	= 0;				// ���������� ��������� DMA � ��� ����� � ���������
    DMA1_Stream4->CR	= 0;

    DMA_ClearFlag(DMA1_Stream3, DMA_IT_TCIF3 | DMA_IT_TEIF3);
    DMA_ClearFlag(DMA1_Stream4, DMA_IT_TCIF4 | DMA_IT_TEIF4);

	DMA1_Stream3->NDTR	= sz;				// ���������� ������ ����������
	DMA1_Stream4->NDTR	= sz;

	DMA1_Stream3->M0AR 	= (u32)Buf;			// ���������� ������
	DMA1_Stream4->M0AR 	= (u32)&dummyDMA;

	DMA1_Stream3->CR	= ( DMA_DIR_PeripheralToMemory | DMA_MemoryInc_Enable  | DMA_Priority_VeryHigh | DMA_IT_TC );
 	DMA1_Stream4->CR	= ( DMA_DIR_MemoryToPeripheral | DMA_MemoryInc_Disable | DMA_Priority_Medium );

    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);
    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);

 	DMA_Cmd(DMA1_Stream3, ENABLE);		// ��������
 	DMA_Cmd(DMA1_Stream4, ENABLE);  	// ��������
}
//===================================================================
//===================================================================
// DMA1_Stream3 �����
// DMA1_Stream4 ��������

void setDmaForAT45_Wr(void *Buf, u16 sz)
{
	DMA1_Stream3->CR	= 0;				// ���������� ��������� DMA � ��� ����� � ���������
    DMA1_Stream4->CR	= 0;

    DMA_ClearFlag(DMA1_Stream3, DMA_IT_TCIF3 | DMA_IT_TEIF3);
    DMA_ClearFlag(DMA1_Stream4, DMA_IT_TCIF4 | DMA_IT_TEIF4);

	DMA1_Stream3->NDTR	= sz;				// ���������� ������ ����������
	DMA1_Stream4->NDTR	= sz;

	DMA1_Stream3->M0AR 	= (u32)&dummyDMA;
	DMA1_Stream4->M0AR 	= (u32)Buf;			// ���������� ������

	DMA1_Stream3->CR	= ( DMA_DIR_PeripheralToMemory | DMA_MemoryInc_Disable | DMA_Priority_VeryHigh | DMA_IT_TC 	);
 	DMA1_Stream4->CR	= ( DMA_DIR_MemoryToPeripheral | DMA_MemoryInc_Enable  | DMA_Priority_Medium 				);

    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);
    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);

 	DMA_Cmd(DMA1_Stream3, ENABLE);		// ��������
 	DMA_Cmd(DMA1_Stream4, ENABLE);
}
// ======================== End Of File =============================
