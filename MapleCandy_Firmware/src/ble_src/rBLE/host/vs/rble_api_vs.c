/**
 ****************************************************************************************
 *
 * @file		rble_api_vs.c
 *
 * @brief	rBLE-Host I/F API Vendor Specific
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
uint8_t *access_buf = NULL;

/**************************** PRIVATE Memory Declaration ******************************/

/**************************** PRIVATE PROTOTYPE Declaration ***************************/

/******************************* Program Erea *********************************************/
/******************************************************************************************/
/**
 *  @brief Vendor Specific Command Enable
 *
 *  Vendor Specific Command Enable@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Store Call Back
 *
 *  @param[in] *call_back	Call Back API Address
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter Error
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_VS_Enable( RBLE_VS_EVENT_HANDLER callback )
{
	RBLE_STATUS 	ret = RBLE_OK;
	RBLE_HEADER_t	*head_pnt;

	/* Parameter Check */
	if(NULL == callback){
		ret = RBLE_PARAM_ERR;
	} else {
		/* Status Check */
		if(RBLE_MODE_ACTIVE != rble_mode){
			ret = RBLE_STATUS_ERROR;
		} else {
			RBLE_VS_Info.Handler = callback;
			/* Create Command Packet Header */
			head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
			head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
			head_pnt->packet_length = 0u;
			head_pnt->opcode		= RBLE_CMD_VS_ENABLE;

		}
	}
	return( ret );
}

/******************************************************************************************/
/**
 *  @brief Test Rx Start
 *
 *  Test Rx Start@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	rx_freq			RX Frequency
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_VS_Test_Rx_Start(uint8_t rx_freq)
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint8_t );
		head_pnt->opcode		= RBLE_CMD_VS_TEST_RX_START;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, rx_freq );
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Test Tx Start
 *
 *  Test Tx Start@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	tx_freq			TX Frequency
 *  @param[in]	test_data_len	Test Data Length
 *  @param[in]	pk_payload_type	Packet Payload Type
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_VS_Test_Tx_Start(uint8_t tx_freq, uint8_t test_data_len, uint8_t pk_payload_type)
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t );
		head_pnt->opcode		= RBLE_CMD_VS_TEST_TX_START;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, tx_freq );
		RBLE_DATA_SET8( Data_p, test_data_len );
		RBLE_DATA_SET8( Data_p, pk_payload_type );
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Test End
 *
 *  Test End@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# Send Message to rBLE Core Task
 *
 *  @param[in]	None
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_VS_Test_End(void)
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = 0u;
		head_pnt->opcode		= RBLE_CMD_VS_TEST_END;
	}
	
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Set Parrameters Command for Direct Test
 *
 *  Set Parrameters Command for Direct Test@n
 * 
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	rx_nb_packet		number of rx packets
 *  @param[in]	tx_nb_packet		number of tx packets
 *  @param[in]	infinite_setting	enable burst transmission
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_VS_Set_Test_Parameter( uint16_t rx_nb_packet, uint16_t tx_nb_packet, uint8_t infinite_setting )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint16_t ) * 2 + sizeof( uint8_t ) * 2;       /* Add Reserve */
		head_pnt->opcode		= RBLE_CMD_VS_SET_TEST_PARAMETER;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET16( Data_p, rx_nb_packet );
		RBLE_DATA_SET16( Data_p, tx_nb_packet );
		RBLE_DATA_SET8( Data_p, infinite_setting );

	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Read RSSI while Direct Test
 *
 *  Read RSSI while Direct Test@n
 * 
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	None
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_VS_Read_Test_RSSI( void )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = 0u;
		head_pnt->opcode		= RBLE_CMD_VS_READ_TEST_RSSI;

		/* No Parameters */

	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Write BD Adsress Command
 *
 *  Write BD Adsress Command@n
 * 
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	address			Pointer to Bd Address
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_VS_Write_Bd_Address( RBLE_BD_ADDR *address )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( RBLE_BD_ADDR );
		head_pnt->opcode		= RBLE_CMD_VS_WRITE_BD_ADDRESS;

		// Create Command Payloard
		memcpy(head_pnt->data, address->addr, head_pnt->packet_length );

	}
	return ret;
}


