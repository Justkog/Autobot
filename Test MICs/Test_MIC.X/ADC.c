
#include <p32xxxx.h>
#include <sys/attribs.h>
#include "types.h"

/*
 *
 * Sound detection workflow proposal :
 *
 * 1/ ADC is enabled and continuously listening
 * Average is continuously calculated to define the real '0'
 * This allows to calculate a threshold independant to long term voltage variations
 *
 * 2/ When a sound exceeding a threshold is detected in the buffer,
 * recording starts to save a certain amount of sound data for each mic in a predefined length buffer.
 *
 * 3/ At the end of the recording (buffer full),
 * We disable the ADC interruption to give PIC time to process the sound data and determine a direction.
 *
 * 4/ Once this is done the workflow restarts from the beginning
 * 
 *
 * May need to disable the sound detection when the bot is moving to avoid movement noise, wind may also be an issue
 */

void __ISR(_ADC_VECTOR, IPL7SRS) ADCHANDLER(void)
{
    s32 val1 = ADC1BUF7;
    s32 val2 = ADC1BUF8;
    s32 val3 = ADC1BUF9;
    if (val2 > 68)
    {
        log_key_val("ADC value 7", val1);
        log_key_val("ADC value 8", val2);
        log_key_val("ADC value 9", val3);
    }
    IFS1bits.AD1IF = 0;
}

void    init_ADC()
{
    // 1. Configure the analog port pins
    TRISBbits.TRISB5 = 1;
    AD1PCFGbits.PCFG5 = 0;

    // 2. Select the analog inputs to the ADC multiplexers
    // Negative input select bit for Sample A Multiplexer Setting
    AD1CHSbits.CH0NA = 0;           // Channel 0 negative input is VREFL
    //AD1CHSbits.CH0SA = 0x5;         // Channel 0 positive input is AN6

    // 3. Select the format of the ADC result
    // Data output format bits
    AD1CON1bits.FORM = 0b101;        // Signed integer 32-bit

    // 4. Select the sample clock source
    // Conversion trigger source
    AD1CON1bits.SSRC = 0b111;       // Auto-convert

    // Automatic sampling
    AD1CON1bits.ASAM = 1;

    // 5. Select the voltage reference source
    // Voltage reference configuration bits
    AD1CON2bits.VCFG = 0;           // AVDD / AVSS

    // 6. Select the Scan mode
    AD1CON2bits.CSCNA = 1;          // Input scan selection bit, enable scan mode
    AD1CSSLbits.CSSL5 = 1;          // ADC Input scan selection bits, AN5 for input scan

    // 7. Set the number of conversions per interrupt (if interrupts used)
    // Sample/Convert sequences per interrupt selection bits
    AD1CON2bits.SMPI = 0b1111;       // Interrupts at the completion of conversion of each 16th S/C sequence

    // 8. Set Buffer Fill mode
    AD1CON2bits.BUFM = 0;           // Buffer configured as one 16-word buffer

    // 9. Select the MUX to be connected to the ADC in ALTS


    // 10. Select the ADC clock source
    AD1CON3bits.ADRC = 0;           // PBCLK as the conversion clock source

    // 11. Select the sample time (if autoconvert used)
    //Auto sample time bits
    AD1CON3bits.SAMC = 4;           // 4 TAD

    // 12. Select the ADC clock prescaler
    AD1CON3bits.ADCS = 0b00001111;  // TAD = 32 * TPB

    // 13. Turn the ADC module on
    AD1CON1bits.ON = 1;

    // 14. ADC interrupt configuration
    // a) clear the interrupt flag bit
    IFS1bits.AD1IF = 0;

    // b) Select the ADC interrupt priority and subpriority
    IPC6bits.AD1IP = 0x7;

    // c) Enable the interruptions
    IEC1bits.AD1IE = 1;

    // 15. Start the conversion sequence by initiating sampling
    
}