/**
 ****************************************************************************************
 *
 * @file		rscip_cntl.c
 *
 * @brief	RSCIP control module functions.
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
#include "timer.h"

/*
 * MACROS
 ****************************************************************************************
 */


/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

typedef struct
{
	uint8_t		seq_num;
	uint8_t		ack_num;
	uint8_t		integrity_check;
	uint8_t		reliable_packet;
	uint8_t		packet_type;
	uint16_t	payload_length;
} header_t;


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

RSCIP_Ctr_t				rscip;
RSCIP_Callback_t		callback_func;

RSCIP_Write_Packet_t	write_packet[RSCIP_WINDOW_SIZE];
RSCIP_Tx_Packet_Ctr_t	tx_packet_ctr;

RSCIP_Packet_Ctr_t		tx;
RSCIP_Packet_Ctr_t		rx;

RSCIP_Ack_Ctr_t			ack_ctr;

RSCIP_Tx_Done_t			tx_done;

uint8_t			read_packet_buf[RSCIP_MAX_PAYLOAD_SIZE];

RSCIP_Retry_t	retry_count;


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static void RSCIP_Sync_Tx_Done(void);
static void RSCIP_Sync_Res_Tx_Done(void);
static void RSCIP_Config_Tx_Done(void);
static void RSCIP_Config_Res_Tx_Done(void);
static void RSCIP_Packet_Tx_Done(void);
static void RSCIP_Ack_Tx_Done(void);

static void RSCIP_Ack_Rx_Done(uint8_t ack_num);
static void RSCIP_Packet_Rx_Done(header_t *header);
static void RSCIP_Sync_Rx_Done(void);
static void RSCIP_Sync_Res_Rx_Done(void);
static void RSCIP_Config_Rx_Done(void);
static void RSCIP_Config_Res_Rx_Done(uint8_t *link_packet_buf);

static uint16_t RSCIP_Data_Packet_Make(uint8_t *packet_header);
static uint16_t RSCIP_Ack_Packet_Make(uint8_t *link_packet);
static uint16_t RSCIP_Sync_Packet_Make(uint8_t *link_packet);
static uint16_t RSCIP_Sync_Res_Packet_Make(uint8_t *link_packet);
static uint16_t RSCIP_Config_Packet_Make(uint8_t *link_packet);
static uint16_t RSCIP_Config_Res_Packet_Make(uint8_t *link_packet);

static uint8_t calc_checksum(uint8_t *buff_top1, uint16_t size1, uint8_t *buff_top2, uint16_t size2);
static void Uart_Next_Data_Check(void);


