
#include "autobot.h"

/*
 *
 */

// buffer 2 is left    (MIC1)
// buffer 1 is back     (MIC2)
// buffer 0 is right     (MIC3)

s32 buffer[3][SOUND_BUFFER_SIZE];               //ADC buffer
u32 adc_buffer_id = 0;
s32 mic_1_avg = 0;
s32 mic_2_avg = 0;
s32 mic_3_avg = 0;
s32 current_threshold = 0;

unsigned int Virt2Phy0(const void* p)
{
    return (int)p<0?((int)p&0x1FFFFFFFL):(unsigned int)((unsigned char*)p+0x40000000L);
}

void    Print_Buffer_Values(u8 count)
{
    u8 i = 0;
    while (i < count)
    {
        log_key_val("m1", buffer[2][i] * 128);
        log_key_val("m2", buffer[1][i] * 128);
        log_key_val("m3", buffer[0][i] * 128);
        put_str_ln("");
        i++;
    }
}

void    Print_Event_Values(void)
{
    log_key_val("m1 avg", mic_1_avg);
    log_key_val("m2 avg", mic_2_avg);
    log_key_val("m3 avg", mic_3_avg);
    log_key_val("threshold", current_threshold);
    put_str_ln("");
}

void    Register_Event_Values(s32 val1, s32 val2, s32 val3,
                                s32 mic_1_average, s32 mic_2_average, s32 mic_3_average,
                                s32 threshold)
{
    buffer[2][0] = val1 / 128;
    buffer[1][0] = val2 / 128;
    buffer[0][0] = val3 / 128;
    mic_1_avg = mic_1_average;
    mic_2_avg = mic_2_average;
    mic_3_avg = mic_3_average;
    current_threshold = threshold;
}

void    Register_Value(u16 index, s32 val1, s32 val2, s32 val3)
{
    buffer[2][index] = val1 / 128;
    buffer[1][index] = val2 / 128;
    buffer[0][index] = val3 / 128;
}

s32     Get_Buffer_Avg(u8 buffer_id)
{
    if (buffer_id == 0)
        return (mic_3_avg);
    else if (buffer_id == 1)
        return (mic_2_avg);
    else if (buffer_id == 2)
        return (mic_1_avg);
}

/*
 *
 * This function is used to find the sound in the mic following the one which triggered
 * It also provides the delay behind the main mic in ticks
 *  
 */

u8     Find_Next_Mic(u8 start_tick, u8 *ticks_ahead, s32 threshold,  u8 start_buffer)
{
    u8 i = start_tick;
    while (i < SOUND_BUFFER_SIZE)
    {
        u8 buffer_id = 0;
        while (buffer_id < 3)
        {
            if (buffer_id == start_buffer)
            {
                buffer_id++;
                continue;
            }
            s32 avg = Get_Buffer_Avg(buffer_id);
            if (buffer[buffer_id][i] * 128 < avg - threshold)
            {
                *ticks_ahead = i;
                return (buffer_id);
            }
            buffer_id++;
        }
        i++;
    }
    *ticks_ahead = 0;
    return (3);
}

/*
 * This function calculates the turn delay
 * in case the right or left mic triggered the event
 * 
 * found_mic is the second receiver mic
 * if it corresponds to the second front facing (close_mic)
 * we get a shortened delay compared to 60°
 * 
 * else we get a longer delay compared to 60°
 */

u16     Calculate_Turn_Delay(u8 found_mic, u8 close_mic, u8 far_mic, u8 ticks_ahead)
{
    u16 turn_delay = SIXTY_DEGREES_TURN_DELAY;             // Default turn delay
    s16 turn_precision = 0;
    if (far_mic == found_mic)
    {
        // second is back so we turn more
        turn_precision = (MIC_ADC_TICKS_DISTANCE - ticks_ahead) * TICK_DELAY_FACTOR;
    }
    else if (close_mic == found_mic)
    {
        // second is right, 25 represents the max observed ticks ahead
        turn_precision = (-MIC_ADC_TICKS_DISTANCE + ticks_ahead) * TICK_DELAY_FACTOR;
    }
    if (turn_precision < -turn_delay)
    {
        turn_precision = -turn_delay + TICK_DELAY_FACTOR;
    }
    turn_delay = turn_delay + turn_precision;
    return (turn_delay);
}

