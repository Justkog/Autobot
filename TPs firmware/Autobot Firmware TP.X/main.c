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
    u16 timer;
    
    TRISFbits.TRISF1 = 0;         //set led gpio as output

    TRISDbits.TRISD8 = 1;         //set button gpio as input

    //LATFbits.LATF1 = 1;
    while (1)
    {
        if (PORTDbits.RD8 == 0)
        {
            LATFbits.LATF1 = ~LATFbits.LATF1;
            timer = 15000;
            while (timer)
                timer--;
        }
    }
    return (0);
}