/******************************************************************************************/
/**
 *  @brief Transmission completion event processing
 *
 *  This function does a transmission completion event processing.@n
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

void RSCIP_Tx_Done(void)
{
    ke_evt_clear(EVT_RSCIP_TX_DONE_BIT);


	if (TRUE == tx_done.uart) {
		tx_done.uart = FALSE;

		switch (rscip.state) {

			case RSCIP_STATE_SYNC_TX:
				//	SYNC Transmission completion processing
				RSCIP_Sync_Tx_Done();
				break;

			case RSCIP_STATE_SYNC_RES_TX:
				//	SYNC RES Transmission completion processing
				RSCIP_Sync_Res_Tx_Done();
				break;

			case RSCIP_STATE_CONFIG_TX:
				//	CONFIG Transmission completion processing
				RSCIP_Config_Tx_Done();
				break;

			case RSCIP_STATE_CONFIG_RES_TX:
				//	CONFIG RES Transmission completion processing
				RSCIP_Config_Res_Tx_Done();
				break;

			case RSCIP_STATE_PACKET_TX:
				//	Data Packet Transmission completion processing
				RSCIP_Packet_Tx_Done();
				break;

			case RSCIP_STATE_ACK_TX:
				//	ACK Packet Transmission completion processing
				RSCIP_Ack_Tx_Done();
				break;

			default:
				break;
		}
	}
	if (TRUE == tx_done.app) {
		RSCIP_Packet_Tx(RSCIP_REQ_PACKET);
		tx_done.app = FALSE;
	}

}


/******************************************************************************************/
/**
 *  @brief SYNC Transmission completion processing
 *
 *  This function does SYNC Transmission completion processing.@n
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

static void RSCIP_Sync_Tx_Done(void)
{
	if (RSCIP_STATE_NON != rscip.tx_end_state) {
		rscip.state = rscip.tx_end_state;
		rscip.tx_end_state = RSCIP_STATE_NON;
	}
	else {
		rscip.state = RSCIP_STATE_SYNC_RES_WAIT;
	}

	//	Resending timer starting
	RBLE_Set_Timer( RSCIP_Time_Up, TIMER_CNT);

	//	It's confirmed whether there was next request-to-send during a transmission.
	if (RSCIP_REQ_SYNC_RES_MESSAGE == (rscip.req_message & RSCIP_REQ_SYNC_RES_MESSAGE)) {	//	SYNC-RES 
		rscip.req_message &= ~RSCIP_REQ_SYNC_RES_MESSAGE;
		rscip.tx_end_state = RSCIP_STATE_SYNC_RES_WAIT;
		RSCIP_Packet_Tx(RSCIP_REQ_SYNC_RES);
	}

	if (RSCIP_REQ_SYNC_MESSAGE == (rscip.req_message & RSCIP_REQ_SYNC_MESSAGE)) {	//	SYNC 
		rscip.req_message &= ~RSCIP_REQ_SYNC_MESSAGE;
		rscip.tx_end_state = RSCIP_STATE_SYNC_RES_WAIT;
		RSCIP_Packet_Tx(RSCIP_REQ_SYNC);
	}
}


/******************************************************************************************/
/**
 *  @brief SYNC RES Transmission completion processing
 *
 *  This function does SYNC RES Transmission completion processing.@n
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

static void RSCIP_Sync_Res_Tx_Done(void)
{
	if (RSCIP_STATE_NON != rscip.tx_end_state) {
		rscip.state = rscip.tx_end_state;
		rscip.tx_end_state = RSCIP_STATE_NON;
	}
	else {
		rscip.state = RSCIP_STATE_SYNC_RES_WAIT;
	}

	//	It's confirmed whether there was next request-to-send during a transmission.
	if (RSCIP_REQ_SYNC_RES_MESSAGE == (rscip.req_message & RSCIP_REQ_SYNC_RES_MESSAGE)) {	//	SYNC-RES 
		rscip.req_message &= ~RSCIP_REQ_SYNC_RES_MESSAGE;
		rscip.tx_end_state = RSCIP_STATE_SYNC_RES_WAIT;
		RSCIP_Packet_Tx(RSCIP_REQ_SYNC_RES);
	}

	if (RSCIP_REQ_SYNC_MESSAGE == (rscip.req_message & RSCIP_REQ_SYNC_MESSAGE)) {	//	SYNC 
		rscip.req_message &= ~RSCIP_REQ_SYNC_MESSAGE;
		rscip.tx_end_state = RSCIP_STATE_SYNC_RES_WAIT;
		RSCIP_Packet_Tx(RSCIP_REQ_SYNC);
	}

	if (RSCIP_REQ_CONFIG_MESSAGE == (rscip.req_message & RSCIP_REQ_CONFIG_MESSAGE)) {	//	CONFIG 
		rscip.req_message &= ~RSCIP_REQ_CONFIG_MESSAGE;
		rscip.tx_end_state = RSCIP_STATE_CONFIG_RES_WAIT;
		RSCIP_Packet_Tx(RSCIP_REQ_CONFIG);
	}
}


/******************************************************************************************/
/**
 *  @brief CONFIG Transmission completion processing
 *
 *  This function does CONFIG Transmission completion processing.@n
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

static void RSCIP_Config_Tx_Done(void)
{
	if (RSCIP_STATE_NON != rscip.tx_end_state) {
		rscip.state = rscip.tx_end_state;
		rscip.tx_end_state = RSCIP_STATE_NON;
	}
	else {
		rscip.state = RSCIP_STATE_CONFIG_RES_WAIT;
	}

	if (RSCIP_STATE_ACTIVE == rscip.state) {
		//	Active Notice.
		if (NULL != callback_func.active_callback) {
			callback_func.active_callback();
		}
	}

	//	Resending timer starting
	RBLE_Set_Timer( RSCIP_Time_Up, TIMER_CNT);

	//	It's confirmed whether there was next request-to-send during a transmission.
	if (RSCIP_REQ_SYNC_RES_MESSAGE == (rscip.req_message & RSCIP_REQ_SYNC_RES_MESSAGE)) {	//	SYNC-RES 
		rscip.req_message &= ~RSCIP_REQ_SYNC_RES_MESSAGE;
		rscip.tx_end_state = RSCIP_STATE_CONFIG_RES_WAIT;
		RSCIP_Packet_Tx(RSCIP_REQ_SYNC_RES);
	}

	if (RSCIP_REQ_CONFIG_RES_MESSAGE == (rscip.req_message & RSCIP_REQ_CONFIG_RES_MESSAGE)) {	//	CONFIG-RES 
		rscip.req_message &= ~RSCIP_REQ_CONFIG_RES_MESSAGE;
		rscip.tx_end_state = RSCIP_STATE_CONFIG_RES_WAIT;
		RSCIP_Packet_Tx(RSCIP_REQ_CONFIG_RES);
	}
}


/******************************************************************************************/
/**
 *  @brief CONFIG RES Transmission completion processing
 *
 *  This function does CONFIG RES Transmission completion processing.@n
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

static void RSCIP_Config_Res_Tx_Done(void)
{
	if (RSCIP_STATE_NON != rscip.tx_end_state) {
		rscip.state = rscip.tx_end_state;
		rscip.tx_end_state = RSCIP_STATE_NON;
	}
	else {
		rscip.state = RSCIP_STATE_CONFIG_RES_WAIT;
	}

	if (RSCIP_STATE_ACTIVE == rscip.state) {
		//	Active Notice.
		if (NULL != callback_func.active_callback) {
			callback_func.active_callback();
		}
	}

	//	It's confirmed whether there was next request-to-send during a transmission.
	if (RSCIP_REQ_SYNC_RES_MESSAGE == (rscip.req_message & RSCIP_REQ_SYNC_RES_MESSAGE)) {	//	SYNC-RES 
		rscip.req_message &= ~RSCIP_REQ_SYNC_RES_MESSAGE;
		rscip.tx_end_state = RSCIP_STATE_CONFIG_RES_WAIT;
		RSCIP_Packet_Tx(RSCIP_REQ_SYNC_RES);
	}

	if (RSCIP_REQ_CONFIG_RES_MESSAGE == (rscip.req_message & RSCIP_REQ_CONFIG_RES_MESSAGE)) {	//	CONFIG-RES 
		rscip.req_message &= ~RSCIP_REQ_CONFIG_RES_MESSAGE;
		rscip.tx_end_state = RSCIP_STATE_CONFIG_RES_WAIT;
		RSCIP_Packet_Tx(RSCIP_REQ_CONFIG_RES);
	}
}


/******************************************************************************************/
/**
 *  @brief Data Packet Transmission completion processing
 *
 *  This function does Data Packet Transmission completion processing.@n
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

static void RSCIP_Packet_Tx_Done(void)
{
	//	Iinitialize request ?
	if (RSCIP_REQ_UNINITIALIZED == (rscip.req_message & RSCIP_REQ_UNINITIALIZED)) {
		//	Transmission buffer initialize.
		RSCIP_Packet_Ctr_Init();
		//	SYNC Transmission.
		rscip.tx_end_state = RSCIP_STATE_NON;
		RSCIP_Packet_Tx(RSCIP_REQ_SYNC);

		//	Reset Notice.
		if (NULL != callback_func.reset_callback) {
			callback_func.reset_callback();
		}
	}

	if (RSCIP_STATE_NON != rscip.tx_end_state) {
		rscip.state = rscip.tx_end_state;
		rscip.tx_end_state = RSCIP_STATE_NON;
	}
	else {
		rscip.state = RSCIP_STATE_ACK_WAIT;
	}

	//	Resending timer starting
	RBLE_Set_Timer( RSCIP_Time_Up, TIMER_CNT);

	//	It's confirmed whether there was next request-to-send during a transmission.
	if (RSCIP_REQ_CONFIG_RES_MESSAGE == (rscip.req_message & RSCIP_REQ_CONFIG_RES_MESSAGE)) {	//	CONFIG-RES 
		rscip.req_message &= ~RSCIP_REQ_CONFIG_RES_MESSAGE;
		rscip.tx_end_state = RSCIP_STATE_ACTIVE;
		RSCIP_Packet_Tx(RSCIP_REQ_CONFIG_RES);
	}
	if (RSCIP_REQ_ACK_MESSAGE == (rscip.req_message & RSCIP_REQ_ACK_MESSAGE)) {	//	ACK 
		rscip.req_message &= ~RSCIP_REQ_ACK_MESSAGE;
		rscip.tx_end_state = RSCIP_STATE_ACTIVE;
		RSCIP_Packet_Tx(RSCIP_REQ_ACK);
	}
	if (0 != tx_packet_ctr.rsize) {
		RSCIP_Packet_Tx(RSCIP_REQ_PACKET);
	}
}


/******************************************************************************************/
/**
 *  @brief ACK Packet Transmission completion processing
 *
 *  This function does ACK Packet Transmission completion processing.@n
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

static void RSCIP_Ack_Tx_Done(void)
{
	//	Iinitialize request ?
	if (RSCIP_REQ_UNINITIALIZED == (rscip.req_message & RSCIP_REQ_UNINITIALIZED)) {
		//	Transmission buffer initialize.
		RSCIP_Packet_Ctr_Init();
		//	SYNC Transmission.
		rscip.tx_end_state = RSCIP_STATE_NON;
		RSCIP_Packet_Tx(RSCIP_REQ_SYNC);

		//	Reset Notice.
		if (NULL != callback_func.reset_callback) {
			callback_func.reset_callback();
		}
	}

	if (RSCIP_STATE_NON != rscip.tx_end_state) {
		rscip.state = rscip.tx_end_state;
		rscip.tx_end_state = RSCIP_STATE_NON;
	}
	else {
		rscip.state = RSCIP_STATE_ACTIVE;
	}

	//	It's confirmed whether there was next request-to-send during a transmission.
	if (RSCIP_REQ_CONFIG_RES_MESSAGE == (rscip.req_message & RSCIP_REQ_CONFIG_RES_MESSAGE)) {	//	CONFIG-RES 
		rscip.req_message &= ~RSCIP_REQ_CONFIG_RES_MESSAGE;
		rscip.tx_end_state = RSCIP_STATE_ACTIVE;
		RSCIP_Packet_Tx(RSCIP_REQ_CONFIG_RES);
	}
	if (RSCIP_REQ_ACK_MESSAGE == (rscip.req_message & RSCIP_REQ_ACK_MESSAGE)) {	//	ACK 
		rscip.req_message &= ~RSCIP_REQ_ACK_MESSAGE;
		rscip.tx_end_state = RSCIP_STATE_ACTIVE;
		RSCIP_Packet_Tx(RSCIP_REQ_ACK);
	}
	if (0 != tx_packet_ctr.rsize) {
		RSCIP_Packet_Tx(RSCIP_REQ_PACKET);
	}
}


/******************************************************************************************/
/**
 *  @brief Reception complete event processing.
 *
 *  This function does a reception complete event processing.@n
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

void RSCIP_Rx_Done(void)
{
	header_t	header;
	uint8_t		link_packet_buf[RSCIP_CONFIG_PACKET_SIZE + RSCIP_CONFIG_FIELD_LENGTH];
	uint16_t	message;
	uint16_t	i;
	uint8_t		*dst_buf_ptr;
	uint16_t	esc_code_count;


	ke_evt_clear(EVT_RSCIP_RX_DONE_BIT);

	uart_rp = &uart[uart_ctr.rp];
	if (FALSE == uart_rp->rx_flag) {
		return;
	}

	header.seq_num = (uint8_t)((uart_rp->rx_header_buf[0] & RSCIP_SEQ_NUM_MASK) >> RSCIP_SEQ_NUM_SIFT);
	header.ack_num = (uint8_t)((uart_rp->rx_header_buf[0] & RSCIP_ACK_NUM_MASK) >> RSCIP_ACK_NUM_SIFT);
	header.integrity_check = (uint8_t)(uart_rp->rx_header_buf[0] & RSCIP_INTEGRITY_CHECK_MASK);
	header.reliable_packet = (uint8_t)(uart_rp->rx_header_buf[0] & RSCIP_RELIABLE_PACKET_MASK);
	header.packet_type = (uint8_t)((uart_rp->rx_header_buf[1] & RSCIP_PACKET_TYPE_MASK) >> RSCIP_PACKET_TYPE_SIFT);
	header.payload_length = (uint16_t)(uart_rp->rx_header_buf[1] & RSCIP_PAYLOAD_LENGTH_MASK);
	header.payload_length = (uint16_t)((header.payload_length << RSCIP_PAYLOAD_LENGTH_SIFT) + uart_rp->rx_header_buf[2]);


	if (0 != header.payload_length) {
		if (RSCIP_PACKET_TYPE_LINK == header.packet_type) {
			dst_buf_ptr = link_packet_buf;
		}
		else {
			dst_buf_ptr = read_packet_buf;
		}

		for (i=0; i<RSCIP_PACKET_HEADER_SIZE; i++) {
			*dst_buf_ptr++ = uart_rp->rx_header_buf[i];
		}

		esc_code_count = 0;
		for (i=0; i<uart_rp->rx_length; i++) {
			if (ESC_CODE == uart_rp->rx_buf[i]) {
				esc_code_count += 1;

				i++;
				switch (uart_rp->rx_buf[i]) {
					case ESC_END_CODE:
						*dst_buf_ptr++ = END_CODE;
						break;
					case ESC_ESC_CODE:
						*dst_buf_ptr++ = ESC_CODE;
						break;
					default:
						uart_rp->rx_flag = FALSE;
						Uart_Next_Data_Check();
						return;
				}
			}
			else {
				*dst_buf_ptr++ = uart_rp->rx_buf[i];
			}
		}

		uart_rp->rx_flag = FALSE;
		if (RSCIP_INTEGRITY_CHECK == header.integrity_check) {
			if ((uart_rp->rx_length - esc_code_count) != (header.payload_length + RSCIP_CHECKSUM_DATA_SIZE)) {
				//	error
				Uart_Next_Data_Check();
				return;
			}
		}
		else {
			if ((uart_rp->rx_length - esc_code_count) != header.payload_length) {
				//	error
				Uart_Next_Data_Check();
				return;
			}
		}
	}
	else {
		uart_rp->rx_flag = FALSE;
	}

	switch (header.packet_type) {

		case RSCIP_PACKET_TYPE_ACK:

			if ((0 != header.seq_num) || (0 != header.integrity_check) || (0 != header.reliable_packet) || (0 != header.payload_length)) {
				//	error Packet
				Uart_Next_Data_Check();
				return;
			}
			//	ACK Packet Receive processing.
			RSCIP_Ack_Rx_Done(header.ack_num);
			break;

		case RSCIP_PACKET_TYPE_COMMAND:
		case RSCIP_PACKET_TYPE_ACL:
		case RSCIP_PACKET_TYPE_SYNC:
		case RSCIP_PACKET_TYPE_EVENT:
		case RSCIP_PACKET_TYPE_RBLE_COMMAND:
		case RSCIP_PACKET_TYPE_RBLE_EVENT:
		case RSCIP_PACKET_TYPE_VENDOR:
			//	Data Packet Receive processing.
			RSCIP_Packet_Rx_Done(&header);
			break;

		case RSCIP_PACKET_TYPE_LINK:

			if ((0 != header.seq_num) || (0 != header.ack_num) || (0 != header.integrity_check) || (0 != header.reliable_packet)) {
				//	error
				Uart_Next_Data_Check();
				return;
			}

			message  = (uint16_t)link_packet_buf[RSCIP_PACKET_HEADER_SIZE];
			message |= (uint16_t)link_packet_buf[RSCIP_PACKET_HEADER_SIZE + 1] << RSCIP_PAYLOAD_LENGTH_SIFT;

			switch (message) {

				case RSCIP_SYNC_MESSAGE:

					if (RSCIP_SYNC_PAYLOAD_LENGTH == header.payload_length) {
						//	SYNC Receive processing.
						RSCIP_Sync_Rx_Done();
					}
					break;

				case RSCIP_SYNC_RES_MESSAGE:

					if (RSCIP_SYNC_RES_PAYLOAD_LENGTH == header.payload_length) {
						//	SYNC RES Receive processing.
						RSCIP_Sync_Res_Rx_Done();
					}
					break;

				case RSCIP_CONFIG_MESSAGE:

					if (RSCIP_CONFIG_PAYLOAD_LENGTH == header.payload_length) {
						//	CONFIG Receive processing.
						RSCIP_Config_Rx_Done();
					}
					break;

				case RSCIP_CONFIG_RES_MESSAGE:

					if (RSCIP_CONFIG_RES_PAYLOAD_LENGTH + RSCIP_CONFIG_FIELD_LENGTH == header.payload_length) {
						//	CONFIG RES Receive processing.
						RSCIP_Config_Res_Rx_Done(link_packet_buf);
					}
					break;

				default:
					break;
			}
			break;

		default:
			break;
	}
	Uart_Next_Data_Check();
}


/******************************************************************************************/
/**
 *  @brief Reception data check processing
 *
 *  This function renews a lead index and checks whether there is a receive data.@n
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

static void Uart_Next_Data_Check(void)
{
	uart_ctr.rp += 1;
	if ((RSCIP_WINDOW_SIZE + 1) <= uart_ctr.rp) {
		uart_ctr.rp = 0;
	}

	if (TRUE == uart[uart_ctr.rp].rx_flag) {
		ke_evt_set( EVT_RSCIP_RX_DONE_BIT );
	}
}


/******************************************************************************************/
/**
 *  @brief ACK Packet Receive processing
 *
 *  This function does ACK Packet receive processing.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *
 *  @param[in]		ack_num			ACK No.
 *
 *  @retval
 */