/******************************************************************************************/
/**
 *  @brief Set Tx Power Command
 *
 *  Set Tx Power Command@n
 * 
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	conhdl				connection handle
 *              power_lvl			tx power level
 *              state				adapt state
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_VS_Set_Tx_Power( uint16_t conhdl, uint8_t power_lvl, uint8_t state )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint16_t ) + sizeof( uint8_t ) + sizeof( uint8_t );
		head_pnt->opcode		= RBLE_CMD_VS_SET_TX_POWER;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET16( Data_p, conhdl );
		RBLE_DATA_SET8( Data_p, power_lvl );
		RBLE_DATA_SET8( Data_p, state );

	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief GPIO Dir Command
 *
 *  GPIO Dir Command@n
 * 
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	dir					direction
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_VS_GPIO_Dir( uint8_t dir )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint8_t );
		head_pnt->opcode		= RBLE_CMD_VS_GPIO_DIR;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, dir );

	}
	return ret;
}


/******************************************************************************************/
/**
 *  @brief GPIO Access Command
 *
 *  GPIO Access Command@n
 * 
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	mode				access mode
 *  			value				output value
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_VS_GPIO_Access( uint8_t mode, uint8_t value )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint8_t ) + sizeof( uint8_t );
		head_pnt->opcode		= RBLE_CMD_VS_GPIO_ACCESS;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, mode );
		RBLE_DATA_SET8( Data_p, value );

	}
	return ret;
}


/******************************************************************************************/
/**
 *  @brief Data Flash Management Command
 *
 *  Data Flash Management Command@n
 * 
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	cmd					start or stop
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_VS_Flash_Management( uint8_t cmd )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint8_t );
		head_pnt->opcode		= RBLE_CMD_VS_FLASH_MANAGEMENT;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, cmd );

	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Data Flash Access Command
 *
 *  Data Flash Access Command@n
 * 
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	param				access parameter
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_VS_Flash_Access( RBLE_VS_FLASH_ACCESS_PARAM *param )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint16_t					cnt;

	/* Status Check */
	if( (RBLE_MODE_ACTIVE != rble_mode) && (param->addr != NULL)){
		ret = RBLE_STATUS_ERROR;
	}
	else if(param->addr == NULL) {
		ret = RBLE_PARAM_ERR;
	}
	else {
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->opcode		= RBLE_CMD_VS_FLASH_ACCESS;
		head_pnt->packet_length = sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t ) + sizeof( uint8_t );

		if(param->cmd == RBLE_VS_FLASH_CMD_WRITE) {
			head_pnt->packet_length += param->size;
		}
		
		/* Create Command Payloard */
		RBLE_Data_Set_Start( (RBLE_HEADER_t *)head_pnt );		/* Start Packet Data Setting */
		
		RBLE_Data_Set8( param->cmd );
		RBLE_Data_Set8( param->id );
		RBLE_Data_Set8( param->size );
		RBLE_Data_Set8( 0 );		/* param.reserved */
		
		if(param->cmd == RBLE_VS_FLASH_CMD_WRITE) {
			for(cnt=0; cnt < param->size; cnt++)
			{
				RBLE_Data_Set8(param->addr[cnt]);
			}
		}
		RBLE_Data_Set_End();		/* End Packet Data Setting */
		
		/* regist buffer address */
		access_buf = param->addr;
	}
	return ret;
}


/******************************************************************************************/
/**
 *  @brief Data Flash Operation Command
 *
 *  Data Flash Operation Command@n
 * 
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	cmd					cleanup or format
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_VS_Flash_Operation( uint8_t cmd )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint8_t );
		head_pnt->opcode		= RBLE_CMD_VS_FLASH_OPERATION;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, cmd );

	}
	return ret;
}


/******************************************************************************************/
/**
 *  @brief Data Flash Get Space Command
 *
 *  Data Flash Get Space Command@n
 * 
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	none
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_VS_Flash_Get_Space( void )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = 0u;
		head_pnt->opcode		= RBLE_CMD_VS_FLASH_GET_SPACE;
	}
	return ret;
}


/******************************************************************************************/
/**
 *  @brief Data Flash Get EEL Version Command
 *
 *  Data Flash Get EEL Version Command@n
 * 
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	none
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_VS_Flash_Get_EEL_Ver( void )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = 0u;
		head_pnt->opcode		= RBLE_CMD_VS_FLASH_GET_EEL_VER;
	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Adaptable Function Enable Command
 *
 *  Adaptable Function Enable Dir Command@n
 * 
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	cmd					command
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_VS_Adapt_Enable( uint8_t cmd )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint8_t );
		head_pnt->opcode		= RBLE_CMD_VS_ADAPT_ENABLE;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, cmd );

	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief RF Control Command
 *
 *  RF Control Command@n
 * 
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	cmd					command
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_VS_RF_Control( uint8_t cmd )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint8_t );
		head_pnt->opcode		= RBLE_CMD_VS_RF_CONTROL;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, cmd );

	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief Set Parameter Command
 *
 *  Set Parameter Command@n
 *
 *  @b Contents of management:
 *    -# Check rBLE Mode
 *    -# Create Send Data
 *    -# Send Message to HOST rBLE Task
 *
 *  @param[in]	param_id			parameter id
 *              param_len			length of parameter
 *              param_data			parameter data
 *
 *  @retval		RBLE_OK				Success
 *  @retval		RBLE_PARAM_ERR		Parameter ERROR
 *  @retval		RBLE_STATUS_ERROR	Status ERROR
 */
/******************************************************************************************/
RBLE_STATUS RBLE_VS_Set_Params( uint8_t param_id, uint8_t param_len, uint8_t *param_data )
{
	RBLE_STATUS					ret = RBLE_OK;
	RBLE_HEADER_t				*head_pnt;
	uint8_t						*Data_p;

	/* Status Check */
	if(RBLE_MODE_ACTIVE != rble_mode){
		ret = RBLE_STATUS_ERROR;
	}

	if(param_len > (RBLE_MAX_PAYLOARD_SIZE - RBLE_HEADER_SIZE - (sizeof(uint8_t) * 2))) {
		ret = RBLE_PARAM_ERR;
	}

	if(RBLE_OK == ret){
		/* Create Command Packet Header */
		head_pnt = (RBLE_HEADER_t *)RBLE_Get_CMD_Buffer();
		head_pnt->packet_type	= RBLE_PACKET_TYPE_COMMAND;
		head_pnt->packet_length = sizeof( uint8_t ) + sizeof( uint8_t ) + param_len;
		head_pnt->opcode		= RBLE_CMD_VS_SET_PARAMS;

		// Create Command Payloard
		Data_p = &head_pnt->data[ 0 ];
		RBLE_DATA_SET8( Data_p, param_id );
		RBLE_DATA_SET8( Data_p, param_len );
		memcpy(Data_p, param_data, param_len );
	}
	return ret;
}


