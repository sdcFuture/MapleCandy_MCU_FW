/**
 ****************************************************************************************
 *
 * @file		rble_host.c
 *
 * @brief	rBLE-Host Task
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
#include	"rscip.h"
#include	"rscip_cntl.h"
#include	"timer.h"

/**************************** define Declaration **************************************/
#define MAX_BUFF_NUM		4

/**************************** structure Declaration ***********************************/

/**************************** PUBLIC Memory Declaration *******************************/
RBLE_INIT_CB	rble_init_cb;									/* Call Back Function */
RBLE_MODE		rble_mode;										/* rBLE Internal Mode */

RBLE_OVER_PACKET_INFO	RBLE_OVER_PACKET_info;
/**************************** PRIVATE Memory Declaration ******************************/
int_t	evt_buff_rd_cnt;								/* Event Buffer Read Counter */
int_t	evt_buff_wr_cnt;								/* Event Buffer Write Counter */
int_t	cmd_buff_rd_cnt;								/* Command Buffer Read Counter */
int_t	cmd_buff_wr_cnt;								/* Command Buffer write Counter */
uint8_t	cmd_buf[MAX_BUFF_NUM][RBLE_MAX_PAYLOARD_SIZE];	/* Commnad Data Buffer */
uint8_t	evt_buf[MAX_BUFF_NUM][RBLE_MAX_PAYLOARD_SIZE];	/* Event Data Buffer */

int_t		Tx_Event_Wr_Cnt;
int_t		Tx_Event_Rd_Cnt;
int_t		Rx_Event_Wr_Cnt;
int_t		Rx_Event_Rd_Cnt;

uint8_t cmd_buff_used_count;
/**************************** PRIVATE PROTOTYPE Declaration ***************************/
static RBLE_STATUS RBLE_Inititilize(void);
static void RBLE_Host_active_call_back(void);
static bool RBLE_Host_read_call_back( RSCIP_PACKET_TYPE type, uint8_t *bufptr, uint16_t size);
static void RBLE_Host_write_call_back( uint8_t *bufptr );
static void RBLE_Host_error_call_back( RSCIP_RESULT_CODE error );
static void RBLE_Host_reset_call_back(void);

