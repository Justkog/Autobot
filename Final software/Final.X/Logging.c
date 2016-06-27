
#include <p32xxxx.h>
#include <sys/attribs.h>
#include "types.h"

s8 logs[256];               //logs buffer
u32 id = 0;

char    wait_for_empty_log_buffer()
{
    while (id)
    {
        while (U1STAbits.UTXBF)
            ;
        if (logs[id])
            U1TXREG = logs[id++];
        else
            id = 0;
    }
}

void    put_log(s8 *str)
{
    u32 i = id;
    if (i)
    {
        while (logs[i])
        {
            i++;
        }
    }
    if (i >= sizeof(logs))
    {
        wait_for_empty_log_buffer();
        i = 0;
    }
    while (*str)
        logs[i++] = *str++;
    logs[i] = 0;
    if (!id)
        U1TXREG = logs[id++];
}

void    nb_to_str(s32 nb, s8 str[12])
{
    u32 i = 0;

    if (nb < 0)
    {
        str[i++] = '-';
        nb = -nb;
    }

    s8 c = 0;
    u8 started = 0;
    s32 pos = 1000000000;
    while (pos > 0)
    {
        c = (nb / pos);
        if (c > 0)
            started = 1;
        if (started)
            str[i++] = c + '0';
        nb = nb % pos;
        pos = pos / 10;
    }
    if (!started)
        str[i++] = '0';
    str[i++] = 0;
}

void    put_log_nb(s32 nb)
{
    s8      nb_buf[12];

    nb_to_str(nb, nb_buf);
    put_log(nb_buf);
}

void    put_nb_ln(s32 nb)
{
    put_log_nb(nb);
    put_log("\r\n");
}

void    put_str_ln(s8 *str)
{
    put_log(str);
    put_log("\r\n");
}

void    log_key_val(s8 *str, s32 nb)
{
    put_log(str);
    put_log(" : ");
    put_log_nb(nb);
    put_log("\r\n");
}

void    __ISR(_UART_1_VECTOR, IPL6SOFT) UARTHANDLER(void)
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
    IPC6bits.U1IP = 0x6;

    //?
    U1STAbits.UTXISEL = 0x1;
    U1STAbits.UTXEN = 0x1;

    IEC0bits.U1TXIE = 0x1;
}