
#include <p32xxxx.h>
#include <sys/attribs.h>
#include "types.h"

void    init_button()
{
    //set BUT pin as input
    TRISDbits.TRISD8 = 1;

    //setup INT1 to interrupt on falling edge
    INTCONbits.INT1EP = 0;

    //clear interrupt
    IFS0bits.INT1IF = 0;

    //set priority
    IPC1bits.INT1IP = 4;
}