/**************************** Extern Prototype **********************************/
RBLE_GAP_INFO	RBLE_GAP_Info;
RBLE_SM_INFO	RBLE_SM_Info;
RBLE_GATT_INFO	RBLE_GATT_Info;
#if (PRF_SEL_FMPL || PRF_SEL_FMPT)
RBLE_FMPL_INFO	RBLE_FMPL_Info;
RBLE_FMPT_INFO	RBLE_FMPT_Info;
#endif
#if (PRF_SEL_HTPC || PRF_SEL_HTPT)
RBLE_HTPC_INFO	RBLE_HTPC_Info;
RBLE_HTPT_INFO	RBLE_HTPT_Info;
#endif
#if (PRF_SEL_PXPM || PRF_SEL_PXPR)
RBLE_PXPM_INFO	RBLE_PXPM_Info;
RBLE_PXPR_INFO	RBLE_PXPR_Info;
#endif
#if (PRF_SEL_BLPC || PRF_SEL_BLPS)
RBLE_BLPC_INFO	RBLE_BLPC_Info;
RBLE_BLPS_INFO	RBLE_BLPS_Info;
#endif
#if (PRF_SEL_HGHD || PRF_SEL_HGBH || PRF_SEL_HGRH)
RBLE_HGHD_INFO	RBLE_HGHD_Info;
RBLE_HGBH_INFO	RBLE_HGBH_Info;
RBLE_HGRH_INFO	RBLE_HGRH_Info;
#endif
#if (PRF_SEL_SPPS || PRF_SEL_SPPC)
RBLE_SPPC_INFO	RBLE_SPPC_Info;
RBLE_SPPS_INFO	RBLE_SPPS_Info;
#endif
#if (PRF_SEL_HRPC || PRF_SEL_HRPS)
RBLE_HRPC_INFO	RBLE_HRPC_Info;
RBLE_HRPS_INFO	RBLE_HRPS_Info;
#endif
#if (PRF_SEL_CSCC || PRF_SEL_CSCS)
RBLE_CSCPC_INFO	RBLE_CSCPC_Info;
RBLE_CSCPS_INFO	RBLE_CSCPS_Info;
#endif
#if (PRF_SEL_GLPC || PRF_SEL_GLPS)
RBLE_GLPC_INFO	RBLE_GLPC_Info;
RBLE_GLPS_INFO	RBLE_GLPS_Info;
#endif
#if (PRF_SEL_CPPS || PRF_SEL_CPPC)
RBLE_CPPC_INFO	RBLE_CPPC_Info;
RBLE_CPPS_INFO	RBLE_CPPS_Info;
#endif
#if (PRF_SEL_TIPS || PRF_SEL_TIPC)
RBLE_TIPC_INFO	RBLE_TIPC_Info;
RBLE_TIPS_INFO	RBLE_TIPS_Info;
#endif
#if (PRF_SEL_ANPS || PRF_SEL_ANPC)
RBLE_ANPC_INFO	RBLE_ANPC_Info;
RBLE_ANPS_INFO	RBLE_ANPS_Info;
#endif
#if (PRF_SEL_LNPS || PRF_SEL_LNPC)
RBLE_LNPC_INFO	RBLE_LNPC_Info;
RBLE_LNPS_INFO	RBLE_LNPS_Info;
#endif
#if (PRF_SEL_PASS || PRF_SEL_PASC)
RBLE_PASPC_INFO	RBLE_PASPC_Info;
RBLE_PASPS_INFO	RBLE_PASPS_Info;
#endif
#if (PRF_SEL_RSCS || PRF_SEL_RSCC)
RBLE_RSCPC_INFO	RBLE_RSCPC_Info;
RBLE_RSCPS_INFO	RBLE_RSCPS_Info;
#endif
RBLE_VS_INFO	RBLE_VS_Info;

/******************************* Program Area *********************************************/

/******************************************************************************************/
/**
 *  @brief rBLE Initialize
 *
 *  This function does the initialization of the rBLE system.@n
 *  It is informed of the result by the function call.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Parameter Check
 *    -# The registration of the call function
 *    -# Create rBLE Task
 *    -# The initialization command of rBLE is transmitted
 *
 *  @param[in]	call_back		Event notice function
 *
 *  @retval		RBLE_OK			Success
 *  @retval		RBLE_ERR		Error
 *  @retval		RBLE_PARAM_ERR	Parameter Error
 */
/******************************************************************************************/
RBLE_STATUS RBLE_Init(RBLE_INIT_CB call_back)
{
	RBLE_STATUS ret = RBLE_OK;
	
	/* rBLE Default State is Initilize State*/
	rble_mode= RBLE_MODE_INITIALIZE;

	/* Parameter Check */
	if ( NULL == call_back ) {
		ret = RBLE_PARAM_ERR;
	}else{
		/* A function is stored. */
		rble_init_cb = call_back;

		ret = RBLE_Inititilize();
	}
	return ret;
}

/******************************************************************************************/
/**
 *  @brief rBLE Exit
 *
 *  This function finishes a rBLE system.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# The exit command of rBLE is transmitted
 *
 *  @param[in]
 *
 *  @retval
 */
/******************************************************************************************/
void rBLE_Exit( void )
{
	serial_exit();
}

/******************************************************************************************/
/**
 *  @brief rBLE Host Task
 *
 *  The transmission of the command to Lower-rBLE and event reception from Lower-rBLE are done.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Receive Message
 *    -# Command request-to-send
 *    -# Event request-to-receive
 *    -# rBLE control event
 *  @param[in]
 *
 *  @retval
 */
