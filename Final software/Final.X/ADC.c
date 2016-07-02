
#include "autobot.h"

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

u8  average_started = 0;
s32 mic_1_average = 0;
s32 mic_2_average = 0;
s32 mic_3_average = 0;
s32 threshold = SOUND_THRESHOLD * 128;
s32 thresholds[5] = { SOUND_THRESHOLD_1 * 128, 
                        SOUND_THRESHOLD_2 * 128,
                        SOUND_THRESHOLD_3 * 128, 
                        SOUND_THRESHOLD_4 * 128,
                        SOUND_THRESHOLD_5 * 128};
u8  selected_threshold_index = 1;
u8  average_counter = 0;

// to be deleted
u8  event_enabled = 1;

u16 register_values = 0;

// sound based configuration : 
// -> 2 claps less than 2 sec appart (and more than 500ms each) = flee switch
// -> 3 claps less than 3 sec appart (and more than 500ms each) = keep rolling switch
// -> 4 claps less than 4 sec appart (and more than 500ms each) = auto circle switch

// User feedback after each config :
// 2 leds shutdown 2 sec
// red led lights up 3 sec if flee mode
// green led lights up 3 sec if keep rolling mode
// red led 500 ms blinking 3 sec if auto circle mode
// green led 500 ms blinking 3 sec if none
// back to green only

void Reset_Mic_Procedure(void)
{
    average_counter = 0;
    threshold = thresholds[selected_threshold_index];
}

void Change_Threshold(void)
{
    selected_threshold_index++;
    if (selected_threshold_index > 4)
        selected_threshold_index = 0;
    if (VERBOSE_MIC_SOFTWARE)
        log_key_val("threshold change to", selected_threshold_index);
    threshold = thresholds[selected_threshold_index];
}

void    Show_Battery_Status(void)
{
    u8 battery_level = 0;
    if (mic_3_average > BATTERY_LEVEL_100_MIC3_AVG)
        battery_level = 4;
    else if (mic_3_average > BATTERY_LEVEL_75_MIC3_AVG)
        battery_level = 3;
    else if (mic_3_average > BATTERY_LEVEL_50_MIC3_AVG)
        battery_level = 2;
    else
        battery_level = 1;
    
    if (VERBOSE_MIC_SOFTWARE)
        log_key_val("battery status", battery_level);
    
}

void Print_average(void)
{
    log_key_val("mic 1 avg", mic_1_average);
    log_key_val("mic 2 avg", mic_2_average);
    log_key_val("mic 3 avg", mic_3_average);
    log_key_val("threshold", threshold);
}

void    Disable_ADC(void)
{
    IEC0bits.AD1IE = 0;                         // Disable ADC interrutpions
}

void    Enable_ADC(void)
{
    IEC0bits.AD1IE = 1;                         // Disable ADC interrutpions
}

