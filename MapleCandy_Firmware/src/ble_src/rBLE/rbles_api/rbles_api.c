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
* File Name    : rbles_api.c
* Version      : 1.0
* Description  : C source file of BLE simple API for control the RL78/G1D
*                module (RY7011).
******************************************************************************/

/******************************************************************************
Includes
******************************************************************************/
#include "rble_host.h"
#include "rbles_api.h"
#include "vuartc.h"
#include "vuarts.h"
#include "timer.h"
#include "rscip_uart.h"
#include <stdlib.h>
#include "r_cg_userdefine.h"



/******************************************************************************
Macro definitions
******************************************************************************/
#define STATIC              static
#define TIME_OUT_TIMER_2s   (200)       /* Communication timeout count between Host MCU and BLE MCU. */
#define TIME_OUT_TIMER_5s   (500)
#define DUR_MAX             (60000)     /* Maximum duration value (60000 * 10msec = 600 sec) */

/******************************************************************************
Typedef definitions
******************************************************************************/
/* Ring buffer structure of receive data */
typedef struct RBLE_RDBUF_CNTL_t
{
    int16_t     p_rd;                       /* Read point */
    int16_t     p_wr;                       /* Write point */
    uint16_t    dnum;                       /* Number of data */
    uint8_t     rdbuf[RBLES_RDBUF_SIZE];    /* Reception data buffer */
} RBLE_RDBUF_CNTL;

/* Information structure */
typedef struct RBLE_SAPI_INFO_t
{
    uint16_t        conhdl;                 /* Connection handle */
    uint8_t         role;                   /* Local device role */
    uint8_t         idx;                    /* Connection index  */

    uint16_t        ediv;                   /* Encrypted Diversifier */
    RBLE_RAND_NB    rand_nb;                /* Random Number         */
    RBLE_KEY_VALUE  ltk;                    /* Long Term Key         */

    uint16_t        seq;                    /* API Sequence */
    uint8_t         get_event;              /* Store event occurred by R_BLES_get_event() */
    uint8_t         get_event_s;            /* Store event occurred by R_BLES_send_data() */
    uint8_t         vs_fm_cmd;              /* Vendor specific flash management command parameter */
    uint8_t         event_status;           /* Status of rBLE event */
    uint8_t         advl_full;              /* Advertising list full flag */
} RBLE_SAPI_INFO;

/* API Sequence definition */
enum rbles_seq_enum
{
    RBLES_SEQ_NONE,
    
    /* -- R_BLES_initialize() -- */
    RBLES_SEQ_GAP_RESET,
    RBLES_SEQ_SET_PERMISSION,
    
    /* -- R_BLES_advertise() -- */
    RBLES_SEQ_GAP_BROADCAST_ENABLE,
    RBLES_SEQ_GAP_BROADCAST_DISABLE,
    RBLES_SEQ_VUART_SERVER_ENABLE,
    RBLES_SEQ_GAP_BONDING_RESPONSE,
    RBLES_SEQ_SM_LTK_REQ_RESP_IND,              /* respond to RBLE_SM_LTK_REQ_IND event */
    RBLES_SEQ_SM_LTK_REQ_RESP_ENC_IND,          /* respond to RBLE_SM_LTK_REQ_FOR_ENC_IND event */
    
    /* -- R_BLES_scan() -- */
    RBLES_SEQ_GAP_OBSERVATION_ENABLE,
    RBLES_SEQ_GAP_OBSERVATION_DISABLE,
    
    /* -- R_BLES_connect() -- */
    RBLES_SEQ_GAP_SET_BONDING_MODE,
    RBLES_SEQ_GAP_CREATE_CONNECTION,
    RBLES_SEQ_GAP_START_BONDING,
    RBLES_SEQ_SM_START_ENC,
    RBLES_SEQ_VUART_CLIENT_ENABLE,
    RBLES_SEQ_GAP_CHANGE_CONNECTION_PARAM_CONN,
    RBLES_SEQ_GAP_CONNECTION_CANCEL,
    
    /* -- R_BLES_disconnect() -- */
    RBLES_SEQ_GAP_DISCONNECT,
    
    /* -- R_BLES_send_data() -- */
    RBLES_SEQ_VUART_SEND,
    
    /* -- R_BLES_whitelist() -- */
    RBLES_SEQ_GAP_DEL_FROM_WHITE_LIST,
    RBLES_SEQ_GAP_ADD_TO_WHITE_LIST,
    
    RBLES_SEQ_EXIT
};

/******************************************************************************
Private global variables and functions
******************************************************************************/
STATIC RBLE_SAPI_INFO           r_sapi_info;        /* Information structure */
STATIC RBLE_RDBUF_CNTL          r_rdbuf_cntl;       /* Ring buffer structure */
STATIC RBLE_ADV_REPORT_LIST     *r_adv_report;      /* Advertising report list pointer for R_BLE_scan() */
STATIC RBLE_CONNECT_INFO        *r_conn_info;       /* Connection information */
STATIC uint8_t                  r_wlst_num;         /* Number of white lists */
STATIC uint8_t                  r_wlst_cnt;         /* White list count */
STATIC BOOL                     r_scan_vuart;       /* Scan parameter RLBE_SCAN_ALLOW_VUART_SRV flag */
STATIC BOOL                     r_gap_chg_con_param_req_ind;    /* Change connection parameter flag */
STATIC BOOL                     timeout_scan_dur;   /* Timeout flag of R_BLE_scan() */
STATIC BOOL r_rble_gap_disconnect_execution;
STATIC BOOL                     r_conn_timeout;
STATIC BOOL                     r_perm_notexec;     /* Executin flag of RBLE_GATT_Set_Permission() */

//const RBLE_BOND_RESP_PARAM r_bond_resp_param = {
STATIC const RBLE_BOND_RESP_PARAM r_bond_resp_param = {
    0,                                  /* conhdl */
    RBLE_OK,                            /* accept */
    RBLE_IO_CAP_NO_INPUT_NO_OUTPUT,     /* io_cap */
    RBLE_OOB_AUTH_DATA_NOT_PRESENT,     /* oob */
    RBLE_AUTH_REQ_NO_MITM_BOND,         /* auth_req */
    RBLE_KEY_LEN,                       /* max_key_size */
    RBLE_KEY_DIST_NONE,                 /* ikeys */
    RBLE_KEY_DIST_ENCKEY,               /* rkeys */
    0,                                  /* reserved */
};

STATIC const RBLE_BOND_PARAM r_bond_param = {
    {0},
    RBLE_OOB_AUTH_DATA_NOT_PRESENT,     /* oob */
    RBLE_IO_CAP_NO_INPUT_NO_OUTPUT,     /* iocap */
    RBLE_AUTH_REQ_NO_MITM_BOND,         /* auth */
    RBLE_KEY_LEN,                       /* key_size */
    RBLE_KEY_DIST_NONE,                 /* ikey_dist */
    RBLE_KEY_DIST_ENCKEY,               /* rkey_dist */
};

STATIC void r_bles_timeout_scan_duration(void);
STATIC void r_bles_timeout_connect_duration(void);
STATIC void r_bles_timeout_advertise_duration(void);
STATIC void r_bles_timeout_communication(void);
STATIC void r_bles_set_sequence(uint8_t seq);

STATIC void RBLE_MODE_callback(RBLE_MODE mode);
STATIC void RBLE_GAP_EVENT_callback(RBLE_GAP_EVENT *event);
STATIC void RBLE_SM_EVENT_callback(RBLE_SM_EVENT *event);
STATIC void RBLE_GATT_EVENT_callback(RBLE_GATT_EVENT *event);
#if 0
STATIC void RBLE_VS_EVENT_callback(RBLE_VS_EVENT *event);
#endif
STATIC void RBLE_VUARTS_callback(RBLE_VUART_EVENT *event);
STATIC void RBLE_VUARTC_callback(RBLE_VUART_EVENT *event);

STATIC void cbsub_rble_mode_initialize(void);
STATIC void cbsub_rble_mode_active(void);
STATIC void cbsub_rble_mode_reset(void);
STATIC void cbsub_rble_mode_sleep(void);
STATIC void cbsub_rble_mode_error(void);

STATIC void cbsub_rble_gap_event_reset_result(RBLE_GAP_EVENT *event);
STATIC void cbsub_rble_gap_event_observation_enable_comp(RBLE_GAP_EVENT *event);
STATIC void cbsub_rble_gap_event_observation_disable_comp(RBLE_GAP_EVENT *event);
STATIC void cbsub_rble_gap_event_broadcast_enable_comp(RBLE_GAP_EVENT *event);
STATIC void cbsub_rble_gap_event_broadcast_disable_comp(RBLE_GAP_EVENT *event);
STATIC void cbsub_rble_gap_event_set_bonding_mode_comp(RBLE_GAP_EVENT *event);
STATIC void cbsub_rble_gap_event_add_to_white_list_comp(RBLE_GAP_EVENT *event);
STATIC void cbsub_rble_gap_event_del_from_white_list_comp(RBLE_GAP_EVENT *event);

STATIC void cbsub_rble_gap_event_connection_comp(RBLE_GAP_EVENT *event);
STATIC void cbsub_rble_gap_event_connection_cancel_comp(RBLE_GAP_EVENT *event);
STATIC void cbsub_rble_gap_event_disconnect_comp(RBLE_GAP_EVENT *event);
STATIC void cbsub_rble_gap_event_advertising_report_ind(RBLE_GAP_EVENT *event);
STATIC void cbsub_rble_gap_event_bonding_comp(RBLE_GAP_EVENT *event);
STATIC void cbsub_rble_gap_event_bonding_req_ind(RBLE_GAP_EVENT *event);
STATIC void cbsub_rble_gap_event_change_connection_param_req_ind(RBLE_GAP_EVENT *event);
STATIC void cbsub_rble_gap_event_change_connection_param_comp(RBLE_GAP_EVENT *event);

STATIC void cbsub_rble_sm_enc_start_ind(RBLE_SM_EVENT *event);
STATIC void cbsub_rble_sm_ltk_req_ind(RBLE_SM_EVENT *event);
STATIC void cbsub_rble_sm_ltk_req_for_enc_ind(RBLE_SM_EVENT *event);
STATIC void cbsub_rble_sm_key_ind(RBLE_SM_EVENT *event);

STATIC void cbsub_rble_gatt_event_set_perm_cmp(RBLE_GATT_EVENT *event);
STATIC void cbsub_rble_vuart_event_server_enable_cmp(RBLE_VUART_EVENT *event);
STATIC void cbsub_rble_vuart_event_server_write_req(RBLE_VUART_EVENT *event);
STATIC void cbsub_rble_vuart_event_server_indication_cfm(RBLE_VUART_EVENT *event);
STATIC void cbsub_rble_vuart_event_client_enable_cmp(RBLE_VUART_EVENT *event);
STATIC void cbsub_rble_vuart_event_client_indication(RBLE_VUART_EVENT *event);
STATIC void cbsub_rble_vuart_event_client_write_rsp(RBLE_VUART_EVENT *event);


/******************************************************************************
BLE Simple API functions
******************************************************************************/

