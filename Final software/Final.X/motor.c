/*
 * File:   motor.c
 * Author: Autobots
 *
 * Created on June 21, 2016, 1:48 PM
 */

#include "autobot.h"

u8 save_available = 1;

// set motor handicap with mechanical testing to balance the manufacture differences
u8 motor_1_handicap = 85;
u8 motor_2_handicap = 100;

s8 motor_step = 1;
s8 motor_gear[7] = {-100, -80, -60, 0, 60, 80, 100};

u8 motor_gear_current_1 = 3;
u8 motor_gear_current_2 = 3;

u8 prev_motor_gear_current_1 = 0;
u8 prev_motor_gear_current_2 = 0;

u8      action_table_length = 0;
u8      action_table_index = 0;
void    (*action_table[ACTION_TABLE_MAX_LENGTH])(u16);
u16     action_table_arg[ACTION_TABLE_MAX_LENGTH];


void    Reset_Motor_Instructions(void)
{
    if (VERBOSE_MOTOR_SOFTWARE)
        put_str_ln("Reset Motor Instructions");
    Motor_Control_Stop(0);
    u8      i = 0;
    while (i < ACTION_TABLE_MAX_LENGTH)
    {
        action_table[i] = 0;
        action_table_arg[i] = 0;
        i++;
    }
    action_table_index = 0;
    action_table_length = 0;
}

void    Add_Motor_Instruction(void (*action)(u16), u16 delay)
{
    if (action_table_length < ACTION_TABLE_MAX_LENGTH)
    {
        action_table[action_table_length] = action;
        action_table_arg[action_table_length] = delay;
        if (VERBOSE_MOTOR_SOFTWARE)
        {
            put_log("Action no ");
            put_log_nb(action_table_length);
            put_str_ln(" added");
        }
    }
    else
    {
        if (VERBOSE_MOTOR_SOFTWARE)
        {
            put_log("Action no ");
            put_log_nb(action_table_length);
            put_str_ln(" ignored");
        }
    }

    action_table_length++;
}

void    Execute_Motor_Instructions(void)
{
    if(action_table_index < ACTION_TABLE_MAX_LENGTH &&
            action_table[action_table_index])
    {
        if (VERBOSE_MOTOR_SOFTWARE)
            log_key_val("Execute instruction number", action_table_index);
        (*action_table[action_table_index])(action_table_arg[action_table_index]);
        action_table_index++;
    }
    else
    {
        Reset_Motor_Instructions();
    }
}

void    Motor_Save()
{
    if (save_available)
    {
        prev_motor_gear_current_1 = motor_gear_current_1;
        prev_motor_gear_current_2 = motor_gear_current_2;
        save_available = 0;
        if (VERBOSE_MOTOR_SOFTWARE)
            put_str_ln("Saving Motor Status");
    }
}

void    Motor_Restore()
{
    motor_gear_current_1 = prev_motor_gear_current_1;
    motor_gear_current_2 = prev_motor_gear_current_2;
    set_Motor_Speed();
    save_available = 1;
    Execute_Motor_Instructions();
    if (VERBOSE_MOTOR_SOFTWARE)
        put_str_ln("Restoring Motor Status");
}

void    Motor_Delay(u16 delay_ms)
{
    if (VERBOSE_MOTOR_SOFTWARE)
        log_key_val("Delay ", delay_ms);
    Start_Motor_Timer(delay_ms);
}

void    Motor_Control_Forward(u16 delay)
{
    if (motor_gear_current_1 == motor_gear_current_2)
    {
        if (motor_gear_current_1 < 6)
        {
            motor_gear_current_1++;
            motor_gear_current_2++;
        }
        set_Motor_Speed();
        if (VERBOSE_MOTOR_SOFTWARE)
            log_key_val("Forward to ", motor_gear_current_1);
    }
    Motor_Save();
    Motor_Delay(delay);
}

void    Motor_Control_Backward(u16 delay)
{
    if (motor_gear_current_1 == motor_gear_current_2)
    {
        if (0 < motor_gear_current_1)
        {
            motor_gear_current_1--;
            motor_gear_current_2--;

        }
        set_Motor_Speed();
        if (VERBOSE_MOTOR_SOFTWARE)
            log_key_val("Backward to ", motor_gear_current_1);
    }
    Motor_Save();
    Motor_Delay(delay);
}

