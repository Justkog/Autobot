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
    log_key_val("ADC value 0", ADC1BUF0);
    log_key_val("ADC value 1", ADC1BUF1);
    log_key_val("ADC value 2", ADC1BUF2);
    log_key_val("ADC value 3", ADC1BUF3);
    log_key_val("ADC value 4", ADC1BUF4);
    log_key_val("ADC value 5", ADC1BUF5);
    log_key_val("ADC value 6", ADC1BUF6);
    log_key_val("ADC value 7", ADC1BUF7);
    log_key_val("ADC value 8", ADC1BUF8);

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

    put_str_ln("Initialising ADC...");
    init_ADC();

    init_DMA();

    //starting interrupts
    IEC0bits.INT1IE = 1;

    INTCONSET = _INTCON_MVEC_MASK;
    __builtin_enable_interrupts();

    while (1)
        WDTCONbits.WDTCLR = 1;
    return (0);
}