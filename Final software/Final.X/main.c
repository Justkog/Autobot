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

void    pic_init(void)
{
    // initialiser tous les registres qui seront utilises
        // setup des GPIO (General Purpose Input Output)
            // les TRIS definissent la direction du pin
            // les LAT definissent l'etat du pin
        // LED en output
            LATCbits.LATC1 = 1;
            TRISCbits.TRISC1 = 0;   // 0 = output
        // BUT en input
            //TRISDbits.TRISD8 = 1;   // 1 = input
}

int main()
{
    pic_init();

    while (1)
    {
        WDTCONbits.WDTCLR = 1;  // ecrire un 1 dans ce bit force la reinitialisation du watchdog
    }
}