void    Motor_Control_Emergency_Stop()
{
    if (VERBOSE_MOTOR_SOFTWARE)
        put_str_ln("/!\\ EMERGENCY STOP /!\\");
    motor_timer_init();
    Reset_Motor_Instructions();
    Add_Motor_Instruction(Motor_Control_Backward, EMERGENCY_STOP_BACKWARD_DELAY);
    Execute_Motor_Instructions();
}

void    Motor_Control_Stop(u16 delay_ms)
{
    if (VERBOSE_MOTOR_SOFTWARE)
        put_str_ln("Motor Status Save Overrided");
    if (delay_ms)
    {
        prev_motor_gear_current_1 = 3;
        prev_motor_gear_current_2 = 3;
        save_available = 0;
        Motor_Delay(delay_ms);
        if (VERBOSE_MOTOR_SOFTWARE)
            put_str_ln("stop with delay");
    }
    else
    {
        prev_motor_gear_current_1 = 3;
        prev_motor_gear_current_2 = 3;
        motor_gear_current_1 = 3;
        motor_gear_current_2 = 3;
        set_Motor_Speed();
        if (VERBOSE_MOTOR_SOFTWARE)
            put_str_ln("stop without delay");
        save_available = 1;
    }
}

void    Motor_Control_Turn_Left(u16 delay)
{
    Motor_Save();
    if (motor_gear_current_1 == motor_gear_current_2)
    {
        if (motor_gear_current_2 > 3)
            motor_gear_current_2--;
        else if (motor_gear_current_2 < 3)
            motor_gear_current_2++;
    }
    set_Motor_Speed();
    if (VERBOSE_MOTOR_SOFTWARE)
        put_str_ln("Turning left");
    Motor_Delay(delay);
}

void    Motor_Control_Turn_Right(u16 delay)
{
    Motor_Save();
    if (motor_gear_current_1 == motor_gear_current_2)
    {
        if (motor_gear_current_1 > 3)
            motor_gear_current_1--;
        else if (motor_gear_current_1 < 3)
            motor_gear_current_1++;
    }
    set_Motor_Speed();
    if (VERBOSE_MOTOR_SOFTWARE)
        put_str_ln("Turning right");
    Motor_Delay(delay);
}

void    fuck_the_motor(void)
{
    u8  i = 0;
    while (i < 5)
    {
        put_str_ln("");
       i++;
    }
    Reset_Motor_Instructions();
    Add_Motor_Instruction(Motor_Control_Forward, 500);
    Add_Motor_Instruction(Motor_Control_Forward, 500);
    Add_Motor_Instruction(Motor_Control_Turn_Left, 3000);
    Add_Motor_Instruction(Motor_Delay, 1000);
    Add_Motor_Instruction(Motor_Control_Turn_Left, 3000);
    Add_Motor_Instruction(Motor_Delay, 1000);
    Add_Motor_Instruction(Motor_Control_Turn_Left, 3000);
    Add_Motor_Instruction(Motor_Delay, 1000);
    Add_Motor_Instruction(Motor_Control_Turn_Left, 3000);
    Execute_Motor_Instructions();
}

void    set_Motor_Speed(void)
{
    set_Motor_1_Speed();
    set_Motor_2_Speed();
}

void    set_Motor_1_Speed(void)
{
    s8 speed = motor_gear[motor_gear_current_1];
    if (speed < 0)
    {
        OC3RS = (100 + speed) * motor_1_handicap * 128 / 10000;
        OC4RS = motor_1_handicap * 128 / 100;
    }
    else
    {
        OC3RS = motor_1_handicap * 128 / 100;
        OC4RS = (100 - speed) * motor_1_handicap * 128 / 10000;
    }
    if (VERBOSE_MOTOR_HARDWARE)
    {
        put_str_ln("motor 1");
        log_key_val("L298 - Output 1", OC3RS);
        log_key_val("L298 - Output 2", OC4RS);
    }
 }

void    set_Motor_2_Speed(void)
{
    s8 speed = motor_gear[motor_gear_current_2];
    if (speed < 0)
    {
        OC2RS = (100 + speed) * motor_2_handicap * 128 / 10000;
        OC1RS = motor_2_handicap * 128 / 100;
    }
    else
    {
        OC2RS = motor_2_handicap * 128 / 100;
        OC1RS = (100 - speed) * motor_2_handicap * 128 / 10000;
    }
    if (VERBOSE_MOTOR_HARDWARE)
    {
        put_str_ln("motor 2");
        log_key_val("L298 - Output 3", OC2RS);
        log_key_val("L298 - Output 4", OC1RS);
    }
 }