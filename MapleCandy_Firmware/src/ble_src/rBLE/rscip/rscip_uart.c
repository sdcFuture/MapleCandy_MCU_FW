/**
 ****************************************************************************************
 *
 * @file		rscip_uart.c
 *
 * @brief	RSCIP UART transport module functions.
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
#include "rscip_uart.h"
#include "rscip_cntl.h"


/*
 * DEFINES
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

RSCIP_Uart_t		uart[RSCIP_WINDOW_SIZE + 1];
RSCIP_Uart_t		*uart_rp;
RSCIP_Uart_t		*uart_wp;

RSCIP_Uart_Ctr_t	uart_ctr;

uint8_t		uart_rx_buf;
uint8_t		uart_tx_buf[(RSCIP_MAX_PAYLOAD_SIZE + RSCIP_PACKET_HEADER_SIZE + RSCIP_CHECKSUM_DATA_SIZE + 1) * 2];

static	RSCIP_UART_STATE	rscip_tx_state;
static	RSCIP_UART_STATE	rscip_rx_state;
static	uint8_t		esc_flag;
static	uint8_t		check_sum;
static	uint16_t	packet_length;

static RSCIP_UART_STATE	last_rscip_rx_state;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */


/******************************************************************************************/
/**
 *  @brief UART initializing process
 *
 *  An UART is initialized for this function.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Initialization of a UART driver.
 *    -# Initialization of a variable.
 *    -# UART reception start.
 *
 *  @param[in]
 *
 *  @retval
 */
/******************************************************************************************/

BOOL RSCIP_Uart_Init(void)
{
	BOOL		Ret;
	uint8_t		i;

	Ret = serial_init();
	if ( TRUE == Ret ) {
	
		for (i = 0; i < (RSCIP_WINDOW_SIZE + 1); i++) {
			uart[i].rx_flag = FALSE;
		}
	
		uart_ctr.rp = 0;
		uart_ctr.wp = 0;
	
		tx_done.app  = FALSE;
		tx_done.uart = FALSE;
	
	    //	initialize tx state
	    rscip_tx_state = RSCIP_UART_STATE_TX_IDLE;
		//start uart reception
		RSCIP_Uart_Read_Start();
        last_rscip_rx_state = RSCIP_UART_STATE_RX_RESET;
        rscip_rx_state      = RSCIP_UART_STATE_RX_RESET;
	}
	return( Ret );
}


/******************************************************************************************/
/**
 *  @brief UART request to send processing
 *
 *  This function does a request-to-send to a UART driver.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *
 *  @param[in]		*headerptr		header buffer address
 *  @param[in]		*bufptr			packet buffer address
 *  @param[in]		checksum		checksum data
 *  @param[in]		size			The number of send data.
 *
 *  @retval
 */
/******************************************************************************************/