/*
 * Special function for back mic,
 * The more ticks from the next mic,
 * The more we have to turn
 */

u16     Calculate_Back_Turn_Delay(u8 ticks_ahead)
{
    if (ticks_ahead > 25)
        ticks_ahead = 25;
    u16 turn_delay = SIXTY_DEGREES_TURN_DELAY * 2;             // Default turn delay
    s16 turn_precision = 0;
    turn_precision = ticks_ahead * TICK_DELAY_FACTOR;
    turn_delay = turn_delay + turn_precision;
    return (turn_delay);
}

void    Analyse_And_Move()
{
    // Mic 1 is the first sequentially sampled,
    // Mic 3 is the last sequentially sampled,
    // so a dely may exist between mic 1 and mic 3

    // a sound may be seen on mic 1 and not mic 3
    // even if they receive it at the exact same moment

    if (VERBOSE_MIC_SOFTWARE)
    {
        log_key_val("m1", buffer[2][0] * 128);
        log_key_val("m2", buffer[1][0] * 128);
        log_key_val("m3", buffer[0][0] * 128);
    }
    
    // if all mics recorded a surge, do nothing (vibrations)
    if (buffer[2][0] * 128 < mic_1_avg - current_threshold && 
            buffer[1][0] * 128 < mic_2_avg - current_threshold &&
            buffer[0][0] * 128 < mic_3_avg - current_threshold)
    {
        if (VERBOSE_MOTOR_SOFTWARE)
            put_str_ln("Doing nothing");
        motor_timer_init();
        Reset_Motor_Instructions();
        Reset_Mic_Procedure();
        Add_Motor_Instruction(Motor_Control_Stop, 50);
        Add_Motor_Instruction(Motor_Control_Stop, NEW_RECORD_DELAY);
        Add_Motor_Instruction(Enable_ADC, 0);
        Execute_Motor_Instructions();
    }

    // if right and left mic, go straight
    else if (buffer[2][0] * 128 < mic_1_avg - current_threshold / 2 && 
            buffer[0][0] * 128 < mic_3_avg - current_threshold / 2)
    {
        if (VERBOSE_MOTOR_SOFTWARE)
            put_str_ln("Going straight");
        if (MOVEMENT_ACTIVATED)
        {
            motor_timer_init();
            Reset_Motor_Instructions();
            Reset_Mic_Procedure();
            Add_Motor_Instruction(Motor_Control_Forward, 100);
            Add_Motor_Instruction(Motor_Control_Forward, 500);
            Add_Motor_Instruction(Motor_Control_Forward, 3000);
            Add_Motor_Instruction(Motor_Control_Stop, 50);
            Add_Motor_Instruction(Motor_Control_Stop, NEW_RECORD_DELAY);
            Add_Motor_Instruction(Enable_ADC, 0);
            Execute_Motor_Instructions();
        }
    }

    // if left mic, turn left
    else if(buffer[2][0] * 128 < mic_1_avg - current_threshold)
    {
        u8 ticks_ahead = 0;
        u8 next_mic = Find_Next_Mic(0, &ticks_ahead, current_threshold / 2, 2);
        u16 turn_delay = Calculate_Turn_Delay(next_mic, 0, 1, ticks_ahead);
        if (VERBOSE_MOTOR_SOFTWARE)
        {
            put_str_ln("Going left");
            log_key_val("next mic is", next_mic);
            log_key_val("ticks ahead", ticks_ahead);
            log_key_val("delay", turn_delay);
        }
        if (MOVEMENT_ACTIVATED)
        {
            motor_timer_init();
            Reset_Motor_Instructions();
            Reset_Mic_Procedure();
            Add_Motor_Instruction(Motor_Control_Forward, 50);
            Add_Motor_Instruction(Motor_Control_Turn_Left, turn_delay);
            Add_Motor_Instruction(Motor_Control_Forward, 50);
            Add_Motor_Instruction(Motor_Control_Forward, 50);
            Add_Motor_Instruction(Motor_Control_Turn_Left, SIDE_SWIM_DELAY);
            Add_Motor_Instruction(Motor_Control_Turn_Right, SIDE_SWIM_DELAY);
            Add_Motor_Instruction(Motor_Control_Turn_Left, SIDE_SWIM_DELAY);
            Add_Motor_Instruction(Motor_Control_Turn_Right, SIDE_SWIM_DELAY);
            Add_Motor_Instruction(Motor_Control_Turn_Left, SIDE_SWIM_DELAY);
            Add_Motor_Instruction(Motor_Control_Stop, 50);
            Add_Motor_Instruction(Motor_Control_Stop, NEW_RECORD_DELAY);
            Add_Motor_Instruction(Enable_ADC, 0);
            Execute_Motor_Instructions();
        }
    }

    // if right mic, turn right
    else if(buffer[0][0] * 128 < mic_3_avg - current_threshold)
    {
        u8 ticks_ahead = 0;
        u8 next_mic = Find_Next_Mic(0, &ticks_ahead, current_threshold / 2, 0);
        u16 turn_delay = Calculate_Turn_Delay(next_mic, 2, 1, ticks_ahead);
        if (VERBOSE_MOTOR_SOFTWARE)
        {
            put_str_ln("Going right");
            log_key_val("next mic is", next_mic);
            log_key_val("ticks ahead", ticks_ahead);
            log_key_val("delay", turn_delay);
        }
        if (MOVEMENT_ACTIVATED)
        {
            motor_timer_init();
            Reset_Motor_Instructions();
            Reset_Mic_Procedure();
            Add_Motor_Instruction(Motor_Control_Forward, 50);
            Add_Motor_Instruction(Motor_Control_Turn_Right, turn_delay);
            Add_Motor_Instruction(Motor_Control_Forward, 50);
            Add_Motor_Instruction(Motor_Control_Forward, 50);
            Add_Motor_Instruction(Motor_Control_Turn_Left, SIDE_SWIM_DELAY);
            Add_Motor_Instruction(Motor_Control_Turn_Right, SIDE_SWIM_DELAY);
            Add_Motor_Instruction(Motor_Control_Turn_Left, SIDE_SWIM_DELAY);
            Add_Motor_Instruction(Motor_Control_Turn_Right, SIDE_SWIM_DELAY);
            Add_Motor_Instruction(Motor_Control_Stop, 50);
            Add_Motor_Instruction(Motor_Control_Stop, NEW_RECORD_DELAY);
            Add_Motor_Instruction(Enable_ADC, 0);
            Execute_Motor_Instructions();
        }
    }

    // if back mic, turn around approx 140°
    else if(buffer[1][0] * 128 < mic_2_avg - current_threshold)
    {
        u8 ticks_ahead = 0;
        u8 next_mic = Find_Next_Mic(0, &ticks_ahead, current_threshold / 2, 1);
        u16 turn_delay = Calculate_Back_Turn_Delay(ticks_ahead);
        if (VERBOSE_MOTOR_SOFTWARE)
        {
            put_str_ln("Going back");
            log_key_val("next mic is", next_mic);
            log_key_val("ticks ahead", ticks_ahead);
            log_key_val("delay", turn_delay);
        }
        if (MOVEMENT_ACTIVATED)
        {
            motor_timer_init();
            Reset_Motor_Instructions();
            Reset_Mic_Procedure();
            Add_Motor_Instruction(Motor_Control_Backward, 50);
            if (next_mic == 2)
                Add_Motor_Instruction(Motor_Control_Turn_Right, turn_delay);
            else
                Add_Motor_Instruction(Motor_Control_Turn_Left, turn_delay);
            /*Add_Motor_Instruction(Motor_Control_Forward, 50);
            Add_Motor_Instruction(Motor_Control_Forward, 50);
            Add_Motor_Instruction(Motor_Control_Forward, 50);
            Add_Motor_Instruction(Motor_Control_Forward, 50);
            Add_Motor_Instruction(Motor_Control_Turn_Left, SIDE_SWIM_DELAY);
            Add_Motor_Instruction(Motor_Control_Turn_Right, SIDE_SWIM_DELAY);
            Add_Motor_Instruction(Motor_Control_Turn_Left, SIDE_SWIM_DELAY);
            Add_Motor_Instruction(Motor_Control_Turn_Right, SIDE_SWIM_DELAY);*/
            Add_Motor_Instruction(Motor_Control_Stop, 50);
            Add_Motor_Instruction(Motor_Control_Stop, NEW_RECORD_DELAY);
            Add_Motor_Instruction(Enable_ADC, 0);
            Execute_Motor_Instructions();
        }
    }
}

