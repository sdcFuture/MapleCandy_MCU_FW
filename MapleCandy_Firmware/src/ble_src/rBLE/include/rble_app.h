/**
 ****************************************************************************************
 *
 * @file        rble_app.h
 *
 * @brief       rBLE Sample Program
 *
 * Copyright(C) 2013-2015 Renesas Electronics Corporation
 *
 * $Rev: 6065 $
 *
 ****************************************************************************************
 */

#ifndef _rBLE_APP_H
#define _rBLE_APP_H

#include    "rble.h"
#include    "rble_api.h"
#include    "rble_api_custom.h"

/**************************** Define Declaration **************************************/

enum RBLE_APP_CMD_NO_enum {
    RBLE_APP_CMD_NON,
    GAP_RESET_CMD,
    GAP_SET_NAME_CMD,
    GAP_OBSERVATION_ENABLE_CMD,
    GAP_OBSERVATION_DISABLE_CMD,
    GAP_BROADCAST_ENABLE_CMD,
    GAP_BROADCAST_DISABLE_CMD,
    GAP_SET_BONDING_MODE_CMD,
    GAP_SET_SECURITY_REQUEST_CMD,
    GAP_GET_DEVICE_INFO_CMD,
    GAP_GET_WHITE_LIST_SIZE_CMD,
    GAP_ADD_TO_WHITE_LIST_CMD,
    GAP_DEL_FROM_WHITE_LIST_CMD,
    GAP_GET_REMOTE_DEVICE_NAME_CMD,
    GAP_GET_REMOTE_DEVICE_INFO_CMD,
    GAP_DEVICE_SEARCH_CMD,
    GAP_SET_RECONNECT_ADDRESS_CMD,
    GAP_SET_RANDOM_ADDRESS_CMD,
    GAP_SET_PRIVACY_FEATURE_CMD,
    GAP_SET_PERIPHERAL_PRIVACY_FEATURE_CMD,
    GAP_CREATE_CONNECTION_CMD,
    GAP_CONNECTION_CANCEL_CMD,
    GAP_DISCONNECT_CMD,
    GAP_START_BONDING_CMD,
    GAP_BONDING_INFO_IND_CMD,
    GAP_BONDING_RESPONSE_CMD,
    GAP_CHANGE_CONNECTION_PARAM_CMD,
    GAP_CHANNEL_MAP_REQ_CMD,
    GAP_READ_RSSI_CMD,
    GAP_AUTHORIZED_IND_CMD,

