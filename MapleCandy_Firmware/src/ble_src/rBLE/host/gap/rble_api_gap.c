/**
 ****************************************************************************************
 *
 * @file		rble_api_gap.c
 *
 * @brief	rBLE-Host GAP I/F API
 *
 * Copyright(C) 2013-2014 Renesas Electronics Corporation
 *
 * $Rev: 2156 $
 *
 ****************************************************************************************
 */

#include	<string.h>

#include	"rble_api.h"
#include	"rble_host.h"
#include	"rble_trans.h"

/**************************** Define Declaration **************************************/

/**************************** Structure Declaration ***********************************/

/**************************** PUBLIC Memory Declaration *******************************/


/**************************** PRIVATE Memory Declaration ******************************/

/**************************** PRIVATE PROTOTYPE Declaration ***************************/

/******************************* Program Erea *********************************************/

/******************************************************************************************/
/**
 *  @brief GAP Reset
 *
 *  GAP Reset @n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Store Call Back API
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	gap_call_back		GAP Call Back API Address
 *  @param[in]	sm_call_back		SM Call Back API Address
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Reset(RBLE_GAP_EVENT_HANDLER gap_call_back, RBLE_SM_EVENT_HANDLER sm_call_back)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	if(RBLE_OK == ret){
		/* Parameter Check */
		if((NULL == gap_call_back) || (NULL == sm_call_back)){
			ret = RBLE_PARAM_ERR;
		}else{
			RBLE_GAP_Info.Handler = gap_call_back;
			RBLE_SM_Info.Handler = sm_call_back;
		}
	}

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = 0u;
		head_pnt->opcode		= RBLE_CMD_GAP_RESET;

		/* Create Command Payloard */
				/* None */
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Set Device Name
 *
 *  Set Device Name @n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	dev_name		Device Name
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Set_Name(RBLE_BD_NAME *dev_name)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;
	uint8_t								*Data_p;
	
	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}
	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = ( sizeof( uint8_t ) + ( sizeof( uint8_t ) * RBLE_BD_NAME_SIZE ) );
		head_pnt->opcode		= RBLE_CMD_GAP_SET_NAME;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, dev_name->namelen );
		memcpy( Data_p, dev_name->name, RBLE_BD_NAME_SIZE );
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Observation Enable
 *
 *  Observation Enable @n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	mode		Device Mode
 *  @param[in]	set_scan	Scan Info
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Observation_Enable(uint16_t mode, RBLE_SCANNING_INFO *set_scan)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;
	uint8_t								*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = ( sizeof( uint16_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint16_t ) + sizeof( uint16_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) );
		head_pnt->opcode		= RBLE_CMD_GAP_OBSERVATION_ENABLE;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET16( Data_p, mode );
		RBLE_DATA_SET8( Data_p, set_scan->set_scan.scan_type );			/* Scan type - 0=passive / 1=active */
		RBLE_DATA_SET8( Data_p, set_scan->set_scan.reserved );			/* Reserve */
		RBLE_DATA_SET16( Data_p, set_scan->set_scan.scan_intv );		/* Scan interval */
		RBLE_DATA_SET16( Data_p, set_scan->set_scan.scan_window );		/* Scan window size */
		RBLE_DATA_SET8( Data_p, set_scan->set_scan.own_addr_type );		/* Own address type - 0=public, 1=random */
		RBLE_DATA_SET8( Data_p, set_scan->set_scan.scan_filt_policy );	/* Scan filter policy */
		RBLE_DATA_SET8( Data_p, set_scan->filter_dup );					/* Filtering policy */
		RBLE_DATA_SET8( Data_p, set_scan->reserved );					/* Reserve */
	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Observation Disable
 *
 *  Observation Disable @n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param		None
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Observation_Disable(void)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = 0u;
		head_pnt->opcode		= RBLE_CMD_GAP_OBSERVATION_DISABLE;

		/* Create Command Payloard */
				/* None */

	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Broadcast Enable
 *
 *  Broadcast Enable @n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	disc_mode	Discovery Mode
 *  @param[in]	conn_mode	Connectable Mode
 *  @param[in]	*adv_info	Advertising Infomation
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Broadcast_Enable(uint16_t disc_mode, uint16_t conn_mode, RBLE_ADV_INFO *adv_info)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;
	uint8_t								*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = ( sizeof( uint16_t ) + sizeof( uint16_t )
									+ sizeof( uint16_t ) + sizeof( uint16_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) + sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t )
									+ sizeof( uint8_t ) + ( sizeof( uint8_t ) * RBLE_ADV_DATA_LEN ) + sizeof( uint8_t ) + ( sizeof( uint8_t ) * RBLE_SCAN_RSP_DATA_LEN ) );
		head_pnt->opcode		= RBLE_CMD_GAP_BROADCAST_ENABLE;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET16( Data_p, disc_mode );
		RBLE_DATA_SET16( Data_p, conn_mode );
		RBLE_DATA_SET16( Data_p, adv_info->adv_param.adv_intv_min );		/* Minimum interval for advertising */
		RBLE_DATA_SET16( Data_p, adv_info->adv_param.adv_intv_max );		/* Maximum interval for advertising */
		RBLE_DATA_SET8( Data_p, adv_info->adv_param.adv_type );				/* Advertising type */
		RBLE_DATA_SET8( Data_p, adv_info->adv_param.own_addr_type );		/* Own address type: public=0x00 /random = 0x01 */
		RBLE_DATA_SET8( Data_p, adv_info->adv_param.direct_addr_type );		/* Direct address type: public=0x00 /random = 0x01 */
		memcpy( Data_p, &adv_info->adv_param.direct_addr.addr[ 0 ], ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) );		/* Direct Bluetooth device address */
		Data_p += ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN );
		RBLE_DATA_SET8( Data_p, adv_info->adv_param.adv_chnl_map );			/* Advertising channel map */
		RBLE_DATA_SET8( Data_p, adv_info->adv_param.adv_filt_policy );		/* Advertising filter policy */
		RBLE_DATA_SET8( Data_p, adv_info->adv_param.reserved );				/* Reserve */

		RBLE_DATA_SET8( Data_p, adv_info->adv_data.adv_data_len ); 			/* Advertising data length */
		memcpy( Data_p, &adv_info->adv_data.adv_data.data[ 0 ], (sizeof( uint8_t ) * RBLE_ADV_DATA_LEN ) );    /* Maximum length data bytes array */
		Data_p += ( sizeof( uint8_t ) * RBLE_ADV_DATA_LEN );
		RBLE_DATA_SET8( Data_p, adv_info->scan_rsp_data.scan_rsp_data_len );/* Scan response data length */
		memcpy( Data_p, &adv_info->scan_rsp_data.data.data[ 0 ], (sizeof( uint8_t ) * RBLE_SCAN_RSP_DATA_LEN ) );    /* Maximum length data bytes array */
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Broadcast Disable
 *
 *  Broadcast Disable @n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param		None
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Broadcast_Disable(void)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = 0u;
		head_pnt->opcode		= RBLE_CMD_GAP_BROADCAST_DISABLE;

		/* Create Command Payloard */
				/* None */

	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Set Bonding Mode
 *
 *  Set Bonding Mode @n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	mode		Device Mode
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Set_Bonding_Mode(uint16_t mode)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;
	uint8_t								*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint16_t );
		head_pnt->opcode		= RBLE_CMD_GAP_SET_BONDING_MODE;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET16( Data_p, mode );
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Set Security Request
 *
 *  Set Security Request @n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	sec			Security Level
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Set_Security_Request(uint8_t sec)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;
	uint8_t								*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint8_t );
		head_pnt->opcode		= RBLE_CMD_GAP_SET_SECURITY_REQUEST;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, sec );
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Get Device Info
 *
 *  Device Infomation Get @n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Get_Device_Info(void)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = 0u;
		head_pnt->opcode		= RBLE_CMD_GAP_GET_DEVICE_INFO;

		/* Create Command Payloard */
			/* Parameter None */

	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Get White List Size
 *
 *  Get White List Size @n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Get_White_List_Size(void)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;
	
	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}
	
	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = 0u;
		head_pnt->opcode		= RBLE_CMD_GAP_GET_WHITE_LIST_SIZE;

		/* Create Command Payloard */
			/* Parameter None */
	}

	return ret;
}