/*void __ISR(_DMA_2_VECTOR, IPL_ISR(PRIORITY_MIC)) DMAHANDLER(void)
{
    if (VERBOSE_MIC_HARDWARE)
    {
        Print_Event_Values();
        Print_Buffer_Values(8);
    }
    
    if (VERBOSE_MIC_SOFTWARE)
        put_str_ln("Recording done");

    // Analyse the sound
    if (Is_Bot_Started())
    {
        // Analyse the sound and act
        Analyse_And_Move();
    }
    else
    {
        // Register the sound based config and reset the related timer
        
    }
    
    // Just wait a little til restarting the ADC to not get another part with the end of the clap
    s32 count = 0;
    while (count < 320000)
        count++;

    DCH0INTbits.CHDDIF = 0;
    IFS1bits.DMA0IF = 0;
    DCH0CONbits.CHEN = 0;
    
    DCH1INTbits.CHDDIF = 0;
    IFS1bits.DMA1IF = 0;
    DCH1CONbits.CHEN = 0;
    
    DCH2INTbits.CHDDIF = 0;
    IFS1bits.DMA2IF = 0;
    DCH2CONbits.CHEN = 0;

    if (!Is_Bot_Started())
        IEC0bits.AD1IE = 1;                 // Reactivate ADC interruptions
}*/

