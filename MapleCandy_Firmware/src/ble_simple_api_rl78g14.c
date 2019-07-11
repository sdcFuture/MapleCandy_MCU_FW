/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. This 
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer *
* Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************/
/*******************************************************************************
* File Name    : ble_simple_api_rl78g14.c
* Version      : 1.0
* Description  : C source file of BLE simple API sample application for RL78/G14.
******************************************************************************/

#include "r_cg_macrodriver.h"
#include "r_cg_sau.h"
#include "r_cg_intp.h"
#include "rskrl78g14def.h"
#include <stdlib.h>
#include "rble_host.h"
#include "rbles_api.h"
#include "rscip_uart.h"
#include "timer.h"
#include "r_cg_userdefine.h"

//Uncomment to enable active measurements from Android/iOS app

//Uncomment to enable debug messages
//#define ENABLE_DEBUG_MESSAGES_BLE
#define ENABLE_DEBUG_TX_RX_MESSAGES_BLE

/* Demonstration mode                             */
/* Set one of the following definition value to 1 */
#define RBLES_SLAVE         (1)     /* also execute advertising  */
#define RBLES_MASTER        (0)     /* connect with slave device */
#define RBLES_ADVERTISER    (0)     /* only execute advertising  */
#define RBLES_SCANNER       (0)     /* only execute scaning      */
/* Set to 1 to use whitelist */
#define RBLES_WHITELIST     (0)


#if RBLES_SLAVE
/* SLAVE */
#define RBLES_ADV_CONN      (1)
#define RBLES_SCAN          (0)
#define RBLES_CONNECT       (0)
#define RBLES_DATA          (1)
#endif
#if RBLES_MASTER
/* MASTER */
#define RBLES_ADV_CONN      (0)
#define RBLES_SCAN          (0)
#define RBLES_CONNECT       (1)
#define RBLES_DATA          (1)
#endif
#if RBLES_ADVERTISER
/* SCANNER */
#define RBLES_ADV_CONN      (1)
#define RBLES_SCAN          (0)
#define RBLES_CONNECT       (0)
#define RBLES_DATA          (0)
#endif
#if RBLES_SCANNER
/* SCANNER */
#define RBLES_ADV_CONN      (0)
#define RBLES_SCAN          (1)
#define RBLES_CONNECT       (0)
#define RBLES_DATA          (0)
#endif


/******************************************************************************
Advertising parameters
******************************************************************************/
RBLE_BROADCAST_ENABLE_PARAM  AdvParam = {
    /* General Discoverable & Undirected Connectable */
    RBLE_GAP_GEN_DISCOVERABLE,                          /* Discoverable mode */
    RBLE_GAP_UND_CONNECTABLE ,                          /* Connectable mode  */
    {
        {
            20,                                         /* Advertising Interval Min:            */
                                                        /*     20 to 10240 (20 ms to 10.24 sec) */
            20,                                         /* Advertising Interval Max:            */
                                                        /*     20 to 10240 (20 ms to 10.24 sec) */
            RBLE_GAP_ADV_CONN_UNDIR,                    /* Advertising type */
            RBLE_ADDR_PUBLIC,                           /* Own address type */
            RBLE_ADDR_PUBLIC,                           /* DirectAddrType   */
            {0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12},       /* DirectAddr       */
            RBLE_ADV_ALL_CHANNELS,                      /* AdvChannelMap    */
            RBLE_ADV_ALLOW_SCAN_ANY_CON_ANY,            /* AdvFiltPolicy    */
            0x00,                                       /* Reserved         */
        },
        {
            /* Advertising data length */
            3+18,

            /* Advertising data */
            2, 0x01, 0x06,                              /* Flag */
            
            17, 0x07,                                   /* VUART service UUID */
            0x1B, 0xC5, 0xD5, 0xA5, 0x02, 0x00, 0xB8, 0x8C,
            0xE5, 0x11, 0x1B, 0xA2, 0x01, 0x00, 0x8C, 0xD6,
            
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00
            
        },
        {
            /* Scan response data length */
            3+4+14,

            /* Scan data */
            0x02, 0x0A, 0x00,                           /* Tx Power Level */
            
            0x03, 0xFF, 0x00, 0x01,                     /* Manufacturer Specific Data (DEMO) ;*/

            13, 0x09,                                   /* Complete Local Name */
            'M', 'a', 'p', 'l', 'e', 'C', 'a', 'n', 'd', 'y', '0','0',
            
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00
        }
    }
};

