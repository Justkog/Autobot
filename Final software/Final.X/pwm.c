/* 
 * File:   pwm.c
 * Author: Autobots
 *
 * Created on June 21, 2016, 1:48 PM
 */

#include "autobot.h"

void    pwm_init(void)
{
    // Pin mapping
        // M1 = RPB13 && RPB14
    ANSELBbits.ANSB13 = 0;          // set pin mode to digital (0 = digital, 1 = analogic)
    ANSELBbits.ANSB14 = 0;              // because pins are elligible to AN* configs
    TRISBbits.TRISB13 = 0;          // set pin as output
    TRISBbits.TRISB14 = 0;          // set pin as output
    RPB13Rbits.RPB13R = 0b0101;     // map OC4 output on RPB13
    RPB14Rbits.RPB14R = 0b0101;     // map OC3 output on RPB14
        // M2 = RPB7 && RPB8
    TRISBbits.TRISB7 = 0;           // set pin as output
    TRISBbits.TRISB8 = 0;           // set pin as output
    RPB7Rbits.RPB7R = 0b0101;       // map OC1 output as RPB7
    RPB8Rbits.RPB8R = 0b0101;       // map OC2 output as RPB8

    // The following steps should be taken when configuring the Output Compare module for PWM operation:
            // 1. Set the PWM period by writing to the selected timer period register (PRy).
                // OCTSEL = 1 -> Timer 3
    OC3CONbits.OCTSEL = 1;  // L298 - Input 1
    OC4CONbits.OCTSEL = 1;  // L298 - Input 2
    OC2CONbits.OCTSEL = 1;  // L298 - Input 3
    OC1CONbits.OCTSEL = 1;  // L298 - Input 4
            // 2. Set the PWM duty cycle by writing to the OCxRS register.
    OC3RS = MOTOR_PWM_PERIOD;
    OC4RS = MOTOR_PWM_PERIOD;
    OC2RS = MOTOR_PWM_PERIOD;
    OC1RS = MOTOR_PWM_PERIOD;
            // 3. Write the OCxR register with the initial duty cycle.
    OC3R = 0;
    OC4R = 0;
    OC2R = 0;
    OC1R = 0;
            // 4. Enable interrupts, if required, for the timer and Output Compare modules. The output compare interrupt is required for PWM Fault pin utilization.
                // /!\ NOT REQUIRED /!\
            // 5. Configure the Output Compare module for one of two PWM Operation modes by writing to the Output Compare mode bits, OCM<2:0> (OCxCON<2:0>).
    OC3CONbits.OCM = 6; // 6 = 110 -> PWM mode on OCx; Fault pin disabled
    OC4CONbits.OCM = 6; // 6 = 110 -> PWM mode on OCx; Fault pin disabled
    OC2CONbits.OCM = 6; // 6 = 110 -> PWM mode on OCx; Fault pin disabled
    OC1CONbits.OCM = 6; // 6 = 110 -> PWM mode on OCx; Fault pin disabled
            // 6. Set the TMRy prescale value and enable the time base by setting TON (TxCON<15>) = 1
    pwm_timer_init();
            // Switch Motors ON
    OC3CONbits.ON = 1;  //
    OC4CONbits.ON = 1;  //
    OC2CONbits.ON = 1;  //
    OC1CONbits.ON = 1;  //
    
    LATAbits.LATA10 = 1;        // set ENABLE A to 1
    LATBbits.LATB5 = 1;         // set ENABLE B to 1
    TRISAbits.TRISA10 = 0;      // set ENABLE A as output
    TRISBbits.TRISB5 = 0;       // set ENABLE B as output
}