/******************************************************************************************/

static void RSCIP_Ack_Rx_Done(uint8_t ack_num)
{
	uint8_t		i;
	uint8_t		j;

	switch (rscip.state) {
		case RSCIP_STATE_SYNC_TX:
		case RSCIP_STATE_SYNC_RES_TX:
			//	Resending timer stop.
			RBLE_Clear_Timer();

			//	SYNC transmission is reserved.
			rscip.req_message |= RSCIP_REQ_SYNC_MESSAGE;
			break;

		case RSCIP_STATE_SYNC_WAIT:
		case RSCIP_STATE_SYNC_RES_WAIT:
			//	Resending timer stop.
			RBLE_Clear_Timer();

			//	SYNC Transmission.
			RSCIP_Packet_Tx(RSCIP_REQ_SYNC);
			break;

		case RSCIP_STATE_ACK_WAIT:

			for (i=0,j=0; i<ack_ctr.count; i++) {
				if (ack_num == ack_ctr.buf[i]) {
					j = (uint8_t)(i + 1);
					break;
				}
			}

			if (0 != j) {
				for (i=0; i<(ack_ctr.count-j); i++) {
					ack_ctr.buf[i] = ack_ctr.buf[j+i];
				}
				for (i=0; j != 0; i++,j--) {
					//	notify data transmission completion.
					if (NULL != write_packet[tx_packet_ctr.ap].bufp){
						uint8_t *buf;
						buf = write_packet[tx_packet_ctr.ap].bufp;

						write_packet[tx_packet_ctr.ap].bufp = NULL;
						callback_func.write_callback(buf);
						ack_ctr.count -= 1;
						//	Transmission buffer release
						tx_packet_ctr.ap += 1;
						if (RSCIP_WINDOW_SIZE <= tx_packet_ctr.ap) {
							tx_packet_ctr.ap = 0;
						}
					}
				}

				if (tx_packet_ctr.ap == tx_packet_ctr.rp) {
					//	Retry counter cleara
					retry_count.ack = 0;
					//	Resending timer stop.
					RBLE_Clear_Timer();

					//	transfer to a standby.
					rscip.state = RSCIP_STATE_ACTIVE;
				}
			}
			else {
				//	When receiving the ACK which isn't expected, there is no resended data in hand, so Packet Destruction.
			}
			break;

		case RSCIP_STATE_ACTIVE:
			//	There is no resended data in hand, so Packet Destruction.
			break;

		case RSCIP_STATE_PACKET_TX:
		case RSCIP_STATE_ACK_TX:

			for (i=0,j=0; i<ack_ctr.count; i++) {
				if (ack_num == ack_ctr.buf[i]) {
					j = (uint8_t)(i + 1);
					break;
				}
			}

			if (0 != j) {
				for (i=0; i<(ack_ctr.count-j); i++) {
					ack_ctr.buf[i] = ack_ctr.buf[j+i];
				}
				for (i=0; j != 0; i++,j--) {
					//	notify data transmission completion.
					if (NULL != write_packet[tx_packet_ctr.ap].bufp){
						uint8_t *buf;
						buf = write_packet[tx_packet_ctr.ap].bufp;
						write_packet[tx_packet_ctr.ap].bufp = NULL;

						callback_func.write_callback(buf);
						ack_ctr.count -= 1;
						//	Transmission buffer release
						tx_packet_ctr.ap += 1;
						if (RSCIP_WINDOW_SIZE <= tx_packet_ctr.ap) {
							tx_packet_ctr.ap = 0;
						}
					}
				}

				if (tx_packet_ctr.ap == tx_packet_ctr.rp) {
					//	Retry counter clear
					retry_count.ack = 0;
					//	Resending timer stop.
					RBLE_Clear_Timer();
				}
			}
			else {
				//	When receiving the ACK which isn't expected, there is no resended data in hand, so Packet  Destruction.
			}
			break;

		case RSCIP_STATE_CONFIG_TX:
		case RSCIP_STATE_CONFIG_RES_TX:
		case RSCIP_STATE_CONFIG_RES_WAIT:
		default:
			//	Packet Destruction
			break;
	}
}


