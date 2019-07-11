/**
 ****************************************************************************************
 *
 * @file		rble_api_cb.c
 *
 * @brief	rBLE I/F API Call back processing
 *
 * Copyright(C) 2013-2014 Renesas Electronics Corporation
 *
 * $Rev: 2156 $
 *
 ****************************************************************************************
 */

#include	<string.h>
#include	<stdio.h>
#include	<stdlib.h>

#include	"rble_api.h"
#include	"rble_host.h"
#include	"rble_trans.h"

/**************************** Define Declaration **************************************/
extern uint8_t *access_buf;
/**************************** PUBLIC Memory Declaration *******************************/
/**************************** PRIVATE Memory Declaration ******************************/
static uint8_t	rBLE_Over_Packet_Temp[ 0x256 ];

/**************************** PRIVATE PROTOTYPE Declaration ***************************/
static RBLE_STATUS RBLE_GAP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt);
static RBLE_STATUS RBLE_SM_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt);
static RBLE_STATUS RBLE_GATT_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt);
#if (PRF_SEL_FMPL || PRF_SEL_FMPT)
static RBLE_STATUS RBLE_FMP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt);
#endif
#if (PRF_SEL_PXPM || PRF_SEL_PXPR)
static RBLE_STATUS RBLE_PXP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt);
#endif
#if (PRF_SEL_HTPC || PRF_SEL_HTPT)
static RBLE_STATUS RBLE_HTP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt);
#endif
#if (PRF_SEL_BLPC || PRF_SEL_BLPS)
static RBLE_STATUS RBLE_BLP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt);
#endif
#if (PRF_SEL_HGHD || PRF_SEL_HGBH || PRF_SEL_HGRH)
static RBLE_STATUS RBLE_HOGP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt);
#endif
#if (PRF_SEL_SPPS || PRF_SEL_SPPC)
static RBLE_STATUS RBLE_ScPP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt);
#endif
#if (PRF_SEL_HRPC || PRF_SEL_HRPS)
static RBLE_STATUS RBLE_HRP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt);
#endif
#if (PRF_SEL_CSCC || PRF_SEL_CSCS)
static RBLE_STATUS RBLE_CSCP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt);
#endif
#if (PRF_SEL_GLPC || PRF_SEL_GLPS)
static RBLE_STATUS RBLE_GLP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt);
#endif
#if (PRF_SEL_CPPS || PRF_SEL_CPPC)
static RBLE_STATUS RBLE_CPP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt);
#endif
#if (PRF_SEL_TIPS || PRF_SEL_TIPC)
static RBLE_STATUS RBLE_TIP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt);
#endif
#if (PRF_SEL_ANPS || PRF_SEL_ANPC)
static RBLE_STATUS RBLE_ANP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt);
#endif
#if (PRF_SEL_LNPS || PRF_SEL_LNPC)
static RBLE_STATUS RBLE_LNP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt);
#endif
#if (PRF_SEL_PASS || PRF_SEL_PASC)
static RBLE_STATUS RBLE_PASP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt);
#endif
#if (PRF_SEL_RSCS || PRF_SEL_RSCC)
static RBLE_STATUS RBLE_RSCP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt);
#endif
static RBLE_STATUS RBLE_VS_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt);

#ifdef RBLE_DEBUG
void rBLE_Data_Disp(RBLE_HEADER_t *pnt);
void rBLE_Change_Event_Code(RBLE_EVENT_NO	opcode);
void rBLE_Change_Command_Code(RBLE_COMMAND_NO	opcode);
#endif

/******************************* Program Area *********************************************/

/******************************************************************************************/
/**
 *  @brief EVENT Data Analysis
 *
 *  This function does Event Data Analysis@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Event Data Analysis
 *    -# Call Back API Call
 *
 *  @param[in]	*data_pnt		Data Address
 *  @param[in]	*data_pnt		Data Size
 *
 *  @retval		RBLE_OK			Success
 *  @retval		RBLE_ERR		Error
 */
/******************************************************************************************/
RBLE_STATUS RBLE_Event_Data_Analisys(uint8_t *data_pnt)
{
	RBLE_STATUS		ret;
	RBLE_HEADER_t	*header_pnt;
	uint16_t		event_group;
	RBLE_EVENT_NO	event_id;
	RBLE_SUBHEADER_t	*subh_pnt;

	if(NULL != data_pnt){
		/* Get Header Address */
		header_pnt = (RBLE_HEADER_t *)data_pnt;

		if( RBLE_API_OVER_PACKET_MASKBIT16 == (header_pnt->opcode & RBLE_API_OVER_PACKET_MASKBIT16) ){
			subh_pnt = (RBLE_SUBHEADER_t *)data_pnt;
			if( 0 == subh_pnt->packet_no ){
				
				RBLE_OVER_PACKET_info.r_head_pnt = (RBLE_HEADER_t *)&rBLE_Over_Packet_Temp[ 0 ];
				
				RBLE_OVER_PACKET_info.r_head_pnt->packet_type		= subh_pnt->packet_type;
				RBLE_OVER_PACKET_info.r_head_pnt->opcode			= subh_pnt->opcode & (~RBLE_API_OVER_PACKET_MASKBIT16);
				memcpy( RBLE_OVER_PACKET_info.r_head_pnt->data, subh_pnt->data, (subh_pnt->packet_length - RBLE_SUBHEADER_SIZE) );
				RBLE_OVER_PACKET_info.r_data_cnt					= subh_pnt->packet_length - RBLE_SUBHEADER_SIZE;
				return RBLE_OK;
			}
			else{
				memcpy( ( RBLE_OVER_PACKET_info.r_head_pnt->data + RBLE_OVER_PACKET_info.r_data_cnt ), &subh_pnt->data[0], (subh_pnt->packet_length - RBLE_SUBHEADER_SIZE) );
				RBLE_OVER_PACKET_info.r_data_cnt		+= (subh_pnt->packet_length - RBLE_SUBHEADER_SIZE);
				
				if( 1 == subh_pnt->packet_end ){
					header_pnt	= RBLE_OVER_PACKET_info.r_head_pnt;
				}else{
					return RBLE_OK;
				}
			}
		}

		/* Get Opcode */
		event_group	= header_pnt->opcode & 0xFF00;
		event_id	= (RBLE_EVENT_NO)header_pnt->opcode;

		/* Event Data Analysis */
		switch(event_group){
			case RBLE_CMD_GAP_TOP:
				ret = RBLE_GAP_Command_Result(event_id,header_pnt->data);
				break;
			case RBLE_CMD_SM_TOP:
				ret = RBLE_SM_Command_Result(event_id,header_pnt->data);
				break;
			case RBLE_CMD_GATT_TOP:
				ret = RBLE_GATT_Command_Result(event_id,header_pnt->data);
				break;
			#if (PRF_SEL_FMPL || PRF_SEL_FMPT)
			case RBLE_CMD_FMP_TARGET_TOP:
				ret = RBLE_FMP_Command_Result(event_id,header_pnt->data);
				break;	
			#endif
			#if (PRF_SEL_PXPM || PRF_SEL_PXPR)
			case RBLE_CMD_PXP_REPORTER_TOP:
				ret = RBLE_PXP_Command_Result(event_id,header_pnt->data);
				break;
			#endif
			#if (PRF_SEL_HTPC || PRF_SEL_HTPT)
			case RBLE_CMD_HTP_THERMOMETER_TOP:
				ret = RBLE_HTP_Command_Result(event_id,header_pnt->data);
				break;
			#endif
			#if (PRF_SEL_BLPC || PRF_SEL_BLPS)
			case RBLE_CMD_BLP_SENSOR_TOP:
				ret = RBLE_BLP_Command_Result(event_id,header_pnt->data);
				break;
			#endif
			#if (PRF_SEL_HGHD || PRF_SEL_HGBH || PRF_SEL_HGRH)
			case RBLE_CMD_HGP_HDEVICE_TOP:
				ret = RBLE_HOGP_Command_Result(event_id,header_pnt->data);
				break;
			#endif
			#if (PRF_SEL_SPPS || PRF_SEL_SPPC)
			case RBLE_CMD_SPP_SERVER_TOP:
				ret = RBLE_ScPP_Command_Result(event_id,header_pnt->data);
				break;
			#endif
			#if (PRF_SEL_HRPC || PRF_SEL_HRPS)
			case RBLE_CMD_HRP_SENSOR_TOP:
				ret = RBLE_HRP_Command_Result(event_id,header_pnt->data);
				break;
			#endif
			#if (PRF_SEL_CSCC || PRF_SEL_CSCS)
			case RBLE_CMD_CSCP_SENSOR_TOP:
				ret = RBLE_CSCP_Command_Result(event_id,header_pnt->data);
				break;
			#endif
			#if (PRF_SEL_GLPC || PRF_SEL_GLPS)
			case RBLE_CMD_GLP_SENSOR_TOP:
				ret = RBLE_GLP_Command_Result(event_id,header_pnt->data);
				break;
			#endif
			#if (PRF_SEL_CPPS || PRF_SEL_CPPC)
			case RBLE_CMD_CPP_SENSOR_TOP:
				ret = RBLE_CPP_Command_Result(event_id,header_pnt->data);
				break;
			#endif
			#if (PRF_SEL_TIPS || PRF_SEL_TIPC)
			case RBLE_CMD_TIP_SERVER_TOP:
				ret = RBLE_TIP_Command_Result(event_id,header_pnt->data);
				break;
			#endif
			#if (PRF_SEL_ANPS || PRF_SEL_ANPC)
			case RBLE_CMD_ANP_SERVER_TOP:
				ret = RBLE_ANP_Command_Result(event_id,header_pnt->data);
				break;
			#endif
			#if (PRF_SEL_LNPS || PRF_SEL_LNPC)
			case RBLE_CMD_LNP_SENSOR_TOP:
				ret = RBLE_LNP_Command_Result(event_id,header_pnt->data);
				break;
			#endif
			#if (PRF_SEL_PASS || PRF_SEL_PASC)
			case RBLE_CMD_PASP_SERVER_TOP:
				ret = RBLE_PASP_Command_Result(event_id,header_pnt->data);
				break;
			#endif
			#if (PRF_SEL_RSCS || PRF_SEL_RSCC)
			case RBLE_CMD_RSCP_SENSOR_TOP:
				ret = RBLE_RSCP_Command_Result(event_id,header_pnt->data);
				break;
			#endif
			case RBLE_CMD_VS_TOP:
				ret = RBLE_VS_Command_Result(event_id,header_pnt->data);
				break;
			default:
				ret = RBLE_ERR;
				break;
		}
		
		if( header_pnt == RBLE_OVER_PACKET_info.r_head_pnt ){
			RBLE_OVER_PACKET_info.r_head_pnt = NULL;
		}
	}else{
		ret = RBLE_ERR;
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Generic Access Profile Commnad Result Notification
 *
 *  This function does DeviceManager Command Result Notification to APP@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Event Data
 *    -# Call Back API Call
 *
 *  @param[in]	evt_id		EVENT ID
 *  @param[in]	data_pnt	Data Address
 *
 *  @retval
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt)
{
	RBLE_STATUS		ret = RBLE_OK;
	RBLE_GAP_EVENT	RBLE_GAP_Event;
	uint8_t			*Data_p;

	if( NULL != RBLE_GAP_Info.Handler){
		Data_p = data_pnt;
		switch(evt_id){
			case RBLE_EVT_GAP_RESET_RESULT:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_RESET_RESULT;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.reset_result.status );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.reset_result.rBLE_major_ver );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.reset_result.rBLE_minor_ver );
				rBLE_Profile_Init();
				break;
			case RBLE_EVT_GAP_SET_NAME_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_SET_NAME_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.status );
				break;
			case RBLE_EVT_GAP_OBSERVATION_ENABLE_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_OBSERVATION_ENABLE_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.status );
				break;
			case RBLE_EVT_GAP_OBSERVATION_DISABLE_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_OBSERVATION_DISABLE_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.status );
				break;
			case RBLE_EVT_GAP_BROADCAST_ENABLE_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_BROADCAST_ENABLE_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.status );
				break;
			case RBLE_EVT_GAP_BROADCAST_DISABLE_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_BROADCAST_DISABLE_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.status );
				break;
			case RBLE_EVT_GAP_SET_BONDING_MODE_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_SET_BONDING_MODE_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.status );
				break;
			case RBLE_EVT_GAP_SET_SECURITY_REQUEST_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_SET_SECURITY_REQUEST_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.set_sec_req.status );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.set_sec_req.sec );
				break;
			case RBLE_EVT_GAP_GET_DEVICE_INFO_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_GET_DEVICE_INFO_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.get_dev_ver.status );
				memcpy(&RBLE_GAP_Event.param.get_dev_ver.addr, Data_p, ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) );
				Data_p += ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN );
				RBLE_DUMMY_GET8( Data_p );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.get_dev_ver.ver_info.hci_ver );		/* HCI Version */
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.get_dev_ver.ver_info.lmp_ver );		/* LMP Version */
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.get_dev_ver.ver_info.host_ver );		/* Host Version */
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.get_dev_ver.ver_info.reserved );		/* reserved */
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.get_dev_ver.ver_info.hci_subver );	/* HCI Sub Versiong */
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.get_dev_ver.ver_info.lmp_subver );	/* LMP Version */
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.get_dev_ver.ver_info.host_subver );	/* Host Sub Versiong */
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.get_dev_ver.ver_info.company_id );	/* Company Id */

				break;
			case RBLE_EVT_GAP_GET_WHITE_LIST_SIZE_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_GET_WHITE_LIST_SIZE_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.get_wlst_size.status );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.get_wlst_size.wlist_size );
				break;
			case RBLE_EVT_GAP_ADD_TO_WHITE_LIST_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_ADD_TO_WHITE_LIST_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.status );
				break;
			case RBLE_EVT_GAP_DEL_FROM_WHITE_LIST_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_DEL_FROM_WHITE_LIST_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.status );
				break;
			case RBLE_EVT_GAP_GET_REMOTE_DEVICE_NAME_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_GET_REMOTE_DEVICE_NAME_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.get_remote_dev_name.status );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.get_remote_dev_name.bd_name.namelen );
				memcpy(&RBLE_GAP_Event.param.get_remote_dev_name.bd_name.name[ 0 ], Data_p, RBLE_BD_NAME_SIZE);
				break;
			case RBLE_EVT_GAP_GET_REMOTE_DEVICE_INFO_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_GET_REMOTE_DEVICE_INFO_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.get_remote_dev_info.status );
				RBLE_DUMMY_GET8( Data_p );
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.get_remote_dev_info.conhdl );
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.get_remote_dev_info.vers );
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.get_remote_dev_info.compid );
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.get_remote_dev_info.subvers );
				memcpy( &RBLE_GAP_Event.param.get_remote_dev_info.feats_used, Data_p, ( sizeof( uint8_t ) * RBLE_LE_FEATS_LEN ) );
				break;
			case RBLE_EVT_GAP_DEVICE_SEARCH_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_DEVICE_SEARCH_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.status );
				break;
			case RBLE_EVT_GAP_DEVICE_SEARCH_RESULT_IND:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_DEVICE_SEARCH_RESULT_IND;
				/* Advertising Report */
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.dev_search_result.adv_resp.evt_type );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.dev_search_result.adv_resp.adv_addr_type );
				memcpy(&RBLE_GAP_Event.param.dev_search_result.adv_resp.adv_addr.addr, Data_p, ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) );
				Data_p += ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) ;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.dev_search_result.adv_resp.data_len );
				memcpy( &RBLE_GAP_Event.param.dev_search_result.adv_resp.data, Data_p, ( sizeof( uint8_t ) * RBLE_ADV_DATA_LEN ) );
				Data_p += ( sizeof( uint8_t ) * RBLE_ADV_DATA_LEN );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.dev_search_result.adv_resp.rssi );
				break;
			case RBLE_EVT_GAP_RPA_RESOLVED:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_RPA_RESOLVED;
				memcpy( &RBLE_GAP_Event.param.rpa_resolved.res_addr.addr, Data_p, ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) );
				Data_p += ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.rpa_resolved.res_addr_type );
				memcpy( &RBLE_GAP_Event.param.rpa_resolved.addr.addr, Data_p, ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) );
				Data_p += ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.rpa_resolved.addr_type );
				break;
			case RBLE_EVT_GAP_SET_RANDOM_ADDRESS_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_SET_RANDOM_ADDRESS_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.set_rand_adr.status );
				memcpy( &RBLE_GAP_Event.param.set_rand_adr.addr, Data_p, ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) );
				break;
			case RBLE_EVT_GAP_SET_PRIVACY_FEATURE_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_SET_PRIVACY_FEATURE_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.status );
				break;
			case RBLE_EVT_GAP_CONNECTION_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_CONNECTION_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.conn_comp.connect_info.status );				/*  Confirmation status */
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.conn_comp.connect_info.role );					/* Role */
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.conn_comp.connect_info.conhdl );				/* Connection handle */
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.conn_comp.connect_info.peer_addr_type );			/* Peer address type */
				memcpy( &RBLE_GAP_Event.param.conn_comp.connect_info.peer_addr, Data_p, ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) );			/* Peer BT address */
				Data_p += ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.conn_comp.connect_info.idx );					/* Connection index */
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.conn_comp.connect_info.con_interval );		/* Connection interval */
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.conn_comp.connect_info.con_latency );			/* Connection latency */
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.conn_comp.connect_info.sup_to );				/* Link supervision timeout */
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.conn_comp.connect_info.clk_accuracy );			/* Clock accuracy */
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.conn_comp.connect_info. reserved3 );
				break;
			case RBLE_EVT_GAP_CONNECTION_CANCEL_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_CONNECTION_CANCEL_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.status );
				break;
			case RBLE_EVT_GAP_DISCONNECT_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_DISCONNECT_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.disconnect.reason );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.disconnect.status );
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.disconnect.conhdl );
				break;
			case RBLE_EVT_GAP_ADVERTISING_REPORT_IND:
   				RBLE_GAP_Event.type = RBLE_GAP_EVENT_ADVERTISING_REPORT_IND;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.adv_report.evt.adv_rep.evt_type );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.adv_report.evt.adv_rep.adv_addr_type );
				memcpy( &RBLE_GAP_Event.param.adv_report.evt.adv_rep.adv_addr, Data_p, ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) );
				Data_p += ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.adv_report.evt.adv_rep.data_len );
				memcpy( &RBLE_GAP_Event.param.adv_report.evt.adv_rep.data, Data_p, ( sizeof( uint8_t ) * RBLE_ADV_DATA_LEN ) );
				Data_p += ( sizeof( uint8_t ) * RBLE_ADV_DATA_LEN );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.adv_report.evt.adv_rep.rssi );
				break;
			case RBLE_EVT_GAP_BONDING_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_BONDING_COMP;
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.bonding_comp.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.bonding_comp.idx );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.bonding_comp.status );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.bonding_comp.key_size );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.bonding_comp.sec_prop );
				break;
			case RBLE_EVT_GAP_BONDING_REQ_IND:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_BONDING_REQ_IND;
				memcpy( &RBLE_GAP_Event.param.bonding_req.bd_addr, Data_p, ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) );
				Data_p += ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.bonding_req.index );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.bonding_req.auth_req );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.bonding_req.io_cap );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.bonding_req.oob_data_flg );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.bonding_req.max_enc_size );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.bonding_req.ikey_dist );
 				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.bonding_req.rkey_dist );
				break;
			case RBLE_EVT_GAP_CHANGE_CONNECTION_PARAM_REQ_IND:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_CHANGE_CONNECTION_PARAM_REQ_IND;
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.chg_connect_param_req.conhdl );
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.chg_connect_param_req.conn_param.intv_min );
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.chg_connect_param_req.conn_param.intv_max );
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.chg_connect_param_req.conn_param.latency );
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.chg_connect_param_req.conn_param.time_out );
				break;
			case RBLE_EVT_GAP_CHANGE_CONNECTION_PARAM_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_CHANGE_CONNECTION_PARAM_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.chg_connect_param.status );
				RBLE_DUMMY_GET8( Data_p );
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.chg_connect_param.con_interval );
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.chg_connect_param.con_latency );
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.chg_connect_param.sup_to );
				break;
			case RBLE_EVT_GAP_CHANGE_CONNECTION_PARAM_RESPONSE:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_CHANGE_CONNECTION_PARAM_RESPONSE;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.chg_connect_param_resp.status );
				RBLE_DUMMY_GET8( Data_p );
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.chg_connect_param_resp.result );
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.chg_connect_param_resp.conhdl );
				break;
			case RBLE_EVT_GAP_CHANNEL_MAP_REQ_CMP_EVT:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_CHANNEL_MAP_REQ_COMP;
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.channel_map_req_cmp.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.channel_map_req_cmp.status );
				memcpy(&RBLE_GAP_Event.param.channel_map_req_cmp.chmap, Data_p, ( sizeof( uint8_t ) * RBLE_LE_CHNL_MAP_LEN ) );
				break;
			case RBLE_EVT_GAP_READ_RSSI_COMP:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_READ_RSSI_COMP;
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.read_rssi.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.read_rssi.status );
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.read_rssi.rssi );
				break;

			case RBLE_EVT_GAP_WR_CHAR_IND:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_WR_CHAR_IND;
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.wr_char.conhdl );
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.wr_char.type );
				if (RBLE_GAP_WR_CHAR_NAME == RBLE_GAP_Event.param.wr_char.type) {
					RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.wr_char.param.name.namelen );
					memset(&RBLE_GAP_Event.param.wr_char.param.name.name[ 0 ], 0, RBLE_BD_NAME_SIZE);
					memcpy(&RBLE_GAP_Event.param.wr_char.param.name.name[ 0 ], Data_p,
						   RBLE_GAP_Event.param.wr_char.param.name.namelen);
				} else {
					RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.wr_char.param.appearance );
				}
				break;
			case RBLE_EVT_GAP_CMD_DISALLOWED_IND:
				RBLE_GAP_Event.type = RBLE_GAP_EVENT_COMMAND_DISALLOWED_IND;
				RBLE_DATA_GET8( Data_p, RBLE_GAP_Event.param.cmd_disallowed_ind.status );
				RBLE_DUMMY_GET8( Data_p );
				RBLE_DATA_GET16( Data_p, RBLE_GAP_Event.param.cmd_disallowed_ind.opcode );
				break;
			default:
				ret = RBLE_ERR;
				break;
		}

		if(RBLE_OK == ret){
			/* Call Back */
			RBLE_GAP_Info.Handler(&RBLE_GAP_Event);
		}
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Security Manager Commnad Result Notification
 *
 *  This function does Security Command Result Notification to APP@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Event Data
 *    -# Call Back API Call
 *
 *  @param[in]	evt_id		EVENT ID
 *  @param[in]	data_pnt	Data Address
 *
 *  @retval
 */
/******************************************************************************************/
RBLE_STATUS RBLE_SM_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt)
{
	RBLE_STATUS		ret = RBLE_OK;
	RBLE_SM_EVENT	RBLE_SM_Event;
	uint8_t			*Data_p;

	if( NULL != RBLE_SM_Info.Handler){
		Data_p = data_pnt;
		switch(evt_id){
			case RBLE_EVT_SM_SET_KEY_CNF:
				RBLE_SM_Event.type = RBLE_SM_EVENT_SET_CNF;
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.set_conf.status );
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.set_conf.key_code );
				break;
			case RBLE_EVT_SM_SEC_START_IND:
				RBLE_SM_Event.type = RBLE_SM_ENC_START_IND;
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.sec_start.idx );
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.sec_start.status );
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.sec_start.key_size );
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.sec_start.sec_prop );
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.sec_start.bonded );
				break;
			case RBLE_EVT_SM_TK_REQ_IND:
				RBLE_SM_Event.type = RBLE_SM_TK_REQ_IND;
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.tk_req.idx );
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.tk_req.oob_en );
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.tk_req.disp_en );
				break;
			case RBLE_EVT_SM_LTK_REQ_IND:
				RBLE_SM_Event.type = RBLE_SM_LTK_REQ_IND;
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.ltk_req.idx );
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.ltk_req.auth_req );
				break;
			case RBLE_EVT_SM_LTK_REQ_FOR_ENC_IND:
				RBLE_SM_Event.type = RBLE_SM_LTK_REQ_FOR_ENC_IND;
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.ltk_req_for_enc.idx );
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.ltk_req_for_enc.auth_req );
				RBLE_DATA_GET16( Data_p, RBLE_SM_Event.param.ltk_req_for_enc.ediv );
				memcpy( &RBLE_SM_Event.param.ltk_req_for_enc.nb.nb, Data_p, ( sizeof( uint8_t ) * RBLE_RAND_NB_LEN ) );
				break;
			case RBLE_EVT_SM_IRK_REQ_IND:
				RBLE_SM_Event.type = RBLE_SM_IRK_REQ_IND;
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.irk_req.idx	);
				break;
			case RBLE_EVT_SM_CSRK_REQ_IND:
				RBLE_SM_Event.type = RBLE_SM_CSRK_REQ_IND;
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.csrk_req.idx );
				memcpy( &RBLE_SM_Event.param.csrk_req.addr.addr, Data_p, ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) );
				Data_p += ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN );
				RBLE_DUMMY_GET8( Data_p );
				RBLE_DATA_GET32( Data_p, RBLE_SM_Event.param.csrk_req.signcnt );
				break;
			case RBLE_EVT_SM_KEY_IND:
				RBLE_SM_Event.type = RBLE_SM_KEY_IND;
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.key_ind.idx	);
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.key_ind.key_code );
				RBLE_DATA_GET16( Data_p, RBLE_SM_Event.param.key_ind.ediv );
				memcpy( &RBLE_SM_Event.param.key_ind.nb.nb, Data_p, ( sizeof( uint8_t ) * RBLE_RAND_NB_LEN ) );
				Data_p += ( sizeof( uint8_t ) * RBLE_RAND_NB_LEN );
				memcpy( &RBLE_SM_Event.param.key_ind.ltk.key, Data_p, ( sizeof( uint8_t ) * RBLE_KEY_LEN ) );
				break;
			case RBLE_EVT_SM_CHK_BD_ADDR_REQ_IND:
				RBLE_SM_Event.type = RBLE_SM_CHK_BD_ADDR_REQ;
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.chk_bdaddr.idx );
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.chk_bdaddr.type	);
				memcpy(&RBLE_SM_Event.param.chk_bdaddr.addr.addr, Data_p, ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) );
				break;
			case RBLE_EVT_SM_TIMEOUT_EVT:
				RBLE_SM_Event.type = RBLE_SM_TIMEOUT_EVT;
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.timeout_evt.idx );
				break;
			case RBLE_EVT_SM_CMD_DISALLOWED_IND:
				RBLE_SM_Event.type = RBLE_SM_EVENT_COMMAND_DISALLOWED_IND;
				RBLE_DATA_GET8( Data_p, RBLE_SM_Event.param.cmd_disallowed_ind.status );
				RBLE_DUMMY_GET8( Data_p );
				RBLE_DATA_GET16( Data_p, RBLE_SM_Event.param.cmd_disallowed_ind.opcode );
				break;
			default:
				ret = RBLE_ERR;
				break;
		}

		if(RBLE_OK == ret){
			/* Call Back */
			RBLE_SM_Info.Handler(&RBLE_SM_Event);
		}
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Generic Access Profile Commnad Result Notification
 *
 *  This function does DeviceManager Command Result Notification to APP@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Assemble event structure
 *    -# Create Event Data
 *    -# Call Back API Call
 *
 *  @param[in]	evt_id		EVENT ID
 *  @param[in]	data_pnt	Data Address
 *
 *  @retval
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GATT_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt)
{
	RBLE_STATUS			ret = RBLE_OK;
	RBLE_GATT_EVENT		RBLE_GATT_Event;
	uint16_t			i;
	uint8_t				*Data_p;

	if (NULL != RBLE_GATT_Info.Handler) {
		Data_p = data_pnt;
		switch (evt_id) {
			case RBLE_EVT_GATT_DISC_SVC_ALL_CMP:			/* Discovery all services complete event */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_DISC_SVC_ALL_CMP;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_svc_all_cmp.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_svc_all_cmp.att_code );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_svc_all_cmp.nb_resp );
				for (i = 0; i < RBLE_GATT_MAX_HDL_LIST; i++) {
					RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_svc_all_cmp.list[ i ].start_hdl );
					RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_svc_all_cmp.list[ i ].end_hdl );
					RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_svc_all_cmp.list[ i ].attr_hdl );
				}
				break;
			case RBLE_EVT_GATT_DISC_SVC_ALL_128_CMP:		/* Discovery all 128bit UUID services complete event */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_DISC_SVC_ALL_128_CMP;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_svc_all_128_cmp.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_svc_all_128_cmp.att_code );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_svc_all_128_cmp.nb_resp );
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_svc_all_128_cmp.list.start_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_svc_all_128_cmp.list.end_hdl );
				memcpy( RBLE_GATT_Event.param.disc_svc_all_128_cmp.list.attr_hdl, Data_p, RBLE_GATT_128BIT_UUID_OCTET);
				break;
			case RBLE_EVT_GATT_DISC_SVC_BY_UUID_CMP:		/* Discovery service by UUID complete event */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_DISC_SVC_BY_UUID_CMP;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_svc_by_uuid_cmp.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_svc_by_uuid_cmp.att_code );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_svc_by_uuid_cmp.nb_resp );
				for (i = 0; i < RBLE_GATT_MAX_HDL_LIST; i++) {
					RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_svc_by_uuid_cmp.list[ i ].start_hdl );
					RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_svc_by_uuid_cmp.list[ i ].end_hdl );
				}
				break;
			case RBLE_EVT_GATT_DISC_SVC_INCL_CMP:			/* Discovery included services complete event */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_DISC_SVC_INCL_CMP;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_svc_incl_cmp.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_svc_incl_cmp.nb_entry );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_svc_incl_cmp.entry_len );
				if ( RBLE_GATT_16BIT_UUID_OCTET == RBLE_GATT_Event.param.disc_svc_incl_cmp.entry_len ) {
					for (i = 0; i < RBLE_GATT_Event.param.disc_svc_incl_cmp.nb_entry; i++) {
						RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_svc_incl_cmp.incl_list.list[ i ].attr_hdl );
						RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_svc_incl_cmp.incl_list.list[ i ].start_hdl );
						RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_svc_incl_cmp.incl_list.list[ i ].end_hdl );
						RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_svc_incl_cmp.incl_list.list[ i ].uuid );
					}
				} else {
					RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_svc_incl_cmp.incl_list.incl.attr_hdl );
					RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_svc_incl_cmp.incl_list.incl.start_hdl );
					RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_svc_incl_cmp.incl_list.incl.end_hdl );
					memcpy( RBLE_GATT_Event.param.disc_svc_incl_cmp.incl_list.incl.uuid, Data_p, RBLE_GATT_128BIT_UUID_OCTET);
				}
				break;
			case RBLE_EVT_GATT_DISC_CHAR_ALL_CMP:			/* Discovery all characteristics complete event */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_DISC_CHAR_ALL_CMP;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_char_all_cmp.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_char_all_cmp.att_code );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_char_all_cmp.nb_entry );
				for (i = 0; i < RBLE_GATT_MAX_HDL_LIST; i++) {
					RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_char_all_cmp.list[ i ].attr_hdl );
					RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_char_all_cmp.list[ i ].prop );
					Data_p++;	/* skip to pad */
					RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_char_all_cmp.list[ i ].pointer_hdl );
					RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_char_all_cmp.list[ i ].uuid );
				}
				break;
			case RBLE_EVT_GATT_DISC_CHAR_ALL_128_CMP:		/* Discovery all 128bit UUID characteristics complete event */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_DISC_CHAR_ALL_128_CMP;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_char_all_128_cmp.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_char_all_128_cmp.att_code );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_char_all_128_cmp.nb_entry );
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_char_all_128_cmp.list.attr_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_char_all_128_cmp.list.prop );
				Data_p++;	/* skip to pad */
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_char_all_128_cmp.list.pointer_hdl );
				memcpy(RBLE_GATT_Event.param.disc_char_all_128_cmp.list.uuid, Data_p, RBLE_GATT_128BIT_UUID_OCTET);
				break;
			case RBLE_EVT_GATT_DISC_CHAR_BY_UUID_CMP:		/* Discovery characteristic by UUID complete event */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_DISC_CHAR_BY_UUID_CMP;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_char_by_uuid_cmp.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_char_by_uuid_cmp.att_code );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_char_by_uuid_cmp.nb_entry );
				for (i = 0; i < RBLE_GATT_MAX_HDL_LIST; i++) {
					RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_char_by_uuid_cmp.list[ i ].attr_hdl );
					RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_char_by_uuid_cmp.list[ i ].prop );
					Data_p++;	/* skip to pad */
					RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_char_by_uuid_cmp.list[ i ].pointer_hdl );
					RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_char_by_uuid_cmp.list[ i ].uuid );
				}
				break;
			case RBLE_EVT_GATT_DISC_CHAR_BY_UUID_128_CMP:	/* Discovery characteristic by 128bit UUID complete event */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_DISC_CHAR_BY_UUID_128_CMP;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_char_by_uuid_128_cmp.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_char_by_uuid_128_cmp.att_code );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_char_by_uuid_128_cmp.nb_entry );
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_char_by_uuid_128_cmp.list.attr_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_char_by_uuid_128_cmp.list.prop );
				Data_p++;	/* skip to pad */
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_char_by_uuid_128_cmp.list.pointer_hdl );
				memcpy(RBLE_GATT_Event.param.disc_char_by_uuid_128_cmp.list.uuid, Data_p, RBLE_GATT_128BIT_UUID_OCTET);
				break;
			case RBLE_EVT_GATT_DISC_CHAR_DESC_CMP:			/* Discovery characteristic descriptor complete event */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_DISC_CHAR_DESC_CMP;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_char_desc_cmp.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_char_desc_cmp.nb_entry );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_char_desc_cmp.reserved );
				for (i = 0; i < RBLE_GATT_MAX_HDL_LIST; i++) {
					RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_char_desc_cmp.list[ i ].attr_hdl );
					RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_char_desc_cmp.list[ i ].desc_hdl );
				}
				break;
			case RBLE_EVT_GATT_DISC_CHAR_DESC_128_CMP:		/* Discovery 128bit characteristic descriptor complete event */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_DISC_CHAR_DESC_128_CMP;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_char_desc_128_cmp.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_char_desc_128_cmp.nb_entry );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.disc_char_desc_128_cmp.reserved );
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.disc_char_desc_128_cmp.list_128.attr_hdl );
				memcpy(RBLE_GATT_Event.param.disc_char_desc_128_cmp.list_128.uuid, Data_p, RBLE_GATT_128BIT_UUID_OCTET);
				break;
			case RBLE_EVT_GATT_READ_CHAR_RESP:				/* Read characteristic response */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_READ_CHAR_RESP;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.read_char_resp.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.read_char_resp.att_code );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.read_char_resp.data.each_len );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.read_char_resp.data.len );
				memcpy(RBLE_GATT_Event.param.read_char_resp.data.data, Data_p, RBLE_GATT_MAX_VALUE);
				break;
			case RBLE_EVT_GATT_READ_CHAR_LONG_RESP:			/* Read characteristic long response */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_READ_CHAR_LONG_RESP;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.read_char_long_resp.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.read_char_long_resp.att_code );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.read_char_long_resp.val_len );
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.read_char_long_resp.attr_hdl );
				memcpy(RBLE_GATT_Event.param.read_char_long_resp.value, Data_p, RBLE_GATT_MAX_VALUE);
				break;
			case RBLE_EVT_GATT_READ_CHAR_MULT_RESP:			/* Read characteristic multiple response */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_READ_CHAR_MULT_RESP;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.read_char_mult_resp.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.read_char_mult_resp.att_code );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.read_char_mult_resp.val_len );
				for (i = 0; i < RBLE_GATT_MAX_NB_HDLS; i++) {
					RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.read_char_mult_resp.data[ i ].len );
					memcpy(RBLE_GATT_Event.param.read_char_mult_resp.data[ i ].value, Data_p, RBLE_GATT_MAX_VALUE);
					Data_p+=RBLE_GATT_MAX_VALUE;
				}
				break;
			case RBLE_EVT_GATT_READ_CHAR_LONG_DESC_RESP:	/* Read characteristic long descriptor response */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_READ_CHAR_LONG_DESC_RESP;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.read_char_long_desc_resp.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.read_char_long_desc_resp.att_code );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.read_char_long_desc_resp.val_len );
				memcpy(RBLE_GATT_Event.param.read_char_long_desc_resp.value, Data_p, RBLE_GATT_MAX_VALUE);
				Data_p+=RBLE_GATT_MAX_VALUE;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.read_char_long_desc_resp.attr_hdl );
				break;
			case RBLE_EVT_GATT_WRITE_CHAR_RESP:				/* Write characteristic response */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_WRITE_CHAR_RESP;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.write_char_resp.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.write_char_resp.att_code );
				break;
			case RBLE_EVT_GATT_WRITE_CHAR_RELIABLE_RESP:	/* Write characteristic reliable response */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_WRITE_CHAR_RELIABLE_RESP;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.write_reliable_resp.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.write_reliable_resp.att_code );
				break;
			case RBLE_EVT_GATT_CANCEL_WRITE_CHAR_RESP:		/* Cancel write characteristic response */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_CANCEL_WRITE_CHAR_RESP;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.cancel_write_resp.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.cancel_write_resp.att_code );
				break;
			case RBLE_EVT_GATT_HANDLE_VALUE_NOTIF:			/* Handle Value notification */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_HANDLE_VALUE_NOTIF;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.handle_value_notif.conhdl );
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.handle_value_notif.charhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.handle_value_notif.size );
				memcpy(RBLE_GATT_Event.param.handle_value_notif.value, Data_p, RBLE_GATT_MAX_VALUE);
				break;
			case RBLE_EVT_GATT_HANDLE_VALUE_IND:			/* Handle Value indication */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_HANDLE_VALUE_IND;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.handle_value_ind.conhdl );
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.handle_value_ind.charhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.handle_value_ind.size );
				memcpy(RBLE_GATT_Event.param.handle_value_ind.value, Data_p, RBLE_GATT_MAX_VALUE);
				break;
			case RBLE_EVT_GATT_HANDLE_VALUE_CFM:			/* Confirmation response */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_HANDLE_VALUE_CFM;
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.handle_value_cfm.status );
				break;
			case RBLE_EVT_GATT_DISCOVERY_CMP:				/* Discovery complete event */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_DISCOVERY_CMP;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.discovery_cmp.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.discovery_cmp.att_code );
				break;
			case RBLE_EVT_GATT_COMPLETE:					/* Generic complete event */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_COMPLETE;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.complete.conhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.complete.att_code );
				break;
			case RBLE_EVT_GATT_WRITE_CMD_IND:				/* Write command indication */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_WRITE_CMD_IND;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.write_cmd_ind.conhdl );
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.write_cmd_ind.elmt );
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.write_cmd_ind.size );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.write_cmd_ind.offset );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.write_cmd_ind.resp );
				memcpy(RBLE_GATT_Event.param.write_cmd_ind.value, Data_p, RBLE_GATT_MAX_VALUE);
				break;
			case RBLE_EVT_GATT_RESP_TIMEOUT:				/* Response timeout notification */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_RESP_TIMEOUT;
				/* No Parameters */
				break;
			case RBLE_EVT_GATT_SET_PERM_CMP:				/* Set Permission complete event */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_SET_PERM_CMP;
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.set_perm_cmp.status );
				break;
			case RBLE_EVT_GATT_SET_DATA_CMP:				/* Set Data complete event */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_SET_DATA_CMP;
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.set_data_cmp.status );
				break;
			case RBLE_EVT_GATT_NOTIFY_CMP:					/* Notification complete event */
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_NOTIFY_COMP;
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.notify_cmp.conhdl );
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.notify_cmp.charhdl );
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.notify_cmp.status );
				break;
			case RBLE_EVT_GATT_CMD_DISALLOWED_IND:
				RBLE_GATT_Event.type = RBLE_GATT_EVENT_COMMAND_DISALLOWED_IND;
				RBLE_DATA_GET8( Data_p, RBLE_GATT_Event.param.cmd_disallowed_ind.status );
				RBLE_DUMMY_GET8( Data_p );
				RBLE_DATA_GET16( Data_p, RBLE_GATT_Event.param.cmd_disallowed_ind.opcode );
				break;
			default:
				ret = RBLE_ERR;
				break;
		}

		if(RBLE_OK == ret){
			/* Call Back */
			RBLE_GATT_Info.Handler(&RBLE_GATT_Event);
		}
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Find Me Profile Commnad Result Notification
 *
 *  This function does Find Me Profile Result Notification to APP@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Event Data
 *    -# Call Back API Call
 *
 *  @param[in]	evt_id		EVENT ID
 *  @param[in]	data_pnt	Data Address
 *
 *  @retval
 */