/******************************************************************************************/
/**
 *  @brief Device Add To White List
 *
 *  @n Device Add To White List
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	dev_info		Device Infomation
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Add_To_White_List(RBLE_DEV_ADDR_INFO *dev_info)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;
	uint8_t								*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint8_t ) + ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN );
		head_pnt->opcode		= RBLE_CMD_GAP_ADD_TO_WHITE_LIST;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, dev_info->dev_addr_type );
		memcpy( Data_p, &dev_info->dev_addr.addr[ 0 ], ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) );
	}

	return ret;
}

/******************************************************************************************/
/**
 *  @brief Device Info Delete From White List
 *
 *  @n Device Info Delete From White List
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	all_dev		All Device Flag
 *  @param[in]	dev_info	Device Info
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Del_From_White_List(BOOL all_dev, RBLE_DEV_ADDR_INFO *dev_info)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;
	uint8_t								*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint8_t ) + sizeof( uint8_t ) + ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN );
		head_pnt->opcode		= RBLE_CMD_GAP_DEL_FROM_WHITE_LIST;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, all_dev );
		RBLE_DATA_SET8( Data_p, dev_info->dev_addr_type );
		memcpy( Data_p, &dev_info->dev_addr.addr[ 0 ], ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) );
	}

	return ret;
}

/******************************************************************************************/
/**
 *  @brief Get Remote Device Name
 *
 *  Get Remote Device Name@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	connect_param	conection parameters
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Get_Remote_Device_Name(RBLE_CREATE_CONNECT_PARAM *connect_param)
{
	RBLE_STATUS 							ret = RBLE_OK;
	RBLE_HEADER_t							*head_pnt;
	uint8_t									*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = ( sizeof( uint16_t ) + sizeof( uint16_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN )
									+ sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint16_t ) + sizeof( uint16_t ) + sizeof( uint16_t ) + sizeof( uint16_t ) + sizeof( uint16_t ) + sizeof( uint16_t ) );
		head_pnt->opcode		= RBLE_CMD_GAP_GET_REMOTE_DEVICE_NAME;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET16( Data_p, connect_param->scan_intv );       /* Scan interval */
		RBLE_DATA_SET16( Data_p, connect_param->scan_window );     /* Scan window size */
		RBLE_DATA_SET8( Data_p, connect_param->init_filt_policy );/* Initiator filter policy */
		RBLE_DATA_SET8( Data_p, connect_param->peer_addr_type );  /* Peer address type - 0=public/1=random */
		memcpy( Data_p, &connect_param->peer_addr.addr[ 0 ], ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) );
		Data_p += ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN );
		RBLE_DATA_SET8( Data_p, connect_param->own_addr_type );   /* Own address type - 0=public/1=random */
		RBLE_DATA_SET8( Data_p, connect_param->reserved );        /* Reserved */
		RBLE_DATA_SET16( Data_p, connect_param->con_intv_min );    /* Minimum of connection interval */
		RBLE_DATA_SET16( Data_p, connect_param->con_intv_max );    /* Maximum of connection interval */
		RBLE_DATA_SET16( Data_p, connect_param->con_latency );     /* Connection latency */
		RBLE_DATA_SET16( Data_p, connect_param->superv_to );       /* Link supervision timeout */
		RBLE_DATA_SET16( Data_p, connect_param->ce_len_min );      /* Minimum CE length */
		RBLE_DATA_SET16( Data_p, connect_param->ce_len_max );      /* Maximum CE length */
	}

	return ret;
}