    SM_SET_KEY_CMD,
    SM_START_ENC_CMD,
    SM_TK_REQ_RESP_CMD,
    SM_LTK_REQ_RESP_CMD,
    SM_LTK_REQ_FOR_ENC_RESP_CMD,
    SM_IRK_REQ_RESP_CMD,
    SM_CSRK_REQ_RESP_CMD,
    SM_CHK_BD_ADDR_REQ_RESP_CMD,
    GATT_ENABLE_CMD,
    GATT_DISCOVERY_SERVICE_REQUEST_CMD,
    GATT_DISCOVERY_CHAR_REQUEST_CMD,
    GATT_DISCOVERY_CHAR_DESCRIPTOR_REQUEST_CMD,
    GATT_READ_CHAR_REQUEST_CMD,
    GATT_WRITE_CHAR_REQUEST_CMD,
    GATT_WRITE_RELIABLE_REQUEST_CMD,
    GATT_EXECUTE_WRITE_CHAR_REQUEST_CMD,
    GATT_NOTIFY_REQUEST_CMD,
    GATT_INDICATE_REQUEST_CMD,
    GATT_WRITE_RESPONSE_CMD,
    GATT_SET_PERMISSION_CMD,
    GATT_SET_DATA_CMD,

/* --------------------------- Find Me Profile ---------------------------------------*/
    FMP_Target_Enable_CMD,
    FMP_Target_Disable_CMD,
    FMP_Locator_Enable_CMD,
    FMP_Locator_Disable_CMD,
    FMP_Locator_Set_Alert_CMD,
    
/* ------------------------- Health Thermometer Profile-------------------------------*/
    HTP_Thermometer_Enable_CMD,
    HTP_Thermometer_Disable_CMD,
    HTP_Thermometer_Send_Temp_CMD,
    HTP_Thermometer_Req_Measurement_Period_Ind_CMD,
    HTP_Collector_Enable_CMD,
    HTP_Collector_Disable_CMD,
    HTP_COLLECTOR_READ_CHAR_CMD,
    HTP_COLLECTOR_WRITE_CHAR_CMD,
    HTP_Collector_Set_Measurement_Period_CMD,
    
/* ------------------------- Proximity Profile---------------------------------------*/
    PXP_Reporter_Enable_CMD,
    PXP_Reporter_Disable_CMD,
    PXP_Monitor_Enable_CMD,
    PXP_Monitor_Disable_CMD,
    PXP_Monitor_Get_Alert_Level_CMD,
    PXP_Monitor_Set_Alert_Level_CMD,
    PXP_Monitor_Get_Tx_Power_CMD,
    
/* ------------------------- Blood Pressure Profile-------------------------------*/
    BLP_Sensor_Enable_CMD,
    BLP_Sensor_Disable_CMD,
    BLP_Sensor_Send_Measurements_CMD,
    BLP_Collector_Enable_CMD,
    BLP_Collector_Disable_CMD,
    BLP_Collector_Read_Char_CMD,
    BLP_Collector_Write_Char_CMD,

/* ------------------------- HID over GATT Profile-------------------------------*/
    HGP_HDevice_Enable_CMD,
    HGP_HDevice_Disable_CMD,
    HGP_HDevice_Send_Report_CMD,
    HGP_HDevice_Send_Battery_Level_CMD,
    HGP_BHost_Enable_CMD,
    HGP_BHost_Disable_CMD,
    HGP_BHost_Read_Char_CMD,
    HGP_BHost_Read_By_UUID_Char_CMD,
    HGP_BHost_Write_Char_CMD,
    HGP_BHost_Set_Report_CMD,
    HGP_BHost_Write_Protocol_Mode_CMD,
    HGP_BHost_Data_Output_CMD,
    HGP_RHost_Enable_CMD,
    HGP_RHost_Disable_CMD,
    HGP_RHost_Read_Char_CMD,
    HGP_RHost_Read_By_UUID_Char_CMD,
    HGP_RHost_Read_Long_Char_CMD,
    HGP_RHost_Write_Char_CMD,
    HGP_RHost_Set_Report_CMD,
    HGP_RHost_Write_Protocol_Mode_CMD,
    HGP_RHost_Data_Output_CMD,
    HGP_RHost_Write_Control_Point_CMD,

/* ------------------------- Scan Parameters Profeils-------------------------------*/
    SPP_Server_Enable_CMD,
    SPP_Server_Disable_CMD,
    SPP_Server_Send_Refresh_CMD,
    SPP_Client_Enable_CMD,
    SPP_Client_Disable_CMD,
    SPP_Client_Write_Char_CMD,
    SPP_Client_Write_Interval_CMD,

/* ------------------------- Heart Rate Profile-------------------------------*/
    HRP_Sensor_Enable_CMD,
    HRP_Sensor_Disable_CMD,
    HRP_Sensor_Send_Measurements_CMD,
    HRP_Collector_Enable_CMD,
    HRP_Collector_Disable_CMD,
    HRP_Collector_Read_Char_CMD,
    HRP_Collector_Write_Control_Point_CMD,
    HRP_Collector_Write_Char_CMD,
    
/* ------------------------- Cycling Speed and Cadence Profile-------------------------------*/
    CSCP_Sensor_Enable_CMD,
    CSCP_Sensor_Disable_CMD,
    CSCP_Sensor_Send_Measurements_CMD,
    CSCP_Sensor_Send_Sc_Control_Point_CMD,
    CSCP_Collector_Enable_CMD,
    CSCP_Collector_Disable_CMD,
    CSCP_Collector_Read_Char_CMD,
    CSCP_Collector_Write_Sc_Control_Point_CMD,
    CSCP_Collector_Write_Char_CMD,
    
/* ------------------------- Glucose Profile-------------------------------*/
    GLP_Sensor_Enable_CMD,
    GLP_Sensor_Disable_CMD,
    GLP_Sensor_Send_Measurements_CMD,
    GLP_Sensor_Send_Measurements_Context_CMD,
    GLP_Sensor_Send_Ra_Control_Point_CMD,
    GLP_Collector_Enable_CMD,
    GLP_Collector_Disable_CMD,
    GLP_Collector_Read_Char_CMD,
    GLP_Collector_Write_Ra_Control_Point_CMD,
    GLP_Collector_Write_Char_CMD,

/* ------------------------- Cycling Power Profeils-------------------------------*/
    CPP_Sensor_Enable_CMD,
    CPP_Sensor_Disable_CMD,
    CPP_Sensor_Send_Measurements_CMD,
    CPP_Sensor_Broadcast_Measurements_CMD,
    CPP_Sensor_Send_Vector_CMD,
    CPP_Sensor_Send_CP_Control_Point_CMD,
    CPP_Sensor_Send_Battery_Level_CMD,
    CPP_Sensor_Send_Write_Response_CMD,
    CPP_Collector_Enable_CMD,
    CPP_Collector_Disable_CMD,
    CPP_Collector_Read_Char_CMD,
    CPP_Collector_Write_CP_Control_Point_CMD,
    CPP_Collector_Write_Char_CMD,

/* ------------------------- Time Profile-------------------------------*/
    TIP_Server_Enable_CMD,
    TIP_Server_Disable_CMD,
    TIP_Server_Send_Current_Time_CMD,
    TIP_Server_Write_Data_CMD,
    TIP_Client_Enable_CMD,
    TIP_Client_Disable_CMD,
    TIP_Client_Read_Char_CMD,
    TIP_Client_Write_Char_CMD,
    TIP_Client_Write_Time_Update_CP_CMD,