/******************************************************************************
Scaning parameters
******************************************************************************/
RBLE_SCANNING_INFO ScanParam = {
    {
        RBLE_SCAN_ACTIVE,                   /* Scan type */
        0,                                  /* Reserved  */
        160,                                /* Scan interval: 2 to 10240 (2.5 ms to 10.24 sec)     */
                                            /*     Note: When 2 ms is specified, it becomes 2.5 ms */
        150,                                /* Scan window  : 2 to 10240 (2.5 ms to 10.24 sec)     */
                                            /*     Note: When 2 ms is specified, it becomes 2.5 ms */
        RBLE_ADDR_PUBLIC,                   /* Own address type   */
        RBLE_SCAN_ALLOW_ADV_ALL             /* Scan filter policy */
    },
    RBLE_SCAN_FILT_DUPLIC_EN,               /* Filtering policy   */
    0                                       /* Reserved           */
};

/******************************************************************************
Connection parameters
******************************************************************************/
RBLE_CREATE_CONNECT_PARAM CreConParam = {
    160,                                    /* Scan interval: 2 to 10240 (2.5 ms to 10.24 sec)     */
                                            /*     Note: When 2 ms is specified, it becomes 2.5 ms */
    150,                                    /* Scan window  : 2 to 10240 (2.5 ms to 10.24 sec)     */
                                            /*     Note: When 2 ms is specified, it becomes 2.5 ms */
    RBLE_GAP_INIT_FILT_IGNORE_WLST,         /* Initiator filter policy */
    RBLE_ADDR_PUBLIC,                       /* Peer address type */
    {0x55, 0x55, 0x77, 0x77, 0x77, 0x77},   /* Peer BD address   */
    RBLE_ADDR_PUBLIC,                       /* Own address type  */
    0,                                      /* Reserved          */
    200,                                    /* Minimum of connection interval: 7 to 4000 (7.5 ms to 4 sec) */
                                            /*     Note: When 7 ms is specified, it becomes 7.5 ms         */
    200,                                    /* Maximum of connection interval: 7 to 4000 (7.5 ms to 4 sec) */
                                            /*     Note: When 7 ms is specified, it becomes 7.5 ms         */
    0,                                      /* Connection latency Range: 0x0000 to 0x01f4                  */
    32000,                                  /* Link supervision timeout: 100 to 32000 (100 ms to 32 sec)   */
    0,                                      /* Reserved */
    0                                       /* Reserved */
};

/******************************************************************************
Whitelist
******************************************************************************/
RBLE_WHITELIST  WhiteList = {
    {
        /* Address type      Device address                        Flag */
        {{RBLE_ADDR_PUBLIC, {0x55, 0x55, 0x77, 0x77, 0x77, 0x77}}, TRUE},
        {{RBLE_ADDR_PUBLIC, {0x02, 0x00, 0xfe, 0xfa, 0xfe, 0xca}}, TRUE},
        {{RBLE_ADDR_PUBLIC, {0x03, 0x00, 0xfe, 0xfa, 0xfe, 0xca}}, TRUE},
        {{RBLE_ADDR_PUBLIC, {0x04, 0x00, 0xfe, 0xfa, 0xfe, 0xca}}, TRUE},
        {{RBLE_ADDR_PUBLIC, {0x05, 0x00, 0xfe, 0xfa, 0xfe, 0xca}}, TRUE},
        {{RBLE_ADDR_PUBLIC, {0x06, 0x00, 0xfe, 0xfa, 0xfe, 0xca}}, TRUE},
        {{RBLE_ADDR_RAND,   {0x01, 0x00, 0xde, 0xfa, 0xfe, 0xca}}, TRUE},
        {{RBLE_ADDR_RAND,   {0x02, 0x00, 0xde, 0xfa, 0xfe, 0xca}}, TRUE},
        {{RBLE_ADDR_RAND,   {0x03, 0x00, 0xde, 0xfa, 0xfe, 0xca}}, TRUE},
        {{RBLE_ADDR_RAND,   {0x04, 0x00, 0xde, 0xfa, 0xfe, 0xca}}, TRUE},
        {{RBLE_ADDR_RAND,   {0x05, 0x00, 0xde, 0xfa, 0xfe, 0xca}}, TRUE},
        {{RBLE_ADDR_RAND,   {0x06, 0x00, 0xde, 0xfa, 0xfe, 0xca}}, TRUE},
    }
};