/******************************************************************************************/
#if (PRF_SEL_FMPL || PRF_SEL_FMPT)
static RBLE_STATUS RBLE_FMP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt)
{
	RBLE_STATUS ret = RBLE_OK;
	RBLE_FMPL_EVENT	RBLE_FMPL_Event;
	RBLE_FMPT_EVENT	RBLE_FMPT_Event;
	uint8_t			*Data_p;

	Data_p = data_pnt;

	switch(evt_id){
		case RBLE_EVT_FMP_TARGET_ENABLE_COMP:
			RBLE_FMPT_Event.type         = RBLE_FMP_EVENT_TARGET_ENABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_FMPT_Event.param.target_enable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_FMPT_Event.param.target_enable.conhdl );
			/* Call Back */
			if( NULL != RBLE_FMPT_Info.Fmt_Handler){
				RBLE_FMPT_Info.Fmt_Handler(&RBLE_FMPT_Event);
			}
			break;
		case RBLE_EVT_FMP_TARGET_DISABLE_COMP:
			RBLE_FMPT_Event.type         = RBLE_FMP_EVENT_TARGET_DISABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_FMPT_Event.param.target_disable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_FMPT_Event.param.target_disable.conhdl );
			/* Call Back */
			if( NULL != RBLE_FMPT_Info.Fmt_Handler){
				RBLE_FMPT_Info.Fmt_Handler(&RBLE_FMPT_Event);
			}
			/* Handler Clear */
			RBLE_FMPT_Info.conhdl = 0;
			RBLE_FMPT_Info.Fmt_Handler = NULL;
			break;
		case RBLE_EVT_FMP_TARGET_ALERT_IND:
			RBLE_FMPT_Event.type = RBLE_FMP_EVENT_TARGET_ALERT_IND;
			RBLE_DATA_GET16( Data_p, RBLE_FMPT_Event.param.target_alert_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_FMPT_Event.param.target_alert_ind.alert_lvl );

			/* Call Back */
			if( NULL != RBLE_FMPT_Info.Fmt_Handler){
				RBLE_FMPT_Info.Fmt_Handler(&RBLE_FMPT_Event);
			}
			break;
		case RBLE_EVT_FMP_TARGET_CMD_DISALLOWED_IND:
			RBLE_FMPT_Event.type = RBLE_FMP_EVENT_TARGET_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_FMPT_Event.param.cmd_disallowed_ind.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_FMPT_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if( NULL != RBLE_FMPT_Info.Fmt_Handler){
				RBLE_FMPT_Info.Fmt_Handler(&RBLE_FMPT_Event);
			}
			break;
		case RBLE_EVT_FMP_LOCATOR_ENABLE_COMP:
			RBLE_FMPL_Event.type = RBLE_FMP_EVENT_LOCATOR_ENABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_FMPL_Event.param.locator_enable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_FMPL_Event.param.locator_enable.conhdl );
			RBLE_DATA_GET16( Data_p, RBLE_FMPL_Event.param.locator_enable.ias.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_FMPL_Event.param.locator_enable.ias.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_FMPL_Event.param.locator_enable.ias.alert_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_FMPL_Event.param.locator_enable.ias.alert_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_FMPL_Event.param.locator_enable.ias.alert_char_prop );

			/* Call Back */
			if( NULL != RBLE_FMPL_Info.Fml_Handler){
				RBLE_FMPL_Info.Fml_Handler(&RBLE_FMPL_Event);
			}
			break;
		case RBLE_EVT_FMP_LOCATOR_DISABLE_COMP:
			RBLE_FMPL_Event.type         = RBLE_FMP_EVENT_LOCATOR_DISABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_FMPL_Event.param.locator_disable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_FMPL_Event.param.locator_disable.conhdl  );

			/* Call Back */
			if( NULL != RBLE_FMPL_Info.Fml_Handler){
				RBLE_FMPL_Info.Fml_Handler(&RBLE_FMPL_Event);
			}
			/* Handler Clear */
			RBLE_FMPL_Info.conhdl      = 0;
			RBLE_FMPL_Info.Fml_Handler = NULL;
			break;
		case RBLE_EVT_FMP_LOCATOR_ERROR_IND:
			RBLE_FMPL_Event.type = RBLE_FMP_EVENT_LOCATOR_ERROR_IND;
			RBLE_DATA_GET8( Data_p, RBLE_FMPL_Event.param.locator_error_ind.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_FMPL_Event.param.locator_error_ind.conhdl );

			/* Call Back */
			if( NULL != RBLE_FMPL_Info.Fml_Handler){
				RBLE_FMPL_Info.Fml_Handler(&RBLE_FMPL_Event);
			}
			break;
		case RBLE_EVT_FMP_LOCATOR_CMD_DISALLOWED_IND:
			RBLE_FMPL_Event.type = RBLE_FMP_EVENT_LOCATOR_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_FMPL_Event.param.cmd_disallowed_ind.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_FMPL_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if( NULL != RBLE_FMPL_Info.Fml_Handler){
				RBLE_FMPL_Info.Fml_Handler(&RBLE_FMPL_Event);
			}
			break;
		default:
			ret = RBLE_ERR;
			break;
	}
	return ret;
}
#endif

/******************************************************************************************/
/**
 *  @brief Health Thermometer Profile Commnad Result Notification
 *
 *  This function does Health Thermometer Profile Result Notification to APP@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Event Data
 *    -# Call Back API Call
 *
 *  @param[in]	evt_id		EVENT ID
 *  @param[in]	data_pnt	Data Address
 *
 *  @retval
 */
/******************************************************************************************/
#if (PRF_SEL_HTPC || PRF_SEL_HTPT)
RBLE_STATUS RBLE_HTP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt)
{
	RBLE_STATUS ret = RBLE_OK;
	RBLE_HTPC_EVENT			RBLE_HTPC_Event;
	RBLE_HTPT_EVENT			RBLE_HTPT_Event;
	uint8_t					*Data_p;

	Data_p = data_pnt;

	switch(evt_id){
		case RBLE_EVT_HTP_COLLECTOR_ENABLE_COMP:
			RBLE_HTPC_Event.type = RBLE_HTP_EVENT_COLLECTOR_ENABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.conhdl );

			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.temp_meas_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.temp_meas_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.temp_meas_cfg_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.temp_meas_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.temp_type_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.temp_type_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.temp_type_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.reserved2 );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.interm_temp_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.interm_temp_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.interm_temp_cfg_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.interm_temp_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.reserved3 );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.meas_intv_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.meas_intv_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.meas_intv_cfg_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.valid_range_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.meas_intv_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.hts.reserved4 );

			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.sys_id_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.sys_id_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.sys_id_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.model_nb_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.model_nb_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.model_nb_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.reserved2 );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.serial_nb_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.serial_nb_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.serial_nb_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.reserved3 );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.fw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.fw_rev_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.fw_rev_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.reserved4 );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.hw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.hw_rev_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.hw_rev_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.reserved5 );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.sw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.sw_rev_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.sw_rev_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.reserved6 );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.manuf_name_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.manuf_name_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.manuf_name_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.reserved7 );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.ieee_certif_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.ieee_certif_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_enable.dis.ieee_certif_prop );

			/* Call Back */
			if( NULL != RBLE_HTPC_Info.Htc_Handler){
				RBLE_HTPC_Info.Htc_Handler(&RBLE_HTPC_Event);
			}
			break;
			
		case RBLE_EVT_HTP_COLLECTOR_DISABLE_COMP:
			RBLE_HTPC_Event.type = RBLE_HTP_EVENT_COLLECTOR_DISABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.collector_disable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.collector_disable.conhdl );

			/* Call Back */
			if( NULL != RBLE_HTPC_Info.Htc_Handler){
				RBLE_HTPC_Info.Htc_Handler(&RBLE_HTPC_Event);
			}
			/* Handler Clear */
			RBLE_HTPC_Info.conhdl      = 0;
			RBLE_HTPC_Info.Htc_Handler = NULL;
			break;
		case RBLE_EVT_HTP_COLLECTOR_ERROR_IND:
			RBLE_HTPC_Event.type = RBLE_HTP_EVENT_COLLECTOR_ERROR_IND;
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.error_ind.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.error_ind.conhdl );

			/* Call Back */
			if( NULL != RBLE_HTPC_Info.Htc_Handler){
				RBLE_HTPC_Info.Htc_Handler(&RBLE_HTPC_Event);
			}
			break;
		case RBLE_EVT_HTP_COLLECTOR_TEMP_IND:
			RBLE_HTPC_Event.type = RBLE_HTP_EVENT_COLLECTOR_TEMP_IND;
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.temp_ind.conhdl );

			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.temp_ind.temp_info.flag_stable_meas );	/* Stable or intermediary type of temperature */
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.temp_ind.temp_info.flags );				/* flags */
			RBLE_DATA_GET32( Data_p, RBLE_HTPC_Event.param.temp_ind.temp_info.temp_val );			/* temp value */
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.temp_ind.temp_info.stamp.year );		/* Year value */
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.temp_ind.temp_info.stamp.month );		/* Month value */
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.temp_ind.temp_info.stamp.day );		/* Day value */
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.temp_ind.temp_info.stamp.hour );		/* Hour value */
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.temp_ind.temp_info.stamp.min );		/* Minute value */
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.temp_ind.temp_info.stamp.sec );		/* Second value */
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.temp_ind.temp_info.stamp.reserved );	/* Reserved */

			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.temp_ind.temp_info.type );				/* type */

			/* Call Back */
			if( NULL != RBLE_HTPC_Info.Htc_Handler){
				RBLE_HTPC_Info.Htc_Handler(&RBLE_HTPC_Event);
			}
			break;
			
		case RBLE_EVT_HTP_COLLECTOR_MEAS_INTV_IND:
			RBLE_HTPC_Event.type = RBLE_HTP_EVENT_COLLECTOR_MEAS_INTV_IND;
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.meas_intv_ind.conhdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.meas_intv_ind.intv );

			/* Call Back */
			if( NULL != RBLE_HTPC_Info.Htc_Handler){
				RBLE_HTPC_Info.Htc_Handler(&RBLE_HTPC_Event);
			}
			break;
			
		case RBLE_EVT_HTP_COLLECTOR_READ_CHAR_RESPONSE:
			RBLE_HTPC_Event.type = RBLE_HTP_EVENT_COLLECTOR_READ_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.rd_char_resp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.rd_char_resp.att_code );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.rd_char_resp.data.each_len );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.rd_char_resp.data.len );
			memcpy(&RBLE_HTPC_Event.param.rd_char_resp.data.data[0], Data_p, RBLE_HTPC_Event.param.rd_char_resp.data.len);

			/* Call Back */
			if( NULL != RBLE_HTPC_Info.Htc_Handler){
				RBLE_HTPC_Info.Htc_Handler(&RBLE_HTPC_Event);
			}
			break;
			
		case RBLE_EVT_HTP_COLLECTOR_WRITE_CHAR_RESPONSE:
			RBLE_HTPC_Event.type = RBLE_HTP_EVENT_COLLECTOR_WRITE_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.wr_char_resp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.wr_char_resp.att_code );

			/* Call Back */
			if( NULL != RBLE_HTPC_Info.Htc_Handler){
				RBLE_HTPC_Info.Htc_Handler(&RBLE_HTPC_Event);
			}
			break;
			
		case RBLE_EVT_HTP_COLLECTOR_CMD_DISALLOWED_IND:
			RBLE_HTPC_Event.type = RBLE_HTP_EVENT_COLLECTOR_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_HTPC_Event.param.cmd_disallowed_ind.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_HTPC_Event.param.cmd_disallowed_ind.opcode );

			/* Call Back */
			if( NULL != RBLE_HTPC_Info.Htc_Handler){
				RBLE_HTPC_Info.Htc_Handler(&RBLE_HTPC_Event);
			}
			break;

		case RBLE_EVT_HTP_THERMOMETER_ENABLE_COMP:
			RBLE_HTPT_Event.type = RBLE_HTP_EVENT_THERMOMETER_ENABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_HTPT_Event.param.thermometer_enable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_HTPT_Event.param.thermometer_enable.conhdl );
			
			/* Call Back */
			if( NULL != RBLE_HTPT_Info.Htt_Handler){
				RBLE_HTPT_Info.Htt_Handler(&RBLE_HTPT_Event);
			}
			break;
			
		case RBLE_EVT_HTP_THERMOMETER_DISABLE_COMP:
			RBLE_HTPT_Event.type = RBLE_HTP_EVENT_THERMOMETER_DISABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_HTPT_Event.param.thermometer_disable.conhdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPT_Event.param.thermometer_disable.therm_info.temp_meas_ind_en );
			RBLE_DATA_GET16( Data_p, RBLE_HTPT_Event.param.thermometer_disable.therm_info.interm_temp_ntf_en );
			RBLE_DATA_GET16( Data_p, RBLE_HTPT_Event.param.thermometer_disable.therm_info.meas_intv_ind_en );
			RBLE_DATA_GET16( Data_p, RBLE_HTPT_Event.param.thermometer_disable.therm_info.meas_intv );

			/* Call Back */
			if( NULL != RBLE_HTPT_Info.Htt_Handler){
				RBLE_HTPT_Info.Htt_Handler(&RBLE_HTPT_Event);
			}
			/* Handler Clear */
			RBLE_HTPT_Info.conhdl      = 0;
			RBLE_HTPT_Info.Htt_Handler = NULL;
			break;

		case RBLE_EVT_HTP_THERMOMETER_ERROR_IND:
			RBLE_HTPT_Event.type = RBLE_HTP_EVENT_THERMOMETER_ERROR_IND;
			RBLE_DATA_GET16( Data_p, RBLE_HTPT_Event.param.error_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HTPT_Event.param.error_ind.status );

			/* Call Back */
			if( NULL != RBLE_HTPT_Info.Htt_Handler){
				RBLE_HTPT_Info.Htt_Handler(&RBLE_HTPT_Event);
			}
			break;

		case RBLE_EVT_HTP_THERMOMETER_SEND_TEMP_COMP:
			RBLE_HTPT_Event.type = RBLE_HTP_EVENT_THERMOMETER_SEND_TEMP_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_HTPT_Event.param.send_temp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HTPT_Event.param.send_temp.status );

			/* Call Back */
			if( NULL != RBLE_HTPT_Info.Htt_Handler){
				RBLE_HTPT_Info.Htt_Handler(&RBLE_HTPT_Event);
			}
			break;
			
		case RBLE_EVT_HTP_THERMOMETER_REQ_MEASUREMENT_PERIOD_IND_COMP:
			RBLE_HTPT_Event.type = RBLE_HTP_EVENT_THERMOMETER_REQ_MEASUREMENT_PERIOD_IND_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_HTPT_Event.param.send_meas_period.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HTPT_Event.param.send_meas_period.status );

			/* Call Back */
			if( NULL != RBLE_HTPT_Info.Htt_Handler){
				RBLE_HTPT_Info.Htt_Handler(&RBLE_HTPT_Event);
			}
			break;
			
		case RBLE_EVT_HTP_THERMOMETER_MEAS_INTV_CHG_IND:
			RBLE_HTPT_Event.type = RBLE_HTP_EVENT_THERMOMETER_MEAS_INTV_CHG_IND;
			RBLE_DATA_GET16( Data_p, RBLE_HTPT_Event.param.meas_intv_chg_ind.conhdl );
			RBLE_DATA_GET16( Data_p, RBLE_HTPT_Event.param.meas_intv_chg_ind.intv );

			/* Call Back */
			if( NULL != RBLE_HTPT_Info.Htt_Handler){
				RBLE_HTPT_Info.Htt_Handler(&RBLE_HTPT_Event);
			}
			break;

		case RBLE_EVT_HTP_THERMOMETER_CFG_INDNTF_IND:
			RBLE_HTPT_Event.type = RBLE_HTP_EVENT_THERMOMETER_CFG_INDNTF_IND;
			RBLE_DATA_GET16( Data_p, RBLE_HTPT_Event.param.htpt_cfg_indntf_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HTPT_Event.param.htpt_cfg_indntf_ind.char_code );
			RBLE_DATA_GET8( Data_p, RBLE_HTPT_Event.param.htpt_cfg_indntf_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_HTPT_Event.param.htpt_cfg_indntf_ind.cfg_val );

			/* Call Back */
			if( NULL != RBLE_HTPT_Info.Htt_Handler){
				RBLE_HTPT_Info.Htt_Handler(&RBLE_HTPT_Event);
			}
			break;

		case RBLE_EVT_HTP_THERMOMETER_CMD_DISALLOWED_IND:
			RBLE_HTPT_Event.type = RBLE_HTP_EVENT_THERMOMETER_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_HTPT_Event.param.cmd_disallowed_ind.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_HTPT_Event.param.cmd_disallowed_ind.opcode );

			/* Call Back */
			if( NULL != RBLE_HTPT_Info.Htt_Handler){
				RBLE_HTPT_Info.Htt_Handler(&RBLE_HTPT_Event);
			}
			break;

		default:
			ret = RBLE_ERR;
			break;
	}
	return ret;
}
#endif

/******************************************************************************************/
/**
 *  @brief Blood Pressure Profile Commnad Result Notification
 *
 *  This function does Blood Pressure Profile Result Notification to APP@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Event Data
 *    -# Call Back API Call
 *
 *  @param[in]	evt_id		EVENT ID
 *  @param[in]	data_pnt	Data Address
 *
 *  @retval
 */
/******************************************************************************************/
#if (PRF_SEL_BLPC || PRF_SEL_BLPS)
RBLE_STATUS RBLE_BLP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt)
{
	RBLE_STATUS ret = RBLE_OK;
	RBLE_BLPC_EVENT			RBLE_BLPC_Event;
	RBLE_BLPS_EVENT			RBLE_BLPS_Event;
	uint8_t					*Data_p;

	Data_p = data_pnt;

	switch(evt_id){
		case RBLE_EVT_BLP_COLLECTOR_ENABLE_COMP:
			RBLE_BLPC_Event.type = RBLE_BLP_EVENT_COLLECTOR_ENABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.conhdl );

			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.bls.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.bls.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.bls.bldprs_meas_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.bls.bldprs_meas_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.bls.bldprs_meas_cfg_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.bls.bldprs_meas_prop );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.bls.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.bls.interm_cufprs_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.bls.interm_cufprs_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.bls.interm_cufprs_cfg_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.bls.interm_cufprs_prop );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.bls.reserved2 );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.bls.bldprs_feat_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.bls.bldprs_feat_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.bls.bldprs_feat_prop );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.bls.reserved3 );

			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.sys_id_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.sys_id_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.sys_id_prop );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.model_nb_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.model_nb_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.model_nb_prop );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.reserved2 );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.serial_nb_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.serial_nb_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.serial_nb_prop );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.reserved3 );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.fw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.fw_rev_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.fw_rev_prop );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.reserved4 );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.hw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.hw_rev_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.hw_rev_prop );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.reserved5 );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.sw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.sw_rev_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.sw_rev_prop );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.reserved6 );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.manuf_name_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.manuf_name_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.manuf_name_prop );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.reserved7 );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.ieee_certif_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.ieee_certif_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_enable.dis.ieee_certif_prop );

			/* Call Back */
			if( NULL != RBLE_BLPC_Info.Blc_Handler){
				RBLE_BLPC_Info.Blc_Handler(&RBLE_BLPC_Event);
			}
			break;

		case RBLE_EVT_BLP_COLLECTOR_DISABLE_COMP:
			RBLE_BLPC_Event.type = RBLE_BLP_EVENT_COLLECTOR_DISABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.collector_disable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.collector_disable.conhdl );

			/* Call Back */
			if( NULL != RBLE_BLPC_Info.Blc_Handler){
				RBLE_BLPC_Info.Blc_Handler(&RBLE_BLPC_Event);
			}
			/* Handler Clear */
			RBLE_BLPC_Info.conhdl      = 0;
			RBLE_BLPC_Info.Blc_Handler = NULL;
			break;

		case RBLE_EVT_BLP_COLLECTOR_ERROR_IND:
			RBLE_BLPC_Event.type = RBLE_BLP_EVENT_COLLECTOR_ERROR_IND;
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.error_ind.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.error_ind.conhdl );

			/* Call Back */
			if( NULL != RBLE_BLPC_Info.Blc_Handler){
				RBLE_BLPC_Info.Blc_Handler(&RBLE_BLPC_Event);
			}
			break;

		case RBLE_EVT_BLP_COLLECTOR_MEASUREMENTS_IND:
			RBLE_BLPC_Event.type = RBLE_BLP_EVENT_COLLECTOR_MEASUREMENTS_IND;
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.measurements_ind.conhdl );

			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.measurements_ind.measurements_info.flag_stable_meas );	/* Stable or intermediary type of measurements */
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.measurements_ind.measurements_info.flags );				/* flags */
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.measurements_ind.measurements_info.press_val_field1 );	/* blood pressure value - systolic or cuff pressure */
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.measurements_ind.measurements_info.press_val_field2 );	/* blood pressure value - diastolic  or subfield1 */
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.measurements_ind.measurements_info.press_val_field3 );	/* blood pressure value - MAP  or subfield1 */
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.measurements_ind.measurements_info.stamp.year );		/* Year value */
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.measurements_ind.measurements_info.stamp.month );		/* Month value */
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.measurements_ind.measurements_info.stamp.day );		/* Day value */
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.measurements_ind.measurements_info.stamp.hour );		/* Hour value */
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.measurements_ind.measurements_info.stamp.min );		/* Minute value */
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.measurements_ind.measurements_info.stamp.sec );		/* Second value */
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.measurements_ind.measurements_info.stamp.reserved );	/* Reserved */
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.measurements_ind.measurements_info.rate );				/* pulse rate */
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.measurements_ind.measurements_info.id );					/* user ID */
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.measurements_ind.measurements_info.reserved );			/* Reserved */
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.measurements_ind.measurements_info.meas_sts );			/* measurement status */

			/* Call Back */
			if( NULL != RBLE_BLPC_Info.Blc_Handler){
				RBLE_BLPC_Info.Blc_Handler(&RBLE_BLPC_Event);
			}
			break;
			
		case RBLE_EVT_BLP_COLLECTOR_READ_CHAR_RESPONSE:
			RBLE_BLPC_Event.type = RBLE_BLP_EVENT_COLLECTOR_READ_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.rd_char_resp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.rd_char_resp.att_code );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.rd_char_resp.data.each_len );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.rd_char_resp.data.len	 );
			memcpy(&RBLE_BLPC_Event.param.rd_char_resp.data.data[0], Data_p, RBLE_BLPC_Event.param.rd_char_resp.data.len);

			/* Call Back */
			if( NULL != RBLE_BLPC_Info.Blc_Handler){
				RBLE_BLPC_Info.Blc_Handler(&RBLE_BLPC_Event);
			}
			break;
			
		case RBLE_EVT_BLP_COLLECTOR_WRITE_CHAR_RESPONSE:
			RBLE_BLPC_Event.type = RBLE_BLP_EVENT_COLLECTOR_WRITE_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.wr_char_resp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.wr_char_resp.att_code );

			/* Call Back */
			if( NULL != RBLE_BLPC_Info.Blc_Handler){
				RBLE_BLPC_Info.Blc_Handler(&RBLE_BLPC_Event);
			}
			break;
			
		case RBLE_EVT_BLP_COLLECTOR_CMD_DISALLOWED_IND:
			RBLE_BLPC_Event.type = RBLE_BLP_EVENT_COLLECTOR_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_BLPC_Event.param.cmd_disallowed_ind.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_BLPC_Event.param.cmd_disallowed_ind.opcode );

			/* Call Back */
			if( NULL != RBLE_BLPC_Info.Blc_Handler){
				RBLE_BLPC_Info.Blc_Handler(&RBLE_BLPC_Event);
			}
			break;

		case RBLE_EVT_BLP_SENSOR_ENABLE_COMP:
			RBLE_BLPS_Event.type = RBLE_BLP_EVENT_SENSOR_ENABLE_COMP;

			RBLE_DATA_GET8( Data_p, RBLE_BLPS_Event.param.sensor_enable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_BLPS_Event.param.sensor_enable.conhdl );
			
			/* Call Back */
			if( NULL != RBLE_BLPS_Info.Bls_Handler){
				RBLE_BLPS_Info.Bls_Handler(&RBLE_BLPS_Event);
			}
			break;
			
		case RBLE_EVT_BLP_SENSOR_DISABLE_COMP:
			RBLE_BLPS_Event.type = RBLE_BLP_EVENT_SENSOR_DISABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_BLPS_Event.param.sensor_disable.conhdl );
			RBLE_DATA_GET16( Data_p, RBLE_BLPS_Event.param.sensor_disable.sensor_info.bldprs_meas_ind_en );
			RBLE_DATA_GET16( Data_p, RBLE_BLPS_Event.param.sensor_disable.sensor_info.interm_cufprs_ntf_en );
			/* Call Back */
			if( NULL != RBLE_BLPS_Info.Bls_Handler){
				RBLE_BLPS_Info.Bls_Handler(&RBLE_BLPS_Event);
			}
			/* Handler Clear */
			RBLE_BLPS_Info.conhdl      = 0;
			RBLE_BLPS_Info.Bls_Handler = NULL;
			break;

		case RBLE_EVT_BLP_SENSOR_ERROR_IND:
			RBLE_BLPS_Event.type = RBLE_BLP_EVENT_SENSOR_ERROR_IND;
			RBLE_DATA_GET16( Data_p, RBLE_BLPS_Event.param.error_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_BLPS_Event.param.error_ind.status );

			/* Call Back */
			if( NULL != RBLE_BLPS_Info.Bls_Handler){
				RBLE_BLPS_Info.Bls_Handler(&RBLE_BLPS_Event);
			}
			break;

		case RBLE_EVT_BLP_SENSOR_SEND_MEASUREMENTS_COMP:
			RBLE_BLPS_Event.type = RBLE_BLP_EVENT_SENSOR_SEND_MEASUREMENTS_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_BLPS_Event.param.send_measurements.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_BLPS_Event.param.send_measurements.status );

			/* Call Back */
			if( NULL != RBLE_BLPS_Info.Bls_Handler){
				RBLE_BLPS_Info.Bls_Handler(&RBLE_BLPS_Event);
			}
			break;

		case RBLE_EVT_BLP_SENSOR_CFG_INDNTF_IND:
			RBLE_BLPS_Event.type = RBLE_BLP_EVENT_SENSOR_CFG_INDNTF_IND;
			RBLE_DATA_GET16( Data_p, RBLE_BLPS_Event.param.blps_cfg_indntf_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_BLPS_Event.param.blps_cfg_indntf_ind.char_code );
			RBLE_DATA_GET8( Data_p, RBLE_BLPS_Event.param.blps_cfg_indntf_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_BLPS_Event.param.blps_cfg_indntf_ind.cfg_val );

			/* Call Back */
			if( NULL != RBLE_BLPS_Info.Bls_Handler){
				RBLE_BLPS_Info.Bls_Handler(&RBLE_BLPS_Event);
			}
			break;
		case RBLE_EVT_BLP_SENSOR_CMD_DISALLOWED_IND:
			RBLE_BLPS_Event.type = RBLE_BLP_EVENT_SENSOR_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_BLPS_Event.param.cmd_disallowed_ind.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_BLPS_Event.param.cmd_disallowed_ind.opcode );

			/* Call Back */
			if( NULL != RBLE_BLPS_Info.Bls_Handler){
				RBLE_BLPS_Info.Bls_Handler(&RBLE_BLPS_Event);
			}
			break;

		default:
			ret = RBLE_ERR;
			break;
	}
	return ret;
}
#endif

/******************************************************************************************/
/**
 *  @brief Proximity Profile Commnad Result Notification
 *
 *  This function does Proximity Profile Result Notification to APP@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Event Data
 *    -# Call Back API Call
 *
 *  @param[in]	evt_id		EVENT ID
 *  @param[in]	data_pnt	Data Address
 *
 *  @retval
 */
/******************************************************************************************/
#if (PRF_SEL_PXPM || PRF_SEL_PXPR)
RBLE_STATUS RBLE_PXP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt)
{
	RBLE_STATUS ret = RBLE_OK;
	RBLE_PXPM_EVENT	RBLE_PXPM_Event;
	RBLE_PXPR_EVENT	RBLE_PXPR_Event;
	uint8_t			*Data_p;

	Data_p = data_pnt;
	switch(evt_id){
		case RBLE_EVT_PXP_MONITOR_ENABLE_COMP:
			RBLE_PXPM_Event.type = RBLE_PXP_EVENT_MONITOR_ENABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_PXPM_Event.param.monitor_enable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_PXPM_Event.param.monitor_enable.conhdl );
			RBLE_DATA_GET16( Data_p, RBLE_PXPM_Event.param.monitor_enable.lls.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_PXPM_Event.param.monitor_enable.lls.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_PXPM_Event.param.monitor_enable.lls.alert_lvl.char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_PXPM_Event.param.monitor_enable.lls.alert_lvl.val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_PXPM_Event.param.monitor_enable.lls.alert_lvl.prop );
			RBLE_DATA_GET8( Data_p, RBLE_PXPM_Event.param.monitor_enable.lls.alert_lvl.value );

			RBLE_DATA_GET16( Data_p, RBLE_PXPM_Event.param.monitor_enable.ias.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_PXPM_Event.param.monitor_enable.ias.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_PXPM_Event.param.monitor_enable.ias.alert_lvl.char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_PXPM_Event.param.monitor_enable.ias.alert_lvl.val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_PXPM_Event.param.monitor_enable.ias.alert_lvl.prop );
			RBLE_DATA_GET8( Data_p, RBLE_PXPM_Event.param.monitor_enable.ias.alert_lvl.value );

			RBLE_DATA_GET16( Data_p, RBLE_PXPM_Event.param.monitor_enable.tps.shdl );              /* Service start handle */
			RBLE_DATA_GET16( Data_p, RBLE_PXPM_Event.param.monitor_enable.tps.ehdl );              /* Service end handle */
			RBLE_DATA_GET16( Data_p, RBLE_PXPM_Event.param.monitor_enable.tps.txpw_lvl_char_hdl ); /* TX Power level char handle */
			RBLE_DATA_GET16( Data_p, RBLE_PXPM_Event.param.monitor_enable.tps.txpw_lvl_val_hdl );  /* TX Power Level value handle */
			RBLE_DATA_GET16( Data_p, RBLE_PXPM_Event.param.monitor_enable.tps.txpw_lvl_cfg_hdl );  /* TX Power Level configuration characteristic value handle */
			RBLE_DATA_GET8( Data_p, RBLE_PXPM_Event.param.monitor_enable.tps.txpw_lvl_prop );     /* TX Power level properties */
			RBLE_DATA_GET8( Data_p, RBLE_PXPM_Event.param.monitor_enable.tps.txpw_lvl );          /* TX Power level */
	
			/* Call Back */
			if( NULL != RBLE_PXPM_Info.Pmm_Handler){
				RBLE_PXPM_Info.Pmm_Handler(&RBLE_PXPM_Event);
			}
			break;
		case RBLE_EVT_PXP_MONITOR_DISABLE_COMP:
			RBLE_PXPM_Event.type = RBLE_PXP_EVENT_MONITOR_DISABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_PXPM_Event.param.monitor_disale.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_PXPM_Event.param.monitor_disale.conhdl );

			/* Call Back */
			if( NULL != RBLE_PXPM_Info.Pmm_Handler){
				RBLE_PXPM_Info.Pmm_Handler(&RBLE_PXPM_Event);
			}
			/* Handler Clear */
			RBLE_PXPM_Info.conhdl      = 0;
			RBLE_PXPM_Info.Pmm_Handler = NULL;
			break;
		case RBLE_EVT_PXP_MONITOR_ERROR_IND:
			RBLE_PXPM_Event.type = RBLE_PXP_EVENT_MONITOR_ERROR_IND;
			RBLE_DATA_GET8( Data_p, RBLE_PXPM_Event.param.monitor_error_ind.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_PXPM_Event.param.monitor_error_ind.conhdl );

			/* Call Back */
			if( NULL != RBLE_PXPM_Info.Pmm_Handler){
				RBLE_PXPM_Info.Pmm_Handler(&RBLE_PXPM_Event);
			}
			break;
		case RBLE_EVT_PXP_MONITOR_READ_CHAR_RESPONSE:
			RBLE_PXPM_Event.type = RBLE_PXP_EVENT_MONITOR_READ_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_PXPM_Event.param.rd_char_resp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_PXPM_Event.param.rd_char_resp.att_code );
			RBLE_DATA_GET8( Data_p, RBLE_PXPM_Event.param.rd_char_resp.data.each_len );
			RBLE_DATA_GET8( Data_p, RBLE_PXPM_Event.param.rd_char_resp.data.len	);
			memcpy(&RBLE_PXPM_Event.param.rd_char_resp.data.data[0], Data_p, RBLE_PXPM_Event.param.rd_char_resp.data.len);

			/* Call Back */
			if( NULL != RBLE_PXPM_Info.Pmm_Handler){
				RBLE_PXPM_Info.Pmm_Handler(&RBLE_PXPM_Event);
			}
			break;
		case RBLE_EVT_PXP_MONITOR_WRITE_CHAR_RESPONSE:
			RBLE_PXPM_Event.type = RBLE_PXP_EVENT_MONITOR_WRITE_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_PXPM_Event.param.wr_char_resp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_PXPM_Event.param.wr_char_resp.att_code );

			/* Call Back */
			if( NULL != RBLE_PXPM_Info.Pmm_Handler){
				RBLE_PXPM_Info.Pmm_Handler(&RBLE_PXPM_Event);
			}
			break;
		case RBLE_EVT_PXP_MONITOR_CMD_DISALLOWED_IND:
			RBLE_PXPM_Event.type = RBLE_PXP_EVENT_MONITOR_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_PXPM_Event.param.cmd_disallowed_ind.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_PXPM_Event.param.cmd_disallowed_ind.opcode );

			/* Call Back */
			if( NULL != RBLE_PXPM_Info.Pmm_Handler){
				RBLE_PXPM_Info.Pmm_Handler(&RBLE_PXPM_Event);
			}
			break;
		case RBLE_EVT_PXP_REPORTER_ENABLE_COMP:
			RBLE_PXPR_Event.type = RBLE_PXP_EVENT_REPORTER_ENABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_PXPR_Event.param.report_enable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_PXPR_Event.param.report_enable.conhdl );

			/* Call Back */
			if( NULL != RBLE_PXPR_Info.Pmr_Handler){
				RBLE_PXPR_Info.Pmr_Handler(&RBLE_PXPR_Event);
			}
			break;
		case RBLE_EVT_PXP_REPORTER_DISABLE_COMP:
			RBLE_PXPR_Event.type = RBLE_PXP_EVENT_REPORTER_DISABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_PXPR_Event.param.report_disable.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_PXPR_Event.param.report_disable.lls_alert_lvl );

			/* Call Back */
			if( NULL != RBLE_PXPR_Info.Pmr_Handler){
				RBLE_PXPR_Info.Pmr_Handler(&RBLE_PXPR_Event);
			}
			/* Handler Clear */
			RBLE_PXPR_Info.conhdl      = 0;
			RBLE_PXPR_Info.Pmr_Handler = NULL;
			break;
		case RBLE_EVT_PXP_REPORTER_ALERT_IND:
			RBLE_PXPR_Event.type = RBLE_PXP_EVENT_REPORTER_ALERT_IND;
			RBLE_DATA_GET16( Data_p, RBLE_PXPR_Event.param.report_alert_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_PXPR_Event.param.report_alert_ind.alert_lvl );

			/* Call Back */
			if( NULL != RBLE_PXPR_Info.Pmr_Handler){
				RBLE_PXPR_Info.Pmr_Handler(&RBLE_PXPR_Event);
			}
			break;
		case RBLE_EVT_PXP_REPORTER_CMD_DISALLOWED_IND:
			RBLE_PXPR_Event.type = RBLE_PXP_EVENT_REPORTER_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_PXPR_Event.param.cmd_disallowed_ind.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_PXPR_Event.param.cmd_disallowed_ind.opcode );

			/* Call Back */
			if( NULL != RBLE_PXPR_Info.Pmr_Handler){
				RBLE_PXPR_Info.Pmr_Handler(&RBLE_PXPR_Event);
			}
			break;
		default:
			ret = RBLE_ERR;
			break;
	}
	return ret;
}
#endif