/******************************************************************************************/
void rBLE_Run(void)
{
	RSCIP_RESULT_CODE	rscip_ret;
	RBLE_HEADER_t   	*header_pnt;
	int_t				cmd_buff_rd_cnt_tmp;

	if ( cmd_buff_wr_cnt != cmd_buff_rd_cnt ) {
		cmd_buff_rd_cnt_tmp = ( cmd_buff_rd_cnt + 1 ) & ( MAX_BUFF_NUM - 1 );
		/* Buffer Address Get */
		header_pnt = ( RBLE_HEADER_t * )( &cmd_buf[cmd_buff_rd_cnt_tmp][0] );
#ifdef RBLE_DEBUG
		rBLE_Data_Disp(header_pnt);
#endif
		/* Data transmission to RSCIP */
		rscip_ret = RSCIP_Write( RSCIP_RBLE_COMMAND, (uint8_t *)header_pnt, ( uint16_t )( header_pnt->packet_length + RBLE_HEADER_SIZE ) );
		/* Change Return Code */
		switch( rscip_ret ){
			case RSCIP_OK:
				cmd_buff_rd_cnt = cmd_buff_rd_cnt_tmp;
				break;
			case RSCIP_INVALID_PARAMTER:
				break;
			case RSCIP_BUSY:
				break;
			case RSCIP_ERROR:
				break;
			default:
				break;
		}
	}
	if ( evt_buff_wr_cnt != evt_buff_rd_cnt ) {
		evt_buff_rd_cnt = ( evt_buff_rd_cnt + 1 ) & ( MAX_BUFF_NUM - 1 );
		/* Buffer Address Get */
		header_pnt = ( RBLE_HEADER_t * )( &evt_buf[ evt_buff_rd_cnt ][ 0 ] );
#ifdef RBLE_DEBUG
		rBLE_Data_Disp(header_pnt);
#endif
		/* Event data analysis */
		RBLE_Event_Data_Analisys( ( uint8_t * )header_pnt );
	}

	if ( Tx_Event_Rd_Cnt != Tx_Event_Wr_Cnt ) {
		Tx_Event_Rd_Cnt++;
		RSCIP_Tx_Done();
	}
	if ( Rx_Event_Rd_Cnt != Rx_Event_Wr_Cnt ) {
		Rx_Event_Rd_Cnt++;
		RSCIP_Rx_Done();
	}

	/* Timer Check */
 	rBLE_Is_Timeup_Call_Func();
}

/******************************************************************************************/
/**
 *  @brief      Check if rBLE is idle state or not
 *              (this is additional function for host sample program)
 *
 *  @attention  It is necessary to forbid interruption before check state
 *  
 *  @param      none
 *
 *  @retval     TRUE if idle state, FALSE if ready state
 */
/******************************************************************************************/
bool rBLE_Is_Idle( void )
{
    bool idle = FALSE;

    if((cmd_buff_wr_cnt == cmd_buff_rd_cnt) &&
       (evt_buff_wr_cnt == evt_buff_rd_cnt) &&
       (Tx_Event_Rd_Cnt == Tx_Event_Wr_Cnt) &&
       (Rx_Event_Rd_Cnt == Rx_Event_Wr_Cnt))
    {
        idle = TRUE;
    }
    return idle;
}

/******************************************************************************************/
/**
 *  @brief The initialization of rBLE-Profiles
 *
 *  initialization of rBLE-Profiles.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# The initialization of rBLE Profiles
 *  @param[in]
 *
 *  @retval
 */
