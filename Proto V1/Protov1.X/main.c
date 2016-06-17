/* 
 * File:   main.c
 * Author: Autobots
 *
 * Created on June 17, 2016, 3:23 PM
 */

#include <p32xxxx.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
// initialiser tous les registres qui seront utilises
    // setup des GPIO (General Purpose Input Output)
            // les TRIS definissent la direction du pin
            // les LAT definissent l'etat du pin
        // LED en output
            LATFbits.LATF1 = 0;     // 0 = etat du pin
            TRISFbits.TRISF1 = 0;   // 0 = output
        // BUT en input
            TRISDbits.TRISD8 = 1;   // 1 = input
    // setup du PWM (Pulse Width Modulation)
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
    // setup des timers
        //The following steps must be performed to configure the timer for 16-bit Synchronous Timer mode.
            // 1. Clear the ON control bit (TxCON<15> = 0) to disable the timer.
                T3CONbits.ON = 0;
            // 2. Clear the TCS control bit (TxCON<1> = 0) to select the internal PBCLK source.
                // /!\ ASK GREGOIRE /!\
            // 3. Select the desired timer input clock prescale.
                T3CONbits.TCKPS = 1;    // prescale de 1:8
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
    // setup des interrupts
                INTCONSET = _INTCON_MVEC_MASK;
                __builtin_enable_interrupts();

    while (1)
    {
        WDTCONbits.WDTCLR = 1;  // ecrire un 1 dans ce bit force la reinitialisation du watchdog
    }
}