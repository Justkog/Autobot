/*
 * File:   motor.c
 * Author: Autobots
 *
 * Created on June 21, 2016, 1:48 PM
 */

#include "autobot.h"

// set motor handicap with mechanical testing to balance the manufacture differences
u8 motor_1_handicap = 95;
u8 motor_2_handicap = 100;

s8 motor_step = 1;
s8 motor_gear[7] = {-100, -80, -60, 0, 60, 80, 100};

u8 motor_gear_current_1 = 3;
u8 motor_gear_current_2 = 3;

u8 prev_motor_gear_current_1 = 0;
u8 prev_motor_gear_current_2 = 0;

//Motor Controls FOT WINNERS ONLY !!!
/*STOP(delai)
right turn (angle)
left turn (angle)*/

void    Motor_Save()
{
    prev_motor_gear_current_1 = motor_gear_current_1;
    prev_motor_gear_current_2 = motor_gear_current_2;
    log_key_val("1", prev_motor_gear_current_1);
    log_key_val("2", prev_motor_gear_current_2);
}

void    Motor_Restore()
{
    log_key_val("11", prev_motor_gear_current_1);
    log_key_val("22", prev_motor_gear_current_2);
    motor_gear_current_1 = prev_motor_gear_current_1;
    motor_gear_current_2 = prev_motor_gear_current_2;
    set_Motor_Speed();
}

void    Motor_Delay(u16 delay_ms)
{
    Start_Motor_Timer(delay_ms);
}

void    Motor_Control_Forward(u8 gear)
{
    if (3 < gear && gear <= 6)
    {
        motor_gear_current_1 = gear;
        motor_gear_current_2 = gear;
        set_Motor_Speed();
    }
}

void    Motor_Control_Backward(u8 gear)
{
    if (0 <= gear && gear < 3)
    {
        motor_gear_current_1 = gear;
        motor_gear_current_2 = gear;
        set_Motor_Speed();
    }
}

// RAJOUTER LE DELAI
void Motor_Control_stop(void)
{
    motor_gear_current_1 = 3;
    motor_gear_current_2 = 3;
    set_Motor_Speed();
}

void    Motor_Control_Speed_Up(void)
{
    if (motor_gear_current_1 == motor_gear_current_2)
    {
        if (3 < motor_gear_current_1 && motor_gear_current_1 < 6)
        {
            motor_gear_current_1++;
            motor_gear_current_2++;
        }
        else if (0 < motor_gear_current_1 && motor_gear_current_1 < 3)
        {
            motor_gear_current_1--;
            motor_gear_current_2--;
        }
        set_Motor_Speed();
    }
}

void    Motor_Control_Slow_Down(void)
{
    if (motor_gear_current_1 == motor_gear_current_2)
    {
        if (3 < motor_gear_current_1 && motor_gear_current_1 <= 6)
        {
            motor_gear_current_1--;
            motor_gear_current_2--;
        }
        else if (0 <= motor_gear_current_1 && motor_gear_current_1 < 3)
        {
            motor_gear_current_1++;
            motor_gear_current_2++;
        }
        set_Motor_Speed();
    }
}

void    fuck_the_motor(void)
{
    Motor_Save();
    Motor_Control_Forward(6);
    Motor_Delay(1500);

    //Motor_Control_Forward(6);
/*    motor_gear_current_1 += motor_step;
    if (motor_gear_current_1 <= 0 || motor_gear_current_1 >= sizeof(motor_gear) - 1)
        motor_step = -motor_step;

    set_Motor_1_Speed(motor_gear[motor_gear_current_1]);
    set_Motor_2_Speed(motor_gear[motor_gear_current_2]);*/

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
 }