void RSCIP_Uart_Write(uint8_t *headerptr, uint8_t *bufptr, uint8_t checksum, uint16_t size)
{
	uint16_t	i;
	uint16_t	tx_data_count;


	if (RSCIP_UART_STATE_TX_IDLE != rscip_tx_state) {
		return;
	}

	tx_data_count = 1;
	uart_tx_buf[0] = END_CODE;

	if (NULL != headerptr) {
		for (i = 0; i < RSCIP_PACKET_HEADER_SIZE; i++, tx_data_count++) {
			switch (headerptr[i]) {
				case END_CODE:
					uart_tx_buf[tx_data_count++] = ESC_CODE;
					uart_tx_buf[tx_data_count]   = ESC_END_CODE;
					break;
				case ESC_CODE:
					uart_tx_buf[tx_data_count++] = ESC_CODE;
					uart_tx_buf[tx_data_count]   = ESC_ESC_CODE;
					break;
				default:
					uart_tx_buf[tx_data_count]   = headerptr[i];
					break;
			}
		}

		for (i = 0; i < size; i++, tx_data_count++) {
			switch (bufptr[i]) {
				case END_CODE:
					uart_tx_buf[tx_data_count++] = ESC_CODE;
					uart_tx_buf[tx_data_count]   = ESC_END_CODE;
					break;
				case ESC_CODE:
					uart_tx_buf[tx_data_count++] = ESC_CODE;
					uart_tx_buf[tx_data_count]   = ESC_ESC_CODE;
					break;
				default:
					uart_tx_buf[tx_data_count]   = bufptr[i];
					break;
			}
		}

		if (RSCIP_INTEGRITY_TYPE_ON == ((rscip.config & RSCIP_INTEGRITY_TYPE_MASK) >> RSCIP_INTEGRITY_TYPE_SIFT)) {
			switch (checksum)
			{
				case END_CODE:
					uart_tx_buf[tx_data_count++] = ESC_CODE;
					uart_tx_buf[tx_data_count++] = ESC_END_CODE;
					break;
				case ESC_CODE:
					uart_tx_buf[tx_data_count++] = ESC_CODE;
					uart_tx_buf[tx_data_count++] = ESC_ESC_CODE;
					break;
				default:
					uart_tx_buf[tx_data_count++] = checksum;
					break;
			}
		}
	}
	else {
		for (i = 0, tx_data_count = 1; i < size; i++, tx_data_count++) {
			switch (bufptr[i]) {
				case END_CODE:
					uart_tx_buf[tx_data_count++] = ESC_CODE;
					uart_tx_buf[tx_data_count]   = ESC_END_CODE;
					break;
				case ESC_CODE:
					uart_tx_buf[tx_data_count++] = ESC_CODE;
					uart_tx_buf[tx_data_count]   = ESC_ESC_CODE;
					break;
				default:
					uart_tx_buf[tx_data_count]   = bufptr[i];
					break;
			}
		}
	}

	uart_tx_buf[tx_data_count++] = END_CODE;

	rscip_tx_state = RSCIP_UART_STATE_TX_ONGOING;
	serial_write(uart_tx_buf, tx_data_count);
}


/******************************************************************************************/
/**
 *  @brief UART reception request processing
 *
 *  This function does a reception request to a UART driver.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *
 *  @param[in]
 *
 *  @retval
 */
/******************************************************************************************/

void RSCIP_Uart_Read_Start(void)
{
    //	initialize rx state
    rscip_rx_state = RSCIP_UART_STATE_RX_END_SEARCH;
	esc_flag = FALSE; 

	//	Set the UART environment to message type 1 byte reception
    serial_read(&uart_rx_buf, 1);
}


/******************************************************************************************/
/**
 *  @brief UART transmission completion processing
 *
 *  This function does transmission completion processing of a UART driver.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *
 *  @param[in]
 *
 *  @retval	
 */
/******************************************************************************************/

void RSCIP_Uart_Tx_Done(void)
{

	rscip_tx_state = RSCIP_UART_STATE_TX_IDLE;

    // Defer the freeing of resources to ensure that it is done in background
	tx_done.uart = TRUE;
    ke_evt_set( EVT_RSCIP_TX_DONE_BIT );
}


/******************************************************************************************/
/**
 *  @brief UART reception completion processing
 *
 *  This function does reception completion processing of a UART driver.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *
 *  @param[in]
 *
 *  @retval
 */
/******************************************************************************************/

