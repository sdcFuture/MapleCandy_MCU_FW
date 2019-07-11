/**
 ****************************************************************************************
 *
 * @file		rscip_uart.h
 *
 * @brief	RSCIP UART transport module header file.
 *
 * Copyright(C) 2013-2014 Renesas Electronics Corporation
 *
 * $Rev: 2144 $
 *
 ****************************************************************************************
 */


#ifndef RSCIP_UART_H_
#define RSCIP_UART_H_


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rscip.h"
#include "rscip_cntl.h"


/*
 * DEFINES
 ****************************************************************************************
 */
#define	_USE_RSCIP

//	SLIP CODE
#define END_CODE		0xC0
#define ESC_CODE		0xDB
#define	ESC_END_CODE	0xDC
#define	ESC_ESC_CODE	0xDD


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

//	RSCIP UART states
typedef enum {
	//	TX STATES
	RSCIP_UART_STATE_TX_ONGOING,
	RSCIP_UART_STATE_TX_IDLE,
	//	RX STATES
	RSCIP_UART_STATE_RX_END_SEARCH,
	RSCIP_UART_STATE_RX_START,
	RSCIP_UART_STATE_RX_HEADER0,
	RSCIP_UART_STATE_RX_HEADER1,
	RSCIP_UART_STATE_RX_HEADER2,
	RSCIP_UART_STATE_RX_HEADER3,
	RSCIP_UART_STATE_RX_PAYLOAD,
	RSCIP_UART_STATE_RX_END,
	RSCIP_UART_STATE_RX_RESET
} RSCIP_UART_STATE;

typedef struct {
	uint8_t		rx_header_buf[RSCIP_PACKET_HEADER_SIZE];
	uint8_t		rx_buf[(RSCIP_MAX_PAYLOAD_SIZE + RSCIP_CHECKSUM_DATA_SIZE) * 2];
	uint16_t	rx_length;
	bool		rx_flag;
} RSCIP_Uart_t;

typedef struct {
	uint8_t		rp;
	uint8_t		wp;
} RSCIP_Uart_Ctr_t;


/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

extern	RSCIP_Uart_t		uart[RSCIP_WINDOW_SIZE + 1];
extern	RSCIP_Uart_t		*uart_rp;
extern	RSCIP_Uart_Ctr_t	uart_ctr;


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif

BOOL RSCIP_Uart_Init(void);
void RSCIP_Uart_Write(uint8_t *headerptr, uint8_t *bufptr, uint8_t checksum, uint16_t size);
void RSCIP_Uart_Read_Start(void);

void RSCIP_Uart_Tx_Done(void);
void RSCIP_Uart_Rx_Done(void);
void RSCIP_Uart_Rx_Error(void);
bool RSCIP_Uart_Rx_FirstByte(void);
bool RSCIP_Uart_Rx_Complete(void);

bool RSCIP_Can_Sleep(void);
#ifdef __cplusplus
}
#endif
#endif	//	RSCIP_UART_H_