    /* ------------------------- ANP Profile-------------------------------*/
    ANP_Server_Enable_CMD,
    ANP_Server_Disable_CMD,
    ANP_Server_Send_New_Alert_CMD,
    ANP_Server_Send_Unread_Alert_CMD,
    ANP_Client_Enable_CMD,
    ANP_Client_Disable_CMD,
    ANP_Client_Read_Char_CMD,
    ANP_Client_Write_Alert_Notification_CP_CMD,
    ANP_Client_Write_Char_CMD,

    /* ------------------------- LNP Profile-------------------------------*/
    LNP_Sensor_Enable_CMD,
    LNP_Sensor_Disable_CMD,
    LNP_Sensor_Send_Location_Speed_CMD,
    LNP_Sensor_Set_Position_Quality_CMD,
    LNP_Sensor_Send_LN_Control_Point_CMD,
    LNP_Sensor_Send_Navigation_CMD,
    LNP_Sensor_Send_Battery_Level_CMD,
    LNP_Collector_Enable_CMD,
    LNP_Collector_Disable_CMD,
    LNP_Collector_Read_Char_CMD,
    LNP_Collector_Write_LN_Control_Point_CMD,
    LNP_Collector_Write_Char_CMD,
    
    /* ------------------------- Phone Alert Status Profile-------------------------------*/
    PASP_Server_Enable_CMD,
    PASP_Server_Disable_CMD,
    PASP_Server_Send_Alert_Status_CMD,
    PASP_Server_Send_Ringer_Setting_CMD,
    PASP_Client_Enable_CMD,
    PASP_Client_Disable_CMD,
    PASP_Client_Read_Char_CMD,
    PASP_Client_Write_Ringer_Control_Point_CMD,
    PASP_Client_Write_Char_CMD,
    
    /* ------------------------- Running Speed and Cadence Profile-------------------------------*/
    RSCP_Sensor_Enable_CMD,
    RSCP_Sensor_Disable_CMD,
    RSCP_Sensor_Send_Measurements_CMD,
    RSCP_Sensor_Send_SC_Control_Point_CMD,
    RSCP_Collector_Enable_CMD,
    RSCP_Collector_Disable_CMD,
    RSCP_Collector_Read_Char_CMD,
    RSCP_Collector_Write_SC_Control_Point_CMD,
    RSCP_Collector_Write_Char_CMD,
    
/* ------------------------- Sample Custom Profile-------------------------------*/
    SCP_Sensor_Enable_CMD,
    SCP_Sensor_Disable_CMD,
    SCP_Sensor_Send_Notify_CMD,
    SCP_Sensor_Send_Indicate_CMD,
    SCP_Collector_Enable_CMD,
    SCP_Collector_Disable_CMD,
    SCP_Collector_Read_Char_CMD,
    SCP_Collector_Write_Char_CMD,

/* ------------------------- Vendor Specific-------------------------------*/
    VS_Enable_CMD,
    VS_Test_Rx_Start_CMD,
    VS_Test_Tx_Start_CMD,
    VS_Test_End_CMD,
    VS_Test_Set_Parameter_CMD,
    VS_Test_Read_RSSI_CMD,
    VS_Write_BdAddress_CMD,
    VS_Set_Tx_Power_CMD,
    VS_GPIO_Dir_CMD,
    VS_GPIO_Access_CMD,
    VS_Flash_Management_CMD,
    VS_Flash_Access_CMD,
    VS_Flash_Operation_CMD,
    VS_Flash_Get_Space_CMD,
    VS_Flash_Get_Eel_Ver_CMD,
    VS_Adapt_Enable_CMD,
    VS_Set_Params_CMD,

