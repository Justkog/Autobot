/* 
 * File:   main.c
 * Author: bocal
 *
 * Created on June 27, 2016, 12:28 AM
 */

#include "autobot.h"

void    pic_init(void)
{
    // initialiser tous les registres qui seront utilises
        // setup des GPIO (General Purpose Input Output)
            // les TRIS definissent la direction du pin
            // les LAT definissent l'etat du pin
    
    // LED en output
    LATCbits.LATC2 = 1;
    ANSELCbits.ANSC2 = 0;   // set pin mode to digital (0 = digital, 1 = analogic)
    TRISCbits.TRISC2 = 0;   // 0 = output
}

void    Stop_Green_Led(void)
{
    LATCbits.LATC2 = 0;
}

void    Start_Green_Led(void)
{
    LATCbits.LATC2 = 1;
}

int main()
{
    pic_init();
    logging_init();
    button_init();
    bumper_init();
    pwm_init();
    motor_timer_init();
    button_timer_init();

    //put_str_ln("Initialising DMA...");
    //init_DMA();

    //put_str_ln("Initialising ADC...");
    init_ADC();

    // setup des interrupts
    INTCONSET = _INTCON_MVEC_MASK;
    __builtin_enable_interrupts();

    if (VERBOSE_PIC_STATUS)
        put_str_ln("Ready.");

    while (1)
    {
        WDTCONbits.WDTCLR = 1;  // ecrire un 1 dans ce bit force la reinitialisation du watchdog
    }
}

