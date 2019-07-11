/**
 ****************************************************************************************
 *
 * @file		rble_trans.h
 *
 * @brief	Header file for rBLE communication.
 *
 * Copyright(C) 2013-2014 Renesas Electronics Corporation
 *
 * $Rev: 4206 $
 *
 ****************************************************************************************
 */

#ifndef	__RBLE_TRAN_INC__
#define	__RBLE_TRAN_INC__

#include "types.h"
#include "rble.h"
#include "rble_api.h"

/**************************** Define Declaration **************************************/
#define RBLE_HEADER_SIZE			4
#define RBLE_SUBHEADER_SIZE			4
#define RBLE_PACKET_TYPE_COMMAND	1
#define RBLE_PACKET_TYPE_EVENT		2

#define RBLE_MAX_PAYLOARD_SIZE		128

#define RBLE_API_OVER_PACKET_MASKBIT16	0x8000

/**************************** Structure Declaration ***********************************/

/* rBLE Command Event Header structure */
typedef struct{
	uint8_t		packet_type;
	uint8_t		packet_length;
	uint16_t	opcode;
	uint8_t		data[RBLE_MAX_PAYLOARD_SIZE - RBLE_HEADER_SIZE];		/* Data (the variable length) */
}RBLE_HEADER_t;

typedef struct{
	uint8_t		packet_type;
	uint8_t		packet_length;
	uint16_t	opcode;
	/*------------------------*/
	uint8_t		packet_no;
	uint8_t		packet_end;
	uint16_t	data_length;
	uint8_t		data[RBLE_MAX_PAYLOARD_SIZE - RBLE_HEADER_SIZE - RBLE_SUBHEADER_SIZE];		/* Data (the variable length) */
}RBLE_SUBHEADER_t;

#endif // __RBLE_TRAN_INC__

