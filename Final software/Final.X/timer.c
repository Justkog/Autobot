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

u32 motor_delay_ms = 0;

void    pwm_timer_init(void)
{
    // PWM Timer Setup
        //The following steps must be performed to configure the timer for 16-bit Synchronous Timer mode.
            // 1. Clear the ON control bit (TxCON<15> = 0) to disable the timer.
                T3CONbits.ON = 0;
            // 2. Clear the TCS control bit (TxCON<1> = 0) to select the internal PBCLK source.
                // /!\ ASK GREGOIRE /!\
            // 3. Select the desired timer input clock prescale.
                T3CONbits.TCKPS = 0b111;    // prescale de 1:8
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

                
        // (un seul timer??) pour les moteurs
        // (pour le son??) si controle
}

void    motor_timer_init(void)
{
    T4CONbits.ON = 0;
    T4CONbits.TCKPS = 0b111;                // presale 1:256
    TMR4 = 0;

    //disable interrupt
    IEC0bits.T4IE = 0;

    //clear interrupt
    IFS0bits.T4IF = 0;

    //set priority
    IPC4bits.T4IP = 3;

    //PR4 = 45000;                                     // 48 000 000 / 4 / 256 = 46 875 ticks for one second
}

void __ISR(_TIMER_4_VECTOR, IPL3SOFT) motorTimerHANDLER(void)
{
    log_key_val("motor delay i", motor_delay_ms);
    if (motor_delay_ms == 0)
    {
        put_str_ln("restore");
        Motor_Restore();
        // Clear the TxIE interrupt enable bit in the IECx register.
        IEC0bits.T4IE = 0;
        // Clear the ON control bit (TxCON<15> = 1) to disable the timer.
        //T4CONbits.ON = 0;
    }
    else
        Start_Motor_Timer(motor_delay_ms);

    IFS0bits.T4IF = 0;
}

void Start_Motor_Timer(u32 delay)
{
    motor_delay_ms = delay;
    if (motor_delay_ms > 1000)
    {
        PR4 = 46875;
        motor_delay_ms = motor_delay_ms - 1000;
    }
    else
    {
        motor_delay_ms = motor_delay_ms * 46875 / 1000;
        PR4 = motor_delay_ms;
        motor_delay_ms = 0;
    }

    // Set the TxIE interrupt enable bit in the IECx register.
    IEC0bits.T4IE = 1;

    // Set the ON control bit (TxCON<15> = 1) to enable the timer.
    T4CONbits.ON = 1;
}