/******************************************************************************
* Function Name: R_BLES_initialize
*
* Description  : Reset the GAP. And register event callback functions of GAP
*                and SM. Hide profiles other than VUART in the BLE_MCU from 
*                the remote device.
*
* Arguments    : none
*
* Return Value : RBLE_STATUS - 
*                    Return value of rBLE API.
*                    RBLE_OK           : Success.
*                    RBLE_STATUS_ERROR : Not executable because the rBLE mode
*                                        is other than RBLE_MODE_ACTIVE.
*                    RBLE_TRANS_ERR    : Communication timeout between Host MCU
*                                        and BLE MCU.
*                    RBLE_ERR          : Sequence error.
******************************************************************************/
RBLE_STATUS R_BLES_initialize(void)
{
    RBLE_STATUS                 ret, sts;
    RBLE_GATT_SET_PERM          gatt_set_permission;
    
    /* Initialize BLE role */
    r_sapi_info.role            = RBLES_ROLE_NONE;
    
    /* Initialize reception data buffer control structure */
    r_rdbuf_cntl.p_rd = 0;
    r_rdbuf_cntl.p_wr = 0;
    r_rdbuf_cntl.dnum = 0;
    
    /* Initialize RBLE_GATT_Set_Permission() parameters */
    r_perm_notexec = FALSE;
    gatt_set_permission.start_hdl = (GATT_HDL_SVC_CHANGED_IND_CFG + 1);
    gatt_set_permission.end_hdl   = (VUART_HDL_SVC - 1);
    gatt_set_permission.perm      = RBLE_GATT_PERM_HIDE;
    if(gatt_set_permission.start_hdl == VUART_HDL_SVC)
    {
        r_perm_notexec = TRUE;
    }
    
    /* Initialize return value */
    ret                      = RBLE_OK;
    r_sapi_info.event_status = RBLE_OK;
    
    /* Initialize RSCIP and Link establishment with BLE MCU */
    RBLE_Init_Timer();
    
    if(RBLE_Init(&RBLE_MODE_callback) != RBLE_OK)
    {
        return(RBLE_ERR);
    }
    
    /* Communication timeout timer between Host MCU and BLE MCU. */
    RBLE_Set_Timer_Id(_RBLE_TIMER_ID_RBLES, (RBLE_FUNC)r_bles_timeout_communication,
                      TIME_OUT_TIMER_2s);
    
    r_sapi_info.seq = RBLES_SEQ_NONE; /* starting state of R_BLES_gap_reset() */
    
    while(1)
    {
        if(r_sapi_info.seq != RBLES_SEQ_NONE)
        {
            switch(r_sapi_info.seq)
            {
                case RBLES_SEQ_GAP_RESET:
                    ret = RBLE_GAP_Reset(&RBLE_GAP_EVENT_callback, &RBLE_SM_EVENT_callback);
                    break;
                    
                case RBLES_SEQ_SET_PERMISSION:
                    ret = RBLE_GATT_Enable(&RBLE_GATT_EVENT_callback);
                    /* Go through. Because RBLE_GATT_Enable() does not occur event. */
                    if(ret == RBLE_OK)
                    {
                        ret = RBLE_GATT_Set_Permission(&gatt_set_permission);
                    }
                    break;
                    
                default:
                    break;
            }
            r_sapi_info.seq = RBLES_SEQ_NONE;
        }
        /* rBLE process.                                  */
        /* Sending rBLE commands and generate rBLE event. */
        rBLE_Run();
        
        /* return value of rBLE API */
        if(ret != RBLE_OK)
        {
            if((ret != RBLE_STATUS_ERROR) && (ret != RBLE_TRANS_ERR))
            {
                ret = RBLE_ERR;
            }
            break;
        }
        /* return status of rBLE Event */
        sts = r_sapi_info.event_status;
        if(sts != RBLE_OK)
        {
            if((sts != RBLE_STATUS_ERROR) && (sts != RBLE_TRANS_ERR))
            {
                ret = RBLE_ERR;
            }
            else
            {
                ret = sts;
            }
            break;
        }
        /* exit a sequence loop */
        if(r_sapi_info.seq == RBLES_SEQ_EXIT)
        {
            break;
        }
    }
    
    /* Stop timeout timer */
    RBLE_Clear_Timer_Id(_RBLE_TIMER_ID_RBLES);
    
    return(ret);
}

/******************************************************************************
* Function Name: R_BLES_advertise
*
* Description  : Advertising is executed for a fixed time by setting the
*                advertising duration. In case of advertising of connectable
*                mode, it can connect with a remote device.
*
* Arguments    : *adv_param - 
*                    Pointer of advertising parameter.
*                duration - 
*                    Duration during which the advertising is executed.
*
* Return Value : RBLE_STATUS - 
*                    Return value of rBLE API.
*                    RBLE_OK           : Success.
*                    RBLE_PARAM_ERR    : Invalid parameter.
*                    RBLE_STATUS_ERROR : Not executable because the rBLE mode
*                                        is other than RBLE_MODE_ACTIVE.
*                    RBLE_TRANS_ERR    : Communication timeout between Host MCU
*                                        and BLE MCU.
*                    RBLE_ADVLIST_FULL : Advertising list is full.
*                    RBLE_CONNECTED    : Connection establishment with remote device.
*                    RBLE_ERR          : Sequence error.
******************************************************************************/
RBLE_STATUS R_BLES_advertise(RBLE_BROADCAST_ENABLE_PARAM *adv_param, 
                             uint16_t duration)
{
    RBLE_STATUS                 ret, sts;
    RBLE_STATUS                 ret_api, ret_evt;
    RBLE_BOND_RESP_PARAM        brp;
    RBLE_BROADCAST_ENABLE_PARAM ap;
    uint32_t                    cnt;
    uint8_t                     i;
    
    /* Parameter check */
    if((adv_param == NULL) || (duration == 0) || (duration > DUR_MAX))
    {
        return(RBLE_PARAM_ERR);
    }
    
    /* Initialize BLE role */
    r_sapi_info.role = RBLES_ROLE_ADVERTISER;
    
    memcpy(&ap, adv_param, sizeof(RBLE_BROADCAST_ENABLE_PARAM));
    memcpy(&brp, &r_bond_resp_param, sizeof(RBLE_BOND_RESP_PARAM));
    
    /* Calclate advertising interval count */
    /* 20ms - 10240ms                        */
    cnt = ap.adv_info.adv_param.adv_intv_max;
    cnt = (cnt * 1000) / 625;
    ap.adv_info.adv_param.adv_intv_min = (uint16_t)cnt;
    ap.adv_info.adv_param.adv_intv_max = (uint16_t)cnt;
    
    /* Initialize return value */
    ret                      = RBLE_OK;
    r_sapi_info.event_status = RBLE_OK;
    
    /* Generate key */
    r_sapi_info.ediv = (uint16_t)rand();
    for(i = 0; i < RBLE_RAND_NB_LEN; i++)
    {
        r_sapi_info.rand_nb.nb[i] = (uint8_t)rand();
    }
    for(i = 0; i < RBLE_KEY_LEN; i++)
    {
        r_sapi_info.ltk.key[i]    = (uint8_t)rand();
    }
    
    /* Communication timeout timer between Host MCU and BLE MCU. */
    RBLE_Set_Timer_Id(_RBLE_TIMER_ID_RBLES, (RBLE_FUNC)r_bles_timeout_communication,
                      (duration + TIME_OUT_TIMER_2s));
    /* Advertising timeout timer. */
    RBLE_Set_Timer_Id(_RBLE_TIMER_ID_RBLES_ADV_DUR, (RBLE_FUNC)r_bles_timeout_advertise_duration,
                      duration);
    
    r_sapi_info.seq = RBLES_SEQ_GAP_BROADCAST_ENABLE; /* starting state of R_BLES_scan() */
    
    while(1)
    {
        if(r_sapi_info.seq != RBLES_SEQ_NONE)
        {
            switch(r_sapi_info.seq)
            {
                case RBLES_SEQ_GAP_BROADCAST_ENABLE:
                    ret = RBLE_GAP_Broadcast_Enable(ap.disc_mode, ap.conn_mode,
                                                    &(ap.adv_info));
                    break;
                    
                case RBLES_SEQ_GAP_BROADCAST_DISABLE:
                    ret = RBLE_GAP_Broadcast_Disable();
                    break;
                    
                case RBLES_SEQ_VUART_SERVER_ENABLE:
                    ret = RBLE_VUART_Server_Enable(r_sapi_info.conhdl, RBLE_VUARTS_callback);
                    break;
                    
                case RBLES_SEQ_GAP_BONDING_RESPONSE:
                    brp.conhdl = r_sapi_info.conhdl;
                    ret = RBLE_GAP_Bonding_Response(&brp);
                    break;
                    
                case RBLES_SEQ_SM_LTK_REQ_RESP_IND:
                    ret = RBLE_SM_Ltk_Req_Resp(r_sapi_info.idx, RBLE_OK, RBLE_SMP_KSEC_UNAUTH_NO_MITM,
                                               r_sapi_info.ediv, &r_sapi_info.rand_nb, &r_sapi_info.ltk);
                    break;
                    
                case RBLES_SEQ_SM_LTK_REQ_RESP_ENC_IND:
                    ret = RBLE_SM_Ltk_Req_Resp(r_sapi_info.idx, RBLE_OK, RBLE_SMP_KSEC_UNAUTH_NO_MITM,
                                               r_sapi_info.ediv, &r_sapi_info.rand_nb, &r_sapi_info.ltk);
                    break;
                    
                case RBLES_SEQ_GAP_DISCONNECT:
                    ret = RBLE_GAP_Disconnect(r_sapi_info.conhdl);
                    break;
                    
                default:
                    break;
            }
            r_sapi_info.seq = RBLES_SEQ_NONE;
        }
        
        /* rBLE process.                                  */
        /* Sending rBLE commands and generate rBLE event. */
        rBLE_Run();
        
        /* When error occurred, after connection establishment as a slave. */
        if((r_sapi_info.role == RBLES_ROLE_SLAVE) && 
           ((ret != RBLE_OK) || (r_sapi_info.event_status != RBLE_OK)))
        {
            r_sapi_info.seq = RBLES_SEQ_GAP_DISCONNECT;
            
            /* backup error value */
            /* after disconnect from remote device, restore it */
            ret_api = ret;
            ret_evt = r_sapi_info.event_status;
            
            ret = RBLE_OK;
            r_sapi_info.event_status = RBLE_OK;
        }
        else if(r_sapi_info.role == RBLES_ROLE_NONE)
        {
            /* restore error value */
            /* move to RBLES_ROLE_NONE state in RBLE_GAP_EVENT_DISCONNECT_COMP event */
            ret = ret_api;
            r_sapi_info.event_status = ret_evt;
        }
        
        /* return value of rBLE API */
        if(ret != RBLE_OK)
        {
            if((ret != RBLE_STATUS_ERROR) && (ret != RBLE_TRANS_ERR))
            {
                ret = RBLE_ERR;
            }
            break;
        }
        /* return status of rBLE Event */
        sts = r_sapi_info.event_status;
        if(sts != RBLE_OK)
        {
            if((sts != RBLE_STATUS_ERROR) && (sts != RBLE_TRANS_ERR))
            {
                ret = RBLE_ERR;
            }
            else
            {
                ret = sts;
            }
            break;
        }
        /* exit a sequence loop */
        if(r_sapi_info.seq == RBLES_SEQ_EXIT)
        {
            if((ret == RBLE_OK) && (r_sapi_info.role == RBLES_ROLE_SLAVE))
            {
                ret = RBLE_CONNECTED;
            }
            break;
        }
    }
    
    /* Stop timeout timer */
    RBLE_Clear_Timer_Id(_RBLE_TIMER_ID_RBLES);
    RBLE_Clear_Timer_Id(_RBLE_TIMER_ID_RBLES_ADV_DUR);
    
    return(ret);
}

