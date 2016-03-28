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
u16 g_timer3_ended = 1;
u16 g_pwm = 0;
u8  g_pwm_mode = 0;
u8  g_pwm_up = 1;

void __ISR(_TIMER_2_VECTOR, IPL7SRS) timerHANDLER(void)
{
    if (!g_pwm_mode)
    {
        if (LATFbits.LATF1 == 1)
            LATFbits.LATF1 = 0;
        else
            LATFbits.LATF1 = 1;
    }
    else
    {
        if (g_pwm_up)
        {
            if (g_pwm < 50)
                g_pwm++;
            else
                g_pwm_up = 0;
        }
        else
        {
            if (g_pwm > 1)
                g_pwm--;
            else
                g_pwm_up = 1;
        }
    }
    IFS0bits.T2IF = 0;              // Clearing Timer2 interrupt flag
}

void __ISR(_TIMER_4_VECTOR, IPL7SRS) timer4HANDLER(void)
{
    if (LATFbits.LATF1 == 1)
    {
        TMR4 = g_pwm * 61;
        LATFbits.LATF1 = 0;
    }
    else
    {
        TMR4 = PR4 - g_pwm * 62;
        LATFbits.LATF1 = 1;
    }

    IFS0bits.T4IF = 0;              // Clearing Timer4 interrupt flag
}

void __ISR(_TIMER_3_VECTOR, IPL7SRS) timer3HANDLER(void)
{
    IEC0bits.T3IE = 0;              // desactivate interrupt
    T3CON = 0x0;                    // clear timer
    TMR3 = 0x0;                     // reset timer start
    g_timer3_ended = 1;             // set timer 3 as ended

    if (PORTDbits.RD8 == 0)         // if the button is indeed still pushed
    {
        //start the software pwm shit with timer2 as reference (timer 2 will be executed 50 times in 5 sec

        IEC0bits.T2IE = 0;
        IFS0bits.T2IF = 0;
        T2CON = 0x0;
        TMR2 = 0x0;
        PR2 = 3125;                    // 8Mhz : 8 (peripheral clock divisor) : 32 (prescale) * 5 : 50
                                        // = 10 timer ticks per 5 sec
        T2CONbits.TCKPS = 0x5;        // 1:32 prescale value

        IFS0bits.T4IF = 0;
        T4CON = 0x0;
        TMR4 = 0x0;
        PR4 = 3101;                    // 8Mhz : 8 (peripheral clock divisor) : 1 (prescale)
        T4CONbits.TCKPS = 0x1;        // 1:2 prescale value
        IPC4bits.T4IP = 7;              // set priority

        IEC0bits.T4IE = 1;              // enable timer 4 interrupts
        IEC0bits.T2IE = 1;              // enable timer 2 interrupts

        T2CONbits.ON = 1;               // start timer
        T4CONbits.ON = 1;               // start timer

        g_pwm_mode = 1;
    }

    IFS0bits.T3IF = 0;              // reset the interrupt flag
}

void __ISR(_EXTERNAL_1_VECTOR, IPL4SOFT) buttonHANDLER(void)
{
    //check on timer 3 to make sure that we are not in push noise
    //if it is ON, it means that the button was just pushed
    if ((g_timer3_ended == 1) && INTCONbits.INT1EP == 0)
    {              
        g_divider = g_divider * 2;
        if (g_divider > 16)
            g_divider = 1;
        PR2 = 62500 / g_divider;
        
        //set rising edge external interrupt
        INTCONbits.INT1EP = 1;
        
        IFS0bits.T3IF = 0;              // clear interrupt
        g_timer3_ended = 0;             
        T3CON = 0x0;                    // clear timer
        TMR3 = 0x0;                     // set timer start
        T3CONbits.TCKPS = 0x5;          // 1:32 prescale value
        PR3 = 62500;                    // 2s (8 : 8 : 32 * 0.1)
        IPC3bits.T3IP = 7;              // set priority
        IEC0bits.T3IE = 1;              // start interrupt
        T3CONbits.ON = 1;               // start timer

        g_pwm_mode = 0;
    }
    else if (INTCONbits.INT1EP == 1)
    {
        IEC0bits.T3IE = 0;              // desactivate interrupt
        T3CON = 0x0;                    // clear timer
        TMR3 = 0x0;                     // reset timer start
        g_timer3_ended = 1;             // set timer 3 as already ended
        
        INTCONbits.INT1EP = 0;          // set falling edge external interrupt
    }

    IFS0bits.INT1IF = 0;
}

/*
 * 
 */

int main(void)
{
    //set high on LED pin as output value
    //LATFbits.LATF1 = 1;

    //set LED pin as output
    TRISFbits.TRISF1 = 0;

    //set BUT pin as input
    TRISDbits.TRISD8 = 1;

    T2CON = 0x0;
    TMR2 = 0x0;
    PR2 = 62500;                    // 8Mhz : 8 (peripheral clock divisor) : 16 (prescale)
                                    // = 62 500 timer ticks per sec
    
    T2CONbits.TCKPS = 0b100;        // 1:16 prescale value

    //setup INT1 to interrupt on falling edge
    INTCONbits.INT1EP = 0;

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

    //set timer 3 as already ended
    g_timer3_ended = 1;
    
    while (1)
    {
        WDTCONbits.WDTCLR = 1;
    }
    return (0);
}