/******************************************************************************************/
/**
 *  @brief HID over GATT Profile Commnad Result Notification
 *
 *  This function does HOGP Command Result Notification to APP@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Assemble event structure
 *    -# Create Event Data
 *    -# Call Back API Call
 *
 *  @param[in]	evt_id		EVENT ID
 *  @param[in]	data_pnt	Data Address
 *
 *  @retval
 */
/******************************************************************************************/
#if (PRF_SEL_HGHD || PRF_SEL_HGBH || PRF_SEL_HGRH)
RBLE_STATUS RBLE_HOGP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt)
{
	RBLE_STATUS			ret = RBLE_OK;
	RBLE_HGHD_EVENT		RBLE_HGHD_Event;
	RBLE_HGBH_EVENT		RBLE_HGBH_Event;
	RBLE_HGRH_EVENT		RBLE_HGRH_Event;
	uint16_t			i;
	uint8_t				*Data_p;

	Data_p = data_pnt;
	switch (evt_id) {
		case RBLE_EVT_HGP_HDEVICE_ENABLE_COMP:				/* HID Device enable Complete Event */
			RBLE_HGHD_Event.type = RBLE_HGP_EVENT_HDEVICE_ENABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_HGHD_Event.param.hdevice_enable.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.hdevice_enable.status );

			/* Call Back */
			if (NULL != RBLE_HGHD_Info.Hghd_Handler) {
				RBLE_HGHD_Info.Hghd_Handler( &RBLE_HGHD_Event );
			}
			break;
		case RBLE_EVT_HGP_HDEVICE_DISABLE_COMP:				/* HID Device disable Complete Event */
			RBLE_HGHD_Event.type = RBLE_HGP_EVENT_HDEVICE_DISABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_HGHD_Event.param.hdevice_disable.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.hdevice_disable.status );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.hdevice_disable.reserved );

			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.hdevice_disable.device_info.hids_inst_num );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.hdevice_disable.device_info.bas_inst_num );
			for (i = 0; i < RBLE_HIDS_INST_MAX; i++) {
				RBLE_DATA_GET16( Data_p, RBLE_HGHD_Event.param.hdevice_disable.device_info.report_input_ntf_en[ i ] );
			}
			for (i = 0; i < RBLE_HIDS_INST_MAX; i++) {
				RBLE_DATA_GET16( Data_p, RBLE_HGHD_Event.param.hdevice_disable.device_info.kb_report_ntf_en[ i ] );
			}
			for (i = 0; i < RBLE_HIDS_INST_MAX; i++) {
				RBLE_DATA_GET16( Data_p, RBLE_HGHD_Event.param.hdevice_disable.device_info.mo_report_ntf_en[ i ] );
			}
			for (i = 0; i < RBLE_HIDS_INST_MAX; i++) {
				RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.hdevice_disable.device_info.protocol_mode_val[ i ] );
			}
#if ((RBLE_HIDS_INST_MAX % 2) != 0)
   			Data_p++;	/* skip to pad */
#endif
			for (i = 0; i < RBLE_BAS_INST_MAX; i++) {
				RBLE_DATA_GET16( Data_p, RBLE_HGHD_Event.param.hdevice_disable.device_info.battery_level_ntf_en[ i ] );
			}

			/* Call Back */
			if (NULL != RBLE_HGHD_Info.Hghd_Handler) {
				RBLE_HGHD_Info.Hghd_Handler( &RBLE_HGHD_Event );
			}
			/* Handler Clear */
			RBLE_HGHD_Info.conhdl      = 0;
			RBLE_HGHD_Info.Hghd_Handler = NULL;
			break;
		case RBLE_EVT_HGP_HDEVICE_ERROR_IND:				/* HID Device error Complete Event */
			RBLE_HGHD_Event.type = RBLE_HGP_EVENT_HDEVICE_ERROR_IND;
			RBLE_DATA_GET16( Data_p, RBLE_HGHD_Event.param.error_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.error_ind.status );
			/* Call Back */
			if (NULL != RBLE_HGHD_Info.Hghd_Handler) {
				RBLE_HGHD_Info.Hghd_Handler( &RBLE_HGHD_Event );
			}
			break;
		case RBLE_EVT_HGP_HDEVICE_CFG_INDNTF_IND:			/* HID Device cfg indntf Complete Event */
			RBLE_HGHD_Event.type = RBLE_HGP_EVENT_HDEVICE_CFG_INDNTF_IND;
			RBLE_DATA_GET16( Data_p, RBLE_HGHD_Event.param.hghd_cfg_indntf_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.hghd_cfg_indntf_ind.inst_idx );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.hghd_cfg_indntf_ind.char_code );
			RBLE_DATA_GET16( Data_p, RBLE_HGHD_Event.param.hghd_cfg_indntf_ind.cfg_val );
			/* Call Back */
			if (NULL != RBLE_HGHD_Info.Hghd_Handler) {
				RBLE_HGHD_Info.Hghd_Handler( &RBLE_HGHD_Event );
			}
			break;
		case RBLE_EVT_HGP_HDEVICE_REPORT_IND:				/* HID Device set report Complete Event */
			RBLE_HGHD_Event.type = RBLE_HGP_EVENT_HDEVICE_REPORT_IND;
			RBLE_DATA_GET16( Data_p, RBLE_HGHD_Event.param.report_chg_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.report_chg_ind.inst_idx );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.report_chg_ind.reserved );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.report_chg_ind.report.device_type );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.report_chg_ind.report.report_type );
			for (i = 0; i < RBLE_HIDS_REPORT_MAX; i++) {
				RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.report_chg_ind.report.value[ i ] );
			}
#if ((RBLE_HIDS_REPORT_MAX % 2) != 0)
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.report_chg_ind.report.reserved );
#endif
			RBLE_DATA_GET16( Data_p, RBLE_HGHD_Event.param.report_chg_ind.report.value_size );
			/* Call Back */
			if (NULL != RBLE_HGHD_Info.Hghd_Handler) {
				RBLE_HGHD_Info.Hghd_Handler( &RBLE_HGHD_Event );
			}
			break;
		case RBLE_EVT_HGP_HDEVICE_PROTOCOL_MODE_CHG_EVT:	/* HID Device protocol mode chg Complete Event */
			RBLE_HGHD_Event.type = RBLE_HGP_EVENT_HDEVICE_PROTOCOL_MODE_CHG_EVT;
			RBLE_DATA_GET16( Data_p, RBLE_HGHD_Event.param.protocol_mode_chg_evt.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.protocol_mode_chg_evt.inst_idx );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.protocol_mode_chg_evt.protocol_mode_val );
			/* Call Back */
			if (NULL != RBLE_HGHD_Info.Hghd_Handler) {
				RBLE_HGHD_Info.Hghd_Handler( &RBLE_HGHD_Event );
			}
			break;
		case RBLE_EVT_HGP_HDEVICE_REPORT_EVT:				/* HID Device data output Complete Event */
			RBLE_HGHD_Event.type = RBLE_HGP_EVENT_HDEVICE_REPORT_EVT;
			RBLE_DATA_GET16( Data_p, RBLE_HGHD_Event.param.report_chg_evt.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.report_chg_evt.inst_idx );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.report_chg_evt.reserved );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.report_chg_evt.report.device_type );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.report_chg_evt.report.report_type );
			for (i = 0; i < RBLE_HIDS_REPORT_MAX; i++) {
				RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.report_chg_evt.report.value[ i ] );
			}
#if ((RBLE_HIDS_REPORT_MAX % 2) != 0)
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.report_chg_evt.report.reserved );
#endif
			RBLE_DATA_GET16( Data_p, RBLE_HGHD_Event.param.report_chg_evt.report.value_size );
			/* Call Back */
			if (NULL != RBLE_HGHD_Info.Hghd_Handler) {
				RBLE_HGHD_Info.Hghd_Handler( &RBLE_HGHD_Event );
			}
			break;
		case RBLE_EVT_HGP_HDEVICE_HID_CP_CHG_EVT:			/* HID Device HID contro point chg Complete Event */
			RBLE_HGHD_Event.type = RBLE_HGP_EVENT_HDEVICE_HID_CP_CHG_EVT;
			RBLE_DATA_GET16( Data_p, RBLE_HGHD_Event.param.hid_cp_chg_evt.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.hid_cp_chg_evt.inst_idx );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.hid_cp_chg_evt.control_point_val );
			/* Call Back */
			if (NULL != RBLE_HGHD_Info.Hghd_Handler) {
				RBLE_HGHD_Info.Hghd_Handler( &RBLE_HGHD_Event );
			}
			break;
		case RBLE_EVT_HGP_HDEVICE_REPORT_COMP:				/* HID Device send report value Complete Event */
			RBLE_HGHD_Event.type = RBLE_HGP_EVENT_HDEVICE_REPORT_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_HGHD_Event.param.send_report.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.send_report.status );
			/* Call Back */
			if (NULL != RBLE_HGHD_Info.Hghd_Handler) {
				RBLE_HGHD_Info.Hghd_Handler( &RBLE_HGHD_Event );
			}
			break;
		case RBLE_EVT_HGP_HDEVICE_SEND_BATTERY_LEVEL_COMP:	/* HID Device send battery level Complete Event */
			RBLE_HGHD_Event.type = RBLE_HGP_EVENT_HDEVICE_SEND_BATTERY_LEVEL_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_HGHD_Event.param.send_battery_level.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.send_battery_level.status );
			/* Call Back */
			if (NULL != RBLE_HGHD_Info.Hghd_Handler) {
				RBLE_HGHD_Info.Hghd_Handler( &RBLE_HGHD_Event );
			}
			break;
		case RBLE_EVT_HGP_HDEVICE_CMD_DISALLOWED_IND:		/* HID Device disallowed Complete Event */
			RBLE_HGHD_Event.type = RBLE_HGP_EVENT_HDEVICE_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.cmd_disallowed_ind.status );
			RBLE_DATA_GET8( Data_p, RBLE_HGHD_Event.param.cmd_disallowed_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_HGHD_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_HGHD_Info.Hghd_Handler) {
				RBLE_HGHD_Info.Hghd_Handler( &RBLE_HGHD_Event );
			}
			break;
		case RBLE_EVT_HGP_BHOST_ENABLE_COMP:				/* Boot Host enable Complete Event */
			RBLE_HGBH_Event.type = RBLE_HGP_EVENT_BHOST_ENABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.status );
			RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids_inst_num );
			RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.bas_inst_num );
			RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.reserved );
			
			RBLE_HGBH_Event.param.bhost_enable.hids = RBLE_HGBH_Info.Hids_Temp_Memory_Top;
			for (i = 0; i < RBLE_HIDS_INST_MAX; i++) {
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].shdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].ehdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].protocol_md_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].protocol_md_val_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].protocol_md_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].reserved );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].report_input_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].report_input_val_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].report_input_cfg_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].input_rep_ref_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].report_input_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].reserved1 );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].report_output_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].report_output_val_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].output_rep_ref_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].report_output_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].reserved2 );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].report_feature_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].report_feature_val_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].feature_rep_ref_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].report_feature_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].reserved3 );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].report_map_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].report_map_val_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].external_rep_ref_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].report_map_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].reserved4 );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].bootkb_input_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].bootkb_input_val_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].bootkb_input_cfg_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].bootkb_input_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].reserved5 );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].bootkb_output_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].bootkb_output_val_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].bootkb_output_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].reserved6 );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].bootmo_input_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].bootmo_input_val_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].bootmo_input_cfg_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].bootmo_input_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].reserved7 );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].hid_info_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].hid_info_val_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].hid_info_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].reserved8 );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].hid_cp_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].hid_cp_val_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].hid_cp_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].reserved9 );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].include_svc_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].include_svc_uuid );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].incl_shdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.hids[ i ].incl_ehdl );
			}
			RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.dis.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.dis.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.dis.pnp_id_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.dis.pnp_id_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.dis.pnp_id_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.dis.reserved );
			
			RBLE_HGBH_Event.param.bhost_enable.bas = RBLE_HGBH_Info.Bas_Temp_Memory_Top;
			for (i = 0; i < RBLE_BAS_INST_MAX; i++) {
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.bas[ i ].shdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.bas[ i ].ehdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.bas[ i ].battery_lvl_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.bas[ i ].battery_lvl_val_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.bas[ i ].battery_lvl_cfg_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_enable.bas[ i ].battery_lvl_rep_ref_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.bas[ i ].battery_lvl_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_enable.bas[ i ].reserved );
			}
			/* Call Back */
			if (NULL != RBLE_HGBH_Info.Hgbh_Handler) {
				RBLE_HGBH_Info.Hgbh_Handler( &RBLE_HGBH_Event );
			}
			break;
		case RBLE_EVT_HGP_BHOST_DISABLE_COMP:				/* Boot Host disable Complete Event */
			RBLE_HGBH_Event.type = RBLE_HGP_EVENT_BHOST_DISABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.bhost_disable.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.bhost_disable.status );

			/* Call Back */
			if (NULL != RBLE_HGBH_Info.Hgbh_Handler) {
				RBLE_HGBH_Info.Hgbh_Handler( &RBLE_HGBH_Event );
			}
			/* Handler Clear */
			RBLE_HGBH_Info.conhdl      = 0;
			RBLE_HGBH_Info.Hgbh_Handler = NULL;
			break;
		case RBLE_EVT_HGP_BHOST_ERROR_IND:					/* Boot Host error Complete Event */
			RBLE_HGBH_Event.type = RBLE_HGP_EVENT_BHOST_ERROR_IND;
			RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.error_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.error_ind.status );
			/* Call Back */
			if (NULL != RBLE_HGBH_Info.Hgbh_Handler) {
				RBLE_HGBH_Info.Hgbh_Handler( &RBLE_HGBH_Event );
			}
			break;
		case RBLE_EVT_HGP_BHOST_READ_CHAR_RESPONSE:			/* Boot Host read Characteristic Complete Event */
			RBLE_HGBH_Event.type = RBLE_HGP_EVENT_BHOST_READ_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.rd_char_resp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.rd_char_resp.att_code );
			RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.rd_char_resp.reserved );
			RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.rd_char_resp.data.each_len );
			RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.rd_char_resp.data.len );
			memcpy( &RBLE_HGBH_Event.param.rd_char_resp.data.data[ 0 ], Data_p, RBLE_ATTM_MAX_VALUE );
			/* Call Back */
			if (NULL != RBLE_HGBH_Info.Hgbh_Handler) {
				RBLE_HGBH_Info.Hgbh_Handler( &RBLE_HGBH_Event );
			}
			break;
		case RBLE_EVT_HGP_BHOST_WRITE_CHAR_RESPONSE:		/* Boot Host write Characteristic Complete Event */
			RBLE_HGBH_Event.type = RBLE_HGP_EVENT_BHOST_WRITE_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.wr_char_resp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.wr_char_resp.att_code );
			/* Call Back */
			if (NULL != RBLE_HGBH_Info.Hgbh_Handler) {
				RBLE_HGBH_Info.Hgbh_Handler( &RBLE_HGBH_Event );
			}
			break;
		case RBLE_EVT_HGP_BHOST_REPORT_NTF:					/* Boot Host data input Complete Event */
			RBLE_HGBH_Event.type = RBLE_HGP_EVENT_BHOST_REPORT_NTF;
			RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.report_ntf.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.report_ntf.inst_idx );
			RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.report_ntf.reserved );
			RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.report_ntf.report.device_type );
			RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.report_ntf.report.report_type );
			for (i = 0; i < RBLE_HIDS_REPORT_MAX; i++) {
				RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.report_ntf.report.value[ i ] );
			}
#if ((RBLE_HIDS_REPORT_MAX % 2) != 0)
			RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.report_ntf.report.reserved );
#endif
			RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.report_ntf.report.value_size );
			/* Call Back */
			if (NULL != RBLE_HGBH_Info.Hgbh_Handler) {
				RBLE_HGBH_Info.Hgbh_Handler( &RBLE_HGBH_Event );
			}
			break;
		case RBLE_EVT_HGP_BHOST_CMD_DISALLOWED_IND:			/* Boot Host Command Disallowed Complete Event */
			RBLE_HGBH_Event.type = RBLE_HGP_EVENT_BHOST_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.cmd_disallowed_ind.status );
			RBLE_DATA_GET8( Data_p, RBLE_HGBH_Event.param.cmd_disallowed_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_HGBH_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_HGBH_Info.Hgbh_Handler) {
				RBLE_HGBH_Info.Hgbh_Handler( &RBLE_HGBH_Event );
			}
			break;

		case RBLE_EVT_HGP_RHOST_ENABLE_COMP:				/* Report Host enable Complete Event */
			RBLE_HGRH_Event.type = RBLE_HGP_EVENT_RHOST_ENABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.status );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids_inst_num );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.bas_inst_num );
			
			RBLE_HGRH_Event.param.rhost_enable.hids = RBLE_HGRH_Info.Hids_Temp_Memory_Top;
			for (i = 0; i < RBLE_HIDS_INST_MAX; i++) {
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].shdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].ehdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].protocol_md_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].protocol_md_val_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].protocol_md_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].reserved );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].report_input_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].report_input_val_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].report_input_cfg_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].input_rep_ref_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].report_input_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].reserved1 );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].report_output_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].report_output_val_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].output_rep_ref_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].report_output_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].reserved2 );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].report_feature_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].report_feature_val_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].feature_rep_ref_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].report_feature_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].reserved3 );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].report_map_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].report_map_val_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].external_rep_ref_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].report_map_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].reserved4 );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].bootkb_input_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].bootkb_input_val_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].bootkb_input_cfg_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].bootkb_input_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].reserved5 );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].bootkb_output_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].bootkb_output_val_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].bootkb_output_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].reserved6 );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].bootmo_input_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].bootmo_input_val_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].bootmo_input_cfg_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].bootmo_input_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].reserved7 );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].hid_info_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].hid_info_val_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].hid_info_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].reserved8 );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].hid_cp_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].hid_cp_val_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].hid_cp_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].reserved9 );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].include_svc_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].include_svc_uuid );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].incl_shdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.hids[ i ].incl_ehdl );
			}
			RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.dis.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.dis.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.dis.pnp_id_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.dis.pnp_id_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.dis.pnp_id_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.dis.reserved );
			
			RBLE_HGRH_Event.param.rhost_enable.bas = RBLE_HGRH_Info.Bas_Temp_Memory_Top;
			for (i = 0; i < RBLE_BAS_INST_MAX; i++) {
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.bas[ i ].shdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.bas[ i ].ehdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.bas[ i ].battery_lvl_char_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.bas[ i ].battery_lvl_val_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.bas[ i ].battery_lvl_cfg_hdl );
				RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_enable.bas[ i ].battery_lvl_rep_ref_hdl );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.bas[ i ].battery_lvl_prop );
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_enable.bas[ i ].reserved );
			}
			/* Call Back */
			if (NULL != RBLE_HGRH_Info.Hgrh_Handler) {
				RBLE_HGRH_Info.Hgrh_Handler( &RBLE_HGRH_Event );
			}
			break;
		case RBLE_EVT_HGP_RHOST_DISABLE_COMP:				/* Report Host disable Complete Event */
			RBLE_HGRH_Event.type = RBLE_HGP_EVENT_RHOST_DISABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_disable.status );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rhost_disable.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rhost_disable.conhdl );
			/* Call Back */
			if (NULL != RBLE_HGRH_Info.Hgrh_Handler) {
				RBLE_HGRH_Info.Hgrh_Handler( &RBLE_HGRH_Event );
			}
			/* Handler Clear */
			RBLE_HGRH_Info.conhdl      = 0;
			RBLE_HGRH_Info.Hgrh_Handler = NULL;
			break;
		case RBLE_EVT_HGP_RHOST_ERROR_IND:					/* Report Host error Complete Event */
			RBLE_HGRH_Event.type = RBLE_HGP_EVENT_RHOST_ERROR_IND;
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.error_ind.status );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.error_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.error_ind.conhdl );
			/* Call Back */
			if (NULL != RBLE_HGRH_Info.Hgrh_Handler) {
				RBLE_HGRH_Info.Hgrh_Handler( &RBLE_HGRH_Event );
			}
			break;
		case RBLE_EVT_HGP_RHOST_READ_CHAR_RESPONSE:			/* Report Host read Characteristic Complete Event */
			RBLE_HGRH_Event.type = RBLE_HGP_EVENT_RHOST_READ_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rd_char_resp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rd_char_resp.att_code );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rd_char_resp.reserved );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rd_char_resp.data.each_len );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rd_char_resp.data.len );
			memcpy( &RBLE_HGRH_Event.param.rd_char_resp.data.data[ 0 ], Data_p, RBLE_ATTM_MAX_VALUE );
			/* Call Back */
			if (NULL != RBLE_HGRH_Info.Hgrh_Handler) {
				RBLE_HGRH_Info.Hgrh_Handler( &RBLE_HGRH_Event );
			}
			break;
		case RBLE_EVT_HGP_RHOST_READ_LONG_CHAR_RESPONSE:	/* Report Host read Long Characteristic Complete Event */
			RBLE_HGRH_Event.type = RBLE_HGP_EVENT_RHOST_READ_LONG_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rd_long_char_resp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rd_long_char_resp.att_code );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rd_long_char_resp.reserved );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rd_long_char_resp.data.val_len );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.rd_long_char_resp.data.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.rd_long_char_resp.data.attr_hdl );
			memcpy( &RBLE_HGRH_Event.param.rd_long_char_resp.data.value[ 0 ], Data_p, RBLE_ATTM_MAX_VALUE );
			/* Call Back */
			if (NULL != RBLE_HGRH_Info.Hgrh_Handler) {
				RBLE_HGRH_Info.Hgrh_Handler( &RBLE_HGRH_Event );
			}
			break;
		case RBLE_EVT_HGP_RHOST_WRITE_CHAR_RESPONSE:		/* Report Host write Characteristic Complete Event */
			RBLE_HGRH_Event.type = RBLE_HGP_EVENT_RHOST_WRITE_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.wr_char_resp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.wr_char_resp.att_code );
			/* Call Back */
			if (NULL != RBLE_HGRH_Info.Hgrh_Handler) {
				RBLE_HGRH_Info.Hgrh_Handler( &RBLE_HGRH_Event );
			}
			break;
		case RBLE_EVT_HGP_RHOST_REPORT_NTF:					/* Report Host data input Complete Event */
			RBLE_HGRH_Event.type = RBLE_HGP_EVENT_RHOST_REPORT_NTF;
			RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.report_ntf.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.report_ntf.inst_idx );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.report_ntf.reserved );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.report_ntf.report.device_type );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.report_ntf.report.report_type );
			for (i = 0; i < RBLE_HIDS_REPORT_MAX; i++) {
				RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.report_ntf.report.value[ i ] );
			}
#if ((RBLE_HIDS_REPORT_MAX % 2) != 0)
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.report_ntf.report.reserved );
#endif
			RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.report_ntf.report.value_size );
			/* Call Back */
			if (NULL != RBLE_HGRH_Info.Hgrh_Handler) {
				RBLE_HGRH_Info.Hgrh_Handler( &RBLE_HGRH_Event );
			}
			break;
		case RBLE_EVT_HGP_RHOST_BATTERY_LEVEL_NTF:			/* Report Host battery level Complete Event */
			RBLE_HGRH_Event.type = RBLE_HGP_EVENT_RHOST_BATTERY_LEVEL_NTF;
			RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.battery_level_ntf.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.battery_level_ntf.inst_idx );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.battery_level_ntf.battery_level );
			/* Call Back */
			if (NULL != RBLE_HGRH_Info.Hgrh_Handler) {
				RBLE_HGRH_Info.Hgrh_Handler( &RBLE_HGRH_Event );
			}
			break;
		case RBLE_EVT_HGP_RHOST_CMD_DISALLOWED_IND:			/* Report Host Command Disallowed Complete Event */
			RBLE_HGRH_Event.type = RBLE_HGP_EVENT_RHOST_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.cmd_disallowed_ind.status );
			RBLE_DATA_GET8( Data_p, RBLE_HGRH_Event.param.cmd_disallowed_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_HGRH_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_HGRH_Info.Hgrh_Handler) {
				RBLE_HGRH_Info.Hgrh_Handler( &RBLE_HGRH_Event );
			}
			break;
		default:
			ret = RBLE_ERR;
			break;
	}

	return ret;
}
#endif

/******************************************************************************************/
/**
 *  @brief Scan Parameters Profile Commnad Result Notification
 *
 *  This function does ScPP Command Result Notification to APP@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Assemble event structure
 *    -# Create Event Data
 *    -# Call Back API Call
 *
 *  @param[in]	evt_id		EVENT ID
 *  @param[in]	data_pnt	Data Address
 *
 *  @retval
 */
/******************************************************************************************/
#if (PRF_SEL_SPPS || PRF_SEL_SPPC)
RBLE_STATUS RBLE_ScPP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt)
{
	RBLE_STATUS			ret = RBLE_OK;
	RBLE_SPPS_EVENT		RBLE_SPPS_Event;
	RBLE_SPPC_EVENT		RBLE_SPPC_Event;
	uint8_t				*Data_p;

	Data_p = data_pnt;
	switch (evt_id) {
		case RBLE_EVT_SPP_SERVER_ENABLE_COMP:
			RBLE_SPPS_Event.type = RBLE_SPP_EVENT_SERVER_ENABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_SPPS_Event.param.server_enable.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_SPPS_Event.param.server_enable.status );
			/* Call Back */
			if (NULL != RBLE_SPPS_Info.Scans_Handler) {
				RBLE_SPPS_Info.Scans_Handler( &RBLE_SPPS_Event );
			}
			break;
		case RBLE_EVT_SPP_SERVER_DISABLE_COMP:
			RBLE_SPPS_Event.type = RBLE_SPP_EVENT_SERVER_DISABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_SPPS_Event.param.server_disable.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_SPPS_Event.param.server_disable.status );
			RBLE_DATA_GET8( Data_p, RBLE_SPPS_Event.param.server_disable.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_SPPS_Event.param.server_disable.device_info.s_refresh_ntf_en );
			/* Call Back */
			if (NULL != RBLE_SPPS_Info.Scans_Handler) {
				RBLE_SPPS_Info.Scans_Handler( &RBLE_SPPS_Event );
			}
			/* Handler Clear */
			RBLE_SPPS_Info.conhdl      = 0;
			RBLE_SPPS_Info.Scans_Handler = NULL;
			break;
		case RBLE_EVT_SPP_SERVER_CFG_INDNTF_IND:
			RBLE_SPPS_Event.type = RBLE_SPP_EVENT_SERVER_CFG_INDNTF_IND;
			RBLE_DATA_GET16( Data_p, RBLE_SPPS_Event.param.scans_cfg_indntf_ind.conhdl );
			RBLE_DATA_GET16( Data_p, RBLE_SPPS_Event.param.scans_cfg_indntf_ind.cfg_val );
			/* Call Back */
			if (NULL != RBLE_SPPS_Info.Scans_Handler) {
				RBLE_SPPS_Info.Scans_Handler( &RBLE_SPPS_Event );
			}
			break;
		case RBLE_EVT_SPP_SERVER_SEND_REFRESH_COMP:
			RBLE_SPPS_Event.type = RBLE_SPP_EVENT_SERVER_SEND_REFRESH_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_SPPS_Event.param.send_refresh.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_SPPS_Event.param.send_refresh.status );
			/* Call Back */
			if (NULL != RBLE_SPPS_Info.Scans_Handler) {
				RBLE_SPPS_Info.Scans_Handler( &RBLE_SPPS_Event );
			}
			break;
		case RBLE_EVT_SPP_SERVER_INTERVAL_CHG_EVT:
			RBLE_SPPS_Event.type = RBLE_SPP_EVENT_SERVER_INTERVAL_CHG_EVT;
			RBLE_DATA_GET16( Data_p, RBLE_SPPS_Event.param.interval_chg_evt.conhdl );
			RBLE_DATA_GET16( Data_p, RBLE_SPPS_Event.param.interval_chg_evt.scan_param.le_scan_interval );
			RBLE_DATA_GET16( Data_p, RBLE_SPPS_Event.param.interval_chg_evt.scan_param.le_scan_window );
			/* Call Back */
			if (NULL != RBLE_SPPS_Info.Scans_Handler) {
				RBLE_SPPS_Info.Scans_Handler( &RBLE_SPPS_Event );
			}
			break;
		case RBLE_EVT_SPP_SERVER_CMD_DISALLOWED_IND:
			RBLE_SPPS_Event.type = RBLE_SPP_EVENT_SERVER_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_SPPS_Event.param.cmd_disallowed_ind.status );
			RBLE_DATA_GET8( Data_p, RBLE_SPPS_Event.param.cmd_disallowed_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_SPPS_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_SPPS_Info.Scans_Handler) {
				RBLE_SPPS_Info.Scans_Handler( &RBLE_SPPS_Event );
			}
			break;
		case RBLE_EVT_SPP_CLIENT_ENABLE_COMP:
			RBLE_SPPC_Event.type = RBLE_SPP_EVENT_CLIENT_ENABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_SPPC_Event.param.client_enable.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_SPPC_Event.param.client_enable.status );
			RBLE_DATA_GET8( Data_p, RBLE_SPPC_Event.param.client_enable.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_SPPC_Event.param.client_enable.sps.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_SPPC_Event.param.client_enable.sps.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_SPPC_Event.param.client_enable.sps.intv_window_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_SPPC_Event.param.client_enable.sps.intv_window_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_SPPC_Event.param.client_enable.sps.intv_window_prop );
			RBLE_DATA_GET8( Data_p, RBLE_SPPC_Event.param.client_enable.sps.reserved1 );
			RBLE_DATA_GET16( Data_p, RBLE_SPPC_Event.param.client_enable.sps.refresh_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_SPPC_Event.param.client_enable.sps.refresh_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_SPPC_Event.param.client_enable.sps.refresh_cfg_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_SPPC_Event.param.client_enable.sps.refresh_prop );
			RBLE_DATA_GET8( Data_p, RBLE_SPPC_Event.param.client_enable.sps.reserved2 );
			/* Call Back */
			if (NULL != RBLE_SPPC_Info.Scanc_Handler) {
				RBLE_SPPC_Info.Scanc_Handler( &RBLE_SPPC_Event );
			}
			break;
		case RBLE_EVT_SPP_CLIENT_DISABLE_COMP:
			RBLE_SPPC_Event.type = RBLE_SPP_EVENT_CLIENT_DISABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_SPPC_Event.param.client_disable.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_SPPC_Event.param.client_disable.status );
			/* Call Back */
			if (NULL != RBLE_SPPC_Info.Scanc_Handler) {
				RBLE_SPPC_Info.Scanc_Handler( &RBLE_SPPC_Event );
			}
			/* Handler Clear */
			RBLE_SPPC_Info.conhdl      = 0;
			RBLE_SPPC_Info.Scanc_Handler = NULL;
			break;
		case RBLE_EVT_SPP_CLIENT_ERROR_IND:
			RBLE_SPPC_Event.type = RBLE_SPP_EVENT_CLIENT_ERROR_IND;
			RBLE_DATA_GET16( Data_p, RBLE_SPPC_Event.param.error_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_SPPC_Event.param.error_ind.status );
			/* Call Back */
			if (NULL != RBLE_SPPC_Info.Scanc_Handler) {
				RBLE_SPPC_Info.Scanc_Handler( &RBLE_SPPC_Event );
			}
			break;
		case RBLE_EVT_SPP_CLIENT_WRITE_CHAR_RESPONSE:
			RBLE_SPPC_Event.type = RBLE_SPP_EVENT_CLIENT_WRITE_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_SPPC_Event.param.wr_char_resp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_SPPC_Event.param.wr_char_resp.att_code );
			/* Call Back */
			if (NULL != RBLE_SPPC_Info.Scanc_Handler) {
				RBLE_SPPC_Info.Scanc_Handler( &RBLE_SPPC_Event );
			}
			break;
		case RBLE_EVT_SPP_CLIENT_CMD_DISALLOWED_IND:
			RBLE_SPPC_Event.type = RBLE_SPP_EVENT_CLIENT_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_SPPC_Event.param.cmd_disallowed_ind.status );
			RBLE_DATA_GET8( Data_p, RBLE_SPPC_Event.param.cmd_disallowed_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_SPPC_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_SPPC_Info.Scanc_Handler) {
				RBLE_SPPC_Info.Scanc_Handler( &RBLE_SPPC_Event );
			}
			break;
		default:
			ret = RBLE_ERR;
			break;
	}

	return ret;
}
#endif

/******************************************************************************************/
/**
 *  @brief Heart Rate Profile Commnad Result Notification
 *
 *  This function does HRP Command Result Notification to APP@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Assemble event structure
 *    -# Create Event Data
 *    -# Call Back API Call
 *
 *  @param[in]	evt_id		EVENT ID
 *  @param[in]	data_pnt	Data Address
 *
 *  @retval
 */
