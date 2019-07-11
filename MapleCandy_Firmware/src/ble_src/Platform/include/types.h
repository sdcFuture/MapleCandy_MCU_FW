/**
 ****************************************************************************************
 *
 * @file		types.h
 *
 * @brief	type define Sample Source for rBLE Sample Program
 *
 * Copyright(C) 2013-2014 Renesas Electronics Corporation
 *
 * $Rev: 2158 $
 *
 ****************************************************************************************
 */

#ifndef INCLUDED_RBL_CPU_DEPEND_H
#define INCLUDED_RBL_CPU_DEPEND_H

#include "compiler.h"
/* CPU common setting */

#if !defined(_USE_STD_C99) && !defined(_USE_STD_C99_C)
/* type Declaration */
typedef	signed char		int8_t;
#if defined(_USE_REL_RL78)
typedef	signed short	int16_t;
#else
typedef	signed int		int16_t;
#endif
typedef	signed long		int32_t;
typedef	unsigned char	uint8_t;
#if defined(_USE_REL_RL78)
typedef	unsigned short	uint16_t;
#else
typedef	unsigned int	uint16_t;
#endif
typedef	unsigned long	uint32_t;
#endif

#ifndef char_t
#define char_t char
#endif

#if defined(_USE_REL_RL78)
#ifndef	BOOL
#define	BOOL		unsigned char
#endif
#ifndef	bool
#define	bool		BOOL
#else
#endif
#else
#ifndef	BOOL
#define	BOOL		unsigned char
#endif
#ifndef	bool
#define	bool		BOOL
#endif
#endif

#ifndef	TRUE
#define	TRUE		1
#endif

#ifndef	FALSE
#define	FALSE		0
#endif

#endif
