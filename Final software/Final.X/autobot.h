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


# define CAT(a, ...)                    a ## __VA_ARGS__
# define TMP(a, ...)                    CAT(a, __VA_ARGS__)
# define ISR_IPL(p)                     CAT(TMP(IPL, p), SOFT)

# define STR(p)                         #p

#endif