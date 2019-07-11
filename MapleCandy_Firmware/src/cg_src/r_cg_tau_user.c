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
* File Name    : r_cg_tau_user.c
* Version      : Code Generator for RL78/L1A V1.01.02.03 [24 May 2018]
* Device(s)    : R5F11MMF
* Tool-Chain   : CCRL
* Description  : This file implements device driver for TAU module.
* Creation Date: 4/16/2019
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_tau.h"
/* Start user code for include. Do not edit comment generated here */
#include "r_cg_12da.h"
#include "types.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#pragma interrupt r_tau0_channel0_interrupt(vect=INTTM00)
#pragma interrupt r_tau0_channel1_interrupt(vect=INTTM01)
#pragma interrupt r_tau0_channel2_interrupt(vect=INTTM02)
#pragma interrupt r_tau0_channel3_interrupt(vect=INTTM03)
#pragma interrupt r_tau0_channel4_interrupt(vect=INTTM04)
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */

#define BLE_SEND_DATA_TIME			400
#define USER_SWITCH_POLL_TIME 		10
#define BLINK_RED_LED_COUNT 		6

extern volatile uint8_t        r_disconnect_req;

static volatile uint8_t ButtonPressedDebounce = 0; //must be zero at bootup

/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_tau0_channel0_interrupt
* Description  : This function INTTM00 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel0_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
	if(MapleParam.Timer.counter_microseconds){
		MapleParam.Timer.counter_microseconds--;
	}

    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_tau0_channel1_interrupt
* Description  : This function INTTM01 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel1_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_tau0_channel2_interrupt
* Description  : This function INTTM02 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel2_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_tau0_channel3_interrupt
* Description  : This function INTTM03 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel3_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_tau0_channel4_interrupt
* Description  : This function INTTM04 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel4_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
#if 1
static uint8_t User_Switch_Poll_Counter = USER_SWITCH_POLL_TIME;
static uint16_t BLE_Send_Data_Counter = BLE_SEND_DATA_TIME;
static uint8_t Metal_Detect_Blink_Red_LED_Count = BLINK_RED_LED_COUNT;
static volatile uint8_t ButtonPressedCount = 0;
static volatile uint8_t ButtonReleasedCount = 0;
static volatile uint8_t ButtonReleasedDebounce = 0;
uint32_t CurrButtonReleaseTimeStamp;

uint8_t Button_Count_Threshold = 5;
#ifdef ENABLE_BLINK_LED
	if (MapleParam.LED.Blink_Timer_Counter) {
		MapleParam.LED.Blink_Timer_Counter--;
	} else {
		//Timer expired, toggle LEDs
		if (MapleParam.LED.Blink_Green) {
	        LED_Green_Toggle();
	    } else if (MapleParam.Metal_Detect_State == Metal_Detect_Change_App_State) {
	    	LED_Green_Off();
        	if (Metal_Detect_Blink_Red_LED_Count) {
        		Metal_Detect_Blink_Red_LED_Count--;
        		LED_Red_Toggle();
        	} else {
        		MapleParam.Metal_Detect_State = Metal_Detect_In_Progress_State;
        		Metal_Detect_Blink_Red_LED_Count = BLINK_RED_LED_COUNT;
        	}
        } else if (MapleParam.LED.Blink_Red) {
	    	LED_Red_Toggle();
	    }
		//Reset timer
		MapleParam.LED.Blink_Timer_Counter = LED_BLINK_TIME;
	}
#endif
    //Poll switch every 10 ms
    if (User_Switch_Poll_Counter) {
        User_Switch_Poll_Counter--;
    } else {
        //Test if user button pressed with debouncing
        if ( (ButtonPressedDebounce == 0) && (P7_bit.no1 == 0)) {
        	//Toggle sending data from Maple Candy
        	//r_send_req^= TRUE;
        	ButtonPressedCount++;
            if (ButtonPressedCount >= Button_Count_Threshold) {
            	ButtonPressedDebounce = 1;
            }
        } else {
        	ButtonPressedCount = 0;
        }
        if ( (ButtonPressedDebounce == 1) && (P7_bit.no1 == 1) ) {
            ButtonReleasedCount++;
        	if (ButtonReleasedCount >= Button_Count_Threshold) {
        		ButtonReleasedDebounce = 1;
        		ButtonPressedDebounce = 0;
        	}
        } else {
            ButtonReleasedCount = 0;
        }
        if (ButtonReleasedDebounce == 1) {
        	//Get current time stamp
        	CurrButtonReleaseTimeStamp = getTimeStampMilliseconds();
        	//Toggle App_State
        	//Block application change state under 5 seconds
        	if (CurrButtonReleaseTimeStamp > MapleParam.Timer.TimeStampPrevChangeAppState + 5000) {

				switch(MapleParam.App_State) {
					case App_BLE_State:
						//Disconnect from BlueTooth
						if (MapleParam.BLE_State == BLE_Data_State) {
							r_disconnect_req = TRUE;
						} else if (MapleParam.BLE_State == BLE_Advertising_State) {
							MapleParam.App_State = App_Metal_Detect_State;
							MapleParam.Metal_Detect_State = Metal_Detect_Change_App_State;
							LED_Green_Blink_Off();
						}
						break;
					case App_Metal_Detect_State:
						MapleParam.App_State = App_BLE_State;
						break;
					default:
						break;
				}

        		//Remember time stamp of the last state transition
				MapleParam.Timer.TimeStampPrevChangeAppState = CurrButtonReleaseTimeStamp;
        	}
        	ButtonReleasedDebounce = 0;
        }
        User_Switch_Poll_Counter = USER_SWITCH_POLL_TIME;
    }
#endif


	if(MapleParam.Timer.counter_milliseconds){
		MapleParam.Timer.counter_milliseconds--;
	}

    //Increment time stamp in milliseconds
    MapleParam.Timer.TimeStampMilliseconds++;

    if (BLE_Send_Data_Counter) {
    	BLE_Send_Data_Counter--;
    } else {
    	// Request BLE send
    	r_send_req = 1;
    	BLE_Send_Data_Counter = BLE_SEND_DATA_TIME;
    }

    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
void Disable_tau0_channel3_interrupt(void) {
	TMMK03 = 1U;    /* disable INTTM03 interrupt */
}
void Enable_tau0_channel3_interrupt(void) {
	TMMK03 = 0U;    /* enable INTTM03 interrupt */
}
/* End user code. Do not edit comment generated here */
