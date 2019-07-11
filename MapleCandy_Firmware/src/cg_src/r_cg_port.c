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
* File Name    : r_cg_port.c
* Version      : Code Generator for RL78/L1A V1.01.02.03 [24 May 2018]
* Device(s)    : R5F11MMF
* Tool-Chain   : CCRL
* Description  : This file implements device driver for Port module.
* Creation Date: 4/16/2019
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
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
* Function Name: R_PORT_Create
* Description  : This function initializes the Port I/O.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_PORT_Create(void)
{
    PMC2 = _34_PMC2_DEFAULT_VALUE | _00_PMCn7_NOT_USE | _00_PMCn6_NOT_USE | _00_PMCn3_NOT_USE | 
           _02_PMCn1_DI_ON | _01_PMCn0_DI_ON;
    PMC10 = _34_PMC10_DEFAULT_VALUE | _80_PMCn7_DI_ON | _00_PMCn6_NOT_USE | _08_PMCn3_DI_ON | _02_PMCn1_DI_ON | 
            _01_PMCn0_DI_ON;
    PFSEG0 = _40_PFSEG06_SEG | _20_PFSEG05_SEG | _00_PFSEG04_PORT | _80_PFSEG0_DEFAULT_VALUE;
    PFSEG1 = _00_PFSEG15_PORT | _00_PFSEG14_PORT | _00_PFSEG13_PORT | _10_PFSEG12_SEG | _0F_PFSEG1_DEFAULT_VALUE;
    PFSEG2 = _40_PFSEG22_SEG | _20_PFSEG21_SEG | _00_PFSEG20_PORT | _00_PFSEG19_PORT | _04_PFSEG18_SEG | 
             _02_PFSEG17_SEG | _01_PFSEG16_SEG | _80_PFSEG2_DEFAULT_VALUE;
    PFSEG3 = _00_PFSEG29_PORT | _08_PFSEG27_SEG | _04_PFSEG26_SEG | _02_PFSEG25_SEG | _D1_PFSEG3_DEFAULT_VALUE;
    PFSEG4 = _80_PFSEG39_SEG | _40_PFSEG38_SEG | _20_PFSEG37_SEG | _10_PFSEG36_SEG | _08_PFSEG35_SEG | 
             _04_PFSEG34_SEG | _00_PFSEG33_PORT | _01_PFSEG4_DEFAULT_VALUE;
    PFSEG5 = _10_PFSEG44_SEG | _08_PFSEG43_SEG | _01_PFSEG40_SEG | _06_PFSEG5_DEFAULT_VALUE;
    ISCLCD = _01_CAPLH_BUFFER_VALID;
    P1 = _10_Pn4_OUTPUT_1;
    P2 = _80_Pn7_OUTPUT_1 | _40_Pn6_OUTPUT_1 | _08_Pn3_OUTPUT_1 | _00_Pn1_OUTPUT_0 | _00_Pn0_OUTPUT_0;
    P3 = _00_Pn7_OUTPUT_0 | _00_Pn6_OUTPUT_0 | _00_Pn5_OUTPUT_0 | _00_Pn2_OUTPUT_0 | _00_Pn1_OUTPUT_0 | 
         _01_Pn0_OUTPUT_1;
    P5 = _00_Pn2_OUTPUT_0 | _00_Pn1_OUTPUT_0 | _01_Pn0_OUTPUT_1;
    P7 = _80_Pn7_OUTPUT_1 | _00_Pn6_OUTPUT_0 | _00_Pn5_OUTPUT_0 | _00_Pn4_OUTPUT_0 | _00_Pn3_OUTPUT_0 | 
         _00_Pn2_OUTPUT_0 | _00_Pn1_OUTPUT_0 | _00_Pn0_OUTPUT_0;
    PU12 = _80_PUn7_PULLUP_ON | _00_PUn6_PULLUP_OFF | _00_PUn5_PULLUP_OFF;
    PM12 = _1F_PM12_DEFAULT_VALUE | _80_PMn7_MODE_INPUT | _40_PMn6_MODE_INPUT | _20_PMn5_MODE_INPUT;
    PM0 = _80_PMn7_MODE_INPUT | _40_PMn6_MODE_INPUT | _20_PMn5_MODE_INPUT | _10_PMn4_MODE_INPUT | 
          _08_PMn3_MODE_INPUT | _04_PMn2_MODE_INPUT | _02_PMn1_MODE_INPUT | _00_PMn0_MODE_OUTPUT;
    PM1 = _EF_PM1_DEFAULT_VALUE | _00_PMn4_MODE_OUTPUT;
    PM2 = _34_PM2_DEFAULT_VALUE | _00_PMn7_MODE_OUTPUT | _00_PMn6_MODE_OUTPUT | _00_PMn3_MODE_OUTPUT | 
          _02_PMn1_MODE_INPUT | _01_PMn0_MODE_INPUT;
    PM3 = _18_PM3_DEFAULT_VALUE | _80_PMn7_MODE_INPUT | _40_PMn6_MODE_INPUT | _20_PMn5_MODE_INPUT | 
          _04_PMn2_MODE_INPUT | _02_PMn1_MODE_INPUT | _00_PMn0_MODE_OUTPUT;
    PM5 = _F8_PM5_DEFAULT_VALUE | _04_PMn2_MODE_INPUT | _02_PMn1_MODE_INPUT | _00_PMn0_MODE_OUTPUT;
    PM7 = _00_PMn7_MODE_OUTPUT | _40_PMn6_MODE_INPUT | _20_PMn5_MODE_INPUT | _10_PMn4_MODE_INPUT | 
          _00_PMn3_MODE_OUTPUT | _00_PMn2_MODE_OUTPUT | _02_PMn1_MODE_INPUT | _01_PMn0_MODE_INPUT;
    PM10 = _34_PM10_DEFAULT_VALUE | _80_PMn7_MODE_INPUT | _00_PMn6_MODE_OUTPUT | _08_PMn3_MODE_INPUT | 
           _02_PMn1_MODE_INPUT | _01_PMn0_MODE_INPUT;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
