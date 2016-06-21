/* 
 * File:   pwm.c
 * Author: Autobots
 *
 * Created on June 21, 2016, 1:48 PM
 */

#include "autobot.h"

void    pwm_init(void)
{
    // The following steps should be taken when configuring the Output Compare module for PWM operation:
            // 1. Set the PWM period by writing to the selected timer period register (PRy).
                // OCTSEL = 1 -> Timer 3
                OC1CONbits.OCTSEL = 1;  // L298 - Input 1
                OC2CONbits.OCTSEL = 1;  // L298 - Input 2
                OC3CONbits.OCTSEL = 1;  // L298 - Input 3
                OC4CONbits.OCTSEL = 1;  // L298 - Input 4
            // 2. Set the PWM duty cycle by writing to the OCxRS register.
                OC1RS = 64;
                OC2RS = 64;
                OC3RS = 64;
                OC4RS = 64;
            // 3. Write the OCxR register with the initial duty cycle.
                OC1R = 0;
                OC2R = 0;
                OC3R = 0;
                OC4R = 0;
            // 4. Enable interrupts, if required, for the timer and Output Compare modules. The output compare interrupt is required for PWM Fault pin utilization.
                // /!\ NOT REQUIRED /!\
            // 5. Configure the Output Compare module for one of two PWM Operation modes by writing to the Output Compare mode bits, OCM<2:0> (OCxCON<2:0>).
                OC1CONbits.OCM = 6; // 6 = 110 -> PWM mode on OCx; Fault pin disabled
                OC2CONbits.OCM = 6; // 6 = 110 -> PWM mode on OCx; Fault pin disabled
                OC3CONbits.OCM = 6; // 6 = 110 -> PWM mode on OCx; Fault pin disabled
                OC4CONbits.OCM = 6; // 6 = 110 -> PWM mode on OCx; Fault pin disabled
            // 6. Set the TMRy prescale value and enable the time base by setting TON (TxCON<15>) = 1
                // cf timer setup.7
}