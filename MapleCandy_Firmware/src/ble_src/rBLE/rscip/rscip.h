/**
 ****************************************************************************************
 *
 * @file		rscip.h
 *
 * @brief	RSCIP module header file.
 *
 * Copyright(C) 2013-2014 Renesas Electronics Corporation
 *
 * $Rev: 2144 $
 *
 ****************************************************************************************
 */

#ifndef RSCIP_H_
#define RSCIP_H_


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rscip_ext.h"


/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * MACROS
 ****************************************************************************************
 */


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

typedef enum {
	RSCIP_OK,
	RSCIP_NOT_ACTIVE,
	RSCIP_INVALID_PARAMTER,
	RSCIP_BUSY,
	RSCIP_ERROR,
	RSCIP_CONFIG_ERROR,
	RSCIP_SYNC_TX_ERROR,
	RSCIP_CONFIG_TX_ERROR,
	RSCIP_PACKET_TX_ERROR,
} RSCIP_RESULT_CODE;

typedef enum {
	RSCIP_HCI_COMMAND			= 1,
	RSCIP_HCI_ACL_DATA			= 2,
	RSCIP_HCI_SYNCHRONOUS_DATA	= 3,
	RSCIP_HCI_EVENT				= 4,
	RSCIP_RBLE_COMMAND			= 5,
	RSCIP_RBLE_EVENT			= 6,
	RSCIP_VENDER_SPECIFIC		= 14,
} RSCIP_PACKET_TYPE;


typedef void (*RSCIP_Active_Callback)(void);
typedef void (*RSCIP_Write_Callback)(
									 uint8_t			*bufptr
									);
typedef	bool (*RSCIP_Read_Callback)(
									RSCIP_PACKET_TYPE	type,
									uint8_t				*bufptr,
									uint16_t			size
									);
typedef	void (*RSCIP_Reset_Callback)(void);
typedef void (*RSCIP_Error_Callback)(
									 RSCIP_RESULT_CODE	error
									);


typedef struct {
	RSCIP_Active_Callback	active_callback;
	RSCIP_Read_Callback		read_callback;
	RSCIP_Write_Callback	write_callback;
	RSCIP_Reset_Callback	reset_callback;
	RSCIP_Error_Callback	error_callback;
} RSCIP_Callback_t;


/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif
RSCIP_RESULT_CODE RSCIP_Initialize(RSCIP_Callback_t *callback);
RSCIP_RESULT_CODE RSCIP_Write(
							  RSCIP_PACKET_TYPE		type,
							  uint8_t				*bufptr,
							  uint16_t				size
							  );

#ifdef __cplusplus
}
#endif

#endif	//	RSCIP_H_
