/*
 * File:   motor.c
 * Author: Autobots
 *
 * Created on June 21, 2016, 1:48 PM
 */

#include "autobot.h"

//void __ISR(_EXTERNAL_2_VECTOR, IPL ## PRIORITY_MOTOR ## SOFT) bumperHANDLER(void)
void __ISR(_EXTERNAL_2_VECTOR, IPL7SOFT) bumperHANDLER(void)
{
    Motor_Control_Emergency_Stop();
    // Relancer une phase d'ecoute ??? (Pas dans le cahier des charges)
    IFS0bits.INT2IF = 0;
}


//void __ISR(_EXTERNAL_1_VECTOR, IPL ## PRIORITY_MOTOR ## SOFT) buttonHANDLER(void)
//void __ISR(_EXTERNAL_1_VECTOR, ISR_IPL(PRIORITY_MOTOR())) buttonHANDLER(void)
void __ISR(_EXTERNAL_1_VECTOR, IPL7SOFT) buttonHANDLER(void)
{
    //no need to turn it on as it is chained
    //DCH1CONbits.CHEN = 1;                       // Turn channel ON, initiate a transfer
    //DCH1ECONbits.SIRQEN = 1;                    // Start channel cell transfer if an interrupt matching CHAIRQ occurs

    //DCH0CONbits.CHEN = 1;                       // Turn channel ON, initiate a transfer
    //DCH0ECONbits.CFORCE = 1;                    // A DMA transfer is forced to begin
    //DCH0ECONbits.SIRQEN = 1;                    // Start channel cell transfer if an interrupt matching CHAIRQ occurs

    //counter = 0;

    //LATCbits.LATC1 = 1 ^ LATCbits.LATC1;          // Toggle LED

    put_str_ln(CAT(ISR_IPL(PRIORITY_MOTOR()), "toto"));

    //put_str_ln("dumping ADC");
    //Print_average();
    //DCH0CONbits.CHEN = 1;                       // Turn channel ON, initiate a transfer
    //DCH0ECONbits.CFORCE = 1;                    // A DMA transfer is forced to begin
    //DCH0ECONbits.SIRQEN = 1;                    // Start channel cell transfer if an interrupt matching CHAIRQ occurs*/

    //IEC0bits.AD1IE = 1;

    //u32 waiter = 500000;
    //while (waiter)
    //    waiter--;


    //fuck_the_motor();
    IFS0bits.INT1IF = 0;
}

void    bumper_init()
{
    IEC0bits.INT2IE = 0;                        // stop the interrupt
    TRISCbits.TRISC6 = 1;                       // set as input
    INT2Rbits.INT2R = 0b0101;                   // Configue RPC6 as INT2 trigger
    INTCONbits.INT2EP = 1;                      // Setup INT2 to interrupt on falling edge (due to pull-up)
    IFS0bits.INT2IF = 0;                        // Clear Interrupt Flag
    IPC2bits.INT2IP = PRIORITY_MOTOR();           // Set Interrupt Priority to MAX level
    IPC2bits.INT2IS = SUBPRIORITY_EMERGENCY;    // Set Interrupt subpriority to MAX level
    IEC0bits.INT2IE = 1;                        // Start BUMPER Interrupt
}

void    button_init()
{
    IEC0bits.INT1IE = 0;                        // stop the interrupt
    TRISBbits.TRISB9 = 1;                       // set BUT pin as input (1 = input)
    INT1Rbits.INT1R = 0b0100;                   // Configure RPB9 as INT1 trigger (Input Pin Selection)
    INTCONbits.INT1EP = 1;                      // setup INT1 to interrupt on falling edge
    IFS0bits.INT1IF = 0;                        // clear interrupt flag
    IPC1bits.INT1IP = PRIORITY_MOTOR();           // set priority
    IEC0bits.INT1IE = 1;                        // starting BUTTON interrupt
}