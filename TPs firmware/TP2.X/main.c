/* 
 * File:   main.c
 * Author: Autobots
 *
 * Created on March 22, 2016, 3:42 PM
 */

#include <p32xxxx.h>
#include "types.h"

int main(void)
{
    T2CON = 0x0;                    //reset timer parameter
    TMR2 = 0x0;                     //set start value at 0
    PR2 = 0xFFFF;                   //set max value at 0xffff
    T2CONbits.TCKPS = 0b100;        //set prescal at 1:16
    T2CONSET = 0x8000;              //start

    u16 timer;
    int j;

    j = 1;
    TRISFbits.TRISF1 = 0;         //set led gpio as output

    TRISDbits.TRISD8 = 1;         //set button gpio as input

    //LATFbits.LATF1 = 1;
    while (1)
    {
        if (TMR2 > 62500 / j)
        {
            LATFbits.LATF1 = ~LATFbits.LATF1;
            TMR2 = 0;
        }
        if (PORTDbits.RD8 == 0)
        {
            j *= 2;
            if ( j > 16 )
                j = 1;
            timer = 50000;
            while (timer)
                timer--;
        }
    }
    return (0);
}