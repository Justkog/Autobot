
#include "autobot.h"
//#include <plib.h>

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
        IEC1bits.U1TXIE = 0;
        wait_for_empty_log_buffer();
        i = 0;
    }
    while (*str)
        logs[i++] = *str++;
    logs[i] = 0;
    if (!id)
    {
        IEC1bits.U1TXIE = 1;                                // Switch interrupt ON
        U1TXREG = logs[id++];
    }
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
    {
        id = 0;
        IEC1bits.U1TXIE = 0;                                // Switch interrupt ON
    }
    IFS1bits.U1TXIF = 0;
}

void    logging_init()
{
    TRISBbits.TRISB4 = 0;                               // Set RPB4 pin as output


    // Mapping
    /*unsigned int status1, status2;
    mSYSTEMUnlock(status1, status2);
    PPSUnLock;
    PPSOutput(1, RPB4, U1TX);
    PPSLock;
    mSYSTEMLock(status1, status2);*/
    /*SYSKEY = 0x00000000;                                // Ensure OSCCON is locked
    SYSKEY = 0xAA996655;                                // Write Key1 to SYSKEY
    SYSKEY = 0x556699AA;                                // Write Key2 to SYSKEY*/
                                                        // OSCCON is now unlocked

    //CFGCONbits.IOLOCK = 0;                              // Allow writes to the control registers

    RPB4Rbits.RPB4R = 0b0001;                           // map UART 1 output on RPB4

    //CFGCONbits.IOLOCK = 1;                              // Prevent writes to the control registers*/

                                                        // Relock SYSKEY
    /*SYSKEY = 0x00000000;                                // Write any value other than Key1 or Key2
                                                        // OSCCON is relocked*/
    
    // UART config
    U1BRG = 77;                                         //(48Mhz / 4) / (16 * 9600) - 1
    U1STA = 0;                                          // reset UART Status and Control Registers

    // Interrupt Setup
    IEC1bits.U1TXIE = 0;                                // Switch interrupt OFF for setup
    IFS1bits.U1TXIF = 0;                                // Reset interrupt flag
    IPC8bits.U1IP = 0x6;                                // Set priority to 6 (only ADC has higher priority)
    U1STAbits.UTXISEL = 1;                              // Interrupt generated when all chars are transmitted
    U1STAbits.UTXEN = 1;                                // Transmit Enable bit

    // Enable UART
    U1MODEbits.ON = 1;                                  // Switch UART ON (after setup is completed)
}