/* 
 * File:   main.c
 * Author: bocal
 *
 * Created on March 23, 2016, 5:20 PM
 */

#include <p32xxxx.h>
#include "types.h"

/*
 * 
 */
int main(void)
{
    u16 divider = 1;

    //set high on LED pin as output value
    LATFbits.LATF1 = 1;

    //set LED pin as output
    TRISFbits.TRISF1 = 0;

    //set BUT pin as input
    TRISDbits.TRISD8 = 1;

    T2CON = 0x0;
    TMR2 = 0x0;
    PR2 = 0xFFFF;

    T2CONbits.TCKPS = 0b100;        // 1:16 prescale value

    T2CONbits.ON = 1;
    while (1)
    {
        if (TMR2 > 62500 / divider)           // 8Mhz : 8 (peripheral clock divisor) : 16 (prescale)
        {                                     // = 62 500 timer ticks per sec
            TMR2 = 0x0;
            if (LATFbits.LATF1 == 1)
                LATFbits.LATF1 = 0;
            else
                LATFbits.LATF1 = 1;
        }

        //BUT pin is set as a pull up,
        //pushing the button changes the state to 0
        if (PORTDbits.RD8 == 0)
        {
            divider = divider * 2;
            if (divider > 16)
                divider = 1;
            u32 timer = 0;
            //small timer to leave after button push noise
            while (timer < 60000)
                timer++;
        }
    }
    return (0);
}