/* Advertising report list */
RBLE_ADV_REPORT_LIST    AdvReportList;

/* Result of connection */
RBLE_CONNECT_INFO       ConInfo;

volatile uint8_t        r_disconnect_req;
volatile uint8_t        r_send_req;
volatile uint8_t		DAC_Send_Count;
volatile uint8_t 		ADC_Send_Count;

void error(uint8_t no);
void sw1_callback(void);
void sw2_callback(void);
void sw3_callback(void);

void sw1_callback(void)
{
}

void sw2_callback(void)
{
    r_send_req = TRUE;
}

void sw3_callback(void)
{
    r_disconnect_req = TRUE;
}

void error(uint8_t no)
{
    while(1);
}

uint16_t Convert_ASCII_String_to_uint16_t(uint8_t *ASCII_String, uint8_t *SubStringLength)
{
uint8_t i;
uint8_t *str;
int16_t result;

	str = ASCII_String;

	// Iterate through all digits and update the result
	for (i=0, result=0; (str[i] != '\0') && (str[i] != '\n') && (str[i] != ','); i++) {
		if (str[i]>='0' && str[i]<= '9') {
			result = result*10 + str[i] - '0';
		} else {
			result = 0;
		}
		//test for unexpected string length
		if (i > DAC_MAX_STR_LEN_FLOAT) {
			result = 0;
			*SubStringLength = 0;
			return(result);
		}
	}
	//Return substring length to indicate start position of next substring
	*SubStringLength = i;
	return(result);
}

float Convert_ASCII_String_to_float(uint8_t *ASCII_String, uint8_t *SubStringLength)
{
uint8_t i;
uint8_t *str;
int16_t result;
char floatString[20];
float floatVal;

	str = ASCII_String;

	// Iterate through all digits and update the result
	for (i=0, result=0; (str[i] != '\0') && (str[i] != '\n') && (str[i] != ','); i++) {
		if ( ((str[i]>='0') && (str[i]<= '9')) || (str[i]=='.') ) {
			floatString[i] = str[i];
		}
		//test for unexpected string length
		if (i > DAC_MAX_STR_LEN_FLOAT) {
			result = 0;
			*SubStringLength = 0;
			return(result);
		}
	}
	//Return substring length to indicate start position of next substring
	*SubStringLength = i;

	//Convert string to float
	floatVal = atof(floatString);

	return(floatVal);
}

void Parse_BLE_Receive_Data_Set_DAC_uint16_t(void)
{
	uint8_t SubStringLength;

	/*
		Expecting 2 integer numbers as a string encoded in ASCII
		to represent the two DAC values

		number 1: DAC_0_3V3 (12-bit)
		number 2: DAC_0_10V (12-bit)

		Example:
		number 1: 1023  	(set DAC_0_3V3)
		number 2: 2047		(set DAC_0_10V)

		String: "1023,2047"

		Encoded: as ASCII 51,50,52,53,44,52,50,54,57,0

		Termination is either ASCII 0 (NULL) or ASCII 10 (LF)
	*/

	// Expecting 2 numbers, example 4095,4095
	// Get DAC 0_3V3 from receive buffer
	MapleParam.BLE_Receive_Buffer.DAC_0_3V3_uint16_t = Convert_ASCII_String_to_uint16_t(MapleParam.UART.MapleCandyReceiveBLEBuffer, &SubStringLength);
	if (MapleParam.BLE_Receive_Buffer.DAC_0_3V3_uint16_t > 4095) {
		MapleParam.BLE_Receive_Buffer.DAC_0_3V3_uint16_t = 0;
	}
	// Get DAC 0_10V from receive buffer
	MapleParam.BLE_Receive_Buffer.DAC_0_10V_uint16_t = Convert_ASCII_String_to_uint16_t(MapleParam.UART.MapleCandyReceiveBLEBuffer+SubStringLength+1, &SubStringLength);
	if (MapleParam.BLE_Receive_Buffer.DAC_0_10V_uint16_t > 4095) {
		MapleParam.BLE_Receive_Buffer.DAC_0_10V_uint16_t = 0;
	}

	//Set DAC 0_3V3
	Maple_Candy_Set_DAC_0_3V3_uint16_t(MapleParam.BLE_Receive_Buffer.DAC_0_3V3_uint16_t);
	//Set DAC 0_10V
	Maple_Candy_Set_DAC_0_10V_uint16_t(MapleParam.BLE_Receive_Buffer.DAC_0_10V_uint16_t);
}