/******************************************************************************************/
/**
 *  @brief Data Packet Receive processing
 *
 *  This function does Data Packet receive processing.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *
 *  @param[in]		*header			Received Header Packet buffer address
 *
 *  @retval
 */
/******************************************************************************************/

static void RSCIP_Packet_Rx_Done(header_t *header)
{
	uint8_t		i;
	uint8_t		j;
	uint8_t		check_data;
	uint8_t		checksum;
	uint8_t		callback_req;
	bool		ack_req;


	switch (rscip.state) {
		case RSCIP_STATE_SYNC_TX:
		case RSCIP_STATE_SYNC_RES_TX:
			//	SYNC transmission is reserved.
			rscip.req_message |= RSCIP_REQ_SYNC_MESSAGE;
			break;

		case RSCIP_STATE_SYNC_WAIT:
		case RSCIP_STATE_SYNC_RES_WAIT:
			//	SYNC Transmission.
			RSCIP_Packet_Tx(RSCIP_REQ_SYNC);
			break;

		case RSCIP_STATE_ACTIVE:
		case RSCIP_STATE_ACK_WAIT:
		case RSCIP_STATE_PACKET_TX:
		case RSCIP_STATE_ACK_TX:

			if (RSCIP_INTEGRITY_CHECK == header->integrity_check) {
				check_data = read_packet_buf[header->payload_length + RSCIP_PACKET_HEADER_SIZE];
				checksum = calc_checksum(NULL, 0, read_packet_buf, ( uint16_t )( header->payload_length + RSCIP_PACKET_HEADER_SIZE ) );
				if (check_data != checksum) {
					//	error(Packet Destruction)
					return;
				}
			}

			if (RSCIP_RELIABLE_PACKET != header->reliable_packet) {
				//	error(Packet Destruction)
				return;
			}

			//	It's confirmed whether seq_num is the expectation value.
			if (header->seq_num == rx.seq_num) {
				callback_req = TRUE;

				if ( 0 != ack_ctr.count) {
					for (i=0,j=0; i<ack_ctr.count; i++) {
						if (header->ack_num == ack_ctr.buf[i]) {
							j = (uint8_t)(i + 1);
							break;
						}
					}

					if (0 != j) {
						for (i=0; i<(ack_ctr.count-j); i++) {
							ack_ctr.buf[i] = ack_ctr.buf[j+i];
						}
						for (i=0; j != 0; i++,j--) {
							//	notify data transmission completion.
							if (NULL != write_packet[tx_packet_ctr.ap].bufp){
								uint8_t *buf;
								buf = write_packet[tx_packet_ctr.ap].bufp;
								write_packet[tx_packet_ctr.ap].bufp = NULL;

								callback_func.write_callback(buf);
								ack_ctr.count -= 1;
								//	Transmission buffer release
								tx_packet_ctr.ap += 1;
								if (RSCIP_WINDOW_SIZE <= tx_packet_ctr.ap) {
									tx_packet_ctr.ap = 0;
								}
							}
						}

						if (tx_packet_ctr.ap == tx_packet_ctr.rp) {
							//	Retry counter cleara
							retry_count.ack = 0;
							//	Resending timer stop.
							RBLE_Clear_Timer();

							//	transfer to a standby.
							if (RSCIP_STATE_ACK_WAIT == rscip.state) {
								rscip.state = RSCIP_STATE_ACTIVE;
							}
						}
					}
					else {
						//	When receiving the ACK which isn't expected, there is no resended data in hand, so Packet  Destruction.
//						callback_req = FALSE;
					}
				}
			}
			else {
				//	error(ACK NO. of the current state is sent.)
				callback_req = FALSE;
			}

			ack_req = FALSE;
			if (TRUE == callback_req) {
				//	notify of data reception complete.
				ack_req = callback_func.read_callback( ( RSCIP_PACKET_TYPE )header->packet_type, &read_packet_buf[RSCIP_PACKET_HEADER_SIZE], header->payload_length);
				if (TRUE == ack_req) {
					rx.seq_num += 1;
					if (RSCIP_PACKET_MAX < rx.seq_num) {
						rx.seq_num = 0;
					}
					tx.ack_num = (uint8_t)(header->seq_num + 1);
					if (RSCIP_PACKET_MAX < tx.ack_num) {
						tx.ack_num = 0;
					}
				}
			}
			else {
				uint8_t windowSize = (uint8_t )((rscip.config & RSCIP_WINDOW_SIZE_MASK) >> RSCIP_WINDOW_SIZE_SIFT);

				if (rx.seq_num > header->seq_num) {
					if ((rx.seq_num - header->seq_num) <= windowSize) {
						ack_req = TRUE;
					}
				}
				else {
					if (((rx.seq_num + RSCIP_PACKET_MAX) - header->seq_num) <=  windowSize) {
						ack_req = TRUE;
					}
				}
				if (TRUE == ack_req) {
					tx.ack_num = (uint8_t)(header->seq_num + 1);
					if (RSCIP_PACKET_MAX < tx.ack_num) {
						tx.ack_num = 0;
					}
				}
			}

			if (TRUE == ack_req) {
				if ((RSCIP_STATE_PACKET_TX == rscip.state) || (RSCIP_STATE_ACK_TX == rscip.state)) {
					//	ACK Transmission reservation.
					rscip.req_message |= RSCIP_REQ_ACK_MESSAGE;
				}
				else {
					//	ACK Transmission
					RSCIP_Packet_Tx(RSCIP_REQ_ACK);
				}
			}

			break;

		case RSCIP_STATE_CONFIG_TX:
		case RSCIP_STATE_CONFIG_RES_TX:
		case RSCIP_STATE_CONFIG_RES_WAIT:
		default:
			//	Packet Destruction
			break;
	}
}


