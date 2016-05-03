/* 
 * File:   main.c
 * Author: bocal
 *
 * Created on March 23, 2016, 7:46 PM
 */

#include <p32xxxx.h>
#include <sys/attribs.h>
#include "types.h"
//#include <plib.h>

s8 logs[256];
u32 id = 0;

void    put_log(s8 *str)
{
    u32 i = id;
    if (i)
        while (logs[i])
            i++;
    while (*str)
        logs[i++] = *str++;
    logs[i] = 0;
    U1TXREG = logs[id++];
}

void __ISR(_EXTERNAL_1_VECTOR, IPL4SOFT) buttonHANDLER(void)
{
    put_log("kbunel\r\n");
    put_log("4567noobs\r\n");

    LATFbits.LATF1 = 1 ^ LATFbits.LATF1;
    IFS0bits.INT1IF = 0;
}

void __ISR(_UART_1_VECTOR, IPL7SRS) UARTHANDLER(void)
{
    if (logs[id])
        U1TXREG = logs[id++];
    else
        id = 0;
    IFS0bits.U1TXIF = 0;
}

void __ISR(_TIMER_2_VECTOR, IPL7SOFT) Timer2HANDLER(void)
{
    
    IFS0bits.T2IF = 0;
}

int main(void)
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

    //UART config
    U1BRG = 25;                                        //(8Mhz / 2) / (16 * 9600) - 1
    U1STA = 0;
    U1MODE = 0x8000;
    //U1MODEbits.PDSEL = 0x0;
    //U1MODEbits.STSEL = 0x0;

    //starting interrupt
    IPC6bits.U1IP = 0x7;

    U1STAbits.UTXISEL = 0x1;
    U1STAbits.UTXEN = 0x1;
    //U1MODEbits.UARTEN = 1;
    //U1MODEbits.ON = 1;

    //PWM config
    /*OC1CON = 0x0;                                      // turn off OC1 when performing setup
    PR2 = 0xFF;                                        // Set period of Timer 2
    T2CONbits.TCKPS = 0b100;                           // 8Mhz / 2 / 16
    
    OC1RS = 0x78;                                       // Initialize secondary Compare register
    OC1R =  0x78;                                       // Initialize primary Compare register
    OC1CONbits.OCM = 0x6;                               // Set OC mode to PWM

    IFS0CLR = 0x100;                                    // Clear Timer interrupt flag
    IEC0SET = 0x100;                                    // Set timer interrupt
    IPC2SET = 0x18;                                     // Set timer priority to 7

    T2CONSET = 0x8000;                                  // Enable timer 2
    OC1CONSET = 0x8000;                                 // Enable Output compare*/

    //starting interrupts  
    IEC0bits.INT1IE = 1;
    IEC0bits.U1TXIE = 0x1;

    INTCONSET = _INTCON_MVEC_MASK;
    __builtin_enable_interrupts();

    while (1)
        WDTCONbits.WDTCLR = 1;
    return (0);
}