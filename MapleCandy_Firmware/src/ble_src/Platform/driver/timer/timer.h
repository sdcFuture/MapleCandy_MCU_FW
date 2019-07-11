/**
 ****************************************************************************************
 *
 * @file		timer.h
 *
 * @brief	timer control module headder file
 *
 * Copyright(C) 2013-2016 Renesas Electronics Corporation
 *
 * $Rev: 2144 $
 *
 ****************************************************************************************
 */

#ifndef	__RBLE_TIMER_INC__
#define	__RBLE_TIMER_INC__

#include "rble.h"
#include "uart.h"

/**************************** define Declaration **************************************/
typedef void ( *RBLE_FUNC )( void );									/* rBLE Call Function */

typedef enum Timer_enum {
	_RBLE_TIMER_ID_RSCIP,
	_RBLE_TIMER_ID_APP,
#if (SERIAL_U_DIV_2WIRE)
	_RBLE_TIMER_ID_SERIAL,
#endif
    _RBLE_TIMER_ID_RBLES,               /* BLE Simple API */
    _RBLE_TIMER_ID_RBLES_ADV_DUR,       /* BLE Simple API */
    _RBLE_TIMER_ID_RBLES_SCAN_DUR,      /* BLE Simple API */
    _RBLE_TIMER_ID_RBLES_CONNECT_DUR,   /* BLE Simple API */
	_RBLE_TIMER_MAX
} TIMER_ENUM;

/**************************** structure Declaration ***********************************/
/**************************** PROTOTYPE Declaration ***********************************/
#ifdef __cplusplus
extern "C" {
#endif
RBLE_STATUS RBLE_Init_Timer( void );								/* Timer Initialize */
RBLE_STATUS RBLE_Set_Timer( RBLE_FUNC Func, uint16_t Timer_Cnt );	/* Timer Set */
RBLE_STATUS RBLE_Clear_Timer( void );								/* Timer Clear */
RBLE_STATUS RBLE_Set_Timer_Id( int_t Id, RBLE_FUNC Func, uint16_t Timer_Cnt );/* Timer Set */
RBLE_STATUS RBLE_Clear_Timer_Id( int_t Id );							/* Timer Clear */
void rBLE_Is_Timeup_Call_Func( void );								/* Timeup Function Call */
void RBLE_Terminate_Timer( void );									/* Timer Terminate */
BOOL rBLE_Timer_Can_Sleep( void );
#ifdef __cplusplus
}
#endif

#endif	// __RBLE_TIMER_INC__
