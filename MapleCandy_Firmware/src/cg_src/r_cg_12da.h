/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2016, 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_12da.h
* Version      : Code Generator for RL78/L1A V1.01.02.03 [24 May 2018]
* Device(s)    : R5F11MMF
* Tool-Chain   : CCRL
* Description  : This file implements device driver for DA module.
* Creation Date: 4/1/2019
***********************************************************************************************************************/
#ifndef DA_H
#define DA_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    D/A Control Register (DACR) 
*/
/* D/A output enable 1 bit (DAOE1) */
#define _00_DAC_ANO1_OUTPUT_DISABLE                       (0x00U) /* analog output of channel 1 (ANO1) is disabled */
#define _80_DAC_ANO1_OUTPUT_ENABLE                        (0x80U) /* analog output of channel 1 (ANO1) is enabled */
/* D/A output enable 0 bit (DAOE0) */
#define _00_DAC_ANO0_OUTPUT_DISABLE                       (0x00U) /* analog output of channel 0 (ANO0) is disabled */
#define _40_DAC_ANO0_OUTPUT_ENABLE                        (0x40U) /* analog output of channel 0 (ANO0) is enabled */

/*
    DADRm Format Select Register (DADPR) 
*/
/* DADRm format select bit (DPSEL) */
#define _00_DAC_DATA_RIGHT                                (0x00U) /* data is flush with the right end */
#define _80_DAC_DATA_LEFT                                 (0x80U) /* data is flush with the left end */

/*
    D/A A/D Synchronous Start Control Register (DAADSCR) 
*/
/* D/A A/D synchronous conversion bit (DAADST) */
#define _00_DAC_ADC_CONVERSION_DISABLE                    (0x00U) /* D/A and A/D conversion is disabled */
#define _80_DAC_ADC_CONVERSION_ENABLE                     (0x80U) /* D/A and A/D conversion is enabled */

/*
    D/A VREF Control Register (DAVREFCR) 
*/
/* D/A reference voltage select bit (REF) */
#define _00_DAC_REF_VOLTAGE_NOT                           (0x00U) /* not selected */
#define _01_DAC_REF_VOLTAGE_AVDD_AVSS                     (0x01U) /* AVDD/AVSS */
#define _06_DAC_REF_VOLTAGE_AVREF_VREFOUT_AVREFM          (0x06U) /* AVREFP or VREFOUT, AVREFM */


/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define _1F_DACR_DEFAULT_VALUE                            (0x1FU) /* DACR default value */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_12DA_Create(void);
void R_12DA0_Start(void);
void R_12DA0_Stop(void);
void R_12DA0_Set_ConversionValue(uint16_t reg_value);
void R_12DA1_Start(void);
void R_12DA1_Stop(void);
void R_12DA1_Set_ConversionValue(uint16_t reg_value);
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
