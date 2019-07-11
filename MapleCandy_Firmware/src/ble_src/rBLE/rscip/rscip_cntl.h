/**
 ****************************************************************************************
 *
 * @file		rscip_cntl.h
 *
 * @brief	RSCIP control module header file.
 *
 * Copyright(C) 2013-2014 Renesas Electronics Corporation
 *
 * $Rev: 2144 $
 *
 ****************************************************************************************
 */

#ifndef RSCIP_CNTL_H_
#define RSCIP_CNTL_H_


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rscip.h"
#include "rble_trans.h"


/*
 * DEFINES
 ****************************************************************************************
 */

//	Configuration Field
#define	RSCIP_INTEGRITY_TYPE_OFF		0x00		//	Data Integrity Check TYpe = 0(OFF)
#define	RSCIP_INTEGRITY_TYPE_ON			0x01		//	Data Integrity Check TYpe = 1(ON)

#define RSCIP_WINDOW_SIZE				0x02		//	Sliding Window Size = 2
#define	RSCIP_INTEGRITY_TYPE			RSCIP_INTEGRITY_TYPE_ON
#define	RSCIP_VERSION_NUM				0x00		//	Version Number = 0

#define RSCIP_WINDOW_SIZE_SIFT			5
#define	RSCIP_INTEGRITY_TYPE_SIFT		3
#define	RSCIP_VERSION_NUM_SIFT			0

#define RSCIP_WINDOW_SIZE_MASK			0xE0		//	11100000b
#define	RSCIP_INTEGRITY_TYPE_MASK		0x08		//	00001000b
#define	RSCIP_VERSION_NUM_MASK			0x07		//	00000111b

#define	RSCIP_PACKET_MAX				7


//	Packet Header
#define	RSCIP_INTEGRITY_CHECK			0x02
#define	RSCIP_RELIABLE_PACKET			0x01

#define	RSCIP_SEQ_NUM_MASK				0xE0		//	11100000b
#define	RSCIP_ACK_NUM_MASK				0x1C		//	00011100b
#define	RSCIP_INTEGRITY_CHECK_MASK		0x02		//	00000010b
#define	RSCIP_RELIABLE_PACKET_MASK		0x01		//	00000001b
#define	RSCIP_PACKET_TYPE_MASK			0xF0		//	11110000b
#define	RSCIP_PAYLOAD_LENGTH_MASK		0x0F		//	00001111b

#define RSCIP_SEQ_NUM_SIFT				5
#define RSCIP_ACK_NUM_SIFT				2
#define	RSCIP_PACKET_TYPE_SIFT			4
#define	RSCIP_PAYLOAD_LENGTH_SIFT		8


//	Packet Type
#define	RSCIP_PACKET_TYPE_ACK			0x00		//	Acknowledgement Packets
#define	RSCIP_PACKET_TYPE_COMMAND		0x01		//	HCI Command Packet
#define	RSCIP_PACKET_TYPE_ACL			0x02		//	HCI ACL Data Packet
#define	RSCIP_PACKET_TYPE_SYNC			0x03		//	HCI Synchronous Data Packet
#define	RSCIP_PACKET_TYPE_EVENT			0x04		//	HCI Event Packet
#define RSCIP_PACKET_TYPE_RBLE_COMMAND	0x05		//	RSCIP Command Packet
#define RSCIP_PACKET_TYPE_RBLE_EVENT	0x06		//	RSCIP Event Packet
#define	RSCIP_PACKET_TYPE_VENDOR		0x0E		//	Vendor Specific
#define	RSCIP_PACKET_TYPE_LINK			0x0F		//	Link Control Packet


//	
#define	RSCIP_MAX_PAYLOAD_SIZE			RBLE_MAX_PAYLOARD_SIZE
#define RSCIP_PACKET_HEADER_SIZE		4
#define	RSCIP_CHECKSUM_DATA_SIZE		1
#define	RSCIP_TX_BUF_SIZE				(RSCIP_MAX_PAYLOAD_SIZE * 2)
#define	RSCIP_MAX_RETRY_COUNT			10

#define	RSCIP_SYNC_PAYLOAD_LENGTH		2
#define	RSCIP_SYNC_RES_PAYLOAD_LENGTH	2
#define	RSCIP_CONFIG_FIELD_LENGTH		1
#define	RSCIP_CONFIG_PAYLOAD_LENGTH		2
#define RSCIP_CONFIG_RES_PAYLOAD_LENGTH	2

