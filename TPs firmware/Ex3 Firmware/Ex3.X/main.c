/* 
 * File:   main.c
 * Author: bocal
 *
 * Created on March 23, 2016, 7:46 PM
 */

#include <p32xxxx.h>
#include <sys/attribs.h>
#include "types.h"

u16 g_divider = 1;

void __ISR(_TIMER_2_VECTOR, IPL7SRS) timerHANDLER(void)
{
    if (LATFbits.LATF1 == 1)
        LATFbits.LATF1 = 0;
    else
        LATFbits.LATF1 = 1;
    IFS0bits.T2IF = 0;
}

void __ISR(_EXTERNAL_1_VECTOR, IPL4Soft) buttonHANDLER(void)
{
    IFS0bits.INT1IF = 0;
    g_divider = g_divider * 2;
    if (g_divider > 16)
        g_divider = 1;
    PR2 = 39062 / g_divider;
    u32 timer = 0;
    //small timer to leave after button push noise
    while (timer < 60000)
        timer++;
}

/*
 * 
 */

int main(void)
{
    //set high on LED pin as output value
    LATFbits.LATF1 = 1;

    //set LED pin as output
    TRISFbits.TRISF1 = 0;

    //set BUT pin as input
    //TRISDbits.TRISD8 = 1;

    T2CON = 0x0;
    TMR2 = 0x0;
    PR2 = 39062;                    // 8Mhz : 8 (peripheral clock divisor) : 16 (prescale)
                                    // = 62 500 timer ticks per sec
    
    T2CONbits.TCKPS = 0b111;        // 1:16 prescale value

    //setup INT1 to interrupt on falling edge
    INTCONbits.INT1EP = 1;

    //clear interrupt
    IFS0bits.T2IF = 0;
    IFS0bits.INT1IF = 0;

    //set priority
    IPC2bits.T2IP = 7;
    IPC1bits.INT1IP = 4;

    //starting interrupts  
    IEC0bits.T2IE = 1;
    IEC0bits.INT1IE = 1;

    INTCONSET = _INTCON_MVEC_MASK;
    __builtin_enable_interrupts();
    
    //start timer
    T2CONbits.ON = 1;

    while (1)
    {

        WDTCONbits.WDTCLR = 1;
    }
    return (0);
}
