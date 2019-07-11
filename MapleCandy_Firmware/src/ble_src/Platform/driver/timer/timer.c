/**
 ****************************************************************************************
 *
 * @file		timer.c
 *
 * @brief	Timer 
 *
 * Copyright(C) 2013-2014 Renesas Electronics Corporation
 *
 * $Rev: 2167 $
 *
 ****************************************************************************************
 */

#include "arch.h"
#include "compiler.h"
#include "timer.h"

#include "r_cg_it.h"
#include "r_cg_userdefine.h"

/**************************** define Declaration **************************************/
///TAU Configuration channel 00
#define CK00_FREQ_CPU         0x00
#define CK00_FREQ_CPU_2       0x01
#define CK00_FREQ_CPU_4       0x02
#define CK00_FREQ_CPU_8       0x03
#define CK00_FREQ_CPU_16      0x04
#define CK00_FREQ_CPU_32      0x05
#define CK00_FREQ_CPU_64      0x06
#define CK00_FREQ_CPU_128     0x07
#define CK00_FREQ_CPU_256     0x08
#define CK00_FREQ_CPU_512     0x09
#define CK00_FREQ_CPU_1024    0x0A
#define CK00_FREQ_CPU_2048    0x0B
#define CK00_FREQ_CPU_4096    0x0C
#define CK00_FREQ_CPU_8192    0x0D
#define CK00_FREQ_CPU_16384   0x0E
#define CK00_FREQ_CPU_32768   0x0F

/**************************** structure Declaration ***********************************/
typedef struct {
	uint16_t		Timer_Cnt;
	BOOL			Timer_Flg;					/* Timeup Flg */
	RBLE_FUNC		Timer_Func;
} TIMER_MEM;

/**************************** PRIVATE Memory Declaration ******************************/
TIMER_MEM		rBLE_Timer_Mem[ _RBLE_TIMER_MAX ];
BOOL			rBLE_Timer_Init;

/**************************** PRIVATE PROTOTYPE Declaration ***************************/
static RBLE_STATUS Set_Timer( int_t No, RBLE_FUNC Func, uint16_t Timer_Cnt );
static RBLE_STATUS Clear_Timer( int_t Id );

/******************************* Program Area *****************************************/

/* Timer Inititalize */
RBLE_STATUS RBLE_Init_Timer( void )
{
	RBLE_STATUS		Ret;

	Ret = RBLE_OK;

	if ( false == rBLE_Timer_Init ) {
        /* Enable timer */
        R_IT_Start();

		rBLE_Timer_Init = true;
	}
	return( Ret );
}

/* Timer Set */
RBLE_STATUS RBLE_Set_Timer( RBLE_FUNC Func, uint16_t Timer_Cnt )
{
	return( Set_Timer( _RBLE_TIMER_ID_RSCIP, Func, Timer_Cnt ) );
}

/* Timer Clear */
RBLE_STATUS RBLE_Clear_Timer( void )
{
	return( Clear_Timer( _RBLE_TIMER_ID_RSCIP ) );
}

/* Timer Set */
RBLE_STATUS RBLE_Set_Timer_Id( int_t Id, RBLE_FUNC Func, uint16_t Timer_Cnt )
{
	return( Set_Timer( Id, Func, Timer_Cnt ) );
}

/* Timer Clear */
RBLE_STATUS RBLE_Clear_Timer_Id( int_t Id )
{
	return( Clear_Timer( Id ) );
}

void rBLE_Is_Timeup_Call_Func( void )
{
	RBLE_FUNC	Func_Temp;
	int_t		i;

	for ( i = 0;i < _RBLE_TIMER_MAX;i++ ) {
		if ( 0 != rBLE_Timer_Mem[ i ].Timer_Flg ) {
			rBLE_Timer_Mem[ i ].Timer_Flg = false;
			Func_Temp = rBLE_Timer_Mem[ i ].Timer_Func;
			( Func_Temp )();
		}
	}
}

void RBLE_Terminate_Timer( void )
{
	int_t		i;

	if ( true == rBLE_Timer_Init ) {
        /* Disable timer */
	    R_IT_Stop();
		rBLE_Timer_Init = false;
		for ( i = 0;i < _RBLE_TIMER_MAX;i++ ) {
			Clear_Timer( i );
		}
	}
}

BOOL rBLE_Timer_Can_Sleep( void )
{
	int_t		i;

	for ( i = 0;i < _RBLE_TIMER_MAX;i++ ) {
		if ( 0 != rBLE_Timer_Mem[ i ].Timer_Flg ) {
			return FALSE;
		}
	}
	return TRUE;
}

/* Timer Set */
static RBLE_STATUS Set_Timer( int_t No, RBLE_FUNC Func, uint16_t Timer_Cnt )
{
	RBLE_STATUS		Ret;
	Ret = RBLE_OK;

	if ( false == rBLE_Timer_Init ) {
		/** Error **/
		Ret = RBLE_ERR;
	} else {
		rBLE_Timer_Mem[ No ].Timer_Flg = FALSE;
		rBLE_Timer_Mem[ No ].Timer_Cnt = Timer_Cnt;
		rBLE_Timer_Mem[ No ].Timer_Func = Func;
	}
	return( Ret );
}

/* Timer Clear */
static RBLE_STATUS Clear_Timer( int_t Id )
{
	rBLE_Timer_Mem[ Id ].Timer_Flg = FALSE;
	rBLE_Timer_Mem[ Id ].Timer_Cnt = 0;
	return( RBLE_OK );
}

void timer_isr(void)
{
int_t		i;

	for ( i = 0;i < _RBLE_TIMER_MAX;i++ ) {
		if ( 0 != rBLE_Timer_Mem[ i ].Timer_Cnt ) {
			rBLE_Timer_Mem[ i ].Timer_Cnt--;
			if ( 0 == rBLE_Timer_Mem[ i ].Timer_Cnt ) {
				rBLE_Timer_Mem[ i ].Timer_Flg = true;
			}
		}
	}
}

