/* 
 * File:   autobot.h
 * Author: Autobots
 *
 * Created on June 21, 2016, 1:15 PM
 */

#ifndef AUTOBOT_H
# define AUTOBOT_H

# include <p32xxxx.h>
# include <sys/attribs.h>
# include "config.h"
# include "types.h"

# define CONCAT(x,y,z) x##y##z
# define IPL_ISR(x) CONCAT(IPL,x,SOFT)

void    Motor_Control_Turn_Left(u16 delay);
void    Motor_Control_Turn_Right(u16 delay);
void    Motor_Control_Stop(u16 delay_ms);
void    Motor_Control_Emergency_Stop();
void    Motor_Control_Backward(u16 delay);
void    Motor_Control_Forward(u16 delay);
void    set_Motor_Speed(void);

void    Start_Bot(void);
void    Stop_Bot(u16 arg);

void    Enable_ADC(void);
void    Disable_ADC(void);

#endif