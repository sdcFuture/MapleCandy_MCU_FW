/**
 ****************************************************************************************
 *
 * @file		rscip.c
 *
 * @brief	RSCIP module functions.
 *
 * Copyright(C) 2013-2014 Renesas Electronics Corporation
 *
 * $Rev: 2156 $
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rscip.h"
#include "rscip_cntl.h"
#include "rscip_uart.h"


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


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */


/******************************************************************************************/
/**
 *  @brief RSCIP driver initializing process.
 *
 *  A RSCIP driver is initialized for this function.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Initialization of a variable.
 *    -# Initialization of an UART.
 *    -# Initialization of TIMER.
 *    -# A SYNC is sent.
 *
 *  @param[in]	*callback				Initialization Parameter Structure Pointer.
 *
 *  @retval		RSCIP_OK				Success
 *  @retval		RSCIP_INVALID_PARAMTER	Parameter Error
 *  @retval		RSCIP_ERROR				RSCIP driver Error
 */
/******************************************************************************************/

RSCIP_RESULT_CODE RSCIP_Initialize(RSCIP_Callback_t *callback){
	RSCIP_RESULT_CODE	Ret_Status;


	if ((NULL == callback) || (NULL == callback->write_callback) || (NULL == callback->read_callback)) {
		return RSCIP_INVALID_PARAMTER;
	}

	callback_func.active_callback = callback->active_callback;
	callback_func.write_callback  = callback->write_callback;
	callback_func.read_callback	  = callback->read_callback;
	callback_func.reset_callback  = callback->reset_callback;
	callback_func.error_callback  = callback->error_callback;

	//	Initialization of a variable.
	RSCIP_Packet_Ctr_Init();

	//	Initialization of an UART.
	if ( TRUE == RSCIP_Uart_Init() ) {
	
		//	Initialization of TIMER.
		RSCIP_Timer_Init();
	
		rscip.state = RSCIP_STATE_NON;
		RSCIP_Packet_Tx(RSCIP_REQ_SYNC);

		Ret_Status = RSCIP_OK;
	} else {
		Ret_Status = RSCIP_ERROR;
	}

	return( Ret_Status );
}


/******************************************************************************************/
/**
 *  @brief RSCIP driver request to send processing
 *
 *  This function does a request-to-send for a RSCIP driver.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter check.
 *    -# It's already sent data, when having that, it's eliminated.
 *    -# I check whether there is a space in a data buffer for transmissions.
 *    -# A request-to-send is performed.
 *
 *  @param[in]	type					Transmission data type.
 *  @param[in]	*bufptr					Send data buffer address.
 *  @param[in]	size					The number of send data.
 *
 *  @retval		RSCIP_OK				Success
 *  @retval		RSCIP_NOT_ACTIVE		Not Active
 *  @retval		RSCIP_INVALID_PARAMTER	Parameter Error
 *  @retval		RSCIP_ERROR				RSCIP driver Error
 *  @retval		RSCIP_BUSY				Transmission data buffer full
 */
/******************************************************************************************/

RSCIP_RESULT_CODE RSCIP_Write(
							  RSCIP_PACKET_TYPE		type,
							  uint8_t				*bufptr,
							  uint16_t				size	)
{
	uint8_t	i;


	if (RSCIP_STATE_ACTIVE > rscip.state) {
		return	RSCIP_NOT_ACTIVE;
	}

	switch (type) {
		case RSCIP_HCI_COMMAND:
		case RSCIP_HCI_ACL_DATA:
		case RSCIP_HCI_SYNCHRONOUS_DATA:
		case RSCIP_HCI_EVENT:
		case RSCIP_RBLE_COMMAND:
		case RSCIP_RBLE_EVENT:
		case RSCIP_VENDER_SPECIFIC:
			break;
		default:
			return RSCIP_INVALID_PARAMTER;
	}

	if (NULL == bufptr) {
		return RSCIP_INVALID_PARAMTER;
	}
	if ((0 == size) || (RSCIP_MAX_PAYLOAD_SIZE < size)) {
		return RSCIP_INVALID_PARAMTER;
	}

	if (RSCIP_OK != rscip.error_state) {
		return RSCIP_ERROR;
	}

	for (i = 0; i < RSCIP_WINDOW_SIZE; i++) {
		if (NULL == write_packet[i].bufp) {
			if (0 != write_packet[i].size) {
				write_packet[i].size = 0;
				tx_packet_ctr.wsize -= 1;
			}
		}
	}

	if (tx_packet_ctr.wsize >= ((rscip.config & RSCIP_WINDOW_SIZE_MASK) >> RSCIP_WINDOW_SIZE_SIFT)) {	//	tx buffer FULL ?
		return RSCIP_BUSY;
	}

	write_packet[tx_packet_ctr.wp].type = type;
	write_packet[tx_packet_ctr.wp].size = size;
	write_packet[tx_packet_ctr.wp].bufp = bufptr;

	tx_packet_ctr.rsize += 1;
	tx_packet_ctr.wsize += 1;

	tx_packet_ctr.wp += 1;
	if (RSCIP_WINDOW_SIZE <= tx_packet_ctr.wp) {
		tx_packet_ctr.wp = 0;
	}

	//	Request-to-send
	tx_done.app = TRUE;
	ke_evt_set( EVT_RSCIP_TX_DONE_BIT );

	return RSCIP_OK;
}