/******************************************************************************
* Function Name: R_BLES_scan
*
* Description  : Scanning is executed for a fixed time by setting the scanning
*                duration. Stores the Advertising report that received during
*                Scanning in *adv_report. When adv_report->adv_list[] is full,
*                scanning is terminated.
*
* Arguments    : *scan_param - 
*                    Pointer of scanning parameter.
*                *adv_report - 
*                    Pointer of advertising report list.
*                duration - 
*                    Duration during which the scanning is executed.
*
* Return Value : RBLE_STATUS - 
*                    Return value of rBLE API.
*                    RBLE_OK           : Success.
*                    RBLE_PARAM_ERR    : Invalid parameter.
*                    RBLE_STATUS_ERROR : Not executable because the rBLE mode
*                                        is other than RBLE_MODE_ACTIVE.
*                    RBLE_TRANS_ERR    : Communication timeout between Host MCU
*                                        and BLE MCU.
*                    RBLE_ADVLIST_FULL : Advertising list is full.
*                    RBLE_ERR          : Sequence error.
******************************************************************************/
RBLE_STATUS R_BLES_scan(RBLE_SCANNING_INFO *scan_param,
                              RBLE_ADV_REPORT_LIST *adv_report, uint16_t duration)
{
    RBLE_STATUS         ret, sts;
    RBLE_SCANNING_INFO  sp;
    uint32_t            cnt;
    
    /* Parameter check */
    if((scan_param == NULL) || (adv_report == NULL) || (duration == 0) || (duration > DUR_MAX))
    {
        return(RBLE_PARAM_ERR);
    }
    
    /* Initialize BLE role */
    r_sapi_info.role = RBLES_ROLE_SCANNER;
    
    /* Notify only device addresses with vuart service */
    r_scan_vuart = ((scan_param->set_scan.scan_filt_policy == RLBE_SCAN_ALLOW_VUART_SRV) ? TRUE : FALSE);
    
    /* Initialize return value */
    ret                      = RBLE_OK;
    r_sapi_info.event_status = RBLE_OK;
    
    /* Initialize advertising report list full flag */
    r_sapi_info.advl_full    = FALSE;
    
    /* Prevent twice calling of RBLE_GAP_Observation_Disable()               */
    /* due to conflict of scan duration timeout and advertising report full. */
    timeout_scan_dur         = FALSE;
    
    /* Initialize advertising report list */
    r_adv_report              = adv_report;  /* copy to global valuable */
    r_adv_report->adv_rep_num = 0;
    
    /* Copy function parameter to local value for calculate scan interval and scan window */
    memcpy(&sp, scan_param, sizeof(RBLE_SCANNING_INFO));
    
    /* Calculate scan interval count and scan window count */
    cnt = sp.set_scan.scan_intv;
    cnt = cnt * 1000;
    if(cnt <= 2000)
    {
       cnt = 2500; 
    }
    sp.set_scan.scan_intv   = (uint16_t)(cnt / 625);
    
    cnt = sp.set_scan.scan_window;
    cnt = cnt * 1000;
    if(cnt <= 2000)
    {
       cnt = 2500; 
    }
    sp.set_scan.scan_window = (uint16_t)(cnt / 625);
    
    /* Communication timeout timer between Host MCU and BLE MCU. */
    RBLE_Set_Timer_Id(_RBLE_TIMER_ID_RBLES, (RBLE_FUNC)r_bles_timeout_communication,
                      (duration + TIME_OUT_TIMER_2s));
    RBLE_Set_Timer_Id(_RBLE_TIMER_ID_RBLES_SCAN_DUR, (RBLE_FUNC)r_bles_timeout_scan_duration,
                      duration);
    
    r_sapi_info.seq = RBLES_SEQ_GAP_OBSERVATION_ENABLE; /* starting state of R_BLES_scan() */
    
    while(1)
    {
        if(r_sapi_info.seq != RBLES_SEQ_NONE)
        {
            switch(r_sapi_info.seq)
            {
                case RBLES_SEQ_GAP_OBSERVATION_ENABLE:
                    ret = RBLE_GAP_Observation_Enable(RBLE_GAP_OBSERVER, &sp);
                    break;
                    
                case RBLES_SEQ_GAP_OBSERVATION_DISABLE:
                    ret = RBLE_GAP_Observation_Disable();
                    break;
                    
                default:
                    break;
            }
            r_sapi_info.seq = RBLES_SEQ_NONE;
        }
        
        /* rBLE process.                                  */
        /* Sending rBLE commands and generate rBLE event. */
        rBLE_Run();
        
        /* return value of rBLE API */
        if(ret != RBLE_OK)
        {
            if((ret != RBLE_STATUS_ERROR) && (ret != RBLE_TRANS_ERR))
            {
                ret = RBLE_ERR;
            }
            break;
        }
        /* return status of rBLE Event */
        sts = r_sapi_info.event_status;
        if(sts != RBLE_OK)
        {
            if((sts != RBLE_STATUS_ERROR) && (sts != RBLE_TRANS_ERR) && (sts != RBLE_ADVLIST_FULL))
            {
                ret = RBLE_ERR;
            }
            else
            {
                ret = sts;
            }
            break;
        }
        /* exit a sequence loop */
        if(r_sapi_info.seq == RBLES_SEQ_EXIT)
        {
            break;
        }
    }
    
    /* Stop timeout timer */
    RBLE_Clear_Timer_Id(_RBLE_TIMER_ID_RBLES);
    RBLE_Clear_Timer_Id(_RBLE_TIMER_ID_RBLES_SCAN_DUR);
    
    return(ret);
}

/******************************************************************************
* Function Name: R_BLES_connect
*
* Description  : This function executes create connection. Create connection is
*                executed for a fixed time by setting the create connection
*                duration. After connecting, executes the service discovery of
*                the remote device. If the general purpose communication profile
*                (Virtual uart profile) can not be found, disconnect it.
*
* Arguments    : *conn_param - 
*                    Pointer of create connection parameter.
*                *conn_info - 
*                    Pointer of connection information.
*                duration - 
*                    Duration during which the connection is executed.
*
* Return Value : RBLE_STATUS - 
*                    Return value of rBLE API.
*                    RBLE_OK           : Success.
*                    RBLE_PARAM_ERR    : Invalid parameter.
*                    RBLE_STATUS_ERROR : Not executable because the rBLE mode
*                                        is other than RBLE_MODE_ACTIVE.
*                    RBLE_TRANS_ERR    : Communication timeout between Host MCU
*                                        and BLE MCU.
*                    RBLE_TIMEOUT      : Create connection timeout.
*                    RBLE_ERR          : Sequence error.
******************************************************************************/
RBLE_STATUS R_BLES_connect(RBLE_CREATE_CONNECT_PARAM *conn_param, 
                           RBLE_CONNECT_INFO *conn_info, uint16_t duration)
{
    RBLE_STATUS                 ret, sts;
    RBLE_CREATE_CONNECT_PARAM   cp;                 /* connection param */
    RBLE_CONN_PARAM             ccp;                /* change connection param */
    RBLE_BOND_PARAM             bp;
    RBLE_STATUS                 ret_api, ret_evt;
    uint32_t                    cnt;
    
    /* Parameter check */
    if((conn_param == NULL) || (conn_info == NULL) || (duration == 0) || (duration > DUR_MAX))
    {
        return(RBLE_PARAM_ERR);
    }
    
    /* Connection timeout flag */
    r_conn_timeout = FALSE;
    
    /* Initialize BLE role */
    r_sapi_info.role = RBLES_ROLE_INITIATOR;
    
    /* Initialize return value */
    ret                      = RBLE_OK;
    r_sapi_info.event_status = RBLE_OK;
    
    r_gap_chg_con_param_req_ind = FALSE;
    
    /* Initialize connection information */
    r_conn_info = conn_info;
    
    /* Copy function parameter to local value                                                */
    /* for calculate scan interval, scan window, connection interval and supervision timeout */
    memcpy(&cp, conn_param, sizeof(RBLE_CREATE_CONNECT_PARAM));
    memcpy(&bp, &r_bond_param, sizeof(RBLE_BOND_PARAM));
    
    /* Calculate scan interval count and scan window count (2500us(2.5ms) - 10240000us(10.24s)) */
    cnt = cp.scan_intv;
    cnt = cnt * 1000;
    if(cnt <= 2000)
    {
       cnt = 2500; 
    }
    cp.scan_intv   = (uint16_t)(cnt / 625);

    cnt = cp.scan_window;
    cnt = cnt * 1000;
    if(cnt <= 2000)
    {
       cnt = 2500; 
    }
    cp.scan_window = (uint16_t)(cnt / 625);
    
    /* Calculate connection interval count (7500us(7.5ms) - 4000000us(4s)) */
    cnt = cp.con_intv_max;
    cnt = cnt * 1000;
    if(cnt <= 7000)
    {
       cnt = 7500; 
    }
    cp.con_intv_max = (uint16_t)(cnt / 1250);
    cp.con_intv_min = cp.con_intv_max;
    
    /* Calculate super vision timeout count (100ms - 32000ms) */
    cp.superv_to   = cp.superv_to / 10;
    
    /* Communication timeout timer between Host MCU and BLE MCU. */
    RBLE_Set_Timer_Id(_RBLE_TIMER_ID_RBLES, (RBLE_FUNC)r_bles_timeout_communication,
                      (duration + TIME_OUT_TIMER_2s));
    RBLE_Set_Timer_Id(_RBLE_TIMER_ID_RBLES_CONNECT_DUR, (RBLE_FUNC)r_bles_timeout_connect_duration,
                      duration);
    
    r_sapi_info.seq = RBLES_SEQ_GAP_SET_BONDING_MODE; /* starting state of R_BLES_connect() */
    
    while(1)
    {
        if(r_sapi_info.seq != RBLES_SEQ_NONE)
        {
            switch(r_sapi_info.seq)
            {
                case RBLES_SEQ_GAP_SET_BONDING_MODE:
                    ret = RBLE_GAP_Set_Bonding_Mode(RBLE_GAP_BONDABLE);
                    break;
                    
                case RBLES_SEQ_GAP_CREATE_CONNECTION:
                    ret = RBLE_GAP_Create_Connection(&cp);
                    break;
                    
                case RBLES_SEQ_GAP_START_BONDING:
                    /* copy remote device address */
                    memcpy(&bp.addr, &conn_info->peer_addr, sizeof(RBLE_BD_ADDR));
                    ret = RBLE_GAP_Start_Bonding(&bp);
                    break;
                    
                case RBLES_SEQ_SM_START_ENC:
                    ret = RBLE_SM_Start_Enc(r_sapi_info.idx, RBLE_AUTH_REQ_NO_MITM_BOND, 
                                            r_sapi_info.ediv, &r_sapi_info.rand_nb, &r_sapi_info.ltk);
                    break;
                    
                case RBLES_SEQ_VUART_CLIENT_ENABLE:
                    ret = RBLE_VUART_Client_Enable(r_sapi_info.conhdl, RBLE_VUARTC_callback);
                    break;
                    
                case RBLES_SEQ_GAP_CHANGE_CONNECTION_PARAM_CONN:
                    /* refuse change_connection_param_req_ind from slave       */
                    /* copy create connection param to change connection param */
                    ccp.intv_min = cp.con_intv_min;
                    ccp.intv_max = cp.con_intv_max;
                    ccp.latency  = cp.con_latency;
                    ccp.time_out = cp.superv_to;
                    ret = RBLE_GAP_Change_Connection_Param(r_sapi_info.conhdl, (uint16_t)0x0001,
                                                           &ccp, r_sapi_info.role);
                    break;
                    
                case RBLES_SEQ_GAP_DISCONNECT:
                    ret = RBLE_GAP_Disconnect(r_sapi_info.conhdl);
                    break;
                    
                case RBLES_SEQ_GAP_CONNECTION_CANCEL:
                    ret = RBLE_GAP_Connection_Cancel();
                    break;
                    
                default:
                    break;
            }
            r_sapi_info.seq = RBLES_SEQ_NONE;
        }

        /* rBLE process.                                  */
        /* Sending rBLE commands and generate rBLE event. */
        rBLE_Run();
        
        /* When error occurred, after connection establishment as a master. */
        if((r_sapi_info.role == RBLES_ROLE_MASTER) && 
           ((ret != RBLE_OK) || (r_sapi_info.event_status != RBLE_OK)))
        {
            r_sapi_info.seq = RBLES_SEQ_GAP_DISCONNECT;
            
            /* backup error value */
            /* after disconnect from remote device, restore it */
            ret_api = ret;
            ret_evt = r_sapi_info.event_status;
            
            ret = RBLE_OK;
            r_sapi_info.event_status = RBLE_OK;
        }
        else if(r_sapi_info.role == RBLES_ROLE_NONE)
        {
            /* restore error value */
            /* move to RBLES_ROLE_NONE state in RBLE_GAP_EVENT_DISCONNECT_COMP event */
            ret = ret_api;
            r_sapi_info.event_status = ret_evt;
        }
        
        /* return value of rBLE API */
        if(ret != RBLE_OK)
        {
            if((ret != RBLE_STATUS_ERROR) && (ret != RBLE_TRANS_ERR))
            {
                ret = RBLE_ERR;
            }
            break;
        }
        /* return status of rBLE Event */
        sts = r_sapi_info.event_status;
        if(sts != RBLE_OK)
        {
            if(sts == RBLE_UNKNOWN_CONNECTION_ID)
            {
                /* create connection was timeout */
                ret = RBLE_TIMEOUT;
            }
            else if((sts != RBLE_STATUS_ERROR) && (sts != RBLE_TRANS_ERR) && (sts != RBLE_TIMEOUT))
            {
                ret = RBLE_ERR;
            }
            else
            {
                ret = sts;
            }
            break;
        }
        /* exit a sequence loop */
        if(r_sapi_info.seq == RBLES_SEQ_EXIT)
        {
            break;
        }
    }
    
    /* Stop timeout timer */
    RBLE_Clear_Timer_Id(_RBLE_TIMER_ID_RBLES);
    
    return(ret);
}

