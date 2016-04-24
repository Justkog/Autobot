/* 
 * File:   main.c
 * Author: Autobots
 *
 * Created on March 29, 2016, 1:45 PM
 */

#include <p32xxxx.h>
#include <sys/attribs.h>
#include "types.h"

void __ISR(_INT_VECTOR_NUMBER, IPLx[SRS|SOFT|AUTO]) isrName(void)
{
    /* Clear the cause of the interrupt (if required) */

    /* Clear the interrupt flag */

    /* ISR-specific processing */
    
}

int main(void)
{
    while (1)
    {
        WDTCONbits.WDTCLR = 1;
    }
    return (0);
}

