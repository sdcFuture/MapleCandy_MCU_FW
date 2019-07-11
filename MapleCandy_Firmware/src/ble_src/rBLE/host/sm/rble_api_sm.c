/**
 ****************************************************************************************
 *
 * @file		rble_api_sm.c
 *
 * @brief	rBLE-Host I/F API Security Manager
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
 *  @brief Set Device Key
 *
 *  Device Key Setting@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Store Call Back
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in] key_code		Key Code
 *  @param[in] *key_value	Key Value Address
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_SM_Set_Key(uint8_t Key_code, RBLE_KEY_VALUE *Key_Value)
{
	RBLE_STATUS 						ret = RBLE_OK;
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
		head_pnt->packet_length = ( sizeof( uint8_t ) + ( sizeof( uint8_t ) * RBLE_KEY_LEN ) );
		head_pnt->opcode		= RBLE_CMD_SM_SET_KEY;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, Key_code );
		memcpy( Data_p, &Key_Value->key[ 0 ], ( sizeof( uint8_t ) * RBLE_KEY_LEN ) );    /* Key value */
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Start Encryption
 *
 *  Encryption Start@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in] idx			Connection Index
 *  @param[in] auth_req		Authentication Requirements
 *  @param[in] ediv			EDIV
 *  @param[in] rand_nb		Random Number
 *  @param[in] ltk			LTK Key Value
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_SM_Start_Enc(uint8_t idx, uint8_t auth_req, uint16_t ediv, RBLE_RAND_NB *rand_nb, RBLE_KEY_VALUE *ltk)
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
		head_pnt->packet_length = ( sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint16_t ) + ( sizeof( uint8_t ) * RBLE_RAND_NB_LEN ) + ( sizeof( uint8_t ) * RBLE_KEY_LEN ) );
		head_pnt->opcode		= RBLE_CMD_SM_START_ENC;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, idx );
		RBLE_DATA_SET8( Data_p, auth_req );
		RBLE_DATA_SET16( Data_p, ediv );

		memcpy( Data_p, &rand_nb->nb, ( sizeof( uint8_t ) * RBLE_RAND_NB_LEN ) );
		Data_p += ( sizeof( uint8_t ) * RBLE_RAND_NB_LEN );
		memcpy( Data_p, &ltk->key, ( sizeof( uint8_t ) * RBLE_KEY_LEN ) );
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Tk Request Response
 *
 *  Response to TK Request@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in] idx			Connection Index
 *  @param[in] status		Status
 *  @param[in] *tk			TK Key Value
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_SM_Tk_Req_Resp(uint8_t idx, uint8_t status, RBLE_KEY_VALUE *tk)
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
		head_pnt->packet_length = sizeof( uint8_t  ) + sizeof( uint8_t ) + ( sizeof( uint8_t ) * RBLE_KEY_LEN );
		head_pnt->opcode		= RBLE_CMD_SM_TK_REQ_RESP;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, idx );
		RBLE_DATA_SET8( Data_p, status );

		memcpy( Data_p, &tk->key, ( sizeof( uint8_t ) * RBLE_KEY_LEN ) );
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Ltk Request Response
 *
 *  Response to LTK Request@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in] idx			Connection Index
 *  @param[in] status		Status
 *  @param[in] sec_prop		Security properties
 *  @param[in] ediv			EDIV
 *  @param[in] nb			RANDOM Number
 *  @param[in] *ltk			LTK Key Value
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_SM_Ltk_Req_Resp(uint8_t idx, uint8_t status, uint8_t sec_prop, uint16_t ediv, RBLE_RAND_NB *nb, RBLE_KEY_VALUE *ltk)
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
		head_pnt->packet_length	= ( sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint16_t ) + ( sizeof( uint8_t ) * RBLE_RAND_NB_LEN ) + ( sizeof( uint8_t ) * RBLE_KEY_LEN ) );
		head_pnt->opcode		= RBLE_CMD_SM_LTK_REQ_RESP;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, idx );
		RBLE_DATA_SET8( Data_p, status );
		RBLE_DATA_SET8( Data_p, sec_prop );
		RBLE_DATA_SET8( Data_p, 0 );				/* Reserved */
		RBLE_DATA_SET16( Data_p, ediv );
		memcpy( Data_p, &nb->nb, ( sizeof( uint8_t ) * RBLE_RAND_NB_LEN ) );
		Data_p += ( sizeof( uint8_t ) * RBLE_RAND_NB_LEN );
		memcpy( Data_p, &ltk->key, ( sizeof( uint8_t ) * RBLE_KEY_LEN ) );

	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Irk Request Response
 *
 *  Response to Irk Request@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in] idx				Connection Index
 *  @param[in] status			Status
 *  @param[in] orig_addr		BD Address
 *  @param[in] *irk				IRK Key Value
 *  @param[in] lk_sec_status	Link Security Status
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_SM_Irk_Req_Resp(uint8_t idx, uint8_t status, RBLE_BD_ADDR *orig_addr, RBLE_KEY_VALUE *irk, uint8_t lk_sec_status)
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
		head_pnt->packet_length	= sizeof( uint8_t ) + sizeof( uint8_t ) + ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) + ( sizeof( uint8_t ) * RBLE_KEY_LEN ) + sizeof( uint8_t );
		head_pnt->opcode		= RBLE_CMD_SM_IRK_REQ_RESP;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, idx );
		RBLE_DATA_SET8( Data_p, status );
		memcpy( Data_p, &orig_addr->addr, ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN ) );
		Data_p += ( sizeof( uint8_t ) * RBLE_BD_ADDR_LEN );
		memcpy( Data_p, &irk->key, ( sizeof( uint8_t ) * RBLE_KEY_LEN ) );
		Data_p += ( sizeof( uint8_t ) * RBLE_KEY_LEN );
		RBLE_DATA_SET8( Data_p, lk_sec_status );
	}

	return ret;
}