/******************************************************************************
* Function Name: R_BLES_disconnect
*
* Description  : This function disconnects the link with the remote device.
*
* Arguments    : none
*
* Return Value : RBLE_STATUS - 
*                    Return value of rBLE API.
*                    RBLE_OK           : Success.
*                    RBLE_STATUS_ERROR : Not executable because the rBLE mode
*                                        is other than RBLE_MODE_ACTIVE.
*                    RBLE_TRANS_ERR    : Communication timeout between Host MCU
*                                        and BLE MCU.
*                    RBLE_ERR          : Sequence error.
******************************************************************************/
RBLE_STATUS R_BLES_disconnect(void)
{
    RBLE_STATUS ret, sts;
    
    r_rble_gap_disconnect_execution = TRUE;
    /* Initialize BLE role */
    r_sapi_info.role   = RBLES_ROLE_NONE;
    
    /* Initialize return value */
    ret                      = RBLE_OK;
    r_sapi_info.event_status = RBLE_OK;
    
    /* Communication timeout timer between Host MCU and BLE MCU. */
    RBLE_Set_Timer_Id(_RBLE_TIMER_ID_RBLES, (RBLE_FUNC)r_bles_timeout_communication,
                      TIME_OUT_TIMER_2s);
    
    r_sapi_info.seq = RBLES_SEQ_GAP_DISCONNECT; /* starting state of R_BLES_disconnect() */
    
    while(1)
    {
        if(r_sapi_info.seq != RBLES_SEQ_NONE)
        {
            switch(r_sapi_info.seq)
            {
                case RBLES_SEQ_GAP_DISCONNECT:
                    ret = RBLE_GAP_Disconnect(r_sapi_info.conhdl);
                    break;
                    
                default:
                    break;
            }
            r_sapi_info.seq = RBLES_SEQ_NONE;
        }
        
        /* rBLE process.                                  */
        /* Sending rBLE commands and generate rBLE event. */
        rBLE_Run();
        
        /* return value of rBLE API */
        if(ret != RBLE_OK)
        {
            if((ret != RBLE_STATUS_ERROR) && (ret != RBLE_TRANS_ERR))
            {
                ret = RBLE_ERR;
            }
            break;
        }
        /* return status of rBLE Event */
        sts = r_sapi_info.event_status;
        if(sts != RBLE_OK)
        {
            if((sts != RBLE_STATUS_ERROR) && (sts != RBLE_TRANS_ERR))
            {
                ret = RBLE_ERR;
            }
            else
            {
                ret = sts;
            }
            break;
        }
        /* exit a sequence loop */
        if(r_sapi_info.seq == RBLES_SEQ_EXIT)
        {
            break;
        }
    }
    
    /* Stop timeout timer */
    RBLE_Clear_Timer_Id(_RBLE_TIMER_ID_RBLES);
    
    r_rble_gap_disconnect_execution = FALSE;
    return(ret);
}

uint8_t Debug_txbuf[5];

/******************************************************************************
* Function Name: R_BLES_send_data
*
* Description  : This function send data to remote device by general purpose
*                communication profile (Virtual uart profile).
*
* Arguments    : *txbuf - 
*                    Pointer of transmission data buffer.
*                len - 
*                    Number of transmission data.
*
* Return Value : RBLE_STATUS - 
*                    Return value of rBLE API.
*                    RBLE_OK           : Success.
*                    RBLE_PARAM_ERR    : Invalid parameter.
*                    RBLE_STATUS_ERROR : Not executable because the rBLE mode
*                                        is other than RBLE_MODE_ACTIVE.
*                    RBLE_TRANS_ERR    : Communication timeout between Host MCU
*                                        and BLE MCU.
*                    RBLE_ERR          : Sequence error.
******************************************************************************/
RBLE_STATUS R_BLES_send_data(uint8_t *txbuf, uint8_t len)
{
    RBLE_STATUS ret, sts;
    
    Debug_txbuf[0] = *txbuf;
    Debug_txbuf[1] = *(txbuf+1);

    /* Parameter check */
    if((txbuf == NULL) || (len == 0) || (len > 20)
       || ((r_sapi_info.role != RBLES_ROLE_MASTER) && (r_sapi_info.role != RBLES_ROLE_SLAVE)))
    {
        return(RBLE_PARAM_ERR);
    }
    
    /* Initialize return value */
    ret                      = RBLE_OK;
    r_sapi_info.event_status = RBLE_OK;
    
    /* Store events that occurred in R_BLES_send_data */
    r_sapi_info.get_event_s = RBLES_EVENT_NONE;
    
    /* Communication timeout timer between Host MCU and BLE MCU. */
    RBLE_Set_Timer_Id(_RBLE_TIMER_ID_RBLES, (RBLE_FUNC)r_bles_timeout_communication,
                      TIME_OUT_TIMER_2s);
    
    r_sapi_info.seq = RBLES_SEQ_VUART_SEND; /* starting state of R_BLES_send_data() */
    
    while(1)
    {
        if(r_sapi_info.seq != RBLES_SEQ_NONE)
        {
            switch(r_sapi_info.seq)
            {
                case RBLES_SEQ_VUART_SEND:
                    if(r_sapi_info.role == RBLES_ROLE_MASTER)
                    {
                        /* RBLES_ROLE_MASTER */
                        ret = RBLE_VUART_Client_Send_Chars((const char *)txbuf, len);
                    }
                    else
                    {
                        /* RBLES_ROLE_SLAVE */
                        ret = RBLE_VUART_Server_Send_Indication((const char *)txbuf, len);
                    }
                    break;
                    
                default:
                    break;
            }
            r_sapi_info.seq = RBLES_SEQ_NONE;
        }
        
        /* rBLE process.                                  */
        /* Sending rBLE commands and generate rBLE event. */
        rBLE_Run();
        
        /* disconnection occurred */
        if(r_sapi_info.get_event == RBLES_EVENT_DISCONNECT)
        {
            r_sapi_info.get_event_s = RBLES_EVENT_DISCONNECT;
            
            ret = RBLE_ERR;
            break;
        }
        
        /* return value of rBLE API */
        if(ret != RBLE_OK)
        {
            if((ret != RBLE_STATUS_ERROR) && (ret != RBLE_TRANS_ERR))
            {
                ret = RBLE_ERR;
            }
            break;
        }
        /* return status of rBLE Event */
        sts = r_sapi_info.event_status;
        if(sts != RBLE_OK)
        {
            if((sts != RBLE_STATUS_ERROR) && (sts != RBLE_TRANS_ERR))
            {
                ret = RBLE_ERR;
            }
            else
            {
                ret = sts;
            }
            break;
        }
        /* exit a sequence loop */
        if(r_sapi_info.seq == RBLES_SEQ_EXIT)
        {
            break;
        }
    }
    
    /* Stop timeout timer */
    RBLE_Clear_Timer_Id(_RBLE_TIMER_ID_RBLES);
    
    return(ret);
}

/******************************************************************************
* Function Name: R_BLES_receive_data
*
* Description  : This function fetches data from reception queue.
*
* Arguments    : *rxbuf - 
*                    Pointer of transmission data buffer.
*                len - 
*                    Number of reception data.
*
* Return Value : 0 - 
*                    There is no data in the queue.
*                not 0 -
*                    Number of read data.
******************************************************************************/
uint16_t R_BLES_receive_data(uint8_t *rxbuf, uint16_t len)
{
    uint16_t    i;
    
    /* Parameter check */
    if((r_rdbuf_cntl.dnum == 0) || (rxbuf == NULL))
    {
        return(0);
    }
    
    for(i = 0; i < len; i++)
    {
        rxbuf[i] = r_rdbuf_cntl.rdbuf[r_rdbuf_cntl.p_rd++];
        r_rdbuf_cntl.dnum--;
        
        if(r_rdbuf_cntl.p_rd == RBLES_RDBUF_SIZE)
        {
            r_rdbuf_cntl.p_rd = 0;
        }
        
        if(r_rdbuf_cntl.dnum == 0)
        {
            i++;
            break;
        }
    }
    
    /* return number of read data */
    return(i);
}

