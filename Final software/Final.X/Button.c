/*
 * File:   motor.c
 * Author: Autobots
 *
 * Created on June 21, 2016, 1:48 PM
 */

#include "autobot.h"

// Button based configuration :
// -> less than 2 sec push : start bot
// -> 2 to 5 sec push : change threshold
// -> more than 5 sec push : show battery status

u8      bot_started = 0;

void    Start_Bot(void)
{
    bot_started = 1;
    //LATCbits.LATC1 = 0;                         // Toggle Green LED
    if (VERBOSE_PIC_STATUS)
        put_str_ln("Bot started");
    
    Stop_Green_Led();
    Reset_Mic_Procedure();
    Enable_Bumper();
    
    //put_str_ln("dumping ADC");
    //Print_average();
    //DCH0CONbits.CHEN = 1;                       // Turn channel ON, initiate a transfer
}

void    Stop_Bot(u16 arg)
{
    bot_started = 0;
    LATCbits.LATC1 = 1;                         // Toggle Green LED
    if (VERBOSE_PIC_STATUS)
        put_str_ln("Bot stopped");
    Start_Green_Led();
    Reset_Mic_Procedure();
    Disable_Bumper();
    Enable_ADC();
}

u8    Is_Bot_Started(void)
{
    return (bot_started);
}

void __ISR(_EXTERNAL_2_VECTOR, IPL_ISR(PRIORITY_MOTOR)) bumperHANDLER(void)
{
    if (VERBOSE_MOTOR_SOFTWARE)
        put_str_ln("/!\\ BUMPER TOUCHED /!\\");
    Motor_Control_Emergency_Stop();
    // Relancer une phase d'ecoute ??? (Pas dans le cahier des charges)
    IFS0bits.INT2IF = 0;
}


void __ISR(_EXTERNAL_1_VECTOR, IPL_ISR(PRIORITY_MOTOR)) buttonHANDLER(void)
{
    if (VERBOSE_PIC_STATUS)
        put_str_ln("Safe button interrupt");
    
    if (!INTCONbits.INT1EP)
    {
        // Change back button interrupt to falling edge
        INTCONbits.INT1EP = 1;
        
        log_key_val("seconds", Get_Button_Seconds_Since_Push());
        
        // check if it complies to a config based on timer count
        if(Get_Button_Seconds_Since_Push() < 2)
        {
            Start_Bot();
            Stop_Button_Timer();
            Enable_ADC();
        }
        else if(Get_Button_Seconds_Since_Push() >= 2 &&
                Get_Button_Seconds_Since_Push() < 5)
        {
            Change_Threshold();
            Show_Threshold_Status();
        }
        /*else if(Get_Button_Seconds_Since_Push() >= 5)
        {
            Show_Battery_Status();
        }*/
        //Stop_Button_Timer();
        //Enable_ADC();
    }
    else
    {
        if (!Is_Bot_Started())
        {
            Disable_ADC();
            
            // Change button interrupt to rising edge
            INTCONbits.INT1EP = 0;                      // setup INT1 to interrupt on rising edge

            // Start button timer
            Start_Button_Timer();
        }
        else
        {
            // Stop the bot
            Motor_Control_Stop(0);
            Stop_Bot(50);
        }
    }
    //no need to turn it on as it is chained
    //DCH1CONbits.CHEN = 1;                       // Turn channel ON, initiate a transfer
    //DCH1ECONbits.SIRQEN = 1;                    // Start channel cell transfer if an interrupt matching CHAIRQ occurs

    //DCH0CONbits.CHEN = 1;                       // Turn channel ON, initiate a transfer
    //DCH0ECONbits.CFORCE = 1;                    // A DMA transfer is forced to begin
    //DCH0ECONbits.SIRQEN = 1;                    // Start channel cell transfer if an interrupt matching CHAIRQ occurs

    //counter = 0;

    //LATCbits.LATC1 = 1 ^ LATCbits.LATC1;          // Toggle LED

    //put_str_ln("dumping ADC");
    //Print_average();
    //DCH0CONbits.CHEN = 1;                       // Turn channel ON, initiate a transfer
    //DCH0ECONbits.CFORCE = 1;                    // A DMA transfer is forced to begin
    //DCH0ECONbits.SIRQEN = 1;                    // Start channel cell transfer if an interrupt matching CHAIRQ occurs*/

    //IEC0bits.AD1IE = 1;

    //u32 waiter = 500000;
    //while (waiter)
    //    waiter--;


    //fuck_the_motor();
    
    IFS0bits.INT1IF = 0;
}

void    bumper_init()
{
    IEC0bits.INT2IE = 0;                        // stop the interrupt
    TRISCbits.TRISC6 = 1;                       // set as input
    INT2Rbits.INT2R = 0b0101;                   // Configue RPC6 as INT2 trigger
    INTCONbits.INT2EP = 1;                      // Setup INT2 to interrupt on falling edge (due to pull-up)
    IFS0bits.INT2IF = 0;                        // Clear Interrupt Flag
    IPC2bits.INT2IP = PRIORITY_MOTOR;           // Set Interrupt Priority to MAX level
    IPC2bits.INT2IS = SUBPRIORITY_EMERGENCY;    // Set Interrupt subpriority to MAX level
    IEC0bits.INT2IE = 0;                        // Disable BUMPER Interrupt
}

void    Enable_Bumper(void)
{
    IFS0bits.INT2IF = 0;                        // Clear Interrupt Flag
    IEC0bits.INT2IE = 1;                        // Start BUMPER Interrupt
}

void    Disable_Bumper(void)
{
    IFS0bits.INT2IF = 0;                        // Clear Interrupt Flag
    IEC0bits.INT2IE = 0;                        // Disable BUMPER Interrupt
}

void    button_init()
{
    IEC0bits.INT1IE = 0;                        // stop the interrupt
    TRISBbits.TRISB9 = 1;                       // set BUT pin as input (1 = input)
    INT1Rbits.INT1R = 0b0100;                   // Configure RPB9 as INT1 trigger (Input Pin Selection)
    INTCONbits.INT1EP = 1;                      // setup INT1 to interrupt on falling edge
    IFS0bits.INT1IF = 0;                        // clear interrupt flag
    IPC1bits.INT1IP = PRIORITY_MOTOR;           // set priority
    IEC0bits.INT1IE = 1;                        // starting BUTTON interrupt
}