/******************************************************************************************/
/**
 *  @brief Get Remote Device Info
 *
 *  Remote Device Infomation Get @n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	conhdl			Connection Handle
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Get_Remote_Device_Info(uint16_t conhdl)
{
	RBLE_STATUS 							ret = RBLE_OK;
	RBLE_HEADER_t							*head_pnt;
	uint8_t									*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}
	
	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint16_t );
		head_pnt->opcode		= RBLE_CMD_GAP_GET_REMOTE_DEVICE_INFO;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET16( Data_p, conhdl );
	}

	return ret;
}

/******************************************************************************************/
/**
 *  @brief Device Search
 *
 *  Remote Device Search @n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	discovery_type	Discovery Type
 *  @param[in]	addr_type		Address Type
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Device_Search(uint8_t discovery_type, uint8_t addr_type)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;
	uint8_t								*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}
	
	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint8_t ) + sizeof( uint8_t );
		head_pnt->opcode		= RBLE_CMD_GAP_DEVICE_SEARCH;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, discovery_type );
		RBLE_DATA_SET8( Data_p, addr_type );
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Set Random Address
 *
 *  Random Address Setting @n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	bd_addr			BD Address
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Set_Random_Address(RBLE_BD_ADDR *bd_addr)
{
	RBLE_STATUS 							ret = RBLE_OK;
	RBLE_HEADER_t							*head_pnt;
	uint8_t									*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN );
		head_pnt->opcode		= RBLE_CMD_GAP_SET_RANDOM_ADDRESS;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		memcpy( Data_p, &bd_addr->addr[ 0 ], ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) );
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Set Privacy Feature
 *
 *  Privacy Feature Setting @n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	priv_flag		Privacy Feature Flag
 *  @param[in]	set_to_ll		Flag to set the generated random address to link layer
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Set_Privacy_Feature(uint8_t priv_flag, uint8_t set_to_ll)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;
	uint8_t								*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}
	
	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint8_t ) + sizeof( uint8_t );
		head_pnt->opcode		= RBLE_CMD_GAP_SET_PRIVACY_FEATURE;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, priv_flag );
		RBLE_DATA_SET8( Data_p, set_to_ll );
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Create Connection
 *
 *  Create Connection @n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	connect_param	Connection Parameter
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Create_Connection(RBLE_CREATE_CONNECT_PARAM *connect_param)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;
	uint8_t								*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}
	
	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = ( sizeof( uint16_t ) + sizeof( uint16_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN )
									+ sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint16_t ) + sizeof( uint16_t ) + sizeof( uint16_t ) + sizeof( uint16_t ) + sizeof( uint16_t ) + sizeof( uint16_t ) );
		head_pnt->opcode		= RBLE_CMD_GAP_CREATE_CONNECTION;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET16( Data_p, connect_param->scan_intv );       /* Scan interval */
		RBLE_DATA_SET16( Data_p, connect_param->scan_window );     /* Scan window size */
		RBLE_DATA_SET8( Data_p, connect_param->init_filt_policy );/* Initiator filter policy */
		RBLE_DATA_SET8( Data_p, connect_param->peer_addr_type );  /* Peer address type - 0=public/1=random */
		memcpy( Data_p, &connect_param->peer_addr.addr[ 0 ], ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) );
		Data_p += ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN );
		RBLE_DATA_SET8( Data_p, connect_param->own_addr_type );   /* Own address type - 0=public/1=random */
		RBLE_DATA_SET8( Data_p, connect_param->reserved );        /* Reserved */
		RBLE_DATA_SET16( Data_p, connect_param->con_intv_min );    /* Minimum of connection interval */
		RBLE_DATA_SET16( Data_p, connect_param->con_intv_max );    /* Maximum of connection interval */
		RBLE_DATA_SET16( Data_p, connect_param->con_latency );     /* Connection latency */
		RBLE_DATA_SET16( Data_p, connect_param->superv_to );       /* Link supervision timeout */
		RBLE_DATA_SET16( Data_p, connect_param->ce_len_min );      /* Minimum CE length */
		RBLE_DATA_SET16( Data_p, connect_param->ce_len_max );      /* Maximum CE length */
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Connection Cancel
 *
 *  Create Connection Cancel @n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	None
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Connection_Cancel(void)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}
	
	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = 0u;
		head_pnt->opcode		= RBLE_CMD_GAP_CONNECTION_CANCEL;

		/* Create Command Payloard */
				/* None */

	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Connection Disconnect
 *
 *  Connection Disconnect @n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	conhdl			Connection Handle
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Disconnect(uint16_t conhdl)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;
	uint8_t								*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint16_t );
		head_pnt->opcode		= RBLE_CMD_GAP_DISCONNECT;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET16( Data_p, conhdl );
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Start Bonding
 *
 *  Bonding Start @n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	bond_param		Bonding Parameter
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Start_Bonding(RBLE_BOND_PARAM *bond_param)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;
	uint8_t								*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = ( ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) + sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) );
		head_pnt->opcode		= RBLE_CMD_GAP_START_BONDING;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		memcpy( Data_p, &bond_param->addr.addr[ 0 ], ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) );
		Data_p += ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN );
		RBLE_DATA_SET8( Data_p, bond_param->oob );       /* OOB information */
		RBLE_DATA_SET8( Data_p, bond_param->iocap );     /* IO capabilities */
		RBLE_DATA_SET8( Data_p, bond_param->auth );      /* Authentication requirements */
		RBLE_DATA_SET8( Data_p, bond_param->key_size );  /* Encryption key size */
		RBLE_DATA_SET8( Data_p, bond_param->ikey_dist ); /* Initiator key distribution */
		RBLE_DATA_SET8( Data_p, bond_param->rkey_dist ); /* Responder key distribution */

	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Bonding Indo Indicate
 *
 *  Bonding Infomatin Indicate @n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	bond_op			Bonding Operation
 *  @param[in]	bond_param		Delete BD Address
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Bonding_Info_Ind(uint8_t bond_op, RBLE_BD_ADDR *addr)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;
	uint8_t								*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint8_t ) + ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN );
		head_pnt->opcode		= RBLE_CMD_GAP_BONDING_INFO_IND;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, bond_op );
		memcpy( Data_p, &addr->addr[ 0 ], ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) );
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Bonding Response
 *
 *  Resopnse to Bonding Start Request@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	res_bond_param	Bonding Parameter
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Bonding_Response(RBLE_BOND_RESP_PARAM *res_bond_param)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;
	uint8_t								*Data_p;
	
	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}
	
	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = ( sizeof( uint16_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) );
		head_pnt->opcode		= RBLE_CMD_GAP_BONDING_RESPONSE;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET16( Data_p, res_bond_param->conhdl );      /* Connection handle */
		RBLE_DATA_SET8( Data_p, res_bond_param->accept );       /* accept or reject bonding */
		RBLE_DATA_SET8( Data_p, res_bond_param->io_cap );       /* IO capabilities */
		RBLE_DATA_SET8( Data_p, res_bond_param->oob );          /* OOB flag */
		RBLE_DATA_SET8( Data_p, res_bond_param->auth_req );     /* Authentication Requirements */
		RBLE_DATA_SET8( Data_p, res_bond_param->max_key_size ); /* Max key size */
		RBLE_DATA_SET8( Data_p, res_bond_param->ikeys) ;        /* Initiator key distribution */
		RBLE_DATA_SET8( Data_p, res_bond_param->rkeys );        /* Responder key distribution */
		RBLE_DATA_SET8( Data_p, res_bond_param->reserved );
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Change Connection Param
 *
 *  Current Connection Parameter Change@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	conhdl			Connection Handle
 *  @param[in]	result			Result
 *  @param[in]	conn_param		Connection Parameter
 *  @param[in]	role			Device Role
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Change_Connection_Param(uint16_t conhdl, uint16_t result, RBLE_CONN_PARAM *conn_param, uint8_t role)
{
	RBLE_STATUS 							ret = RBLE_OK;
	RBLE_HEADER_t							*head_pnt;
	uint8_t									*Data_p;
	
	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}
	
	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = ( sizeof( uint16_t ) + sizeof( uint16_t ) + sizeof( uint16_t ) + sizeof( uint16_t ) + sizeof( uint16_t ) + sizeof( uint16_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) ); /* Add Reserve */
		head_pnt->opcode		= RBLE_CMD_GAP_CHANGE_CONNECTION_PARAM;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET16( Data_p, conhdl );
		RBLE_DATA_SET16( Data_p, result );
		RBLE_DATA_SET16( Data_p, conn_param->intv_min );    /* Connection interval minimum */
		RBLE_DATA_SET16( Data_p, conn_param->intv_max );    /* Connection interval maximum */
		RBLE_DATA_SET16( Data_p, conn_param->latency );     /* Latency */
		RBLE_DATA_SET16( Data_p, conn_param->time_out );    /* Supervision timeout */

		RBLE_DATA_SET8( Data_p, role );
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief 
 *
 *  @n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	update_map		Connection Handle
 *  @param[in]	con_hdl			Result
 *  @param[in]	chmap			Connection Parameter
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Channel_Map_Req(BOOL update_map, uint16_t conhdl, RBLE_LE_CHNL_MAP *chmap)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;
	uint8_t								*Data_p;
	
	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}
	
	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = ( sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint16_t ) + ( sizeof( uint8_t ) * RBLE_LE_CHNL_MAP_LEN ) + sizeof( uint8_t ) );
		head_pnt->opcode		= RBLE_CMD_GAP_CHANNEL_MAP_REQ;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, update_map );
		RBLE_DATA_SET8( Data_p, 0 );            /* Reserve */
		RBLE_DATA_SET16( Data_p, conhdl );
		memcpy( Data_p, &chmap->map[ 0 ], ( sizeof( uint8_t ) * RBLE_LE_CHNL_MAP_LEN ) );

	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Read RSSI
 *
 *  Read RSSI @n
 *  
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	conhdl				Connection Handle
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Read_RSSI(uint16_t conhdl)
{
	RBLE_STATUS							ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;
	uint8_t								*Data_p;
	
	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}
	
	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint16_t );
		head_pnt->opcode		= RBLE_CMD_GAP_READ_RSSI;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET16( Data_p, conhdl );

	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Authorized information indicate 
 *
 *  Authorized information indicate @n
 *  
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	conhdl				Connection Handle
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GAP_Authorized_Ind(uint16_t conhdl)
{
	RBLE_STATUS							ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;
	uint8_t								*Data_p;
	
	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}
	
	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint16_t );
		head_pnt->opcode		= RBLE_CMD_GAP_AUTHORIZED_INFO_IND;

		/* Create Command Payloard */
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET16( Data_p, conhdl );

	}
	return ret;
}