/******************************************************************************************/
void rBLE_Profile_Init( void )
{
	#if (PRF_SEL_FMPL || PRF_SEL_FMPT)
	memset(&RBLE_FMPL_Info,0x00,sizeof(RBLE_FMPL_INFO));
	memset(&RBLE_FMPT_Info,0x00,sizeof(RBLE_FMPT_INFO));
	#endif
	#if (PRF_SEL_HTPC || PRF_SEL_HTPT)
	memset(&RBLE_HTPC_Info,0x00,sizeof(RBLE_HTPC_INFO));
	memset(&RBLE_HTPT_Info,0x00,sizeof(RBLE_HTPT_INFO));
	#endif
	#if (PRF_SEL_PXPM || PRF_SEL_PXPR)
	memset(&RBLE_PXPM_Info,0x00,sizeof(RBLE_PXPM_INFO));
	memset(&RBLE_PXPR_Info,0x00,sizeof(RBLE_PXPR_INFO));
	#endif
	#if (PRF_SEL_BLPC || PRF_SEL_BLPS)
	memset(&RBLE_BLPC_Info,0x00,sizeof(RBLE_BLPC_INFO));
	memset(&RBLE_BLPS_Info,0x00,sizeof(RBLE_BLPS_INFO));
	#endif
	#if (PRF_SEL_HGHD || PRF_SEL_HGBH || PRF_SEL_HGRH)
	memset(&RBLE_HGHD_Info,0x00,sizeof(RBLE_HGHD_INFO));
	memset(&RBLE_HGBH_Info,0x00,sizeof(RBLE_HGBH_INFO));
	memset(&RBLE_HGRH_Info,0x00,sizeof(RBLE_HGRH_INFO));
	#endif
	#if (PRF_SEL_SPPS || PRF_SEL_SPPC)
	memset(&RBLE_SPPC_Info,0x00,sizeof(RBLE_SPPC_INFO));
	memset(&RBLE_SPPS_Info,0x00,sizeof(RBLE_SPPS_INFO));
	#endif
	#if (PRF_SEL_HRPC || PRF_SEL_HRPS)
	memset(&RBLE_HRPC_Info,0x00,sizeof(RBLE_HRPC_INFO));
	memset(&RBLE_HRPS_Info,0x00,sizeof(RBLE_HRPS_INFO));
	#endif
	#if (PRF_SEL_CSCC || PRF_SEL_CSCS)
	memset(&RBLE_CSCPC_Info,0x00,sizeof(RBLE_CSCPC_INFO));
	memset(&RBLE_CSCPS_Info,0x00,sizeof(RBLE_CSCPS_INFO));
	#endif
	#if (PRF_SEL_GLPC || PRF_SEL_GLPS)
	memset(&RBLE_GLPC_Info,0x00,sizeof(RBLE_GLPC_INFO));
	memset(&RBLE_GLPS_Info,0x00,sizeof(RBLE_GLPS_INFO));
	#endif
	#if (PRF_SEL_CPPS || PRF_SEL_CPPC)
	memset(&RBLE_CPPC_Info,0x00,sizeof(RBLE_CPPC_INFO));
	memset(&RBLE_CPPS_Info,0x00,sizeof(RBLE_CPPS_INFO));
	#endif
	#if (PRF_SEL_TIPS || PRF_SEL_TIPC)
	memset(&RBLE_TIPC_Info,0x00,sizeof(RBLE_TIPC_INFO));
	memset(&RBLE_TIPS_Info,0x00,sizeof(RBLE_TIPS_INFO));
	#endif
	#if (PRF_SEL_ANPS || PRF_SEL_ANPC)
	memset(&RBLE_ANPC_Info,0x00,sizeof(RBLE_ANPC_INFO));
	memset(&RBLE_ANPS_Info,0x00,sizeof(RBLE_ANPS_INFO));
	#endif
	#if (PRF_SEL_LNPS || PRF_SEL_LNPC)
	memset(&RBLE_LNPC_Info,0x00,sizeof(RBLE_LNPC_INFO));
	memset(&RBLE_LNPS_Info,0x00,sizeof(RBLE_LNPS_INFO));
	#endif
	#if (PRF_SEL_PASS || PRF_SEL_PASC)
	memset(&RBLE_PASPC_Info,0x00,sizeof(RBLE_PASPC_INFO));
	memset(&RBLE_PASPS_Info,0x00,sizeof(RBLE_PASPS_INFO));
	#endif
	#if (PRF_SEL_RSCS || PRF_SEL_RSCC)
	memset(&RBLE_RSCPC_Info,0x00,sizeof(RBLE_RSCPC_INFO));
	memset(&RBLE_RSCPS_Info,0x00,sizeof(RBLE_RSCPS_INFO));
	#endif
	memset(&RBLE_VS_Info,0x00,sizeof(RBLE_VS_INFO));
}

