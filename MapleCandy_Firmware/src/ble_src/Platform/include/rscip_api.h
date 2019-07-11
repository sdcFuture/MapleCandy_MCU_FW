/**
 ****************************************************************************************
 *
 * @file		rscip_api.h
 *
 * @brief	RSCIP control module header file.
 *
 * Copyright(C) 2013-2014 Renesas Electronics Corporation
 *
 * $Rev: 2144 $
 *
 ****************************************************************************************
 */

#ifndef RSCIP_API_H_
#define RSCIP_API_H_


/*
 * INCLUDE FILES
 ****************************************************************************************
 */


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
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

void codeptr RSCIP_Tx_Done(void);
void codeptr RSCIP_Rx_Done(void);

void RSCIP_Uart_Tx_Done(void);
void RSCIP_Uart_Rx_Done(void);
void RSCIP_Uart_Rx_Error(void);

bool RSCIP_Can_Sleep(void);

#endif	//	RSCIP_API_H_
