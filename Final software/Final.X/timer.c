/* 
 * File:   timer.c
 * Author: Autobots
 *
 * Created on June 21, 2016, 1:54 PM
 */

#include "autobot.h"

/* PIC Timer Usage
 *  1 -> ? New sound record callback if keep rolling mode ?
 *  2 -> ? Timed sound based configuration ? 
 *  3 -> PWM
 *  4 -> Motor action callback
 *  5 -> ? Button timed push ?
 */

u32 motor_delay_ms = 0;

// PWM timer (3)

void    pwm_timer_init(void)
{
    // PWM Timer Setup
        //The following steps must be performed to configure the timer for 16-bit Synchronous Timer mode.
            // 1. Clear the ON control bit (TxCON<15> = 0) to disable the timer.
                T3CONbits.ON = 0;
            // 2. Clear the TCS control bit (TxCON<1> = 0) to select the internal PBCLK source.
                // /!\ ASK GREGOIRE /!\
            // 3. Select the desired timer input clock prescale.
                T3CONbits.TCKPS = 0b111;    // prescale de 1:256
            // 4. Load/Clear the timer register TMRx.
                TMR3 = 0;
            // 5. Load the period register PRx with the desired 16-bit match value.
                PR3 = MOTOR_PWM_PERIOD;
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

// Button timer (5)

u8      button_seconds_since_push = 0;

u8      Get_Button_Seconds_Since_Push(void)
{
    return (button_seconds_since_push);
}

void    button_timer_init(void)
{
    //disable interrupt
    IEC0bits.T5IE = 0;

    T5CONbits.ON = 0;
    T5CONbits.TCKPS = 0b111;                // prescale 1:256

    //clear interrupt
    IFS0bits.T5IF = 0;

    //set priority
    IPC5bits.T5IP = PRIORITY_MOTOR;

    PR5 = 46875;                            // Ticks for 500 ms
}

void    Start_Button_Timer(void)
{
    TMR5 = 0;
    
    // Set the TxIE interrupt enable bit in the IECx register.
    IEC0bits.T5IE = 1;

    // Set the ON control bit (TxCON<15> = 1) to enable the timer.
    T5CONbits.ON = 1;
}

void    Stop_Button_Timer(void)
{
    TMR5 = 0;
    
    // Set the TxIE interrupt enable bit in the IECx register.
    IEC0bits.T5IE = 0;

    // Set the ON control bit (TxCON<15> = 1) to enable the timer.
    T5CONbits.ON = 0;
    
    button_seconds_since_push = 0;
}

void __ISR(_TIMER_5_VECTOR, IPL_ISR(PRIORITY_MOTOR)) buttonTimerHANDLER(void)
{
    button_seconds_since_push++;
    
    if (button_seconds_since_push > 10)
    {
        Stop_Button_Timer();
    }
    else
    {
        Start_Button_Timer();
    }

    IFS0bits.T5IF = 0;
}

// Sound config timer (2)

u8      half_seconds_since_sound = 0;

void    sound_config_timer_init(void)
{
    //disable interrupt
    IEC0bits.T2IE = 0;

    T2CONbits.ON = 0;
    T2CONbits.TCKPS = 0b111;                // prescale 1:256

    //clear interrupt
    IFS0bits.T2IF = 0;

    //set priority
    IPC2bits.T2IP = PRIORITY_MOTOR;

    PR2 = 46875 / 2;                            // Ticks for 500 ms
}

void    Start_Sound_Timer(void)
{
    TMR2 = 0;
    
    // Set the TxIE interrupt enable bit in the IECx register.
    IEC0bits.T2IE = 1;

    // Set the ON control bit (TxCON<15> = 1) to enable the timer.
    T2CONbits.ON = 1;
}

void    Stop_Sound_Timer(void)
{
    TMR2 = 0;
    
    // Set the TxIE interrupt enable bit in the IECx register.
    IEC0bits.T2IE = 0;

    // Set the ON control bit (TxCON<15> = 1) to enable the timer.
    T2CONbits.ON = 0;
    
    half_seconds_since_sound = 0;
}

void __ISR(_TIMER_2_VECTOR, IPL_ISR(PRIORITY_MOTOR)) soundTimerHANDLER(void)
{
    half_seconds_since_sound++;
    
    if (half_seconds_since_sound > 10)
    {
        Stop_Sound_Timer();
    }
    else
    {
        Start_Sound_Timer();
    }

    IFS0bits.T2IF = 0;
}

// Motor timer (4)

void    motor_timer_init(void)
{
    //disable interrupt
    IEC0bits.T4IE = 0;

    T4CONbits.ON = 0;
    T4CONbits.TCKPS = 0b111;                // prescale 1:256

    //clear interrupt
    IFS0bits.T4IF = 0;

    //set priority
    IPC4bits.T4IP = PRIORITY_MOTOR;

    //PR4 = 45000;                                     // 48 000 000 / 4 / 256 = 46 875 ticks for one second
}

void    motor_timer_start(void)
{
    // Set the TxIE interrupt enable bit in the IECx register.
    IEC0bits.T4IE = 1;

    // Set the ON control bit (TxCON<15> = 1) to enable the timer.
    T4CONbits.ON = 1;
}

void __ISR(_TIMER_4_VECTOR, IPL_ISR(PRIORITY_MOTOR)) motorTimerHANDLER(void)
{
    if (motor_delay_ms == 0)
    {
        motor_timer_init();
        Motor_Restore();
    }
    else
        Start_Motor_Timer(motor_delay_ms);

    IFS0bits.T4IF = 0;
}

void Start_Motor_Timer(u32 delay)
{
    TMR4 = 0;
    motor_delay_ms = delay;
    if (motor_delay_ms > 1000)
    {
        PR4 = 46875;                            // Ticks for one second
        motor_delay_ms = motor_delay_ms - 1000;
    }
    else
    {
        motor_delay_ms = motor_delay_ms * 46875 / 1000;
        PR4 = motor_delay_ms;
        motor_delay_ms = 0;
    }
    motor_timer_start();
}