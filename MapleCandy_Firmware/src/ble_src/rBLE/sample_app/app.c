/**
 ****************************************************************************************
 *
 * @file        rble_app.c
 *
 * @brief       rBLE Sample Application
 *
 * Copyright(C) 2013-2015 Renesas Electronics Corporation
 *
 * $Rev: 6128 $
 *
 ****************************************************************************************
 */

#include    "r_cg_macrodriver.h"
#include    "timer.h"
#ifdef USE_RSK_LCD
#include    "lcd.h"
#endif
#ifdef USE_RSK_ADC
#include    "r_cg_adc.h"
#endif

#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>

#include    "ll.h"
#include    "rble.h"
#include    "rble_app.h"
#include    "rscip_ext.h"

#include    "prf_sel.h"

/**************************** Define Declaration **************************************/
#define     RUN_COMMAND_QUEUE_SIZE          10
#define     ILLEGAL_CONNECTION_HANDLE       0xffff

enum APP_KEY_FLAG_enum {
    APP_LTK,
    APP_ENCKEY,
    APP_IDKEY,
    APP_SIGNKEY
};

typedef struct {
    uint8_t                         Key_Set_Flags;
    struct RBLE_EVT_SM_Key_t        EncKey;
    struct RBLE_EVT_SM_Key_t        IrKey;
    struct RBLE_EVT_SM_Key_t        SignKey;
} APP_KEY_DATA;

/*
    USE_PAIRING_JUSTWORKS macro is defined(undefined) by Project Compile Option
      - if defined    , Pairing method is Just Works    setting
      - if not defined, Pairing method is Passkey Entry setting
*/

/******************************* PRIVATE PROTOTYPE ************************************/
static void APP_RBLE_CallBack( RBLE_MODE mode );
static BOOL APP_RBLE_Active_CallBack( void );
static BOOL APP_RBLE_Exit_CallBack( void );

/* --------------------------- Generic Access Profile --------------------------------*/
static RBLE_STATUS APP_GAP_Reset_Command( void );
static RBLE_STATUS APP_GAP_Broadcast_Enable_Command( void );
static RBLE_STATUS APP_GAP_Set_Bonding_Mode_Command( void );
static RBLE_STATUS APP_GAP_Set_Security_Request_Command( void );
static RBLE_STATUS APP_GAP_Bonding_Response_Command( void );
static RBLE_STATUS APP_GAP_Disconnection_Command( void );

static void APP_GAP_CallBack( RBLE_GAP_EVENT *event );
static BOOL APP_GAP_Reset_CallBack( RBLE_GAP_EVENT *event );
static BOOL APP_GAP_Broadcast_Enable_CallBack( RBLE_GAP_EVENT *event );
static BOOL APP_GAP_Connection_CallBack( RBLE_GAP_EVENT *event );
static BOOL APP_GAP_Disconnection_CallBack( RBLE_GAP_EVENT *event );
static BOOL APP_GAP_Bonding_Callback( RBLE_GAP_EVENT *event );
static BOOL APP_GAP_Set_Bonding_Mode_CallBack( RBLE_GAP_EVENT *event );
static BOOL APP_GAP_Set_Security_Request_CallBack( RBLE_GAP_EVENT *event );
static BOOL APP_GAP_RPA_Resolved_CallBack( RBLE_GAP_EVENT *event );
static BOOL APP_GAP_Bonding_Request_CallBack( RBLE_GAP_EVENT *event );

/* ----------------------------- Security Manager ------------------------------------*/
static RBLE_STATUS APP_SM_Check_BDAddr_Response_Command( void );
static RBLE_STATUS APP_SM_TK_Response_Command( void );
static RBLE_STATUS APP_SM_LTK_Response_Command( void );
static RBLE_STATUS APP_SM_LTK_Response_For_Encryption_Command( void );
static RBLE_STATUS APP_SM_IRK_Response_Command( void );

static void APP_SM_CallBack( RBLE_SM_EVENT *event );
static BOOL APP_SM_Encryption_Start_Callback( RBLE_SM_EVENT *event );
static BOOL APP_SM_TK_Request_CallBack( RBLE_SM_EVENT *event );
static BOOL APP_SM_LTK_Request_CallBack( RBLE_SM_EVENT *event );
static BOOL APP_SM_LTK_Request_For_Encryption_CallBack( RBLE_SM_EVENT *event );
static BOOL APP_SM_IRK_Request_CallBack( RBLE_SM_EVENT *event );
static BOOL APP_SM_Key_CallBack( RBLE_SM_EVENT *event );
static BOOL APP_SM_BDAddr_Check_Request_CallBack( RBLE_SM_EVENT *event );

/* ------------------------- Sample Custom Profile-------------------------------*/
static RBLE_STATUS APP_SCPS_Enable_Command( void );
static RBLE_STATUS APP_SCPS_Send_Notify_Command( void );

static void APP_SCPS_CallBack( RBLE_SCPS_EVENT *event );
static BOOL APP_SCPS_Enable_CallBack( RBLE_SCPS_EVENT *event );
static BOOL APP_SCPS_Send_Notify_CallBack( RBLE_SCPS_EVENT *event );
static BOOL APP_SCPS_Change_NtfInd_Config_CallBack( RBLE_SCPS_EVENT *event );

/* --------------------------------- Utility -----------------------------------------*/
static void APP_Set_RunCmd( uint16_t cmd );
static BOOL APP_Compare_BDAddr(RBLE_BD_ADDR* a, RBLE_BD_ADDR* b);
static int_t APP_Get_Random( void );

/* --------------------------------- LCD Display -------------------------------------*/
#ifdef USE_RSK_LCD
static void APP_Disp( char_t* str );
static void APP_Disp_Status( RBLE_STATUS status );
static void APP_Disp_PassKey( uint32_t passkey );
#else
#define APP_Disp            //APP_Disp
#define APP_Disp_Status     //APP_Disp_Status
#define APP_Disp_PassKey    //APP_Disp_PassKey
#endif

static char_t *APP_Get_Status_Str( RBLE_STATUS status );

/**************************** PUBLIC Memory Declaration *******************************/

/**************************** PRIVATE Memory Declaration ******************************/
static uint16_t         ConnectionHdl = ILLEGAL_CONNECTION_HANDLE;
static RBLE_BD_ADDR     BondedAddr    ;
static uint8_t          SecStatus     ;
static APP_KEY_DATA     LocalKey      ;
static APP_KEY_DATA     RemoteKey     ;
static uint16_t		      RcvEdiv       ;
static RBLE_RAND_NB	    RcvNb         ;
static uint8_t          ConnectionIdx ;
static uint8_t          ConnectionType;
static RBLE_BD_ADDR     RemoteDevice  ;
static BOOL             FoundFlg      ;
static BOOL             CheckedIrkFlg ;

static uint16_t         RunCmd[RUN_COMMAND_QUEUE_SIZE];    // RBLE_APP_CMD_NO_enum value
static uint8_t          WrPtr     ;
static uint8_t          RdPtr     ;
#ifdef USE_RSK_LCD
static char             LcdPrev[9];
#endif

static uint16_t         DataCnt  ;
static uint8_t          DataValue;

