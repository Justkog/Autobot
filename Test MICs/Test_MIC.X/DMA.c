
#include <p32xxxx.h>
#include <sys/attribs.h>
#include "types.h"

/*
 *
 */

s16 buffer[3][2048];               //ADC buffer
u32 adc_buffer_id = 0;

void __ISR(_DMA_0_VECTOR, IPL6SOFT) DMAHANDLER(void)
{

    IFS1bits.DMA0IF = 0;
}

void    init_DMA()
{
    
    
}