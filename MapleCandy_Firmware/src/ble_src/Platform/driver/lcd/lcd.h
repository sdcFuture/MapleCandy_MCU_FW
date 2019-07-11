/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software.  By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : lcd.h
* Version      : 1.00
* Device       : R5F104PJAFB
* Tool Chain   : CA78K0R
* H/W Platform : RSKRL78G14
* Description  : Defines LCD macros and function prototypes.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History      : 21.05.2012 Ver. 1.00 First Release
***********************************************************************************************************************/

/* Multi inclusion prevention macro */
#ifndef _LCD_H
#define _LCD_H

/***********************************************************************************************************************
User Includes (Project level includes)
***********************************************************************************************************************/
/* Defines C99 variable types */
#include "r_cg_macrodriver.h"

/* Defines used within the project scope */
#include "rskrl78G14def.h"

/***********************************************************************************************************************
LCD Macro Defines
***********************************************************************************************************************/

/* RS Register Select pin */
#define RS_PIN                     PORT(0, 6)
/* Display Enable pin */
#define EN_PIN                     PORT(0, 5)

/* Data bus port */
#define DATA_PORT                  P5
/* Bit mask from entire port */
#define DATA_PORT_MASK             0xF0
/* Number of bits data needs to shift */
#define DATA_PORT_SHIFT            4

#define DATA_WR                    1
#define CTRL_WR                    0

/* Set to ensure base delay */
#define DELAY_TIMING               0x08
/* number of lines on the LCD display */
#define NUMB_CHARS_PER_LINE        8
/* Maximum characters per line of LCD display. */
#define MAXIMUM_LINES              2

#define LCD_LINE1                  0
#define LCD_LINE2                  16

/***********************************************************************************************************************
LCD commands - use LCD_write function to write these commands to the LCD.
 **********************************************************************************************************************/
/* Clear LCD display and home cursor */
#define LCD_CLEAR                  0x01
/* move cursor to line 1 */
#define LCD_HOME_L1                0x80
/* move cursor to line 2 */
#define LCD_HOME_L2                0xC0
/* Cursor auto decrement after R/W */
#define CURSOR_MODE_DEC            0x04
/* Cursor auto increment after R/W */
#define CURSOR_MODE_INC            0x06
/* Setup, 4 bits,2 lines, 5X7 */
#define FUNCTION_SET               0x28
/* Display ON with Cursor */
#define LCD_CURSOR_ON              0x0E
/* Display ON with Cursor off */
#define LCD_CURSOR_OFF             0x0C
/* Display on with blinking cursor */
#define LCD_CURSOR_BLINK           0x0D
/*Move Cursor Left One Position */
#define LCD_CURSOR_LEFT            0x10
/* Move Cursor Right One Position */
#define LCD_CURSOR_RIGHT           0x14
/* Turn on the display */
#define LCD_DISPLAY_ON             0x04
/* Set cursor to start of line 2 */
#define LCD_TWO_LINE               0x08

/***********************************************************************************************************************
LCD Function Prototypes
***********************************************************************************************************************/
/* LCD intialisation function prototype */
void Init_LCD(void);
/* LCD string display function prototype */
void Display_LCD(uint8_t position, const char * string);
/* LCD byte write function prototype */
void LCD_write(uint8_t data_or_ctrl, int8_t value);
/* LCD nibble write function prototype */
void LCD_nibble_write(uint8_t data_or_ctrl, int8_t value);
/* Delay function prototype */
void Display_Delay(uint32_t units);

#endif