void    init_DMA()
{
    IEC1bits.DMA0IE = 0;            // Disable DMA channel 0 interrupts
    IFS1bits.DMA0IF = 0;            // Clear any existing DMA channel 0 interrupt flag
    IEC1bits.DMA1IE = 0;            // Disable DMA channel 1 interrupts
    IFS1bits.DMA1IF = 0;            // Clear any existing DMA channel 1 interrupt flag
    IEC1bits.DMA2IE = 0;            // Disable DMA channel 1 interrupts
    IFS1bits.DMA2IF = 0;            // Clear any existing DMA channel 1 interrupt flag*/

    //DMACONbits.ON = 1;              // Enable the DMA controller

    // Master Channel 0 setup
    DCH0CONbits.CHEN = 0;           // Channel 0 off
    DCH0CONbits.CHCHN = 0;          // no chaining
    DCH0CONbits.CHPRI = 3;          // Priority 3 (highest)
    DCH0CONbits.CHAEN = 1;          // Channel is continuously enabled, and not automatically disabled after a block transfer is complete
    // Slave Channel 1 setup
    DCH1CONbits.CHEN = 0;           // Channel 1 off
    DCH1CONbits.CHCHN = 1;          // chaining
    DCH1CONbits.CHPRI = 2;          // Priority 2
    DCH1CONbits.CHAED = 1;          // Channel start/abort events will be registered, even if the channel is disabled
    DCH1CONbits.CHCHNS = 0;         // Chain to channel higher in natural priority (CH1 will be enabled by CH0 transfer complete*/
    //DCH1CONbits.CHAEN = 1;
    // Slave Channel 2 setup
    DCH2CONbits.CHEN = 0;           // Channel 2 off
    DCH2CONbits.CHCHN = 1;          // chaining
    DCH2CONbits.CHPRI = 2;          // Priority 2
    DCH2CONbits.CHAED = 1;          // Channel start/abort events will be registered, even if the channel is disabled
    DCH2CONbits.CHCHNS = 0;         // Chain to channel higher in natural priority (CH2 will be enabled by CH1 transfer complete*/
    //DCH2CONbits.CHAEN = 1;
    
    // Program the transfer on Channel 0
    DCH0SSA = Virt2Phy0(&ADC1BUF0);             // Transfer source physical address
    DCH0DSA = Virt2Phy0(&(buffer[0][1]));             // Transfer destination physical address
    DCH0SSIZ = sizeof(ADC1BUF0);                // Source size
    DCH0DSIZ = sizeof(buffer[0]) - sizeof(buffer[0][0]);               // Destination size
    DCH0CSIZ = sizeof(ADC1BUF0);                // bytes transferred per event

    // Program the transfer on Channel 1
    DCH1SSA = Virt2Phy0(&ADC1BUF1);             // Transfer source physical address
    DCH1DSA = Virt2Phy0(&(buffer[1][1]));             // Transfer destination physical address
    DCH1SSIZ = sizeof(ADC1BUF1);                // Source size
    DCH1DSIZ = sizeof(buffer[1]) - sizeof(buffer[1][0]);               // Destination size
    DCH1CSIZ = sizeof(ADC1BUF1);                // bytes transferred per event*/
    
    // Program the transfer on Channel 2
    DCH2SSA = Virt2Phy0(&ADC1BUF2);             // Transfer source physical address
    DCH2DSA = Virt2Phy0(&(buffer[2][1]));             // Transfer destination physical address
    DCH2SSIZ = sizeof(ADC1BUF2);                // Source size
    DCH2DSIZ = sizeof(buffer[2]) - sizeof(buffer[2][0]);               // Destination size
    DCH2CSIZ = sizeof(ADC1BUF2);                // bytes transferred per event*/
    
    DCH0ECONbits.CHSIRQ = _ADC_IRQ;             //link DMA channel 0 start to ADC interrupt
    DCH1ECONbits.CHSIRQ = _ADC_IRQ;             //link DMA channel 1 start to ADC interrupt
    DCH2ECONbits.CHSIRQ = _ADC_IRQ;             //link DMA channel 1 start to ADC interrupt
    DCH0ECONbits.SIRQEN = 1;                    // Start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH1ECONbits.SIRQEN = 1;                    // Start channel cell transfer if an interrupt matching CHSIRQ occurs
    DCH2ECONbits.SIRQEN = 1;                    // Start channel cell transfer if an interrupt matching CHSIRQ occurs

    // Master Channel 0
    DCH0INTCLR = 0x00FF00FF;                    // Clear existing events, disable all interrupts
    DCH0INTbits.CHERIE = 1;                     // Enable Channel address error interrupt
    //DCH0INTbits.CHDDIE = 1;                   // Enable Channel destination done interrupt
    DCH0INTbits.CHBCIE = 1;
    // Slave Channel 1
    DCH1INTCLR = 0x00FF00FF;                    // Clear existing events, disable all interrupts
    DCH1INTbits.CHERIE = 1;                     // Enable Channel address error interrupt
    //DCH1INTbits.CHDDIE = 1;                     // Enable Channel destination done interrupt*/
    DCH1INTbits.CHBCIE = 1;
    // Slave Channel 2
    DCH2INTCLR = 0x00FF00FF;                    // Clear existing events, disable all interrupts
    DCH2INTbits.CHERIE = 1;                     // Enable Channel address error interrupt
    DCH2INTbits.CHDDIE = 1;                     // Enable Channel destination done interrupt*/

    // Master Channel 0
    /*IPC10bits.DMA0IP = PRIORITY_MIC;            // Set Interrupt priority to 4
    IEC1bits.DMA0IE = 1;                        // Enable DMA Channel 0 interrupts*/

    // Slave Channel 1 
    /*IPC10bits.DMA1IP = PRIORITY_MIC;            // Set Interrupt priority to 4
    IPC10bits.DMA1IS = 2;                       // Set Interrupt subpriority to 2
    IEC1bits.DMA1IE = 1;                        // Enable DMA Channel 1 interrupts*/
    
    // Slave Channel 2
    IPC10bits.DMA2IP = PRIORITY_MIC;            // Set Interrupt priority to 4
    IPC10bits.DMA2IS = 2;                       // Set Interrupt subpriority to 2
    IEC1bits.DMA2IE = 1;                        // Enable DMA Channel 1 interrupts*/

    DMACONbits.ON = 1;                          // Enable the DMA controller
    
}