/******************************************************************************
* Function Name: R_BLES_get_event
*
* Description  : This function returns the event that occurred.
*
* Arguments    : *evt - 
*                    Event is stored.
*
* Return Value : RBLE_STATUS - 
*                    Return value of rBLE API.
*                    RBLE_OK           : Success.
*                    RBLE_PARAM_ERR    : Invalid parameter.
*                    RBLE_STATUS_ERROR : Not executable because the rBLE mode
*                                        is other than RBLE_MODE_ACTIVE.
*                    RBLE_ERR          : Sequence error.
******************************************************************************/
RBLE_STATUS R_BLES_get_event(uint8_t *evt)
{
    RBLE_STATUS ret;
    
    /* Parameter check */
    if(evt == NULL)
    {
        return(RBLE_PARAM_ERR);
    }

    /* Initialize event value */
    r_sapi_info.get_event = RBLES_EVENT_NONE;

    /* rBLE process.                                  */
    /* Sending rBLE commands and generate rBLE event. */
    rBLE_Run();

    if(r_sapi_info.get_event_s != RBLES_EVENT_NONE)
    {
        *evt = r_sapi_info.get_event_s;
        r_sapi_info.get_event_s = RBLES_EVENT_NONE;
    }
    else if(r_sapi_info.get_event != RBLES_EVENT_NONE)
    {
        *evt = r_sapi_info.get_event;
        r_sapi_info.get_event = RBLES_EVENT_NONE;
    }
    else
    {
        *evt = RBLES_EVENT_NONE;
    }
    
    /* return status of rBLE Event */
    ret = r_sapi_info.event_status;
    if(ret != RBLE_OK)
    {
        if((ret != RBLE_STATUS_ERROR) && (ret != RBLE_TRANS_ERR))
        {
            ret = RBLE_ERR;
        }
    }

    return(ret);
}

/******************************************************************************
* Function Name: R_BLES_whitelist
*
* Description  : add device address to whitelist or delete device address from
*                whitelist.
*
* Arguments    : *whitelist - 
*                    Pointer of device address list.
*
* Return Value : RBLE_STATUS - 
*                    Return value of rBLE API.
*                    RBLE_OK           : Success.
*                    RBLE_PARAM_ERR    : Invalid parameter.
*                    RBLE_STATUS_ERROR : Not executable because the rBLE mode
*                                        is other than RBLE_MODE_ACTIVE.
*                    RBLE_TRANS_ERR    : Communication timeout between Host MCU
*                                        and BLE MCU.
*                    RBLE_ERR          : Sequence error.
******************************************************************************/
RBLE_STATUS R_BLES_whitelist(RBLE_WHITELIST *whitelist)
{
    RBLE_STATUS ret, sts;
    
    /* Parameter check */
    if(whitelist == NULL)
    {
        return(RBLE_PARAM_ERR);
    }
    
    /* Initialize return value */
    ret                      = RBLE_OK;
    r_sapi_info.event_status = RBLE_OK;
    
    /* Initialize white list value */
    r_wlst_num = (sizeof(whitelist->dev_list)/sizeof(RBLE_WLIST_DEV_ADDR));
    r_wlst_cnt = 0;
    
    /* Communication timeout timer between Host MCU and BLE MCU. */
    RBLE_Set_Timer_Id(_RBLE_TIMER_ID_RBLES, (RBLE_FUNC)r_bles_timeout_communication,
                      TIME_OUT_TIMER_2s);
    
    r_sapi_info.seq = RBLES_SEQ_GAP_DEL_FROM_WHITE_LIST; /* starting state of R_BLES_whitelist() */
    
    while(1)
    {
        if(r_sapi_info.seq != RBLES_SEQ_NONE)
        {
            switch(r_sapi_info.seq)
            {
                case RBLES_SEQ_GAP_DEL_FROM_WHITE_LIST:
                    ret = RBLE_GAP_Del_From_White_List(TRUE, NULL);
                    break;
                    
                case RBLES_SEQ_GAP_ADD_TO_WHITE_LIST:
                    ret = RBLE_GAP_Add_To_White_List(&whitelist->dev_list[r_wlst_cnt++].dev_info);
                    break;
                    
                default:
                    break;
            }
            r_sapi_info.seq = RBLES_SEQ_NONE;
        }
        
        /* rBLE process.                                  */
        /* Sending rBLE commands and generate rBLE event. */
        rBLE_Run();
        
        /* return value of rBLE API */
        if(ret != RBLE_OK)
        {
            if((ret != RBLE_STATUS_ERROR) && (ret != RBLE_TRANS_ERR))
            {
                ret = RBLE_ERR;
            }
            break;
        }
        /* return status of rBLE Event */
        sts = r_sapi_info.event_status;
        if(sts != RBLE_OK)
        {
            if((sts != RBLE_STATUS_ERROR) && (sts != RBLE_TRANS_ERR))
            {
                ret = RBLE_ERR;
            }
            else
            {
                ret = sts;
            }
            break;
        }
        /* exit a sequence loop */
        if(r_sapi_info.seq == RBLES_SEQ_EXIT)
        {
            break;
        }
    }
    
    /* Stop timeout timer */
    RBLE_Clear_Timer_Id(_RBLE_TIMER_ID_RBLES);
    
    return(ret);
}

/******************************************************************************
BLE Simple API sub routines
******************************************************************************/
/******************************************************************************
* Function Name: r_bles_set_sequence
*
* Description  : Move sequence of api.
*
* Arguments    : sec - 
*                    Sequence.
******************************************************************************/
STATIC void r_bles_set_sequence(uint8_t seq)
{
    r_sapi_info.seq = seq;
}

/******************************************************************************
* Function Name: r_bles_timeout_scan_duration
*
* Description  : Timeout function of scanning duration
******************************************************************************/
STATIC void r_bles_timeout_scan_duration(void)
{
    //R_GPIO_PinWrite(GPIO_PORT_A_PIN_6, GPIO_LEVEL_LOW);//DEBUG

    RBLE_Clear_Timer_Id(_RBLE_TIMER_ID_RBLES_SCAN_DUR);

    r_bles_set_sequence(RBLES_SEQ_GAP_OBSERVATION_DISABLE);

    timeout_scan_dur = TRUE;
}

/******************************************************************************
* Function Name: r_bles_timeout_connect_duration
*
* Description  : Timeout function of connection duration
******************************************************************************/
STATIC void r_bles_timeout_connect_duration(void)
{
    RBLE_Clear_Timer_Id(_RBLE_TIMER_ID_RBLES_CONNECT_DUR);

    r_bles_set_sequence(RBLES_SEQ_GAP_CONNECTION_CANCEL);
}

/******************************************************************************
* Function Name: r_bles_timeout_advertise_duration
*
* Description  : Timeout function of advertising duration
******************************************************************************/
STATIC void r_bles_timeout_advertise_duration(void)
{
    RBLE_Clear_Timer_Id(_RBLE_TIMER_ID_RBLES_ADV_DUR);

    r_bles_set_sequence(RBLES_SEQ_GAP_BROADCAST_DISABLE);
}

/******************************************************************************
* Function Name: r_bles_timeout_communication
*
* Description  : Timeout function of advertising duration
******************************************************************************/
STATIC void r_bles_timeout_communication(void)
{
    //R_GPIO_PinWrite(GPIO_PORT_B_PIN_0, GPIO_LEVEL_LOW);//DEBUG

    RBLE_Clear_Timer_Id(_RBLE_TIMER_ID_RBLES);

    r_sapi_info.event_status = RBLE_TRANS_ERR;
}

/******************************************************************************
RBLE Callback Functions.
******************************************************************************/
/******************************************************************************
* Function Name: RBLE_MODE_callback
*
* Description  : Callback function of RBLE_initialize.
*
* Arguments    : none
******************************************************************************/
STATIC void RBLE_MODE_callback(RBLE_MODE mode)
{
    switch(mode){
        case RBLE_MODE_INITIALIZE:
            cbsub_rble_mode_initialize();
            break;
        case RBLE_MODE_ACTIVE:
            cbsub_rble_mode_active();
            break;
        case RBLE_MODE_RESET:
            cbsub_rble_mode_reset();
            break;
        case RBLE_MODE_SLEEP:
            cbsub_rble_mode_sleep();
            break;
        case RBLE_MODE_ERROR:
            cbsub_rble_mode_error();
            break;
        default:
            break;
    }
}

/******************************************************************************
* Function Name: RBLE_GAP_EVENT_callback
*
* Description  : Callback function of GAP (generic access profile) event.
*
* Arguments    : none
******************************************************************************/
STATIC void RBLE_GAP_EVENT_callback(RBLE_GAP_EVENT *event)
{
    switch( event->type ) {
        case RBLE_GAP_EVENT_RESET_RESULT:
            cbsub_rble_gap_event_reset_result(event);
            break;
        case RBLE_GAP_EVENT_SET_NAME_COMP:
            break;
        case RBLE_GAP_EVENT_OBSERVATION_ENABLE_COMP:
            cbsub_rble_gap_event_observation_enable_comp(event);
            break;
        case RBLE_GAP_EVENT_OBSERVATION_DISABLE_COMP:
            cbsub_rble_gap_event_observation_disable_comp(event);
            break;
        case RBLE_GAP_EVENT_BROADCAST_ENABLE_COMP:
            cbsub_rble_gap_event_broadcast_enable_comp(event);
            break;
        case RBLE_GAP_EVENT_BROADCAST_DISABLE_COMP:
            cbsub_rble_gap_event_broadcast_disable_comp(event);
            break;
        case RBLE_GAP_EVENT_SET_BONDING_MODE_COMP:
            cbsub_rble_gap_event_set_bonding_mode_comp(event);
            break;
        case RBLE_GAP_EVENT_SET_SECURITY_REQUEST_COMP:
            break;
        case RBLE_GAP_EVENT_GET_DEVICE_INFO_COMP:
            break;
        case RBLE_GAP_EVENT_RPA_RESOLVED:
            break;
        case RBLE_GAP_EVENT_GET_WHITE_LIST_SIZE_COMP:
            break;
        case RBLE_GAP_EVENT_ADD_TO_WHITE_LIST_COMP:
            cbsub_rble_gap_event_add_to_white_list_comp(event);
            break;
        case RBLE_GAP_EVENT_DEL_FROM_WHITE_LIST_COMP:
            cbsub_rble_gap_event_del_from_white_list_comp(event);
            break;
        case RBLE_GAP_EVENT_GET_REMOTE_DEVICE_NAME_COMP:
            break;
        case RBLE_GAP_EVENT_GET_REMOTE_DEVICE_INFO_COMP:
            break;
        case RBLE_GAP_EVENT_DEVICE_SEARCH_COMP:
            break;
        case RBLE_GAP_EVENT_DEVICE_SEARCH_RESULT_IND:
            break;
        case RBLE_GAP_EVENT_SET_RANDOM_ADDRESS_COMP:
            break;
        case RBLE_GAP_EVENT_SET_PRIVACY_FEATURE_COMP:
            break;
        case RBLE_GAP_EVENT_CONNECTION_COMP:
            cbsub_rble_gap_event_connection_comp(event);
            break;
        case RBLE_GAP_EVENT_CONNECTION_CANCEL_COMP:
            cbsub_rble_gap_event_connection_cancel_comp(event);
            break;
        case RBLE_GAP_EVENT_DISCONNECT_COMP:
            cbsub_rble_gap_event_disconnect_comp(event);
            break;
        case RBLE_GAP_EVENT_ADVERTISING_REPORT_IND:
            cbsub_rble_gap_event_advertising_report_ind(event);
            break;
        case RBLE_GAP_EVENT_BONDING_COMP:
            cbsub_rble_gap_event_bonding_comp(event);
            break;
        case RBLE_GAP_EVENT_BONDING_REQ_IND:
            cbsub_rble_gap_event_bonding_req_ind(event);
            break;
        case RBLE_GAP_EVENT_CHANGE_CONNECTION_PARAM_REQ_IND:
            cbsub_rble_gap_event_change_connection_param_req_ind(event);
            break;
        case RBLE_GAP_EVENT_CHANGE_CONNECTION_PARAM_COMP:
            cbsub_rble_gap_event_change_connection_param_comp(event);
            break;
        case RBLE_GAP_EVENT_CHANGE_CONNECTION_PARAM_RESPONSE:
            break;
        case RBLE_GAP_EVENT_CHANNEL_MAP_REQ_COMP:
            break;
        case RBLE_GAP_EVENT_READ_RSSI_COMP:
            break;
        case RBLE_GAP_EVENT_WR_CHAR_IND:
            break;
        case RBLE_GAP_EVENT_COMMAND_DISALLOWED_IND:
            break;
        default:
            break;
    }
}