/******************************************************************************************/
/**
 *  @brief The initialization of rBLE-Host
 *
 *  rBLE-Host is initialized, and transmission and reception of the data are prepared.@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# The initialization of rBLE Host Task
 *    -# The initialization of RSCIP
 *  @param[in]
 *
 *  @retval
 */
/******************************************************************************************/
static RBLE_STATUS RBLE_Inititilize(void)
{
	RBLE_STATUS			Ret_Status = RBLE_OK;
	RSCIP_Callback_t	callback;
	RSCIP_RESULT_CODE	ret;

	/* Memory Clear */
	evt_buff_rd_cnt = -1;
	evt_buff_wr_cnt = -1;
	cmd_buff_rd_cnt = -1;
	cmd_buff_wr_cnt = -1;

    cmd_buff_used_count = 0;

	/* Buffer Clear */
	memset(cmd_buf,0x00,sizeof(cmd_buf));
	memset(evt_buf,0x00,sizeof(evt_buf));

	/* Event Call Back Info Clear */
	memset(&RBLE_GAP_Info,0x00,sizeof(RBLE_GAP_INFO));
	memset(&RBLE_SM_Info,0x00,sizeof(RBLE_SM_INFO));
	memset(&RBLE_GATT_Info,0x00,sizeof(RBLE_GATT_INFO));

	rBLE_Profile_Init();

	memset(&RBLE_OVER_PACKET_info,0x00,sizeof(RBLE_OVER_PACKET_INFO));

	if(rble_mode == RBLE_MODE_INITIALIZE){
		/* The parameter of RSCIP is set up. (function call) */
		callback.active_callback = &RBLE_Host_active_call_back;
		callback.read_callback   = &RBLE_Host_read_call_back;
		callback.write_callback  = &RBLE_Host_write_call_back;
		callback.reset_callback  = &RBLE_Host_reset_call_back;
		callback.error_callback  = &RBLE_Host_error_call_back;
	
		/* RSCIP Initialize */
		ret = RSCIP_Initialize(&callback);
		if ( RSCIP_ERROR == ret ) {
			Ret_Status = RBLE_ERR;
		}
	}
	return( Ret_Status );
}

/******************************************************************************************/
/**
 *  @brief the function that it is called by the link established success notice from RSCIP
 *
 *  the link established success notice management@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -#  A link established message is issued for the rBLE-Host task
 *  @param[in]
 *
 *  @retval
 */
/******************************************************************************************/
static void RBLE_Host_active_call_back(void)
{
	rble_mode = RBLE_MODE_ACTIVE;
	/* The notice of the initialization completion */
	rble_init_cb(RBLE_MODE_ACTIVE);
}

/******************************************************************************************/
/**
 *  @brief The function called by the data reception notice from RSCIP
 *
 *  the data reception management@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -#  A data reception message is issued for the rBLE-Host task
 *  @param[in]
 *
 *  @retval
 */
/******************************************************************************************/
static bool RBLE_Host_read_call_back(RSCIP_PACKET_TYPE type, uint8_t *bufptr, uint16_t size)
{
	bool		Ret = TRUE;

	/* Parameter Check */
	if(RSCIP_RBLE_EVENT != type){
	} else if(RBLE_MAX_PAYLOARD_SIZE < size ){
	} else {
		evt_buff_wr_cnt = ( evt_buff_wr_cnt + 1 ) & ( MAX_BUFF_NUM - 1 );
		/* Receive data are copied. */
		memcpy((void *)( &evt_buf[ evt_buff_wr_cnt ][0] ),(void *)bufptr,(size_t)size);
	}
	return( Ret );
}

