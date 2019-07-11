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
* File Name    : r_cg_main.c
* Version      : Code Generator for RL78/L1A V1.01.02.03 [24 May 2018]
* Device(s)    : R5F11MMF
* Tool-Chain   : CCRL
* Description  : This file implements main function.
* Creation Date: 4/16/2019
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_tau.h"
#include "r_cg_it.h"
#include "r_cg_12adc.h"
#include "r_cg_12da.h"
#include "r_cg_sau.h"
#include "r_cg_iica.h"
#include "r_cg_intp.h"
/* Start user code for include. Do not edit comment generated here */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
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
MapleCandyGlobal MapleParam;

//#define ENABLE_TEST_API
#ifdef ENABLE_TEST_API
static uint8_t Test_API(void);
#endif

static void Init_MapleCandy(void);
static void Init_D2A_Converter(void);

static void Metal_Detector_Disable_Unused_DAC_Channels(void);
static void Metal_Detector_Enable_Unused_DAC_Channels(void);
static void Metal_Detector_Set_Pulse(uint8_t value);
static void Metal_Detector_Reset_Capacitor(void);
static void Metal_Detector_Get_Measurement(void);
static void Metal_Detector_Set_LEDs(int32_t diffSum, uint32_t flash_period);

static void Start_Utility_Timer(void);

/* End user code. Do not edit comment generated here */