    EXIT_CMD
};

typedef struct RBLE_GAP_Observation_Enable_t {
    uint16_t            mode;
    RBLE_SCANNING_INFO  scan_info;
} RBLE_OBSERVATION_ENABLE_PARAM;

typedef struct RBLE_GAP_Broadcast_Enable_t {
    uint16_t            disc_mode;
    uint16_t            conn_mode;
    RBLE_ADV_INFO       adv_info;
} RBLE_BROADCAST_ENABLE_PARAM;

typedef struct RBLE_GAP_Privacy_Feature_Param_t {
    uint8_t             priv_flag;
    uint8_t             set_to_ll;
} RBLE_GAP_PRIV_FEAT_PRAM;

typedef struct RBLE_FMP_Locator_Set_Alert_Param_t {
    uint8_t                 alert_lvl;
} RBLE_FMP_LOCATOR_SET_ALERT_PARAM;

typedef struct RBLE_HTP_Therm_Enable_Param_t {
    uint8_t                 sec_lvl;
    uint8_t                 con_type;
    RBLE_HTP_THERM_PARAM    param;
} RBLE_HTP_THERM_ENABLE_PARAM;

typedef struct RBLE_BLP_Write_Char_t {
    uint8_t             char_code;
    uint16_t            cfg_val;
} RBLE_BLP_WRITE_CHAR_PARAM;

typedef struct RBLE_HGP_HDevice_Enable_Param_t {
    uint8_t                 sec_lvl;
    uint8_t                 con_type;
    RBLE_HGP_DEVICE_PARAM   param;
} RBLE_HGP_HDEVICE_ENABLE_PARAM;

typedef struct RBLE_HGP_HDevice_Send_Report_Param_t {
    uint8_t                 inst_idx;
    RBLE_HGP_REPORT_DESC    report;
} RBLE_HGP_HDEVICE_SEND_REPORT_PARAM;

typedef struct RBLE_HGP_BHost_Enable_Param_t {
    uint8_t                 con_type;
    uint8_t                 hids_inst_num;
    uint8_t                 bas_inst_num;
    RBLE_HIDS_CONTENT       hids[RBLE_HIDS_INST_MAX];
    RBLE_DIS11_CONTENT      dis;
    RBLE_BAS_CONTENT        bas[RBLE_BAS_INST_MAX];
} RBLE_HGP_BHost_ENABLE_PARAM;

typedef struct RBLE_HGP_BHost_Send_Report_Param_t {
    uint8_t                 inst_idx;
    RBLE_HGP_REPORT_DESC    report;
} RBLE_HGP_BHOST_SEND_REPORT_PARAM;

typedef struct RBLE_HGP_BHost_Data_Output_Param_t {
    uint8_t                 inst_idx;
    RBLE_HGP_REPORT_DESC    report;
} RBLE_HGP_BHOST_DATA_OUTPUT_PARAM;

typedef struct RBLE_HGP_RHost_Enable_Param_t {
    uint8_t                 con_type;
    uint8_t                 hids_inst_num;
    uint8_t                 bas_inst_num;
    RBLE_HIDS_CONTENT       hids[RBLE_HIDS_INST_MAX];
    RBLE_DIS11_CONTENT      dis;
    RBLE_BAS_CONTENT        bas[RBLE_BAS_INST_MAX];
} RBLE_HGP_RHost_ENABLE_PARAM;

typedef struct RBLE_HGP_RHost_Send_Report_Param_t {
    uint8_t                 inst_idx;
    RBLE_HGP_REPORT_DESC    report;
} RBLE_HGP_RHOST_SEND_REPORT_PARAM;

typedef struct RBLE_HGP_RHost_Data_Output_Param_t {
    uint8_t                 inst_idx;
    RBLE_HGP_REPORT_DESC    report;
} RBLE_HGP_RHOST_DATA_OUTPUT_PARAM;

/**************************** PUBLIC Memory Declaration *******************************/

/**************************** PROTOTYPE Declaration ***********************************/
#ifdef __cplusplus
extern "C" {
#endif
BOOL APP_Init( void );
void APP_Run( void );
void APP_Exit( void );
BOOL APP_Is_Idle( void );
void APP_Disconnect(void);
#ifdef USE_RSK_ADC
void APP_Send_Data(uint8_t val);
#else
void APP_Send_Data(void);
#endif
#ifdef __cplusplus
}
#endif
#endif  /* _rBLE_APP_H */