/******************************************************************************************/
/**
 *  @brief The function called by the reset notice from RSCIP
 *
 *  Reset message receiving management@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# A sleep message is issued for the rBLE-Host task.
 *  @param[in]
 *
 *  @retval
 */
/******************************************************************************************/
static void RBLE_Host_reset_call_back(void)
{
	rble_mode = RBLE_MODE_RESET;
	rble_init_cb(RBLE_MODE_RESET);
}

/******************************************************************************************/
/**
 *  @brief The function called by the data transmit notice from RSCIP
 *
 *  the data transmit management@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -#  A data reception message is issued for the rBLE-Host task
 *  @param[in]
 *
 *  @retval
 */
/******************************************************************************************/
static void RBLE_Host_write_call_back( uint8_t *bufptr )
{
	cmd_buff_used_count--;
}

/******************************************************************************************/
/**
 *  @brief The function called by the error notice from RSCIP
 *
 *  Error message receiving management@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# A error message is issued for the rBLE-Host task.
 *  @param[in] error		error code
 *
 *  @retval
 */
/******************************************************************************************/
static void RBLE_Host_error_call_back( RSCIP_RESULT_CODE error )
{
	rble_mode = RBLE_MODE_ERROR;
	rble_init_cb(RBLE_MODE_ERROR);
}

/******************************************************************************************/
/**
 *  @brief Get Buffer Address
 *
 *  This function get buffer address@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Get semaphore
 *    -# Get Buffer Address
 *    -# Return Semaphore
 *
 *  @param[in]	*data_pnt
 *
 *  @retval		Buffer Address
 */
/******************************************************************************************/
uint8_t *RBLE_Get_CMD_Buffer(void)
{
	cmd_buff_wr_cnt = ( cmd_buff_wr_cnt + 1 ) & ( MAX_BUFF_NUM - 1 );
	cmd_buff_used_count++;
	/* Buffer Address Get */
	return( &cmd_buf[cmd_buff_wr_cnt][0] );
}



/**********************************************************************************************/
/**
 *  @brief Event Set
 *
 *  Set Event Type for rBLE sample program@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Send Message Event Type 
 *
 *  @param[in] Event	Event_Type
 *
 *  @retval		TRUE				Success
 *  @retval		FALSE				Error
 */
/**********************************************************************************************/
BOOL ke_evt_set( evt_field_t Event )
{
	BOOL	Ret = FALSE;

	if ( EVT_RSCIP_TX_DONE_BIT == Event ) {
		Tx_Event_Wr_Cnt++;
	} else if ( EVT_RSCIP_RX_DONE_BIT == Event ) {
		Rx_Event_Wr_Cnt++;
	}
	return( Ret );
}

/**********************************************************************************************/
/**
 *  @brief Event Clear
 *
 *  Clear Event Type for rBLE sample program@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *    -# Nothing is done.
 *
 *  @param[in] Event	Event_Type
 *
 *  @retval
 */
/**********************************************************************************************/
void ke_evt_clear( evt_field_t Event )
{
}

/**********************************************************************************************/
/**
 *  @brief Packet Data Set
 *
 *  Set Packet Data to RSCIP Buffer@n
 *  
 *  @attention
 *  
 *  @b Contents of management:
 *
 *  @param[in]
 *
 *  @retval
 */
