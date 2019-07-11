/**
 ****************************************************************************************
 *
 * @file	rble_api_gatt.c
 *
 * @brief	rBLE-Host GATT I/F API
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

/**************************** Define Declaration ****************************************/
/**************************** Structure Declaration *************************************/
/**************************** PUBLIC Memory Declaration *********************************/
/**************************** PRIVATE Memory Declaration ********************************/
/**************************** PRIVATE PROTOTYPE Declaration *****************************/
/******************************* Program Erea *************************************/

/******************************************************************************************/
/**
 *  @brief GATT Enable Command
 *
 *  GATT Enable @n
 *  
 *  @attention		It's necessary to call this function before perform other functions
 *  
 *  @b Contents of management:
 *    -# check parameter
 *    -# register callback function
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	callback			GATT Call Back Function Address
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status Error
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GATT_Enable( RBLE_GATT_EVENT_HANDLER callback )
{
	RBLE_STATUS		ret = RBLE_OK;
	RBLE_HEADER_t	*head_pnt;

	/* Parameter Check */
	if (NULL != callback) {
		/* Status Check */
		if (RBLE_MODE_ACTIVE != rble_mode) {
			ret = RBLE_STATUS_ERROR;
		} else {
			RBLE_GATT_Info.Handler = callback;
		}
	} else {
		ret = RBLE_PARAM_ERR;
	}

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = 0u;
		head_pnt->opcode		= RBLE_CMD_GATT_ENABLE;

		/* No Parameters */

	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief GATT Service discovery request Command
 *
 *  GATT Service discovery request @n
 *
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	*disc_svc			Pointer to Service discovery request Parameter
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status Error
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GATT_Discovery_Service_Request( RBLE_GATT_DISC_SVC_REQ *disc_svc )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if (RBLE_MODE_ACTIVE != rble_mode) {
		ret = RBLE_STATUS_ERROR;
	}

	if (RBLE_OK == ret) {
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint8_t ) * 2 + sizeof( uint16_t ) * 4 + RBLE_GATT_128BIT_UUID_OCTET;
		head_pnt->opcode		= RBLE_CMD_GATT_DISCOVERY_SERVICE_REQUEST;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];

		RBLE_DATA_SET8( Data_p, disc_svc->req_type );
		Data_p++; /* skip to pad */
		RBLE_DATA_SET16( Data_p, disc_svc->conhdl );
		RBLE_DATA_SET16( Data_p, disc_svc->start_hdl );
		RBLE_DATA_SET16( Data_p, disc_svc->end_hdl );
		RBLE_DATA_SET16( Data_p, disc_svc->desired_svc.value_size );
		memcpy(Data_p, disc_svc->desired_svc.value, RBLE_GATT_128BIT_UUID_OCTET);

	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief GATT Characteristic discovery request Command
 *
 *  GATT Characteristic discovery request @n
 *
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	disc_char			Pointer to Characteristic discovery request parameter
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status Error
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GATT_Discovery_Char_Request( RBLE_GATT_DISC_CHAR_REQ *disc_char )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if (RBLE_MODE_ACTIVE != rble_mode) {
		ret = RBLE_STATUS_ERROR;
	}

	if (RBLE_OK == ret) {
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = ( sizeof( uint8_t ) * 2 ) + ( sizeof( uint16_t ) * 4 ) + RBLE_GATT_128BIT_UUID_OCTET;
		head_pnt->opcode		= RBLE_CMD_GATT_DISCOVERY_CHAR_REQUEST;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];

		RBLE_DATA_SET8( Data_p, disc_char->req_type );
		Data_p++; /* skip to pad */
		RBLE_DATA_SET16( Data_p, disc_char->conhdl );
		RBLE_DATA_SET16( Data_p, disc_char->start_hdl );
		RBLE_DATA_SET16( Data_p, disc_char->end_hdl );
		RBLE_DATA_SET16( Data_p, disc_char->desired_char.value_size );
		memcpy(Data_p, disc_char->desired_char.value, RBLE_GATT_128BIT_UUID_OCTET);

	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief GATT Characteristic descriptor discovery request Command
 *
 *  GATT Characteristic descriptor discovery request @n
 *
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	disc_char_desc		Pointer to Characteristic descriptor discovery request parameter
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status Error
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GATT_Discovery_Char_Descriptor_Request( RBLE_GATT_DISC_CHAR_DESC_REQ *disc_char_desc )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if (RBLE_MODE_ACTIVE != rble_mode) {
		ret = RBLE_STATUS_ERROR;
	}

	if (RBLE_OK == ret) {
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint16_t ) * 3;
		head_pnt->opcode		= RBLE_CMD_GATT_DISCOVERY_CHAR_DESCRIPTOR_REQUEST;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];

		RBLE_DATA_SET16( Data_p, disc_char_desc->conhdl );
		RBLE_DATA_SET16( Data_p, disc_char_desc->start_hdl );
		RBLE_DATA_SET16( Data_p, disc_char_desc->end_hdl );

	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief GATT Read characteristic request Command
 *
 *  GATT Read characteristic request @n
 *
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	rd_char				Pointer to Read Characteristic request parameter
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status Error
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GATT_Read_Char_Request( RBLE_GATT_READ_CHAR_REQ *rd_char )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;
	uint16_t					i;


	/* Status Check */
	if (RBLE_MODE_ACTIVE != rble_mode) {
		ret = RBLE_STATUS_ERROR;
	}

	if (RBLE_OK == ret) {
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint8_t ) * 2 + sizeof( uint16_t ) * 5 
								  + ( sizeof( uint8_t ) * 2 + RBLE_GATT_128BIT_UUID_OCTET ) * RBLE_GATT_MAX_NB_HDLS;
		head_pnt->opcode		= RBLE_CMD_GATT_READ_CHAR_REQUEST;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];

		RBLE_DATA_SET8( Data_p, rd_char->req_type );
		Data_p++; /* skip to pad */
		RBLE_DATA_SET16( Data_p, rd_char->offset );
		RBLE_DATA_SET16( Data_p, rd_char->conhdl );
		RBLE_DATA_SET16( Data_p, rd_char->start_hdl );
		RBLE_DATA_SET16( Data_p, rd_char->end_hdl );
		RBLE_DATA_SET16( Data_p, rd_char->nb_uuid );
		for (i = 0; i < RBLE_GATT_MAX_NB_HDLS; i++) {
			RBLE_DATA_SET8( Data_p,  rd_char->uuid[ i ].value_size );
			RBLE_DATA_SET8( Data_p,  rd_char->uuid[ i ].expect_resp_size );
			memcpy(Data_p, rd_char->uuid[ i ].value, RBLE_GATT_128BIT_UUID_OCTET);
			Data_p += RBLE_GATT_128BIT_UUID_OCTET;
		}

	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief GATT Write characteristic request Command
 *
 *  GATT Write characteristic request @n
 *
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	wr_char				Pointer to Write Characteristic request parameter
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status Error
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GATT_Write_Char_Request( RBLE_GATT_WRITE_CHAR_REQ *wr_char )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if (RBLE_MODE_ACTIVE != rble_mode) {
		ret = RBLE_STATUS_ERROR;
	}

	if (RBLE_OK == ret) {
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint16_t ) * 4 + sizeof( uint8_t ) * 2 + RBLE_GATT_MAX_LONG_VALUE;
		head_pnt->opcode		= RBLE_CMD_GATT_WRITE_CHAR_REQUEST;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];

		RBLE_DATA_SET16( Data_p, wr_char->conhdl );
		RBLE_DATA_SET16( Data_p, wr_char->charhdl );
		RBLE_DATA_SET16( Data_p, wr_char->wr_offset );
		RBLE_DATA_SET16( Data_p, wr_char->val_len );
		RBLE_DATA_SET8( Data_p, wr_char->req_type );
		RBLE_DATA_SET8( Data_p, wr_char->auto_execute );
		memcpy(Data_p, wr_char->value, RBLE_GATT_MAX_LONG_VALUE);

	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief GATT Write reliable characteristic request Command
 *
 *  GATT Write reliable characteristic request @n
 *
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	rel_write			Pointer to Write reliable Characteristic request parameter
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status Error
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GATT_Write_Reliable_Request( RBLE_GATT_WRITE_RELIABLE_REQ *rel_write )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;
	uint16_t					i;


	/* Status Check */
	if (RBLE_MODE_ACTIVE != rble_mode) {
		ret = RBLE_STATUS_ERROR;
	}

	if (RBLE_OK == ret) {
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint8_t ) * 2 + sizeof( uint16_t ) * 1 
								  + ( sizeof( uint16_t ) * 2 + RBLE_GATT_MAX_RELIABLE_WRITE_CONTENTS ) * RBLE_GATT_MAX_RELIABLE_WRITE_NUM;
		head_pnt->opcode		= RBLE_CMD_GATT_WRITE_RELIABLE_REQUEST;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];

		RBLE_DATA_SET8( Data_p, rel_write->nb_writes );
		RBLE_DATA_SET8( Data_p, rel_write->auto_execute );
		RBLE_DATA_SET16( Data_p, rel_write->conhdl );

		for (i = 0; i < RBLE_GATT_MAX_RELIABLE_WRITE_NUM; i++) {
			RBLE_DATA_SET16( Data_p, rel_write->value[ i ].elmt_hdl );
			RBLE_DATA_SET16( Data_p, rel_write->value[ i ].size );
			memcpy(Data_p, rel_write->value[ i ].value, RBLE_GATT_MAX_RELIABLE_WRITE_CONTENTS);
			Data_p += RBLE_GATT_MAX_RELIABLE_WRITE_CONTENTS;
		}

	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief GATT Execute write characteristic request Command
 *
 *  GATT Execute write characteristic request @n
 *
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	exe_wr_char			Pointer to Execute write Characteristic request parameter
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status Error
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GATT_Execute_Write_Char_Request( RBLE_GATT_EXE_WR_CHAR_REQ *exe_wr_char )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if (RBLE_MODE_ACTIVE != rble_mode) {
		ret = RBLE_STATUS_ERROR;
	}

	if (RBLE_OK == ret) {
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint8_t ) * 2 + sizeof( uint16_t ) * 1;
		head_pnt->opcode		= RBLE_CMD_GATT_EXECUTE_WRITE_CHAR_REQUEST;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];

		RBLE_DATA_SET8( Data_p, exe_wr_char->exe_wr_ena );
		Data_p++; /* skip to pad */
		RBLE_DATA_SET16( Data_p, exe_wr_char->conhdl );

	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief GATT Notification request Command
 *
 *  GATT Notification request request @n
 *
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	notify				Pointer to Notification request parameter
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status Error
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GATT_Notify_Request( RBLE_GATT_NOTIFY_REQ *notify )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if (RBLE_MODE_ACTIVE != rble_mode) {
		ret = RBLE_STATUS_ERROR;
	}

	if (RBLE_OK == ret) {
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint16_t ) * 2;
		head_pnt->opcode		= RBLE_CMD_GATT_NOTIFY_REQUEST;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];

		RBLE_DATA_SET16( Data_p, notify->conhdl );
		RBLE_DATA_SET16( Data_p, notify->charhdl );

	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief GATT Indication request Command
 *
 *  GATT Indication request request @n
 *
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	indicate			Pointer to Indication request parameter
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status Error
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GATT_Indicate_Request( RBLE_GATT_INDICATE_REQ *indicate )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if (RBLE_MODE_ACTIVE != rble_mode) {
		ret = RBLE_STATUS_ERROR;
	}

	if (RBLE_OK == ret) {
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint16_t ) * 2;
		head_pnt->opcode		= RBLE_CMD_GATT_INDICATE_REQUEST;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];

		RBLE_DATA_SET16( Data_p, indicate->conhdl );
		RBLE_DATA_SET16( Data_p, indicate->charhdl );

	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief GATT Write Response Command
 *
 *  GATT Write Response for Write Command @n
 *
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	*wr_resp		Pointer to Write Response parameter
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status Error
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GATT_Write_Response( RBLE_GATT_WRITE_RESP *wr_resp )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if (RBLE_MODE_ACTIVE != rble_mode) {
		ret = RBLE_STATUS_ERROR;
	}

	if (RBLE_OK == ret) {
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = ( sizeof( uint16_t ) * 2 ) + ( sizeof( uint8_t ) * 2 );
		head_pnt->opcode		= RBLE_CMD_GATT_WRITE_RESP;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];

		RBLE_DATA_SET16( Data_p, wr_resp->conhdl );
		RBLE_DATA_SET16( Data_p, wr_resp->att_hdl );
		RBLE_DATA_SET8( Data_p, wr_resp->att_code );
		/* Pad */
	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief GATT Set Permission Command
 *
 *  GATT Set Permission of local db @n
 *
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	*set_perm		Pointer to Set Permission parameter
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status Error
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GATT_Set_Permission( RBLE_GATT_SET_PERM *set_perm )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if (RBLE_MODE_ACTIVE != rble_mode) {
		ret = RBLE_STATUS_ERROR;
	}

	if (RBLE_OK == ret) {
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = ( sizeof( uint16_t ) * 3 );
		head_pnt->opcode		= RBLE_CMD_GATT_SET_PERM;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];

		RBLE_DATA_SET16( Data_p, set_perm->start_hdl );
		RBLE_DATA_SET16( Data_p, set_perm->end_hdl );
		RBLE_DATA_SET16( Data_p, set_perm->perm );
	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief GATT Set Data Command
 *
 *  GATT Set Data to local db @n
 *
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	*set_data		Pointer to Set Data parameter
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status Error
 */
/******************************************************************************************/
RBLE_STATUS RBLE_GATT_Set_Data( RBLE_GATT_SET_DATA *set_data )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if (RBLE_MODE_ACTIVE != rble_mode) {
		ret = RBLE_STATUS_ERROR;
	}

	if (RBLE_OK == ret) {
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = ( sizeof( uint16_t ) * 2 ) + ( sizeof( uint8_t ) * RBLE_GATT_MAX_LONG_VALUE );
		head_pnt->opcode		= RBLE_CMD_GATT_SET_DATA;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];

		RBLE_DATA_SET16( Data_p, set_data->val_hdl );
		RBLE_DATA_SET16( Data_p, set_data->val_len );
		memcpy(Data_p, set_data->value, RBLE_GATT_MAX_LONG_VALUE);
	}
	return ret;
}