static void R_MAIN_UserInit(void);
/***********************************************************************************************************************
* Function Name: main
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void main(void)
{
       R_MAIN_UserInit();
    /* Start user code. Do not edit comment generated here */

    Init_MapleCandy();

    delayMilliseconds( 100 );

    if(!readTest_Jig_IO()){
		while (1U)
		{
			switch(MapleParam.App_State) {
				case App_BLE_State:
					app_BLE();
					break;
				case App_Metal_Detect_State:
					app_Metal_Detector();
					break;
				default:
					app_BLE();
					break;
			}
		}
    }else{
    	routineTest_JIG();
    }
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: R_MAIN_UserInit
* Description  : This function adds user code before implementing main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void R_MAIN_UserInit(void)
{
    /* Start user code. Do not edit comment generated here */
    EI();
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */

//Maple Candy API functions

/**
 * @brief Sets the DAC 0-10V (output), P20/ANO0
 *
 * @note The naming of DAC1 refers to DAC Channel0 hardware
 * @note This is because we want ADC Channel 0-10V to loop back to DAC 0-10V reference
 *
 * @param fVolts Desired output voltage (0.0 - 10.0V)
 * @return 0 if DAC voltage set, 1 if error
 */
uint8_t Maple_Candy_Set_DAC_0_10V_float(float fVolts)
{
uint8_t status = 0;

    if ( (fVolts >= 0.23) && (fVolts <= 10.0) ){

        //Convert fVolts to 12bits, Vout = (Vout_Desired)*(341.4) + 391.18
        MapleParam.Measure.DAC.g_12da0_value = (uint16_t) ((fVolts * 341.4) + 391.18);

    } else {
        MapleParam.Measure.DAC.g_12da0_value = 0;
        status = 1;
    }         
    R_12DA0_Set_ConversionValue(MapleParam.Measure.DAC.g_12da0_value);
    return status;         
}

/**
 * @brief Sets the DAC 0-10V (output), P20/ANO0 for Test Jig
 *
 * @param fVolts Desired output voltage (0 - 4095)
 * @return 0 if DAC voltage set, 1 if error
 */
uint8_t Maple_Candy_Set_DAC_0_10V_testjig(float fVolts)
{
uint8_t status = 0;
float tempfloat;

    if ( (fVolts >= 0.23) && (fVolts <= 10.0) ){

        //Convert fVolts to 12bits, Vout = (Vout_Desired/10)*(3850-35) + 335
    	tempfloat = (fVolts*341.4) + 391.18;
        MapleParam.Measure.DAC.g_12da0_value = (uint16_t) (tempfloat);

    } else {
        MapleParam.Measure.DAC.g_12da0_value = 0;
        if((fVolts >= 0.0) && (fVolts <= 10.0)) status = 0;
        else status = 1;
    }
    R_12DA0_Set_ConversionValue(MapleParam.Measure.DAC.g_12da0_value);
    return status;
}

/**
 * @brief Sets the DAC 0-10V(output), P20/ANO0
 *
 * @note The naming of DAC1 refers to DAC Channel0 hardware
 * @note This is because we want ADC Channel 0-10V to loop back to DAC 0-10V reference
 *
 * @param Volts Desired output voltage (0-4095)
 * @return 0 if DAC voltage set, 1 if error
 */
uint8_t Maple_Candy_Set_DAC_0_10V_uint16_t(uint16_t Volts)
{
uint8_t status = 0;

    if (Volts <= 4095) {

        //Convert fVolts to 12 bits, , (fVolts * 4095 / 10.0), round up to nearest integer
        MapleParam.Measure.DAC.g_12da0_value = (uint16_t) Volts;

    } else {
        MapleParam.Measure.DAC.g_12da0_value = 0;
        status = 1;
    }
    R_12DA0_Set_ConversionValue(MapleParam.Measure.DAC.g_12da0_value);
    return status;
}

/**
 * @brief Sets the DAC 0-3.3V output, P100/AN01
 *
 * @note The naming of DAC0 refers to DAC Channel1 hardware
 * @note This is because we want ADC Channel 0-3V to loop back to DAC 0-3V reference
 *
 * @param fVolts Desired output voltage (0.0 - 3.0V)
 * @return 0 if DAC voltage set, 1 if error
 */
uint8_t Maple_Candy_Set_DAC_0_3V3_float(float fVolts)
{
uint8_t status = 0;

    if ( (fVolts >= 0.001979) && (fVolts <= 3.3) ){

        //Convert fVolts to 12 bits, (fVolts * 4095 / 3.3), round up to nearest integer
    	//Linearization equation, hex = volts * 1235.5 - 2.4455
        MapleParam.Measure.DAC.g_12da1_value = (uint16_t) ((fVolts * 1235.5)+2.4455 );

    } else {
    	MapleParam.Measure.DAC.g_12da1_value = 0;
        status = 1;
    }
    R_12DA1_Set_ConversionValue(MapleParam.Measure.DAC.g_12da1_value);
    return status;         
}

/**
 * @brief Calibrate the ADC input to be linear for the ADC 0-3.3V
 *
 * @note There is an offset between the linearization 3.3/4095 and the real voltage
 * @note This formula does the final value a little more precise.
 * @note It is done here so we don't modify Android and iOS Apps.
 *
 * @param fhex
 * @return
 */
uint16_t Calibrate_ADC_0_3V3(uint16_t fhex){
	float tempVal;
	uint16_t adjustVal;

	tempVal = (fhex*1.0044)+2.4565;
	adjustVal = (uint16_t)tempVal;
	return adjustVal;
}

/**
 * @brief Calibrate the ADC input to be linear for the ADC 0-10.0V
 *
 * @note There is an offset between the linearization 10.0/4095 and the real voltage
 * @note This formula does the final value a little more precise.
 * @note It is done here so we don't modify Android and iOS Apps.
 *
 * @param fhex
 * @return
 */
uint16_t Calibrate_ADC_0_10V(uint16_t fhex){
	float tempVal;
	uint16_t adjustVal;

	tempVal = (fhex*1.0071)+3.0652;
	adjustVal = (uint16_t)tempVal;
	return adjustVal;
}

/**
 * @brief Sets the DAC 0-3.3V output, P100/AN01
 *
 * @note The naming of DAC0 refers to DAC Channel1 hardware
 * @note This is because we want ADC Channel 0-3V to loop back to DAC 0-3V reference
 *
 * @param Volts Desired output voltage (0-4095)
 * @return 0 if DAC voltage set, 1 if error
 */
uint8_t Maple_Candy_Set_DAC_0_3V3_uint16_t(uint16_t Volts)
{
uint8_t status = 0;

    if (Volts <= 4095){

        MapleParam.Measure.DAC.g_12da1_value = (uint16_t) (Volts);

    } else {
    	MapleParam.Measure.DAC.g_12da1_value = 0;
    	status = 1;
    }
    R_12DA1_Set_ConversionValue(MapleParam.Measure.DAC.g_12da1_value);
    return status;
}

/**
 * @brief Gets the ADC 0-3V input, P101/ANIO0
 *
 * @return 12-bit ADC input measurement
 */
uint16_t Maple_Candy_Get_ADC_0_3V3_uint16_t(void)
{
    MapleParam.Measure.ADC.Flag_get_ADC_0_3V3 = 1;
    Disable_tau0_channel3_interrupt();

    R_12ADC_Start();
    while (MapleParam.Measure.ADC.Flag_get_ADC_0_3V3);
    {
    }
    Enable_tau0_channel3_interrupt();
    return(MapleParam.Measure.ADC.g_12adc_0_3V3) ;
}
          
/**
 * @brief Gets the ADC 0-10V input, P142/ANI07
 *
 * @return 12-bit ADC input measurement
 */
uint16_t Maple_Candy_Get_ADC_0_10V_uint16_t(void)
{
    MapleParam.Measure.ADC.Flag_get_ADC_0_10_V = 1;
    Disable_tau0_channel3_interrupt();

    R_12ADC_Start();
    while (MapleParam.Measure.ADC.Flag_get_ADC_0_10_V);
    {
    }
    Enable_tau0_channel3_interrupt();
    return(MapleParam.Measure.ADC.g_12adc_0_10V) ;
}

/**
 * @brief Gets the ADC 4-20mA input, P143/ANI08
 *
 * @return 12-bit ADC input measurement
 */
uint16_t Maple_Candy_Get_ADC_2_20mA_uint16_t(void)
{
    MapleParam.Measure.ADC.Flag_get_ADC_4_20mA = 1;
    Disable_tau0_channel3_interrupt();

    R_12ADC_Start();

    while (MapleParam.Measure.ADC.Flag_get_ADC_4_20mA);
    {
    }
    Enable_tau0_channel3_interrupt();
    return(MapleParam.Measure.ADC.g_12adc_4_20mA) ;
}

static void Init_MapleCandy(void)
{
	//Set Metal Detect to Idle
	MapleParam.Metal_Detect_State = Metal_Detect_Idle_State;

	//Select the application to start with, BLE or Metal Detector
    MapleParam.App_State = App_BLE_State;

    Start_Utility_Timer();
    Init_D2A_Converter();
    Init_A2D_Converter();

	/* Enable UART to send terminal debug messages */
	switch(MAPLE_CANDY_DEBUG_UART_NUMBER) {
		case 0:
			R_UART0_Start();
			break;
		case 2:
		    R_UART2_Start();
			break;
		default:
			break;
	}
}
          
static void Init_D2A_Converter(void)
{
    /* Initialize DA converter*/
    R_12DA_Create();
    
    /* Enable DA0 converter */
    R_12DA0_Start();
    
    Maple_Candy_Set_DAC_0_10V_uint16_t(0);
    
    /* Enable DA1 converter */
    R_12DA1_Start();
    
    Maple_Candy_Set_DAC_0_3V3_uint16_t(0);

    MapleParam.Measure.DAC.g_12da0_value = 0;
    MapleParam.Measure.DAC.g_12da1_value = 0;
}

void Init_A2D_Converter(void)
{
	R_12ADC_Stop();

    R_12ADC_Create();

    R_12ADC_Start();
}

void Debug_UART_Send(uint8_t *tx_buf, uint16_t tx_num)
{
#if MAPLE_CANDY_DEBUG_UART_NUMBER==0 || MAPLE_CANDY_DEBUG_UART_NUMBER==2
	while(MapleParam.UART.Debug_UART_flag_tx_in_progress){};
	MapleParam.UART.Debug_UART_flag_tx_in_progress=1;
	switch(MAPLE_CANDY_DEBUG_UART_NUMBER) {
		case 0:
			R_UART0_Send(tx_buf, tx_num);
			break;
		case 2:
			R_UART2_Send(tx_buf, tx_num);
			break;
		default:
			break;
	}
#endif
}

void Debug_Message(char *MessageStr)
{
	Debug_UART_Send((uint8_t*) MessageStr, strlen(MessageStr));
}

void Reset_BLE(void)
{
	//Reset G1D BLE module
	P5_bit.no0 = 0;

	//Wait 20 millisec
	delayMilliseconds(500);

	P5_bit.no0 = 1;
}

void LED_Green_On(void)
{
    P7_bit.no2 = 0;
}

void LED_Green_Off(void)
{
    P7_bit.no2 = 1;    
}

void LED_Green_Toggle(void)
{
    P7_bit.no2 ^= 1;     
}

void LED_Red_On(void)
{
    P7_bit.no3 = 0;
}

void LED_Red_Off(void)
{
    P7_bit.no3 = 1;    
}

void LED_Red_Toggle(void)
{
    P7_bit.no3 ^= 1;     
}

uint8_t isLED_Green_On(void)
{
	if (P7_bit.no2 == 0) {
		return(1);
	} else {
		return(0);
	}
}

static void Metal_Detector_Set_Pulse(uint8_t value)
{
    P10_bit.no6 = value;
}

static void Start_Utility_Timer(void)
{
    /* Start TAU channel 4 counter, 1 millisecond timer
		- Updates time stamp
		- Polls switch
		- Blinks LEDS
	*/
    R_TAU0_Channel4_Start();
}

void LED_Green_Blink_On(void)
{
	MapleParam.LED.Blink_Green = 1;
	MapleParam.LED.Blink_Timer_Counter = LED_BLINK_TIME;
	LED_Green_Off();
}

void LED_Green_Blink_Off(void)
{
	MapleParam.LED.Blink_Green = 0;
}

//Metal Detector defines
#define N_PULSE_METAL_DETECTOR 3
#define DEBUG_METAL_DETECTOR 1

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

static void Metal_Detector_Reset_Capacitor(void)
{  
    //set P107 (Port 10 pin 7) to digial I/O, disable ADC pin
    PMC10 &= ~0x80; 

    //set (Port 10 pin 7) P107 to Output mode
    PM10 &= ~0x80;
      
    //set P107 (Port 10 pin 7) to low to reset the capacitor
    P10_bit.no7 = 0;

    delayMicroseconds(20);

    //set (Port 10 pin 7) P107 to Input mode
    PM10 |= 0x80;  

    //set P107 pin to ADC input mode enable
    PMC10 |= 0x80;                
}

static void Metal_Detector_Disable_Unused_DAC_Channels(void)
{
    //Disable P101/ANI00
    PMC10 &= ~0x02;
    
    //Disable P142/ANI07
    PMC14 &= ~0x04;
    
    //Disable P143/ANI08
    PMC14 &= ~0x08;
    
    //Disable P21/ANI09
    PMC2 &= ~0x02;    
}

static void Metal_Detector_Enable_Unused_DAC_Channels(void)
{
    //Enable P101/ANI00
    PMC10 |= 0x02;
    
    //Enable P142/ANI07
    PMC14 |= 0x04;
    
    //Enable P143/ANI08
    PMC14 |= 0x08;
    
    //Enable P21/ANI09
    PMC2 |= 0x02;        
}

void ClearTimeStampMilliseconds(void)
{
    MapleParam.Timer.TimeStampMilliseconds = 0;
    MapleParam.Timer.TimeStampPrevChangeAppState = 0;
}

uint32_t getTimeStampMilliseconds(void)
{
    return(MapleParam.Timer.TimeStampMilliseconds);
}

static void Metal_Detector_Set_LEDs(int32_t diffSum, uint32_t flash_period)
{
    //determine if LEDs should be on or off
    uint8_t ledstat=0;
    
    if (diffSum > 0) {
    	ledstat = 1;
    }
    if (diffSum < 0) {
    	ledstat = 2;
    }
	//For better user experience,
	//Lower flash period threshold (1000 to 400) to reach steady state faster
	//When steady state is reached, turn green LED "OFF" 
    if (flash_period > 400) {
    	ledstat = 0;
    }

    //switch the LEDs to this setting
    if (ledstat==0){
    	LED_Green_Off();
      LED_Red_Off();
    }
    if (ledstat==1){
    	LED_Green_On();
      LED_Red_Off();
    }
    if (ledstat==2){
		//For better user experience,
		//Disable red LED, turn green LED "ON" 
    	LED_Green_On();
      LED_Red_Off();
    }  
}

static void Metal_Detector_Get_Measurement(void)
{

// Metal detector
// Runs a pulse over the search loop in series with resistor
// Voltage over search loop spikes
// Through a diode this charges a capacitor
// Value of capacitor after series of pulses is read by ADC

// Metal objects near search loop change inductance.
// ADC reading depends on inductance.
// changes wrt long-running mean are indicated by LEDs
// LED1 indicates rise in inductance
// LED2 indicates fall in inductance
// the flash rate indicates how large the difference is

// wiring:
// 220Ohm resistor on D2
// 10-loop D=10cm search loop between ground and resistor
// diode (-) on pin A0 and (+) on loop-resistor connection
// 10nF capacitor between A0 and ground

// First time, run with with serial print on and tune value of N_PULSE_METAL_DETECTOR
// to get capacitor reading between 200 and 300

uint32_t sumsum=0; //running sum of 64 sums 
uint16_t skip=0;   //number of skipped sums
uint32_t flash_period=0; //period (in ms) to flash led
uint16_t imeas;
uint16_t ipulse;


	while (MapleParam.Metal_Detect_State == Metal_Detect_Change_App_State)
	{
		/*
			Red LED's blink three times in this while loop (managed by timer 4),
			indicates changing of the application state from BLE to Metal Detector
		*/
	}

    MapleParam.Measure.MetalDetector.diffSum = 0;
    MapleParam.Measure.MetalDetector.nmeas=256;  //measurements to take
    
    ClearTimeStampMilliseconds();

    MapleParam.Metal_Detect_State = Metal_Detect_In_Progress_State;

    while(MapleParam.App_State == App_Metal_Detect_State) {

      MapleParam.Measure.MetalDetector.minval=1023;
      MapleParam.Measure.MetalDetector.maxval=0;
      
      //perform measurement
      MapleParam.Measure.MetalDetector.sum=0;

      for (imeas=0; imeas<MapleParam.Measure.MetalDetector.nmeas+2; imeas++){
          
        R_12ADC_Stop();
        
        //Reset capacitor
        Metal_Detector_Reset_Capacitor();
          
        //apply pulses to metal detector input
        for (ipulse = 0; ipulse < N_PULSE_METAL_DETECTOR; ipulse++) {
          Metal_Detector_Set_Pulse(1); //takes ? microseconds
          delayMicroseconds(3);
          Metal_Detector_Set_Pulse(0);  //takes ? microseconds
          delayMicroseconds(3);
        }
        
        //Reset ADC finish flag
        MapleParam.Measure.MetalDetector.ADCFinish = 0;
        Disable_tau0_channel3_interrupt();

        R_12ADC_Start();
        
        //Wait for ADC measurement to finish
        while(!MapleParam.Measure.MetalDetector.ADCFinish)
        {
        };
        Enable_tau0_channel3_interrupt();

        //uint16_t val = analogRead(pin_cap); //takes ? microseconds
        MapleParam.Measure.MetalDetector.minval = min(MapleParam.Measure.MetalDetector.Output,MapleParam.Measure.MetalDetector.minval);
        MapleParam.Measure.MetalDetector.maxval = max(MapleParam.Measure.MetalDetector.Output,MapleParam.Measure.MetalDetector.maxval);
        MapleParam.Measure.MetalDetector.sum+=MapleParam.Measure.MetalDetector.Output;
        
        //Metal Detector set LEDs
        Metal_Detector_Set_LEDs(MapleParam.Measure.MetalDetector.diffSum, flash_period);
      
      }
     
      //subtract minimum and maximum value to remove spikes
      MapleParam.Measure.MetalDetector.sum-=MapleParam.Measure.MetalDetector.minval;
      MapleParam.Measure.MetalDetector.sum-=MapleParam.Measure.MetalDetector.maxval;
      
      //process
      if (sumsum==0) sumsum=MapleParam.Measure.MetalDetector.sum<<6; //set sumsum to expected value
      MapleParam.Measure.MetalDetector.avgsum=(sumsum+32)>>6;
      MapleParam.Measure.MetalDetector.diffSum=MapleParam.Measure.MetalDetector.sum-MapleParam.Measure.MetalDetector.avgsum;
      if (abs(MapleParam.Measure.MetalDetector.diffSum)<MapleParam.Measure.MetalDetector.avgsum>>10){      //adjust for small changes
        sumsum=sumsum+MapleParam.Measure.MetalDetector.sum-MapleParam.Measure.MetalDetector.avgsum;
        skip=0;
      } else {
        skip++;
      }
      if (skip>64){     // break off in case of prolonged skipping
        sumsum=MapleParam.Measure.MetalDetector.sum<<6;
        skip=0;
      }

      // one permille change = 2 ticks/s
      if (MapleParam.Measure.MetalDetector.diffSum==0) flash_period=1000000;
      else flash_period=MapleParam.Measure.MetalDetector.avgsum/(2*abs(MapleParam.Measure.MetalDetector.diffSum));
              
    }
    MapleParam.Metal_Detect_State = Metal_Detect_Idle_State;
}

void app_Metal_Detector(void)
{
	//Disable DAC channels unused for metal detector
	Metal_Detector_Disable_Unused_DAC_Channels();


	Metal_Detector_Get_Measurement();

	//Disable DAC channels unused for metal detector
	Metal_Detector_Enable_Unused_DAC_Channels();
}

#ifdef ENABLE_TEST_API
//For debugging
uint16_t ADC_0_3V3_uint16_t;
uint16_t ADC_0_10V_uint16_t;
uint16_t ADC_2_20mA_uint16_t;

static uint8_t Test_API(void)
{
uint8_t status=0;

	//DAC 0-10V
	status |= Maple_Candy_Set_DAC_0_10V_float(1.5);

	status |= Maple_Candy_Set_DAC_0_10V_float(2.0);

	status |= Maple_Candy_Set_DAC_0_10V_float(3.0);

	status |= Maple_Candy_Set_DAC_0_10V_float(4.5);

	status |= Maple_Candy_Set_DAC_0_10V_float(7.0);

	status |= Maple_Candy_Set_DAC_0_10V_float(9.0);

	//DAC 0-3V3
	status |= Maple_Candy_Set_DAC_0_3V3_float(0.2);

	status |= Maple_Candy_Set_DAC_0_3V3_float(0.4);

	status |= Maple_Candy_Set_DAC_0_3V3_float(1.0);

	status |= Maple_Candy_Set_DAC_0_3V3_float(1.8);

	status |= Maple_Candy_Set_DAC_0_3V3_float(2.7);

	status |= Maple_Candy_Set_DAC_0_3V3_float(3.3);

	//ADC 0-3V3
	ADC_0_3V3_uint16_t = Maple_Candy_Get_ADC_0_3V3_uint16_t();

	//ADC 0-10V
	ADC_0_10V_uint16_t = Maple_Candy_Get_ADC_0_10V_uint16_t();

	//ADC 4_20mA
	ADC_2_20mA_uint16_t = Maple_Candy_Get_ADC_2_20mA_uint16_t();

    return status;
}
#endif

void delayMicroseconds(uint16_t Microseconds)
{
	MapleParam.Timer.counter_microseconds = Microseconds;
	R_TAU0_Channel0_Start();
	while(MapleParam.Timer.counter_microseconds);
	R_TAU0_Channel0_Stop();
}

void delayMilliseconds(uint16_t Milliseconds)
{
	MapleParam.Timer.counter_milliseconds = Milliseconds;
	R_TAU0_Channel2_Start();
	while(MapleParam.Timer.counter_milliseconds);
	R_TAU0_Channel2_Stop();
}

/**
 * Function to test IO pin for TEST JIG mode enable
 *
 * @return 0=disable, 1=enable
 */
uint8_t readTest_Jig_IO(void){
	uint8_t ret;
#ifndef TEST_JIG_ENABLE_SIMULATOR
	//The port 12 pin 7 has a pull up, 1=disable and 0=enable
	if(P12_bit.no7){
		ret=0; //Disable
	}else{
		ret=1; //Enable
	}
#else
	ret=1; //Enable
#endif
	return ret;
}
/**
 * Test JIG routine
 */
void routineTest_JIG(void){
#define TEST_COUNT	6	//Array size for test values
#define BIT_TOLERANCE 0.05 //Tolerance in percentage
#define VALUE_TOLERANCE 4095*0.07
#define MOISTURE_MIN	0
#define MOISTURE_MAX	150
#define CURRENT_4_20MA_MIN		3500
#define CURRENT_4_20MA_MAX		4000
//MV: This define send data to the console for debugging the ADCs
//#define DEBUG_ADCS
uint16_t 	xx, yy;
uint16_t	valueADC;
uint16_t	tempADC;
int16_t		maxADC;
int16_t		minADC;
uint8_t 	verrors[5];
//int16_t values_hex_10V[TEST_COUNT];
float tolerance_float[TEST_COUNT];
float values_10V[TEST_COUNT] = { 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};
float values_3_3V[TEST_COUNT]={ 0, 0.5, 1.0, 1.75, 2.0, 2.5};
#ifdef DEBUG_ADCS
char Test_Starting_String[] = {"\n** Test running ... **\n"};
char buffserial[50];
#endif
float tempfloat;

/********************************************************************************
 * Code to test DAC and ADC signals in a Console
 ********************************************************************************/
#ifdef DEBUG_ADCS
	Debug_UART_Send((uint8_t*) Test_Starting_String, strlen(Test_Starting_String));
	for(xx=0;xx<=4095;xx=xx+5){
		//Set DAC 0_10V
		Maple_Candy_Set_DAC_0_3V3_uint16_t(xx);
		//Delay 100 ms
		delayMilliseconds(100);
		//Set DAC 0_10V
		Maple_Candy_Set_DAC_0_3V3_uint16_t(xx);
		//Delay 100 ms
		delayMilliseconds(200);
		//Read ADC 0_10V
		valueADC = MapleParam.Measure.ADC.g_12adc_0_3V3;
		sprintf(buffserial,"%d\t%d\r\n",xx,valueADC);
		//Debug_Message(APP_Running_String);
		Debug_UART_Send((uint8_t*) buffserial, strlen(buffserial));
	}
#endif
/********************************************************************************
 * 1)	0-10V OUT (pin ANO0) connected to 0-10V IN (pin ANI07)
 * 			a. Generate a range of voltages from 0 to 10V on pin ANO0
 * 			b. For each voltage, read the value from ANI07 and validate
 * 			within a bracket (Ex:+/-0.1V).
 ********************************************************************************/

	for(xx=0;xx<TEST_COUNT;xx++){
		//Set DAC 0_10V
		Maple_Candy_Set_DAC_0_10V_testjig(values_10V[xx]);
		//Delay 100 ms
		delayMilliseconds(100);
		//Set DAC 0_10V
		Maple_Candy_Set_DAC_0_10V_testjig(values_10V[xx]);
		//Delay 100 ms
		delayMilliseconds(200);
		//Read ADC 0_10V
		valueADC = MapleParam.Measure.ADC.g_12adc_0_10V;
		//valueADC = Convert_ADC_10V_To_Pre_Conversion_Value(MapleParam.Measure.ADC.g_12adc_0_10V);
		tolerance_float[xx] = values_10V[xx]*BIT_TOLERANCE;
		if(!tolerance_float[xx])tolerance_float[xx] = 0.00086242;
		tempfloat = values_10V[xx]+tolerance_float[xx];
		//Linearization equation, voltage on pin Vs ADC reading
		tempfloat = (tempfloat * 406.62)-3.0436;
		//tolerance_hex = (uint16_t) (tempfloat * 409.5);
		maxADC =  (uint16_t)(tempfloat);
		if(maxADC>4095){
			maxADC=4095;
		}
		tempfloat = values_10V[xx]-tolerance_float[xx];
		//Linearization equation, voltage on pin Vs ADC reading
		tempfloat = (tempfloat * 406.62)-3.0436;
		minADC = (uint16_t)(tempfloat);
		if(minADC<0){
			minADC=0;
		}
		if(minADC<=valueADC && valueADC<=maxADC){
			verrors[0]=0;
		}else{
			verrors[0]+=1;
		}
	}
/********************************************************************************
 * 2)	MCU_DAC (pin ANO1) connected to MCU_ADC (pin ANI00):
 *			a. Generate a range of voltages from 0.7V to 3.3V on pin ANO1
 * 			b. For each voltage, read the value from ANI00 and validate the result
 * 			within a bracket (Ex: +/-0.1V).
 *
 ********************************************************************************/

	for(xx=0;xx<TEST_COUNT;xx++){
		//Set DAC 0_3V3
		Maple_Candy_Set_DAC_0_3V3_float(values_3_3V[xx]);
		//Delay 100 ms
		delayMilliseconds(100);
		//Set DAC 0_3V3
		Maple_Candy_Set_DAC_0_3V3_float(values_3_3V[xx]);
		//Delay 200 ms
		delayMilliseconds(200);
		//Read ADC 0_3.3V
		valueADC = MapleParam.Measure.ADC.g_12adc_0_3V3;
		tolerance_float[xx] = (values_3_3V[xx]*BIT_TOLERANCE);
		//MV:Increased from 0.00086242V to 0.075V
		if(!tolerance_float[xx])tolerance_float[xx] = 0.08;
		//Linearization equation, voltage on pin Vs ADC reading
		tempfloat = values_3_3V[xx] + tolerance_float[xx];
		tempfloat = (uint16_t) ((tempfloat * 1235.5)+2.4455 );
		maxADC = (uint16_t) (tempfloat);
		if(maxADC>4095){
			maxADC=4095;
		}
		//Linearization equation, voltage on pin Vs ADC reading
		tempfloat = values_3_3V[xx] - tolerance_float[xx];
		tempfloat = (uint16_t) ((tempfloat * 1235.5)+2.4455 );
		minADC = (uint16_t) (tempfloat );
		if(minADC<=1){
			minADC=0;
		}
		if(xx!=0){
			if(xx==1){
				tempfloat = 0.30;
				tempfloat = (uint16_t) ((tempfloat * 1235.5)+2.4455 );
				tempADC = (uint16_t) (tempfloat);
				valueADC += tempADC;
			}else if(xx==2){
				tempfloat = 0.36;
				tempfloat = (uint16_t) ((tempfloat * 1235.5)+2.4455 );
				tempADC = (uint16_t) (tempfloat);
				valueADC += tempADC;
			}else{
				tempfloat = 0.378;
				tempfloat = (uint16_t) ((tempfloat * 1235.5)+2.4455 );
				tempADC = (uint16_t) (tempfloat);
				valueADC += tempADC;
			}

		}
		if(minADC<=valueADC && valueADC<=maxADC){
			verrors[1]=0;
		}else{
			verrors[1]+=1;
		}
	}
	// Set DAC0 to 0 so the moisture sensor voltage can be read
	Maple_Candy_Set_DAC_0_3V3_float(0.0);
	//Delay 100 ms
	delayMilliseconds(100);
	Maple_Candy_Set_DAC_0_3V3_float(0.0);
	//Delay 100 ms
	delayMilliseconds(500);

/********************************************************************************
 * 3)	Moisture detector VOUT (P6 pin 2) connected MCU_ADC (pin ANI00):
 * 			a.	Supply a fixed voltage of 0.7V to MCU_ADC (pin ANI00)
 * 			    (Diodes will be used to �OR� VOUT and MCU_DAC)
 * 			b.	Read and validate the value from ANI00
 *
 ********************************************************************************/
	//MV:it might better be connected to ANI08 4-20mA ADC Input

	//Read ADC 0_3.3V cannel 0 (ANI00)
	valueADC = MapleParam.Measure.ADC.g_12adc_0_3V3;

	if(MOISTURE_MIN<=valueADC && valueADC<=MOISTURE_MAX){
		verrors[2]=0;
	}else{
		verrors[2]=1;
	}
/********************************************************************************
 * 4)	4-20mA IN connected to a little circuit in the test jig:
 *			a.	Apply a constant current of 12mA
 * 			b.	Read the corresponding value from ANI08
 *
 ********************************************************************************/
	//Read ADC 4_20mA channel 8 (ANI08)
	valueADC = MapleParam.Measure.ADC.g_12adc_4_20mA;

	if( CURRENT_4_20MA_MIN<=valueADC && valueADC<=CURRENT_4_20MA_MAX){
		verrors[3]=0;
	}else{
		verrors[3]=1;
	}
/********************************************************************************
 * 5)	Metal detector IN (pin P106) connected to Metal detector OUT (pin P107):
 *			a.	Force a logic level on pin P107.
 *			b.	Read pin P106 status and validate
 *
 ********************************************************************************/
	//Output port 10 bit 6 = 1
	P10_bit.no6 = 1;

	//Disable ADC P107/ANI04
	PMC10 &= ~0x80;

	//Configure P10 pin 7 as input
	PM10 |= 0x80;

	//Output Port P10 pin 7 = 1
	//P10_bit.no7 = 1;
	delayMilliseconds(100);
	//Test loop back on Port 10 pin 6
	//For test jig available
#if 1
	if(P10_bit.no7 == 1){
		verrors[4]=0;
	}
#else
	//MV:Tested with zero so it doesn't generate an error.
	if(P10_bit.no6 == 0){
		verrors[4]=0;
	}
#endif
	else {
		verrors[4]=1;
	}
/*
 * Turn On Green led if verrors is equal to zero, Red if it is not zero.
 */
	LED_Red_Off();
	LED_Green_Off();
	yy=0;
	for(xx=0;xx<5;xx++){
		yy=yy+verrors[xx];
	}
	if(yy){
//		LED_Red_On();
		delayMilliseconds(300);
		while(1){
			for(xx=0;xx<5;xx++){
				if(verrors[xx]){
					for(yy=0;yy<xx+1;yy++){
						LED_Red_On();
						delayMilliseconds(250);
						LED_Red_Off();
						delayMilliseconds(250);
					}
				}
				//LED_Red_On();
				delayMilliseconds(1000);
			}
		}
	}else{
		LED_Green_On();
	}
	while(1);
}

/* End user code. Do not edit comment generated here */