/**********************************************************************************************/
void RBLE_Data_Set_Start( RBLE_HEADER_t *h_pnt )
{
	memset( &RBLE_OVER_PACKET_info, 0, sizeof( RBLE_OVER_PACKET_INFO ) );
	RBLE_OVER_PACKET_info.w_subhead_pnt = (RBLE_SUBHEADER_t *)h_pnt;
	
	if( (RBLE_MAX_PAYLOARD_SIZE - RBLE_HEADER_SIZE) < h_pnt->packet_length ){
		RBLE_OVER_PACKET_info.w_subhead_pnt->opcode				|= RBLE_API_OVER_PACKET_MASKBIT16;
		RBLE_OVER_PACKET_info.w_subhead_pnt->packet_no			= 0;
		RBLE_OVER_PACKET_info.w_subhead_pnt->data_length		= h_pnt->packet_length;
		
		RBLE_OVER_PACKET_info.w_data_pnt						= &RBLE_OVER_PACKET_info.w_subhead_pnt->data[0];
		RBLE_OVER_PACKET_info.w_subhead_pnt->packet_length		= 0 + RBLE_SUBHEADER_SIZE;
	}
	else{
		RBLE_OVER_PACKET_info.w_data_pnt						= &h_pnt->data[0];
		RBLE_OVER_PACKET_info.w_subhead_pnt->packet_length		= 0;
	}
	
}

void RBLE_Data_Set( uint8_t type_size, uint32_t dt )
{
	RBLE_SUBHEADER_t	*subh_pnt;

	if( ( sizeof( uint8_t ) != type_size ) && ( sizeof( uint16_t ) != type_size ) && ( sizeof( uint32_t ) != type_size ) ){
		return;
	}

	if( RBLE_API_OVER_PACKET_MASKBIT16 == (RBLE_OVER_PACKET_info.w_subhead_pnt->opcode & RBLE_API_OVER_PACKET_MASKBIT16) ){
		if( ( RBLE_MAX_PAYLOARD_SIZE - RBLE_HEADER_SIZE - type_size ) < RBLE_OVER_PACKET_info.w_subhead_pnt->packet_length ){
			subh_pnt = (RBLE_SUBHEADER_t *)RBLE_Get_CMD_Buffer();
			memcpy( subh_pnt, RBLE_OVER_PACKET_info.w_subhead_pnt, (RBLE_HEADER_SIZE + RBLE_SUBHEADER_SIZE) );
			
			RBLE_OVER_PACKET_info.w_subhead_pnt = (RBLE_SUBHEADER_t *)subh_pnt;
			
			RBLE_OVER_PACKET_info.w_subhead_pnt->packet_no		+= 1;
			RBLE_OVER_PACKET_info.w_subhead_pnt->packet_length	= 0 + RBLE_SUBHEADER_SIZE;
			RBLE_OVER_PACKET_info.w_data_pnt					= &RBLE_OVER_PACKET_info.w_subhead_pnt->data[0];
		}
	}
	
		*( RBLE_OVER_PACKET_info.w_data_pnt + 0 ) = ( uint8_t )( ( dt >> 0 ) & 0xff );
	if( ( sizeof( uint16_t ) == type_size ) || ( sizeof( uint32_t ) == type_size ) ){
		*( RBLE_OVER_PACKET_info.w_data_pnt + 1 ) = ( uint8_t )( ( dt >> 8 ) & 0xff );
	}
	if( sizeof( uint32_t ) == type_size ){
		*( RBLE_OVER_PACKET_info.w_data_pnt + 2 ) = ( uint8_t )( ( dt >> 16 ) & 0xff );
		*( RBLE_OVER_PACKET_info.w_data_pnt + 3 ) = ( uint8_t )( ( dt >> 24 ) & 0xff );
	}
	
	RBLE_OVER_PACKET_info.w_data_pnt += type_size;
	
	RBLE_OVER_PACKET_info.w_subhead_pnt->packet_length += type_size;
}

void RBLE_Data_Set_End( void )
{
	if( RBLE_API_OVER_PACKET_MASKBIT16 == (RBLE_OVER_PACKET_info.w_subhead_pnt->opcode & RBLE_API_OVER_PACKET_MASKBIT16) ){
			RBLE_OVER_PACKET_info.w_subhead_pnt->packet_end = 1;
	}
}

BOOL RBLE_Is_Cmd_Buff_Available(void)
{
    BOOL ret = FALSE;
    if (cmd_buff_used_count < (MAX_BUFF_NUM - 1))
    {
        ret = TRUE;
    }
    return ret;
}
