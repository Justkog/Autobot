/* 
 * File:   main.c
 * Author: bocal
 *
 * Created on March 23, 2016, 7:46 PM
 */

#include <p32xxxx.h>
#include <sys/attribs.h>
#include "types.h"

void __ISR(_EXTERNAL_1_VECTOR, IPL4SOFT) buttonHANDLER(void)
{
    //no need to turn it on as it is chained
    //DCH1CONbits.CHEN = 1;                       // Turn channel ON, initiate a transfer
    DCH1ECONbits.SIRQEN = 1;                    // Start channel cell transfer if an interrupt matching CHAIRQ occurs
    
    DCH0CONbits.CHEN = 1;                       // Turn channel ON, initiate a transfer
    //DCH0ECONbits.CFORCE = 1;                    // A DMA transfer is forced to begin
    DCH0ECONbits.SIRQEN = 1;                    // Start channel cell transfer if an interrupt matching CHAIRQ occurs

    //counter = 0;
    
    LATFbits.LATF1 = 1 ^ LATFbits.LATF1;
    IFS0bits.INT1IF = 0;
}

int main(void)
{
    //set high on LED pin as output value
    LATFbits.LATF1 = 0;

    //set LED pin as output
    TRISFbits.TRISF1 = 0;

    //mic alim setup on RE1
    //TRISEbits.TRISE2 = 0;
    //LATEbits.LATE2 = 1;

    init_button();

    //UART logging initialisation
    init_logging();

    put_str_ln("Initialising DMA...");
    init_DMA();

    put_str_ln("Initialising ADC...");
    init_ADC();

    //starting interrupts
    IEC0bits.INT1IE = 1;

    INTCONSET = _INTCON_MVEC_MASK;
    __builtin_enable_interrupts();

    while (1)
        WDTCONbits.WDTCLR = 1;
    return (0);
}