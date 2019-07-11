/**
 ****************************************************************************************
 *
 * @file		rscip_ext.h
 *
 * @brief	RSCIP module header file.
 *
 * Copyright(C) 2013-2014 Renesas Electronics Corporation
 *
 * $Rev: 2144 $
 *
 ****************************************************************************************
 */

#ifndef RSCIP_EXT_INC_H
#define RSCIP_EXT_INC_H

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "types.h"
#include	<stdio.h>

/* The function following definition calls to the time of an event in Event_Hdlr in turn is registered. */
#define		EVT_RSCIP_TX_DONE_BIT	0
#define		EVT_RSCIP_RX_DONE_BIT	1

/*
 * DEFINES
 ****************************************************************************************
 */

typedef void (*evt_ptr_t)(void);

#ifndef	evt_field_t
typedef uint32_t evt_field_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif
extern void Event_Init( void );
extern void Event_Exit( void );
extern BOOL ke_evt_set( evt_field_t Event );
extern void ke_evt_clear( evt_field_t Event );

extern BOOL serial_init( void );
extern void serial_exit( void );
extern BOOL serial_write( uint8_t *bufptr, uint16_t size );
extern BOOL serial_read( uint8_t *bufptr, uint16_t size );
#ifdef __cplusplus
}
#endif

/*
 * MACROS
 ****************************************************************************************
 */


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

#endif
