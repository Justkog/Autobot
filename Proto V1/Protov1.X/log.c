/* 
 * File:   log.c
 * Author: Autobots
 *
 * Created on June 21, 2016, 1:15 PM
 */

#include "autobot.h"

u16         logs_cursor;
u8          logs_buf[LOG_BUFFER_SIZE];

void        log()
{

}

void        __ISR(_UART_1_VECTOR, IPL6SOFT) UARTHANDLER(void)
{
    if (logs_buf[logs_cursor])
        U1TXREG = logs_buf[logs_cursor++];
    else
        logs_cursor = 0;
    IFS0bits.U1TXIF = 0;
}

void        init_logging()
{
    // LET SHUT IT DOWN
    U1MODEbits.ON = 0;
    // UART Config
    U1BRG = 64;
    U1STA = 0;




    U1MODEbits.ON = 1;
}