/******************************************************************************************/
/**
 *  @brief SYNC Receive processing
 *
 *  This function does SYNC receive processing.@n
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

static void RSCIP_Sync_Rx_Done(void)
{
	switch (rscip.state) {
		case RSCIP_STATE_SYNC_TX:
		case RSCIP_STATE_SYNC_RES_TX:
		case RSCIP_STATE_CONFIG_TX:
		case RSCIP_STATE_CONFIG_RES_TX:
			//	SYNC-RES transmission is reserved.
			rscip.req_message |= RSCIP_REQ_SYNC_RES_MESSAGE;
			break;

		case RSCIP_STATE_SYNC_RES_WAIT:
			//	SYNC RES Transmission
			RSCIP_Packet_Tx(RSCIP_REQ_SYNC_RES);
			break;

		case RSCIP_STATE_CONFIG_RES_WAIT:
			//	SYNC RES Transmission
			RSCIP_Packet_Tx(RSCIP_REQ_SYNC_RES);
			rscip.tx_end_state = RSCIP_STATE_CONFIG_RES_WAIT;
			break;

		case RSCIP_STATE_ACTIVE:
		case RSCIP_STATE_ACK_WAIT:
			//	Transmission buffer initialize.
			RSCIP_Packet_Ctr_Init();
			//	SYNC Transmission.
			rscip.tx_end_state = RSCIP_STATE_NON;
			RSCIP_Packet_Tx(RSCIP_REQ_SYNC);

			//	Reset Notice.
			if (NULL != callback_func.reset_callback) {
				callback_func.reset_callback();
			}
			break;

		case RSCIP_STATE_PACKET_TX:
		case RSCIP_STATE_ACK_TX:
			//	Initialize request
			rscip.req_message |= RSCIP_REQ_UNINITIALIZED;
			break;

		default:
			break;
	}
}


/******************************************************************************************/
/**
 *  @brief SYNC RES Receive processing
 *
 *  This function does SYNC RES receive processing.@n
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

static void RSCIP_Sync_Res_Rx_Done(void)
{
	//	Resending timer stop.
	RBLE_Clear_Timer();

	retry_count.sync_res = 0;

	switch (rscip.state) {
		case RSCIP_STATE_SYNC_TX:
			break;

		case RSCIP_STATE_SYNC_WAIT:
			//	SYNC Transmission.
			RSCIP_Packet_Tx(RSCIP_REQ_SYNC);
			break;

		case RSCIP_STATE_SYNC_RES_TX:
			//	SYNC RES Transmission -> CONFIG transmission is reserved.
			rscip.req_message |= RSCIP_REQ_CONFIG_MESSAGE;
			break;

		case RSCIP_STATE_SYNC_RES_WAIT:
			//	CONFIG Transmission
			RSCIP_Packet_Tx(RSCIP_REQ_CONFIG);
			break;

		case RSCIP_STATE_CONFIG_TX:
		case RSCIP_STATE_CONFIG_RES_TX:
		case RSCIP_STATE_CONFIG_RES_WAIT:
		case RSCIP_STATE_ACTIVE:
		case RSCIP_STATE_PACKET_TX:
		case RSCIP_STATE_ACK_TX:
		case RSCIP_STATE_ACK_WAIT:
			//	Packet Destruction
			break;

		default:
			break;
	}
}


/******************************************************************************************/
/**
 *  @brief CONFIG Receive processing
 *
 *  This function does CONFIG receive processing.@n
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

static void RSCIP_Config_Rx_Done(void)
{
	switch (rscip.state) {
		case RSCIP_STATE_SYNC_TX:
		case RSCIP_STATE_SYNC_RES_TX:
			//	SYNC transmission is reserved.
			rscip.req_message |= RSCIP_REQ_SYNC_MESSAGE;
			break;

		case RSCIP_STATE_SYNC_WAIT:
		case RSCIP_STATE_SYNC_RES_WAIT:
			//	SYNC Transmission.
			RSCIP_Packet_Tx(RSCIP_REQ_SYNC);
			break;

		case RSCIP_STATE_CONFIG_TX:
		case RSCIP_STATE_CONFIG_RES_TX:
			//	CONFIG-RES transmission is reserved.
			rscip.req_message |= RSCIP_REQ_CONFIG_RES_MESSAGE;
			break;

		case RSCIP_STATE_CONFIG_RES_WAIT:
			//	CONFIG RES Transmission
			RSCIP_Packet_Tx(RSCIP_REQ_CONFIG_RES);
			break;

		case RSCIP_STATE_ACTIVE:
		case RSCIP_STATE_ACK_WAIT:
			//	CONFIG-RES Transmission
			RSCIP_Packet_Tx(RSCIP_REQ_CONFIG_RES);
			break;

		case RSCIP_STATE_PACKET_TX:
		case RSCIP_STATE_ACK_TX:
			//	CONFIG-RES transmission is reserved.
			rscip.req_message |= RSCIP_REQ_CONFIG_RES_MESSAGE;
			break;

		default:
			break;
	}
}


/******************************************************************************************/
/**
 *  @brief CONFIG RES Receive processing
 *
 *  This function does CONFIG RES receive processing.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *
 *  @param[in]		*link_packet_buf	Received Link Packet buffer address
 *
 *  @retval
 */