void __ISR(_ADC_VECTOR, IPL_ISR(PRIORITY_MIC)) ADCHANDLER(void)
{
    s32 val3 = ADC1BUF0 * 128;                            // MIC3 (left)
    s32 val2 = ADC1BUF1 * 128;                            // MIC2 (back)
    s32 val1 = ADC1BUF2 * 128;                            // MIC1 (right)
    //ADC1BUF0;
    //ADC1BUF1;
    //ADC1BUF2;

    /*mic_1_average = val1 / SOUND_AVERAGE_VALUE_COUNT + mic_1_average - mic_1_average / SOUND_AVERAGE_VALUE_COUNT;
    mic_2_average = val2 / SOUND_AVERAGE_VALUE_COUNT + mic_2_average - mic_2_average / SOUND_AVERAGE_VALUE_COUNT;
    mic_3_average = val3 / SOUND_AVERAGE_VALUE_COUNT + mic_3_average - mic_3_average / SOUND_AVERAGE_VALUE_COUNT;*/

    if (register_values)
    {
        Register_Value(register_values, val1, val2, val3);
        register_values++;
        if (register_values >= SOUND_BUFFER_SIZE)
        {
            register_values = 0;
            if (VERBOSE_MIC_HARDWARE)
            {
                Print_Event_Values();
                Print_Buffer_Values(18);
            }
            if (VERBOSE_MIC_SOFTWARE)
                put_str_ln("Recording done");
            s32 count = 0;
            while (count < 320000)
                count++;
            if (!Is_Bot_Started())
                IEC0bits.AD1IE = 1;                 // Reactivate ADC interruptions
        }
        IFS0bits.AD1IF = 0;                         // Reset ADC interrutpion flag
        return ;
    }
    
    if (average_counter < SOUND_AVERAGE_VALUE_COUNT)
    {
        mic_1_average = val1 / SOUND_AVERAGE_VALUE_COUNT + mic_1_average - mic_1_average / SOUND_AVERAGE_VALUE_COUNT;
        mic_2_average = val2 / SOUND_AVERAGE_VALUE_COUNT + mic_2_average - mic_2_average / SOUND_AVERAGE_VALUE_COUNT;
        mic_3_average = val3 / SOUND_AVERAGE_VALUE_COUNT + mic_3_average - mic_3_average / SOUND_AVERAGE_VALUE_COUNT;
        average_counter++;
        return ;
    }

    if (val1 < mic_1_average - threshold ||
            val2 < mic_2_average - threshold ||
            val3 < mic_3_average - threshold)
    {
        //DCH0CONbits.CHEN = 1;                       // Turn DMA channel ON, initiate a transfer
        register_values = 1;
        
        Register_Event_Values(val1, val2, val3, 
                                mic_1_average, mic_2_average, mic_3_average,
                                threshold);
        
        if (VERBOSE_MIC_HARDWARE)
        {
            /*put_str_ln("Event");
            log_key_val("mic 1", val1);
            log_key_val("mic 1 av", mic_1_average);
            log_key_val("mic 2", val2);
            log_key_val("mic 2 av", mic_2_average);
            log_key_val("mic 3", val3);
            log_key_val("mic 3 av", mic_3_average);             //*/
        }
        
        //IEC1bits.DMA0IE = 1;
        //DCH1CONbits.CHEN = 1;                       // Turn DMA channel ON, initiate a transfer
        //DCH0CONbits.CHEN = 1;                       // Turn DMA channel ON, initiate a transfer
        //DMACONbits.ON = 1;
        
        IFS0bits.AD1IF = 0;                         // Reset ADC interrutpion flag
        //IEC0bits.AD1IE = 0;                         // Disable ADC interrutpions
        return ;

        //IEC1bits.DMA0IE = 1;
        //DCH0CONbits.CHEN = 1;                       // Turn channel ON, initiate a transfer
    }
    
    mic_1_average = val1 / SOUND_AVERAGE_VALUE_COUNT + mic_1_average - mic_1_average / SOUND_AVERAGE_VALUE_COUNT;
    mic_2_average = val2 / SOUND_AVERAGE_VALUE_COUNT + mic_2_average - mic_2_average / SOUND_AVERAGE_VALUE_COUNT;
    mic_3_average = val3 / SOUND_AVERAGE_VALUE_COUNT + mic_3_average - mic_3_average / SOUND_AVERAGE_VALUE_COUNT;

    /*if (!average_started && (val0 > -25 || val0 < -100))
    {
        IFS0bits.AD1IF = 0;
        return;
    }

    if (!average_started)
    {
        average = val0 * 100;
        average_started = 1;
    }

    if (!print && event_enabled && (val0 * 100 > average + threshold || val0 * 100 < average - threshold))
    {
        //print = 1;
        //DCH1ECONbits.SIRQEN = 1;

        put_str_ln("Event");
        log_key_val("value", val0);
        log_key_val("average", average / 100);

        IFS0bits.AD1IF = 0;
        IEC0bits.AD1IE = 0;

        DCH0CONbits.CHEN = 1;                       // Turn channel ON, initiate a transfer


        /*AD1CON1bits.ON = 0;

        DCH0CONbits.CHEN = 1;                       // Turn channel ON, initiate a transfer

        AD1CSSLbits.CSSL2 = 1;          // ADC Input scan selection bits, AN2 for input scan
        AD1CSSLbits.CSSL3 = 1;          // ADC Input scan selection bits, AN3 for input scan
        AD1CSSLbits.CSSL4 = 1;          // ADC Input scan selection bits, AN4 for input scan

        AD1CON2bits.SMPI = 2;

        AD1CON1bits.ON = 1;
        
        //DCH0ECONbits.CFORCE = 1;                    // A DMA transfer is forced to begin
        //DCH0ECONbits.SIRQEN = 1;                    // Start channel cell transfer if an interrupt matching CHAIRQ occurs

        //AD1CON1bits.ON = 0;
        //AD1CSSLbits.CSSL2 = 1;          // ADC Input scan selection bits, AN2 for input scan
        //AD1CSSLbits.CSSL3 = 1;          // ADC Input scan selection bits, AN3 for input scan
        //AD1CSSLbits.CSSL4 = 0;          // ADC Input scan selection bits, AN4 for input scan
        //AD1CON1bits.ON = 1;

        //disable ADC interrupts
        //IFS0bits.AD1IF = 0;
        //IEC0bits.AD1IE = 0;
    }*/

    /*if (print)
    {
        print++;
        log_key_val("dest 0 value", val0);
        if (print > 16)
        {
            print = 0;
            put_str_ln("Done");
        }
    }*/

    // update of the average
    // (calculation is wrong as we remove a proportional part as the first element,
    // but the approximation is ok as long as we don't have too crazy values)
    // average = val0 * 100 / average_count + average - average / average_count;

    
    //log_key_val("value", ADC1BUF0);

    IFS0bits.AD1IF = 0;                         // Reset ADC interrutpion flag
}

