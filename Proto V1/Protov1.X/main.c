/*
 * File:   main.c
 * Author: Autobots
 *
 * Created on June 17, 2016, 3:23 PM
 */

#include <p32xxxx.h>
#include <stdio.h>
#include <stdlib.h>

void    pic_init(void)
{
    // initialiser tous les registres qui seront utilises
        // setup des GPIO (General Purpose Input Output)
            // les TRIS definissent la direction du pin
            // les LAT definissent l'etat du pin
        // LED en output
            LATFbits.LATF1 = 0;     // 0 = etat du pin
            TRISFbits.TRISF1 = 0;   // 0 = output
        // BUT en input
            TRISDbits.TRISD8 = 1;   // 1 = input
}

int main(int argc, char** argv)
{
    // Initialisation des pins du PIC
        pic_init();
    // setup du PWM (Pulse Width Modulation)
        pwm_init();
    // setup des Timers
        timers_init();
        
    // setup des interrupts
                INTCONSET = _INTCON_MVEC_MASK;
                __builtin_enable_interrupts();

    while (1)
    {
        WDTCONbits.WDTCLR = 1;  // ecrire un 1 dans ce bit force la reinitialisation du watchdog
    }
}