/******************************************************************************************/
#if (PRF_SEL_HRPC || PRF_SEL_HRPS)
RBLE_STATUS RBLE_HRP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt)
{
	RBLE_STATUS			ret = RBLE_OK;
	RBLE_HRPS_EVENT		RBLE_HRPS_Event;
	RBLE_HRPC_EVENT		RBLE_HRPC_Event;
	uint8_t				*Data_p;
	int_t				i;

	Data_p = data_pnt;
	switch (evt_id) {
		case RBLE_EVT_HRP_SENSOR_ENABLE_COMP:
			RBLE_HRPS_Event.type = RBLE_HRP_EVENT_SENSOR_ENABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_HRPS_Event.param.sensor_enable.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPS_Event.param.sensor_enable.status );
			/* Call Back */
			if (NULL != RBLE_HRPS_Info.Hrs_Handler) {
				RBLE_HRPS_Info.Hrs_Handler( &RBLE_HRPS_Event );
			}
			break;
		case RBLE_EVT_HRP_SENSOR_DISABLE_COMP:
			RBLE_HRPS_Event.type = RBLE_HRP_EVENT_SENSOR_DISABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_HRPS_Event.param.sensor_disable.conhdl );
			RBLE_DATA_GET16( Data_p, RBLE_HRPS_Event.param.sensor_disable.sensor_info.hrtrate_meas_ntf_en );
			/* Call Back */
			if (NULL != RBLE_HRPS_Info.Hrs_Handler) {
				RBLE_HRPS_Info.Hrs_Handler( &RBLE_HRPS_Event );
			}
			/* Handler Clear */
			RBLE_HRPS_Info.conhdl      = 0;
			RBLE_HRPS_Info.Hrs_Handler = NULL;
			break;
		case RBLE_EVT_HRP_SENSOR_ERROR_IND:
			RBLE_HRPS_Event.type = RBLE_HRP_EVENT_SENSOR_ERROR_IND;
			RBLE_DATA_GET16( Data_p, RBLE_HRPS_Event.param.error_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPS_Event.param.error_ind.status );
			/* Call Back */
			if (NULL != RBLE_HRPS_Info.Hrs_Handler) {
				RBLE_HRPS_Info.Hrs_Handler( &RBLE_HRPS_Event );
			}
			break;
		case RBLE_EVT_HRP_SENSOR_SEND_MEASUREMENTS_COMP:
			RBLE_HRPS_Event.type = RBLE_HRP_EVENT_SENSOR_SEND_MEASUREMENTS_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_HRPS_Event.param.send_measurements.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPS_Event.param.send_measurements.status );
			/* Call Back */
			if (NULL != RBLE_HRPS_Info.Hrs_Handler) {
				RBLE_HRPS_Info.Hrs_Handler( &RBLE_HRPS_Event );
			}
			break;
		case RBLE_EVT_HRP_SENSOR_CHG_CP_IND:
			RBLE_HRPS_Event.type = RBLE_HRP_EVENT_SENSOR_CHG_CP_IND;
			RBLE_DATA_GET16( Data_p, RBLE_HRPS_Event.param.hrps_chg_cp_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPS_Event.param.hrps_chg_cp_ind.cp_val );
			/* Call Back */
			if (NULL != RBLE_HRPS_Info.Hrs_Handler) {
				RBLE_HRPS_Info.Hrs_Handler( &RBLE_HRPS_Event );
			}
			break;
		case RBLE_EVT_HRP_SENSOR_CFG_NTF_IND:
			RBLE_HRPS_Event.type = RBLE_HRP_EVENT_SENSOR_CFG_NTF_IND;
			RBLE_DATA_GET16( Data_p, RBLE_HRPS_Event.param.hrps_cfg_ntf_ind.conhdl );
			RBLE_DATA_GET16( Data_p, RBLE_HRPS_Event.param.hrps_cfg_ntf_ind.cfg_val );
			/* Call Back */
			if (NULL != RBLE_HRPS_Info.Hrs_Handler) {
				RBLE_HRPS_Info.Hrs_Handler( &RBLE_HRPS_Event );
			}
			break;
		case RBLE_EVT_HRP_SENSOR_CMD_DISALLOWED_IND:
			RBLE_HRPS_Event.type = RBLE_HRP_EVENT_SENSOR_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_HRPS_Event.param.cmd_disallowed_ind.status );
			RBLE_DATA_GET8( Data_p, RBLE_HRPS_Event.param.cmd_disallowed_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_HRPS_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_HRPS_Info.Hrs_Handler) {
				RBLE_HRPS_Info.Hrs_Handler( &RBLE_HRPS_Event );
			}
			break;

		case RBLE_EVT_HRP_COLLECTOR_ENABLE_COMP:
			RBLE_HRPC_Event.type = RBLE_HRP_EVENT_COLLECTOR_ENABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.status );
			RBLE_DUMMY_GET8( Data_p );

			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.hrs.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.hrs.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.hrs.hrtrate_meas_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.hrs.hrtrate_meas_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.hrs.hrtrate_meas_cfg_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.hrs.hrtrate_meas_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.hrs.reserved1 );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.hrs.body_sensor_loc_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.hrs.body_sensor_loc_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.hrs.body_sensor_loc_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.hrs.reserved2 );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.hrs.hrtrate_cp_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.hrs.hrtrate_cp_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.hrs.hrtrate_cp_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.hrs.reserved3 );

			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.sys_id_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.sys_id_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.sys_id_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.model_nb_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.model_nb_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.model_nb_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.reserved2 );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.serial_nb_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.serial_nb_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.serial_nb_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.reserved3 );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.fw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.fw_rev_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.fw_rev_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.reserved4 );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.hw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.hw_rev_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.hw_rev_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.reserved5 );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.sw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.sw_rev_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.sw_rev_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.reserved6 );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.manuf_name_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.manuf_name_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.manuf_name_prop );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.reserved7 );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.ieee_certif_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.ieee_certif_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_enable.dis.ieee_certif_prop );
			/* Call Back */
			if (NULL != RBLE_HRPC_Info.Hrc_Handler) {
				RBLE_HRPC_Info.Hrc_Handler( &RBLE_HRPC_Event );
			}
			break;
		case RBLE_EVT_HRP_COLLECTOR_DISABLE_COMP:
			RBLE_HRPC_Event.type = RBLE_HRP_EVENT_COLLECTOR_DISABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.collector_disable.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.collector_disable.status );
			/* Call Back */
			if (NULL != RBLE_HRPC_Info.Hrc_Handler) {
				RBLE_HRPC_Info.Hrc_Handler( &RBLE_HRPC_Event );
			}
			/* Handler Clear */
			RBLE_HRPC_Info.conhdl      = 0;
			RBLE_HRPC_Info.Hrc_Handler = NULL;
			break;
		case RBLE_EVT_HRP_COLLECTOR_ERROR_IND:
			RBLE_HRPC_Event.type = RBLE_HRP_EVENT_COLLECTOR_ERROR_IND;
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.error_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.error_ind.status );
			/* Call Back */
			if (NULL != RBLE_HRPC_Info.Hrc_Handler) {
				RBLE_HRPC_Info.Hrc_Handler( &RBLE_HRPC_Event );
			}
			break;
		case RBLE_EVT_HRP_COLLECTOR_MEASUREMENTS_NTF:
			RBLE_HRPC_Event.type = RBLE_HRP_EVENT_COLLECTOR_MEASUREMENTS_NTF;
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.measurements_ntf.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.measurements_ntf.measurements_info.flags );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.measurements_ntf.measurements_info.rr_interval_num );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.measurements_ntf.measurements_info.heart_rate_measure );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.measurements_ntf.measurements_info.energy_expended );
			for(i = 0; i < RBLE_HRP_RR_INTERVAL_MAX; i++ ) {
				RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.measurements_ntf.measurements_info.rr_interval[ i ] );
			}
			/* Call Back */
			if (NULL != RBLE_HRPC_Info.Hrc_Handler) {
				RBLE_HRPC_Info.Hrc_Handler( &RBLE_HRPC_Event );
			}
			break;
		case RBLE_EVT_HRP_COLLECTOR_READ_CHAR_RESPONSE:
			RBLE_HRPC_Event.type = RBLE_HRP_EVENT_COLLECTOR_READ_CHAR_RESPONSE;

			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.rd_char_resp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.rd_char_resp.att_code );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.rd_char_resp.reserved );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.rd_char_resp.data.each_len );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.rd_char_resp.data.len );
			memcpy( &RBLE_HRPC_Event.param.rd_char_resp.data.data[ 0 ], Data_p, RBLE_HRPC_Event.param.rd_char_resp.data.len );
			/* Call Back */
			if (NULL != RBLE_HRPC_Info.Hrc_Handler) {
				RBLE_HRPC_Info.Hrc_Handler( &RBLE_HRPC_Event );
			}
			break;
		case RBLE_EVT_HRP_COLLECTOR_WRITE_CHAR_RESPONSE:
			RBLE_HRPC_Event.type = RBLE_HRP_EVENT_COLLECTOR_WRITE_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.wr_char_resp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.wr_char_resp.att_code );
			/* Call Back */
			if (NULL != RBLE_HRPC_Info.Hrc_Handler) {
				RBLE_HRPC_Info.Hrc_Handler( &RBLE_HRPC_Event );
			}
			break;
		case RBLE_EVT_HRP_COLLECTOR_CMD_DISALLOWED_IND:
			RBLE_HRPC_Event.type = RBLE_HRP_EVENT_COLLECTOR_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.cmd_disallowed_ind.status );
			RBLE_DATA_GET8( Data_p, RBLE_HRPC_Event.param.cmd_disallowed_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_HRPC_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_HRPC_Info.Hrc_Handler) {
				RBLE_HRPC_Info.Hrc_Handler( &RBLE_HRPC_Event );
			}
			break;
		default:
			ret = RBLE_ERR;
			break;
	}

	return ret;
}
#endif

/******************************************************************************************/
/**
 *  @brief Cycling Speed and Cadence Profile Commnad Result Notification
 *
 *  This function does CSCP Command Result Notification to APP@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Assemble event structure
 *    -# Create Event Data
 *    -# Call Back API Call
 *
 *  @param[in]	evt_id		EVENT ID
 *  @param[in]	data_pnt	Data Address
 *
 *  @retval
 */
/******************************************************************************************/
#if (PRF_SEL_CSCC || PRF_SEL_CSCS)
RBLE_STATUS RBLE_CSCP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt)
{
	RBLE_STATUS			ret = RBLE_OK;
	RBLE_CSCPS_EVENT	RBLE_CSCPS_Event;
	RBLE_CSCPC_EVENT	RBLE_CSCPC_Event;
	uint8_t				*Data_p;
	int_t				i;

	Data_p = data_pnt;
	switch (evt_id) {
		case RBLE_EVT_CSCP_SENSOR_ENABLE_COMP:
			RBLE_CSCPS_Event.type = RBLE_CSCP_EVENT_SENSOR_ENABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_CSCPS_Event.param.sensor_enable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPS_Event.param.sensor_enable.conhdl );
			/* Call Back */
			if (NULL != RBLE_CSCPS_Info.Cscs_Handler) {
				RBLE_CSCPS_Info.Cscs_Handler( &RBLE_CSCPS_Event );
			}
			break;
		case RBLE_EVT_CSCP_SENSOR_DISABLE_COMP:
			RBLE_CSCPS_Event.type = RBLE_CSCP_EVENT_SENSOR_DISABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_CSCPS_Event.param.sensor_disable.conhdl );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPS_Event.param.sensor_disable.sensor_info.cycspd_meas_ntf_en );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPS_Event.param.sensor_disable.sensor_info.sc_cp_ind_en );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPS_Event.param.sensor_disable.sensor_info.sensor_location );
			/* Call Back */
			if (NULL != RBLE_CSCPS_Info.Cscs_Handler) {
				RBLE_CSCPS_Info.Cscs_Handler( &RBLE_CSCPS_Event );
			}
			/* Handler Clear */
			RBLE_CSCPS_Info.conhdl      = 0;
			RBLE_CSCPS_Info.Cscs_Handler = NULL;
			break;
		case RBLE_EVT_CSCP_SENSOR_ERROR_IND:
			RBLE_CSCPS_Event.type = RBLE_CSCP_EVENT_SENSOR_ERROR_IND;
			RBLE_DATA_GET16( Data_p, RBLE_CSCPS_Event.param.error_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPS_Event.param.error_ind.status );
			/* Call Back */
			if (NULL != RBLE_CSCPS_Info.Cscs_Handler) {
				RBLE_CSCPS_Info.Cscs_Handler( &RBLE_CSCPS_Event );
			}
			break;
		case RBLE_EVT_CSCP_SENSOR_SEND_MEASUREMENTS_COMP:
			RBLE_CSCPS_Event.type = RBLE_CSCP_EVENT_SENSOR_SEND_MEASUREMENTS_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_CSCPS_Event.param.send_measurements.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPS_Event.param.send_measurements.status );
			/* Call Back */
			if (NULL != RBLE_CSCPS_Info.Cscs_Handler) {
				RBLE_CSCPS_Info.Cscs_Handler( &RBLE_CSCPS_Event );
			}
			break;
		case RBLE_EVT_CSCP_SENSOR_SEND_SC_CP_COMP:
			RBLE_CSCPS_Event.type = RBLE_CSCP_EVENT_SENSOR_SEND_SC_CP_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_CSCPS_Event.param.send_sc_cp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPS_Event.param.send_sc_cp.status );
			/* Call Back */
			if (NULL != RBLE_CSCPS_Info.Cscs_Handler) {
				RBLE_CSCPS_Info.Cscs_Handler( &RBLE_CSCPS_Event );
			}
			break;
		case RBLE_EVT_CSCP_SENSOR_CHG_SC_CP_IND:
			RBLE_CSCPS_Event.type = RBLE_CSCP_EVENT_SENSOR_CHG_SC_CP_IND;
			RBLE_DATA_GET16( Data_p, RBLE_CSCPS_Event.param.cscps_chg_sc_cp_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPS_Event.param.cscps_chg_sc_cp_ind.sc_cp_info.OpCode );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPS_Event.param.cscps_chg_sc_cp_ind.sc_cp_info.reserved1 );
			RBLE_DATA_GET32( Data_p, RBLE_CSCPS_Event.param.cscps_chg_sc_cp_ind.sc_cp_info.cumulative_value );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPS_Event.param.cscps_chg_sc_cp_ind.sc_cp_info.sensor_location );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPS_Event.param.cscps_chg_sc_cp_ind.sc_cp_info.request_op_code );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPS_Event.param.cscps_chg_sc_cp_ind.sc_cp_info.response_value );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPS_Event.param.cscps_chg_sc_cp_ind.sc_cp_info.reserved2 );
			/* Call Back */
			if (NULL != RBLE_CSCPS_Info.Cscs_Handler) {
				RBLE_CSCPS_Info.Cscs_Handler( &RBLE_CSCPS_Event );
			}
			break;
		case RBLE_EVT_CSCP_SENSOR_CFG_INDNTF_IND:
			RBLE_CSCPS_Event.type = RBLE_CSCP_EVENT_SENSOR_CFG_INDNTF_IND;
			RBLE_DATA_GET16( Data_p, RBLE_CSCPS_Event.param.cscps_cfg_indntf_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPS_Event.param.cscps_cfg_indntf_ind.char_code );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPS_Event.param.cscps_cfg_indntf_ind.cfg_val );
			/* Call Back */
			if (NULL != RBLE_CSCPS_Info.Cscs_Handler) {
				RBLE_CSCPS_Info.Cscs_Handler( &RBLE_CSCPS_Event );
			}
			break;
		case RBLE_EVT_CSCP_SENSOR_CMD_DISALLOWED_IND:
			RBLE_CSCPS_Event.type = RBLE_CSCP_EVENT_SENSOR_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_CSCPS_Event.param.cmd_disallowed_ind.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPS_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_CSCPS_Info.Cscs_Handler) {
				RBLE_CSCPS_Info.Cscs_Handler( &RBLE_CSCPS_Event );
			}
			break;

		case RBLE_EVT_CSCP_COLLECTOR_ENABLE_COMP:
			RBLE_CSCPC_Event.type = RBLE_CSCP_EVENT_COLLECTOR_ENABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.conhdl );

			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.cscs.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.cscs.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.cscs.cycspd_meas_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.cscs.cycspd_meas_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.cscs.cycspd_meas_cfg_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.cscs.cycspd_meas_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.cscs.reserved1 );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.cscs.csc_feature_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.cscs.csc_feature_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.cscs.csc_feature_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.cscs.reserved2 );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.cscs.sensor_loc_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.cscs.sensor_loc_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.cscs.sensor_loc_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.cscs.reserved3 );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.cscs.sc_cp_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.cscs.sc_cp_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.cscs.sc_cp_cfg_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.cscs.sc_cp_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.cscs.reserved4 );

			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.sys_id_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.sys_id_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.sys_id_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.model_nb_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.model_nb_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.model_nb_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.reserved2 );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.serial_nb_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.serial_nb_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.serial_nb_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.reserved3 );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.fw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.fw_rev_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.fw_rev_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.reserved4 );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.hw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.hw_rev_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.hw_rev_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.reserved5 );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.sw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.sw_rev_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.sw_rev_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.reserved6 );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.manuf_name_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.manuf_name_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.manuf_name_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.reserved7 );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.ieee_certif_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.ieee_certif_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_enable.dis.ieee_certif_prop );
			/* Call Back */
			if (NULL != RBLE_CSCPC_Info.Cscc_Handler) {
				RBLE_CSCPC_Info.Cscc_Handler( &RBLE_CSCPC_Event );
			}
			break;
		case RBLE_EVT_CSCP_COLLECTOR_DISABLE_COMP:
			RBLE_CSCPC_Event.type = RBLE_CSCP_EVENT_COLLECTOR_DISABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.collector_disable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.collector_disable.conhdl );
			/* Call Back */
			if (NULL != RBLE_CSCPC_Info.Cscc_Handler) {
				RBLE_CSCPC_Info.Cscc_Handler( &RBLE_CSCPC_Event );
			}
			/* Handler Clear */
			RBLE_CSCPC_Info.conhdl      = 0;
			RBLE_CSCPC_Info.Cscc_Handler = NULL;
			break;
		case RBLE_EVT_CSCP_COLLECTOR_ERROR_IND:
			RBLE_CSCPC_Event.type = RBLE_CSCP_EVENT_COLLECTOR_ERROR_IND;
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.error_ind.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.error_ind.conhdl );
			/* Call Back */
			if (NULL != RBLE_CSCPC_Info.Cscc_Handler) {
				RBLE_CSCPC_Info.Cscc_Handler( &RBLE_CSCPC_Event );
			}
			break;
		case RBLE_EVT_CSCP_COLLECTOR_MEASUREMENTS_NTF:
			RBLE_CSCPC_Event.type = RBLE_CSCP_EVENT_COLLECTOR_MEASUREMENTS_NTF;
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.measurements_ntf.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.measurements_ntf.measure_info.flags );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.measurements_ntf.measure_info.reserved );
			RBLE_DATA_GET32( Data_p, RBLE_CSCPC_Event.param.measurements_ntf.measure_info.wheel_revolutions );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.measurements_ntf.measure_info.wheel_event_time );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.measurements_ntf.measure_info.crank_revolutions );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.measurements_ntf.measure_info.crank_event_time );
			/* Call Back */
			if (NULL != RBLE_CSCPC_Info.Cscc_Handler) {
				RBLE_CSCPC_Info.Cscc_Handler( &RBLE_CSCPC_Event );
			}
			break;
		case RBLE_EVT_CSCP_COLLECTOR_SC_CP_IND:
			RBLE_CSCPC_Event.type = RBLE_CSCP_EVENT_COLLECTOR_SC_CP_IND;
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.sc_cp_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.sc_cp_ind.sc_cp_info.OpCode );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.sc_cp_ind.sc_cp_info.reserved1 );
			RBLE_DATA_GET32( Data_p, RBLE_CSCPC_Event.param.sc_cp_ind.sc_cp_info.cumulative_value );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.sc_cp_ind.sc_cp_info.sensor_location );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.sc_cp_ind.sc_cp_info.request_op_code );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.sc_cp_ind.sc_cp_info.response_value );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.sc_cp_ind.sc_cp_info.reserved2);
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.sc_cp_ind.location_num );
			for(i = 0; i < RBLE_CSCP_SENSORE_LOCATION_MAX; i++ ) {
				RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.sc_cp_ind.response_param[ i ] );
			}
			/* Call Back */
			if (NULL != RBLE_CSCPC_Info.Cscc_Handler) {
				RBLE_CSCPC_Info.Cscc_Handler( &RBLE_CSCPC_Event );
			}
			break;
		case RBLE_EVT_CSCP_COLLECTOR_READ_CHAR_RESPONSE:
			RBLE_CSCPC_Event.type = RBLE_CSCP_EVENT_COLLECTOR_READ_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.rd_char_resp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.rd_char_resp.att_code );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.rd_char_resp.reserved );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.rd_char_resp.data.each_len );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.rd_char_resp.data.len );
			memcpy( &RBLE_CSCPC_Event.param.rd_char_resp.data.data[ 0 ], Data_p, RBLE_CSCPC_Event.param.rd_char_resp.data.len );
			/* Call Back */
			if (NULL != RBLE_CSCPC_Info.Cscc_Handler) {
				RBLE_CSCPC_Info.Cscc_Handler( &RBLE_CSCPC_Event );
			}
			break;
		case RBLE_EVT_CSCP_COLLECTOR_WRITE_CHAR_RESPONSE:
			RBLE_CSCPC_Event.type = RBLE_CSCP_EVENT_COLLECTOR_WRITE_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.wr_char_resp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.wr_char_resp.att_code );
			/* Call Back */
			if (NULL != RBLE_CSCPC_Info.Cscc_Handler) {
				RBLE_CSCPC_Info.Cscc_Handler( &RBLE_CSCPC_Event );
			}
			break;
		case RBLE_EVT_CSCP_COLLECTOR_CMD_DISALLOWED_IND:
			RBLE_CSCPC_Event.type = RBLE_CSCP_EVENT_COLLECTOR_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.cmd_disallowed_ind.status );
			RBLE_DATA_GET8( Data_p, RBLE_CSCPC_Event.param.cmd_disallowed_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_CSCPC_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_CSCPC_Info.Cscc_Handler) {
				RBLE_CSCPC_Info.Cscc_Handler( &RBLE_CSCPC_Event );
			}
			break;
		default:
			ret = RBLE_ERR;
			break;
	}

	return ret;
}
#endif

/******************************************************************************************/
/**
 *  @brief Glucose Profile Commnad Result Notification
 *
 *  This function does Glucose Profile Result Notification to APP@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Event Data
 *    -# Call Back API Call
 *
 *  @param[in]	evt_id		EVENT ID
 *  @param[in]	data_pnt	Data Address
 *
 *  @retval
 */
/******************************************************************************************/
#if (PRF_SEL_GLPC || PRF_SEL_GLPS)
RBLE_STATUS RBLE_GLP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt)
{
	RBLE_STATUS ret = RBLE_OK;
	RBLE_GLPC_EVENT			RBLE_GLPC_Event;
	RBLE_GLPS_EVENT			RBLE_GLPS_Event;
	uint8_t					*Data_p;

	Data_p = data_pnt;

	switch(evt_id){
		case RBLE_EVT_GLP_COLLECTOR_ENABLE_COMP:					/* Collector enable Complete Event */
			RBLE_GLPC_Event.type = RBLE_GLP_EVENT_COLLECTOR_ENABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.collector_enable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.conhdl );

			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.gls.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.gls.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.gls.glucose_meas_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.gls.glucose_meas_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.gls.glucose_meas_cfg_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.collector_enable.gls.	glucose_meas_prop );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.gls.glucose_meas_context_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.gls.glucose_meas_context_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.gls.glucose_meas_context_cfg_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.collector_enable.gls.	glucose_meas_context_prop );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.gls.glucose_feature_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.gls.glucose_feature_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.collector_enable.gls.	glucose_feature_prop );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.gls.glucose_ra_cp_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.gls.glucose_ra_cp_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.gls.glucose_ra_cp_cfg_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.collector_enable.gls.	glucose_ra_cp_prop );
			RBLE_DUMMY_GET8( Data_p );

			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.sys_id_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.sys_id_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.sys_id_prop );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.model_nb_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.model_nb_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.model_nb_prop );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.serial_nb_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.serial_nb_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.serial_nb_prop );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.fw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.fw_rev_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.fw_rev_prop );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.hw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.hw_rev_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.hw_rev_prop );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.sw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.sw_rev_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.sw_rev_prop );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.manuf_name_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.manuf_name_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.manuf_name_prop );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.ieee_certif_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.ieee_certif_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.collector_enable.dis.ieee_certif_prop );

			/* Call Back */
			if( NULL != RBLE_GLPC_Info.Glc_Handler){
				RBLE_GLPC_Info.Glc_Handler(&RBLE_GLPC_Event);
			}
			break;
		case RBLE_EVT_GLP_COLLECTOR_DISABLE_COMP:				/* Collector disable Complete Event */
			RBLE_GLPC_Event.type = RBLE_GLP_EVENT_COLLECTOR_DISABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.collector_disable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.collector_disable.conhdl );

			/* Call Back */
			if( NULL != RBLE_GLPC_Info.Glc_Handler){
				RBLE_GLPC_Info.Glc_Handler(&RBLE_GLPC_Event);
			}
			/* Handler Clear */
			RBLE_GLPC_Info.conhdl      = 0;
			RBLE_GLPC_Info.Glc_Handler = NULL;
			break;
		case RBLE_EVT_GLP_COLLECTOR_ERROR_IND:					/* Collector error Complete Event */
			RBLE_GLPC_Event.type = RBLE_GLP_EVENT_COLLECTOR_ERROR_IND;
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.error_ind.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.error_ind.conhdl );

			/* Call Back */
			if( NULL != RBLE_GLPC_Info.Glc_Handler){
				RBLE_GLPC_Info.Glc_Handler(&RBLE_GLPC_Event);
			}
			break;
		case RBLE_EVT_GLP_COLLECTOR_MEASUREMENTS_NTF:			/* Collector measurements Complete Event */
			RBLE_GLPC_Event.type = RBLE_GLP_EVENT_COLLECTOR_MEASUREMENTS_NTF;
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.measurements_ntf.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.measurements_ntf.measure_info.flags );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.measurements_ntf.measure_info.seq_num );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.measurements_ntf.measure_info.stamp.year );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.measurements_ntf.measure_info.stamp.month );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.measurements_ntf.measure_info.stamp.day );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.measurements_ntf.measure_info.stamp.hour );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.measurements_ntf.measure_info.stamp.min );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.measurements_ntf.measure_info.stamp.sec );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.measurements_ntf.measure_info.time_offset );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.measurements_ntf.measure_info.concentration );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.measurements_ntf.measure_info.type );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.measurements_ntf.measure_info.sample_location );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.measurements_ntf.measure_info.sensor_status_annun );

			/* Call Back */
			if( NULL != RBLE_GLPC_Info.Glc_Handler){
				RBLE_GLPC_Info.Glc_Handler(&RBLE_GLPC_Event);
			}
			break;
		case RBLE_EVT_GLP_COLLECTOR_MEASUREMENTS_CONTEXT_NTF:	/* Collector measurements context Complete Event */
			RBLE_GLPC_Event.type = RBLE_GLP_EVENT_COLLECTOR_MEASUREMENTS_CONTEXT_NTF;
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.measurements_context_ntf.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.measurements_context_ntf.measure_context_info.flags );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.measurements_context_ntf.measure_context_info.seq_num );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.measurements_context_ntf.measure_context_info.ex_flags );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.measurements_context_ntf.measure_context_info.carbohydrate_id );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.measurements_context_ntf.measure_context_info.carbohydrate_kg );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.measurements_context_ntf.measure_context_info.meal );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.measurements_context_ntf.measure_context_info.tester );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.measurements_context_ntf.measure_context_info.health );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.measurements_context_ntf.measure_context_info.exercise_duration );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.measurements_context_ntf.measure_context_info.exercise_intensity );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.measurements_context_ntf.measure_context_info.medication_id );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.measurements_context_ntf.measure_context_info.medication );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.measurements_context_ntf.measure_context_info.HbA1c );

			/* Call Back */
			if( NULL != RBLE_GLPC_Info.Glc_Handler){
				RBLE_GLPC_Info.Glc_Handler(&RBLE_GLPC_Event);
			}
			break;
		case RBLE_EVT_GLP_COLLECTOR_RA_CP_IND:					/* Collector control point Complete Event */
			RBLE_GLPC_Event.type = RBLE_GLP_EVENT_COLLECTOR_RA_CP_IND;
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.ra_cp_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.ra_cp_ind.ra_cp_ind_info.OpCode );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.ra_cp_ind.ra_cp_ind_info.racp_operator );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.ra_cp_ind.ra_cp_ind_info.num_of_records );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.ra_cp_ind.ra_cp_ind_info.request_op_code );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.ra_cp_ind.ra_cp_ind_info.response_code_value );

			/* Call Back */
			if( NULL != RBLE_GLPC_Info.Glc_Handler){
				RBLE_GLPC_Info.Glc_Handler(&RBLE_GLPC_Event);
			}
			break;
		case RBLE_EVT_GLP_COLLECTOR_READ_CHAR_RESPONSE:			/* Collector read Characteristic Complete Event */
			RBLE_GLPC_Event.type = RBLE_GLP_EVENT_COLLECTOR_READ_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.rd_char_resp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.rd_char_resp.att_code );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.rd_char_resp.data.each_len );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.rd_char_resp.data.len	 );
			memcpy(&RBLE_GLPC_Event.param.rd_char_resp.data.data[0], Data_p, RBLE_GLPC_Event.param.rd_char_resp.data.len);

			/* Call Back */
			if( NULL != RBLE_GLPC_Info.Glc_Handler){
				RBLE_GLPC_Info.Glc_Handler(&RBLE_GLPC_Event);
			}
			break;
		case RBLE_EVT_GLP_COLLECTOR_WRITE_CHAR_RESPONSE:		/* Collector write Characteristic Complete Event */
			RBLE_GLPC_Event.type = RBLE_GLP_EVENT_COLLECTOR_WRITE_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.wr_char_resp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.wr_char_resp.att_code );

			/* Call Back */
			if( NULL != RBLE_GLPC_Info.Glc_Handler){
				RBLE_GLPC_Info.Glc_Handler(&RBLE_GLPC_Event);
			}
			break;
		case RBLE_EVT_GLP_COLLECTOR_CMD_DISALLOWED_IND:			/* Collector Command Disallowed Complete Event */
			RBLE_GLPC_Event.type = RBLE_GLP_EVENT_COLLECTOR_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_GLPC_Event.param.cmd_disallowed_ind.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPC_Event.param.cmd_disallowed_ind.opcode );

			/* Call Back */
			if( NULL != RBLE_GLPC_Info.Glc_Handler){
				RBLE_GLPC_Info.Glc_Handler(&RBLE_GLPC_Event);
			}
			break;

		case RBLE_EVT_GLP_SENSOR_ENABLE_COMP:						/* Sensor enable Complete Event */
			RBLE_GLPS_Event.type = RBLE_GLP_EVENT_SENSOR_ENABLE_COMP;

			RBLE_DATA_GET8( Data_p, RBLE_GLPS_Event.param.sensor_enable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPS_Event.param.sensor_enable.conhdl );

			/* Call Back */
			if( NULL != RBLE_GLPS_Info.Gls_Handler){
				RBLE_GLPS_Info.Gls_Handler(&RBLE_GLPS_Event);
			}
			break;
		case RBLE_EVT_GLP_SENSOR_DISABLE_COMP:						/* Sensor disable Complete Event */
			RBLE_GLPS_Event.type = RBLE_GLP_EVENT_SENSOR_DISABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_GLPS_Event.param.sensor_disable.conhdl );
			RBLE_DATA_GET16( Data_p, RBLE_GLPS_Event.param.sensor_disable.sensor_info.glp_meas_ntf_en );
			RBLE_DATA_GET16( Data_p, RBLE_GLPS_Event.param.sensor_disable.sensor_info.glp_meas_context_ntf_en );
			RBLE_DATA_GET16( Data_p, RBLE_GLPS_Event.param.sensor_disable.sensor_info.ra_cp_ind_en );

			/* Call Back */
			if( NULL != RBLE_GLPS_Info.Gls_Handler){
				RBLE_GLPS_Info.Gls_Handler(&RBLE_GLPS_Event);
			}
			/* Handler Clear */
			RBLE_GLPS_Info.conhdl      = 0;
			RBLE_GLPS_Info.Gls_Handler = NULL;
			break;
		case RBLE_EVT_GLP_SENSOR_ERROR_IND:							/* Sensor error Complete Event */
			RBLE_GLPS_Event.type = RBLE_GLP_EVENT_SENSOR_ERROR_IND;
			RBLE_DATA_GET16( Data_p, RBLE_GLPS_Event.param.error_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPS_Event.param.error_ind.status );

			/* Call Back */
			if( NULL != RBLE_GLPS_Info.Gls_Handler){
				RBLE_GLPS_Info.Gls_Handler(&RBLE_GLPS_Event);
			}
			break;
		case RBLE_EVT_GLP_SENSOR_SEND_MEASUREMENTS_COMP:			/* Sensor send measurements Complete Event */
			RBLE_GLPS_Event.type = RBLE_GLP_EVENT_SENSOR_SEND_MEASUREMENTS_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_GLPS_Event.param.send_measurements.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPS_Event.param.send_measurements.status );

			/* Call Back */
			if( NULL != RBLE_GLPS_Info.Gls_Handler){
				RBLE_GLPS_Info.Gls_Handler(&RBLE_GLPS_Event);
			}
			break;
		case RBLE_EVT_GLP_SENSOR_SEND_MEASUREMENTS_CONTEXT_COMP:	/* Sensor send measurements context Complete Event */
			RBLE_GLPS_Event.type = RBLE_GLP_EVENT_SENSOR_SEND_MEASUREMENTS_CONTEXT_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_GLPS_Event.param.send_measurements_context.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPS_Event.param.send_measurements_context.status );

			/* Call Back */
			if( NULL != RBLE_GLPS_Info.Gls_Handler){
				RBLE_GLPS_Info.Gls_Handler(&RBLE_GLPS_Event);
			}
			break;
		case RBLE_EVT_GLP_SENSOR_SEND_RA_CP_COMP:					/* Sensor send control point Complete Event */
			RBLE_GLPS_Event.type = RBLE_GLP_EVENT_SENSOR_SEND_RA_CP_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_GLPS_Event.param.send_ra_cp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPS_Event.param.send_ra_cp.status );

			/* Call Back */
			if( NULL != RBLE_GLPS_Info.Gls_Handler){
				RBLE_GLPS_Info.Gls_Handler(&RBLE_GLPS_Event);
			}
			break;
		case RBLE_EVT_GLP_SENSOR_CHG_RA_CP_IND:						/* Sensor Control Point change Complete Event */
			RBLE_GLPS_Event.type = RBLE_GLP_EVENT_SENSOR_CHG_RA_CP_IND;
			RBLE_DATA_GET16( Data_p, RBLE_GLPS_Event.param.glps_chg_ra_cp_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPS_Event.param.glps_chg_ra_cp_ind.ra_cp_info.OpCode );
			RBLE_DATA_GET8( Data_p, RBLE_GLPS_Event.param.glps_chg_ra_cp_ind.ra_cp_info.racp_operator );
			RBLE_DATA_GET8( Data_p, RBLE_GLPS_Event.param.glps_chg_ra_cp_ind.ra_cp_info.operand_value );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPS_Event.param.glps_chg_ra_cp_ind.ra_cp_info.min_sequence_num );
			RBLE_DATA_GET16( Data_p, RBLE_GLPS_Event.param.glps_chg_ra_cp_ind.ra_cp_info.max_sequence_num );
			RBLE_DATA_GET16( Data_p, RBLE_GLPS_Event.param.glps_chg_ra_cp_ind.ra_cp_info.min_stamp.year );
			RBLE_DATA_GET8( Data_p, RBLE_GLPS_Event.param.glps_chg_ra_cp_ind.ra_cp_info.min_stamp.month );
			RBLE_DATA_GET8( Data_p, RBLE_GLPS_Event.param.glps_chg_ra_cp_ind.ra_cp_info.min_stamp.day );
			RBLE_DATA_GET8( Data_p, RBLE_GLPS_Event.param.glps_chg_ra_cp_ind.ra_cp_info.min_stamp.hour );
			RBLE_DATA_GET8( Data_p, RBLE_GLPS_Event.param.glps_chg_ra_cp_ind.ra_cp_info.min_stamp.min );
			RBLE_DATA_GET8( Data_p, RBLE_GLPS_Event.param.glps_chg_ra_cp_ind.ra_cp_info.min_stamp.sec );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPS_Event.param.glps_chg_ra_cp_ind.ra_cp_info.max_stamp.year );
			RBLE_DATA_GET8( Data_p, RBLE_GLPS_Event.param.glps_chg_ra_cp_ind.ra_cp_info.max_stamp.month );
			RBLE_DATA_GET8( Data_p, RBLE_GLPS_Event.param.glps_chg_ra_cp_ind.ra_cp_info.max_stamp.day );
			RBLE_DATA_GET8( Data_p, RBLE_GLPS_Event.param.glps_chg_ra_cp_ind.ra_cp_info.max_stamp.hour );
			RBLE_DATA_GET8( Data_p, RBLE_GLPS_Event.param.glps_chg_ra_cp_ind.ra_cp_info.max_stamp.min );
			RBLE_DATA_GET8( Data_p, RBLE_GLPS_Event.param.glps_chg_ra_cp_ind.ra_cp_info.max_stamp.sec );

			/* Call Back */
			if( NULL != RBLE_GLPS_Info.Gls_Handler){
				RBLE_GLPS_Info.Gls_Handler(&RBLE_GLPS_Event);
			}
			break;
		case RBLE_EVT_GLP_SENSOR_CFG_INDNTF_IND:					/* Sensor cfg indntf Complete Event */
			RBLE_GLPS_Event.type = RBLE_GLP_EVENT_SENSOR_CFG_INDNTF_IND;
			RBLE_DATA_GET16( Data_p, RBLE_GLPS_Event.param.glps_cfg_indntf_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_GLPS_Event.param.glps_cfg_indntf_ind.char_code );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPS_Event.param.glps_cfg_indntf_ind.cfg_val );

			/* Call Back */
			if( NULL != RBLE_GLPS_Info.Gls_Handler){
				RBLE_GLPS_Info.Gls_Handler(&RBLE_GLPS_Event);
			}
			break;
		case RBLE_EVT_GLP_SENSOR_CMD_DISALLOWED_IND:				/* Sensor Command disallowed Complete Event */
			RBLE_GLPS_Event.type = RBLE_GLP_EVENT_SENSOR_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_GLPS_Event.param.cmd_disallowed_ind.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_GLPS_Event.param.cmd_disallowed_ind.opcode );

			/* Call Back */
			if( NULL != RBLE_GLPS_Info.Gls_Handler){
				RBLE_GLPS_Info.Gls_Handler(&RBLE_GLPS_Event);
			}
			break;
		default:
			ret = RBLE_ERR;
			break;
	}
	return ret;
}
#endif

/******************************************************************************************/
/**
 *  @brief Cycling Power Profile Commnad Result Notification
 *
 *  This function does CPP Command Result Notification to APP@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Assemble event structure
 *    -# Create Event Data
 *    -# Call Back API Call
 *
 *  @param[in]	evt_id		EVENT ID
 *  @param[in]	data_pnt	Data Address
 *
 *  @retval
 */
