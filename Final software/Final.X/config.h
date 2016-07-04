/* 
 * File:   config.h
 * Author: Autobots
 *
 * Created on July 1, 2016, 6:07 PM
 */

# define LOG_BUFFER_SIZE                2048

# define PRIORITY_MOTOR                 7
# define PRIORITY_MIC                   6
# define PRIORITY_UART                  5

# define SUBPRIORITY_EMERGENCY          3

# define MOTOR_PWM_PERIOD               1024

# define MOTOR_1_HANDICAP               100
# define MOTOR_2_HANDICAP               100

# define ACTION_TABLE_MAX_LENGTH        16
# define EMERGENCY_STOP_BACKWARD_DELAY  500
# define NEW_RECORD_DELAY               2000
# define SIXTY_DEGREES_TURN_DELAY       1000            //25 * TICK_DELAY_FACTOR
# define TICK_DELAY_FACTOR              40              // SIXTY_DEGREES_TURN_DELAY / 25
# define SIDE_SWIM_DELAY                600

# define MOVEMENT_ACTIVATED             0

# define SOUND_AVERAGE_VALUE_COUNT      64
# define SOUND_THRESHOLD                75
# define MIC_ADC_TICKS_DISTANCE         25

# define SOUND_THRESHOLD_1              75
# define SOUND_THRESHOLD_2              90
# define SOUND_THRESHOLD_3              100
# define SOUND_THRESHOLD_4              150
# define SOUND_THRESHOLD_5              200

# define SOUND_BUFFER_SIZE              64

# define BATTERY_LEVEL_100_MIC3_AVG     (-65 * 128)
# define BATTERY_LEVEL_75_MIC3_AVG     (-75 * 128)
# define BATTERY_LEVEL_50_MIC3_AVG     (-90 * 128)
# define BATTERY_LEVEL_25_MIC3_AVG     (-110 * 128)

# define VERBOSE_PIC_STATUS             1
# define VERBOSE_MOTOR_HARDWARE         0
# define VERBOSE_MOTOR_SOFTWARE         1
# define VERBOSE_MIC_HARDWARE           1
# define VERBOSE_MIC_SOFTWARE           1