void RSCIP_Uart_Rx_Done(void)
{
    last_rscip_rx_state = rscip_rx_state;
	switch (rscip_rx_state) {

		case RSCIP_UART_STATE_RX_END_SEARCH:
		case RSCIP_UART_STATE_RX_RESET:

			if (END_CODE == uart_rx_buf) {
				rscip_rx_state = RSCIP_UART_STATE_RX_START;
			}
			serial_read(&uart_rx_buf, 1);
			break;

		case RSCIP_UART_STATE_RX_START:

			if (END_CODE == uart_rx_buf) {
				rscip_rx_state = RSCIP_UART_STATE_RX_HEADER0;
				serial_read(&uart_rx_buf, 1);
			}
			else {
				uart_wp = &uart[uart_ctr.wp];
				if (TRUE == uart_wp->rx_flag) {
					RSCIP_Uart_Read_Start();
					return;
				}

				if (TRUE == esc_flag) {
					esc_flag = FALSE;
					if (ESC_END_CODE == uart_rx_buf) {
						uart_rx_buf = END_CODE;
					}
					else {
						uart_rx_buf = ESC_CODE;
					}
				}
				else {
					if (ESC_CODE == uart_rx_buf) {
						esc_flag = TRUE;
						serial_read(&uart_rx_buf, 1);
						return;
					}
				}

				uart_wp->rx_header_buf[0] = uart_rx_buf;
				check_sum = uart_rx_buf;
				rscip_rx_state = RSCIP_UART_STATE_RX_HEADER1;
				serial_read(&uart_rx_buf, 1);
			}
			break;

		case RSCIP_UART_STATE_RX_HEADER0:

			uart_wp = &uart[uart_ctr.wp];
			if (TRUE == uart_wp->rx_flag) {
				RSCIP_Uart_Read_Start();
				return;
			}

			if (TRUE == esc_flag) {
				esc_flag = FALSE;
				if (ESC_END_CODE == uart_rx_buf) {
					uart_rx_buf = END_CODE;
				}
				else {
					uart_rx_buf = ESC_CODE;
				}
			}
			else {
				if (ESC_CODE == uart_rx_buf) {
					esc_flag = TRUE;
					serial_read(&uart_rx_buf, 1);
					return;
				}
			}

			uart_wp->rx_header_buf[0] = uart_rx_buf;
			check_sum = uart_rx_buf;
			rscip_rx_state = RSCIP_UART_STATE_RX_HEADER1;
			serial_read(&uart_rx_buf, 1);
			break;

		case RSCIP_UART_STATE_RX_HEADER1:

			if (TRUE == esc_flag) {
				esc_flag = FALSE;
				if (ESC_END_CODE == uart_rx_buf) {
					uart_rx_buf = END_CODE;
				}
				else {
					uart_rx_buf = ESC_CODE;
				}
			}
			else {
				if (ESC_CODE == uart_rx_buf) {
					esc_flag = TRUE;
					serial_read(&uart_rx_buf, 1);
					return;
				}
			}

			uart_wp->rx_header_buf[1] = uart_rx_buf;
			check_sum = (uint8_t)(check_sum + uart_rx_buf);
			packet_length = (uint16_t)(uart_rx_buf & RSCIP_PAYLOAD_LENGTH_MASK);
			rscip_rx_state = RSCIP_UART_STATE_RX_HEADER2;
			serial_read(&uart_rx_buf, 1);
			break;

		case RSCIP_UART_STATE_RX_HEADER2:

			if (TRUE == esc_flag) {
				esc_flag = FALSE;
				if (ESC_END_CODE == uart_rx_buf) {
					uart_rx_buf = END_CODE;
				}
				else {
					uart_rx_buf = ESC_CODE;
				}
			}
			else {
				if (ESC_CODE == uart_rx_buf) {
					esc_flag = TRUE;
					serial_read(&uart_rx_buf, 1);
					return;
				}
			}

			uart_wp->rx_header_buf[2] = uart_rx_buf;
			check_sum = (uint8_t)(check_sum + uart_rx_buf);
			packet_length = (uint16_t)((packet_length << RSCIP_PAYLOAD_LENGTH_SIFT) + uart_rx_buf);
			rscip_rx_state = RSCIP_UART_STATE_RX_HEADER3;
			serial_read(&uart_rx_buf, 1);
			break;

		case RSCIP_UART_STATE_RX_HEADER3:

			if (TRUE == esc_flag) {
				esc_flag = FALSE;
				if (ESC_END_CODE == uart_rx_buf) {
					uart_rx_buf = END_CODE;
				}
				else {
					uart_rx_buf = ESC_CODE;
				}
			}
			else {
				if (ESC_CODE == uart_rx_buf) {
					esc_flag = TRUE;
					serial_read(&uart_rx_buf, 1);
					return;
				}
			}

			uart_wp->rx_header_buf[3] = uart_rx_buf;
			check_sum = (uint8_t)(check_sum + uart_rx_buf);

			if (0x00 == check_sum) {
				if (0 == packet_length) {
					rscip_rx_state = RSCIP_UART_STATE_RX_END;
					serial_read(&uart_rx_buf, 1);
				}
				else {
					if (RSCIP_INTEGRITY_CHECK == (uart_wp->rx_header_buf[0] & RSCIP_INTEGRITY_CHECK_MASK)) {
						packet_length = (uint16_t)(packet_length + RSCIP_CHECKSUM_DATA_SIZE);
					}
					rscip_rx_state = RSCIP_UART_STATE_RX_PAYLOAD;
					serial_read(uart_wp->rx_buf, ( uint16_t ) (packet_length + 1));
				}
			}
			else {	//	Header Check Sum Error
				RSCIP_Uart_Read_Start();
			}

			break;

		case RSCIP_UART_STATE_RX_PAYLOAD:

			if (END_CODE == uart_wp->rx_buf[packet_length]) {

				uart_wp->rx_length = packet_length;
				uart_wp->rx_flag = TRUE;

				uart_ctr.wp += 1;
				if ((RSCIP_WINDOW_SIZE + 1) <= uart_ctr.wp) {
					uart_ctr.wp = 0;
				}

				//	initialize rx state
				rscip_rx_state = RSCIP_UART_STATE_RX_START;
				esc_flag = FALSE;
				//Set the UART environment to message type 1 byte reception
				serial_read(&uart_rx_buf, 1);

				// Defer the freeing of resources to ensure that it is done in background
				ke_evt_set( EVT_RSCIP_RX_DONE_BIT );
			}
			else {
				packet_length += 1;
				serial_read(&uart_wp->rx_buf[packet_length], 1);
			}
			break;

		case RSCIP_UART_STATE_RX_END:

			if (END_CODE == uart_rx_buf) {

				uart_wp->rx_length = packet_length;
				uart_wp->rx_flag = TRUE;

				uart_ctr.wp += 1;
				if ((RSCIP_WINDOW_SIZE + 1) <= uart_ctr.wp) {
					uart_ctr.wp = 0;
				}

				//	initialize rx state
				rscip_rx_state = RSCIP_UART_STATE_RX_START;
				esc_flag = FALSE;
				//Set the UART environment to message type 1 byte reception
				serial_read(&uart_rx_buf, 1);

				// Defer the freeing of resources to ensure that it is done in background
				ke_evt_set( EVT_RSCIP_RX_DONE_BIT );
			}
			else {
				RSCIP_Uart_Read_Start();
			}
			break;

		default:
			//	error
			RSCIP_Uart_Read_Start();
			break;
	}
}


