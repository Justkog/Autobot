
#include <p32xxxx.h>
#include <sys/attribs.h>
#include "types.h"

/*void __ISR(_UART_1_VECTOR, IPL7SRS) ADCHANDLER(void)
{
    
    IFS0bits.U1TXIF = 0;
}*/

void    init_ADC()
{
    // 1. Configure the analog port pins
    AD1PCFGbits.PCFG6 = 0;

    // 2. Select the analog inputs to the ADC multiplexers
    // Negative input select bit for Sample A Multiplexer Setting
    AD1CHSbits.CH0NA = 0;           // Channel 0 negative input is VREFL
    AD1CHSbits.CH0SA = 0x6;         // Channel 0 positive input is AN6

    // 3. Select the format of the ADC result
    // Data output format bits
    AD1CON1bits.FORM = 0b101        // Signed integer 32-bit

    // 4. Select the sample clock source
    // Conversion trigger source
    AD1CON1bits.SSRC = 0b111;       // Auto-convert
    // may need to set up AD1CON3bits.SAMC

    // Automatic sampling
    AD1CON1bits.ASAM = 1;

    // 5. Select the voltage reference source
    // Voltage reference configuration bits
    AD1CON2bits.VCFG = 0;           // AVDD / AVSS

    // 6. Select the Scan mode


    // 7. Set the number of conversions per interrupt (if interrupts used)


    // 8. Set Buffer Fill mode


    // 9. Select the MUX to be connected to the ADC in ALTS


    // 10. Select the ADC clock source


    // 11. Select the sample time (if autoconvert used)


    // 12. Select the ADC clock prescaler


    // 13. Turn the ADC module on


    // 14. ADC interrupt configuration
    // a) clear the interrupt flag bit


    // b) Select the ADC interrupt priority and subpriority


    // 15. Start the conversion sequence by initiating sampling
    
}