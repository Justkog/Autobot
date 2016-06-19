
#include <p32xxxx.h>
#include <sys/attribs.h>
#include "types.h"

//#include <plib.h>

/*
 *
 */

s32 buffer[3][256];               //ADC buffer
u32 adc_buffer_id = 0;

unsigned int Virt2Phy0(const void* p)
{
    return (int)p<0?((int)p&0x1FFFFFFFL):(unsigned int)((unsigned char*)p+0x40000000L);
}

void __ISR(_DMA_0_VECTOR, IPL6SOFT) DMA0HANDLER(void)
{
    u32 i = 0;
    while (i < 32)
    {
        log_key_val("dest value", buffer[0][i]);
        //buffer[0][i] = 0;
        i++;
    }

    log_key_val("ADC BUF0 value", ADC1BUF0);

    s32 count = 0;
    while (count < 320000)
        count++;
    IEC1bits.AD1IE = 1;             //restart ADC interrupt

    DCH0INTbits.CHDDIF = 0;         // Clear the Channel block transfer complete interrupt flag
    IFS1bits.DMA0IF = 0;            // Clear the DMA0 interrupt flags
}

void    init_DMA()
{
    IEC1bits.DMA0IE = 0;            // Disable DMA channel 0 interrupts
    IFS1bits.DMA0IF = 0;            // Clear any existing DMA channel 0 interrupt flag

    DMACONbits.ON = 1;              // Enable the DMA controller

    DCH0CONbits.CHEN = 0;           // Channel 0 off
    DCH0CONbits.CHCHN = 0;          // no chaining
    DCH0CONbits.CHPRI = 3;          // Priority 3 (highest)

    // Program the transfer
    DCH0SSA = Virt2Phy0(&ADC1BUF0);             // Transfer source physical address
    DCH0DSA = Virt2Phy0(buffer[0]);             // Transfer destination physical address
    DCH0SSIZ = sizeof(ADC1BUF0);                // Source size
    DCH0DSIZ = sizeof(buffer[0]);               // Destination size
    DCH0CSIZ = sizeof(ADC1BUF0);                // bytes transferred per event

    DCH0ECONbits.CHSIRQ = _ADC_IRQ;             //link DMA start to ADC interrupt

    DCH0INTCLR = 0x00FF00FF;        // Clear existing events, disable all interrupts
    DCH0INTbits.CHERIE = 1;         // Enable Channel address error interrupt
    DCH0INTbits.CHDDIE = 1;         // Enable Channel destination done interrupt

    IPC9bits.DMA0IP = 6;            // Set Interrupt priority to 6
    IEC1bits.DMA0IE = 1;            // Enable DMA Channel 0 interrupts

    
}