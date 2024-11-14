/*****************************************************************************
******************************************************************************/
#ifndef DMA_H
#define DMA_H

#include "globalIncludes.h"

void DMAInitForSpi();

void setDmaForAT45_Rd(void *Buf, u16 sz);
void setDmaForAT45_Wr(void *Buf, u16 sz);

void DMA1_Stream3_IRQHandler(void);

#endif // DMA_H
// ====================== End Of File ===============================