/******************************* Program Area *****************************************/

/**************************************************************************************/
/**
 *  @brief      Application Initialize
 *  
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
BOOL APP_Init( void )
{
    BOOL        ret = TRUE;
    RBLE_STATUS ret_status;

    /* Initialize rBLE */
    APP_Disp("c:BLEIni");
    ret_status = RBLE_Init(&APP_RBLE_CallBack);
    if ( RBLE_OK != ret_status ) {
        ret = FALSE;
    }
    /* Initialize Run Command Queue Pointer, LCD String Buffer */
    WrPtr = 0;
    RdPtr = 0;
    #ifdef USE_RSK_LCD
    LcdPrev[0] = '\0';
    #endif

    /* Initialize Bonding Information Buffer */
    SecStatus      = RBLE_SMP_SEC_NONE;
    RcvEdiv        = 0;
    memset(&RcvNb,      0, sizeof(RcvNb));
    memset(&BondedAddr, 0, sizeof(RBLE_BD_ADDR));
    memset(&LocalKey  , 0, sizeof(APP_KEY_DATA));
    memset(&RemoteKey , 0, sizeof(APP_KEY_DATA));

    return( ret );
}

/**************************************************************************************/
/**
 *  @brief      Application Command Execute
 *  
 *  @retval     None
 */
/******************************************************************************************/
void APP_Run( void )
{
    /* Loop until Run Command Queue is empty */
    while(RdPtr != WrPtr)
    {
        switch(RunCmd[RdPtr])
        {
            case RBLE_APP_CMD_NON:
                break;
            /* --------------------------- Generic Access Profile --------------------------------*/
            case GAP_RESET_CMD:
                APP_GAP_Reset_Command();
                break;
            case GAP_BROADCAST_ENABLE_CMD:
                APP_GAP_Broadcast_Enable_Command();
                break;
            case GAP_SET_BONDING_MODE_CMD:
                APP_GAP_Set_Bonding_Mode_Command();
                break;
            case GAP_SET_SECURITY_REQUEST_CMD:
                APP_GAP_Set_Security_Request_Command();
                break;
            case GAP_BONDING_RESPONSE_CMD:
                APP_GAP_Bonding_Response_Command();
                break;
            case GAP_DISCONNECT_CMD:
                APP_GAP_Disconnection_Command();
                break;

            /* --------------------------- Security Manager -------------------------------------*/
            case SM_CHK_BD_ADDR_REQ_RESP_CMD:
                APP_SM_Check_BDAddr_Response_Command();
                break;
            case SM_TK_REQ_RESP_CMD:
                APP_SM_TK_Response_Command();
                break;
            case SM_LTK_REQ_RESP_CMD:
                APP_SM_LTK_Response_Command();
                break;
            case SM_LTK_REQ_FOR_ENC_RESP_CMD:
                APP_SM_LTK_Response_For_Encryption_Command();
                break;
            case SM_IRK_REQ_RESP_CMD:
                APP_SM_IRK_Response_Command();
                break;

            /* ------------------------- Sample Custom Profile-------------------------------*/
            case SCP_Sensor_Enable_CMD:
                APP_SCPS_Enable_Command();
                break;
            case SCP_Sensor_Send_Notify_CMD:
                APP_SCPS_Send_Notify_Command();
                break;

            default:
                break;
        }
        
        /* Operate Run Command Queue Pointer as a Ring Buffer */
        GLOBAL_INT_DISABLE();
        RdPtr = (RdPtr == (RUN_COMMAND_QUEUE_SIZE - 1)) ? (0) : (RdPtr + 1);
        GLOBAL_INT_RESTORE();
    }
}

/**************************************************************************************/
/**
 *  @brief      rBLE API Call : GAP Reset
 *  
 *  @retval     rBLE API Return Status
 */
/******************************************************************************************/
static RBLE_STATUS APP_GAP_Reset_Command( void )
{
    RBLE_STATUS ret_status;

    ConnectionHdl = ILLEGAL_CONNECTION_HANDLE;
    APP_Disp("c:Reset ");
    ret_status = RBLE_GAP_Reset( &APP_GAP_CallBack, &APP_SM_CallBack );

    return( ret_status );
}

/**************************************************************************************/
/**
 *  @brief      rBLE API Call : GAP Broadcast Enable
 *  
 *  @retval     rBLE API Return Status
 */
/******************************************************************************************/
static RBLE_STATUS APP_GAP_Broadcast_Enable_Command( void )
{
    RBLE_STATUS ret_status;
    static RBLE_BROADCAST_ENABLE_PARAM  param = {
        /* General Discoverable & Undirected Connectable */
        RBLE_GAP_GEN_DISCOVERABLE,
        RBLE_GAP_UND_CONNECTABLE ,
        {
            {
                0x30,                                       /* AdvIntervalMin */
                0x60,                                       /* AdvIntervalMax */
                RBLE_GAP_ADV_CONN_UNDIR,                    /* AdvType        */
                RBLE_ADDR_PUBLIC,                           /* OwnAddrType    */
                RBLE_ADDR_PUBLIC,                           /* DirectAddrType */
                { 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12 },     /* DirectAddr     */
                RBLE_ADV_ALL_CHANNELS,                      /* AdvChannelMap  */
                RBLE_ADV_ALLOW_SCAN_ANY_CON_ANY,            /* AdvFiltPolicy  */
                0x00,                                       /* Reserved       */
            },
            {
                /* AdvDataLen     */
                3+13,
                /* AdvData        */
                2,  0x01, 0x06,                                                     /* Flags               */ 
                12, 0x09, 'R', 'e', 'n', 'e', 's', 'a', 's', '-', 'B', 'L', 'E',    /* Complete local name */
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            },
            {
                /* ScanRspDataLen */
                0,
                /* ScanData       */
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            }
        }
    };

    APP_Disp("c:BcstEn");
    ret_status = RBLE_GAP_Broadcast_Enable( param.disc_mode, param.conn_mode, &param.adv_info );

    return( ret_status );
}

/**************************************************************************************/
/**
 *  @brief      rBLE API Call : GAP Set Bonding Mode
 *  
 *  @retval     rBLE API Return Status
 */
/******************************************************************************************/
static RBLE_STATUS APP_GAP_Set_Bonding_Mode_Command( void )
{
    RBLE_STATUS ret_status;

    APP_Disp("c:BondMd");
    ret_status = RBLE_GAP_Set_Bonding_Mode(RBLE_GAP_BONDABLE);

    return( ret_status );
}

/**************************************************************************************/
/**
 *  @brief      rBLE API Call : GAP Set Security Request
 *  
 *  @retval     rBLE API Return Status
 */
/******************************************************************************************/
static RBLE_STATUS APP_GAP_Set_Security_Request_Command( void )
{
    RBLE_STATUS ret_status;

    APP_Disp("c:SecReq");
    ret_status = RBLE_GAP_Set_Security_Request( RBLE_GAP_SEC1_NOAUTH_PAIR_ENC );

    return( ret_status );
}

/**************************************************************************************/
/**
 *  @brief      rBLE API Call : GAP Bonding Response
 *  
 *  @retval     rBLE API Return Status
 */