/******************************************************************************************/
#if (PRF_SEL_CPPS || PRF_SEL_CPPC)
RBLE_STATUS RBLE_CPP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt)
{
	RBLE_STATUS			ret = RBLE_OK;
	RBLE_CPPS_EVENT		RBLE_CPPS_Event;
	RBLE_CPPC_EVENT		RBLE_CPPC_Event;
	uint8_t				*Data_p;
	int_t				i;

	Data_p = data_pnt;
	switch (evt_id) {
		case RBLE_EVT_CPP_SENSOR_ENABLE_COMP:
			RBLE_CPPS_Event.type = RBLE_CPP_EVENT_SENSOR_ENABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_CPPS_Event.param.sensor_enable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.sensor_enable.conhdl );
			/* Call Back */
			if (NULL != RBLE_CPPS_Info.Cps_Handler) {
				RBLE_CPPS_Info.Cps_Handler( &RBLE_CPPS_Event );
			}
			break;
		case RBLE_EVT_CPP_SENSOR_DISABLE_COMP:
			RBLE_CPPS_Event.type = RBLE_CPP_EVENT_SENSOR_DISABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.sensor_disable.conhdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.sensor_disable.sensor_info.cp_meas_ntf_en );
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.sensor_disable.sensor_info.cp_meas_brd_en );
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.sensor_disable.sensor_info.cp_vector_ntf_en );
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.sensor_disable.sensor_info.cp_cp_ind_en );
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.sensor_disable.sensor_info.battery_level_ntf_en );
			RBLE_DATA_GET8( Data_p, RBLE_CPPS_Event.param.sensor_disable.sensor_info.sensor_location );
			/* Call Back */
			if (NULL != RBLE_CPPS_Info.Cps_Handler) {
				RBLE_CPPS_Info.Cps_Handler( &RBLE_CPPS_Event );
			}
			/* Handler Clear */
			RBLE_CPPS_Info.conhdl      = 0;
			RBLE_CPPS_Info.Cps_Handler = NULL;
			break;
		case RBLE_EVT_CPP_SENSOR_ERROR_IND:
			RBLE_CPPS_Event.type = RBLE_CPP_EVENT_SENSOR_ERROR_IND;
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.error_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPS_Event.param.error_ind.status );
			/* Call Back */
			if (NULL != RBLE_CPPS_Info.Cps_Handler) {
				RBLE_CPPS_Info.Cps_Handler( &RBLE_CPPS_Event );
			}
			break;
		case RBLE_EVT_CPP_SENSOR_SEND_MEASUREMENTS_COMP:
			RBLE_CPPS_Event.type = RBLE_CPP_EVENT_SENSOR_SEND_MEASUREMENTS_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.send_measurements.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPS_Event.param.send_measurements.status );
			/* Call Back */
			if (NULL != RBLE_CPPS_Info.Cps_Handler) {
				RBLE_CPPS_Info.Cps_Handler( &RBLE_CPPS_Event );
			}
			break;
		case RBLE_EVT_CPP_SENSOR_BROADCAST_MEASUREMENTS_COMP:
			RBLE_CPPS_Event.type = RBLE_CPP_EVENT_SENSOR_BROADCAST_MEASUREMENTS_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.broadcast_measurements.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPS_Event.param.broadcast_measurements.status );
			/* Call Back */
			if (NULL != RBLE_CPPS_Info.Cps_Handler) {
				RBLE_CPPS_Info.Cps_Handler( &RBLE_CPPS_Event );
			}
			break;
		case RBLE_EVT_CPP_SENSOR_SEND_VECTOR_COMP:
			RBLE_CPPS_Event.type = RBLE_CPP_EVENT_SENSOR_SEND_VECTOR_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.send_vector.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPS_Event.param.send_vector.status );
			/* Call Back */
			if (NULL != RBLE_CPPS_Info.Cps_Handler) {
				RBLE_CPPS_Info.Cps_Handler( &RBLE_CPPS_Event );
			}
			break;
		case RBLE_EVT_CPP_SENSOR_SEND_CP_CP_COMP:
			RBLE_CPPS_Event.type = RBLE_CPP_EVENT_SENSOR_SEND_CP_CP_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.send_cp_cp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPS_Event.param.send_cp_cp.status );
			/* Call Back */
			if (NULL != RBLE_CPPS_Info.Cps_Handler) {
				RBLE_CPPS_Info.Cps_Handler( &RBLE_CPPS_Event );
			}
			break;
		case RBLE_EVT_CPP_SENSOR_SEND_BATTERY_LEVEL_COMP:
			RBLE_CPPS_Event.type = RBLE_CPP_EVENT_SENSOR_SEND_BATTERY_LEVEL_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.send_battery_level.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPS_Event.param.send_battery_level.status );
			/* Call Back */
			if (NULL != RBLE_CPPS_Info.Cps_Handler) {
				RBLE_CPPS_Info.Cps_Handler( &RBLE_CPPS_Event );
			}
			break;
		case RBLE_EVT_CPP_SENSOR_CHG_CP_CP_IND:
			RBLE_CPPS_Event.type = RBLE_CPP_EVENT_SENSOR_CHG_CP_CP_IND;
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.chg_cp_cp_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPS_Event.param.chg_cp_cp_ind.wr_cp_info.OpCode );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET32( Data_p, RBLE_CPPS_Event.param.chg_cp_cp_ind.wr_cp_info.cumulative_value );
			RBLE_DATA_GET8( Data_p, RBLE_CPPS_Event.param.chg_cp_cp_ind.wr_cp_info.sensor_location );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.chg_cp_cp_ind.wr_cp_info.crank_length );
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.chg_cp_cp_ind.wr_cp_info.chain_length );
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.chg_cp_cp_ind.wr_cp_info.chain_weight );
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.chg_cp_cp_ind.wr_cp_info.span_length );
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.chg_cp_cp_ind.wr_cp_info.mask_meas_content );
			/* Call Back */
			if (NULL != RBLE_CPPS_Info.Cps_Handler) {
				RBLE_CPPS_Info.Cps_Handler( &RBLE_CPPS_Event );
			}
			break;
		case RBLE_EVT_CPP_SENSOR_CFG_INDNTFBRD_IND:
			RBLE_CPPS_Event.type = RBLE_CPP_EVENT_SENSOR_CFG_INDNTFBRD_IND;
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.cfg_indntfbrd_ind.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPS_Event.param.cfg_indntfbrd_ind.char_code );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.cfg_indntfbrd_ind.cfg_val );
			/* Call Back */
			if (NULL != RBLE_CPPS_Info.Cps_Handler) {
				RBLE_CPPS_Info.Cps_Handler( &RBLE_CPPS_Event );
			}
			break;
		case RBLE_EVT_CPP_SENSOR_CMD_DISALLOWED_IND:
			RBLE_CPPS_Event.type = RBLE_CPP_EVENT_SENSOR_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_CPPS_Event.param.cmd_disallowed_ind.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_CPPS_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_CPPS_Info.Cps_Handler) {
				RBLE_CPPS_Info.Cps_Handler( &RBLE_CPPS_Event );
			}
			break;

		case RBLE_EVT_CPP_COLLECTOR_ENABLE_COMP:
			RBLE_CPPC_Event.type = RBLE_CPP_EVENT_COLLECTOR_ENABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.conhdl );

			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.cp_meas_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.cp_meas_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.cp_meas_cfg_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.cp_meas_brd_cfg_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.cp_meas_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.reserved1 );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.cp_feature_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.cp_feature_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.cp_feature_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.reserved2 );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.sensor_loc_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.sensor_loc_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.sensor_loc_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.reserved3 );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.cp_vector_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.cp_vector_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.cp_vector_cfg_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.cp_vector_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.reserved4 );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.cp_cp_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.cp_cp_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.cp_cp_cfg_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.cp_cp_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.cps.reserved5 );

			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.sys_id_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.sys_id_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.sys_id_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.model_nb_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.model_nb_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.model_nb_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.reserved2 );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.serial_nb_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.serial_nb_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.serial_nb_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.reserved3 );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.fw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.fw_rev_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.fw_rev_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.reserved4 );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.hw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.hw_rev_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.hw_rev_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.reserved5 );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.sw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.sw_rev_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.sw_rev_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.reserved6 );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.manuf_name_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.manuf_name_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.manuf_name_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.reserved7 );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.ieee_certif_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.ieee_certif_val_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.ieee_certif_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.dis.reserved8 );
			
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.bas.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.bas.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.bas.battery_lvl_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.bas.battery_lvl_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_enable.bas.battery_lvl_cfg_hdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.bas.battery_lvl_prop );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_enable.bas.reserved );
			
			/* Call Back */
			if (NULL != RBLE_CPPC_Info.Cpc_Handler) {
				RBLE_CPPC_Info.Cpc_Handler( &RBLE_CPPC_Event );
			}
			break;
		case RBLE_EVT_CPP_COLLECTOR_DISABLE_COMP:
			RBLE_CPPC_Event.type = RBLE_CPP_EVENT_COLLECTOR_DISABLE_COMP;
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.collector_disable.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.collector_disable.conhdl );
			/* Call Back */
			if (NULL != RBLE_CPPC_Info.Cpc_Handler) {
				RBLE_CPPC_Info.Cpc_Handler( &RBLE_CPPC_Event );
			}
			/* Handler Clear */
			RBLE_CPPC_Info.conhdl      = 0;
			RBLE_CPPC_Info.Cpc_Handler = NULL;
			break;
		case RBLE_EVT_CPP_COLLECTOR_ERROR_IND:
			RBLE_CPPC_Event.type = RBLE_CPP_EVENT_COLLECTOR_ERROR_IND;
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.error_ind.status );
			RBLE_DUMMY_GET8( Data_p );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.error_ind.conhdl );
			/* Call Back */
			if (NULL != RBLE_CPPC_Info.Cpc_Handler) {
				RBLE_CPPC_Info.Cpc_Handler( &RBLE_CPPC_Event );
			}
			break;
		case RBLE_EVT_CPP_COLLECTOR_MEASUREMENTS_NTF:
			RBLE_CPPC_Event.type = RBLE_CPP_EVENT_COLLECTOR_MEASUREMENTS_NTF;
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.measurements_ntf.conhdl );
			
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.measurements_ntf.measurements_info.flags );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.measurements_ntf.measurements_info.instant_power );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.measurements_ntf.measurements_info.pedal_balance );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.measurements_ntf.measurements_info.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.measurements_ntf.measurements_info.accumulated_torque );
			RBLE_DATA_GET32( Data_p, RBLE_CPPC_Event.param.measurements_ntf.measurements_info.wheel_revolutions );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.measurements_ntf.measurements_info.wheel_event );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.measurements_ntf.measurements_info.crank_revolutions );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.measurements_ntf.measurements_info.crank_event );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.measurements_ntf.measurements_info.max_force_magnitude );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.measurements_ntf.measurements_info.min_force_magnitude );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.measurements_ntf.measurements_info.max_torque_magnitude );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.measurements_ntf.measurements_info.min_torque_magnitude );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.measurements_ntf.measurements_info.max_angle );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.measurements_ntf.measurements_info.min_angle );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.measurements_ntf.measurements_info.top_dead_spot );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.measurements_ntf.measurements_info.bottom_dead_spot );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.measurements_ntf.measurements_info.accumulated_energy );
			/* Call Back */
			if (NULL != RBLE_CPPC_Info.Cpc_Handler) {
				RBLE_CPPC_Info.Cpc_Handler( &RBLE_CPPC_Event );
			}
			break;
		case RBLE_EVT_CPP_COLLECTOR_VECTOR_NTF:
			RBLE_CPPC_Event.type = RBLE_CPP_EVENT_COLLECTOR_VECTOR_NTF;
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.vector_ntf.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.vector_ntf.vector_info.flags );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.vector_ntf.vector_info.reserved1 );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.vector_ntf.vector_info.crank_revolutions );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.vector_ntf.vector_info.crank_event );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.vector_ntf.vector_info.first_crank_angle );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.vector_ntf.vector_info.array_num );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.vector_ntf.vector_info.reserved2 );
			for(i = 0; i < RBLE_CPP_MAGNITUDE_MAX; i++ ) {
				RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.vector_ntf.vector_info.magnitude[ i ] );
			}
			/* Call Back */
			if (NULL != RBLE_CPPC_Info.Cpc_Handler) {
				RBLE_CPPC_Info.Cpc_Handler( &RBLE_CPPC_Event );
			}
			break;
		case RBLE_EVT_CPP_COLLECTOR_CP_CP_IND:
			RBLE_CPPC_Event.type = RBLE_CPP_EVENT_COLLECTOR_CP_CP_IND;
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.conhdl );
			
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.ind_cp_info.OpCode );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.ind_cp_info.request_op_code );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.ind_cp_info.response_value );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.ind_cp_info.reserved1 );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.ind_cp_info.crank_length );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.ind_cp_info.chain_length );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.ind_cp_info.chain_weight );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.ind_cp_info.span_length );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.ind_cp_info.offset_compensation );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.ind_cp_info.sampling_rate );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.ind_cp_info.reserved2 );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.ind_cp_info.stamp.year );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.ind_cp_info.stamp.month );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.ind_cp_info.stamp.day );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.ind_cp_info.stamp.hour );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.ind_cp_info.stamp.min );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.ind_cp_info.stamp.sec );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.ind_cp_info.reserved2 );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.location_num );
			for(i = 0; i < RBLE_CPP_SENSORE_LOCATION_MAX; i++ ) {
				RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.cp_cp_ind.supported_location[ i ] );
			}
			/* Call Back */
			if (NULL != RBLE_CPPC_Info.Cpc_Handler) {
				RBLE_CPPC_Info.Cpc_Handler( &RBLE_CPPC_Event );
			}
			break;
		case RBLE_EVT_CPP_COLLECTOR_BATTERY_LEVEL_NTF:
			RBLE_CPPC_Event.type = RBLE_CPP_EVENT_COLLECTOR_BATTERY_LEVEL_NTF;
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.battery_level_ntf.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.battery_level_ntf.battery_level );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.battery_level_ntf.reserved );
			/* Call Back */
			if (NULL != RBLE_CPPC_Info.Cpc_Handler) {
				RBLE_CPPC_Info.Cpc_Handler( &RBLE_CPPC_Event );
			}
			break;
		case RBLE_EVT_CPP_COLLECTOR_READ_CHAR_RESPONSE:
			RBLE_CPPC_Event.type = RBLE_CPP_EVENT_COLLECTOR_READ_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.rd_char_resp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.rd_char_resp.att_code );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.rd_char_resp.reserved );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.rd_char_resp.data.each_len );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.rd_char_resp.data.len );
			memcpy( &RBLE_CPPC_Event.param.rd_char_resp.data.data[ 0 ], Data_p, RBLE_CPPC_Event.param.rd_char_resp.data.len );
			/* Call Back */
			if (NULL != RBLE_CPPC_Info.Cpc_Handler) {
				RBLE_CPPC_Info.Cpc_Handler( &RBLE_CPPC_Event );
			}
			break;
		case RBLE_EVT_CPP_COLLECTOR_WRITE_CHAR_RESPONSE:
			RBLE_CPPC_Event.type = RBLE_CPP_EVENT_COLLECTOR_WRITE_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.wr_char_resp.conhdl );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.wr_char_resp.att_code );
			/* Call Back */
			if (NULL != RBLE_CPPC_Info.Cpc_Handler) {
				RBLE_CPPC_Info.Cpc_Handler( &RBLE_CPPC_Event );
			}
			break;
		case RBLE_EVT_CPP_COLLECTOR_CMD_DISALLOWED_IND:
			RBLE_CPPC_Event.type = RBLE_CPP_EVENT_COLLECTOR_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.cmd_disallowed_ind.status );
			RBLE_DATA_GET8( Data_p, RBLE_CPPC_Event.param.cmd_disallowed_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_CPPC_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_CPPC_Info.Cpc_Handler) {
				RBLE_CPPC_Info.Cpc_Handler( &RBLE_CPPC_Event );
			}
			break;
		default:
			ret = RBLE_ERR;
			break;
	}

	return ret;
}
#endif

/******************************************************************************************/
/**
 *  @brief Time Profile Commnad Result Notification
 *
 *  This function does TIP Command Result Notification to APP@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Assemble event structure
 *    -# Create Event Data
 *    -# Call Back API Call
 *
 *  @param[in]	evt_id		EVENT ID
 *  @param[in]	data_pnt	Data Address
 *
 *  @retval
 */
/******************************************************************************************/
#if (PRF_SEL_TIPS || PRF_SEL_TIPC)
RBLE_STATUS RBLE_TIP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt)
{
	RBLE_STATUS			ret = RBLE_OK;
	RBLE_TIPS_EVENT		RBLE_TIPS_Event;
	RBLE_TIPC_EVENT		RBLE_TIPC_Event;
	uint8_t				*Data_p;

	Data_p = data_pnt;
	switch (evt_id) {
		case RBLE_EVT_TIP_SERVER_ENABLE_COMP:
			RBLE_TIPS_Event.type = RBLE_TIP_EVENT_SERVER_ENABLE_COMP;
			RBLE_DATA_GET8(  Data_p, RBLE_TIPS_Event.param.server_enable.status );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPS_Event.param.server_enable.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_TIPS_Event.param.server_enable.conhdl );
			/* Call Back */
			if (NULL != RBLE_TIPS_Info.Tis_Handler) {
				RBLE_TIPS_Info.Tis_Handler( &RBLE_TIPS_Event );
			}
			break;
		case RBLE_EVT_TIP_SERVER_DISABLE_COMP:
			RBLE_TIPS_Event.type = RBLE_TIP_EVENT_SERVER_DISABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_TIPS_Event.param.server_disable.conhdl );
			RBLE_DATA_GET16( Data_p, RBLE_TIPS_Event.param.server_disable.current_time_ntf_en );
			/* Call Back */
			if (NULL != RBLE_TIPS_Info.Tis_Handler) {
				RBLE_TIPS_Info.Tis_Handler( &RBLE_TIPS_Event );
			}
			/* Handler Clear */
			RBLE_TIPS_Info.conhdl      = 0;
			RBLE_TIPS_Info.Tis_Handler = NULL;
			break;
		case RBLE_EVT_TIP_SERVER_ERROR_IND:
			RBLE_TIPS_Event.type = RBLE_TIP_EVENT_SERVER_ERROR_IND;
			RBLE_DATA_GET8(  Data_p, RBLE_TIPS_Event.param.error_ind.status );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPS_Event.param.error_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_TIPS_Event.param.error_ind.conhdl );
			/* Call Back */
			if (NULL != RBLE_TIPS_Info.Tis_Handler) {
				RBLE_TIPS_Info.Tis_Handler( &RBLE_TIPS_Event );
			}
			break;
		case RBLE_EVT_TIP_SERVER_SEND_CURRENT_TIME_COMP:
			RBLE_TIPS_Event.type = RBLE_TIP_EVENT_SERVER_SEND_CURRENT_TIME_COMP;
			RBLE_DATA_GET8(  Data_p, RBLE_TIPS_Event.param.send_current_time.status );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPS_Event.param.send_current_time.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_TIPS_Event.param.send_current_time.conhdl );
			/* Call Back */
			if (NULL != RBLE_TIPS_Info.Tis_Handler) {
				RBLE_TIPS_Info.Tis_Handler( &RBLE_TIPS_Event );
			}
			break;
		case RBLE_EVT_TIP_SERVER_WRITE_DATA_COMP:
			RBLE_TIPS_Event.type = RBLE_TIP_EVENT_SERVER_WRITE_DATA_COMP;
			RBLE_DATA_GET8(  Data_p, RBLE_TIPS_Event.param.write_data.status );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPS_Event.param.write_data.reserved );
			/* Call Back */
			if (NULL != RBLE_TIPS_Info.Tis_Handler) {
				RBLE_TIPS_Info.Tis_Handler( &RBLE_TIPS_Event );
			}
			break;
		case RBLE_EVT_TIP_SERVER_CHG_TIME_UPDATE_CP_IND:
			RBLE_TIPS_Event.type = RBLE_TIP_EVENT_SERVER_CHG_TIME_UPDATE_CP_IND;
			RBLE_DATA_GET16( Data_p, RBLE_TIPS_Event.param.chg_cp_ind.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPS_Event.param.chg_cp_ind.update_cp );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPS_Event.param.chg_cp_ind.reserved );
			/* Call Back */
			if (NULL != RBLE_TIPS_Info.Tis_Handler) {
				RBLE_TIPS_Info.Tis_Handler( &RBLE_TIPS_Event );
			}
			break;
		case RBLE_EVT_TIP_SERVER_CFG_NTF_IND:
			RBLE_TIPS_Event.type = RBLE_TIP_EVENT_SERVER_CFG_NTF_IND;
			RBLE_DATA_GET16( Data_p, RBLE_TIPS_Event.param.cfg_ntf_ind.conhdl );
			RBLE_DATA_GET16( Data_p, RBLE_TIPS_Event.param.cfg_ntf_ind.cfg_val );
			/* Call Back */
			if (NULL != RBLE_TIPS_Info.Tis_Handler) {
				RBLE_TIPS_Info.Tis_Handler( &RBLE_TIPS_Event );
			}
			break;
		case RBLE_EVT_TIP_SERVER_CMD_DISALLOWED_IND:
			RBLE_TIPS_Event.type = RBLE_TIP_EVENT_SERVER_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8(  Data_p, RBLE_TIPS_Event.param.cmd_disallowed_ind.status );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPS_Event.param.cmd_disallowed_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_TIPS_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_TIPS_Info.Tis_Handler) {
				RBLE_TIPS_Info.Tis_Handler( &RBLE_TIPS_Event );
			}
			break;
		case RBLE_EVT_TIP_CLIENT_ENABLE_COMP:
			RBLE_TIPC_Event.type = RBLE_TIP_EVENT_CLIENT_ENABLE_COMP;
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.client_enable.status );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.client_enable.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_enable.conhdl );
			/* cts */
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_enable.cts.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_enable.cts.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_enable.cts.current_time_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_enable.cts.current_time_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_enable.cts.current_time_cfg_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.client_enable.cts.current_time_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.client_enable.cts.reserved1 );
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_enable.cts.local_time_info_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_enable.cts.local_time_info_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.client_enable.cts.local_time_info_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.client_enable.cts.reserved2 );
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_enable.cts.ref_time_info_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_enable.cts.ref_time_info_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.client_enable.cts.ref_time_info_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.client_enable.cts.reserved3 );
			/* ndcs */
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_enable.ndcs.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_enable.ndcs.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_enable.ndcs.time_dst_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_enable.ndcs.time_dst_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.client_enable.ndcs.time_dst_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.client_enable.ndcs.reserved1 );
			/* rtus */
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_enable.rtus.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_enable.rtus.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_enable.rtus.update_cp_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_enable.rtus.update_cp_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.client_enable.rtus.update_cp_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.client_enable.rtus.reserved1 );
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_enable.rtus.update_state_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_enable.rtus.update_state_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.client_enable.rtus.update_state_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.client_enable.rtus.reserved2 );
			/* Call Back */
			if (NULL != RBLE_TIPC_Info.Tic_Handler) {
				RBLE_TIPC_Info.Tic_Handler( &RBLE_TIPC_Event );
			}
			break;
		case RBLE_EVT_TIP_CLIENT_DISABLE_COMP:
			RBLE_TIPC_Event.type = RBLE_TIP_EVENT_CLIENT_DISABLE_COMP;
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.client_disable.status );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.client_disable.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.client_disable.conhdl );
			/* Call Back */
			if (NULL != RBLE_TIPC_Info.Tic_Handler) {
				RBLE_TIPC_Info.Tic_Handler( &RBLE_TIPC_Event );
			}
			/* Handler Clear */
			RBLE_TIPC_Info.conhdl      = 0;
			RBLE_TIPC_Info.Tic_Handler = NULL;
			break;
		case RBLE_EVT_TIP_CLIENT_ERROR_IND:
			RBLE_TIPC_Event.type = RBLE_TIP_EVENT_CLIENT_ERROR_IND;
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.error_ind.status );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.error_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.error_ind.conhdl );
			/* Call Back */
			if (NULL != RBLE_TIPC_Info.Tic_Handler) {
				RBLE_TIPC_Info.Tic_Handler( &RBLE_TIPC_Event );
			}
			break;
		case RBLE_EVT_TIP_CLIENT_CURRENT_TIME_NTF:
			RBLE_TIPC_Event.type = RBLE_TIP_EVENT_CLIENT_CURRENT_TIME_NTF;
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.current_time_ntf.conhdl );
			/* current_time */
			/* stamp */
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.current_time_ntf.current_time.stamp.year );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.current_time_ntf.current_time.stamp.month );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.current_time_ntf.current_time.stamp.day );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.current_time_ntf.current_time.stamp.hour );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.current_time_ntf.current_time.stamp.min );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.current_time_ntf.current_time.stamp.sec );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.current_time_ntf.current_time.stamp.reserved );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.current_time_ntf.current_time.day_of_week );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.current_time_ntf.current_time.fractions256 );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.current_time_ntf.current_time.adjust_reason );
			/* Call Back */
			if (NULL != RBLE_TIPC_Info.Tic_Handler) {
				RBLE_TIPC_Info.Tic_Handler( &RBLE_TIPC_Event );
			}
			break;
		case RBLE_EVT_TIP_CLIENT_READ_CHAR_RESPONSE:
			RBLE_TIPC_Event.type = RBLE_TIP_EVENT_CLIENT_READ_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.rd_char_resp.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.rd_char_resp.att_code );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.rd_char_resp.reserved );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.rd_char_resp.data.each_len );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.rd_char_resp.data.len );
			memcpy( &RBLE_TIPC_Event.param.rd_char_resp.data.data[ 0 ], Data_p, RBLE_TIPC_Event.param.rd_char_resp.data.len );
			/* Call Back */
			if (NULL != RBLE_TIPC_Info.Tic_Handler) {
				RBLE_TIPC_Info.Tic_Handler( &RBLE_TIPC_Event );
			}
			break;
		case RBLE_EVT_TIP_CLIENT_WRITE_CHAR_RESPONSE:
			RBLE_TIPC_Event.type = RBLE_TIP_EVENT_CLIENT_WRITE_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.wr_char_resp.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.wr_char_resp.att_code );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.wr_char_resp.reserved );
			/* Call Back */
			if (NULL != RBLE_TIPC_Info.Tic_Handler) {
				RBLE_TIPC_Info.Tic_Handler( &RBLE_TIPC_Event );
			}
			break;
		case RBLE_EVT_TIP_CLIENT_CMD_DISALLOWED_IND:
			RBLE_TIPC_Event.type = RBLE_TIP_EVENT_CLIENT_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.cmd_disallowed_ind.status );
			RBLE_DATA_GET8(  Data_p, RBLE_TIPC_Event.param.cmd_disallowed_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_TIPC_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_TIPC_Info.Tic_Handler) {
				RBLE_TIPC_Info.Tic_Handler( &RBLE_TIPC_Event );
			}
			break;
		default:
			ret = RBLE_ERR;
			break;
	}

	return ret;
}
#endif

/******************************************************************************************/
/**
 *  @brief ANP Profile Commnad Result Notification
 *
 *  This function does ANP Command Result Notification to APP@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Assemble event structure
 *    -# Create Event Data
 *    -# Call Back API Call
 *
 *  @param[in]	evt_id		EVENT ID
 *  @param[in]	data_pnt	Data Address
 *
 *  @retval
 */
/******************************************************************************************/
#if (PRF_SEL_ANPS || PRF_SEL_ANPC)
RBLE_STATUS RBLE_ANP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt)
{
	RBLE_STATUS			ret = RBLE_OK;
	RBLE_ANPS_EVENT		RBLE_ANPS_Event;
	RBLE_ANPC_EVENT		RBLE_ANPC_Event;
	uint8_t				*Data_p;

	Data_p = data_pnt;
	switch (evt_id) {
		case RBLE_EVT_ANP_SERVER_ENABLE_COMP:
			RBLE_ANPS_Event.type = RBLE_ANP_EVENT_SERVER_ENABLE_COMP;
			RBLE_DATA_GET8(  Data_p, RBLE_ANPS_Event.param.server_enable.status );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPS_Event.param.server_enable.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_ANPS_Event.param.server_enable.conhdl );
			/* Call Back */
			if (NULL != RBLE_ANPS_Info.Ans_Handler) {
				RBLE_ANPS_Info.Ans_Handler( &RBLE_ANPS_Event );
			}
			break;
		case RBLE_EVT_ANP_SERVER_DISABLE_COMP:
			RBLE_ANPS_Event.type = RBLE_ANP_EVENT_SERVER_DISABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_ANPS_Event.param.server_disable.conhdl );
			RBLE_DATA_GET16( Data_p, RBLE_ANPS_Event.param.server_disable.new_alert_ntf_en );
			RBLE_DATA_GET16( Data_p, RBLE_ANPS_Event.param.server_disable.unread_alert_ntf_en );
			/* Call Back */
			if (NULL != RBLE_ANPS_Info.Ans_Handler) {
				RBLE_ANPS_Info.Ans_Handler( &RBLE_ANPS_Event );
			}
			/* Handler Clear */
			RBLE_ANPS_Info.conhdl      = 0;
			RBLE_ANPS_Info.Ans_Handler = NULL;
			break;
		case RBLE_EVT_ANP_SERVER_ERROR_IND:
			RBLE_ANPS_Event.type = RBLE_ANP_EVENT_SERVER_ERROR_IND;
			RBLE_DATA_GET8(  Data_p, RBLE_ANPS_Event.param.error_ind.status );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPS_Event.param.error_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_ANPS_Event.param.error_ind.conhdl );
			/* Call Back */
			if (NULL != RBLE_ANPS_Info.Ans_Handler) {
				RBLE_ANPS_Info.Ans_Handler( &RBLE_ANPS_Event );
			}
			break;
		case RBLE_EVT_ANP_SERVER_SEND_NEW_ALERT_COMP:
			RBLE_ANPS_Event.type = RBLE_ANP_EVENT_SERVER_SEND_NEW_ALERT_COMP;
			RBLE_DATA_GET8(  Data_p, RBLE_ANPS_Event.param.send_new_alert.status );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPS_Event.param.send_new_alert.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_ANPS_Event.param.send_new_alert.conhdl );
			/* Call Back */
			if (NULL != RBLE_ANPS_Info.Ans_Handler) {
				RBLE_ANPS_Info.Ans_Handler( &RBLE_ANPS_Event );
			}
			break;
		case RBLE_EVT_ANP_SERVER_SEND_UNREAD_ALERT_COMP:
			RBLE_ANPS_Event.type = RBLE_ANP_EVENT_SERVER_SEND_UNREAD_ALERT_COMP;
			RBLE_DATA_GET8(  Data_p, RBLE_ANPS_Event.param.send_unread_alert.status );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPS_Event.param.send_unread_alert.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_ANPS_Event.param.send_unread_alert.conhdl );
			/* Call Back */
			if (NULL != RBLE_ANPS_Info.Ans_Handler) {
				RBLE_ANPS_Info.Ans_Handler( &RBLE_ANPS_Event );
			}
			break;
		case RBLE_EVT_ANP_SERVER_CHG_ALERT_NTF_CP_IND:
			RBLE_ANPS_Event.type = RBLE_ANP_EVENT_SERVER_CHG_ALERT_NTF_CP_IND;
			RBLE_DATA_GET16( Data_p, RBLE_ANPS_Event.param.chg_cp_ind.conhdl );
			/* cp_info */
			RBLE_DATA_GET8(  Data_p, RBLE_ANPS_Event.param.chg_cp_ind.cp_info.command_id );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPS_Event.param.chg_cp_ind.cp_info.category_id );
			/* Call Back */
			if (NULL != RBLE_ANPS_Info.Ans_Handler) {
				RBLE_ANPS_Info.Ans_Handler( &RBLE_ANPS_Event );
			}
			break;
		case RBLE_EVT_ANP_SERVER_CFG_NTF_IND:
			RBLE_ANPS_Event.type = RBLE_ANP_EVENT_SERVER_CFG_NTF_IND;
			RBLE_DATA_GET16( Data_p, RBLE_ANPS_Event.param.cfg_ntf_ind.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPS_Event.param.cfg_ntf_ind.char_code );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPS_Event.param.cfg_ntf_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_ANPS_Event.param.cfg_ntf_ind.cfg_val );
			/* Call Back */
			if (NULL != RBLE_ANPS_Info.Ans_Handler) {
				RBLE_ANPS_Info.Ans_Handler( &RBLE_ANPS_Event );
			}
			break;
		case RBLE_EVT_ANP_SERVER_CMD_DISALLOWED_IND:
			RBLE_ANPS_Event.type = RBLE_ANP_EVENT_SERVER_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8(  Data_p, RBLE_ANPS_Event.param.cmd_disallowed_ind.status );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPS_Event.param.cmd_disallowed_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_ANPS_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_ANPS_Info.Ans_Handler) {
				RBLE_ANPS_Info.Ans_Handler( &RBLE_ANPS_Event );
			}
			break;
		case RBLE_EVT_ANP_CLIENT_ENABLE_COMP:
			RBLE_ANPC_Event.type = RBLE_ANP_EVENT_CLIENT_ENABLE_COMP;
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.client_enable.status );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.client_enable.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.client_enable.conhdl );
			/* ans */
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.client_enable.ans.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.client_enable.ans.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.client_enable.ans.supp_new_alert_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.client_enable.ans.supp_new_alert_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.client_enable.ans.supp_new_alert_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.client_enable.ans.reserved1 );
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.client_enable.ans.new_alert_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.client_enable.ans.new_alert_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.client_enable.ans.new_alert_cfg_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.client_enable.ans.new_alert_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.client_enable.ans.reserved2 );
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.client_enable.ans.supp_unread_alert_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.client_enable.ans.supp_unread_alert_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.client_enable.ans.supp_unread_alert_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.client_enable.ans.reserved3 );
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.client_enable.ans.unread_alert_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.client_enable.ans.unread_alert_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.client_enable.ans.unread_alert_cfg_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.client_enable.ans.unread_alert_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.client_enable.ans.reserved4 );
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.client_enable.ans.alert_ntf_cp_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.client_enable.ans.alert_ntf_cp_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.client_enable.ans.alert_ntf_cp_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.client_enable.ans.reserved5 );
			/* Call Back */
			if (NULL != RBLE_ANPC_Info.Anc_Handler) {
				RBLE_ANPC_Info.Anc_Handler( &RBLE_ANPC_Event );
			}
			break;
		case RBLE_EVT_ANP_CLIENT_DISABLE_COMP:
			RBLE_ANPC_Event.type = RBLE_ANP_EVENT_CLIENT_DISABLE_COMP;
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.client_disable.status );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.client_disable.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.client_disable.conhdl );
			/* Call Back */
			if (NULL != RBLE_ANPC_Info.Anc_Handler) {
				RBLE_ANPC_Info.Anc_Handler( &RBLE_ANPC_Event );
			}
			/* Handler Clear */
			RBLE_ANPC_Info.conhdl      = 0;
			RBLE_ANPC_Info.Anc_Handler = NULL;
			break;
		case RBLE_EVT_ANP_CLIENT_ERROR_IND:
			RBLE_ANPC_Event.type = RBLE_ANP_EVENT_CLIENT_ERROR_IND;
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.error_ind.status );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.error_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.error_ind.conhdl );
			/* Call Back */
			if (NULL != RBLE_ANPC_Info.Anc_Handler) {
				RBLE_ANPC_Info.Anc_Handler( &RBLE_ANPC_Event );
			}
			break;
		case RBLE_EVT_ANP_CLIENT_NEW_ALERT_NTF:
			RBLE_ANPC_Event.type = RBLE_ANP_EVENT_CLIENT_NEW_ALERT_NTF;
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.new_alert_ntf.conhdl );
			/* new_alert */
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.new_alert_ntf.new_alert.category_id );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.new_alert_ntf.new_alert.alert_num );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.new_alert_ntf.new_alert.text_size );
			memcpy(&RBLE_ANPC_Event.param.new_alert_ntf.new_alert.text[0], Data_p, RBLE_ANP_ALERT_TEXT_MAX);

			/* Call Back */
			if (NULL != RBLE_ANPC_Info.Anc_Handler) {
				RBLE_ANPC_Info.Anc_Handler( &RBLE_ANPC_Event );
			}
			break;
		case RBLE_EVT_ANP_CLIENT_UNREAD_ALERT_NTF:
			RBLE_ANPC_Event.type = RBLE_ANP_EVENT_CLIENT_UNREAD_ALERT_NTF;
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.unread_alert_ntf.conhdl );
			/* unread_alert */
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.unread_alert_ntf.unread_alert.category_id );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.unread_alert_ntf.unread_alert.unread_count );
			/* Call Back */
			if (NULL != RBLE_ANPC_Info.Anc_Handler) {
				RBLE_ANPC_Info.Anc_Handler( &RBLE_ANPC_Event );
			}
			break;
		case RBLE_EVT_ANP_CLIENT_READ_CHAR_RESPONSE:
			RBLE_ANPC_Event.type = RBLE_ANP_EVENT_CLIENT_READ_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.rd_char_resp.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.rd_char_resp.att_code );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.rd_char_resp.reserved );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.rd_char_resp.data.each_len );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.rd_char_resp.data.len );
			memcpy( &RBLE_ANPC_Event.param.rd_char_resp.data.data[ 0 ], Data_p, RBLE_ANPC_Event.param.rd_char_resp.data.len );
			/* Call Back */
			if (NULL != RBLE_ANPC_Info.Anc_Handler) {
				RBLE_ANPC_Info.Anc_Handler( &RBLE_ANPC_Event );
			}
			break;
		case RBLE_EVT_ANP_CLIENT_WRITE_CHAR_RESPONSE:
			RBLE_ANPC_Event.type = RBLE_ANP_EVENT_CLIENT_WRITE_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.wr_char_resp.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.wr_char_resp.att_code );
			/* Call Back */
			if (NULL != RBLE_ANPC_Info.Anc_Handler) {
				RBLE_ANPC_Info.Anc_Handler( &RBLE_ANPC_Event );
			}
			break;
		case RBLE_EVT_ANP_CLIENT_CMD_DISALLOWED_IND:
			RBLE_ANPC_Event.type = RBLE_ANP_EVENT_CLIENT_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.cmd_disallowed_ind.status );
			RBLE_DATA_GET8(  Data_p, RBLE_ANPC_Event.param.cmd_disallowed_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_ANPC_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_ANPC_Info.Anc_Handler) {
				RBLE_ANPC_Info.Anc_Handler( &RBLE_ANPC_Event );
			}
			break;
		default:
			ret = RBLE_ERR;
			break;
	}

	return ret;
}
#endif

/******************************************************************************************/
/**
 *  @brief LNP Profile Commnad Result Notification
 *
 *  This function does LNP Command Result Notification to APP@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Assemble event structure
 *    -# Create Event Data
 *    -# Call Back API Call
 *
 *  @param[in]	evt_id		EVENT ID
 *  @param[in]	data_pnt	Data Address
 *
 *  @retval
 */
