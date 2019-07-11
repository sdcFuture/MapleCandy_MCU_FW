/**
 ****************************************************************************************
 *
 * @file		plf.h
 *
 * @brief       This file contains the Low Level RENESAS RL78 MCU definitions and
 *              initialization prototypes.
 *
 * Copyright (C) RivieraWaves 2009-2012
 *
 * Copyright(C) 2013-2014 Renesas Electronics Corporation
 *
 * $Rev: 2297 $
 *
 ****************************************************************************************
 */

#ifndef PLF_H_
#define PLF_H_

/**
 ****************************************************************************************
 * @addtogroup PLATFORM_DRIVERS
 * @brief Low Level MCU initialisation definitions and functions.
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#ifdef _USE_STD_C99
 #include <stdint.h>
#endif //_USE_STD_C99

/*
 * DEFINES
 ****************************************************************************************
 */

///Option-bytes and security ID
#define OPT_BYTES_SIZE     4
#define SECU_ID_SIZE       10

#define WATCHDOG_DISABLED  0xEE
#define LVD_MODE_OFF       0xFF
#define FRQ_24MHZ          0xE0

#define OCD_DISABLED       0x04
#define OCD_ENABLED_ERASE  0x84
#define OCD_ENABLED        0x85


///Clock Configuration
#define FX_02_10           0x00
#define FX_10_20           0x01

#define LOW_POWER_OSC      0x04
#define XT1_XT2_INP        0x00
#define XT1_XT2_RES        0x10

#define X1_X2_INP1         0x00
#define X1_X2_RES          0x40
#define X1_X2_INP2         0x80
#define X1_X2_EXT          0xC0

#define STAB_TIME_MIN      0x00
#define STAB_TIME_1        0x01
#define STAB_TIME_2        0x02
#define STAB_TIME_3        0x03
#define STAB_TIME_4        0x04
#define STAB_TIME_5        0x05
#define STAB_TIME_6        0x06
#define STAB_TIME_MAX      0x07

#define FREQ_LOW           0x00
#define FREQ_HIGH          0x01

#define OSC_STAB_MAX       0xFF

#define FREQ_OSC           0xF8
#define FREQ_OSC_2         0xF9
#define FREQ_OSC_4         0xFA
#define FREQ_OSC_8         0xFB
#define FREQ_OSC_16        0xFC

/**
 *****************************************************************************************
 * @brief The function which gets PSW status
 *****************************************************************************************
 */
#ifdef _USE_REL_RL78
__istate_t __get_interrupt_state(void);
#endif //_USE_REL_RL78


/// @} module

#endif // PLF_H_
