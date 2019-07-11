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
* File Name    : r_cg_12adc_user.c
* Version      : Code Generator for RL78/L1A V1.01.02.03 [24 May 2018]
* Device(s)    : R5F11MMF
* Tool-Chain   : CCRL
* Description  : This file implements device driver for ADC module.
* Creation Date: 4/1/2019
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_12adc.h"
/* Start user code for include. Do not edit comment generated here */
//#include "r_cg_vr.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#pragma interrupt r_12adc_interrupt(vect=INTAD)
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_12adc_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_12adc_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
        
	if (MapleParam.App_State == App_BLE_State) {

		/* Stop AD converter */
		R_12ADC_Stop();

		/* Get AD converter result */

		/* P101/ANI00 = set as ADC input for 0-3.3V */
		R_12ADC_Get_ValueResult(ADCHANNEL0, (uint16_t *) &MapleParam.Measure.ADC.g_12adc_0_3V3);

		/* P142/ANI07 = set as ADC input for 0-10V */
		R_12ADC_Get_ValueResult(ADCHANNEL7, (uint16_t *) &MapleParam.Measure.ADC.g_12adc_0_10V);

		/* P143/ANI08 = set as ADC input for 4-20mA */
		R_12ADC_Get_ValueResult(ADCHANNEL8, (uint16_t *) &MapleParam.Measure.ADC.g_12adc_4_20mA);

		/* Start AD converter */
		R_12ADC_Start();
	}

    if (MapleParam.Metal_Detect_State == Metal_Detect_In_Progress_State) {
        /* Stop AD converter */
        R_12ADC_Stop();
        
        //read the charge on the capacitor
        /* P107/ANI04 = set as Metal Detector Output */
        R_12ADC_Get_ValueResult(ADCHANNEL4, (uint16_t *) &MapleParam.Measure.MetalDetector.Output);
        
        MapleParam.Measure.MetalDetector.ADCFinish = 1;
    }
    
    if (MapleParam.Measure.ADC.Flag_get_ADC_0_3V3) {
        /* Stop AD converter */
        R_12ADC_Stop();
        
        /* P101/ANI00 = set as ADC input for 0-3.3V */
        R_12ADC_Get_ValueResult(ADCHANNEL0, (uint16_t *) &MapleParam.Measure.ADC.g_12adc_0_3V3);
                
        MapleParam.Measure.ADC.Flag_get_ADC_0_3V3 = 0;
    }
                    
    if (MapleParam.Measure.ADC.Flag_get_ADC_0_10_V) {
        /* Stop AD converter */
        R_12ADC_Stop();
        
        /* P142/ANI07 = set as ADC input for 0-10V */
        R_12ADC_Get_ValueResult(ADCHANNEL7, (uint16_t * )&MapleParam.Measure.ADC.g_12adc_0_10V);
                
        MapleParam.Measure.ADC.Flag_get_ADC_0_10_V = 0;
    }
    
    if (MapleParam.Measure.ADC.Flag_get_ADC_4_20mA) {
        /* Stop AD converter */
        R_12ADC_Stop();
        
        /* P143/ANI08 = set as ADC input for 4-20mA */
        R_12ADC_Get_ValueResult(ADCHANNEL8, (uint16_t *) &MapleParam.Measure.ADC.g_12adc_4_20mA);
                
        MapleParam.Measure.ADC.Flag_get_ADC_4_20mA = 0;
    }
    
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