/******************************************************************************************/
#if (PRF_SEL_LNPS || PRF_SEL_LNPC)
RBLE_STATUS RBLE_LNP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt)
{
	RBLE_STATUS			ret = RBLE_OK;
	RBLE_LNPS_EVENT		RBLE_LNPS_Event;
	RBLE_LNPC_EVENT		RBLE_LNPC_Event;
	uint8_t				*Data_p;

	Data_p = data_pnt;
	switch (evt_id) {
		case RBLE_EVT_LNP_SENSOR_ENABLE_COMP:
			RBLE_LNPS_Event.type = RBLE_LNP_EVENT_SENSOR_ENABLE_COMP;
			RBLE_DATA_GET8(  Data_p, RBLE_LNPS_Event.param.sensor_enable.status );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPS_Event.param.sensor_enable.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_LNPS_Event.param.sensor_enable.conhdl );
			/* Call Back */
			if (NULL != RBLE_LNPS_Info.Lns_Handler) {
				RBLE_LNPS_Info.Lns_Handler( &RBLE_LNPS_Event );
			}
			break;
		case RBLE_EVT_LNP_SENSOR_DISABLE_COMP:
			RBLE_LNPS_Event.type = RBLE_LNP_EVENT_SENSOR_DISABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_LNPS_Event.param.sensor_disable.conhdl );
			/* sensor_info */
			RBLE_DATA_GET16( Data_p, RBLE_LNPS_Event.param.sensor_disable.sensor_info.location_speed_ntf_en );
			RBLE_DATA_GET16( Data_p, RBLE_LNPS_Event.param.sensor_disable.sensor_info.ln_cp_ind_en );
			RBLE_DATA_GET16( Data_p, RBLE_LNPS_Event.param.sensor_disable.sensor_info.navigation_ntf_en );
			RBLE_DATA_GET16( Data_p, RBLE_LNPS_Event.param.sensor_disable.sensor_info.battery_level_ntf_en );
			/* Call Back */
			if (NULL != RBLE_LNPS_Info.Lns_Handler) {
				RBLE_LNPS_Info.Lns_Handler( &RBLE_LNPS_Event );
			}
			/* Handler Clear */
			RBLE_LNPS_Info.conhdl      = 0;
			RBLE_LNPS_Info.Lns_Handler = NULL;
			break;
		case RBLE_EVT_LNP_SENSOR_ERROR_IND:
			RBLE_LNPS_Event.type = RBLE_LNP_EVENT_SENSOR_ERROR_IND;
			RBLE_DATA_GET16( Data_p, RBLE_LNPS_Event.param.error_ind.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPS_Event.param.error_ind.status );
			/* Call Back */
			if (NULL != RBLE_LNPS_Info.Lns_Handler) {
				RBLE_LNPS_Info.Lns_Handler( &RBLE_LNPS_Event );
			}
			break;
		case RBLE_EVT_LNP_SENSOR_SEND_LOCATION_SPEED_COMP:
			RBLE_LNPS_Event.type = RBLE_LNP_EVENT_SENSOR_SEND_LOCATION_SPEED_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_LNPS_Event.param.send_location_speed.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPS_Event.param.send_location_speed.status );
			/* Call Back */
			if (NULL != RBLE_LNPS_Info.Lns_Handler) {
				RBLE_LNPS_Info.Lns_Handler( &RBLE_LNPS_Event );
			}
			break;
		case RBLE_EVT_LNP_SENSOR_SET_POSITION_QUALITY_COMP:
			RBLE_LNPS_Event.type = RBLE_LNP_EVENT_SENSOR_SET_POSITION_QUALITY_COMP;
			RBLE_DATA_GET8(  Data_p, RBLE_LNPS_Event.param.set_position_quality.status );
			/* Call Back */
			if (NULL != RBLE_LNPS_Info.Lns_Handler) {
				RBLE_LNPS_Info.Lns_Handler( &RBLE_LNPS_Event );
			}
			break;
		case RBLE_EVT_LNP_SENSOR_SEND_LN_CP_COMP:
			RBLE_LNPS_Event.type = RBLE_LNP_EVENT_SENSOR_SEND_LN_CP_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_LNPS_Event.param.send_ln_cp.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPS_Event.param.send_ln_cp.status );
			/* Call Back */
			if (NULL != RBLE_LNPS_Info.Lns_Handler) {
				RBLE_LNPS_Info.Lns_Handler( &RBLE_LNPS_Event );
			}
			break;
		case RBLE_EVT_LNP_SENSOR_SEND_NAVIGATION_COMP:
			RBLE_LNPS_Event.type = RBLE_LNP_EVENT_SENSOR_SEND_NAVIGATION_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_LNPS_Event.param.send_navigation.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPS_Event.param.send_navigation.status );
			/* Call Back */
			if (NULL != RBLE_LNPS_Info.Lns_Handler) {
				RBLE_LNPS_Info.Lns_Handler( &RBLE_LNPS_Event );
			}
			break;
		case RBLE_EVT_LNP_SENSOR_SEND_BATTERY_LEVEL_COMP:
			RBLE_LNPS_Event.type = RBLE_LNP_EVENT_SENSOR_SEND_BATTERY_LEVEL_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_LNPS_Event.param.send_battery_level.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPS_Event.param.send_battery_level.status );
			/* Call Back */
			if (NULL != RBLE_LNPS_Info.Lns_Handler) {
				RBLE_LNPS_Info.Lns_Handler( &RBLE_LNPS_Event );
			}
			break;
		case RBLE_EVT_LNP_SENSOR_CHG_LN_CP_IND:
			RBLE_LNPS_Event.type = RBLE_LNP_EVENT_SENSOR_CHG_LN_CP_IND;
			RBLE_DATA_GET16( Data_p, RBLE_LNPS_Event.param.chg_ln_cp_ind.conhdl );
			/* wr_cp_info */
			RBLE_DATA_GET8(  Data_p, RBLE_LNPS_Event.param.chg_ln_cp_ind.wr_cp_info.OpCode );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPS_Event.param.chg_ln_cp_ind.wr_cp_info.reserved );
			RBLE_DATA_GET32( Data_p, RBLE_LNPS_Event.param.chg_ln_cp_ind.wr_cp_info.cumulative_value );
			RBLE_DATA_GET16( Data_p, RBLE_LNPS_Event.param.chg_ln_cp_ind.wr_cp_info.content_mask );
			RBLE_DATA_GET16( Data_p, RBLE_LNPS_Event.param.chg_ln_cp_ind.wr_cp_info.route_num );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPS_Event.param.chg_ln_cp_ind.wr_cp_info.control_val );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPS_Event.param.chg_ln_cp_ind.wr_cp_info.fix_rate );
			RBLE_DATA_GET32( Data_p, RBLE_LNPS_Event.param.chg_ln_cp_ind.wr_cp_info.elevation );
			/* Call Back */
			if (NULL != RBLE_LNPS_Info.Lns_Handler) {
				RBLE_LNPS_Info.Lns_Handler( &RBLE_LNPS_Event );
			}
			break;
		case RBLE_EVT_LNP_SENSOR_CFG_INDNTF_IND:
			RBLE_LNPS_Event.type = RBLE_LNP_EVENT_SENSOR_CFG_INDNTF_IND;
			RBLE_DATA_GET16( Data_p, RBLE_LNPS_Event.param.cfg_indntf_ind.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPS_Event.param.cfg_indntf_ind.char_code );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPS_Event.param.cfg_indntf_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_LNPS_Event.param.cfg_indntf_ind.cfg_val );
			/* Call Back */
			if (NULL != RBLE_LNPS_Info.Lns_Handler) {
				RBLE_LNPS_Info.Lns_Handler( &RBLE_LNPS_Event );
			}
			break;
		case RBLE_EVT_LNP_SENSOR_CMD_DISALLOWED_IND:
			RBLE_LNPS_Event.type = RBLE_LNP_EVENT_SENSOR_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8(  Data_p, RBLE_LNPS_Event.param.cmd_disallowed_ind.status );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPS_Event.param.cmd_disallowed_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_LNPS_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_LNPS_Info.Lns_Handler) {
				RBLE_LNPS_Info.Lns_Handler( &RBLE_LNPS_Event );
			}
			break;
		case RBLE_EVT_LNP_COLLECTOR_ENABLE_COMP:
			RBLE_LNPC_Event.type = RBLE_LNP_EVENT_COLLECTOR_ENABLE_COMP;
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.status );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.conhdl );
			/* lns */
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.lns.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.lns.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.lns.ln_feature_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.lns.ln_feature_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.lns.ln_feature_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.lns.reserved1 );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.lns.location_speed_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.lns.location_speed_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.lns.location_speed_cfg_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.lns.location_speed_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.lns.reserved2 );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.lns.position_quality_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.lns.position_quality_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.lns.position_quality_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.lns.reserved3 );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.lns.ln_cp_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.lns.ln_cp_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.lns.ln_cp_cfg_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.lns.ln_cp_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.lns.reserved4 );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.lns.navigation_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.lns.navigation_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.lns.navigation_cfg_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.lns.navigation_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.lns.reserved5 );
			/* dis */
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.dis.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.dis.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.dis.sys_id_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.dis.sys_id_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.dis.sys_id_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.dis.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.dis.model_nb_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.dis.model_nb_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.dis.model_nb_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.dis.reserved2 );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.dis.serial_nb_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.dis.serial_nb_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.dis.serial_nb_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.dis.reserved3 );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.dis.fw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.dis.fw_rev_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.dis.fw_rev_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.dis.reserved4 );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.dis.hw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.dis.hw_rev_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.dis.hw_rev_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.dis.reserved5 );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.dis.sw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.dis.sw_rev_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.dis.sw_rev_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.dis.reserved6 );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.dis.manuf_name_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.dis.manuf_name_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.dis.manuf_name_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.dis.reserved7 );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.dis.ieee_certif_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.dis.ieee_certif_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.dis.ieee_certif_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.dis.reserved8 );
			/* bas */
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.bas.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.bas.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.bas.battery_lvl_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.bas.battery_lvl_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_enable.bas.battery_lvl_cfg_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.bas.battery_lvl_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_enable.bas.reserved );
			/* Call Back */
			if (NULL != RBLE_LNPC_Info.Lnc_Handler) {
				RBLE_LNPC_Info.Lnc_Handler( &RBLE_LNPC_Event );
			}
			break;
		case RBLE_EVT_LNP_COLLECTOR_DISABLE_COMP:
			RBLE_LNPC_Event.type = RBLE_LNP_EVENT_COLLECTOR_DISABLE_COMP;
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_disable.status );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.collector_disable.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.collector_disable.conhdl );
			/* Call Back */
			if (NULL != RBLE_LNPC_Info.Lnc_Handler) {
				RBLE_LNPC_Info.Lnc_Handler( &RBLE_LNPC_Event );
			}
			/* Handler Clear */
			RBLE_LNPC_Info.conhdl      = 0;
			RBLE_LNPC_Info.Lnc_Handler = NULL;
			break;
		case RBLE_EVT_LNP_COLLECTOR_ERROR_IND:
			RBLE_LNPC_Event.type = RBLE_LNP_EVENT_COLLECTOR_ERROR_IND;
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.error_ind.status );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.error_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.error_ind.conhdl );
			/* Call Back */
			if (NULL != RBLE_LNPC_Info.Lnc_Handler) {
				RBLE_LNPC_Info.Lnc_Handler( &RBLE_LNPC_Event );
			}
			break;
		case RBLE_EVT_LNP_COLLECTOR_LOCATION_SPEED_NTF:
			RBLE_LNPC_Event.type = RBLE_LNP_EVENT_COLLECTOR_LOCATION_SPEED_NTF;
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.location_speed_ntf.conhdl );
			/* location_speed_info */
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.location_speed_ntf.location_speed_info.flags );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.location_speed_ntf.location_speed_info.instant_speed );
			RBLE_DATA_GET32( Data_p, RBLE_LNPC_Event.param.location_speed_ntf.location_speed_info.total_distance );
			RBLE_DATA_GET32( Data_p, RBLE_LNPC_Event.param.location_speed_ntf.location_speed_info.latitude );
			RBLE_DATA_GET32( Data_p, RBLE_LNPC_Event.param.location_speed_ntf.location_speed_info.longitude );
			RBLE_DATA_GET32( Data_p, RBLE_LNPC_Event.param.location_speed_ntf.location_speed_info.elevation );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.location_speed_ntf.location_speed_info.heading );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.location_speed_ntf.location_speed_info.rolling_time );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.location_speed_ntf.location_speed_info.reserved );
			/* utc_time */
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.location_speed_ntf.location_speed_info.utc_time.year );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.location_speed_ntf.location_speed_info.utc_time.month );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.location_speed_ntf.location_speed_info.utc_time.day );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.location_speed_ntf.location_speed_info.utc_time.hour );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.location_speed_ntf.location_speed_info.utc_time.min );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.location_speed_ntf.location_speed_info.utc_time.sec );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.location_speed_ntf.location_speed_info.utc_time.reserved );
			/* Call Back */
			if (NULL != RBLE_LNPC_Info.Lnc_Handler) {
				RBLE_LNPC_Info.Lnc_Handler( &RBLE_LNPC_Event );
			}
			break;
		case RBLE_EVT_LNP_COLLECTOR_LN_CP_IND:
			RBLE_LNPC_Event.type = RBLE_LNP_EVENT_COLLECTOR_LN_CP_IND;
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.ln_cp_ind.conhdl );
			/* ind_cp_info */
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.ln_cp_ind.ind_cp_info.OpCode );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.ln_cp_ind.ind_cp_info.request_op_code );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.ln_cp_ind.ind_cp_info.response_value );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.ln_cp_ind.ind_cp_info.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.ln_cp_ind.ind_cp_info.route_num );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.ln_cp_ind.ind_cp_info.name_size );
			memcpy(&RBLE_LNPC_Event.param.ln_cp_ind.ind_cp_info.route_name[0], Data_p, RBLE_LNP_SENSOR_ROUTE_NAME_MAX);
			Data_p += RBLE_LNP_SENSOR_ROUTE_NAME_MAX;
			/* Call Back */
			if (NULL != RBLE_LNPC_Info.Lnc_Handler) {
				RBLE_LNPC_Info.Lnc_Handler( &RBLE_LNPC_Event );
			}
			break;
		case RBLE_EVT_LNP_COLLECTOR_NAVIGATION_NTF:
			RBLE_LNPC_Event.type = RBLE_LNP_EVENT_COLLECTOR_NAVIGATION_NTF;
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.navigation_ntf.conhdl );
			/* navigation_info */
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.navigation_ntf.navigation_info.flags );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.navigation_ntf.navigation_info.bearing );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.navigation_ntf.navigation_info.heading );
			RBLE_DATA_GET32( Data_p, RBLE_LNPC_Event.param.navigation_ntf.navigation_info.remain_dis );
			RBLE_DATA_GET32( Data_p, RBLE_LNPC_Event.param.navigation_ntf.navigation_info.remain_v_dis );
			/* estimate_time */
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.navigation_ntf.navigation_info.estimate_time.year );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.navigation_ntf.navigation_info.estimate_time.month );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.navigation_ntf.navigation_info.estimate_time.day );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.navigation_ntf.navigation_info.estimate_time.hour );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.navigation_ntf.navigation_info.estimate_time.min );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.navigation_ntf.navigation_info.estimate_time.sec );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.navigation_ntf.navigation_info.estimate_time.reserved );
			/* Call Back */
			if (NULL != RBLE_LNPC_Info.Lnc_Handler) {
				RBLE_LNPC_Info.Lnc_Handler( &RBLE_LNPC_Event );
			}
			break;
		case RBLE_EVT_LNP_COLLECTOR_BATTERY_LEVEL_NTF:
			RBLE_LNPC_Event.type = RBLE_LNP_EVENT_COLLECTOR_BATTERY_LEVEL_NTF;
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.battery_level_ntf.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.battery_level_ntf.battery_level );
			/* Call Back */
			if (NULL != RBLE_LNPC_Info.Lnc_Handler) {
				RBLE_LNPC_Info.Lnc_Handler( &RBLE_LNPC_Event );
			}
			break;
		case RBLE_EVT_LNP_COLLECTOR_READ_CHAR_RESPONSE:
			RBLE_LNPC_Event.type = RBLE_LNP_EVENT_COLLECTOR_READ_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.rd_char_resp.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.rd_char_resp.att_code );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.rd_char_resp.reserved );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.rd_char_resp.data.each_len );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.rd_char_resp.data.len );
			memcpy( &RBLE_LNPC_Event.param.rd_char_resp.data.data[ 0 ], Data_p, RBLE_LNPC_Event.param.rd_char_resp.data.len );
			/* Call Back */
			if (NULL != RBLE_LNPC_Info.Lnc_Handler) {
				RBLE_LNPC_Info.Lnc_Handler( &RBLE_LNPC_Event );
			}
			break;
		case RBLE_EVT_LNP_COLLECTOR_WRITE_CHAR_RESPONSE:
			RBLE_LNPC_Event.type = RBLE_LNP_EVENT_COLLECTOR_WRITE_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.wr_char_resp.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.wr_char_resp.att_code );
			/* Call Back */
			if (NULL != RBLE_LNPC_Info.Lnc_Handler) {
				RBLE_LNPC_Info.Lnc_Handler( &RBLE_LNPC_Event );
			}
			break;
		case RBLE_EVT_LNP_COLLECTOR_CMD_DISALLOWED_IND:
			RBLE_LNPC_Event.type = RBLE_LNP_EVENT_COLLECTOR_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.cmd_disallowed_ind.status );
			RBLE_DATA_GET8(  Data_p, RBLE_LNPC_Event.param.cmd_disallowed_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_LNPC_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_LNPC_Info.Lnc_Handler) {
				RBLE_LNPC_Info.Lnc_Handler( &RBLE_LNPC_Event );
			}
			break;
		default:
			ret = RBLE_ERR;
			break;
	}

	return ret;
}
#endif

/******************************************************************************************/
/**
 *  @brief Phone Alert Status Profile Commnad Result Notification
 *
 *  This function does PASP Command Result Notification to APP@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Assemble event structure
 *    -# Create Event Data
 *    -# Call Back API Call
 *
 *  @param[in]	evt_id		EVENT ID
 *  @param[in]	data_pnt	Data Address
 *
 *  @retval
 */
/******************************************************************************************/
#if (PRF_SEL_PASS || PRF_SEL_PASC)
RBLE_STATUS RBLE_PASP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt)
{
	RBLE_STATUS			ret = RBLE_OK;
	RBLE_PASPS_EVENT		RBLE_PASPS_Event;
	RBLE_PASPC_EVENT		RBLE_PASPC_Event;
	uint8_t				*Data_p;

	Data_p = data_pnt;
	switch (evt_id) {
		case RBLE_EVT_PASP_SERVER_ENABLE_COMP:
			RBLE_PASPS_Event.type = RBLE_PASP_EVENT_SERVER_ENABLE_COMP;
			RBLE_DATA_GET8(  Data_p, RBLE_PASPS_Event.param.server_enable.status );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPS_Event.param.server_enable.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_PASPS_Event.param.server_enable.conhdl );
			/* Call Back */
			if (NULL != RBLE_PASPS_Info.Pass_Handler) {
				RBLE_PASPS_Info.Pass_Handler( &RBLE_PASPS_Event );
			}
			break;
		case RBLE_EVT_PASP_SERVER_DISABLE_COMP:
			RBLE_PASPS_Event.type = RBLE_PASP_EVENT_SERVER_DISABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_PASPS_Event.param.server_disable.conhdl );
			/* param */
			RBLE_DATA_GET16( Data_p, RBLE_PASPS_Event.param.server_disable.server_info.alert_status_ntf_en );
			RBLE_DATA_GET16( Data_p, RBLE_PASPS_Event.param.server_disable.server_info.ringer_setting_ntf_en );
			/* Call Back */
			if (NULL != RBLE_PASPS_Info.Pass_Handler) {
				RBLE_PASPS_Info.Pass_Handler( &RBLE_PASPS_Event );
			}
			/* Handler Clear */
			RBLE_PASPS_Info.conhdl      = 0;
			RBLE_PASPS_Info.Pass_Handler = NULL;
			break;
		case RBLE_EVT_PASP_SERVER_ERROR_IND:
			RBLE_PASPS_Event.type = RBLE_PASP_EVENT_SERVER_ERROR_IND;
			RBLE_DATA_GET16( Data_p, RBLE_PASPS_Event.param.error_ind.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPS_Event.param.error_ind.status );
			/* Call Back */
			if (NULL != RBLE_PASPS_Info.Pass_Handler) {
				RBLE_PASPS_Info.Pass_Handler( &RBLE_PASPS_Event );
			}
			break;
		case RBLE_EVT_PASP_SERVER_SEND_ALERT_STATUS_COMP:
			RBLE_PASPS_Event.type = RBLE_PASP_EVENT_SERVER_SEND_ALERT_STATUS_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_PASPS_Event.param.send_alert.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPS_Event.param.send_alert.status );
			/* Call Back */
			if (NULL != RBLE_PASPS_Info.Pass_Handler) {
				RBLE_PASPS_Info.Pass_Handler( &RBLE_PASPS_Event );
			}
			break;
		case RBLE_EVT_PASP_SERVER_SEND_RINGER_SETTING_COMP:
			RBLE_PASPS_Event.type = RBLE_PASP_EVENT_SERVER_SEND_RINGER_SETTING_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_PASPS_Event.param.send_ringer.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPS_Event.param.send_ringer.status );
			/* Call Back */
			if (NULL != RBLE_PASPS_Info.Pass_Handler) {
				RBLE_PASPS_Info.Pass_Handler( &RBLE_PASPS_Event );
			}
			break;
		case RBLE_EVT_PASP_SERVER_CHG_RINGER_CP_IND:
			RBLE_PASPS_Event.type = RBLE_PASP_EVENT_SERVER_CHG_RINGER_CP_IND;
			RBLE_DATA_GET16( Data_p, RBLE_PASPS_Event.param.chg_ringer_cp_ind.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPS_Event.param.chg_ringer_cp_ind.cp_val );
			/* Call Back */
			if (NULL != RBLE_PASPS_Info.Pass_Handler) {
				RBLE_PASPS_Info.Pass_Handler( &RBLE_PASPS_Event );
			}
			break;
		case RBLE_EVT_PASP_SERVER_CFG_NTF_IND:
			RBLE_PASPS_Event.type = RBLE_PASP_EVENT_SERVER_CFG_NTF_IND;
			RBLE_DATA_GET16( Data_p, RBLE_PASPS_Event.param.cfg_ntf_ind.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPS_Event.param.cfg_ntf_ind.char_code );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPS_Event.param.cfg_ntf_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_PASPS_Event.param.cfg_ntf_ind.cfg_val );
			/* Call Back */
			if (NULL != RBLE_PASPS_Info.Pass_Handler) {
				RBLE_PASPS_Info.Pass_Handler( &RBLE_PASPS_Event );
			}
			break;
		case RBLE_EVT_PASP_SERVER_CMD_DISALLOWED_IND:
			RBLE_PASPS_Event.type = RBLE_PASP_EVENT_SERVER_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8(  Data_p, RBLE_PASPS_Event.param.cmd_disallowed_ind.status );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPS_Event.param.cmd_disallowed_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_PASPS_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_PASPS_Info.Pass_Handler) {
				RBLE_PASPS_Info.Pass_Handler( &RBLE_PASPS_Event );
			}
			break;
		case RBLE_EVT_PASP_CLIENT_ENABLE_COMP:
			RBLE_PASPC_Event.type = RBLE_PASP_EVENT_CLIENT_ENABLE_COMP;
			RBLE_DATA_GET8(  Data_p, RBLE_PASPC_Event.param.client_enable.status );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPC_Event.param.client_enable.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_PASPC_Event.param.client_enable.conhdl );
			/* pass */
			RBLE_DATA_GET16( Data_p, RBLE_PASPC_Event.param.client_enable.pass.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_PASPC_Event.param.client_enable.pass.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_PASPC_Event.param.client_enable.pass.alert_status_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_PASPC_Event.param.client_enable.pass.alert_status_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_PASPC_Event.param.client_enable.pass.alert_status_cfg_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPC_Event.param.client_enable.pass.alert_status_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPC_Event.param.client_enable.pass.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_PASPC_Event.param.client_enable.pass.ringer_setting_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_PASPC_Event.param.client_enable.pass.ringer_setting_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_PASPC_Event.param.client_enable.pass.ringer_setting_cfg_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPC_Event.param.client_enable.pass.ringer_setting_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPC_Event.param.client_enable.pass.reserved2 );
			RBLE_DATA_GET16( Data_p, RBLE_PASPC_Event.param.client_enable.pass.ringer_cp_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_PASPC_Event.param.client_enable.pass.ringer_cp_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPC_Event.param.client_enable.pass.ringer_cp_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPC_Event.param.client_enable.pass.reserved3 );
			/* Call Back */
			if (NULL != RBLE_PASPC_Info.Pasc_Handler) {
				RBLE_PASPC_Info.Pasc_Handler( &RBLE_PASPC_Event );
			}
			break;
		case RBLE_EVT_PASP_CLIENT_DISABLE_COMP:
			RBLE_PASPC_Event.type = RBLE_PASP_EVENT_CLIENT_DISABLE_COMP;
			RBLE_DATA_GET8(  Data_p, RBLE_PASPC_Event.param.client_disable.status );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPC_Event.param.client_disable.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_PASPC_Event.param.client_disable.conhdl );
			/* Call Back */
			if (NULL != RBLE_PASPC_Info.Pasc_Handler) {
				RBLE_PASPC_Info.Pasc_Handler( &RBLE_PASPC_Event );
			}
			/* Handler Clear */
			RBLE_PASPC_Info.conhdl      = 0;
			RBLE_PASPC_Info.Pasc_Handler = NULL;
			break;
		case RBLE_EVT_PASP_CLIENT_ERROR_IND:
			RBLE_PASPC_Event.type = RBLE_PASP_EVENT_CLIENT_ERROR_IND;
			RBLE_DATA_GET8(  Data_p, RBLE_PASPC_Event.param.error_ind.status );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPC_Event.param.error_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_PASPC_Event.param.error_ind.conhdl );
			/* Call Back */
			if (NULL != RBLE_PASPC_Info.Pasc_Handler) {
				RBLE_PASPC_Info.Pasc_Handler( &RBLE_PASPC_Event );
			}
			break;
		case RBLE_EVT_PASP_CLIENT_ALERT_STATUS_NTF:
			RBLE_PASPC_Event.type = RBLE_PASP_EVENT_CLIENT_ALERT_STATUS_NTF;
			RBLE_DATA_GET16( Data_p, RBLE_PASPC_Event.param.alert_ntf.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPC_Event.param.alert_ntf.alert_status );
			/* Call Back */
			if (NULL != RBLE_PASPC_Info.Pasc_Handler) {
				RBLE_PASPC_Info.Pasc_Handler( &RBLE_PASPC_Event );
			}
			break;
		case RBLE_EVT_PASP_CLIENT_RINGER_SETTING_NTF:
			RBLE_PASPC_Event.type = RBLE_PASP_EVENT_CLIENT_RINGER_SETTING_NTF;
			RBLE_DATA_GET16( Data_p, RBLE_PASPC_Event.param.ringer_ntf.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPC_Event.param.ringer_ntf.ringer_setting );
			/* Call Back */
			if (NULL != RBLE_PASPC_Info.Pasc_Handler) {
				RBLE_PASPC_Info.Pasc_Handler( &RBLE_PASPC_Event );
			}
			break;
		case RBLE_EVT_PASP_CLIENT_READ_CHAR_RESPONSE:
			RBLE_PASPC_Event.type = RBLE_PASP_EVENT_CLIENT_READ_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_PASPC_Event.param.rd_char_resp.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPC_Event.param.rd_char_resp.att_code );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPC_Event.param.rd_char_resp.data.each_len );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPC_Event.param.rd_char_resp.data.len );
			memcpy( &RBLE_PASPC_Event.param.rd_char_resp.data.data[ 0 ], Data_p, RBLE_PASPC_Event.param.rd_char_resp.data.len );
			/* Call Back */
			if (NULL != RBLE_PASPC_Info.Pasc_Handler) {
				RBLE_PASPC_Info.Pasc_Handler( &RBLE_PASPC_Event );
			}
			break;
		case RBLE_EVT_PASP_CLIENT_WRITE_CHAR_RESPONSE:
			RBLE_PASPC_Event.type = RBLE_PASP_EVENT_CLIENT_WRITE_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_PASPC_Event.param.wr_char_resp.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPC_Event.param.wr_char_resp.att_code );
			/* Call Back */
			if (NULL != RBLE_PASPC_Info.Pasc_Handler) {
				RBLE_PASPC_Info.Pasc_Handler( &RBLE_PASPC_Event );
			}
			break;
		case RBLE_EVT_PASP_CLIENT_CMD_DISALLOWED_IND:
			RBLE_PASPC_Event.type = RBLE_PASP_EVENT_CLIENT_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8(  Data_p, RBLE_PASPC_Event.param.cmd_disallowed_ind.status );
			RBLE_DATA_GET8(  Data_p, RBLE_PASPC_Event.param.cmd_disallowed_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_PASPC_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_PASPC_Info.Pasc_Handler) {
				RBLE_PASPC_Info.Pasc_Handler( &RBLE_PASPC_Event );
			}
			break;
		default:
			ret = RBLE_ERR;
			break;
	}

	return ret;
}
#endif

/******************************************************************************************/
/**
 *  @brief Running Speed and Cadence Profile Commnad Result Notification
 *
 *  This function does RSCP Command Result Notification to APP@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Assemble event structure
 *    -# Create Event Data
 *    -# Call Back API Call
 *
 *  @param[in]	evt_id		EVENT ID
 *  @param[in]	data_pnt	Data Address
 *
 *  @retval
 */
/******************************************************************************************/
#if (PRF_SEL_RSCS || PRF_SEL_RSCC)
RBLE_STATUS RBLE_RSCP_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt)
{
	RBLE_STATUS			ret = RBLE_OK;
	RBLE_RSCPS_EVENT		RBLE_RSCPS_Event;
	RBLE_RSCPC_EVENT		RBLE_RSCPC_Event;
	uint8_t				*Data_p;

	Data_p = data_pnt;
	switch (evt_id) {
		case RBLE_EVT_RSCP_SENSOR_ENABLE_COMP:
			RBLE_RSCPS_Event.type = RBLE_RSCP_EVENT_SENSOR_ENABLE_COMP;
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPS_Event.param.sensor_enable.status );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPS_Event.param.sensor_enable.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPS_Event.param.sensor_enable.conhdl );
			/* Call Back */
			if (NULL != RBLE_RSCPS_Info.Rscs_Handler) {
				RBLE_RSCPS_Info.Rscs_Handler( &RBLE_RSCPS_Event );
			}
			break;
		case RBLE_EVT_RSCP_SENSOR_DISABLE_COMP:
			RBLE_RSCPS_Event.type = RBLE_RSCP_EVENT_SENSOR_DISABLE_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_RSCPS_Event.param.sensor_disable.conhdl );
			/* sensor_info */
			RBLE_DATA_GET16( Data_p, RBLE_RSCPS_Event.param.sensor_disable.sensor_info.rsc_meas_ntf_en );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPS_Event.param.sensor_disable.sensor_info.sc_cp_ind_en );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPS_Event.param.sensor_disable.sensor_info.sensor_location );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPS_Event.param.sensor_disable.sensor_info.reserved );
			/* Call Back */
			if (NULL != RBLE_RSCPS_Info.Rscs_Handler) {
				RBLE_RSCPS_Info.Rscs_Handler( &RBLE_RSCPS_Event );
			}
			/* Handler Clear */
			RBLE_RSCPS_Info.conhdl      = 0;
			RBLE_RSCPS_Info.Rscs_Handler = NULL;
			break;
		case RBLE_EVT_RSCP_SENSOR_ERROR_IND:
			RBLE_RSCPS_Event.type = RBLE_RSCP_EVENT_SENSOR_ERROR_IND;
			RBLE_DATA_GET16( Data_p, RBLE_RSCPS_Event.param.error_ind.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPS_Event.param.error_ind.status );
			/* Call Back */
			if (NULL != RBLE_RSCPS_Info.Rscs_Handler) {
				RBLE_RSCPS_Info.Rscs_Handler( &RBLE_RSCPS_Event );
			}
			break;
		case RBLE_EVT_RSCP_SENSOR_SEND_MEASUREMENTS_COMP:
			RBLE_RSCPS_Event.type = RBLE_RSCP_EVENT_SENSOR_SEND_MEASUREMENTS_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_RSCPS_Event.param.send_measurements.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPS_Event.param.send_measurements.status );
			/* Call Back */
			if (NULL != RBLE_RSCPS_Info.Rscs_Handler) {
				RBLE_RSCPS_Info.Rscs_Handler( &RBLE_RSCPS_Event );
			}
			break;
		case RBLE_EVT_RSCP_SENSOR_SEND_SC_CP_COMP:
			RBLE_RSCPS_Event.type = RBLE_RSCP_EVENT_SENSOR_SEND_SC_CP_COMP;
			RBLE_DATA_GET16( Data_p, RBLE_RSCPS_Event.param.send_sc_cp.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPS_Event.param.send_sc_cp.status );
			/* Call Back */
			if (NULL != RBLE_RSCPS_Info.Rscs_Handler) {
				RBLE_RSCPS_Info.Rscs_Handler( &RBLE_RSCPS_Event );
			}
			break;
		case RBLE_EVT_RSCP_SENSOR_CHG_SC_CP_IND:
			RBLE_RSCPS_Event.type = RBLE_RSCP_EVENT_SENSOR_CHG_SC_CP_IND;
			RBLE_DATA_GET16( Data_p, RBLE_RSCPS_Event.param.chg_sc_cp_ind.conhdl );
			/* sc_cp_info */
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPS_Event.param.chg_sc_cp_ind.sc_cp_info.OpCode );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPS_Event.param.chg_sc_cp_ind.sc_cp_info.reserved1 );
			RBLE_DATA_GET32( Data_p, RBLE_RSCPS_Event.param.chg_sc_cp_ind.sc_cp_info.cumulative_value );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPS_Event.param.chg_sc_cp_ind.sc_cp_info.sensor_location );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPS_Event.param.chg_sc_cp_ind.sc_cp_info.request_op_code );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPS_Event.param.chg_sc_cp_ind.sc_cp_info.response_value );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPS_Event.param.chg_sc_cp_ind.sc_cp_info.reserved );
			/* Call Back */
			if (NULL != RBLE_RSCPS_Info.Rscs_Handler) {
				RBLE_RSCPS_Info.Rscs_Handler( &RBLE_RSCPS_Event );
			}
			break;
		case RBLE_EVT_RSCP_SENSOR_CFG_INDNTF_IND:
			RBLE_RSCPS_Event.type = RBLE_RSCP_EVENT_SENSOR_CFG_INDNTF_IND;
			RBLE_DATA_GET16( Data_p, RBLE_RSCPS_Event.param.cfg_indntf_ind.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPS_Event.param.cfg_indntf_ind.char_code );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPS_Event.param.cfg_indntf_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPS_Event.param.cfg_indntf_ind.cfg_val );
			/* Call Back */
			if (NULL != RBLE_RSCPS_Info.Rscs_Handler) {
				RBLE_RSCPS_Info.Rscs_Handler( &RBLE_RSCPS_Event );
			}
			break;
		case RBLE_EVT_RSCP_SENSOR_CMD_DISALLOWED_IND:
			RBLE_RSCPS_Event.type = RBLE_RSCP_EVENT_SENSOR_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPS_Event.param.cmd_disallowed_ind.status );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPS_Event.param.cmd_disallowed_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPS_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_RSCPS_Info.Rscs_Handler) {
				RBLE_RSCPS_Info.Rscs_Handler( &RBLE_RSCPS_Event );
			}
			break;
		case RBLE_EVT_RSCP_COLLECTOR_ENABLE_COMP:
			RBLE_RSCPC_Event.type = RBLE_RSCP_EVENT_COLLECTOR_ENABLE_COMP;
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.status );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.conhdl );
			/* rscs */
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.rscs.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.rscs.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.rscs.rsc_meas_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.rscs.rsc_meas_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.rscs.rsc_meas_cfg_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.rscs.rsc_meas_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.rscs.reserved1 );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.rscs.rsc_feature_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.rscs.rsc_feature_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.rscs.rsc_feature_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.rscs.reserved2 );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.rscs.sensor_loc_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.rscs.sensor_loc_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.rscs.sensor_loc_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.rscs.reserved3 );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.rscs.sc_cp_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.rscs.sc_cp_val_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.rscs.sc_cp_cfg_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.rscs.sc_cp_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.rscs.reserved4 );
			/* dis */
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.shdl );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.ehdl );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.sys_id_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.sys_id_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.sys_id_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.model_nb_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.model_nb_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.model_nb_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.reserved2 );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.serial_nb_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.serial_nb_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.serial_nb_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.reserved3 );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.fw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.fw_rev_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.fw_rev_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.reserved4 );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.hw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.hw_rev_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.hw_rev_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.reserved5 );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.sw_rev_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.sw_rev_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.sw_rev_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.reserved6 );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.manuf_name_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.manuf_name_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.manuf_name_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.reserved7 );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.ieee_certif_char_hdl );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.ieee_certif_val_hdl );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.ieee_certif_prop );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_enable.dis.reserved8 );
			/* Call Back */
			if (NULL != RBLE_RSCPC_Info.Rscc_Handler) {
				RBLE_RSCPC_Info.Rscc_Handler( &RBLE_RSCPC_Event );
			}
			break;
		case RBLE_EVT_RSCP_COLLECTOR_DISABLE_COMP:
			RBLE_RSCPC_Event.type = RBLE_RSCP_EVENT_COLLECTOR_DISABLE_COMP;
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_disable.status );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.collector_disable.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.collector_disable.conhdl );
			/* Call Back */
			if (NULL != RBLE_RSCPC_Info.Rscc_Handler) {
				RBLE_RSCPC_Info.Rscc_Handler( &RBLE_RSCPC_Event );
			}
			/* Handler Clear */
			RBLE_RSCPC_Info.conhdl      = 0;
			RBLE_RSCPC_Info.Rscc_Handler = NULL;
			break;
		case RBLE_EVT_RSCP_COLLECTOR_ERROR_IND:
			RBLE_RSCPC_Event.type = RBLE_RSCP_EVENT_COLLECTOR_ERROR_IND;
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.error_ind.status );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.error_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.error_ind.conhdl );
			/* Call Back */
			if (NULL != RBLE_RSCPC_Info.Rscc_Handler) {
				RBLE_RSCPC_Info.Rscc_Handler( &RBLE_RSCPC_Event );
			}
			break;
		case RBLE_EVT_RSCP_COLLECTOR_MEASUREMENTS_NTF:
			RBLE_RSCPC_Event.type = RBLE_RSCP_EVENT_COLLECTOR_MEASUREMENTS_NTF;
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.measurements_ntf.conhdl );
			/* measure_info */
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.measurements_ntf.measure_info.flags );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.measurements_ntf.measure_info.instant_cadence );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.measurements_ntf.measure_info.instant_speed );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.measurements_ntf.measure_info.instant_stride_len );
			RBLE_DATA_GET32( Data_p, RBLE_RSCPC_Event.param.measurements_ntf.measure_info.total_distance );
			/* Call Back */
			if (NULL != RBLE_RSCPC_Info.Rscc_Handler) {
				RBLE_RSCPC_Info.Rscc_Handler( &RBLE_RSCPC_Event );
			}
			break;
		case RBLE_EVT_RSCP_COLLECTOR_SC_CP_IND:
			RBLE_RSCPC_Event.type = RBLE_RSCP_EVENT_COLLECTOR_SC_CP_IND;
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.sc_cp_ind.conhdl );
			/* sc_cp_info */
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.sc_cp_ind.sc_cp_info.OpCode );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.sc_cp_ind.sc_cp_info.reserved1 );
			RBLE_DATA_GET32( Data_p, RBLE_RSCPC_Event.param.sc_cp_ind.sc_cp_info.cumulative_value );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.sc_cp_ind.sc_cp_info.sensor_location );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.sc_cp_ind.sc_cp_info.request_op_code );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.sc_cp_ind.sc_cp_info.response_value );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.sc_cp_ind.sc_cp_info.reserved );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.sc_cp_ind.location_num );
			memcpy(&RBLE_RSCPC_Event.param.sc_cp_ind.response_param[0], Data_p, RBLE_RSCP_SENSORE_LOCATION_MAX);
			Data_p += RBLE_RSCP_SENSORE_LOCATION_MAX;
			/* Call Back */
			if (NULL != RBLE_RSCPC_Info.Rscc_Handler) {
				RBLE_RSCPC_Info.Rscc_Handler( &RBLE_RSCPC_Event );
			}
			break;
		case RBLE_EVT_RSCP_COLLECTOR_READ_CHAR_RESPONSE:
			RBLE_RSCPC_Event.type = RBLE_RSCP_EVENT_COLLECTOR_READ_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.rd_char_resp.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.rd_char_resp.att_code );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.rd_char_resp.reserved );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.rd_char_resp.data.each_len );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.rd_char_resp.data.len );
			memcpy( &RBLE_RSCPC_Event.param.rd_char_resp.data.data[ 0 ], Data_p, RBLE_RSCPC_Event.param.rd_char_resp.data.len );
			/* Call Back */
			if (NULL != RBLE_RSCPC_Info.Rscc_Handler) {
				RBLE_RSCPC_Info.Rscc_Handler( &RBLE_RSCPC_Event );
			}
			break;
		case RBLE_EVT_RSCP_COLLECTOR_WRITE_CHAR_RESPONSE:
			RBLE_RSCPC_Event.type = RBLE_RSCP_EVENT_COLLECTOR_WRITE_CHAR_RESPONSE;
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.wr_char_resp.conhdl );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.wr_char_resp.att_code );
			/* Call Back */
			if (NULL != RBLE_RSCPC_Info.Rscc_Handler) {
				RBLE_RSCPC_Info.Rscc_Handler( &RBLE_RSCPC_Event );
			}
			break;
		case RBLE_EVT_RSCP_COLLECTOR_CMD_DISALLOWED_IND:
			RBLE_RSCPC_Event.type = RBLE_RSCP_EVENT_COLLECTOR_COMMAND_DISALLOWED_IND;
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.cmd_disallowed_ind.status );
			RBLE_DATA_GET8(  Data_p, RBLE_RSCPC_Event.param.cmd_disallowed_ind.reserved );
			RBLE_DATA_GET16( Data_p, RBLE_RSCPC_Event.param.cmd_disallowed_ind.opcode );
			/* Call Back */
			if (NULL != RBLE_RSCPC_Info.Rscc_Handler) {
				RBLE_RSCPC_Info.Rscc_Handler( &RBLE_RSCPC_Event );
			}
			break;
		default:
			ret = RBLE_ERR;
			break;
	}

	return ret;
}
#endif