/******************************************************************************************/

static void RSCIP_Config_Res_Rx_Done(uint8_t *link_packet_buf)
{
	//	Resending timer stop.
	RBLE_Clear_Timer();

	retry_count.config_res = 0;

	switch (rscip.state) {
		case RSCIP_STATE_SYNC_TX:
		case RSCIP_STATE_SYNC_RES_TX:
			//	SYNC transmission is reserved.
			rscip.req_message |= RSCIP_REQ_SYNC_MESSAGE;
			break;

		case RSCIP_STATE_SYNC_WAIT:
		case RSCIP_STATE_SYNC_RES_WAIT:
			//	SYNC Transmission.
			RSCIP_Packet_Tx(RSCIP_REQ_SYNC);
			break;

		case RSCIP_STATE_CONFIG_TX:
		case RSCIP_STATE_CONFIG_RES_TX:
			rscip.config = link_packet_buf[RSCIP_PACKET_HEADER_SIZE + 2];

			//	Sliding Window Size
			if (RSCIP_WINDOW_SIZE < ((rscip.config & RSCIP_WINDOW_SIZE_MASK) >> RSCIP_WINDOW_SIZE_SIFT)) {
				//	error
				rscip.error_state = RSCIP_CONFIG_ERROR;
			}

			//	Data Integrity Check TYpe
			if ((RSCIP_INTEGRITY_TYPE    != ((rscip.config & RSCIP_INTEGRITY_TYPE_MASK) >> RSCIP_INTEGRITY_TYPE_SIFT)) && 
			    (RSCIP_INTEGRITY_TYPE_ON == ((rscip.config & RSCIP_INTEGRITY_TYPE_MASK) >> RSCIP_INTEGRITY_TYPE_SIFT))) {
				//	error
				rscip.error_state = RSCIP_CONFIG_ERROR;
			}

			if (RSCIP_CONFIG_ERROR != rscip.error_state) {
				//	transfer to a standby.
				rscip.tx_end_state = RSCIP_STATE_ACTIVE;
			}
			else {
				//	Configration Error Notice
				if (NULL != callback_func.error_callback) {
					callback_func.error_callback(RSCIP_CONFIG_ERROR);
				}
				//	It's un-making about processing after error occurrence.
			}
			break;

		case RSCIP_STATE_CONFIG_RES_WAIT:
			rscip.config = link_packet_buf[RSCIP_PACKET_HEADER_SIZE + 2];

			//	Sliding Window Size
			if (RSCIP_WINDOW_SIZE < ((rscip.config & RSCIP_WINDOW_SIZE_MASK) >> RSCIP_WINDOW_SIZE_SIFT)) {
				//	error
				rscip.error_state = RSCIP_CONFIG_ERROR;
			}

			//	Data Integrity Check TYpe
			if ((RSCIP_INTEGRITY_TYPE    != ((rscip.config & RSCIP_INTEGRITY_TYPE_MASK) >> RSCIP_INTEGRITY_TYPE_SIFT)) && 
			    (RSCIP_INTEGRITY_TYPE_ON == ((rscip.config & RSCIP_INTEGRITY_TYPE_MASK) >> RSCIP_INTEGRITY_TYPE_SIFT))) {
				//	error
				rscip.error_state = RSCIP_CONFIG_ERROR;
			}

			if (RSCIP_CONFIG_ERROR != rscip.error_state) {
				//	transfer to a standby.
				rscip.state = RSCIP_STATE_ACTIVE;
				//	Active Notice.
				if (NULL != callback_func.active_callback) {
					callback_func.active_callback();
				}
			}
			else {
				//	Configration Error Notice
				if (NULL != callback_func.error_callback) {
					callback_func.error_callback(RSCIP_CONFIG_ERROR);
				}
				//	It's un-making about processing after error occurrence.
			}
			break;

		case RSCIP_STATE_ACTIVE:
		case RSCIP_STATE_PACKET_TX:
		case RSCIP_STATE_ACK_TX:
		case RSCIP_STATE_ACK_WAIT:
			//	Packet Destruction
			break;

		default:
			break;
	}
}


/******************************************************************************************/
/**
 *  @brief Transmission buffer initializing process
 *
 *  The transmission buffer when an opponent detected a reset thing, is initialized for this function.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Initialization of a variable.
 *
 *  @param[in]
 *
 *  @retval
 */
/******************************************************************************************/

void RSCIP_Packet_Ctr_Init(void)
{
	uint8_t	i;

	tx_packet_ctr.rp = 0;
	tx_packet_ctr.wp = 0;
	tx_packet_ctr.ap = 0;
	tx_packet_ctr.rsize = 0;
	tx_packet_ctr.wsize = 0;

	for (i = 0; i < RSCIP_WINDOW_SIZE; i++) {
		write_packet[i].size    = 0;
		write_packet[i].bufp    = 0;
		write_packet[i].seq_num = 0;
	}

	i =   (RSCIP_WINDOW_SIZE << RSCIP_WINDOW_SIZE_SIFT);
    i =   (RSCIP_INTEGRITY_TYPE << RSCIP_INTEGRITY_TYPE_SIFT);
    i =   (RSCIP_VERSION_NUM << RSCIP_VERSION_NUM_SIFT);



	rscip.config =   (RSCIP_WINDOW_SIZE << RSCIP_WINDOW_SIZE_SIFT)
				   | (RSCIP_INTEGRITY_TYPE << RSCIP_INTEGRITY_TYPE_SIFT)
				   | (RSCIP_VERSION_NUM << RSCIP_VERSION_NUM_SIFT);

	rscip.req_message = 0x00;
	rscip.tx_end_state = RSCIP_STATE_NON;
	rscip.error_state  = RSCIP_OK;

	tx.seq_num = 0;
	tx.ack_num = 0;
	rx.seq_num = 0;
	rx.ack_num = 0;

	ack_ctr.count = 0;
}