/******************************************************************************************/
static RBLE_STATUS APP_GAP_Bonding_Response_Command( void )
{
    RBLE_STATUS ret_status;
    static RBLE_BOND_RESP_PARAM Param = {
        0x0000,                                         /* Connection handle           */
        RBLE_OK,                                        /* accept or reject bonding    */
        #ifdef USE_PAIRING_JUSTWORKS
        RBLE_IO_CAP_NO_INPUT_NO_OUTPUT,                 /* IO capabilities             */
        #else
        RBLE_IO_CAP_DISPLAY_ONLY,                       /* IO capabilities             */
        #endif
        RBLE_OOB_AUTH_DATA_NOT_PRESENT,                 /* OOB flag                    */
        #ifdef USE_PAIRING_JUSTWORKS
        RBLE_AUTH_REQ_NO_MITM_BOND,                     /* Authentication Requirements */
        #else
        RBLE_AUTH_REQ_MITM_BOND,                        /* Authentication Requirements */
        #endif
        RBLE_SMP_MAX_ENC_SIZE_LEN,                      /* Encryption key size         */
        RBLE_KEY_DIST_ENCKEY | RBLE_KEY_DIST_IDKEY,     /* Initiator key distribution  */
        RBLE_KEY_DIST_ENCKEY,                           /* Responder key distribution  */
        0x00                                            /* Reserved                    */
    };

    Param.conhdl = ConnectionHdl;
    APP_Disp("c:BndRsp");
    ret_status = RBLE_GAP_Bonding_Response( &Param );

    return( ret_status );
}

/**************************************************************************************/
/**
 *  @brief      rBLE API Call : GAP Disconnection
 *  
 *  @retval     rBLE API Return Status
 */
/******************************************************************************************/
static RBLE_STATUS APP_GAP_Disconnection_Command( void )
{
    RBLE_STATUS ret_status = FALSE;

    if(ConnectionHdl != ILLEGAL_CONNECTION_HANDLE)
    {
        APP_Disp("c:DisCon");
        ret_status = RBLE_GAP_Disconnect( ConnectionHdl );
    }

    return( ret_status );
}

/**************************************************************************************/
/**
 *  @brief      rBLE API Call : SM Check BD Address Request Resonse
 *  
 *  @retval     rBLE API Return Status
 */
/******************************************************************************************/
static RBLE_STATUS APP_SM_Check_BDAddr_Response_Command( void )
{
    RBLE_STATUS     ret_status;
    uint8_t         sec_status = RBLE_SMP_SEC_NONE;

    if(FoundFlg)
    {
        sec_status = SecStatus;
    }

    APP_Disp("c:ChkADR");
    ret_status = RBLE_SM_Chk_Bd_Addr_Req_Resp(ConnectionIdx, ConnectionType, FoundFlg, sec_status, &BondedAddr);

    return( ret_status );
}

/**************************************************************************************/
/**
 *  @brief      rBLE API Call : SM TK Request Response
 *  
 *  @retval     rBLE API Return Status
 */
/******************************************************************************************/
static RBLE_STATUS APP_SM_TK_Response_Command( void )
{
    RBLE_STATUS ret_status;
    static RBLE_KEY_VALUE tk = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00      /* TK(Temporary Key) */
    };
    uint32_t passkey;
    uint8_t* byteptr = (uint8_t*)&passkey;
    uint8_t  i;

    /* Generate PassKey (range:000,000 - 999,999) */
    passkey  = (uint32_t)APP_Get_Random();
    passkey |= (uint32_t)APP_Get_Random() << 16;
    passkey %= 1000000;

    /* Copy PassKey to TK(Temporary Key) */
    for(i = 0; i < sizeof(uint32_t); i++)
    {
        tk.key[RBLE_KEY_LEN - 1 - i] = byteptr[i];
    }
    APP_Disp("c:TkRsp ");
    APP_Disp_PassKey( passkey );
    ret_status = RBLE_SM_Tk_Req_Resp( ConnectionIdx, RBLE_OK, &tk );

    return( ret_status );
}

/**************************************************************************************/
/**
 *  @brief      rBLE API Call : SM LTK Request Response
 *  
 *  @retval     rBLE API Return Status
 */
/******************************************************************************************/
static RBLE_STATUS APP_SM_LTK_Response_Command( void )
{
    RBLE_STATUS   ret_status;
    const uint8_t sec_prop = RBLE_SMP_KSEC_UNAUTH_NO_MITM;
    uint8_t       status   = RBLE_OK;
    int_t         i;

    /* Generate LTK(Long Term Key), Random Number, EDIV(Encrypted Diversifier)  */
    for(i = 0; i <  RBLE_RAND_NB_LEN; i++)
    {
        LocalKey.EncKey.nb.nb[i]   = (uint8_t)APP_Get_Random();
    }
    for(i = 0; i <  RBLE_KEY_LEN; i++)
    {
        LocalKey.EncKey.ltk.key[i] = (uint8_t)APP_Get_Random();
    }
    LocalKey.EncKey.ediv    = (uint16_t)APP_Get_Random();
    LocalKey.Key_Set_Flags |= (1 << APP_ENCKEY);

    APP_Disp("c:LtkRs");
    ret_status = RBLE_SM_Ltk_Req_Resp( ConnectionIdx, status, sec_prop,
                                       LocalKey.EncKey.ediv,
                                      &LocalKey.EncKey.nb  ,
                                      &LocalKey.EncKey.ltk );

    return( ret_status );
}

/**************************************************************************************/
/**
 *  @brief      rBLE API Call : SM LTK Request Response
 *
 *  @retval     rBLE API Return Status
 */
/******************************************************************************************/
static RBLE_STATUS APP_SM_LTK_Response_For_Encryption_Command( void )
{
    RBLE_STATUS   ret_status;
    const uint8_t sec_prop = RBLE_SMP_KSEC_UNAUTH_NO_MITM;
    uint8_t       status   = RBLE_OK;

    /* Check if latest LTK is valid */
    if((LocalKey.EncKey.ediv != RcvEdiv) ||
       (0 != memcmp(&LocalKey.EncKey.nb, &RcvNb, sizeof(LocalKey.EncKey.nb))))
    {
        status = RBLE_ERR;
    }

    APP_Disp("c:Ltk4ER");
    ret_status = RBLE_SM_Ltk_Req_Resp( ConnectionIdx, status, sec_prop,
                                       LocalKey.EncKey.ediv,
                                      &LocalKey.EncKey.nb  ,
                                      &LocalKey.EncKey.ltk );

    return( ret_status );
}

/**************************************************************************************/
/**
 *  @brief      rBLE API Call : SM IRK Request Response
 *  
 *  @retval     rBLE API Return Status
 */
/******************************************************************************************/
static RBLE_STATUS APP_SM_IRK_Response_Command( void )
{
    RBLE_STATUS   ret_status;
    uint8_t       status   = RBLE_ERR;
    static RBLE_KEY_VALUE irk = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  /* IRK(Identity Resolving Key) */
    };

    if((!CheckedIrkFlg) && (0u != (RemoteKey.Key_Set_Flags & (1 << APP_IDKEY))))
    {
        CheckedIrkFlg = TRUE;
        irk    = RemoteKey.IrKey.ltk;
        status = RBLE_OK            ;
    }

    APP_Disp("c:IrkRsp");
    ret_status = RBLE_SM_Irk_Req_Resp( ConnectionIdx, status, &BondedAddr, &irk, SecStatus );

    return( ret_status );
}

