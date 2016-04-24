/* 
 * File:   main.c
 * Author: bocal
 *
 * Created on March 23, 2016, 7:46 PM
 */

#include <p32xxxx.h>
#include <sys/attribs.h>
#include "types.h"

u32 test = 0;

void __ISR(_EXTERNAL_1_VECTOR, IPL4SOFT) buttonHANDLER(void)
{
    U1TXREG = 'k';
    LATFbits.LATF1 = 1 ^ LATFbits.LATF1;
    IFS0bits.INT1IF = 0;
}

void __ISR(_UART_1_VECTOR, IPL7SRS) UARTHANDLER(void)
{
    //LATFbits.LATF1 = 1 ^ LATFbits.LATF1;
    //U1TXREG = 'k';
    test++;
    IFS0bits.U1TXIF = 0;
}

/*
 * 
 */

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

    //starting interrupts  
    IEC0bits.INT1IE = 1;
    IEC0bits.U1TXIE = 0x1;

    INTCONSET = _INTCON_MVEC_MASK;
    __builtin_enable_interrupts();
    
    while (1)
    {
        WDTCONbits.WDTCLR = 1;
    }
    return (0);
}