/******************************************************************************************/
/**
 *  @brief Retransmission processing by a time coat
 *
 *  This function does retransmission processing by a time coat.@n
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

void RSCIP_Time_Up(void)
{
	RSCIP_STATE	state;

	//	Resending timer stop.
	RBLE_Clear_Timer();

	if (RSCIP_STATE_ACK_TX == rscip.state) {
		state = rscip.tx_end_state;
	}
	else {
		state = rscip.state;
	}

	switch (state) {

		case RSCIP_STATE_SYNC_RES_WAIT:
			//	SYNC Transmission
			retry_count.sync_res += 1;
			if (RSCIP_MAX_RETRY_COUNT < retry_count.sync_res) {
				rscip.error_state = RSCIP_SYNC_TX_ERROR;
				//	SYNC Transmission Error Notice
				if (NULL != callback_func.error_callback) {
					callback_func.error_callback(RSCIP_SYNC_TX_ERROR);
				}
			}
			else {
				RSCIP_Packet_Tx(RSCIP_REQ_SYNC);
			}
			break;

		case RSCIP_STATE_CONFIG_RES_WAIT:
			//	CONFIG Transmission
			retry_count.config_res += 1;
			if (RSCIP_MAX_RETRY_COUNT < retry_count.config_res) {
				rscip.error_state = RSCIP_CONFIG_TX_ERROR;
				//	CONFIG Transmission Error Notice
				if (NULL != callback_func.error_callback) {
					callback_func.error_callback(RSCIP_CONFIG_TX_ERROR);
				}
			}
			else {
				RSCIP_Packet_Tx(RSCIP_REQ_CONFIG);
			}
			break;

		case RSCIP_STATE_ACK_WAIT:
			//	Packet Transmission
			retry_count.ack += 1;
			if (RSCIP_MAX_RETRY_COUNT < retry_count.ack) {
				rscip.error_state = RSCIP_PACKET_TX_ERROR;
				//	Packet Transmission Error Notice
				if (NULL != callback_func.error_callback) {
					callback_func.error_callback(RSCIP_PACKET_TX_ERROR);
				}
			}
			else if ( 0 != ack_ctr.count ) {
				tx_packet_ctr.rp = tx_packet_ctr.ap;
				tx.seq_num = write_packet[tx_packet_ctr.rp].seq_num;
				tx_packet_ctr.rsize += ack_ctr.count;
				ack_ctr.count = 0;

				RSCIP_Packet_Tx(RSCIP_REQ_PACKET);
			}
			break;

		default:
			//	error
			break;
	}
}


/******************************************************************************************/
/**
 *  @brief Packet transmission processing
 *
 *  This function does packet transmission processing.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *
 *  @param[in]		packet_type		packet type
 *
 *  @retval
 */
/******************************************************************************************/

void RSCIP_Packet_Tx(RSCIP_REQ_PACKET_TYPE packet_type)
{
	uint8_t		packet_buf[RSCIP_CONFIG_PACKET_SIZE + RSCIP_CONFIG_FIELD_LENGTH];
	uint16_t	size = 0;
	uint8_t		checksum = 0;

	switch (rscip.state) {
		case RSCIP_STATE_NON:
		case RSCIP_STATE_SYNC_WAIT:
		case RSCIP_STATE_SYNC_RES_WAIT:
		case RSCIP_STATE_CONFIG_RES_WAIT:
		case RSCIP_STATE_ACTIVE:
		case RSCIP_STATE_ACK_WAIT:
			break;
		default:
			switch (packet_type) {
				case RSCIP_REQ_SYNC:
					//	SYNC transmission is reserved.
					rscip.req_message |= RSCIP_REQ_SYNC_MESSAGE;
					break;
				case RSCIP_REQ_CONFIG:
					//	CONFIG transmission is reserved.
					rscip.req_message |= RSCIP_REQ_CONFIG_MESSAGE;
					break;
				default:
					break;
			}
			return;
	}

	packet_buf[0] = 0x00;
	packet_buf[1] = RSCIP_PACKET_TYPE_LINK << RSCIP_PACKET_TYPE_SIFT;

	switch (packet_type) {

		case RSCIP_REQ_PACKET:

			if (NULL != write_packet[tx_packet_ctr.rp].bufp) {
				//	Header Packet make
				size = RSCIP_Data_Packet_Make(packet_buf);

				if (RSCIP_INTEGRITY_TYPE_ON == ((rscip.config & RSCIP_INTEGRITY_TYPE_MASK) >> RSCIP_INTEGRITY_TYPE_SIFT)) {
					checksum = calc_checksum(packet_buf, RSCIP_PACKET_HEADER_SIZE, (write_packet[tx_packet_ctr.rp].bufp), size);
				}
				rscip.state = RSCIP_STATE_PACKET_TX;
				RSCIP_Uart_Write(packet_buf, (write_packet[tx_packet_ctr.rp].bufp), checksum, size);

				write_packet[tx_packet_ctr.rp].seq_num = tx.seq_num;

				tx.seq_num += 1;
				if (RSCIP_PACKET_MAX < tx.seq_num) {
					tx.seq_num = 0;
				}

				ack_ctr.buf[ack_ctr.count]= tx.seq_num;
				ack_ctr.count += 1;

				tx_packet_ctr.rp += 1;
				if (RSCIP_WINDOW_SIZE <= tx_packet_ctr.rp) {
					tx_packet_ctr.rp = 0;
				}

				tx_packet_ctr.rsize -= 1;
			}
			return;

		case RSCIP_REQ_ACK:
			//	ACK Packet make
			size = RSCIP_Ack_Packet_Make(packet_buf);
			rscip.tx_end_state = rscip.state;
			rscip.state = RSCIP_STATE_ACK_TX;
			break;

		case RSCIP_REQ_SYNC:
			//	SYNC Packet make
			size = RSCIP_Sync_Packet_Make(packet_buf);
			rscip.state = RSCIP_STATE_SYNC_TX;
			break;

		case RSCIP_REQ_SYNC_RES:
			//	SYNC RES Packet make
			size = RSCIP_Sync_Res_Packet_Make(packet_buf);
			rscip.state = RSCIP_STATE_SYNC_RES_TX;
			break;

		case RSCIP_REQ_CONFIG:
			//	CONFIG Packet make
			size = RSCIP_Config_Packet_Make(packet_buf);
			rscip.state = RSCIP_STATE_CONFIG_TX;
			break;

		case RSCIP_REQ_CONFIG_RES:
			//	CONFIG RES Packet make
			size = RSCIP_Config_Res_Packet_Make(packet_buf);
			rscip.state = RSCIP_STATE_CONFIG_RES_TX;
			break;

		default:
			//	error
			return;
	}
	//	Packet Transmission
	RSCIP_Uart_Write(NULL, packet_buf, 0, size);
}


/******************************************************************************************/
/**
 *  @brief Header Packet make processing
 *
 *  This function does header packet creation processing.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Header Packet make
 *
 *  @param[in]		*packet_header		Hedda packet buffer address
 *
 *  @retval			packet size
 */
/******************************************************************************************/