/******************************************************************************************/
/**
 *  @brief Vendor Specific Commnad Result Notification
 *
 *  This function does Vendor Specific Command Result Notification to APP@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Event Data
 *    -# Call Back API Call
 *
 *  @param[in]	evt_id		EVENT ID
 *  @param[in]	data_pnt	Data Address
 *
 *  @retval
 */
/******************************************************************************************/
RBLE_STATUS RBLE_VS_Command_Result(RBLE_EVENT_NO evt_id, uint8_t *data_pnt)
{
	RBLE_STATUS			ret = RBLE_OK;
	RBLE_VS_EVENT		RBLE_VS_Event;

	uint8_t			*Data_p;

	if( NULL != RBLE_VS_Info.Handler){
		Data_p = data_pnt;
		switch(evt_id){
			case RBLE_EVT_VS_TEST_RX_START_COMP:
				RBLE_VS_Event.type = RBLE_VS_EVENT_TEST_RX_START_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.status );
				break;
			case RBLE_EVT_VS_TEST_TX_START_COMP:
				RBLE_VS_Event.type = RBLE_VS_EVENT_TEST_TX_START_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.status );
				break;
			case RBLE_EVT_VS_TEST_END_COMP:
				RBLE_VS_Event.type = RBLE_VS_EVENT_TEST_END_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.test_end_cmp.status );
				RBLE_DUMMY_GET8( Data_p );
				RBLE_DATA_GET16( Data_p, RBLE_VS_Event.param.test_end_cmp.nb_packet_received );
				break;
			case RBLE_EVT_VS_WR_BD_ADDR_COMP:
				RBLE_VS_Event.type = RBLE_VS_EVENT_WR_BD_ADDR_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.status );
				break;
			case RBLE_EVT_VS_SET_TEST_PARAM_COMP:
				RBLE_VS_Event.type = RBLE_VS_EVENT_SET_TEST_PARAM_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.status );
				break;
			case RBLE_EVT_VS_READ_TEST_RSSI_COMP:
				RBLE_VS_Event.type = RBLE_VS_EVENT_READ_TEST_RSSI_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.test_rssi_cmp.status );
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.test_rssi_cmp.rssi );
				break;
			case RBLE_EVT_VS_GPIO_DIR_COMP:
				RBLE_VS_Event.type = RBLE_VS_EVENT_GPIO_DIR_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.gpio_dir_cmp.status );
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.gpio_dir_cmp.mask );
				break;
			case RBLE_EVT_VS_GPIO_ACCESS_COMP:
				RBLE_VS_Event.type = RBLE_VS_EVENT_GPIO_ACCESS_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.gpio_access_cmp.status );
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.gpio_access_cmp.value );
				break;
			case RBLE_EVT_VS_FLASH_MANAGEMENT_COMP:
				RBLE_VS_Event.type = RBLE_VS_EVENT_FLASH_MANAGEMENT_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.management_comp.status );
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.management_comp.cmd );
				break;
			case RBLE_EVT_VS_FLASH_ACCESS_COMP:
				RBLE_VS_Event.type = RBLE_VS_EVENT_FLASH_ACCESS_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.access_comp.status );
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.access_comp.cmd );
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.access_comp.id );
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.access_comp.size );
				RBLE_VS_Event.param.access_comp.addr = access_buf;
				access_buf = NULL;
				
				if( RBLE_VS_Event.param.access_comp.cmd == RBLE_VS_FLASH_CMD_READ ) {
					memcpy( RBLE_VS_Event.param.access_comp.addr, Data_p, RBLE_VS_Event.param.access_comp.size );
				}
				break;
			case RBLE_EVT_VS_FLASH_OPERATION_COMP:
				RBLE_VS_Event.type = RBLE_VS_EVENT_FLASH_OPERATION_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.operation_comp.status );
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.operation_comp.cmd );
				break;
			case RBLE_EVT_VS_FLASH_GET_SPACE_COMP:
				RBLE_VS_Event.type = RBLE_VS_EVENT_FLASH_GET_SPACE_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.get_space.status );
				RBLE_DUMMY_GET8( Data_p );
				RBLE_DATA_GET16( Data_p, RBLE_VS_Event.param.get_space.size );
				break;
			case RBLE_EVT_VS_FLASH_GET_EEL_VER_COMP:
				RBLE_VS_Event.type = RBLE_VS_EVENT_FLASH_GET_EEL_VER_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.get_eel_ver.status );
				memcpy( RBLE_VS_Event.param.get_eel_ver.version, Data_p, sizeof(uint8_t)*24 );
				break;
			case RBLE_EVT_VS_ADAPT_ENABLE_COMP:
				RBLE_VS_Event.type = RBLE_VS_EVENT_ADAPT_ENABLE_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.adapt_enable_cmp.status );
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.adapt_enable_cmp.cmd );
				break;
			case RBLE_EVT_VS_ADAPT_STATE_IND:
				RBLE_VS_Event.type = RBLE_VS_EVENT_ADAPT_STATE_IND;
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.adapt_state_ind.state );
				break;
			case RBLE_EVT_VS_CMD_DISALLOWED_IND:
				RBLE_VS_Event.type = RBLE_VS_EVENT_COMMAND_DISALLOWED_IND;
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.cmd_disallowed_ind.status );
				RBLE_DUMMY_GET8( Data_p );
				RBLE_DATA_GET16( Data_p, RBLE_VS_Event.param.cmd_disallowed_ind.opcode );
				break;
			case RBLE_EVT_VS_SET_TX_POWER_IND:
				RBLE_VS_Event.type = RBLE_VS_EVENT_SET_TX_POWER_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.status );
				break;
			case RBLE_EVT_VS_SET_PARAMS_COMP:
				RBLE_VS_Event.type = RBLE_VS_EVENT_SET_PARAMS_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.status );
				break;
			case RBLE_EVT_VS_RF_CONTROL_COMP:
				RBLE_VS_Event.type = RBLE_VS_EVENT_RF_CONTROL_COMP;
				RBLE_DATA_GET8( Data_p, RBLE_VS_Event.param.rf_control_cmp.status );
				break;
			default:
				ret = RBLE_ERR;
				break;
		}

		if(RBLE_OK == ret){
			/* Call Back */
			RBLE_VS_Info.Handler(&RBLE_VS_Event);
		}
	}
	
	return ret;
}

#ifdef RBLE_DEBUG
void rBLE_Data_Disp(RBLE_HEADER_t *pnt)
{
	uint8_t cnt,sub_cnt;

	if(pnt->packet_type == RBLE_PACKET_TYPE_EVENT){
		puts( "\n**** rBLE Recive Event Data ***\n\n");
	}else if(pnt->packet_type == RBLE_PACKET_TYPE_COMMAND){
		puts( "\n**** rBLE Recive Command Data ***\n\n");
	}else{
		puts( "\n**** Un-Known Packet ***\n\n");
	}

	printf( "[Packet Type ]  : 0x%x \n", pnt->packet_type);
	printf( "[Packet Length] : 0x%x \n", pnt->packet_length);

	if(pnt->packet_type == RBLE_PACKET_TYPE_EVENT){
		rBLE_Change_Event_Code( (RBLE_EVENT_NO)pnt->opcode);		/* Event Acquisition */
	}else if(pnt->packet_type == RBLE_PACKET_TYPE_COMMAND){
		rBLE_Change_Command_Code( (RBLE_COMMAND_NO)pnt->opcode);	/* Command Acquisition */
	}else{
		puts( "\n**** Un-Known Packet ***\n\n");
	}
	
	printf( "[Opecode]       : 0x%x \n", pnt->opcode);
	if(pnt->packet_length == 0){
		puts("[Data]          : None\n\n" );
	}else{
		puts( "[Data]\n" );
		puts( "|0x00|0x01|0x02|0x03|0x04|0x05|0x06|0x07|0x08|0x09|0x0A|0x0B|0x0C|0x0D|0x0E|0x0F|\n" );
		puts( "|-------------------------------------------------------------------------------|\n" );
		puts(" " );
	
		for(cnt=0,sub_cnt=1;cnt < pnt->packet_length;cnt++,sub_cnt++){
			printf( "0x" );
			if(pnt->data[cnt] < 0x10){
				puts( "0" );
			}

			if(sub_cnt == 16){
				printf( "%x\n",pnt->data[cnt]);
				puts(" ");
				sub_cnt = 0;
			}else if(cnt == ( (pnt->packet_length) - 1 )){
				printf( "%x",pnt->data[cnt]);
			}else{
				printf( "%x,",pnt->data[cnt]);
			}
		}
		puts( "\n\n" );
	}
}

void rBLE_Change_Command_Code(RBLE_COMMAND_NO	opcode)
{
	char_t *str;

	switch(opcode){
		case RBLE_CMD_GAP_RESET:
			str = "RBLE_CMD_GAP_RESET";
			break;
		case RBLE_CMD_GAP_SET_NAME:
			str = "RBLE_CMD_GAP_SET_NAME";
			break;
		case RBLE_CMD_GAP_OBSERVATION_ENABLE:
			str = "RBLE_CMD_GAP_OBSERVATION_ENABLE";
			break;
		case RBLE_CMD_GAP_OBSERVATION_DISABLE:
			str = "RBLE_CMD_GAP_OBSERVATION_DISABLE";
			break;
		case RBLE_CMD_GAP_BROADCAST_ENABLE:
			str = "RBLE_CMD_GAP_BROADCAST_ENABLE";
			break;
		case RBLE_CMD_GAP_BROADCAST_DISABLE:
			str = "RBLE_CMD_GAP_BROADCAST_DISABLE";
			break;
		case RBLE_CMD_GAP_SET_BONDING_MODE:
			str = "RBLE_CMD_GAP_SET_BONDING_MODE";
			break;
		case RBLE_CMD_GAP_SET_SECURITY_REQUEST:
			str = "RBLE_CMD_GAP_SET_SECURITY_REQUEST";
			break;
		case RBLE_CMD_GAP_GET_DEVICE_INFO:
			str = "RBLE_CMD_GAP_GET_DEVICE_INFO";
			break;
		case RBLE_CMD_GAP_GET_WHITE_LIST_SIZE:
			str = "RBLE_CMD_GAP_GET_WHITE_LIST_SIZE";
			break;
		case RBLE_CMD_GAP_ADD_TO_WHITE_LIST:
			str = "RBLE_CMD_GAP_ADD_TO_WHITE_LIST";
			break;
		case RBLE_CMD_GAP_DEL_FROM_WHITE_LIST:
			str = "RBLE_CMD_GAP_DEL_FROM_WHITE_LIST";
			break;
		case RBLE_CMD_GAP_GET_REMOTE_DEVICE_NAME:
			str = "RBLE_CMD_GAP_GET_REMOTE_DEVICE_NAME";
			break;
		case RBLE_CMD_GAP_GET_REMOTE_DEVICE_INFO:
			str = "RBLE_CMD_GAP_GET_REMOTE_DEVICE_INFO";
			break;
		case RBLE_CMD_GAP_DEVICE_SEARCH:
			str = "RBLE_CMD_GAP_DEVICE_SEARCH";
			break;
//		case RBLE_CMD_GAP_SET_RECONNECT_ADDRESS:
//			str = "RBLE_CMD_GAP_SET_RECONNECT_ADDRESS";
//			break;
		case RBLE_CMD_GAP_SET_RANDOM_ADDRESS:
			str = "RBLE_CMD_GAP_SET_RANDOM_ADDRESS";
			break;
		case RBLE_CMD_GAP_SET_PRIVACY_FEATURE:
			str = "RBLE_CMD_GAP_SET_PRIVACY_FEATURE";
			break;
//		case RBLE_CMD_GAP_SET_PERIPHERAL_PRIVACY_FEATURE:
//			str = "RBLE_CMD_GAP_SET_PERIPHERAL_PRIVACY_FEATURE";
//			break;
		case RBLE_CMD_GAP_CREATE_CONNECTION:
			str = "RBLE_CMD_GAP_CREATE_CONNECTION";
			break;
		case RBLE_CMD_GAP_CONNECTION_CANCEL:
			str = "RBLE_CMD_GAP_CONNECTION_CANCEL";
			break;
		case RBLE_CMD_GAP_DISCONNECT:
			str = "RBLE_CMD_GAP_DISCONNECT";
			break;
		case RBLE_CMD_GAP_START_BONDING:
			str = "RBLE_CMD_GAP_START_BONDING";
			break;
		case RBLE_CMD_GAP_BONDING_INFO_IND:
			str = "RBLE_CMD_GAP_BONDING_INFO_IND";
			break;
		case RBLE_CMD_GAP_BONDING_RESPONSE:
			str = "RBLE_CMD_GAP_BONDING_RESPONSE";
			break;
		case RBLE_CMD_GAP_CHANGE_CONNECTION_PARAM:
			str = "RBLE_CMD_GAP_CHANGE_CONNECTION_PARAM";
			break;
		case RBLE_CMD_GAP_CHANNEL_MAP_REQ:
			str = "RBLE_CMD_GAP_CHANNEL_MAP_REQ";
			break;
//		case RBLE_CMD_GAP_GET_ATTRIBUTE_HANDLE:
//			str = "RBLE_CMD_GAP_GET_ATTRIBUTE_HANDLE";
//			break;
		case RBLE_CMD_SM_SET_KEY:
			str = "RBLE_CMD_SM_SET_KEY";
			break;
		case RBLE_CMD_GATT_ENABLE:
			str = "RBLE_CMD_GATT_ENABLE";
			break;
		case RBLE_CMD_GATT_DISCOVERY_CHAR_REQUEST:
			str = "RBLE_CMD_GATT_DISCOVERY_CHAR_REQUEST";
			break;
		case RBLE_CMD_FMP_TARGET_ENABLE:
			str = "RBLE_CMD_FMP_TARGET_ENABLE";
			break;
		case RBLE_CMD_FMP_TARGET_DISABLE:
			str = "RBLE_CMD_FMP_TARGET_DISABLE";
			break;
		case RBLE_CMD_FMP_LOCATOR_ENABLE:
			str = "RBLE_CMD_FMP_LOCATOR_ENABLE";
			break;
		case RBLE_CMD_FMP_LOCATOR_DISABLE:
			str = "RBLE_CMD_FMP_LOCATOR_DISABLE";
			break;
		case RBLE_CMD_FMP_LOCATOR_SET_ALERT:
			str = "RBLE_CMD_FMP_LOCATOR_SET_ALERT";
			break;
		case RBLE_CMD_PXP_REPORTER_ENABLE:
			str = "RBLE_CMD_PXP_REPORTER_ENABLE";
			break;
		case RBLE_CMD_PXP_REPORTER_DISABLE:
			str = "RBLE_CMD_PXP_REPORTER_DISABLE";
			break;
		case RBLE_CMD_PXP_MONITOR_ENABLE:
			str = "RBLE_CMD_PXP_MONITOR_ENABLE";
			break;
		case RBLE_CMD_PXP_MONITOR_DISABLE:
			str = "RBLE_CMD_PXP_MONITOR_DISABLE";
			break;
		case RBLE_CMD_PXP_MONITOR_GET_ALERT_LEVEL:
			str = "RBLE_CMD_PXP_MONITOR_GET_ALERT_LEVEL";
			break;
		case RBLE_CMD_PXP_MONITOR_SET_ALERT_LEVEL:
			str = "RBLE_CMD_PXP_MONITOR_SET_ALERT_LEVEL";
			break;
		case RBLE_CMD_PXP_MONITOR_GET_TX_POWER:
			str = "RBLE_CMD_PXP_MONITOR_GET_TX_POWER";
			break;
		case RBLE_CMD_HTP_THERMOMETER_ENABLE:
			str = "RBLE_CMD_HTP_THERMOMETER_ENABLE";
			break;
		case RBLE_CMD_HTP_THERMOMETER_DISABLE:
			str = "RBLE_CMD_HTP_THERMOMETER_DISABLE";
			break;
		case RBLE_CMD_HTP_THERMOMETER_SEND_TEMP:
			str = "RBLE_CMD_HTP_THERMOMETER_SEND_TEMP";
			break;
		case RBLE_CMD_HTP_THERMOMETER_REQ_MEASUREMENT_PERIOD_IND:
			str = "RBLE_CMD_HTP_THERMOMETER_REQ_MEASUREMENT_PERIOD_IND";
			break;
		case RBLE_CMD_HTP_COLLECTOR_ENABLE:
			str = "RBLE_CMD_HTP_COLLECTOR_ENABLE";
			break;
		case RBLE_CMD_HTP_COLLECTOR_DISABLE:
			str = "RBLE_CMD_HTP_COLLECTOR_DISABLE";
			break;
		case RBLE_CMD_HTP_COLLECTOR_SET_MEASUREMENT_PERIOD:
			str = "RBLE_CMD_HTP_COLLECTOR_SET_MEASUREMENT_PERIOD";
			break;
		case RBLE_CMD_BLP_SENSOR_ENABLE:
			str = "RBLE_CMD_BLP_SENSOR_ENABLE";
			break;
		case RBLE_CMD_BLP_SENSOR_DISABLE:
			str = "RBLE_CMD_BLP_SENSOR_DISABLE";
			break;
		case RBLE_CMD_BLP_SENSOR_SEND_MEASUREMENTS:
			str = "RBLE_CMD_BLP_SENSOR_SEND_MEASUREMENTS";
			break;
		case RBLE_CMD_BLP_COLLECTOR_ENABLE:
			str = "RBLE_CMD_BLP_COLLECTOR_ENABLE";
			break;
		case RBLE_CMD_BLP_COLLECTOR_DISABLE:
			str = "RBLE_CMD_BLP_COLLECTOR_DISABLE";
			break;
		case RBLE_CMD_BLP_COLLECTOR_READ_CHAR:
			str = "RBLE_CMD_BLP_COLLECTOR_READ_CHAR";
			break;
		case RBLE_CMD_BLP_COLLECTOR_WRITE_CHAR:
			str = "RBLE_CMD_BLP_COLLECTOR_WRITE_CHAR";
			break;
		case RBLE_CMD_HRP_SENSOR_ENABLE:
			str = "RBLE_CMD_HRP_SENSOR_ENABLE";
			break;
		case RBLE_CMD_HRP_SENSOR_DISABLE:
			str = "RBLE_CMD_HRP_SENSOR_DISABLE";
			break;
		case RBLE_CMD_HRP_SENSOR_SEND_MEASUREMENTS:
			str = "RBLE_CMD_HRP_SENSOR_SEND_MEASUREMENTS";
			break;
		case RBLE_CMD_HRP_COLLECTOR_ENABLE:
			str = "RBLE_CMD_HRP_COLLECTOR_ENABLE";
			break;
		case RBLE_CMD_HRP_COLLECTOR_DISABLE:
			str = "RBLE_CMD_HRP_COLLECTOR_DISABLE";
			break;
		case RBLE_CMD_HRP_COLLECTOR_READ_CHAR:
			str = "RBLE_CMD_HRP_COLLECTOR_READ_CHAR";
			break;
		case RBLE_CMD_HRP_COLLECTOR_WRITE_CONTROL_POINT:
			str = "RBLE_CMD_HRP_COLLECTOR_WRITE_CONTROL_POINT";
			break;
		case RBLE_CMD_HRP_COLLECTOR_WRITE_CHAR:
			str = "RBLE_CMD_HRP_COLLECTOR_WRITE_CHAR";
			break;
		case RBLE_CMD_CSCP_SENSOR_ENABLE:
			str = "RBLE_CMD_CSCP_SENSOR_ENABLE";
			break;
		case RBLE_CMD_CSCP_SENSOR_DISABLE:
			str = "RBLE_CMD_CSCP_SENSOR_DISABLE";
			break;
		case RBLE_CMD_CSCP_SENSOR_SEND_MEASUREMENTS:
			str = "RBLE_CMD_CSCP_SENSOR_SEND_MEASUREMENTS";
			break;
		case RBLE_CMD_CSCP_SENSOR_SEND_SC_CONTROL_POINT:
			str = "RBLE_CMD_CSCP_SENSOR_SEND_SC_CONTROL_POINT";
			break;
		case RBLE_CMD_CSCP_COLLECTOR_ENABLE:
			str = "RBLE_CMD_CSCP_COLLECTOR_ENABLE";
			break;
		case RBLE_CMD_CSCP_COLLECTOR_DISABLE:
			str = "RBLE_CMD_CSCP_COLLECTOR_DISABLE";
			break;
		case RBLE_CMD_CSCP_COLLECTOR_READ_CHAR:
			str = "RBLE_CMD_CSCP_COLLECTOR_READ_CHAR";
			break;
		case RBLE_CMD_CSCP_COLLECTOR_WRITE_SC_CONTROL_POINT:
			str = "RBLE_CMD_CSCP_COLLECTOR_WRITE_SC_CONTROL_POINT";
			break;
		case RBLE_CMD_CSCP_COLLECTOR_WRITE_CHAR:
			str = "RBLE_CMD_CSCP_COLLECTOR_WRITE_CHAR";
			break;
		case RBLE_CMD_CPP_SENSOR_ENABLE:
			str = "RBLE_CMD_CPP_SENSOR_ENABLE";
			break;
		case RBLE_CMD_CPP_SENSOR_DISABLE:
			str = "RBLE_CMD_CPP_SENSOR_DISABLE";
			break;
		case RBLE_CMD_CPP_SENSOR_SEND_MEASUREMENTS:
			str = "RBLE_CMD_CPP_SENSOR_SEND_MEASUREMENTS";
			break;
		case RBLE_CMD_CPP_SENSOR_BROADCAST_MEASUREMENTS:
			str = "RBLE_CMD_CPP_SENSOR_BROADCAST_MEASUREMENTS";
			break;
		case RBLE_CMD_CPP_SENSOR_SEND_VECTOR:
			str = "RBLE_CMD_CPP_SENSOR_SEND_VECTOR";
			break;
		case RBLE_CMD_CPP_SENSOR_SEND_CONTROL_POINT:
			str = "RBLE_CMD_CPP_SENSOR_SEND_CONTROL_POINT";
			break;
		case RBLE_CMD_CPP_SENSOR_SEND_BATTERY_LEVEL:
			str = "RBLE_CMD_CPP_SENSOR_SEND_BATTERY_LEVEL";
			break;
		case RBLE_CMD_CPP_SENSOR_SEND_WRITE_RESPONSE:
			str = "RBLE_CMD_CPP_SENSOR_SEND_WRITE_RESPONSE";
			break;
		case RBLE_CMD_CPP_COLLECTOR_ENABLE:
			str = "RBLE_CMD_CPP_COLLECTOR_ENABLE";
			break;
		case RBLE_CMD_CPP_COLLECTOR_DISABLE:
			str = "RBLE_CMD_CPP_COLLECTOR_DISABLE";
			break;
		case RBLE_CMD_CPP_COLLECTOR_READ_CHAR:
			str = "RBLE_CMD_CPP_COLLECTOR_READ_CHAR";
			break;
		case RBLE_CMD_CPP_COLLECTOR_WRITE_CONTROL_POINT:
			str = "RBLE_CMD_CPP_COLLECTOR_WRITE_CONTROL_POINT";
			break;
		case RBLE_CMD_CPP_COLLECTOR_WRITE_CHAR:
			str = "RBLE_CMD_CPP_COLLECTOR_WRITE_CHAR";
			break;
		case RBLE_CMD_TIP_SERVER_ENABLE:
			str = "RBLE_CMD_TIP_SERVER_ENABLE";
			break;
		case RBLE_CMD_TIP_SERVER_DISABLE:
			str = "RBLE_CMD_TIP_SERVER_DISABLE";
			break;
		case RBLE_CMD_TIP_SERVER_SEND_CURRENT_TIME:
			str = "RBLE_CMD_TIP_SERVER_SEND_CURRENT_TIME";
			break;
		case RBLE_CMD_TIP_SERVER_WRITE_DATA:
			str = "RBLE_CMD_TIP_SERVER_WRITE_DATA";
			break;
		case RBLE_CMD_TIP_CLIENT_ENABLE:
			str = "RBLE_CMD_TIP_CLIENT_ENABLE";
			break;
		case RBLE_CMD_TIP_CLIENT_DISABLE:
			str = "RBLE_CMD_TIP_CLIENT_DISABLE";
			break;
		case RBLE_CMD_TIP_CLIENT_READ_CHAR:
			str = "RBLE_CMD_TIP_CLIENT_READ_CHAR";
			break;
		case RBLE_CMD_TIP_CLIENT_WRITE_CHAR:
			str = "RBLE_CMD_TIP_CLIENT_WRITE_CHAR";
			break;
		case RBLE_CMD_TIP_CLIENT_WRITE_TIME_UPDATE_CP:
			str = "RBLE_CMD_TIP_CLIENT_WRITE_TIME_UPDATE_CP";
			break;
		case RBLE_CMD_ANP_SERVER_ENABLE:
			str = "RBLE_CMD_ANP_SERVER_ENABLE";
			break;
		case RBLE_CMD_ANP_SERVER_DISABLE:
			str = "RBLE_CMD_ANP_SERVER_DISABLE";
			break;
		case RBLE_CMD_ANP_SERVER_SEND_NEW_ALERT:
			str = "RBLE_CMD_ANP_SERVER_SEND_NEW_ALERT";
			break;
		case RBLE_CMD_ANP_SERVER_SEND_UNREAD_ALERT:
			str = "RBLE_CMD_ANP_SERVER_SEND_UNREAD_ALERT";
			break;
		case RBLE_CMD_ANP_CLIENT_ENABLE:
			str = "RBLE_CMD_ANP_CLIENT_ENABLE";
			break;
		case RBLE_CMD_ANP_CLIENT_DISABLE:
			str = "RBLE_CMD_ANP_CLIENT_DISABLE";
			break;
		case RBLE_CMD_ANP_CLIENT_READ_CHAR:
			str = "RBLE_CMD_ANP_CLIENT_READ_CHAR";
			break;
		case RBLE_CMD_ANP_CLIENT_WRITE_ALERT_NOTIFICATION_CP:
			str = "RBLE_CMD_ANP_CLIENT_WRITE_ALERT_NOTIFICATION_CP";
			break;
		case RBLE_CMD_ANP_CLIENT_WRITE_CHAR:
			str = "RBLE_CMD_ANP_CLIENT_WRITE_CHAR";
			break;
		case RBLE_CMD_LNP_SENSOR_ENABLE:
			str = "RBLE_CMD_LNP_SENSOR_ENABLE";
			break;
		case RBLE_CMD_LNP_SENSOR_DISABLE:
			str = "RBLE_CMD_LNP_SENSOR_DISABLE";
			break;
		case RBLE_CMD_LNP_SENSOR_SEND_LOCATION_SPEED:
			str = "RBLE_CMD_LNP_SENSOR_SEND_LOCATION_SPEED";
			break;
		case RBLE_CMD_LNP_SENSOR_SET_POSITION_QUALITY:
			str = "RBLE_CMD_LNP_SENSOR_SET_POSITION_QUALITY";
			break;
		case RBLE_CMD_LNP_SENSOR_SEND_LN_CONTROL_POINT:
			str = "RBLE_CMD_LNP_SENSOR_SEND_LN_CONTROL_POINT";
			break;
		case RBLE_CMD_LNP_SENSOR_SEND_NAVIGATION:
			str = "RBLE_CMD_LNP_SENSOR_SEND_NAVIGATION";
			break;
		case RBLE_CMD_LNP_SENSOR_SEND_BATTERY_LEVEL:
			str = "RBLE_CMD_LNP_SENSOR_SEND_BATTERY_LEVEL";
			break;
		case RBLE_CMD_LNP_COLLECTOR_ENABLE:
			str = "RBLE_CMD_LNP_COLLECTOR_ENABLE";
			break;
		case RBLE_CMD_LNP_COLLECTOR_DISABLE:
			str = "RBLE_CMD_LNP_COLLECTOR_DISABLE";
			break;
		case RBLE_CMD_LNP_COLLECTOR_READ_CHAR:
			str = "RBLE_CMD_LNP_COLLECTOR_READ_CHAR";
			break;
		case RBLE_CMD_LNP_COLLECTOR_WRITE_LN_CONTROL_POINT:
			str = "RBLE_CMD_LNP_COLLECTOR_WRITE_LN_CONTROL_POINT";
			break;
		case RBLE_CMD_LNP_COLLECTOR_WRITE_CHAR:
			str = "RBLE_CMD_LNP_COLLECTOR_WRITE_CHAR";
			break;
		case RBLE_CMD_PASP_SERVER_ENABLE:
			str = "RBLE_CMD_PASP_SERVER_ENABLE";
			break;
		case RBLE_CMD_PASP_SERVER_DISABLE:
			str = "RBLE_CMD_PASP_SERVER_DISABLE";
			break;
		case RBLE_CMD_PASP_SERVER_SEND_ALERT_STATUS:
			str = "RBLE_CMD_PASP_SERVER_SEND_ALERT_STATUS";
			break;
		case RBLE_CMD_PASP_SERVER_SEND_RINGER_SETTING:
			str = "RBLE_CMD_PASP_SERVER_SEND_RINGER_SETTING";
			break;
		case RBLE_CMD_PASP_CLIENT_ENABLE:
			str = "RBLE_CMD_PASP_CLIENT_ENABLE";
			break;
		case RBLE_CMD_PASP_CLIENT_DISABLE:
			str = "RBLE_CMD_PASP_CLIENT_DISABLE";
			break;
		case RBLE_CMD_PASP_CLIENT_READ_CHAR:
			str = "RBLE_CMD_PASP_CLIENT_READ_CHAR";
			break;
		case RBLE_CMD_PASP_CLIENT_WRITE_RINGER_CONTROL_POINT:
			str = "RBLE_CMD_PASP_CLIENT_WRITE_RINGER_CONTROL_POINT";
			break;
		case RBLE_CMD_PASP_CLIENT_WRITE_CHAR:
			str = "RBLE_CMD_PASP_CLIENT_WRITE_CHAR";
			break;
		case RBLE_CMD_RSCP_SENSOR_ENABLE:
			str = "RBLE_CMD_RSCP_SENSOR_ENABLE";
			break;
		case RBLE_CMD_RSCP_SENSOR_DISABLE:
			str = "RBLE_CMD_RSCP_SENSOR_DISABLE";
			break;
		case RBLE_CMD_RSCP_SENSOR_SEND_MEASUREMENTS:
			str = "RBLE_CMD_RSCP_SENSOR_SEND_MEASUREMENTS";
			break;
		case RBLE_CMD_RSCP_SENSOR_SEND_SC_CONTROL_POINT:
			str = "RBLE_CMD_RSCP_SENSOR_SEND_SC_CONTROL_POINT";
			break;
		case RBLE_CMD_RSCP_COLLECTOR_ENABLE:
			str = "RBLE_CMD_RSCP_COLLECTOR_ENABLE";
			break;
		case RBLE_CMD_RSCP_COLLECTOR_DISABLE:
			str = "RBLE_CMD_RSCP_COLLECTOR_DISABLE";
			break;
		case RBLE_CMD_RSCP_COLLECTOR_READ_CHAR:
			str = "RBLE_CMD_RSCP_COLLECTOR_READ_CHAR";
			break;
		case RBLE_CMD_RSCP_COLLECTOR_WRITE_SC_CONTROL_POINT:
			str = "RBLE_CMD_RSCP_COLLECTOR_WRITE_SC_CONTROL_POINT";
			break;
		case RBLE_CMD_RSCP_COLLECTOR_WRITE_CHAR:
			str = "RBLE_CMD_RSCP_COLLECTOR_WRITE_CHAR";
			break;
		case RBLE_CMD_VS_TEST_RX_START:
			str = "RBLE_CMD_VS_TEST_RX_START";
			break;
		case RBLE_CMD_VS_TEST_TX_START:
			str = "RBLE_CMD_VS_TEST_TX_START";
			break;
		case RBLE_CMD_VS_TEST_END:
			str = "RBLE_CMD_VS_TEST_END";
			break;
		case RBLE_CMD_VS_GPIO_DIR:
			str = "RBLE_CMD_VS_GPIO_DIR";
			break;
		case RBLE_CMD_VS_GPIO_ACCESS:
			str = "RBLE_CMD_VS_GPIO_ACCESS";
			break;
		default:
			str = "UN-KNOWN COMMAND CODE";
			break;
	}

	printf( "[COMMAND        : %s \n", str);
}

