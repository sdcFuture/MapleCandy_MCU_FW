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
* File Name	   : rbles_api.h
* Version      : 1.0
* Description  : Header file of BLE simple API for control the RL78/G1D
*                module (RY7011).
******************************************************************************/
#ifndef _RBLES_API_H
#define _RBLES_API_H

/******************************************************************************
Includes
******************************************************************************/
#include "rble_api.h"

/******************************************************************************
Macro definitions
******************************************************************************/
#define ADV_REPORT_LIST_NUM     (30)    /* Number of advertising report */
#define WL_DEVADDR_LIST_NUM     (12)     /* Number of device address for white list */
#define RBLES_RDBUF_SIZE        (100)   /* Ring buffer size of receive data. */

/* Duration */
#define DUR_500MS               (50)    /*   50 * 10ms = 500ms */
#define DUR_1S                  (100)   /*  100 * 10ms = 1s    */
#define DUR_2S                  (200)   /*  200 * 10ms = 2s    */
#define DUR_3S                  (300)   /*  300 * 10ms = 3s    */
#define DUR_4S                  (400)   /*  400 * 10ms = 4s    */
#define DUR_5S                  (500)   /*  500 * 10ms = 5s    */
#define DUR_10S                 (1000)  /* 1000 * 10ms = 10s   */
#define DUR_20S                 (2000)  /* 2000 * 10ms = 20s   */

#define SVC_VUART_SIZE          (16)    /* VUART UUID SIZE */

enum rbles_role_enum
{
    RBLES_ROLE_MASTER,
    RBLES_ROLE_SLAVE,
    RBLES_ROLE_ADVERTISER,
    RBLES_ROLE_SCANNER,
    RBLES_ROLE_INITIATOR,
    RBLES_ROLE_NONE = 0xff
};

enum def_rbles_event
{
    RBLES_EVENT_NONE,
    RBLES_EVENT_DISCONNECT,
    RBLES_EVENT_RECEIVE_DATA
};

/******************************************************************************
Typedef definitions for API parameter
******************************************************************************/
/* Advertising parameter structure */
typedef struct RBLE_GAP_Broadcast_Enable_t {
	uint16_t			disc_mode;
	uint16_t			conn_mode;
	RBLE_ADV_INFO		adv_info;
} RBLE_BROADCAST_ENABLE_PARAM;

/* Advertising report structure */
typedef struct RBLE_ADV_REPORT_LIST_t
{
    RBLE_ADV_REPORT     adv_list[ADV_REPORT_LIST_NUM];  /* Advertising report array              */
    uint16_t            adv_rep_num;                    /* Number of received advertising report */
} RBLE_ADV_REPORT_LIST;

/* Device address structure for whitelist */
typedef struct RBLE_WLIST_DEV_ADDR_t
{
    RBLE_DEV_ADDR_INFO  dev_info;    /* Device address structure */
    bool                dev_en;      /* Device address enable flag. FALSE=Removed from whitelist/TRUE=Added to whitelist*/
} RBLE_WLIST_DEV_ADDR;

/* Whitelist structure */
typedef struct RBLE_WHITELIST_t
{
    RBLE_WLIST_DEV_ADDR dev_list[WL_DEVADDR_LIST_NUM];
} RBLE_WHITELIST;

/******************************************************************************
Exported global functions
******************************************************************************/
RBLE_STATUS R_BLES_initialize(void);
RBLE_STATUS R_BLES_advertise(RBLE_BROADCAST_ENABLE_PARAM *adv_param, uint16_t duration);
RBLE_STATUS R_BLES_scan(RBLE_SCANNING_INFO *scan_param, RBLE_ADV_REPORT_LIST *adv_report, uint16_t duration);
RBLE_STATUS R_BLES_connect(RBLE_CREATE_CONNECT_PARAM *conn_param, RBLE_CONNECT_INFO *conn_info, uint16_t duration);
RBLE_STATUS R_BLES_disconnect(void);
RBLE_STATUS R_BLES_whitelist(RBLE_WHITELIST *whitelist);
RBLE_STATUS R_BLES_send_data(uint8_t *txbuf, uint8_t len);
uint16_t R_BLES_receive_data(uint8_t *rxbuf, uint16_t len);
RBLE_STATUS R_BLES_get_event(uint8_t *evt);

#endif /* _RBLES_API_H */