void Parse_BLE_Receive_Data_Set_DAC_float(void)
{
	uint8_t SubStringLength;

	/*
		Expecting 2 float numbers as a string encoded in ASCII
		to represent the two DAC values

		number 1: DAC_0_3V3 (12-bit)
		number 2: DAC_0_10V (12-bit)

		Example:
		number 1: 3.2  	(set DAC_0_3V3)
		number 2: 2.16	(set DAC_0_10V)

		String: "3.2,2.16"

		Encoded: as ASCII 53,46,52,44,52,46,51,56,0

		Termination is either ASCII 0 (NULL) or ASCII 10 (LF)
	*/

	// Expecting 2 float strings, example "3.3,2.16"
	//Get DAC 0_3V3
	MapleParam.BLE_Receive_Buffer.DAC_0_3V3_float = Convert_ASCII_String_to_float(MapleParam.UART.MapleCandyReceiveBLEBuffer, &SubStringLength);
	//Get DAC 0_10f
	MapleParam.BLE_Receive_Buffer.DAC_0_10V_float = Convert_ASCII_String_to_float(MapleParam.UART.MapleCandyReceiveBLEBuffer+SubStringLength+1, &SubStringLength);

	//Set DAC 0_3V3
	Maple_Candy_Set_DAC_0_3V3_float(MapleParam.BLE_Receive_Buffer.DAC_0_3V3_float);
	//Set DAC 0_10V
	Maple_Candy_Set_DAC_0_10V_float(MapleParam.BLE_Receive_Buffer.DAC_0_10V_float);

	DAC_Send_Count--;
}

char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    int16_t shifter;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

void Debug_Message_get_DAC_String(char *DACStrOut) {
char Str1[15], Str2[15];

    Str1[0] = '\0';
    itoa(MapleParam.Measure.DAC.g_12da1_value, Str2);
    strcat(Str1,Str2);
    strcat(Str1, ",");
    itoa(MapleParam.Measure.DAC.g_12da0_value, Str2);
    strcat(Str1,Str2);
    strcat(Str1, "\n");
    strcpy(DACStrOut,Str1);
}

void Debug_Message_get_ADC_String(char *ADCStrOut) {
char Str1[20], Str2[20];

	Str1[0] = '\0';
	itoa(MapleParam.Measure.ADC.g_12adc_0_3V3, Str2);
	strcat(Str1,Str2);
	strcat(Str1,",");
	itoa(MapleParam.Measure.ADC.g_12adc_0_10V, Str2);
	strcat(Str1,Str2);
	strcat(Str1, "\n");
	strcpy(ADCStrOut,Str1);
}

char		DAC_String[20];
char		ADC_String[20];
char APP_Running_String[] = {"\n** MAPLE CANDY BLE APP running ... **\n"};
uint16_t 	txbuf[3];
uint16_t 	absDiffADC[3];
uint8_t UART_Number;

#ifdef ENABLE_DEBUG_MESSAGES_BLE
    char BLE_Advertising_String [] = {"\n** BLE Server Advertising  **\n"};
    char BLE_Client_Connected_String[] = {"\n** BLE Client connected **\n"};
    char BLE_Client_Disconnected_String[] = {"\n** BLE Client disconnected **\n"};
#endif


