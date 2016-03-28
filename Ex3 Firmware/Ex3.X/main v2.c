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
u32 g_pwm = 0;
u8 g_mode = 0;

void __ISR(_TIMER_2_VECTOR, IPL7SRS) timerHANDLER(void)
{
    if (OC1CONbits.ON == 0)
    {
        if (LATFbits.LATF1 == 1)
            LATFbits.LATF1 = 0;
        else
            LATFbits.LATF1 = 1;
    }
    else
    {
        if (Mode)
        {
            if ( g_pwm < 1570 )     // Ramp up mode        (1 cycle * 5000ms : 3.184) = 1570 cycles for 5 sec
            {
                g_pwm ++;           // If the duty cycle is not at max, increase
                OC1RS = g_pwm;      // Write new duty cycle
            }
            else
            {
                Mode = 0;           // g_pwm is at max, change mode to ramp down
            }
        }                           // End of ramp up
        else
        {
            if (!g_pwm)             // Ramp Down mode
            {
                g_pwm --;           // If the duty cycle is not at min, increase
                OC1RS = g_pwm;      // Write new duty cycle
            }
            else
            {
                Mode = 1;           // g_pwm is at min, change mode to ramp up
            }
        }                           // End of ramp down
    }
    IFS0bits.T2IF = 0;              // Clearing Timer2 interrupt flag
}

void __ISR(_TIMER_3_VECTOR, IPL6AUTO) timerHANDLER(void)
{
    IEC0bits.T3IE = 0;              // desactivate interrupt
    T3CON = 0x0;                    // clear timer
    TMR3 = 0x0;                     // reset timer start
    g_timer3_ended = 1;             // set timer 3 as ended
    IFS0bits.T3IF = 0;              // reset the interrupt flag
        
    if (PORTDbits.RD8 == 0)         // if the button is indeed still pushed
    {
        //start the g_pwm shit with timer2 as reference
        OC1CON = 0x0000;            // Turn off the OC1 when performing the setup
        OC1R = 0x0064;              // Initialize primary Compare register
        OC1RS = 0x0064;             // Initialize secondary Compare register
        OC1CON = 0x0006;            // Configure for g_pwm mode without Fault pin enabled
        
        PR2 = 0x00C7; //199         // Set period ?? ms (8 : 8 : )    3.184 ms if 16 prescale (1000ms * 199 : 62500)
        IFS0CLR = 0x00000100;       // Clear the T2 interrupt flag
        IEC0SET = 0x00000100;       // Enable T2 interrupt
        IPC2SET = 0x0000001C;       // Set T2 interrupt priority to 7
        
        T2CONSET = 0x8000;          // Enable Timer2
        OC1CONSET = 0x8000;         // Enable OC1
    }
}

void __ISR(_EXTERNAL_1_VECTOR, IPL4Soft) buttonHANDLER(void)
{
    IFS0bits.INT1IF = 0;
    
    //check on timer 3 to make sure that we are not in push noise
    //if it is ON, it means that the button was just pushed
    if ((g_timer3_ended == 1 || TMR3 > 6250 / 2) && INTCONbits.INT1EP == 1)
    {       
        OC1CON = 0x0000;            // Turn off the OC1 in case it was running
        
        g_divider = g_divider * 2;
        if (g_divider > 16)
            g_divider = 1;
        PR2 = 62500 / g_divider;
        
        //set rising edge external interrupt
        INTCONbits.INT1EP = 0;
        
        IFS0bits.T3IF = 0;              // clear interrupt
        g_timer3_ended = 0;             
        T3CON = 0x0;                    // clear timer
        TMR3 = 0x0;                     // set timer start
        T3CONbits.TCKPS = 0b100;        // 1:32 prescale value
        PR3 = 62500;                    // 2s (8 : 8 : 32 * 0.1)
        IPC2bits.T3IP = 4;              // set priority
        IEC0bits.T3IE = 1;              // start interrupt
        T3CONbits.ON = 1;               // start timer
    }
    else if (INTCONbits.INT1EP == 0 && TMR3 > 6250 / 2)
    {
        IEC0bits.T3IE = 0;              // desactivate interrupt
        T3CON = 0x0;                    // clear timer
        TMR3 = 0x0;                     // reset timer start
        g_timer3_ended = 1;             // set timer 3 as already ended
        
        INTCONbits.INT1EP = 1;          // set falling edge external interrupt
    }
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
    TRISDbits.TRISD8 = 1;

    T2CON = 0x0;
    TMR2 = 0x0;
    PR2 = 62500;                    // 8Mhz : 8 (peripheral clock divisor) : 16 (prescale)
                                    // = 62 500 timer ticks per sec
    
    T2CONbits.TCKPS = 0b100;        // 1:16 prescale value

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

    //set timer 3 as already ended
    g_timer3_ended = 1;
    while (1)
    {
        WDTCONbits.WDTCLR = 1;
    }
    return (0);
}
