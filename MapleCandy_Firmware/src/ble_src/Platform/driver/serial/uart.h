/**
 ****************************************************************************************
 *
 * @file		uart.h
 *
 * @brief UART Driver - UART0 of RL78 RENESAS 16b MCU.
 *
 * Copyright (C) RivieraWaves 2009-2012
 *
 * Copyright(C) 2013-2016 Renesas Electronics Corporation
 *
 * $Rev: 2297 $
 *
 ****************************************************************************************
 */

#ifndef _UART_H_
#define _UART_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#ifdef _USE_STD_C99
 #include <stdint.h>
 #include <stdbool.h>
#endif //_USE_STD_C99

/*
 * DEFINES
 ****************************************************************************************
 */
#define SERIAL_U_DIV_2WIRE  (0)

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
BOOL serial_init(void);
void serial_exit(void);
BOOL serial_read(uint8_t *bufptr, uint16_t size);
BOOL serial_write(uint8_t *bufptr, uint16_t size);

/// @}

#endif /* _UART_H_ */