/******************************************************************************************/
/**
 *  @brief Csrk Request Response
 *
 *  Response to Csrk Request@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in] idx			Connection Index
 *  @param[in] status		Status
 *  @param[in] *csrk		CSRK Key Value
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_SM_Csrk_Req_Resp(uint8_t idx, uint8_t status, RBLE_KEY_VALUE *csrk, uint8_t lk_sec_status)
{
	RBLE_STATUS 						ret = RBLE_OK;
	RBLE_HEADER_t						*head_pnt;
//	uint8_t								*Data_p;
	
	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}
	
	/* Parameter Check */

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length	= sizeof( uint8_t ) + sizeof( uint8_t ) + ( sizeof( uint8_t ) * RBLE_KEY_LEN  + sizeof( uint8_t ));
		head_pnt->opcode		= RBLE_CMD_SM_CSRK_REQ_RESP;

		// Create Command Payloard
#if 0   // for VC Compiler
		Data_p = &head_pnt->data[0];
		RBLE_DATA_SET8( Data_p, idx );
		RBLE_DATA_SET8( Data_p, status );
		memcpy( Data_p, &csrk->key, ( sizeof( uint8_t ) * RBLE_KEY_LEN ) );
		RBLE_DATA_SET8( Data_p, lk_sec_status );
#else
        head_pnt->data[ 0 ] = idx;
        head_pnt->data[ 1 ] = status;
		memcpy( &head_pnt->data[ 2 ], &csrk->key, ( sizeof( uint8_t ) * RBLE_KEY_LEN ) );
        head_pnt->data[ 2 + RBLE_KEY_LEN ] = lk_sec_status;
#endif
	}

	return ret;
}

/******************************************************************************************/
/**
 *  @brief Check BD Address Request Response
 *
 *  Response to Check BD Address Request@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Create Send Data
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in] idx			Connection Index
 *  @param[in] type			Address Check Type
 *  @param[in] status		Status
 *  @param[in] *addr		BD Address
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_SM_Chk_Bd_Addr_Req_Resp(uint8_t idx, uint8_t type, uint8_t found_flag, uint8_t lk_sec_status, RBLE_BD_ADDR *addr)
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
		head_pnt->packet_length	= ( sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + ( sizeof( uint8_t  ) * RBLE_BD_ADDR_LEN ) );
		head_pnt->opcode		= RBLE_CMD_SM_CHK_BD_ADDR_REQ_RESP;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, idx );
		RBLE_DATA_SET8( Data_p, type );
		RBLE_DATA_SET8( Data_p, found_flag );
		RBLE_DATA_SET8( Data_p, lk_sec_status );

		memcpy( Data_p, &addr->addr, ( sizeof( uint8_t  ) * RBLE_BD_ADDR_LEN ) );
	}

	return ret;
}

