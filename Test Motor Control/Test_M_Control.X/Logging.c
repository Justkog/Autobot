
#include <p32xxxx.h>
#include <sys/attribs.h>
#include "types.h"

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

void __ISR(_UART_1_VECTOR, IPL7SRS) UARTHANDLER(void)
{
    if (logs[id])
        U1TXREG = logs[id++];
    else
        id = 0;
    IFS0bits.U1TXIF = 0;
}

void    init_logging()
{
    //UART config
    U1BRG = 64;                                        //(80Mhz / 8 / 2) / (16 * 9600) - 1
    U1STA = 0;
    U1MODE = 0x8000;

    //setting interrupt priority
    IPC6bits.U1IP = 0x7;

    //?
    U1STAbits.UTXISEL = 0x1;
    U1STAbits.UTXEN = 0x1;

    IEC0bits.U1TXIE = 0x1;
}