#define RSCIP_SYNC_PACKET_SIZE			(RSCIP_PACKET_HEADER_SIZE + RSCIP_SYNC_PAYLOAD_LENGTH)
#define RSCIP_SYNC_RES_PACKET_SIZE		(RSCIP_PACKET_HEADER_SIZE + RSCIP_SYNC_RES_PAYLOAD_LENGTH)
#define RSCIP_CONFIG_PACKET_SIZE		(RSCIP_PACKET_HEADER_SIZE + RSCIP_CONFIG_PAYLOAD_LENGTH)
#define	RSCIP_CONFIG_RES_PACKET_SIZE	(RSCIP_PACKET_HEADER_SIZE + RSCIP_CONFIG_RES_PAYLOAD_LENGTH)
#define RSCIP_ACK_PACKET_SIZE			RSCIP_PACKET_HEADER_SIZE


//	Message
#define	RSCIP_SYNC_MESSAGE				0x7E01		//	0x017E
#define	RSCIP_SYNC_RES_MESSAGE			0x7D02		//	0x027D
#define	RSCIP_CONFIG_MESSAGE			0xFC03		//	0x03FC
#define	RSCIP_CONFIG_RES_MESSAGE		0x7B04		//	0x047B

#define	RSCIP_MESSAGE_MASK				0x00FF
#define	RSCIP_MESSAGE_SIFT				8


//	Request Message flag
#define	RSCIP_REQ_SYNC_MESSAGE			0x01
#define	RSCIP_REQ_SYNC_RES_MESSAGE		0x02
#define	RSCIP_REQ_CONFIG_MESSAGE		0x04
#define	RSCIP_REQ_CONFIG_RES_MESSAGE	0x08
#define	RSCIP_REQ_ACK_MESSAGE			0x20
#define	RSCIP_REQ_UNINITIALIZED			0x40


//	Timer
#define	TIMER_CNT		25		//	Timer count(base=10ms)


/*
 * MACROS
 ****************************************************************************************
 */


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

//	RSCIP states
typedef enum {
	RSCIP_STATE_NON,

	RSCIP_STATE_SYNC_TX,
	RSCIP_STATE_SYNC_WAIT,
	RSCIP_STATE_SYNC_RES_TX,
	RSCIP_STATE_SYNC_RES_WAIT,

	RSCIP_STATE_CONFIG_TX,
	RSCIP_STATE_CONFIG_RES_TX,
	RSCIP_STATE_CONFIG_RES_WAIT,

	RSCIP_STATE_ACTIVE,
	RSCIP_STATE_PACKET_TX,
	RSCIP_STATE_ACK_TX,
	RSCIP_STATE_ACK_WAIT,
} RSCIP_STATE;

//	request packet type
typedef enum {
	RSCIP_REQ_SYNC,
	RSCIP_REQ_SYNC_RES,
	RSCIP_REQ_CONFIG,
	RSCIP_REQ_CONFIG_RES,
	RSCIP_REQ_PACKET,
	RSCIP_REQ_ACK,
} RSCIP_REQ_PACKET_TYPE;


typedef struct {
	RSCIP_STATE			state;
	RSCIP_RESULT_CODE	error_state;
	RSCIP_STATE			tx_end_state;
	uint8_t				config;
	uint8_t				req_message;
} RSCIP_Ctr_t;

typedef struct {
	RSCIP_PACKET_TYPE	type;
	uint16_t			size;
	uint8_t				*bufp;
	uint8_t				seq_num;
} RSCIP_Write_Packet_t;

typedef struct {
	uint8_t		rp;
	uint8_t		wp;
	uint8_t		ap;
	uint8_t		rsize;
	uint8_t		wsize;
} RSCIP_Tx_Packet_Ctr_t;

typedef struct {
	uint8_t		seq_num;
	uint8_t		ack_num;
} RSCIP_Packet_Ctr_t;

typedef struct {
	uint8_t		buf[RSCIP_WINDOW_SIZE];
	uint8_t		count;
} RSCIP_Ack_Ctr_t;

typedef struct {
	bool		app;
	bool		uart;
} RSCIP_Tx_Done_t;

typedef struct {
	uint8_t		sync_res;
	uint8_t		config_res;
	uint8_t		ack;
} RSCIP_Retry_t;


/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

extern	RSCIP_Ctr_t				rscip;
extern	RSCIP_Callback_t		callback_func;

extern	RSCIP_Write_Packet_t	write_packet[RSCIP_WINDOW_SIZE];

extern	RSCIP_Tx_Packet_Ctr_t	tx_packet_ctr;

extern	RSCIP_Packet_Ctr_t		tx;
extern	RSCIP_Packet_Ctr_t		rx;
extern	RSCIP_Ack_Ctr_t			ack_ctr;
extern	RSCIP_Tx_Done_t			tx_done;


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif
void RSCIP_Tx_Done(void);
void RSCIP_Rx_Done(void);
void RSCIP_Time_Up(void);

void RSCIP_Timer_Init(void);
void RSCIP_Packet_Ctr_Init(void);
void RSCIP_Packet_Tx(RSCIP_REQ_PACKET_TYPE packet_type);
#ifdef __cplusplus
}
#endif

#endif	//	RSCIP_CNTL_H_