void    init_ADC(void)
{
    // 1. Configure the analog port pins
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB2 = 1;

    // This is not on our PIC
    /*AD1PCFGbits.PCFG2 = 0;
    AD1PCFGbits.PCFG3 = 0;
    AD1PCFGbits.PCFG4 = 0;*/

    // 2. Select the analog inputs to the ADC multiplexers
    // Negative input select bit for Sample A Multiplexer Setting
    AD1CHSbits.CH0NA = 0;           // Channel 0 negative input is VREFL
    //AD1CHSbits.CH0SA = 0x5;         // Channel 0 positive input is AN5

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
    AD1CSSLbits.CSSL2 = 1;          // ADC Input scan selection bits, AN2 for input scan
    AD1CSSLbits.CSSL3 = 1;          // ADC Input scan selection bits, AN3 for input scan
    AD1CSSLbits.CSSL4 = 1;          // ADC Input scan selection bits, AN4 for input scan

    // 7. Set the number of conversions per interrupt (if interrupts used)
    // Sample/Convert sequences per interrupt selection bits
    AD1CON2bits.SMPI = 2;       // Interrupts at the completion of conversion of each 3rd S/C sequence

    // 8. Set Buffer Fill mode
    AD1CON2bits.BUFM = 0;           // Buffer configured as one 16-word buffer

    // 9. Select the MUX to be connected to the ADC in ALTS


    // 10. Select the ADC clock source
    AD1CON3bits.ADRC = 0;           // PBCLK as the conversion clock source

    // 11. Select the sample time (if autoconvert used)
    // Auto sample time bits
    AD1CON3bits.SAMC = 5;           // 3 TAD

    // 12. Select the ADC clock prescaler
    AD1CON3bits.ADCS = 3;  // TAD = 32 * TPB = 2 * (ADCS + 1) * TPB

    // 13. Turn the ADC module on
    //AD1CON1bits.ON = 1;

    // 14. ADC interrupt configuration
    // a) clear the interrupt flag bit
    IFS0bits.AD1IF = 0;

    // b) Select the ADC interrupt priority and subpriority
    IPC5bits.AD1IP = PRIORITY_MIC;

    // c) Enable the interruptions
    IEC0bits.AD1IE = 1;

    // 15. Start the conversion sequence by initiating sampling
    AD1CON1bits.ON = 1;
    
    Reset_Mic_Procedure();
    
}