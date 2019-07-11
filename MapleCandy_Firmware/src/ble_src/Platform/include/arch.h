/**
 ****************************************************************************************
 *
 * @file        arch.h
 *
 * @brief       This file contains the definitions of the macros and functions that are
 *              architecture dependent.  The implementation of those is implemented in the
 *              appropriate architecture directory.
 *
 * Copyright (C) RivieraWaves 2009-2012
 *
 * Copyright(C) 2013-2014 Renesas Electronics Corporation
 *
 * $Rev: 2171 $
 *
 ****************************************************************************************
 */

#ifndef _ARCH_H_
#define _ARCH_H_

/** @defgroup PLATFORM_DRIVERS PLATFORM_DRIVERS
 * @ingroup BLEREFIP
 * @brief Declaration of the platform architecture API.
 * @{
 */

// required to define GLOBAL_INT_** macros as inline assembly
#include "compiler.h"
#include "ll.h"

/*
 * CPU Clock
 ****************************************************************************************
 */
// MCU Clock is 8MHz
#define CLK_HOCO_8MHZ
//#define CLK_HOCO_16MHZ
//#define CLK_HOCO_32MHZ


/// @}
#endif // _ARCH_H_