void rBLE_Change_Event_Code(RBLE_EVENT_NO	opcode)
{
	char_t *str;

	switch(opcode){
		case RBLE_EVT_GAP_RESET_RESULT:
			str = "RBLE_GAP_EVT_INIT_RESULT";
			break;
		case RBLE_EVT_GAP_SET_NAME_COMP:
			str = "RBLE_EVT_GAP_SET_NAME_COMP";
			break;
		case RBLE_EVT_GAP_OBSERVATION_ENABLE_COMP:
			str = "RBLE_EVT_GAP_OBSERVATION_ENABLE_COMP";
			break;
		case RBLE_EVT_GAP_OBSERVATION_DISABLE_COMP:
			str = "RBLE_EVT_GAP_OBSERVATION_DISABLE_COMP";
			break;
		case RBLE_EVT_GAP_BROADCAST_ENABLE_COMP:
			str = "RBLE_EVT_GAP_BROADCAST_ENABLE_COMP";
			break;
		case RBLE_EVT_GAP_BROADCAST_DISABLE_COMP:
			str = "RBLE_EVT_GAP_BROADCAST_DISABLE_COMP";
			break;
		case RBLE_EVT_GAP_SET_BONDING_MODE_COMP:
			str = "RBLE_EVT_GAP_SET_BONDING_MODE_COMP";
			break;
		case RBLE_EVT_GAP_SET_SECURITY_REQUEST_COMP:
			str = "RBLE_EVT_GAP_SET_SECURITY_REQUEST_COMP";
			break;
		case RBLE_EVT_GAP_GET_DEVICE_INFO_COMP:
			str = "RBLE_EVT_GAP_GET_DEVICE_INFO_COMP";
			break;
//		case RBLE_EVT_GAP_KNOWN_ADDRESS_IND:
//			str = "RBLE_EVT_GAP_KNOWN_ADDRESS_IND";
//			break;
		case RBLE_EVT_GAP_GET_WHITE_LIST_SIZE_COMP:
			str = "RBLE_EVT_GAP_GET_WHITE_LIST_SIZE_COMP";
			break;
		case RBLE_EVT_GAP_ADD_TO_WHITE_LIST_COMP:
			str = "RBLE_EVT_GAP_ADD_TO_WHITE_LIST_COMP";
			break;
		case RBLE_EVT_GAP_DEL_FROM_WHITE_LIST_COMP:
			str = "RBLE_EVT_GAP_DEL_FROM_WHITE_LIST_COMP";
			break;
		case RBLE_EVT_GAP_GET_REMOTE_DEVICE_NAME_COMP:
			str = "RBLE_EVT_GAP_GET_REMOTE_DEVICE_NAME_COMP";
			break;
		case RBLE_EVT_GAP_GET_REMOTE_DEVICE_INFO_COMP:
			str = "RBLE_EVT_GAP_GET_REMOTE_DEVICE_INFO_COMP";
			break;
		case RBLE_EVT_GAP_DEVICE_SEARCH_COMP:
			str = "RBLE_EVT_GAP_DEVICE_SEARCH_COMP";
			break;
		case RBLE_EVT_GAP_DEVICE_SEARCH_RESULT_IND:
			str = "RBLE_EVT_GAP_DEVICE_SEARCH_RESULT_IND";
			break;
//		case RBLE_EVT_GAP_KNOWN_DEVICE_SEARCH_RESULT_IND:
//			str = "RBLE_EVT_GAP_KNOWN_DEVICE_SEARCH_RESULT_IND";
//			break;
//		case RBLE_EVT_GAP_SET_RECONNECT_ADDRESS_COMP:
//			str = "RBLE_EVT_GAP_SET_RECONNECT_ADDRESS_COMP";
//			break;
		case RBLE_EVT_GAP_SET_RANDOM_ADDRESS_COMP:
			str = "RBLE_EVT_GAP_SET_RANDOM_ADDRESS_COMP";
			break;
		case RBLE_EVT_GAP_SET_PRIVACY_FEATURE_COMP:
			str = "RBLE_EVT_GAP_SET_PRIVACY_FEATURE_COMP";
			break;
//		case RBLE_EVT_GAP_SET_PERIPHERAL_PRIVACY_FEATURE_COMP:
//			str = "RBLE_EVT_GAP_SET_PERIPHERAL_PRIVACY_FEATURE_COMP";
//			break;
		case RBLE_EVT_GAP_CONNECTION_COMP:
			str = "RBLE_EVT_GAP_CONNECTION_COMP";
			break;
		case RBLE_EVT_GAP_CONNECTION_CANCEL_COMP:
			str = "RBLE_EVT_GAP_CONNECTION_CANCEL_COMP";
			break;
		case RBLE_EVT_GAP_DISCONNECT_COMP:
			str = "RBLE_EVT_GAP_DISCONNECT_COMP";
			break;
		case RBLE_EVT_GAP_ADVERTISING_REPORT_IND:
			str = "RBLE_EVT_GAP_ADVERTISING_REPORT_IND";
			break;
		case RBLE_EVT_GAP_BONDING_COMP:
			str = "RBLE_EVT_GAP_BONDING_COMP";
			break;
		case RBLE_EVT_GAP_CHANGE_CONNECTION_PARAM_REQ_IND:
			str = "RBLE_EVT_GAP_CHANGE_CONNECTION_PARAM_REQ_IND";
			break;
		case RBLE_EVT_GAP_CHANGE_CONNECTION_PARAM_COMP:
			str = "RBLE_EVT_GAP_CHANGE_CONNECTION_PARAM_COMP";
			break;
		case RBLE_EVT_GAP_CHANGE_CONNECTION_PARAM_RESPONSE:
			str = "RBLE_EVT_GAP_CHANGE_CONNECTION_PARAM_RESPONSE";
			break;
		case RBLE_EVT_GAP_CHANNEL_MAP_REQ_CMP_EVT:
			str = "RBLE_EVT_GAP_CHANNEL_MAP_REQ_CMP_EVT";
			break;
//		case RBLE_EVT_GAP_ATTRIBUTE_HANDLE_IND:
//			str = "RBLE_EVT_GAP_ATTRIBUTE_HANDLE_IND";
//			break;
//		case RBLE_EVT_GAP_GET_ATTRIBUTE_HANDLE_COMP:
//			str = "RBLE_EVT_GAP_GET_ATTRIBUTE_HANDLE_COMP";
//			break;
		case RBLE_EVT_GAP_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_GAP_CMD_DISALLOWED_IND";
			break;
		case RBLE_EVT_SM_SET_KEY_CNF:
			str = "RBLE_EVT_SM_SET_KEY_CNF";
			break;
		case RBLE_EVT_SM_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_SM_CMD_DISALLOWED_IND";
			break;
		case RBLE_EVT_GATT_DISC_CHAR_BY_UUID_CMP:
			str = "RBLE_EVT_GATT_DISC_CHAR_BY_UUID_CMP";
			break;
		case RBLE_EVT_GATT_COMPLETE:
			str = "RBLE_EVT_GATT_COMPLETE";
			break;
		case RBLE_EVT_FMP_LOCATOR_ENABLE_COMP:
			str = "RBLE_EVT_FMP_LOCATOR_ENABLE_COMP";
			break;
		case RBLE_EVT_FMP_LOCATOR_ERROR_IND:
			str = "RBLE_EVT_FMP_LOCATOR_ERROR_IND";
			break;
		case RBLE_EVT_FMP_TARGET_ALERT_IND:
			str = "RBLE_EVT_FMP_TARGET_ALERT_IND";
			break;
		case RBLE_EVT_PXP_MONITOR_ENABLE_COMP:
			str = "RBLE_EVT_PXP_MONITOR_ENABLE_COMP";
			break;
		case RBLE_EVT_PXP_MONITOR_ERROR_IND:
			str = "RBLE_EVT_PXP_MONITOR_ERROR_IND";
			break;
		case RBLE_EVT_PXP_MONITOR_READ_CHAR_RESPONSE:
			str = "RBLE_EVT_PXP_MONITOR_READ_CHAR_RESPONSE";
			break;
		case RBLE_EVT_PXP_MONITOR_WRITE_CHAR_RESPONSE:
			str = "RBLE_EVT_PXP_MONITOR_WRITE_CHAR_RESPONSE";
			break;
		case RBLE_EVT_PXP_REPORTER_DISABLE_COMP:
			str = "RBLE_EVT_PXP_REPORTER_DISABLE_COMP";
			break;
		case RBLE_EVT_PXP_REPORTER_ALERT_IND:
			str = "RBLE_EVT_PXP_REPORTER_ALERT_IND";
			break;
#if 0
		case RBLE_EVT_PXP_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_PXP_CMD_DISALLOWED_IND";
			break;
#endif
		case RBLE_EVT_HTP_COLLECTOR_ENABLE_COMP:
			str = "RBLE_EVT_HTP_COLLECTOR_ENABLE_COMP";
			break;
		case RBLE_EVT_HTP_COLLECTOR_ERROR_IND:
			str = "RBLE_EVT_HTP_COLLECTOR_ERROR_IND";
			break;
		case RBLE_EVT_HTP_COLLECTOR_TEMP_IND:
			str = "RBLE_EVT_HTP_COLLECTOR_TEMP_IND";
			break;
		case RBLE_EVT_HTP_COLLECTOR_MEAS_INTV_IND:
			str = "RBLE_EVT_HTP_COLLECTOR_MEAS_INTV_IND";
			break;
		case RBLE_EVT_HTP_COLLECTOR_READ_CHAR_RESPONSE:
			str = "RBLE_EVT_HTP_COLLECTOR_READ_CHAR_RESPONSE";
			break;
		case RBLE_EVT_HTP_COLLECTOR_WRITE_CHAR_RESPONSE:
			str = "RBLE_EVT_HTP_COLLECTOR_WRITE_CHAR_RESPONSE";
			break;
		case RBLE_EVT_HTP_THERMOMETER_ENABLE_COMP:
			str = "RBLE_EVT_HTP_THERMOMETER_ENABLE_COMP";
			break;
		case RBLE_EVT_HTP_THERMOMETER_DISABLE_COMP:
			str = "RBLE_EVT_HTP_THERMOMETER_DISABLE_COMP";
			break;
		case RBLE_EVT_HTP_THERMOMETER_ERROR_IND:
			str = "RBLE_EVT_HTP_THERMOMETER_ERROR_IND";
			break;
		case RBLE_EVT_HTP_THERMOMETER_SEND_TEMP_COMP:
			str = "RBLE_EVT_HTP_THERMOMETER_SEND_TEMP_COMP";
			break;
		case RBLE_EVT_HTP_THERMOMETER_REQ_MEASUREMENT_PERIOD_IND_COMP:
			str = "RBLE_EVT_HTP_THERMOMETER_REQ_MEASUREMENT_PERIOD_IND_COMP";
			break;
		case RBLE_EVT_HTP_THERMOMETER_MEAS_INTV_CHG_IND:
			str = "RBLE_EVT_HTP_THERMOMETER_MEAS_INTV_CHG_IND";
			break;
#if 0
		case RBLE_EVT_HTP_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_HTP_CMD_DISALLOWED_IND";
			break;
#endif
		case RBLE_EVT_BLP_COLLECTOR_ENABLE_COMP:
			str = "RBLE_EVT_BLP_COLLECTOR_ENABLE_COMP";
			break;
		case RBLE_EVT_BLP_COLLECTOR_ERROR_IND:
			str = "RBLE_EVT_BLP_COLLECTOR_ERROR_IND";
			break;
		case RBLE_EVT_BLP_COLLECTOR_MEASUREMENTS_IND:
			str = "RBLE_EVT_BLP_COLLECTOR_MEASUREMENTS_IND";
			break;
		case RBLE_EVT_BLP_COLLECTOR_READ_CHAR_RESPONSE:
			str = "RBLE_EVT_BLP_COLLECTOR_READ_CHAR_RESPONSE";
			break;
		case RBLE_EVT_BLP_COLLECTOR_WRITE_CHAR_RESPONSE:
			str = "RBLE_EVT_BLP_COLLECTOR_WRITE_CHAR_RESPONSE";
			break;
		case RBLE_EVT_BLP_SENSOR_ENABLE_COMP:
			str = "RBLE_EVT_BLP_SENSOR_ENABLE_COMP";
			break;
		case RBLE_EVT_BLP_SENSOR_DISABLE_COMP:
			str = "RBLE_EVT_BLP_SENSOR_DISABLE_COMP";
			break;
		case RBLE_EVT_BLP_SENSOR_ERROR_IND:
			str = "RBLE_EVT_BLP_SENSOR_ERROR_IND";
			break;
		case RBLE_EVT_BLP_SENSOR_SEND_MEASUREMENTS_COMP:
			str = "RBLE_EVT_BLP_SENSOR_SEND_MEASUREMENTS_COMP";
			break;
#if 0
		case RBLE_EVT_BLP_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_BLP_CMD_DISALLOWED_IND";
			break;
#endif
		case RBLE_EVT_HRP_SENSOR_ENABLE_COMP:
			str = "RBLE_EVT_HRP_SENSOR_ENABLE_COMP";
			break;
		case RBLE_EVT_HRP_SENSOR_DISABLE_COMP:
			str = "RBLE_EVT_HRP_SENSOR_DISABLE_COMP";
			break;
		case RBLE_EVT_HRP_SENSOR_ERROR_IND:
			str = "RBLE_EVT_HRP_SENSOR_ERROR_IND";
			break;
		case RBLE_EVT_HRP_SENSOR_SEND_MEASUREMENTS_COMP:
			str = "RBLE_EVT_HRP_SENSOR_SEND_MEASUREMENTS_COMP";
			break;
		case RBLE_EVT_HRP_SENSOR_CHG_CP_IND:
			str = "RBLE_EVT_HRP_SENSOR_CHG_CP_IND";
			break;
		case RBLE_EVT_HRP_SENSOR_CFG_NTF_IND:
			str = "RBLE_EVT_HRP_SENSOR_CFG_NTF_IND";
			break;
		case RBLE_EVT_HRP_SENSOR_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_HRP_SENSOR_CMD_DISALLOWED_IND";
			break;
		case RBLE_EVT_HRP_COLLECTOR_ENABLE_COMP:
			str = "RBLE_EVT_HRP_COLLECTOR_ENABLE_COMP";
			break;
		case RBLE_EVT_HRP_COLLECTOR_DISABLE_COMP:
			str = "RBLE_EVT_HRP_COLLECTOR_DISABLE_COMP";
			break;
		case RBLE_EVT_HRP_COLLECTOR_ERROR_IND:
			str = "RBLE_EVT_HRP_COLLECTOR_ERROR_IND";
			break;
		case RBLE_EVT_HRP_COLLECTOR_MEASUREMENTS_NTF:
			str = "RBLE_EVT_HRP_COLLECTOR_MEASUREMENTS_NTF";
			break;
		case RBLE_EVT_HRP_COLLECTOR_READ_CHAR_RESPONSE:
			str = "RBLE_EVT_HRP_COLLECTOR_READ_CHAR_RESPONSE";
			break;
		case RBLE_EVT_HRP_COLLECTOR_WRITE_CHAR_RESPONSE:
			str = "RBLE_EVT_HRP_COLLECTOR_WRITE_CHAR_RESPONSE";
			break;
		case RBLE_EVT_HRP_COLLECTOR_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_HRP_COLLECTOR_CMD_DISALLOWED_IND";
			break;
		case RBLE_EVT_CSCP_SENSOR_ENABLE_COMP:
			str = "RBLE_EVT_CSCP_SENSOR_ENABLE_COMP";
			break;
		case RBLE_EVT_CSCP_SENSOR_DISABLE_COMP:
			str = "RBLE_EVT_CSCP_SENSOR_DISABLE_COMP";
			break;
		case RBLE_EVT_CSCP_SENSOR_ERROR_IND:
			str = "RBLE_EVT_CSCP_SENSOR_ERROR_IND";
			break;
		case RBLE_EVT_CSCP_SENSOR_SEND_MEASUREMENTS_COMP:
			str = "RBLE_EVT_CSCP_SENSOR_SEND_MEASUREMENTS_COMP";
			break;
		case RBLE_EVT_CSCP_SENSOR_SEND_SC_CP_COMP:
			str = "RBLE_EVT_CSCP_SENSOR_SEND_SC_CP_COMP";
			break;
		case RBLE_EVT_CSCP_SENSOR_CHG_SC_CP_IND:
			str = "RBLE_EVT_CSCP_SENSOR_CHG_SC_CP_IND";
			break;
		case RBLE_EVT_CSCP_SENSOR_CFG_INDNTF_IND:
			str = "RBLE_EVT_CSCP_SENSOR_CFG_INDNTF_IND";
			break;
		case RBLE_EVT_CSCP_SENSOR_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_CSCP_SENSOR_CMD_DISALLOWED_IND";
			break;
		case RBLE_EVT_CSCP_COLLECTOR_ENABLE_COMP:
			str = "RBLE_EVT_CSCP_COLLECTOR_ENABLE_COMP";
			break;
		case RBLE_EVT_CSCP_COLLECTOR_DISABLE_COMP:
			str = "RBLE_EVT_CSCP_COLLECTOR_DISABLE_COMP";
			break;
		case RBLE_EVT_CSCP_COLLECTOR_ERROR_IND:
			str = "RBLE_EVT_CSCP_COLLECTOR_ERROR_IND";
			break;
		case RBLE_EVT_CSCP_COLLECTOR_MEASUREMENTS_NTF:
			str = "RBLE_EVT_CSCP_COLLECTOR_MEASUREMENTS_NTF";
			break;
		case RBLE_EVT_CSCP_COLLECTOR_SC_CP_IND:
			str = "RBLE_EVT_CSCP_COLLECTOR_SC_CP_IND";
			break;
		case RBLE_EVT_CSCP_COLLECTOR_READ_CHAR_RESPONSE:
			str = "RBLE_EVT_CSCP_COLLECTOR_READ_CHAR_RESPONSE";
			break;
		case RBLE_EVT_CSCP_COLLECTOR_WRITE_CHAR_RESPONSE:
			str = "RBLE_EVT_CSCP_COLLECTOR_WRITE_CHAR_RESPONSE";
			break;
		case RBLE_EVT_CSCP_COLLECTOR_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_CSCP_COLLECTOR_CMD_DISALLOWED_IND";
			break;
		case RBLE_EVT_CPP_SENSOR_ENABLE_COMP:
			str = "RBLE_EVT_CPP_SENSOR_ENABLE_COMP";
			break;
		case RBLE_EVT_CPP_SENSOR_DISABLE_COMP:
			str = "RBLE_EVT_CPP_SENSOR_DISABLE_COMP";
			break;
		case RBLE_EVT_CPP_SENSOR_ERROR_IND:
			str = "RBLE_EVT_CPP_SENSOR_ERROR_IND";
			break;
		case RBLE_EVT_CPP_SENSOR_SEND_MEASUREMENTS_COMP:
			str = "RBLE_EVT_CPP_SENSOR_SEND_MEASUREMENTS_COMP";
			break;
		case RBLE_EVT_CPP_SENSOR_BROADCAST_MEASUREMENTS_COMP:
			str = "RBLE_EVT_CPP_SENSOR_BROADCAST_MEASUREMENTS_COMP";
			break;
		case RBLE_EVT_CPP_SENSOR_SEND_VECTOR_COMP:
			str = "RBLE_EVT_CPP_SENSOR_SEND_VECTOR_COMP";
			break;
		case RBLE_EVT_CPP_SENSOR_SEND_CP_CP_COMP:
			str = "RBLE_EVT_CPP_SENSOR_SEND_CP_CP_COMP";
			break;
		case RBLE_EVT_CPP_SENSOR_SEND_BATTERY_LEVEL_COMP:
			str = "RBLE_EVT_CPP_SENSOR_SEND_BATTERY_LEVEL_COMP";
			break;
		case RBLE_EVT_CPP_SENSOR_CHG_CP_CP_IND:
			str = "RBLE_EVT_CPP_SENSOR_CHG_CP_CP_IND";
			break;
		case RBLE_EVT_CPP_SENSOR_CFG_INDNTFBRD_IND:
			str = "RBLE_EVT_CPP_SENSOR_CFG_INDNTFBRD_IND";
			break;
		case RBLE_EVT_CPP_SENSOR_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_CPP_SENSOR_CMD_DISALLOWED_IND";
			break;
			
		case RBLE_EVT_CPP_COLLECTOR_ENABLE_COMP:
			str = "RBLE_EVT_CPP_COLLECTOR_ENABLE_COMP";
			break;
		case RBLE_EVT_CPP_COLLECTOR_DISABLE_COMP:
			str = "RBLE_EVT_CPP_COLLECTOR_DISABLE_COMP";
			break;
		case RBLE_EVT_CPP_COLLECTOR_ERROR_IND:
			str = "RBLE_EVT_CPP_COLLECTOR_ERROR_IND";
			break;
		case RBLE_EVT_CPP_COLLECTOR_MEASUREMENTS_NTF:
			str = "RBLE_EVT_CPP_COLLECTOR_MEASUREMENTS_NTF";
			break;
		case RBLE_EVT_CPP_COLLECTOR_VECTOR_NTF:
			str = "RBLE_EVT_CPP_COLLECTOR_VECTOR_NTF";
			break;
		case RBLE_EVT_CPP_COLLECTOR_CP_CP_IND:
			str = "RBLE_EVT_CPP_COLLECTOR_CP_CP_IND";
			break;
		case RBLE_EVT_CPP_COLLECTOR_BATTERY_LEVEL_NTF:
			str = "RBLE_EVT_CPP_COLLECTOR_BATTERY_LEVEL_NTF";
			break;
		case RBLE_EVT_CPP_COLLECTOR_READ_CHAR_RESPONSE:
			str = "RBLE_EVT_CPP_COLLECTOR_READ_CHAR_RESPONSE";
			break;
		case RBLE_EVT_CPP_COLLECTOR_WRITE_CHAR_RESPONSE:
			str = "RBLE_EVT_CPP_COLLECTOR_WRITE_CHAR_RESPONSE";
			break;
		case RBLE_EVT_CPP_COLLECTOR_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_CPP_COLLECTOR_CMD_DISALLOWED_IND";
			break;
			
		case RBLE_EVT_TIP_SERVER_ENABLE_COMP:
			str = "RBLE_EVT_TIP_SERVER_ENABLE_COMP";
			break;
		case RBLE_EVT_TIP_SERVER_DISABLE_COMP:
			str = "RBLE_EVT_TIP_SERVER_DISABLE_COMP";
			break;
		case RBLE_EVT_TIP_SERVER_ERROR_IND:
			str = "RBLE_EVT_TIP_SERVER_ERROR_IND";
			break;
		case RBLE_EVT_TIP_SERVER_SEND_CURRENT_TIME_COMP:
			str = "RBLE_EVT_TIP_SERVER_SEND_CURRENT_TIME_COMP";
			break;
		case RBLE_EVT_TIP_SERVER_WRITE_DATA_COMP:
			str = "RBLE_EVT_TIP_SERVER_WRITE_DATA_COMP";
			break;
		case RBLE_EVT_TIP_SERVER_CHG_TIME_UPDATE_CP_IND:
			str = "RBLE_EVT_TIP_SERVER_CHG_TIME_UPDATE_CP_IND";
			break;
		case RBLE_EVT_TIP_SERVER_CFG_NTF_IND:
			str = "RBLE_EVT_TIP_SERVER_CFG_NTF_IND";
			break;
		case RBLE_EVT_TIP_SERVER_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_TIP_SERVER_CMD_DISALLOWED_IND";
			break;
			
		case RBLE_EVT_TIP_CLIENT_ENABLE_COMP:
			str = "RBLE_EVT_TIP_CLIENT_ENABLE_COMP";
			break;
		case RBLE_EVT_TIP_CLIENT_DISABLE_COMP:
			str = "RBLE_EVT_TIP_CLIENT_DISABLE_COMP";
			break;
		case RBLE_EVT_TIP_CLIENT_ERROR_IND:
			str = "RBLE_EVT_TIP_CLIENT_ERROR_IND";
			break;
		case RBLE_EVT_TIP_CLIENT_CURRENT_TIME_NTF:
			str = "RBLE_EVT_TIP_CLIENT_CURRENT_TIME_NTF";
			break;
		case RBLE_EVT_TIP_CLIENT_READ_CHAR_RESPONSE:
			str = "RBLE_EVT_TIP_CLIENT_READ_CHAR_RESPONSE";
			break;
		case RBLE_EVT_TIP_CLIENT_WRITE_CHAR_RESPONSE:
			str = "RBLE_EVT_TIP_CLIENT_WRITE_CHAR_RESPONSE";
			break;
		case RBLE_EVT_TIP_CLIENT_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_TIP_CLIENT_CMD_DISALLOWED_IND";
			break;
		
		case RBLE_EVT_ANP_SERVER_ENABLE_COMP:
			str = "RBLE_EVT_ANP_SERVER_ENABLE_COMP";
			break;
		case RBLE_EVT_ANP_SERVER_DISABLE_COMP:
			str = "RBLE_EVT_ANP_SERVER_DISABLE_COMP";
			break;
		case RBLE_EVT_ANP_SERVER_ERROR_IND:
			str = "RBLE_EVT_ANP_SERVER_ERROR_IND";
			break;
		case RBLE_EVT_ANP_SERVER_SEND_NEW_ALERT_COMP:
			str = "RBLE_EVT_ANP_SERVER_SEND_NEW_ALERT_COMP";
			break;
		case RBLE_EVT_ANP_SERVER_SEND_UNREAD_ALERT_COMP:
			str = "RBLE_EVT_ANP_SERVER_SEND_UNREAD_ALERT_COMP";
			break;
		case RBLE_EVT_ANP_SERVER_CHG_ALERT_NTF_CP_IND:
			str = "RBLE_EVT_ANP_SERVER_CHG_ALERT_NTF_CP_IND";
			break;
		case RBLE_EVT_ANP_SERVER_CFG_NTF_IND:
			str = "RBLE_EVT_ANP_SERVER_CFG_NTF_IND";
			break;
		case RBLE_EVT_ANP_SERVER_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_ANP_SERVER_CMD_DISALLOWED_IND";
			break;
		
		case RBLE_EVT_ANP_CLIENT_ENABLE_COMP:
			str = "RBLE_EVT_ANP_CLIENT_ENABLE_COMP";
			break;
		case RBLE_EVT_ANP_CLIENT_DISABLE_COMP:
			str = "RBLE_EVT_ANP_CLIENT_DISABLE_COMP";
			break;
		case RBLE_EVT_ANP_CLIENT_ERROR_IND:
			str = "RBLE_EVT_ANP_CLIENT_ERROR_IND";
			break;
		case RBLE_EVT_ANP_CLIENT_NEW_ALERT_NTF:
			str = "RBLE_EVT_ANP_CLIENT_NEW_ALERT_NTF";
			break;
		case RBLE_EVT_ANP_CLIENT_UNREAD_ALERT_NTF:
			str = "RBLE_EVT_ANP_CLIENT_UNREAD_ALERT_NTF";
			break;
		case RBLE_EVT_ANP_CLIENT_READ_CHAR_RESPONSE:
			str = "RBLE_EVT_ANP_CLIENT_READ_CHAR_RESPONSE";
			break;
		case RBLE_EVT_ANP_CLIENT_WRITE_CHAR_RESPONSE:
			str = "RBLE_EVT_ANP_CLIENT_WRITE_CHAR_RESPONSE";
			break;
		case RBLE_EVT_ANP_CLIENT_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_ANP_CLIENT_CMD_DISALLOWED_IND";
			break;
			
		case RBLE_EVT_LNP_SENSOR_ENABLE_COMP:
			str = "RBLE_EVT_LNP_SENSOR_ENABLE_COMP";
			break;
		case RBLE_EVT_LNP_SENSOR_DISABLE_COMP:
			str = "RBLE_EVT_LNP_SENSOR_DISABLE_COMP";
			break;
		case RBLE_EVT_LNP_SENSOR_ERROR_IND:
			str = "RBLE_EVT_LNP_SENSOR_ERROR_IND";
			break;
		case RBLE_EVT_LNP_SENSOR_SEND_LOCATION_SPEED_COMP:
			str = "RBLE_EVT_LNP_SENSOR_SEND_LOCATION_SPEED_COMP";
			break;
		case RBLE_EVT_LNP_SENSOR_SET_POSITION_QUALITY_COMP:
			str = "RBLE_EVT_LNP_SENSOR_SET_POSITION_QUALITY_COMP";
			break;
		case RBLE_EVT_LNP_SENSOR_SEND_LN_CP_COMP:
			str = "RBLE_EVT_LNP_SENSOR_SEND_LN_CP_COMP";
			break;
		case RBLE_EVT_LNP_SENSOR_SEND_NAVIGATION_COMP:
			str = "RBLE_EVT_LNP_SENSOR_SEND_NAVIGATION_COMP";
			break;
		case RBLE_EVT_LNP_SENSOR_SEND_BATTERY_LEVEL_COMP:
			str = "RBLE_EVT_LNP_SENSOR_SEND_BATTERY_LEVEL_COMP";
			break;
		case RBLE_EVT_LNP_SENSOR_CHG_LN_CP_IND:
			str = "RBLE_EVT_LNP_SENSOR_CHG_LN_CP_IND";
			break;
		case RBLE_EVT_LNP_SENSOR_CFG_INDNTF_IND:
			str = "RBLE_EVT_LNP_SENSOR_CFG_INDNTF_IND";
			break;
		case RBLE_EVT_LNP_SENSOR_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_LNP_SENSOR_CMD_DISALLOWED_IND";
			break;
		case RBLE_EVT_LNP_COLLECTOR_ENABLE_COMP:
			str = "RBLE_EVT_LNP_COLLECTOR_ENABLE_COMP";
			break;
		case RBLE_EVT_LNP_COLLECTOR_DISABLE_COMP:
			str = "RBLE_EVT_LNP_COLLECTOR_DISABLE_COMP";
			break;
		case RBLE_EVT_LNP_COLLECTOR_ERROR_IND:
			str = "RBLE_EVT_LNP_COLLECTOR_ERROR_IND";
			break;
		case RBLE_EVT_LNP_COLLECTOR_LOCATION_SPEED_NTF:
			str = "RBLE_EVT_LNP_COLLECTOR_LOCATION_SPEED_NTF";
			break;
		case RBLE_EVT_LNP_COLLECTOR_LN_CP_IND:
			str = "RBLE_EVT_LNP_COLLECTOR_LN_CP_IND";
			break;
		case RBLE_EVT_LNP_COLLECTOR_NAVIGATION_NTF:
			str = "RBLE_EVT_LNP_COLLECTOR_NAVIGATION_NTF";
			break;
		case RBLE_EVT_LNP_COLLECTOR_BATTERY_LEVEL_NTF:
			str = "RBLE_EVT_LNP_COLLECTOR_BATTERY_LEVEL_NTF";
			break;
		case RBLE_EVT_LNP_COLLECTOR_READ_CHAR_RESPONSE:
			str = "RBLE_EVT_LNP_COLLECTOR_READ_CHAR_RESPONSE";
			break;
		case RBLE_EVT_LNP_COLLECTOR_WRITE_CHAR_RESPONSE:
			str = "RBLE_EVT_LNP_COLLECTOR_WRITE_CHAR_RESPONSE";
			break;
		case RBLE_EVT_LNP_COLLECTOR_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_LNP_COLLECTOR_CMD_DISALLOWED_IND";
			break;
			
		case RBLE_EVT_PASP_SERVER_ENABLE_COMP:
			str = "RBLE_EVT_PASP_SERVER_ENABLE_COMP";
			break;
		case RBLE_EVT_PASP_SERVER_DISABLE_COMP:
			str = "RBLE_EVT_PASP_SERVER_DISABLE_COMP";
			break;
		case RBLE_EVT_PASP_SERVER_ERROR_IND:
			str = "RBLE_EVT_PASP_SERVER_ERROR_IND";
			break;
		case RBLE_EVT_PASP_SERVER_SEND_ALERT_STATUS_COMP:
			str = "RBLE_EVT_PASP_SERVER_SEND_ALERT_STATUS_COMP";
			break;
		case RBLE_EVT_PASP_SERVER_SEND_RINGER_SETTING_COMP:
			str = "RBLE_EVT_PASP_SERVER_SEND_RINGER_SETTING_COMP";
			break;
		case RBLE_EVT_PASP_SERVER_CHG_RINGER_CP_IND:
			str = "RBLE_EVT_PASP_SERVER_CHG_RINGER_CP_IND";
			break;
		case RBLE_EVT_PASP_SERVER_CFG_NTF_IND:
			str = "RBLE_EVT_PASP_SERVER_CFG_NTF_IND";
			break;
		case RBLE_EVT_PASP_SERVER_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_PASP_SERVER_CMD_DISALLOWED_IND";
			break;
		case RBLE_EVT_PASP_CLIENT_ENABLE_COMP:
			str = "RBLE_EVT_PASP_CLIENT_ENABLE_COMP";
			break;
		case RBLE_EVT_PASP_CLIENT_DISABLE_COMP:
			str = "RBLE_EVT_PASP_CLIENT_DISABLE_COMP";
			break;
		case RBLE_EVT_PASP_CLIENT_ERROR_IND:
			str = "RBLE_EVT_PASP_CLIENT_ERROR_IND";
			break;
		case RBLE_EVT_PASP_CLIENT_ALERT_STATUS_NTF:
			str = "RBLE_EVT_PASP_CLIENT_ALERT_STATUS_NTF";
			break;
		case RBLE_EVT_PASP_CLIENT_RINGER_SETTING_NTF:
			str = "RBLE_EVT_PASP_CLIENT_RINGER_SETTING_NTF";
			break;
		case RBLE_EVT_PASP_CLIENT_READ_CHAR_RESPONSE:
			str = "RBLE_EVT_PASP_CLIENT_READ_CHAR_RESPONSE";
			break;
		case RBLE_EVT_PASP_CLIENT_WRITE_CHAR_RESPONSE:
			str = "RBLE_EVT_PASP_CLIENT_WRITE_CHAR_RESPONSE";
			break;
		case RBLE_EVT_PASP_CLIENT_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_PASP_CLIENT_CMD_DISALLOWED_IND";
			break;
			
		case RBLE_EVT_RSCP_SENSOR_ENABLE_COMP:
			str = "RBLE_EVT_RSCP_SENSOR_ENABLE_COMP";
			break;
		case RBLE_EVT_RSCP_SENSOR_DISABLE_COMP:
			str = "RBLE_EVT_RSCP_SENSOR_DISABLE_COMP";
			break;
		case RBLE_EVT_RSCP_SENSOR_ERROR_IND:
			str = "RBLE_EVT_RSCP_SENSOR_ERROR_IND";
			break;
		case RBLE_EVT_RSCP_SENSOR_SEND_MEASUREMENTS_COMP:
			str = "RBLE_EVT_RSCP_SENSOR_SEND_MEASUREMENTS_COMP";
			break;
		case RBLE_EVT_RSCP_SENSOR_SEND_SC_CP_COMP:
			str = "RBLE_EVT_RSCP_SENSOR_SEND_SC_CP_COMP";
			break;
		case RBLE_EVT_RSCP_SENSOR_CHG_SC_CP_IND:
			str = "RBLE_EVT_RSCP_SENSOR_CHG_SC_CP_IND";
			break;
		case RBLE_EVT_RSCP_SENSOR_CFG_INDNTF_IND:
			str = "RBLE_EVT_RSCP_SENSOR_CFG_INDNTF_IND";
			break;
		case RBLE_EVT_RSCP_SENSOR_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_RSCP_SENSOR_CMD_DISALLOWED_IND";
			break;
		case RBLE_EVT_RSCP_COLLECTOR_ENABLE_COMP:
			str = "RBLE_EVT_RSCP_COLLECTOR_ENABLE_COMP";
			break;
		case RBLE_EVT_RSCP_COLLECTOR_DISABLE_COMP:
			str = "RBLE_EVT_RSCP_COLLECTOR_DISABLE_COMP";
			break;
		case RBLE_EVT_RSCP_COLLECTOR_ERROR_IND:
			str = "RBLE_EVT_RSCP_COLLECTOR_ERROR_IND";
			break;
		case RBLE_EVT_RSCP_COLLECTOR_MEASUREMENTS_NTF:
			str = "RBLE_EVT_RSCP_COLLECTOR_MEASUREMENTS_NTF";
			break;
		case RBLE_EVT_RSCP_COLLECTOR_SC_CP_IND:
			str = "RBLE_EVT_RSCP_COLLECTOR_SC_CP_IND";
			break;
		case RBLE_EVT_RSCP_COLLECTOR_READ_CHAR_RESPONSE:
			str = "RBLE_EVT_RSCP_COLLECTOR_READ_CHAR_RESPONSE";
			break;
		case RBLE_EVT_RSCP_COLLECTOR_WRITE_CHAR_RESPONSE:
			str = "RBLE_EVT_RSCP_COLLECTOR_WRITE_CHAR_RESPONSE";
			break;
		case RBLE_EVT_RSCP_COLLECTOR_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_RSCP_COLLECTOR_CMD_DISALLOWED_IND";
			break;
		case RBLE_EVT_VS_TEST_RX_START_COMP:
			str = "RBLE_EVT_VS_TEST_RX_START_COMP";
			break;
		case RBLE_EVT_VS_TEST_TX_START_COMP:
			str = "RBLE_EVT_VS_TEST_TX_START_COMP";
			break;
		case RBLE_EVT_VS_TEST_END_COMP:
			str = "RBLE_EVT_VS_TEST_END_COMP";
			break;
		case RBLE_EVT_VS_GPIO_DIR_COMP:
			str = "RBLE_EVT_VS_GPIO_DIR_COMP";
			break;
		case RBLE_EVT_VS_GPIO_ACCESS_COMP:
			str = "RBLE_EVT_VS_GPIO_ACCESS_COMP";
			break;
		case RBLE_EVT_VS_CMD_DISALLOWED_IND:
			str = "RBLE_EVT_VS_CMD_DISALLOWED_IND";
			break;
		default:
			str = "UN-KNOWN EVENT CODE";
			break;
	}

	printf( "[EVENT]         : %s \n", str);
}
#endif