/**************************************************************************************/
/**
 *  @brief      rBLE API Call : SCP Server Enable
 *  
 *  @retval     rBLE API Return Status
 */
/******************************************************************************************/

static RBLE_STATUS APP_SCPS_Enable_Command( void )
{
    RBLE_STATUS ret_status;
    static RBLE_SCP_SERVER_PARAM    param = {
        RBLE_SCP_STOP_NTFIND,   /* Notification configuration */
        RBLE_SCP_STOP_NTFIND,   /* Indication   configuration */
    };

    APP_Disp("c:ScpEna");
    ret_status = RBLE_SCP_Server_Enable( ConnectionHdl, RBLE_SCP_CON_CFG, &param, &APP_SCPS_CallBack );

    return( ret_status );
}

/**************************************************************************************/
/**
 *  @brief      rBLE API Call : SCP Server Send Notification
 *  
 *  @retval     rBLE API Return Status
 */
/******************************************************************************************/

static RBLE_STATUS APP_SCPS_Send_Notify_Command( void )
{
    RBLE_STATUS ret_status;
    static RBLE_SCP_NOTIFY_INFO     notify_info = {
        0x04,                   /* Length */
        {0x00,0x00,0x00,0x00}   /* Data   */
    };
    notify_info.data[0] = (uint8_t)(DataCnt >> 8    );
    notify_info.data[1] = (uint8_t)(DataCnt & 0x00FF);
    notify_info.data[3] = DataValue;
    DataCnt++;

    APP_Disp("c:ScpSnd");
    ret_status = RBLE_SCP_Server_Send_Notify( ConnectionHdl, &notify_info );

    return( ret_status );
}

/**************************************************************************************/
/**
 *  @brief      Application Idle Check
 *  
 *  @retval     TRUE if Command Queue is empty , FALSE if it not empty
 */
/******************************************************************************************/
BOOL APP_Is_Idle( void )
{
    BOOL    Idle = FALSE;

    /* Application Status is Idle when Command Queue is Empty */
    if(RdPtr == WrPtr)
    {
        Idle = TRUE;
    }

    return( Idle );
}

/********************  Call Back API ********************/

/******************************************************************************************/
/**
 *  @brief      Event Handle : rBLE
 *  
 *  @param      mode    rBLE Mode , defined by RBLE_MODE_enum
 *
 *  @retval     None
 */
/******************************************************************************************/
static void APP_RBLE_CallBack( RBLE_MODE mode )
{
    switch(mode){
        case RBLE_MODE_INITIALIZE:
            break;
        case RBLE_MODE_ACTIVE:
            APP_RBLE_Active_CallBack();
            break;
        case RBLE_MODE_RESET:
            break;
        case RBLE_MODE_SLEEP:
            break;
        case RBLE_MODE_ERROR:
            APP_RBLE_Exit_CallBack();
            break;
        default:
            break;
    }
}

/******************************************************************************************/
/**
 *  @brief      Event Respond : rBLE Active Event
 *  
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_RBLE_Active_CallBack( void )
{
    APP_Disp("e:BleAct");
    APP_Set_RunCmd(GAP_RESET_CMD);
    return( TRUE );
}

/******************************************************************************************/
/**
 *  @brief      Event Respond : rBLE Exit Event
 *  
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_RBLE_Exit_CallBack( void )
{
    APP_Disp("e:BleExt");
    return( TRUE );
}

/* --------------------------- Generic Access Profile --------------------------------*/

/******************************************************************************************/
/**
 *  @brief      Event Handler : GAP Event
 *
 *  @param      event   GAP Event Parameters
 *
 *  @retval     None
 */
/******************************************************************************************/
static void APP_GAP_CallBack( RBLE_GAP_EVENT *event )
{
    switch( event->type ) {
        case RBLE_GAP_EVENT_RESET_RESULT:
            APP_GAP_Reset_CallBack( event );
            break;
        case RBLE_GAP_EVENT_SET_NAME_COMP:
            break;
        case RBLE_GAP_EVENT_OBSERVATION_ENABLE_COMP:
            break;
        case RBLE_GAP_EVENT_OBSERVATION_DISABLE_COMP:
            break;
        case RBLE_GAP_EVENT_BROADCAST_ENABLE_COMP:
            APP_GAP_Broadcast_Enable_CallBack( event );
            break;
        case RBLE_GAP_EVENT_BROADCAST_DISABLE_COMP:
            break;
        case RBLE_GAP_EVENT_SET_BONDING_MODE_COMP:
            APP_GAP_Set_Bonding_Mode_CallBack( event );
            break;
        case RBLE_GAP_EVENT_SET_SECURITY_REQUEST_COMP:
            APP_GAP_Set_Security_Request_CallBack( event );
            break;
        case RBLE_GAP_EVENT_GET_DEVICE_INFO_COMP:
            break;
        case RBLE_GAP_EVENT_RPA_RESOLVED:
            APP_GAP_RPA_Resolved_CallBack( event );
            break;
        case RBLE_GAP_EVENT_GET_WHITE_LIST_SIZE_COMP:
            break;
        case RBLE_GAP_EVENT_ADD_TO_WHITE_LIST_COMP:
            break;
        case RBLE_GAP_EVENT_DEL_FROM_WHITE_LIST_COMP:
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
            APP_GAP_Connection_CallBack( event );
            break;
        case RBLE_GAP_EVENT_CONNECTION_CANCEL_COMP:
            break;
        case RBLE_GAP_EVENT_DISCONNECT_COMP:
            APP_GAP_Disconnection_CallBack( event );
            break;
        case RBLE_GAP_EVENT_ADVERTISING_REPORT_IND:
            break;
        case RBLE_GAP_EVENT_BONDING_COMP:
            APP_GAP_Bonding_Callback( event );
            break;
        case RBLE_GAP_EVENT_BONDING_REQ_IND:
            APP_GAP_Bonding_Request_CallBack( event );
            break;
        case RBLE_GAP_EVENT_CHANGE_CONNECTION_PARAM_REQ_IND:
            break;
        case RBLE_GAP_EVENT_CHANGE_CONNECTION_PARAM_COMP:
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

/******************************************************************************************/
/**
 *  @brief      Event Respond : GAP Reset Complete Event
 *
 *  @param      event   GAP Event Parameters
 *
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_GAP_Reset_CallBack( RBLE_GAP_EVENT *event )
{
    RBLE_STATUS     ret_status = FALSE;

    if(RBLE_OK == event->param.status) {
        ret_status = TRUE;
        
        /* Set Bonding Mode */
        APP_Set_RunCmd(GAP_SET_BONDING_MODE_CMD);
    }

    APP_Disp("e:RstCmp");
    APP_Disp_Status(event->param.status);
    return( ret_status );
}