void app_BLE(void)
{
    RBLE_STATUS ret;
    uint8_t     evt;
    //uint8_t     rxbuf[20];
    //uint16_t    rxnum;
    uint8_t     specific_data = 0;

    //Reset BLE G1D module
    //Reset_BLE();

    //Reinitialize A2D converter each time BLE application starts
	Init_A2D_Converter();
    
    RPECTL = 0x80;
    
    r_disconnect_req = FALSE;
    r_send_req       = FALSE;
    DAC_Send_Count = NUM_DAC_MESSAGES_TO_ENABLE_SEND;
	ADC_Send_Count = NUM_ADC_MESSAGES_TO_ENABLE_SEND;
    
    //Reset time stamp
    ClearTimeStampMilliseconds();
	
    /* Enable UART for communication with RL78/G1D */
	UART_Number = MAPLE_CANDY_BLE_UART_NUMBER;

	switch(UART_Number) {
		case 1:
			R_UART1_Start();
			break;
		case 2:
			R_UART2_Start();
			break;
		case 3:
			R_UART3_Start();
			break;
		default:
			R_UART3_Start();
			break;
	}

    //Debug_Message(APP_Running_String);
	Debug_UART_Send((uint8_t*) APP_Running_String, strlen(APP_Running_String));

    /* The function is used to initialize the pseudo-random number generator */
    /* for function rand() by passing the argument seed.                     */
    srand(0x12ef);
    
    
    /* Simple API operation */
    LED_Red_Off();
    LED_Green_Off();
    
    if(RBLE_OK != R_BLES_initialize())
    {
        LED_Red_On();
        error(1);
    } else {
    	MapleParam.BLE_State = BLE_Advertising_State;
    }
	LED_Red_Off();
    LED_Green_Blink_On();

    LED0 = LED_ON;

#if RBLES_WHITELIST
    R_BLES_whitelist(&WhiteList);
#endif

    while(MapleParam.App_State == App_BLE_State)
    {

#if RBLES_ADV_CONN

#ifdef ENABLE_DEBUG_MESSAGES_BLE
        Debug_UART_Send((uint8_t*) BLE_Advertising_String, strlen(BLE_Advertising_String));
#endif

        while( (MapleParam.App_State == App_BLE_State) && (MapleParam.BLE_State == BLE_Advertising_State) )
        {
            /* Update the Tx Power Level of advertising data */
            /* until it is connected with the master.        */

            ret = R_BLES_advertise(&AdvParam, DUR_5S);
            //LED_Red_Toggle();
            if(ret == RBLE_CONNECTED)
            {
                LED1 = LED_ON;
                LED_Green_Blink_Off();
                LED_Green_On();
                MapleParam.BLE_State = BLE_Data_State;

#ifdef ENABLE_DEBUG_MESSAGES_BLE
                Debug_UART_Send((uint8_t*) BLE_Client_Connected_String, strlen(BLE_Client_Connected_String));
#endif
                break;
            }
            
            /* DEMO : Update manufacturer specific data in advertising data */
            AdvParam.adv_info.scan_rsp_data.data.data[6] = specific_data++;
            if(specific_data == 0xff) { specific_data = 0; }
        }
#endif/* RBLES_ADVERTISE */

#if RBLES_SCAN
        while(MapleParam.App_State == App_BLE_State)
        {
            ret = R_BLES_scan(&ScanParam, &AdvReportList, DUR_10S);
            if(ret == RBLE_TRANS_ERR)
            {
                error(1);
            }
        }
#endif/* RBLES_SCAN */

#if RBLES_CONNECT
        while(1)
        {
            ret = R_BLES_connect(&CreConParam, &ConInfo, DUR_10S);
            if(ret == RBLE_OK)
            {
                LED1 = LED_ON;
                break;
            }
        }
#endif/* RBLES_CONNECT */

#if RBLES_DATA
        //rxnum = 0;
        while ( (MapleParam.App_State == App_BLE_State) && (MapleParam.BLE_State == BLE_Data_State) )
        {
            R_BLES_get_event(&evt);
            
            if(evt == RBLES_EVENT_DISCONNECT)
            {
                /* disconnection */
                LED1 = LED_OFF;
                // Blink green led on to indicate BLE server is advertising and ready to connect
                LED_Green_Blink_On();

                MapleParam.BLE_State = BLE_Advertising_State;

#ifdef ENABLE_DEBUG_MESSAGES_BLE
                Debug_UART_Send((uint8_t*) BLE_Client_Disconnected_String, strlen(BLE_Client_Disconnected_String));
#endif
                break;
            }
            else if(evt == RBLES_EVENT_RECEIVE_DATA)
            {
                /* get BLE receive data */
            	memset(MapleParam.UART.MapleCandyReceiveBLEBuffer,0,20);
                R_BLES_receive_data(MapleParam.UART.MapleCandyReceiveBLEBuffer, 20);
                Parse_BLE_Receive_Data_Set_DAC_float();
#ifdef ENABLE_DEBUG_TX_RX_MESSAGES_BLE
                Debug_Message(">>>> Rx\n");
                Debug_Message_get_DAC_String(DAC_String);
                Debug_Message( DAC_String);
#endif
            }
            else
            {
                /* do nothing */
            }

			//Send 3 ADC measurements
			txbuf[0] = Calibrate_ADC_0_3V3(MapleParam.Measure.ADC.g_12adc_0_3V3);
			txbuf[1] = Calibrate_ADC_0_10V(MapleParam.Measure.ADC.g_12adc_0_10V);
			txbuf[2] = MapleParam.Measure.ADC.g_12adc_4_20mA;

			absDiffADC[0] = abs(MapleParam.Measure.ADC.g_Prev_12adc_0_3V3 - txbuf[0]);
			absDiffADC[1] = abs(MapleParam.Measure.ADC.g_Prev_12adc_0_10V - txbuf[1]);
			absDiffADC[2] = abs(MapleParam.Measure.ADC.g_Prev_12adc_4_20mA - txbuf[2]);
            
            if ( ( (r_send_req == TRUE) && (ADC_Send_Count!=0) ) || (DAC_Send_Count == 0) )
            {
				R_BLES_send_data((uint8_t *) txbuf, 6 );
				r_send_req = FALSE;
				if(ADC_Send_Count) {
					ADC_Send_Count--;
				}
				DAC_Send_Count = NUM_DAC_MESSAGES_TO_ENABLE_SEND;
#ifdef ENABLE_DEBUG_TX_RX_MESSAGES_BLE
				Debug_Message(">>>> Tx\n");
				Debug_Message_get_ADC_String(ADC_String);
				Debug_Message(ADC_String);
#endif
            } else if (
				(absDiffADC[0] > ADC_VARIATION_TOLERANCE_3V3) ||
				(absDiffADC[1] > ADC_VARIATION_TOLERANCE_10V) ||
				(absDiffADC[2] > ADC_VARIATION_TOLERANCE_4_20mA)
            ) {
				R_BLES_send_data((uint8_t *) txbuf, 6 );
				r_send_req = FALSE;
				ADC_Send_Count = NUM_ADC_MESSAGES_TO_ENABLE_SEND;
				DAC_Send_Count = NUM_DAC_MESSAGES_TO_ENABLE_SEND;
				MapleParam.Measure.ADC.g_Prev_12adc_0_3V3 = txbuf[0];
				MapleParam.Measure.ADC.g_Prev_12adc_0_10V = txbuf[1];
				MapleParam.Measure.ADC.g_Prev_12adc_4_20mA = txbuf[2];
#ifdef ENABLE_DEBUG_TX_RX_MESSAGES_BLE
				Debug_Message(">>>> Tx1\n");
				Debug_Message_get_ADC_String(ADC_String);
				Debug_Message(ADC_String);
#endif
        	}
            
            if(r_disconnect_req == TRUE)
            {
                R_BLES_disconnect();
                
                /* disconnection */
                r_disconnect_req = FALSE;
                LED1 = LED_OFF;
                LED_Green_Off();
    			MapleParam.App_State = App_Metal_Detect_State;
    			MapleParam.Metal_Detect_State = Metal_Detect_Change_App_State;
                break;
            }
        }
#endif/* RBLES_DATA */
    
    }
}

int __far putchar(int c){
	Debug_UART_Send((uint8_t*) &c, 1);
	return c;
}
