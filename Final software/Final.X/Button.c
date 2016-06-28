
#include "autobot.h"

void __ISR(_EXTERNAL_1_VECTOR, IPL4SOFT) buttonHANDLER(void)
{
    //no need to turn it on as it is chained
    //DCH1CONbits.CHEN = 1;                       // Turn channel ON, initiate a transfer
    //DCH1ECONbits.SIRQEN = 1;                    // Start channel cell transfer if an interrupt matching CHAIRQ occurs

    //DCH0CONbits.CHEN = 1;                       // Turn channel ON, initiate a transfer
    //DCH0ECONbits.CFORCE = 1;                    // A DMA transfer is forced to begin
    //DCH0ECONbits.SIRQEN = 1;                    // Start channel cell transfer if an interrupt matching CHAIRQ occurs

    //counter = 0;

    LATCbits.LATC1 = 1 ^ LATCbits.LATC1;

    put_str_ln("YOLOSWAG!!!");
    
    IFS0bits.INT1IF = 0;
}

void    init_button()
{
    //stop interrupts
    IEC0bits.INT1IE = 0;

    //set BUT pin as input
    TRISBbits.TRISB9 = 1;

    // Configure RPB9 as INT1 trigger (Input Pin Selection)
    INT1Rbits.INT1R = 0b0100;

    //setup INT1 to interrupt on falling edge
    INTCONbits.INT1EP = 0;

    //clear interrupt
    IFS0bits.INT1IF = 0;

    //set priority
    IPC1bits.INT1IP = 4;

    //starting interrupts
    IEC0bits.INT1IE = 1;
}