/******************************************************************************************/
/**
 *  @brief      Event Respond : GAP Broadcast Enable Complete Event
 *
 *  @param      event   GAP Event Parameters
 *
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_GAP_Broadcast_Enable_CallBack( RBLE_GAP_EVENT *event )
{
    RBLE_STATUS     ret_status = FALSE;

    if(RBLE_OK == event->param.status) {
        ret_status = TRUE;
    }

    APP_Disp("e:BcstEn");
    APP_Disp_Status(event->param.status);
    return( ret_status );
}

/******************************************************************************************/
/**
 *  @brief      Event Respond : GAP Connection Complete Event
 *
 *  @param      event   GAP Event Parameters
 *
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_GAP_Connection_CallBack( RBLE_GAP_EVENT *event )
{
    RBLE_STATUS ret_status = FALSE;

    if(RBLE_OK == event->param.status) {
        ConnectionHdl  = event->param.conn_comp.connect_info.conhdl;
        RemoteDevice   = event->param.conn_comp.connect_info.peer_addr;
        FoundFlg       = FALSE;
        CheckedIrkFlg  = FALSE;
        ret_status = TRUE;
        
        /* Enable Profile (Profile is Disabled automatically when Disconnection) */
        APP_Set_RunCmd(SCP_Sensor_Enable_CMD);
    }

    APP_Disp("e:Connct");
    APP_Disp_Status(event->param.status);
    return( ret_status );
}

/******************************************************************************************/
/**
 *  @brief      Event Respond : GAP Disconnection Complete Event
 *
 *  @param      event   GAP Event Parameters
 *
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_GAP_Disconnection_CallBack( RBLE_GAP_EVENT *event )
{
    ConnectionHdl = ILLEGAL_CONNECTION_HANDLE;

    /* Disable timer for Notification trigger */
    RBLE_Clear_Timer_Id( _RBLE_TIMER_ID_APP );

    /* Restart Broadcast for the Next Connection */
    APP_Set_RunCmd(GAP_BROADCAST_ENABLE_CMD);

    APP_Disp("e:DisCon");
    APP_Disp_Status(event->param.disconnect.status);
    return( TRUE );
}

/******************************************************************************************/
/**
 *  @brief      Event Respond : GAP Set Bonding Mode Complete Event
 *
 *  @param      event   GAP Event Parameters
 *
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_GAP_Set_Bonding_Mode_CallBack( RBLE_GAP_EVENT *event )
{
    RBLE_STATUS ret_status = FALSE;

    if(RBLE_OK == event->param.status) {
        ret_status = TRUE;
        
        /* Set Security Request */
        APP_Set_RunCmd(GAP_SET_SECURITY_REQUEST_CMD);
    }

    APP_Disp("e:BondMd");
    APP_Disp_Status(event->param.status);
    return( ret_status );
}

/******************************************************************************************/
/**
 *  @brief      Event Respond : GAP Set Security Request Event
 *
 *  @param      event   GAP Event Parameters
 *
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_GAP_Set_Security_Request_CallBack( RBLE_GAP_EVENT *event )
{
    RBLE_STATUS ret_status = FALSE;

    if(RBLE_OK == event->param.set_sec_req.status) {
        ret_status = TRUE;
        
        /* Start Broadcast for the First Connection */
        APP_Set_RunCmd(GAP_BROADCAST_ENABLE_CMD);
    }

    APP_Disp("e:SecReq");
    APP_Disp_Status(event->param.set_sec_req.status);
    return( ret_status );
}

/******************************************************************************************/
/**
 *  @brief      Event Respond : GAP RPA Resolved Event
 *
 *  @param      event   GAP Event Parameters
 *
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_GAP_RPA_Resolved_CallBack( RBLE_GAP_EVENT *event )
{
    APP_Disp("e:RpaRsv");
    return( TRUE );
}

/******************************************************************************************/
/**
 *  @brief      Event Respond : GAP Bonding Complete Event
 *
 *  @param      event   GAP Event Parameters
 *
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_GAP_Bonding_Callback( RBLE_GAP_EVENT *event )
{
    SecStatus      = event->param.bonding_comp.sec_prop | RBLE_SMP_BONDED;
    BondedAddr     = RemoteDevice  ;
    APP_Disp("e:Bonded");
    APP_Disp_Status(RBLE_OK);

    return( TRUE );
}

/******************************************************************************************/
/**
 *  @brief      Event Respond : GAP Bonding Request Event
 *
 *  @param      event   GAP Event Parameters
 *
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_GAP_Bonding_Request_CallBack( RBLE_GAP_EVENT *event )
{
    /* Reset Remote Device Property */
    SecStatus      = RBLE_SMP_SEC_NONE;
    memset(&BondedAddr, 0, sizeof(RBLE_BD_ADDR));
    memset(&LocalKey  , 0, sizeof(APP_KEY_DATA));
    memset(&RemoteKey , 0, sizeof(APP_KEY_DATA));

    /* Reply Bonding Response */
    APP_Set_RunCmd(GAP_BONDING_RESPONSE_CMD);

    APP_Disp("e:BndReq");
    return( TRUE );
}

/* --------------------------- Security Manager -------------------------------------*/

/******************************************************************************************/
/**
 *  @brief      Event Handler : SM Event
 *
 *  @param      event   SM Event Parameters
 *
 *  @retval     None
 */
/******************************************************************************************/
static void APP_SM_CallBack( RBLE_SM_EVENT *event )
{
    switch( event->type ){
        case RBLE_SM_EVENT_SET_CNF:
            break;
        case RBLE_SM_ENC_START_IND:
            APP_SM_Encryption_Start_Callback( event );
            break;
        case RBLE_SM_TK_REQ_IND:
            APP_SM_TK_Request_CallBack( event );
            break;
        case RBLE_SM_LTK_REQ_IND:
            APP_SM_LTK_Request_CallBack( event );
            break;
        case RBLE_SM_LTK_REQ_FOR_ENC_IND:
            APP_SM_LTK_Request_For_Encryption_CallBack( event );
            break;
        case RBLE_SM_IRK_REQ_IND:
            APP_SM_IRK_Request_CallBack( event );
            break;
        case RBLE_SM_CSRK_REQ_IND:
            break;
        case RBLE_SM_KEY_IND:
            APP_SM_Key_CallBack( event );
            break;
        case RBLE_SM_CHK_BD_ADDR_REQ:
            APP_SM_BDAddr_Check_Request_CallBack( event );
            break;
        case RBLE_SM_TIMEOUT_EVT:
            break;
        case RBLE_SM_EVENT_COMMAND_DISALLOWED_IND:
            break;
        default:
            break;
    }
}

/******************************************************************************************/
/**
 *  @brief      Event Respond : SM Start Encryption Complete Event
 *
 *  @param      event   SM Event Parameters
 *
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_SM_Encryption_Start_Callback( RBLE_SM_EVENT *event )
{
    APP_Disp("e:StaEnc");
    APP_Disp_Status(event->param.sec_start.status);

    return( TRUE );
}

/******************************************************************************************/
/**
 *  @brief      Event Respond : SM TK Request Event
 *
 *  @param      event   SM Event Parameters
 *
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_SM_TK_Request_CallBack( RBLE_SM_EVENT *event )
{
    ConnectionIdx = event->param.tk_req.idx;

    /* Reply TK(Temporary Key) */
    APP_Set_RunCmd(SM_TK_REQ_RESP_CMD);
    APP_Disp("e:TkReq ");

    return( TRUE );
}

