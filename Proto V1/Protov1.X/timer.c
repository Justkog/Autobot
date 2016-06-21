/* 
 * File:   timer.c
 * Author: Autobots
 *
 * Created on June 21, 2016, 1:54 PM
 */

#include "autobot.h"

/* PIC Timer Usage
 *  1 -> unused
 *  2 -> unused
 *  3 -> PWM
 *  4 -> unused
 *  5 -> unused
 */

void    pwm_timer_setup(void)
{
    // PWM Timer Setup
        //The following steps must be performed to configure the timer for 16-bit Synchronous Timer mode.
            // 1. Clear the ON control bit (TxCON<15> = 0) to disable the timer.
                T3CONbits.ON = 0;
            // 2. Clear the TCS control bit (TxCON<1> = 0) to select the internal PBCLK source.
                // /!\ ASK GREGOIRE /!\
            // 3. Select the desired timer input clock prescale.
                T3CONbits.TCKPS = 0b011;    // prescale de 1:8
            // 4. Load/Clear the timer register TMRx.
                TMR3 = 0;
            // 5. Load the period register PRx with the desired 16-bit match value.
                PR3 = 128;
/*            // 6. If interrupts are used:
                // a) Clear the TxIF interrupt flag bit in the IFSx register.
                IFS0bits.T3IF = 0;
                // b) Configure the interrupt priority and subpriority levels in the IPCx register.
                IPC3bits.T3IP = 7;
                IPC3bits.T3IS = 0;
                // c) Set the TxIE interrupt enable bit in the IECx register.
                IEC0bits.T3IE = 1;
*/            // 7. Set the ON control bit (TxCON<15> = 1) to enable the timer.
                T3CONbits.ON = 1;

                OC1CONbits.ON = 1;  //
                OC2CONbits.ON = 1;  //
                OC3CONbits.ON = 1;  //
                OC4CONbits.ON = 1;  //
        // (un seul timer??) pour les moteurs
        // (pour le son??) si controle
}

void    timers_init(void)
{
    pwm_timer_setup();
}