/* 
 * File:   main.c
 * Author: bocal
 *
 * Created on March 23, 2016, 7:46 PM
 */

#include <p32xxxx.h>
#include <sys/attribs.h>
#include "types.h"

s8 motor_speed = 0;
u8 motor_step = 20;

void    __ISR(_EXTERNAL_1_VECTOR, IPL4SOFT) buttonHANDLER(void)
{
    put_log("kbunel\r\n");
    put_log("4567noobs\r\n");

    LATFbits.LATF1 = 1 ^ LATFbits.LATF1;

    //motor_speed = -100;
    motor_speed = motor_speed + motor_step;
    if (motor_speed >= 100 || motor_speed <= -100)
        motor_step = -motor_step;

    setMotorSpeed(motor_speed);

    IFS0bits.INT1IF = 0;
}

void __ISR(_TIMER_2_VECTOR, IPL7SOFT) Timer2HANDLER(void)
{

    IFS0bits.T2IF = 0;
}

void    initialiseMotorControl()
{
    //timer 2 initialisation
    PR2 = 0xFF;                                         // Set period of Timer 2
    T2CONbits.TCKPS = 0b111;                            // prescale
    IFS0bits.T2IF = 0;                                  // Clear Timer interrupt flag
    IEC0bits.T2IE = 1;                                  // Set timer interrupt
    IPC2bits.T2IP = 0x7;                              // Set timer priority to 7

    //input 1 initialisation
    OC1CON = 0x0;                                       // turn off OC1 when performing setup
    OC1RS = 0x0;                                        // Initialize secondary Compare register
    OC1R =  0x0;                                        // Initialize primary Compare register
    OC1CONbits.OCM = 0x6;                               // Set OC mode to PWM

    //input 2 initialisation
    OC2CON = 0x0;                                       // turn off OC1 when performing setup
    OC2RS = 0x0;                                        // Initialize secondary Compare register
    OC2R =  0x0;                                        // Initialize primary Compare register
    OC2CONbits.OCM = 0x6;                               // Set OC mode to PWM

    // Set peripherals to ON
    T2CONbits.TON = 1;                                  // Enable timer 2
    OC1CONbits.ON = 1;                                  // Enable Output compare
    OC2CONbits.ON = 1;                                  // Enable Output compare
}

void    setMotorSpeed(s8 speed)
{
    if (speed < -100)
        speed = -100;
    else if (speed > 100)
        speed = 100;
    u8 period = (100 + speed) / 2 * PR2 / 100;            // Convert speed percentage into Compare register value
    OC1RS = period;                                       // Initialize secondary Compare register
    OC1R = period;                                        // Initialize primary Compare register
    OC2RS = PR2 - period;                                 // Initialize secondary Compare register
    OC2R = PR2 - period;                                  // Initialize primary Compare register
}

int     main(void)
{
    //set high on LED pin as output value
    //LATFbits.LATF1 = 1;

    //set LED pin as output
    TRISFbits.TRISF1 = 0;

    //set BUT pin as input
    TRISDbits.TRISD8 = 1;

    //setup INT1 to interrupt on falling edge
    INTCONbits.INT1EP = 0;

    //clear interrupt
    IFS0bits.INT1IF = 0;

    //set priority
    IPC1bits.INT1IP = 4;

    //Logic supply voltage setup on RB10
    TRISBbits.TRISB10 = 0;
    LATBbits.LATB10 = 1;

    //L298 Enable A on RE3
    TRISEbits.TRISE3 = 0;
    LATEbits.LATE3 = 1;

    //starting interrupts
    IEC0bits.INT1IE = 1;

    //UART logging initialisation
    init_logging();

    //Motor control initialisation
    initialiseMotorControl();

    INTCONSET = _INTCON_MVEC_MASK;
    __builtin_enable_interrupts();

    while (1)
        WDTCONbits.WDTCLR = 1;
    return (0);
}