/******************************************************************************
* Function Name: RBLE_SM_EVENT_callback
*
* Description  : Callback function of SM (security manager) event.
*
* Arguments    : none
******************************************************************************/
STATIC void RBLE_SM_EVENT_callback(RBLE_SM_EVENT *event)
{
    switch( event->type ){
        case RBLE_SM_EVENT_SET_CNF:
            break;
        case RBLE_SM_ENC_START_IND:
            cbsub_rble_sm_enc_start_ind(event);
            break;
        case RBLE_SM_TK_REQ_IND:
            break;
        case RBLE_SM_LTK_REQ_IND:
            cbsub_rble_sm_ltk_req_ind(event);
            break;
        case RBLE_SM_LTK_REQ_FOR_ENC_IND:
            cbsub_rble_sm_ltk_req_for_enc_ind(event);
            break;
        case RBLE_SM_IRK_REQ_IND:
            break;
        case RBLE_SM_CSRK_REQ_IND:
            break;
        case RBLE_SM_KEY_IND:
            cbsub_rble_sm_key_ind(event);
            break;
        case RBLE_SM_CHK_BD_ADDR_REQ:
            break;
        case RBLE_SM_TIMEOUT_EVT:
            break;
        case RBLE_SM_EVENT_COMMAND_DISALLOWED_IND:
            break;
        default:
            break;
    }
}

/******************************************************************************
* Function Name: RBLE_GATT_EVENT_callback
*
* Description  : Callback function of GATT (generic attribute profile) event.
*
* Arguments    : none
******************************************************************************/
STATIC void RBLE_GATT_EVENT_callback(RBLE_GATT_EVENT *event)
{
    switch( event->type ){
        case RBLE_GATT_EVENT_DISC_SVC_ALL_CMP:
            break;
        case RBLE_GATT_EVENT_DISC_SVC_ALL_128_CMP:
            break;
        case RBLE_GATT_EVENT_DISC_SVC_BY_UUID_CMP:
            break;
        case RBLE_GATT_EVENT_DISC_SVC_INCL_CMP:
            break;
        case RBLE_GATT_EVENT_DISC_CHAR_ALL_CMP:
            break;
        case RBLE_GATT_EVENT_DISC_CHAR_ALL_128_CMP:
            break;
        case RBLE_GATT_EVENT_DISC_CHAR_BY_UUID_CMP:
            break;
        case RBLE_GATT_EVENT_DISC_CHAR_BY_UUID_128_CMP:
            break;
        case RBLE_GATT_EVENT_DISC_CHAR_DESC_CMP:
            break;
        case RBLE_GATT_EVENT_DISC_CHAR_DESC_128_CMP:
            break;
        case RBLE_GATT_EVENT_READ_CHAR_RESP:
            break;
        case RBLE_GATT_EVENT_READ_CHAR_LONG_RESP:
            break;
        case RBLE_GATT_EVENT_READ_CHAR_MULT_RESP:
            break;
        case RBLE_GATT_EVENT_READ_CHAR_LONG_DESC_RESP:
            break;
        case RBLE_GATT_EVENT_WRITE_CHAR_RESP:
            break;
        case RBLE_GATT_EVENT_WRITE_CHAR_RELIABLE_RESP:
            break;
        case RBLE_GATT_EVENT_CANCEL_WRITE_CHAR_RESP:
            break;
        case RBLE_GATT_EVENT_HANDLE_VALUE_NOTIF:
            break;
        case RBLE_GATT_EVENT_HANDLE_VALUE_IND:
            break;
        case RBLE_GATT_EVENT_HANDLE_VALUE_CFM:
            break;
        case RBLE_GATT_EVENT_DISCOVERY_CMP:
            break;
        case RBLE_GATT_EVENT_COMPLETE:
            break;
        case RBLE_GATT_EVENT_WRITE_CMD_IND:
            break;
        case RBLE_GATT_EVENT_RESP_TIMEOUT:
            break;
        case RBLE_GATT_EVENT_SET_PERM_CMP:
            cbsub_rble_gatt_event_set_perm_cmp(event);
            break;
        case RBLE_GATT_EVENT_SET_DATA_CMP:
            break;
        case RBLE_GATT_EVENT_NOTIFY_COMP:
            break;
        case RBLE_GATT_EVENT_COMMAND_DISALLOWED_IND:
            break;
        default:
            break;
    }
}

#if 0
/******************************************************************************
* Function Name: RBLE_VS_EVENT_callback
*
* Description  : Callback function of VS (vendor specific) event.
*
* Arguments    : none
******************************************************************************/
STATIC void RBLE_VS_EVENT_callback(RBLE_VS_EVENT *event)
{
    switch( event->type ){
        case RBLE_VS_EVENT_TEST_RX_START_COMP:
            break;
        case RBLE_VS_EVENT_TEST_TX_START_COMP:
            break;
        case RBLE_VS_EVENT_TEST_END_COMP:
            break;
        case RBLE_VS_EVENT_WR_BD_ADDR_COMP:
            break;
        case RBLE_VS_EVENT_SET_TEST_PARAM_COMP:
            break;
        case RBLE_VS_EVENT_READ_TEST_RSSI_COMP:
            break;
        case RBLE_VS_EVENT_GPIO_DIR_COMP:
            break;
        case RBLE_VS_EVENT_GPIO_ACCESS_COMP:
            break;
        case RBLE_VS_EVENT_FLASH_MANAGEMENT_COMP:
            break;
        case RBLE_VS_EVENT_FLASH_ACCESS_COMP:
            break;
        case RBLE_VS_EVENT_FLASH_OPERATION_COMP:
            break;
        case RBLE_VS_EVENT_FLASH_GET_SPACE_COMP:
            break;
        case RBLE_VS_EVENT_FLASH_GET_EEL_VER_COMP:
            break;
        case RBLE_VS_EVENT_ADAPT_ENABLE_COMP:
            break;
        case RBLE_VS_EVENT_ADAPT_STATE_IND:
            break;
        case RBLE_VS_EVENT_COMMAND_DISALLOWED_IND:
            break;
        case RBLE_VS_EVENT_SET_TX_POWER_COMP:
            break;
        case RBLE_VS_EVENT_SET_PARAMS_COMP:
            break;
        case RBLE_VS_EVENT_RF_CONTROL_COMP:
            break;
        default:
            break;
    }
}
#endif

/******************************************************************************
* Function Name: RBLE_VUARTS_callback
*
* Description  : Callback function of VUART server.
*
* Arguments    : none
******************************************************************************/
STATIC void RBLE_VUARTS_callback(RBLE_VUART_EVENT *event)
{
    switch (event->type)
    {
        case RBLE_VUART_EVENT_SERVER_ENABLE_CMP:
            cbsub_rble_vuart_event_server_enable_cmp(event);
            break;
        case RBLE_VUART_EVENT_SERVER_WRITE_REQ:
            cbsub_rble_vuart_event_server_write_req(event);
            break;
        case RBLE_VUART_EVENT_SERVER_INDICATION_CFM:
            cbsub_rble_vuart_event_server_indication_cfm(event);
            break;
        default:
            break;
    }
}

/******************************************************************************
* Function Name: RBLE_VUARTC_callback
*
* Description  : Callback function of VUART client.
*
* Arguments    : none
******************************************************************************/
STATIC void RBLE_VUARTC_callback(RBLE_VUART_EVENT *event)
{
    switch (event->type)
    {
        case RBLE_VUART_EVENT_CLIENT_ENABLE_CMP:
            cbsub_rble_vuart_event_client_enable_cmp(event);
            break;
        case RBLE_VUART_EVENT_CLIENT_INDICATION:
            cbsub_rble_vuart_event_client_indication(event);
            break;
        case RBLE_VUART_EVENT_CLIENT_WRITE_RSP:
            cbsub_rble_vuart_event_client_write_rsp(event);
            break;
        default:
            break;
    }
}

/******************************************************************************
RBLE Event Functions.
******************************************************************************/
/******************************************************************************
* rBLE Event        : RBLE_MODE_INITIALIZE
* Source rBLE API   : RBLE_Init()
******************************************************************************/
STATIC void cbsub_rble_mode_initialize(void)
{
}

/******************************************************************************
* rBLE Event        : RBLE_MODE_ACTIVE
* Source rBLE API   : RBLE_Init()
******************************************************************************/
STATIC void cbsub_rble_mode_active(void)
{
    r_bles_set_sequence(RBLES_SEQ_GAP_RESET);

}

/******************************************************************************
* rBLE Event        : RBLE_MODE_RESET
* Source rBLE API   : RBLE_Init()
******************************************************************************/
STATIC void cbsub_rble_mode_reset(void)
{
}

/******************************************************************************
* rBLE Event        : RBLE_MODE_SLEEP
* Source rBLE API   : RBLE_Init()
******************************************************************************/
STATIC void cbsub_rble_mode_sleep(void)
{
}

/******************************************************************************
* rBLE Event        : RBLE_MODE_ERROR
* Source rBLE API   : RBLE_Init()
******************************************************************************/
STATIC void cbsub_rble_mode_error(void)
{
    //APP_RBLE_Exit_CallBack();
}

/******************************************************************************
* rBLE Event        : RBLE_GAP_EVENT_RESET_RESULT
*
* Source rBLE API   : RBLE_GAP_Reset()
*
* Simple API which this event is used :
*                     R_BLES_initialize()
******************************************************************************/
STATIC void cbsub_rble_gap_event_reset_result(RBLE_GAP_EVENT *event)
{
    r_sapi_info.event_status = event->param.reset_result.status;
    if(r_perm_notexec != FALSE)
    {
        r_bles_set_sequence(RBLES_SEQ_EXIT);
    }
    else
    {
        r_bles_set_sequence(RBLES_SEQ_SET_PERMISSION);
    }
}