static uint16_t RSCIP_Data_Packet_Make(uint8_t *packet_header)
{
	packet_header[0] = (uint8_t)((tx.seq_num << RSCIP_SEQ_NUM_SIFT) | (tx.ack_num << RSCIP_ACK_NUM_SIFT) | RSCIP_RELIABLE_PACKET);
	if (RSCIP_INTEGRITY_TYPE_ON == ((rscip.config & RSCIP_INTEGRITY_TYPE_MASK) >> RSCIP_INTEGRITY_TYPE_SIFT)) {
		packet_header[0] |= RSCIP_INTEGRITY_CHECK;
	}
	packet_header[1] = (uint8_t)(write_packet[tx_packet_ctr.rp].type << RSCIP_PACKET_TYPE_SIFT);
	packet_header[1] |= (uint8_t)(write_packet[tx_packet_ctr.rp].size >> RSCIP_PAYLOAD_LENGTH_SIFT);
	packet_header[2] = (uint8_t)write_packet[tx_packet_ctr.rp].size;
	packet_header[3] = (uint8_t)(~(packet_header[0] + packet_header[1] + packet_header[2]) + 1);	//	Complement of 2

	return (write_packet[tx_packet_ctr.rp].size);
}


/******************************************************************************************/
/**
 *  @brief ACK Packet make processing
 *
 *  This function does ACK packet creation processing.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# ACK Packet make
 *
 *  @param[in]		*link_packet		Link packet buffer address
 *
 *  @retval			packet size
 */
/******************************************************************************************/

static uint16_t RSCIP_Ack_Packet_Make(uint8_t *link_packet)
{
	link_packet[0] = (uint8_t)(0x00 + (tx.ack_num << RSCIP_ACK_NUM_SIFT));
	link_packet[1] = 0x00;
	link_packet[2] = 0x00;
	link_packet[3] = (uint8_t)(~(link_packet[0] + link_packet[1] + link_packet[2]) + 1);	//	Complement of 2

	return (RSCIP_ACK_PACKET_SIZE);
}


/******************************************************************************************/
/**
 *  @brief SYNC Packet make processing
 *
 *  This function does SYNC packet creation processing.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# SYNC Packet make
 *
 *  @param[in]		*link_packet		Link packet buffer address
 *
 *  @retval			packet size
 */
/******************************************************************************************/

static uint16_t RSCIP_Sync_Packet_Make(uint8_t *link_packet)
{
	link_packet[2] = RSCIP_SYNC_PAYLOAD_LENGTH;
	link_packet[3] = (uint8_t)(~(link_packet[0] + link_packet[1] + link_packet[2]) + 1);	//	Complement of 2
	link_packet[4] = ( uint8_t )(RSCIP_SYNC_MESSAGE & RSCIP_MESSAGE_MASK);
	link_packet[5] = RSCIP_SYNC_MESSAGE >> RSCIP_MESSAGE_SIFT;

	return (RSCIP_SYNC_PACKET_SIZE);
}


/******************************************************************************************/
/**
 *  @brief SYNC RES Packet make processing
 *
 *  This function does SYNC RES packet creation processing.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# SYNC RES Packet make
 *
 *  @param[in]		*link_packet		Link packet buffer address
 *
 *  @retval			packet size
 */
/******************************************************************************************/

static uint16_t RSCIP_Sync_Res_Packet_Make(uint8_t *link_packet)
{
	link_packet[2] = RSCIP_SYNC_RES_PAYLOAD_LENGTH;
	link_packet[3] = (uint8_t)(~(link_packet[0] + link_packet[1] + link_packet[2]) + 1);	//	Complement of 2
	link_packet[4] = ( uint8_t )(RSCIP_SYNC_RES_MESSAGE & RSCIP_MESSAGE_MASK);
	link_packet[5] = RSCIP_SYNC_RES_MESSAGE >> RSCIP_MESSAGE_SIFT;

	return (RSCIP_SYNC_RES_PACKET_SIZE);
}


/******************************************************************************************/
/**
 *  @brief CONFIG Packet make processing
 *
 *  This function does CONFIG packet creation processing.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# CONFIG Packet make
 *
 *  @param[in]		*link_packet		Link packet buffer address
 *
 *  @retval			packet size
 */
/******************************************************************************************/

static uint16_t RSCIP_Config_Packet_Make(uint8_t *link_packet)
{
	link_packet[2] = RSCIP_CONFIG_PAYLOAD_LENGTH;
	link_packet[2] += RSCIP_CONFIG_FIELD_LENGTH;
	link_packet[6] = rscip.config;
	link_packet[3] = ( uint8_t )(~(link_packet[0] + link_packet[1] + link_packet[2]) + 1);	//	Complement of 2
	link_packet[4] = ( uint8_t )(RSCIP_CONFIG_MESSAGE & RSCIP_MESSAGE_MASK);
	link_packet[5] = RSCIP_CONFIG_MESSAGE >> RSCIP_MESSAGE_SIFT;

	return (uint16_t)(RSCIP_PACKET_HEADER_SIZE + link_packet[2]);
}


/******************************************************************************************/
/**
 *  @brief CONFIG RES Packet make processing
 *
 *  This function does CONFIG RES packet creation processing.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# CONFIG RES Packet make
 *
 *  @param[in]		*link_packet		Link packet buffer address
 *
 *  @retval			packet size
 */
/******************************************************************************************/

static uint16_t RSCIP_Config_Res_Packet_Make(uint8_t *link_packet)
{
	link_packet[2] = RSCIP_CONFIG_RES_PAYLOAD_LENGTH;
	link_packet[3] = ( uint8_t )(~(link_packet[0] + link_packet[1] + link_packet[2]) + 1);	//	Complement of 2
	link_packet[4] = ( uint8_t )(RSCIP_CONFIG_RES_MESSAGE & RSCIP_MESSAGE_MASK);
	link_packet[5] = RSCIP_CONFIG_RES_MESSAGE >> RSCIP_MESSAGE_SIFT;

	return (uint16_t)(RSCIP_PACKET_HEADER_SIZE + link_packet[2]);
}


/******************************************************************************************/
/**
 *  @brief Timer initializing process
 *
 *  A timer for retransmission processing is initialized for this function.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Initialization of a variable.
 *
 *  @param[in]
 *
 *  @retval
 */
/******************************************************************************************/

void RSCIP_Timer_Init(void)
{
	retry_count.sync_res   = 0;
	retry_count.config_res = 0;
	retry_count.ack        = 0;
}


/******************************************************************************************/
/**
 *  @brief Check sum computation process
 *
 *  This function calculates Check sum.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Checksum operation of buff1 is performed.
 *    -# Checksum operation of buff2 is performed.
 *
 *  @param[in]		*buff_top1			Hedda packet buffer address
 *  @param[in]		size1				Headda size
 *  @param[in]		*buff_top2			Packet buffer address
 *  @param[in]		size2				Packet size
 *
 *  @retval			checksum
 */
/******************************************************************************************/

static uint8_t calc_checksum(uint8_t *buff_top1, uint16_t size1, uint8_t *buff_top2, uint16_t size2)
{
	uint8_t		*rd_p;
	uint8_t		checksum;

	rd_p = buff_top1;
	checksum = 0;

	if (NULL != buff_top1) {
	    while ( size1 != 0 ) {
    	    checksum = ( uint8_t )(checksum + *rd_p);
        	rd_p++;
        	size1--;
		}
	}

	rd_p = buff_top2;
    while ( size2 != 0 ) {
   	    checksum = ( uint8_t )(checksum + *rd_p);
       	rd_p++;
       	size2--;
   	}
    return( checksum );
}