/******************************************************************************************/
/**
 *  @brief      Event Respond : SM LTK Request Event
 *
 *  @param      event   SM Event Parameters
 *
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_SM_LTK_Request_CallBack( RBLE_SM_EVENT *event )
{
    ConnectionIdx = event->param.ltk_req.idx;

    /* Reply LTK(Long Term Key) */
    APP_Set_RunCmd(SM_LTK_REQ_RESP_CMD);
    APP_Disp("e:LtkReq");

    return( TRUE );
}

/******************************************************************************************/
/**
 *  @brief      Event Respond : SM LTK Request For Encryption Event
 *
 *  @param      event   SM Event Parameters
 *
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_SM_LTK_Request_For_Encryption_CallBack( RBLE_SM_EVENT *event )
{
    RcvEdiv = event->param.ltk_req_for_enc.ediv;
    memcpy(&RcvNb, &event->param.ltk_req_for_enc.nb, sizeof(RcvNb));

    /* Reply LTK(Long Term Key) */
    APP_Set_RunCmd(SM_LTK_REQ_FOR_ENC_RESP_CMD);
    APP_Disp("e:Ltr4Re");

    return( TRUE );
}

/******************************************************************************************/
/**
 *  @brief      Event Respond : SM IRK Request Event
 *
 *  @param      event   SM Event Parameters
 *
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_SM_IRK_Request_CallBack( RBLE_SM_EVENT *event )
{
    ConnectionIdx = event->param.irk_req.idx;

    /* Reply IRK(Identity Resolving Key) */
    APP_Set_RunCmd(SM_IRK_REQ_RESP_CMD);
    APP_Disp("e:IrkReq");

    return( TRUE );
}

/******************************************************************************************/
/**
 *  @brief      Event Respond : SM Key Indication Event
 *
 *  @param      event   SM Event Parameters
 *
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_SM_Key_CallBack( RBLE_SM_EVENT *event )
{
    /* Store Encryption Key */
    if(RBLE_SMP_KDIST_ENCKEY       == event->param.key_ind.key_code)
    {
        RemoteKey.EncKey         = event->param.key_ind;
        RemoteKey.Key_Set_Flags |= (1 << APP_ENCKEY );
    }
    else if(RBLE_SMP_KDIST_IDKEY   == event->param.key_ind.key_code)
    {
        RemoteKey.IrKey          = event->param.key_ind;
        RemoteKey.Key_Set_Flags |= (1 << APP_IDKEY  );
    }
    else if(RBLE_SMP_KDIST_SIGNKEY == event->param.key_ind.key_code)
    {
        RemoteKey.SignKey        = event->param.key_ind;
        RemoteKey.Key_Set_Flags |= (1 << APP_SIGNKEY);
    }
    APP_Disp("e:SecKey");

    return( TRUE );
}

/******************************************************************************************/
/**
 *  @brief      Event Respond : SM Check BD Address Request Event
 *
 *  @param      event   SM Event Parameters
 *
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_SM_BDAddr_Check_Request_CallBack( RBLE_SM_EVENT *event )
{
    ConnectionIdx = event->param.chk_bdaddr.idx;
    
    /* Check if Remote Device is already bonded */
    FoundFlg = APP_Compare_BDAddr(&BondedAddr, &event->param.chk_bdaddr.addr);
    
    /* Reply BD Address Check Result */
    APP_Set_RunCmd(SM_CHK_BD_ADDR_REQ_RESP_CMD);
    APP_Disp("e:ChkADR");

    return( TRUE );
}

/* ------------------------- Sample Custom Profile-------------------------------*/

/******************************************************************************************/
/**
 *  @brief      Event Handler : SCP Server Event
 *
 *  @param      event   SCP Server Event Parameters
 *
 *  @retval     None
 */
/******************************************************************************************/
static void APP_SCPS_CallBack( RBLE_SCPS_EVENT *event )
{
    switch( event->type){
        case RBLE_SCP_EVENT_SERVER_ENABLE_COMP:
            APP_SCPS_Enable_CallBack( event );
            break;
        case RBLE_SCP_EVENT_SERVER_DISABLE_COMP:
            break;
        case RBLE_SCP_EVENT_SERVER_ERROR_IND:
            break;
        case RBLE_SCP_EVENT_SERVER_SEND_NOTIFY_COMP:
            APP_SCPS_Send_Notify_CallBack( event );
            break;
        case RBLE_SCP_EVENT_SERVER_SEND_IND_COMP:
            break;
        case RBLE_SCP_EVENT_SERVER_CHG_INDNTF_IND:
            APP_SCPS_Change_NtfInd_Config_CallBack( event );
            break;
        case RBLE_SCP_EVENT_SERVER_CHG_CHAR_IND:
            break;
        case RBLE_SCP_EVENT_SERVER_COMMAND_DISALLOWED_IND:
            break;
        default:
            break;
    }
}

/******************************************************************************************/
/**
 *  @brief      Event Respond : SCP Server Enable Complete Event
 *
 *  @param      event   SCP Server Event Parameters
 *
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_SCPS_Enable_CallBack( RBLE_SCPS_EVENT *event )
{
    DataCnt = 0;
    APP_Disp("e:ScpEna");
    APP_Disp_Status(event->param.server_enable.status);
    
    return( TRUE );
}

/******************************************************************************************/
/**
 *  @brief      Event Respond : SCP Server Change Notification/Indication Configuration Event
 *
 *  @param      event   SCP Server Event Parameters
 *
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_SCPS_Change_NtfInd_Config_CallBack( RBLE_SCPS_EVENT *event )
{
    if (event->param.cfg_indntf.char_code == RBLE_SCP_SCS_NTF_CFG)
    {
        if(event->param.cfg_indntf.cfg_val & RBLE_SCP_START_NTF)
        {
            APP_Disp("e:NtfEna");
            APP_Disp_Status(RBLE_OK);
            
            /* Enable timer for Notification trigger */
            #ifdef USE_RSK_ADC
            RBLE_Set_Timer_Id( _RBLE_TIMER_ID_APP, R_ADC_Start  , 100 );
            #else
            RBLE_Set_Timer_Id( _RBLE_TIMER_ID_APP, APP_Send_Data, 100 );
            #endif
        }
        else
        {
            APP_Disp("e:NtfDis");
            APP_Disp_Status(RBLE_OK);
            
            /* Disable timer for Notification trigger */
            RBLE_Clear_Timer_Id( _RBLE_TIMER_ID_APP );
        }
    }
    
    return( TRUE );
}

/******************************************************************************************/
/**
 *  @brief      Event Respond : SCP Server Send Notification Complete Event
 *
 *  @param      event   SCP Server Event Parameters
 *
 *  @retval     TRUE if succeeded , FALSE if failed
 */
/******************************************************************************************/
static BOOL APP_SCPS_Send_Notify_CallBack( RBLE_SCPS_EVENT *event )
{
    APP_Disp("e:ScpNtf");
    APP_Disp_Status(event->param.send_notify.status);
    return( TRUE );
}

/* ---------------------------------- LCD Display Functions  -----------------------------*/