/******************************************************************************
* rBLE Event        : RBLE_GAP_EVENT_OBSERVATION_ENABLE_COMP
*
* Source rBLE API   : RBLE_GAP_Observation_Enable()
*
* Simple API which this event is used :
*                     R_BLES_scan()
******************************************************************************/
STATIC void cbsub_rble_gap_event_observation_enable_comp(RBLE_GAP_EVENT *event)
{
    r_sapi_info.event_status = event->param.status;
}

/******************************************************************************
* rBLE Event        : RBLE_GAP_EVENT_OBSERVATION_DISABLE_COMP
*
* Source rBLE API   : RBLE_GAP_Observation_Disable()
*
* Simple API which this event is used :
*                     R_BLES_scan()
******************************************************************************/
STATIC void cbsub_rble_gap_event_observation_disable_comp(RBLE_GAP_EVENT *event)
{
    r_sapi_info.event_status = event->param.status;
    
    /* Advertising list is full */
    if(r_sapi_info.advl_full == TRUE)
    {
        r_sapi_info.event_status = RBLE_ADVLIST_FULL;
    }
    
    r_bles_set_sequence(RBLES_SEQ_EXIT);
}

/******************************************************************************
* rBLE Event        : RBLE_GAP_EVENT_BROADCAST_ENABLE_COMP
*
* Source rBLE API   : RBLE_GAP_Broadcast_Enable()
*
* Simple API which this event is used :
*                     R_BLES_advertise()
******************************************************************************/
STATIC void cbsub_rble_gap_event_broadcast_enable_comp(RBLE_GAP_EVENT *event)
{
    r_sapi_info.event_status = event->param.status;
}

/******************************************************************************
* rBLE Event        : RBLE_GAP_EVENT_BROADCAST_DISABLE_COMP
*
* Source rBLE API   : RBLE_GAP_Broadcast_Disable()
*
* Simple API which this event is used :
*                     R_BLES_advertise()
******************************************************************************/
STATIC void cbsub_rble_gap_event_broadcast_disable_comp(RBLE_GAP_EVENT *event)
{
    r_sapi_info.event_status = event->param.status;

    r_bles_set_sequence(RBLES_SEQ_EXIT);
}

/******************************************************************************
* rBLE Event        : RBLE_GAP_EVENT_SET_BONDING_MODE_COMP
*
* Source rBLE API   : RBLE_GAP_Set_Bonding_Mode()
*
* Simple API which this event is used :
*                     R_BLES_connect()
******************************************************************************/
STATIC void cbsub_rble_gap_event_set_bonding_mode_comp(RBLE_GAP_EVENT *event)
{
    r_sapi_info.event_status = event->param.status;
    
    r_bles_set_sequence(RBLES_SEQ_GAP_CREATE_CONNECTION);
}

/******************************************************************************
* rBLE Event        : RBLE_GAP_EVENT_ADD_TO_WHITE_LIST_COMP
*
* Source rBLE API   : RBLE_GAP_Add_To_White_List()
*
* Simple API which this event is used :
*                     R_BLES_whitelist()
******************************************************************************/
STATIC void cbsub_rble_gap_event_add_to_white_list_comp(RBLE_GAP_EVENT *event)
{
    if(event->param.status != RBLE_OK)
    {
        /* Error process */
        r_bles_set_sequence(RBLES_SEQ_EXIT);
    }
    else
    {
        if(r_wlst_num > r_wlst_cnt)
        {
            r_bles_set_sequence(RBLES_SEQ_GAP_ADD_TO_WHITE_LIST);
        }
        else
        {
            /* registerd all device addresses */
            r_bles_set_sequence(RBLES_SEQ_EXIT);
        }
    }
    r_sapi_info.event_status = event->param.status;
}

/******************************************************************************
* rBLE Event        : RBLE_GAP_EVENT_DEL_FROM_WHITE_LIST_COMP
*
* Source rBLE API   : RBLE_GAP_Del_From_White_List()
*
* Simple API which this event is used :
*                     R_BLES_whitelist()
******************************************************************************/
STATIC void cbsub_rble_gap_event_del_from_white_list_comp(RBLE_GAP_EVENT *event)
{
    if(event->param.status != RBLE_OK)
    {
        /* Error process */
        r_bles_set_sequence(RBLES_SEQ_EXIT);
    }
    else
    {
        r_bles_set_sequence(RBLES_SEQ_GAP_ADD_TO_WHITE_LIST);
    }
    r_sapi_info.event_status = event->param.status;
}

/******************************************************************************
* rBLE Event        : RBLE_GAP_EVENT_CONNECTION_COMP
*
* Source rBLE API   : RBLE_GAP_Create_Connection()
*
* Simple API which this event is used :
*                     R_BLES_advertise()
*                     R_BLES_connect()
******************************************************************************/
STATIC void cbsub_rble_gap_event_connection_comp(RBLE_GAP_EVENT *event)
{
    r_sapi_info.event_status = event->param.conn_comp.connect_info.status;

    if(r_sapi_info.event_status != RBLE_OK)
    {
        r_bles_set_sequence(RBLES_SEQ_GAP_DISCONNECT);
    }
    else
    {
        r_sapi_info.conhdl = event->param.conn_comp.connect_info.conhdl;
        //r_sapi_info.idx    = event->param.conn_comp.connect_info.idx;

        if(r_sapi_info.role == RBLES_ROLE_INITIATOR)
        {
            /* MOVE TO MASTER */

            /* Connection duration timer stop */
            RBLE_Clear_Timer_Id(_RBLE_TIMER_ID_RBLES_CONNECT_DUR);

            r_sapi_info.role = RBLES_ROLE_MASTER;

            /* copy RBLE_CONNECT_INFO structure to global value */
            memcpy(r_conn_info, &(event->param.conn_comp.connect_info), sizeof(RBLE_CONNECT_INFO));

            r_bles_set_sequence(RBLES_SEQ_GAP_START_BONDING);
        }
        else
        {
            /* MOVE TO SLAVE */
            
            /* Advertising duration timer stop */
            RBLE_Clear_Timer_Id(_RBLE_TIMER_ID_RBLES_ADV_DUR);
            
            r_sapi_info.role = RBLES_ROLE_SLAVE;
            
            r_bles_set_sequence(RBLES_SEQ_VUART_SERVER_ENABLE);
        }
    }
}

/******************************************************************************
* rBLE Event        : RBLE_GAP_EVENT_CONNECTION_CANCEL_COMP
*
* Source rBLE API   : RBLE_GAP_Connection_Cancel()
*
* Simple API which this event is used :
*                     R_BLES_connect()
******************************************************************************/
STATIC void cbsub_rble_gap_event_connection_cancel_comp(RBLE_GAP_EVENT *event)
{
    /* connection duration timer is expired */
    if(r_sapi_info.role == RBLES_ROLE_MASTER)
    {
        r_sapi_info.event_status = event->param.status;
        if(event->param.status == RBLE_OK)
        {
            /* set connection timeout flag */
            /* this error is RBLE_TIMEOUT  */
            r_conn_timeout = TRUE;
        }
    }
}

/******************************************************************************
* rBLE Event        : RBLE_GAP_EVENT_DISCONNECT_COMP
*
* Source rBLE API   : RBLE_GAP_Disconnect()
*
* Simple API which this event is used :
*                     R_BLES_disconnect()
*                     R_BLES_get_event()
******************************************************************************/
STATIC void cbsub_rble_gap_event_disconnect_comp(RBLE_GAP_EVENT *event)
{
    r_sapi_info.event_status = event->param.disconnect.status;
    
    if(r_rble_gap_disconnect_execution == FALSE)
    {
        /* Set event for R_BLES_get_event() */
        r_sapi_info.get_event = RBLES_EVENT_DISCONNECT;
    }
    
    /* Disable VUART function */
    if(r_sapi_info.role == RBLES_ROLE_SLAVE)
    {
        RBLE_VUART_Server_Disable();
    }
    else
    {
        RBLE_VUART_Client_Disable();
    }

    /* Initialize role */
    r_sapi_info.role = RBLES_ROLE_NONE;
    
    /* R_BLES_disconnect() was executed  */
    r_bles_set_sequence(RBLES_SEQ_EXIT);
}

