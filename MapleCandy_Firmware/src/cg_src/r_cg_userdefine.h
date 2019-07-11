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
* File Name    : r_cg_userdefine.h
* Version      : Code Generator for RL78/L1A V1.01.02.03 [24 May 2018]
* Device(s)    : R5F11MMF
* Tool-Chain   : CCRL
* Description  : This file includes user definition.
* Creation Date: 4/16/2019
***********************************************************************************************************************/
#ifndef _USER_DEF_H
#define _USER_DEF_H

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/

/* Start user code for function. Do not edit comment generated here */

#define FW_REVISION		"1.0.0"

// --------------- Maple Candy API -----------------------
uint8_t Maple_Candy_Set_DAC_0_3V3_float(float fVolts);
uint8_t Maple_Candy_Set_DAC_0_3V3_uint16_t(uint16_t Volts);
uint8_t Maple_Candy_Set_DAC_0_10V_float(float fVolts);
uint8_t Maple_Candy_Set_DAC_0_10V_uint16_t(uint16_t Volts);
uint16_t Maple_Candy_Get_ADC_0_3V3_uint16_t(void);
uint16_t Maple_Candy_Get_ADC_0_10V_uint16_t(void);
uint16_t Maple_Candy_Get_ADC_2_20mA_uint16_t(void);
//Calibration
uint16_t Calibrate_ADC_0_3V3(uint16_t fhex);
uint16_t Calibrate_ADC_0_10V(uint16_t fhex);
// ------------------------------------------------------*/

#define ENABLE_BLINK_LED

#define MAX_DAC_VALUE               0x0FFF
#define LED_BLINK_TIME  			500

#define	ADC_VARIATION_TOLERANCE_3V3			8
#define	ADC_VARIATION_TOLERANCE_10V			50
#define	ADC_VARIATION_TOLERANCE_4_20mA		8

#define	NUM_DAC_MESSAGES_TO_ENABLE_SEND 	3
#define NUM_ADC_MESSAGES_TO_ENABLE_SEND		3

//Select UART to connect with BLE
#define MAPLE_CANDY_BLE_UART_NUMBER	3

//Select UART for debugging messages to terminal
#define MAPLE_CANDY_DEBUG_UART_NUMBER 2 //MV: Select port 3 to disable Debug Port

#define DAC_MAX_STR_LEN_FLOAT	10

// --------------- Test JIG Global Defines -----------------------
//Uncomment for testing without the Test JIG fixture
//#define TEST_JIG_ENABLE_SIMULATOR
uint8_t readTest_Jig_IO(void);
void routineTest_JIG(void);


typedef struct {
    volatile uint16_t g_12adc_0_3V3;  /* P101/ANI00 = set as ADC input for 0-3.3V */
    volatile uint16_t g_12adc_0_10V;  /* P142/ANI07 = set as ADC input for 0-10V */
    volatile uint16_t g_12adc_4_20mA;  /* P143/ANI08 = set as ADC input for 4-20mA */
    volatile uint16_t g_Prev_12adc_0_3V3;	/* Previous ADC input 0-3.3V */
    volatile uint16_t g_Prev_12adc_0_10V;	/* Previous ADC input 0-10V */
    volatile uint16_t g_Prev_12adc_4_20mA;	/* Previous ADC input 4-20mA */
    volatile uint8_t Flag_get_ADC_0_3V3;
    volatile uint8_t Flag_get_ADC_0_10_V;
    volatile uint8_t Flag_get_ADC_4_20mA;
} ADC_Param;

typedef struct {
    volatile uint16_t g_12da0_value;
    volatile uint16_t g_12da1_value;
} DAC_Param;

typedef struct {
    uint8_t Enable;
    uint16_t nmeas;
    uint16_t minval;
    uint16_t maxval;
    int32_t avgsum;
    int32_t diffSum;        //difference between sum and avgsum
    uint32_t sum;
    uint16_t Output;
    uint8_t ADCFinish;    
} MetalDetectorParam;

typedef struct {
    uint8_t Blink_Red_Green;
    uint8_t Blink_Green;
    uint8_t Blink_Red;
    uint8_t BLink_Red_Count;
    uint32_t Blink_Timer_Counter;
} LED_Param;

typedef struct {
    uint16_t counter_microseconds;
    uint16_t counter_milliseconds;
    uint32_t TimeStampMilliseconds;
    uint32_t TimeStampPrevChangeAppState;
} TimerParam;

typedef enum {
	NO_MEASUREMENTS = 				0,
	SET_DAC_0_3V3_OUTPUT =			1<<0,
	SET_DAC_0_10V_OUTPUT =			1<<1,
	GET_ADC_0_3V3_INPUT =			1<<2,
	GET_ADC_0_10V_INPUT	=			1<<3,
	GET_ADC_4_20MA_INPUT =			1<<4,
	GET_METAL_DETECTOR_OUTPUT = 	1<<5,
} Maple_Candy_Active_Measurements;

typedef struct {
    ADC_Param ADC;
    DAC_Param DAC;
    MetalDetectorParam MetalDetector;
} MeasurementParam;

typedef struct {
	volatile uint8_t Debug_UART_flag_tx_in_progress;
	uint8_t MapleCandyReceiveBLEBuffer[20];
} UARTParam;

typedef enum {
	App_BLE_State,
	App_Metal_Detect_State,
} Maple_Candy_App_State;

typedef enum {
	BLE_Idle_State,
	BLE_Advertising_State,
	BLE_Data_State,
} Maple_Candy_BLE_State;

typedef enum {
	Metal_Detect_Idle_State,
	Metal_Detect_Change_App_State,
	Metal_Detect_In_Progress_State,
} Maple_Candy_Metal_Detect_State;

typedef struct {
	float DAC_0_3V3_float;
	float DAC_0_10V_float;
	uint16_t DAC_0_3V3_uint16_t;
	uint16_t DAC_0_10V_uint16_t;
} Maple_Candy_Receive_Buffer;

typedef struct {
	MeasurementParam Measure;
    LED_Param LED;
    TimerParam Timer;
    UARTParam UART;
	Maple_Candy_App_State App_State;
	Maple_Candy_BLE_State BLE_State;
	Maple_Candy_Metal_Detect_State Metal_Detect_State;
	Maple_Candy_Receive_Buffer BLE_Receive_Buffer;
} MapleCandyGlobal;

extern MapleCandyGlobal MapleParam;

typedef struct {
	uint16_t ADC_0_3V3_uint16_t;
	uint16_t ADC_0_10V_uint16_t;
	uint16_t ADC_4_20MA_Input;
} Maple_Candy_Send_Buffer;

void app_BLE(void);
void app_Metal_Detector(void);

void Init_A2D_Converter(void);
void Disable_tau0_channel3_interrupt(void);
void Enable_tau0_channel3_interrupt(void);

void Reset_BLE(void);

void LED_Green_On(void);
void LED_Green_Off(void);
void LED_Green_Toggle(void);
void LED_Red_On(void);
void LED_Red_Off(void);
void LED_Red_Toggle(void);
void LED_Green_Blink_On(void);
void LED_Green_Blink_Off(void);
uint8_t isLED_Green_On(void);

void ClearTimeStampMilliseconds(void);
uint32_t getTimeStampMilliseconds(void);

void delayMicroseconds(uint16_t Microseconds);
void delayMilliseconds(uint16_t Milliseconds);

void Debug_Message(char *MessageStr);
void Debug_UART_Send(uint8_t *tx_buf, uint16_t tx_num);

extern volatile uint8_t        r_send_req;

/* End user code. Do not edit comment generated here */
#endif
