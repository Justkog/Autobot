/* 
 * File:   config.h
 * Author: Autobots
 *
 * Created on July 1, 2016, 6:07 PM
 */

# define LOG_BUFFER_SIZE                256

# define PRIORITY_MOTOR()                 7
# define PRIORITY_MIC                   6
# define PRIORITY_UART                  5

# define SUBPRIORITY_EMERGENCY          3

# define ACTION_TABLE_MAX_LENGTH        16
# define EMERGENCY_STOP_BACKWARD_DELAY  500

# define SOUND_AVERAGE_VALUE_COUNT      32
# define SOUND_THRESHOLD                200

# define VERBOSE_PIC_STATUS             1
# define VERBOSE_MOTOR_HARDWARE         0
# define VERBOSE_MOTOR_SOFTWARE         1
# define VERBOSE_MIC_HARDWARE           1
# define VERBOSE_MIC_SOFTWARE           1