#ifdef USE_RSK_LCD
/******************************************************************************************/
/**
 *  @brief      LCD Display : String
 *
 *  @param      str     Displayed String
 *
 *  @retval     None
 */
/******************************************************************************************/
static void APP_Disp( char_t* str )
{
    char  lcd_next[9];
    uint8_t len;
    
    /* cut the string (max number of character is 8) */
    len = (uint8_t)strlen(str);
    len = (len > 8) ? (8) : (len);
    strncpy((char *)lcd_next, str, len);
    lcd_next[len] = '\0';

    if(LcdPrev[0] == '\0')
    {
        GLOBAL_INT_DISABLE();
        Display_LCD(LCD_LINE1, lcd_next);
        GLOBAL_INT_RESTORE();
    }
    else
    {
        GLOBAL_INT_DISABLE();
        Display_LCD(LCD_LINE1, LcdPrev);
        GLOBAL_INT_RESTORE();
        GLOBAL_INT_DISABLE();
        Display_LCD(LCD_LINE2, lcd_next);
        GLOBAL_INT_RESTORE();
    }
    memcpy(LcdPrev, lcd_next, 9);
}

/******************************************************************************************/
/**
 *  @brief      LCD Display : Status
 *
 *  @param      status  rBLE API Return Status , defined by RBLE_STATUS_enum
 *
 *  @retval     None
 */
/******************************************************************************************/
static void APP_Disp_Status( RBLE_STATUS status )
{
    APP_Disp(APP_Get_Status_Str(status));
}

/******************************************************************************************/
/**
 *  @brief      LCD Display : PassKey Number
 *
 *  @param      passkey PassKey
 *
 *  @retval     None
 */
/******************************************************************************************/
static void APP_Disp_PassKey( uint32_t passkey )
{
    char  lcd_next[9];
    uint8_t i;

    /* passkey range is 6digits(000,000 - 999,999) */
    for(i = 0; i < 6; i++)
    {
        lcd_next[6 - 1 - i] = '0' + (char_t)(passkey % 10);
        passkey /= 10;
    }
    lcd_next[6] = ' ' ;
    lcd_next[7] = ' ' ;
    lcd_next[8] = '\0';

    GLOBAL_INT_DISABLE();
    Display_LCD(LCD_LINE1, "PassKey ");
    GLOBAL_INT_RESTORE();
    GLOBAL_INT_DISABLE();
    Display_LCD(LCD_LINE2, lcd_next);
    GLOBAL_INT_RESTORE();

    LcdPrev[0] = '\0';
}

/******************************************************************************************/
/**
 *  @brief      Search Status String Pointer for LCD Display
 *
 *  @param      status  rBLE API Return Status , defined by RBLE_STATUS_enum
 *
 *  @retval     Pointer to the Status String
 */
/******************************************************************************************/
static char_t  *APP_Get_Status_Str( RBLE_STATUS status )
{
    typedef struct Status_String_Info_t {
        RBLE_STATUS     rble_status;
        char_t*         string;
    } STATUS_STRING_INFO;

    static const STATUS_STRING_INFO status_string[] = {
        { RBLE_OK,                                  ">OK     "},
        { RBLE_UNKNOWN_HCI_COMMAND,                 ">UN_HCI "},
        { RBLE_UNKNOWN_CONNECTION_ID,               ">UN_CNID"},
        { RBLE_HARDWARE_FAILURE,                    ">HW_FAIL"},
        { RBLE_PAGE_TIMEOUT,                        ">PAGE_TO"},
        { RBLE_AUTH_FAILURE,                        ">ATHFAIL"},
        { RBLE_PIN_MISSING,                         ">PINMISS"},
        { RBLE_MEMORY_CAPA_EXCEED,                  ">MEM_EXD"},
        { RBLE_CON_TIMEOUT,                         ">CON_TO "},
        { RBLE_CON_LIMIT_EXCEED,                    ">CON_EXD"},
        { RBLE_COMMAND_DISALLOWED,                  ">CMD_DIS"},
        { RBLE_CONN_REJ_LIMITED_RESOURCES,          ">REJ_RES"},
        { RBLE_CONN_REJ_SECURITY_REASONS,           ">REJ_SEC"},
        { RBLE_CONN_REJ_UNACCEPTABLE_BDADDR,        ">REJ_BDA"},
        { RBLE_CONN_ACCEPT_TIMEOUT_EXCEED,          ">TO_EXD "},
        { RBLE_UNSUPPORTED,                         ">UNSPRT "},
        { RBLE_INVALID_HCI_PARAM,                   ">INV_PRM"},
        { RBLE_REMOTE_USER_TERM_CON,                ">RMT_USR"},
        { RBLE_REMOTE_DEV_TERM_LOW_RESOURCES,       ">RMT_RES"},
        { RBLE_REMOTE_DEV_POWER_OFF,                ">RMT_PWR"},
        { RBLE_CON_TERM_BY_LOCAL_HOST,              ">TRM_LCL"},
        { RBLE_REPEATED_ATTEMPTS,                   ">RPT_ATP"},
        { RBLE_PAIRING_NOT_ALLOWED,                 ">PAR_ALW"},
        { RBLE_UNSUPPORTED_REMOTE_FEATURE,          ">RMT_FET"},
        { RBLE_UNSPECIFIED_ERROR,                   ">ERROR  "},
        { RBLE_LMP_RSP_TIMEOUT,                     ">RSP_TO "},
        { RBLE_ENC_MODE_NOT_ACCEPT,                 ">ENC_NOT"},
        { RBLE_LINK_KEY_CANT_CHANGE,                ">LINKKEY"},
        { RBLE_INSTANT_PASSED,                      ">INST_PS"},
        { RBLE_PAIRING_WITH_UNIT_KEY_NOT_SUP,       ">PARKEY "},
        { RBLE_DIFF_TRANSACTION_COLLISION,          ">TRACCOL"},
        { RBLE_CHANNEL_CLASS_NOT_SUP,               ">CH_CLS "},
        { RBLE_INSUFFICIENT_SECURITY,               ">INS_SEC"},
        { RBLE_PARAM_OUT_OF_MAND_RANGE,             ">PRM_OUT"},
        { RBLE_SP_NOT_SUPPORTED_HOST,               ">SP_HOST"},
        { RBLE_HOST_BUSY_PAIRING,                   ">HST_BSY"},
        { RBLE_CONTROLLER_BUSY,                     ">CTL_BSY"},
        { RBLE_UNACCEPTABLE_CONN_INT,               ">CON_INT"},
        { RBLE_DIRECT_ADV_TO,                       ">DADV_TO"},
        { RBLE_TERMINATED_MIC_FAILURE,              ">MICFAIL"},
        { RBLE_CONN_FAILED_TO_BE_ES,                ">FAIL_ES"},
        { RBLE_GAP_INVALID_PARAM_ERR,               ">GP_PRM "},
        { RBLE_GAP_AUTO_EST_ERR,                    ">GP_AEST"},
        { RBLE_GAP_SELECT_EST_ERR,                  ">GP_SEST"},
        { RBLE_GAP_SET_RECON_ADDR_ERR,              ">GP_ADDR"},
        { RBLE_GAP_SET_PRIVACY_FEAT_ERR,            ">GP_PRV "},
        { RBLE_GATT_INVALID_PARAM_ERR,              ">GT_PRM "},
        { RBLE_GATT_INDICATE_NOT_ALLOWED,           ">GT_IND "},
        { RBLE_GATT_NOTIFY_NOT_ALLOWED,             ">GT_NTF "},
        { RBLE_ATT_INVALID_PARAM_ERR,               ">AT_PRM "},
        { RBLE_SM_INVALID_PARAM_ERR,                ">SM_PRM "},
        { RBLE_SM_PAIR_ERR_PASSKEY_ENTRY_FAILED,    ">SM_PSKY"},
        { RBLE_SM_PAIR_ERR_OOB_NOT_AVAILABLE,       ">SM_OOB "},
        { RBLE_SM_PAIR_ERR_AUTH_REQUIREMENTS,       ">SM_AUTH"},
        { RBLE_SM_PAIR_ERR_CFM_VAL_FAILED,          ">SM_CFM "},
        { RBLE_SM_PAIR_ERR_PAIRING_NOT_SUPPORTED,   ">SM_PAIR"},
        { RBLE_SM_PAIR_ERR_ENCRYPTION_KEY_SIZE,     ">SM_KYSZ"},
        { RBLE_SM_PAIR_ERR_CMD_NOT_SUPPORTED,       ">SM_CMD "},
        { RBLE_SM_PAIR_ERR_UNSPECIFIED_REASON,      ">SM_RSN "},
        { RBLE_SM_PAIR_ERR_REPEATED_ATTEMPTS,       ">SM_ATP "},
        { RBLE_SM_PAIR_ERR_INVALID_PARAMS,          ">SM_PRM "},
        { RBLE_L2C_INVALID_PARAM_ERR,               ">L2C_PRM"},
        { RBLE_PRF_ERR_INVALID_PARAM,               ">PF_PRM "},
        { RBLE_PRF_ERR_INEXISTENT_HDL,              ">PF_HDL "},
        { RBLE_PRF_ERR_STOP_DISC_CHAR_MISSING,      ">PF_CHAR"},
        { RBLE_PRF_ERR_MULTIPLE_IAS,                ">PF_IAS "},
        { RBLE_PRF_ERR_INCORRECT_PROP,              ">PF_PROP"},
        { RBLE_PRF_ERR_MULTIPLE_CHAR,               ">PF_MT_C"},
        { RBLE_PRF_ERR_NOT_WRITABLE,                ">PF_WR  "},
        { RBLE_PRF_ERR_NOT_READABLE,                ">PF_RD  "},
        { RBLE_PRF_ERR_REQ_DISALLOWED,              ">PF_REQ "},
        { RBLE_PRF_ERR_NTF_DISABLED,                ">PF_NTF "},
        { RBLE_PRF_ERR_IND_DISABLED,                ">PF_IND "},
        { RBLE_PRF_ERR_ATT_NOT_SUPPORTED,           ">PF_ATT "},
        /*------ When adding it, it's added in after a while. -----*/
        { RBLE_TRANS_ERR,                           ">TRANS  "},
        { RBLE_STATUS_ERROR,                        ">STATUS "},
        { RBLE_PARAM_ERR,                           ">PARAM  "},
        { RBLE_BUSY,                                ">BUSY   "},
        { RBLE_SHORTAGE_OF_RESOURCE,                ">SHT_RES"},
        { RBLE_VERSION_FAIL,                        ">VERFAIL"},
        { RBLE_TEST_VERSION,                        ">TESTVER"},
        { RBLE_ERR,                                 ">UNKSTAT"},
    };

    char_t*     ret_ptr = NULL;
    uint16_t    i;

    /* Search Status String Pointer */
    for(i = 0; i < (sizeof(status_string) / sizeof(STATUS_STRING_INFO)); i++) {
        if ( status == status_string[i].rble_status ) {
            ret_ptr = status_string[i].string;
            break;
        }
    }
    /* Set Default Pointer when Not Found */
    if ( NULL == ret_ptr ) {
        ret_ptr = status_string[(sizeof(status_string) / sizeof(STATUS_STRING_INFO)) - 1].string;
    }
    return( ret_ptr );
}
#endif /* USE_RSK_LCD */