/******************************************************************************
* rBLE Event        : RBLE_GAP_EVENT_ADVERTISING_REPORT_IND
*
* Source rBLE API   : RBLE_GAP_Start_Bonding()
*
* Simple API which this event is used :
*                     R_BLES_scan()
******************************************************************************/
STATIC void cbsub_rble_gap_event_advertising_report_ind(RBLE_GAP_EVENT *event)
{
    RBLE_ADV_REPORT     *adv_rep_p;
    uint8_t             *uuid_p;
    uint8_t             svc_vuart[SVC_VUART_SIZE] = RBLE_SVC_VUART;
    uint8_t             i;
    uint8_t             data_len;
    int16_t             ret;

    adv_rep_p = &event->param.adv_report.evt.adv_rep;

    if(r_adv_report->adv_rep_num >= ADV_REPORT_LIST_NUM)
    {
        /* When adv_list[] is full, it returns.  */
        return;
    }

    if(r_scan_vuart == TRUE)
    {
        /* scan_filt_policy :                                            */
        /*     RLBE_SCAN_ALLOW_VUART_SRV : search for vuart service uuid */

        i = 0;
        data_len = adv_rep_p->data_len;

        while(1)
        {
            /* Search for position(address) of 0x1b */
            /* ("0x1b" is start byte of VUART UUID) */
            uuid_p = memchr(&adv_rep_p->data[i], 0x1b, data_len);

            if(uuid_p != NULL)
            {
                /* found 0x1b */

                if(((uint32_t)(adv_rep_p->data) + data_len) < ((uint32_t)uuid_p + SVC_VUART_SIZE))
                {
                    /* Adding 16 to the point where "0x1b" was found will over the advdata size.           */
                    /* So it is not VUART UUID                                                             */
                    /* ((start address of adv_rep_p->data) + 30(data_len)  <  uuid_p + 16(SVC_VUART_SIZE)) */
                    break;
                }
                else
                {
                    ret = memcmp(uuid_p, svc_vuart, SVC_VUART_SIZE);
                    if(ret == 0)
                    {
                        /* Copy advertising report to adv_list[] */
                        memcpy(&r_adv_report->adv_list[r_adv_report->adv_rep_num++], adv_rep_p, sizeof(RBLE_ADV_REPORT));
                        break;
                    }
                    else
                    {
                        /* increment search point */
                        i = (uint8_t)((uint32_t)uuid_p - (uint32_t)(adv_rep_p->data)) + 1;
                    }
                }
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        /* scan_filt_policy :           */
        /*     RBLE_SCAN_ALLOW_ADV_ALL  */
        /*     RBLE_SCAN_ALLOW_ADV_WLST */

        /* Copy advertising report to adv_list[] */
        memcpy(&r_adv_report->adv_list[r_adv_report->adv_rep_num++], adv_rep_p, sizeof(RBLE_ADV_REPORT));
    }

    /* Is adv_list[] full? */
    if(r_adv_report->adv_rep_num >= ADV_REPORT_LIST_NUM)
    {
        /* RBLE_ADVLIST_FULL of return value set after RBLE_GAP_EVENT_OBSERVATION_DISABLE_COMP event */
        r_sapi_info.event_status = RBLE_OK;
        r_sapi_info.advl_full    = TRUE;

        RBLE_Clear_Timer_Id(_RBLE_TIMER_ID_RBLES_SCAN_DUR);
        if(timeout_scan_dur == FALSE)
        {
            r_bles_set_sequence(RBLES_SEQ_GAP_OBSERVATION_DISABLE);
        }
    }
}

/******************************************************************************
* rBLE Event        : RBLE_GAP_EVENT_BONDING_COMP
*
* Source rBLE API   : RBLE_GAP_Start_Bonding()
*
* Simple API which this event is used :
*                     R_BLES_advertise()
*                     R_BLES_connect()
******************************************************************************/
STATIC void cbsub_rble_gap_event_bonding_comp(RBLE_GAP_EVENT *event)
{
    if(r_sapi_info.role == RBLES_ROLE_MASTER)
    {
        r_bles_set_sequence(RBLES_SEQ_SM_START_ENC);
    }

    r_sapi_info.idx          = event->param.bonding_comp.idx;
    r_sapi_info.event_status = event->param.bonding_comp.status;
}

/******************************************************************************
* rBLE Event        : RBLE_GAP_EVENT_BONDING_REQ_IND
*
* Source rBLE API   : RBLE_GAP_Start_Bonding() (calls at remote device)
*
* Simple API which this event is used :
*                     R_BLES_advertise()
******************************************************************************/
STATIC void cbsub_rble_gap_event_bonding_req_ind(RBLE_GAP_EVENT *event)
{
    r_bles_set_sequence(RBLES_SEQ_GAP_BONDING_RESPONSE);
}

/******************************************************************************
* rBLE Event        : RBLE_GAP_EVENT_CHANGE_CONNECTION_PARAM_REQ_IND
*
* Source rBLE API   : RBLE_GAP_Change_Connection_Param() (calls at remote device)
*
* Simple API which this event is used :
*                     R_BLES_connect()
******************************************************************************/
STATIC void cbsub_rble_gap_event_change_connection_param_req_ind(RBLE_GAP_EVENT *event)
{
    /* RBLE_GAP_EVENT_CHANGE_CONNECTION_PARAM_REQ_IND occurred */

    r_gap_chg_con_param_req_ind = TRUE;
}

/******************************************************************************
* rBLE Event        : RBLE_GAP_EVENT_CHANGE_CONNECTION_PARAM_COMP
*
* Source rBLE API   : RBLE_GAP_Change_Connection_Param()
*
* Simple API which this event is used :
*                     R_BLES_connect()
******************************************************************************/
STATIC void cbsub_rble_gap_event_change_connection_param_comp(RBLE_GAP_EVENT *event)
{
    r_bles_set_sequence(RBLES_SEQ_EXIT);
    
    r_sapi_info.event_status = event->param.chg_connect_param.status;
}

/******************************************************************************
* rBLE Event        : RBLE_SM_ENC_START_IND
*
* Source rBLE API   : RBLE_SM_Start_Enc()
*
* Simple API which this event is used :
*                     R_BLES_advertise()
*                     R_BLES_connect()
******************************************************************************/
STATIC void cbsub_rble_sm_enc_start_ind(RBLE_SM_EVENT *event)
{
    r_sapi_info.event_status = event->param.sec_start.status;

    if(r_sapi_info.role == RBLES_ROLE_MASTER)
    {
        r_bles_set_sequence(RBLES_SEQ_VUART_CLIENT_ENABLE);
    }
    else
    {
        /* In case of RBLES_ROLE_MASTER, do nothing */
    }
}

/******************************************************************************
* rBLE Event        : RBLE_SM_LTK_REQ_IND
*
* Source rBLE API   : -
*
* Simple API which this event is used :
*                     R_BLES_advertise()
******************************************************************************/
STATIC void cbsub_rble_sm_ltk_req_ind(RBLE_SM_EVENT *event)
{
    r_sapi_info.idx = event->param.ltk_req.idx;
    
    r_bles_set_sequence(RBLES_SEQ_SM_LTK_REQ_RESP_IND);
}

/******************************************************************************
* rBLE Event        : RBLE_SM_LTK_REQ_FOR_ENC_IND
*
* Source rBLE API   : RBLE_GAP_Start_Bonding() (calls at remote device)
*
* Simple API which this event is used :
*                     R_BLES_advertise()
******************************************************************************/
STATIC void cbsub_rble_sm_ltk_req_for_enc_ind(RBLE_SM_EVENT *event)
{
    r_sapi_info.idx = event->param.ltk_req_for_enc.idx;
    
    r_bles_set_sequence(RBLES_SEQ_SM_LTK_REQ_RESP_ENC_IND);
}

/******************************************************************************
* rBLE Event        : RBLE_SM_KEY_IND
*
* Source rBLE API   : RBLE_SM_Ltk_Req_Resp() (calls at remote device)
*
* Simple API which this event is used :
*                     R_BLES_connect()
******************************************************************************/
STATIC void cbsub_rble_sm_key_ind(RBLE_SM_EVENT *event)
{
    if(r_sapi_info.role == RBLES_ROLE_MASTER)
    {
        memcpy(&r_sapi_info.rand_nb, &event->param.key_ind.nb, RBLE_RAND_NB_LEN);
        memcpy(&r_sapi_info.ltk, &event->param.key_ind.ltk, RBLE_KEY_LEN);
        r_sapi_info.ediv = event->param.key_ind.ediv;
    }
}

/******************************************************************************
* rBLE Event        : RBLE_GATT_EVENT_SET_PERM_CMP
*
* Source rBLE API   : RBLE_GATT_Set_Permission()
*
* Simple API which this event is used :
*                     R_BLES_initialize()
******************************************************************************/
STATIC void cbsub_rble_gatt_event_set_perm_cmp(RBLE_GATT_EVENT *event)
{
    r_sapi_info.event_status = event->param.set_perm_cmp.status;

    r_bles_set_sequence(RBLES_SEQ_EXIT);
}

/******************************************************************************
* rBLE Event        : RBLE_VUART_EVENT_SERVER_ENABLE_CMP
*
* Source rBLE API   : RBLE_VUART_Server_Enable()
*
* Simple API which this event is used :
*                     R_BLES_advertise()
******************************************************************************/
STATIC void cbsub_rble_vuart_event_server_enable_cmp(RBLE_VUART_EVENT *event)
{
    r_sapi_info.event_status = event->param.server_enable_cmp.status;

    r_bles_set_sequence(RBLES_SEQ_EXIT);
}

/******************************************************************************
* rBLE Event        : RBLE_VUART_EVENT_SERVER_WRITE_REQ
*
* Source rBLE API   : RBLE_VUART_Client_Send_Chars()
*
* Simple API which this event is used :
*                     R_BLES_get_event()
******************************************************************************/
STATIC void cbsub_rble_vuart_event_server_write_req(RBLE_VUART_EVENT *event)
{
    uint16_t    i;
    
    /* Receive data from client(master) */
    
    if((uint32_t)(r_rdbuf_cntl.dnum + event->param.server_write_req.len) > RBLES_RDBUF_SIZE)
    {
        /* Reception buffer overflow */
        r_sapi_info.event_status = RBLE_ERR;
    }
    else
    {
        for(i = 0; i < event->param.server_write_req.len; i++)
        {
            r_rdbuf_cntl.rdbuf[r_rdbuf_cntl.p_wr++] = event->param.server_write_req.value[i];
            r_rdbuf_cntl.dnum++;
            
            if(r_rdbuf_cntl.p_wr == RBLES_RDBUF_SIZE)
            {
                r_rdbuf_cntl.p_wr = 0;
            }
        }
        
        r_sapi_info.event_status = event->param.server_write_req.status;
    }
    
    r_sapi_info.get_event = RBLES_EVENT_RECEIVE_DATA;
}

/******************************************************************************
* rBLE Event        : RBLE_VUART_EVENT_SERVER_INDICATION_CFM
*
* Source rBLE API   : RBLE_VUART_Server_Send_Indication()
*
* Simple API which this event is used :
*                     R_BLES_send_data()
******************************************************************************/
STATIC void cbsub_rble_vuart_event_server_indication_cfm(RBLE_VUART_EVENT *event)
{
    /* send data to client(master) */

    r_sapi_info.event_status = event->param.server_indicate_cnf.status;

    r_bles_set_sequence(RBLES_SEQ_EXIT);
}

/******************************************************************************
* rBLE Event        : RBLE_VUART_EVENT_CLIENT_ENABLE_CMP
*
* Source rBLE API   : RBLE_VUART_Client_Enable()
*
* Simple API which this event is used :
*                     R_BLES_connect()
******************************************************************************/
STATIC void cbsub_rble_vuart_event_client_enable_cmp(RBLE_VUART_EVENT *event)
{
    if(r_gap_chg_con_param_req_ind == TRUE)
    {
        r_bles_set_sequence(RBLES_SEQ_GAP_CHANGE_CONNECTION_PARAM_CONN);
    }
    else
    {
        /* RBLE_GAP_EVENT_CHANGE_CONNECTION_PARAM_REQ_IND does not occur. */
        r_bles_set_sequence(RBLES_SEQ_EXIT);
    }

    r_sapi_info.event_status = event->param.client_enable_cmp.status;
}

/******************************************************************************
* rBLE Event        : RBLE_VUART_EVENT_CLIENT_INDICATION
*
* Source rBLE API   : RBLE_VUART_Server_Send_Indication()
*
* Simple API which this event is used :
*                     R_BLES_get_event()
******************************************************************************/
STATIC void cbsub_rble_vuart_event_client_indication(RBLE_VUART_EVENT *event)
{
    uint16_t    i;

    /* Receive data from server(slave) */

    if((uint32_t)(r_rdbuf_cntl.dnum + event->param.client_indication.len) > RBLES_RDBUF_SIZE)
    {
        /* Reception buffer overflow */
        r_sapi_info.event_status = RBLE_ERR;
    }
    else
    {
        for(i = 0; i < event->param.client_indication.len; i++)
        {
            r_rdbuf_cntl.rdbuf[r_rdbuf_cntl.p_wr++] = event->param.client_indication.value[i];
            r_rdbuf_cntl.dnum++;
            
            if(r_rdbuf_cntl.p_wr == RBLES_RDBUF_SIZE)
            {
                r_rdbuf_cntl.p_wr = 0;
            }
        }
        
        r_sapi_info.event_status = event->param.client_indication.status;
    }
    
    /* Set event for R_BLES_get_event() */
    r_sapi_info.get_event = RBLES_EVENT_RECEIVE_DATA;
}


/******************************************************************************
* rBLE Event        : RBLE_VUART_EVENT_CLIENT_WRITE_RSP
*
* Source rBLE API   : RBLE_VUART_Client_Send_Chars()
*
* Simple API which this event is used :
*                     R_BLES_send_data()
******************************************************************************/
STATIC void cbsub_rble_vuart_event_client_write_rsp(RBLE_VUART_EVENT *event)
{
    /* Send data to server(slave) */

    r_sapi_info.event_status = event->param.client_indication.status;

    r_bles_set_sequence(RBLES_SEQ_EXIT);
}
