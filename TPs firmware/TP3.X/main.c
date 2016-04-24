/* 
 * File:   main.c
 * Author: Autobots
 *
 * Created on March 22, 2016, 3:42 PM
 */

#include <p32xxxx.h>
#include "types.h"
#include <sys/attribs.h>

void __ISR(_TIMER_2_VECTOR, IPL7AUTO) CoreTimerHandler(void)
{
    LATFbits.LATF1 = ~LATFbits.LATF1;
    TMR2 = 0;
    IFS0bits.T2IF = 0;
}

int main(void)
{
    T2CON = 0x0;                    //reset timer parameter
    TMR2 = 0x0;                     //set start value at 0
    PR2 = 62500;                    //set max value at 0xffff
    T2CONbits.TCKPS = 0b100;        //set prescal at 1:16

    u16 timer;
   
    TRISFbits.TRISF1 = 0;           //set led gpio as output
    TRISDbits.TRISD8 = 1;           //set button gpio as input

    IPC2bits.T2IP = 7;              //priority level
    IFS0bits.T2IF = 0;              //reset flag
    IEC0bits.T2IE = 1;              //enable interupt

    INTCONSET = _INTCON_MVEC_MASK;  //set multi vector mode
    __builtin_enable_interrupts();  //set CP0 status IE to turn on interrupts

    //T2CONSET = 0x8000;              //start
    T2CONbits.TON = 1;

    //LATFbits.LATF1 = 1;
    while (1)
    {
       
        if (PORTDbits.RD8 == 0)
        {
            timer = 50000;
            while (timer)
                timer--;
        }
    }
    return (0);
}