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
    LATF |= 0x2 ;
    TRISF |= 0x2;
    while (1)
    {
        LATF |= 0x2 ;
    }
    return (0);
}