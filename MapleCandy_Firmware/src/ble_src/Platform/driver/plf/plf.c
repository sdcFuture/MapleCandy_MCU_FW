/**
 ****************************************************************************************
 *
 * @file plf.c
 *
 * @brief This file contains the Low Level RENESAS RL78 MCU clock initialisation functions.
 *
 * Copyright (C) RivieraWaves 2009-2012
 *
 * $Rev: 2171 $
 *
 * Copyright(C) 2012-2014 Renesas Electronics Corporation
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup PLATFORM_DRIVERS
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "arch.h"
#include "plf.h"

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
#ifdef _USE_IAR_RL78
///Option Byte section definition
__root __far const uint8_t ll_option_bytes[OPT_BYTES_SIZE]  @ 0x00C0=
                                                                       {WATCHDOG_DISABLED,
                                                                        LVD_MODE_OFF,
                                                                        FRQ_24MHZ,
                                                                        OCD_ENABLED, };

///Security Byte Definition
__root __far const uint8_t ll_secu_id[SECU_ID_SIZE] @ 0x00C4=
                                                         { 0xff, 0xff, 0xff, 0xff, 0xff,
                                                           0xff, 0xff, 0xff, 0xff, 0xff };
#endif //_USE_STD_C99


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#ifdef _USE_REL_RL78
/**
 *****************************************************************************************
 * @brief The function which gets PSW status
 *****************************************************************************************
 */
__istate_t __get_interrupt_state(void)
{
    #asm
    mov    a, PSW
    mov    c, a
    #endasm
}
#endif //_USE_REL_RL78


/// @} module
