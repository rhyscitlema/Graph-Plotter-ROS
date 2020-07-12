
/****************************************************************************
    dma.c
    8237 ISA Direct Memory Access Controller (DMAC)
****************************************************************************/

#ifndef ARCH_X86
#error "[dma.cpp for i86] requires i86 architecture. Define ARCH_X86"
#endif

#include "dma.h"
#include "hal.h"


void dma_mask_channel(uint8b channel)      // masks a channel
{
	if (channel <= 4)
		outportb(DMA0_CHANMASK_REG, (1 << (channel-1)));
	else
		outportb(DMA1_CHANMASK_REG, (1 << (channel-5)));
}

void dma_unmask_channel (uint8b channel)   // unmasks a channel
{
	if (channel <= 4)
		outportb(DMA0_CHANMASK_REG, channel);
	else
		outportb(DMA1_CHANMASK_REG, channel);
}

void dma_unmask_all (int32b dma)        // unmasks all channels
{
	// it doesnt matter what is written to this register
	outportb(DMA1_UNMASK_ALL_REG, 0xff);
    dma=0; // no use!
}

void dma_reset (int32b dma)             // resets controller to defaults
{
	// it doesnt matter what is written to this register
	outportb(DMA0_TEMP_REG, 0xff);
    dma=0; // no use!
}

void dma_reset_flipflop(int32b dma)     // resets flipflop
{
	if (dma < 2)
		return;
	// it doesnt matter what is written to this register
	outportb( (dma==0) ? DMA0_CLEARBYTE_FLIPFLOP_REG : DMA1_CLEARBYTE_FLIPFLOP_REG, 0xff);
}

// sets the address of a channel
void dma_set_address(uint8b channel, uint8b low, uint8b high)
{
    unsigned short port;
	if ( channel > 8 )
		return;
	port = 0;
	switch ( channel )
	{
		case 0: {port = DMA0_CHAN0_ADDR_REG; break;}
		case 1: {port = DMA0_CHAN1_ADDR_REG; break;}
		case 2: {port = DMA0_CHAN2_ADDR_REG; break;}
		case 3: {port = DMA0_CHAN3_ADDR_REG; break;}
		case 4: {port = DMA1_CHAN4_ADDR_REG; break;}
		case 5: {port = DMA1_CHAN5_ADDR_REG; break;}
		case 6: {port = DMA1_CHAN6_ADDR_REG; break;}
		case 7: {port = DMA1_CHAN7_ADDR_REG; break;}
	}
	outportb(port, low);
	outportb(port, high);
}

// sets the counter of a channel
void dma_set_count(uint8b channel, uint8b low, uint8b high)
{
    unsigned short port;
	if ( channel > 8 )
		return;
	port = 0;
	switch ( channel )
	{
		case 0: {port = DMA0_CHAN0_COUNT_REG; break;}
		case 1: {port = DMA0_CHAN1_COUNT_REG; break;}
		case 2: {port = DMA0_CHAN2_COUNT_REG; break;}
		case 3: {port = DMA0_CHAN3_COUNT_REG; break;}
		case 4: {port = DMA1_CHAN4_COUNT_REG; break;}
		case 5: {port = DMA1_CHAN5_COUNT_REG; break;}
		case 6: {port = DMA1_CHAN6_COUNT_REG; break;}
		case 7: {port = DMA1_CHAN7_COUNT_REG; break;}
	}
	outportb(port, low);
	outportb(port, high);
}

void dma_set_mode (uint8b channel, uint8b mode)
{
	int dma = (channel < 4) ? 0 : 1;
	int chan = (dma==0) ? channel : channel-4;
	dma_mask_channel (channel);
	outportb ( (channel < 4) ? (DMA0_MODE_REG) : DMA1_MODE_REG, chan | (mode) );
	dma_unmask_all ( dma );
}

void dma_set_read (uint8b channel)     // prepares channel for read
{
	dma_set_mode (channel,	DMA_MODE_READ_TRANSFER | DMA_MODE_TRANSFER_SINGLE);
}

void dma_set_write (uint8b channel)    // prepares channel for write
{
	dma_set_mode (channel,
		DMA_MODE_WRITE_TRANSFER | DMA_MODE_TRANSFER_SINGLE);
}

// writes to an external page register
void dma_set_external_page_register (uint8b reg, uint8b val)
{
    unsigned short port;
	if (reg > 14)
		return;
	port = 0;
	switch ( reg )
	{
		case 1: {port = DMA_PAGE_CHAN1_ADDRBYTE2; break;}
		case 2: {port = DMA_PAGE_CHAN2_ADDRBYTE2; break;}
		case 3: {port = DMA_PAGE_CHAN3_ADDRBYTE2; break;}
		case 4: {return;}// nothing should ever write to register 4
		case 5: {port = DMA_PAGE_CHAN5_ADDRBYTE2; break;}
		case 6: {port = DMA_PAGE_CHAN6_ADDRBYTE2; break;}
		case 7: {port = DMA_PAGE_CHAN7_ADDRBYTE2; break;}
	}
	outportb(port, val);
}
