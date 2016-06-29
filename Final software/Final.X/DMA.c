
#include <p32xxxx.h>
#include <sys/attribs.h>
#include "types.h"

//#include <plib.h>

/*
 *
 */

s32 buffer[3][128];               //ADC buffer
u32 adc_buffer_id = 0;

unsigned int Virt2Phy0(const void* p)
{
    return (int)p<0?((int)p&0x1FFFFFFFL):(unsigned int)((unsigned char*)p+0x40000000L);
}

void __ISR(_DMA_0_VECTOR, IPL4SOFT) DMA0HANDLER(void)
{
    u32 i = 0;
    while (i < 16)
    {
        log_key_val("b0", buffer[0][i]);
        //log_key_val("dest 1 value", buffer[1][i]);
        //buffer[0][i] = 0;
        i++;
    }

    put_str_ln("Done");
    //log_key_val("ADC BUF0 value", ADC1BUF0);

    //Just wait a little til restarting the ADC to not get another part with the end of the clap
    s32 count = 0;
    while (count < 320000)
        count++;
    
    //IEC0bits.AD1IE = 1;             //restart ADC interrupt

    DCH0INTbits.CHDDIF = 0;
    IFS1bits.DMA0IF = 0;
    DCH0CONbits.CHEN = 0;

    IEC0bits.AD1IE = 1;

    //DCH1INTbits.CHDDIF = 0;         // Clear the Channel block transfer complete interrupt flag
    //IFS1bits.DMA1IF = 0;            // Clear the DMA0 interrupt flags
}

void    init_DMA()
{
    IEC1bits.DMA0IE = 0;            // Disable DMA channel 0 interrupts
    IFS1bits.DMA0IF = 0;            // Clear any existing DMA channel 0 interrupt flag
    //IEC1bits.DMA1IE = 0;            // Disable DMA channel 1 interrupts
    //IFS1bits.DMA1IF = 0;            // Clear any existing DMA channel 1 interrupt flag

    //DMACONbits.ON = 1;              // Enable the DMA controller

    // Master Channel 0 setup
    DCH0CONbits.CHEN = 0;           // Channel 0 off
    DCH0CONbits.CHCHN = 0;          // no chaining
    DCH0CONbits.CHPRI = 3;          // Priority 3 (highest)
    DCH0CONbits.CHAEN = 1;
    // Slave Channel 1 setup
    /*DCH1CONbits.CHEN = 0;           // Channel 1 off
    DCH1CONbits.CHCHN = 1;          // chaining
    DCH1CONbits.CHPRI = 2;          // Priority 2
    DCH1CONbits.CHAED = 1;          // Channel start/abort events will be registered, even if the channel is disabled
    DCH1CONbits.CHCHNS = 0;         // Chain to channel higher in natural priority (CH1 will be enabled by CH0 transfer complete*/

    // Program the transfer on Channel 1
    DCH0SSA = Virt2Phy0(&ADC1BUF2);             // Transfer source physical address
    DCH0DSA = Virt2Phy0(buffer[0]);             // Transfer destination physical address
    DCH0SSIZ = sizeof(ADC1BUF0);                // Source size
    DCH0DSIZ = sizeof(buffer[0]);               // Destination size
    DCH0CSIZ = sizeof(ADC1BUF0);                // bytes transferred per event

    // Program the transfer on Channel 2
    /*DCH1SSA = Virt2Phy0(&ADC1BUF1);             // Transfer source physical address
    DCH1DSA = Virt2Phy0(buffer[1]);             // Transfer destination physical address
    DCH1SSIZ = sizeof(ADC1BUF1);                // Source size
    DCH1DSIZ = sizeof(buffer[1]);               // Destination size
    DCH1CSIZ = sizeof(ADC1BUF1);                // bytes transferred per event*/
    
    DCH0ECONbits.CHSIRQ = _ADC_IRQ;             //link DMA channel 0 start to ADC interrupt
    //DCH1ECONbits.CHSIRQ = _ADC_IRQ;             //link DMA channel 1 start to ADC interrupt
    DCH0ECONbits.SIRQEN = 1;

    // Master Channel 0
    DCH0INTCLR = 0x00FF00FF;        // Clear existing events, disable all interrupts
    DCH0INTbits.CHERIE = 1;         // Enable Channel address error interrupt
    DCH0INTbits.CHDDIE = 1;         // Enable Channel destination done interrupt

    // Slave Channel 1
    /*DCH1INTCLR = 0x00FF00FF;        // Clear existing events, disable all interrupts
    DCH1INTbits.CHERIE = 1;         // Enable Channel address error interrupt
    DCH1INTbits.CHDDIE = 1;         // Enable Channel destination done interrupt*/

    // Master Channel
    IPC10bits.DMA0IP = 4;            // Set Interrupt priority to 4
    IEC1bits.DMA0IE = 1;            // Enable DMA Channel 0 interrupts

    // Slave Channel
    /*IPC10bits.DMA1IP = 4;            // Set Interrupt priority to 4
    IPC10bits.DMA1IS = 2;            // Set Interrupt subpriority to 2
    IEC1bits.DMA1IE = 1;            // Enable DMA Channel 1 interrupts*/

    DMACONbits.ON = 1;              // Enable the DMA controller
    
}