/* --------------------------- Utility -------------------------------------*/

#ifdef USE_RSK_ADC
/******************************************************************************************/
/**
 *  @brief      Data Send by SCP Server Notification
 *
 *  @param      val     Data Value
 *
 *  @retval     None
 */
/******************************************************************************************/
void APP_Send_Data(uint8_t val)
{
    DataValue = val;
    /* Send Data by Profile Notification */
    APP_Set_RunCmd(SCP_Sensor_Send_Notify_CMD);
    
    /* Enable timer for Notification trigger */
    RBLE_Set_Timer_Id( _RBLE_TIMER_ID_APP, R_ADC_Start, 100 );
}
#else
/******************************************************************************************/
/**
 *  @brief      Data Send by SCP Server Notification
 *
 *  @retval     None
 */
/******************************************************************************************/
void APP_Send_Data(void)
{
    DataValue = 0x00;
    /* Send Data by Profile Notification */
    APP_Set_RunCmd(SCP_Sensor_Send_Notify_CMD);
    
    /* Enable timer for Notification trigger */
    RBLE_Set_Timer_Id( _RBLE_TIMER_ID_APP, APP_Send_Data, 100 );
}
#endif

/******************************************************************************************/
/**
 *  @brief      Disconnection
 *
 *  @retval     None
 */
/******************************************************************************************/
void APP_Disconnect(void)
{
    /* Disconnection */
    APP_Set_RunCmd(GAP_DISCONNECT_CMD);
}

/******************************************************************************************/
/**
 *  @brief      Queueing Command
 *
 *  @param      cmd     Command Value , defined by RBLE_APP_CMD_NO_enum
 *
 *  @retval     None
 */
/******************************************************************************************/
static void APP_Set_RunCmd( uint16_t cmd )
{
    RunCmd[WrPtr] = cmd;

    /* Operate Run Command Queue Pointer as a Ring Buffer */
    GLOBAL_INT_DISABLE();
    WrPtr = (WrPtr == (RUN_COMMAND_QUEUE_SIZE - 1)) ? (0) : (WrPtr + 1);
    GLOBAL_INT_RESTORE();
}

/******************************************************************************************/
/**
 *  @brief      Compare BD Addresses
 *
 *  @param      a       BD Address
 *  @param      b       BD Address
 *
 *  @retval     TRUE if same , FALSE if different
 */
/******************************************************************************************/
static BOOL APP_Compare_BDAddr(RBLE_BD_ADDR* a, RBLE_BD_ADDR* b)
{
    uint8_t i;

    for(i = 0; i < RBLE_BD_ADDR_LEN; i++)
    {
        if(a->addr[i] != b->addr[i])
        {
            return FALSE;
        }
    }
    return TRUE;
}

/******************************************************************************************/
/**
 *  @brief      Get Random Number
 *
 *  @retval     Random Number
 */
/******************************************************************************************/
static int_t APP_Get_Random( void )
{
    return( rand() );
}