/******************************************************************************************/
/**
 *  @brief UART reception error handling
 *
 *  This function does an error handling of a UART driver.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# A reception request is performed to a UART driver.
 *
 *  @param[in]
 *
 *  @retval	
 */
/******************************************************************************************/

void RSCIP_Uart_Rx_Error(void)
{
	RSCIP_Uart_Read_Start();
}

/******************************************************************************************/
/**
 *  @brief State check of a RSCIP driver
 *
 *  This function judges whether a RSCIP is in the state of after acquisition packet of header.@n
 *  
 *  @param[in]
 *
 *  @retval			true:		in the state of after acquisition packet of header
 *					false:		in the other state
 */
/******************************************************************************************/

bool RSCIP_Uart_Rx_FirstByte(void)
{
	if((RSCIP_UART_STATE_RX_START      == last_rscip_rx_state) &&
	   (RSCIP_UART_STATE_RX_END_SEARCH !=      rscip_rx_state))
	{
		return TRUE;
	}
	return FALSE;
}
/******************************************************************************************/
/**
 *  @brief State check of a RSCIP driver
 *
 *  This function judges whether a RSCIP is in the packet complete state.@n
 *  
 *  @param[in]
 *
 *  @retval			true:		in the packet complete state
 *					false:		in the other state
 */
/******************************************************************************************/

bool RSCIP_Uart_Rx_Complete(void)
{
	if((RSCIP_UART_STATE_RX_START ==      rscip_rx_state) &&
	   (RSCIP_UART_STATE_RX_START != last_rscip_rx_state) &&
	   (RSCIP_UART_STATE_RX_RESET != last_rscip_rx_state)
	   )
	{
		return TRUE;
	}
	return FALSE;
}

/******************************************************************************************/
/**
 *  @brief State check of a RSCIP driver
 *
 *  This function judges whether a RSCIP screwdriver is in the sleep possible state.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# A transmission/receiving condition is checked.
 *
 *  @param[in]
 *
 *  @retval			true:		sleep OK
 *					false:		sleep NG
 */
/******************************************************************************************/

bool RSCIP_Can_Sleep(void)
{
	if (RSCIP_UART_STATE_TX_IDLE != rscip_tx_state)
	{
		return	FALSE;
	}
	if ((RSCIP_UART_STATE_RX_START      == rscip_rx_state) ||
		(RSCIP_UART_STATE_RX_END_SEARCH == rscip_rx_state) ||
		(RSCIP_UART_STATE_RX_RESET      == rscip_rx_state))
	{
		return	TRUE;
	}
	return	FALSE;
}
