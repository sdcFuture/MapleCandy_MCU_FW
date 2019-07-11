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
* File Name    : r_cg_12da.c
* Version      : Code Generator for RL78/L1A V1.01.02.03 [24 May 2018]
* Device(s)    : R5F11MMF
* Tool-Chain   : CCRL
* Description  : This file implements device driver for DA module.
* Creation Date: 4/1/2019
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_12da.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_12DA_Create
* Description  : This function initializes the DA converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_12DA_Create(void)
{
    DACEN = 1U;     /* enables input clock supply */
    DADPR = _00_DAC_DATA_RIGHT;
    DAVREFCR = _01_DAC_REF_VOLTAGE_AVDD_AVSS;
    DAADSCR = _00_DAC_ADC_CONVERSION_DISABLE;
    /* Set ANO0 pin */
    PMC2 |= 0x01U;
    PM2 |= 0x01U;
    /* Set ANO1 pin */
    PMC10 |= 0x01U;
    PM10 |= 0x01U;
}
/***********************************************************************************************************************
* Function Name: R_12DA0_Start
* Description  : This function enables the DA0 converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_12DA0_Start(void)
{
    DACR |= _40_DAC_ANO0_OUTPUT_ENABLE;
}
/***********************************************************************************************************************
* Function Name: R_12DA1_Start
* Description  : This function enables the DA1 converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_12DA1_Start(void)
{
    DACR |= _80_DAC_ANO1_OUTPUT_ENABLE;
}
/***********************************************************************************************************************
* Function Name: R_12DA0_Stop
* Description  : This function stops the DA0 converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_12DA0_Stop(void)
{
    DACR &= (uint8_t)~(_40_DAC_ANO0_OUTPUT_ENABLE);
}
/***********************************************************************************************************************
* Function Name: R_12DA1_Stop
* Description  : This function stops the DA1 converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_12DA1_Stop(void)
{
    DACR &= (uint8_t)~(_80_DAC_ANO1_OUTPUT_ENABLE);
}
/***********************************************************************************************************************
* Function Name: R_12DA0_Set_ConversionValue
* Description  : This function sets the DA0 converter value.
* Arguments    : reg_value -
*                    value of conversion
* Return Value : None
***********************************************************************************************************************/
void R_12DA0_Set_ConversionValue(uint16_t reg_value)
{
    DADR0 = (uint16_t)(reg_value & 0x0FFFU);
}
/***********************************************************************************************************************
* Function Name: R_12DA1_Set_ConversionValue
* Description  : This function sets the DA1 converter value.
* Arguments    : reg_value -
*                    value of conversion
* Return Value : None
***********************************************************************************************************************/
void R_12DA1_Set_ConversionValue(uint16_t reg_value)
{
    DADR1 = (uint16_t)(reg_value & 0x0FFFU);
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
