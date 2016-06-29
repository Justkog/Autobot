/* 
 * File:   main.c
 * Author: bocal
 *
 * Created on June 27, 2016, 12:28 AM
 */

#include "autobot.h"

/*
 * 
 */

void    init_pic(void)
{
    // initialiser tous les registres qui seront utilises
        // setup des GPIO (General Purpose Input Output)
            // les TRIS definissent la direction du pin
            // les LAT definissent l'etat du pin
        // LED en output
            LATCbits.LATC1 = 1;
            ANSELCbits.ANSC1 = 0;
            TRISCbits.TRISC1 = 0;   // 0 = output
}

int main()
{
    init_pic();
    init_logging();
    init_button();

    //put_str_ln("Initialising DMA...");
    init_DMA();

    //put_str_ln("Initialising ADC...");
    init_ADC();

    INTCONSET = _INTCON_MVEC_MASK;
    __builtin_enable_interrupts();

    while (1)
    {
        WDTCONbits.WDTCLR = 1;  // ecrire un 1 dans ce bit force la reinitialisation du watchdog
    }
}

