/**
 ****************************************************************************************
 *
 * @file		rble_host.h
 *
 * @brief	rBLE Host Header File
 *
 * Copyright(C) 2013-2014 Renesas Electronics Corporation
 *
 * $Rev: 2987 $
 *
 ****************************************************************************************
 */

#ifndef	__RBLE_HOST_INC__
#define	__RBLE_HOST_INC__

#include "types.h"
#include "rble_api.h"
#include "rble_trans.h"

/**************************** Define Declaration **************************************/
#define		RBLE_DATA_SET8( p, dt )			{ ( *( p + 0 ) = ( uint8_t )( ( dt >> 0 ) & 0xff ) );p += sizeof( uint8_t ); }
#define		RBLE_DATA_SET16( p, dt )		{ ( *( p + 0 ) = ( uint8_t )( ( dt >> 0 ) & 0xff ) );  ( *( p + 1 ) = ( uint8_t )( ( dt >> 8 ) & 0xff ) );p += sizeof( uint16_t ); }
#define		RBLE_DATA_SET32( p, dt )		{ ( *( p + 0 ) = ( uint8_t )( ( dt >> 0 ) & 0xff ) );  ( *( p + 1 ) = ( uint8_t )( ( dt >> 8 ) & 0xff ) ); ( *( p + 2 ) = ( uint8_t )( ( dt >> 16 ) & 0xff ) );  ( *( p + 3 ) = ( uint8_t )( ( dt >> 24 ) & 0xff ) );p += sizeof( uint32_t ); }

#define		RBLE_Data_Set8( dt )			{ RBLE_Data_Set( sizeof( uint8_t ),  dt ); }
#define		RBLE_Data_Set16( dt )			{ RBLE_Data_Set( sizeof( uint16_t ), dt ); }
#define		RBLE_Data_Set32( dt )			{ RBLE_Data_Set( sizeof( uint32_t ), dt ); }

#define		RBLE_DUMMY_GET8( p )			{ p += sizeof( uint8_t ); }
#define		RBLE_DATA_GET8( p, dt )			{ dt = ( uint8_t )( ( ( ( *( p + 0 ) ) << 0 ) & 0xff ) );p += sizeof( uint8_t ); }
#define		RBLE_DATA_GET16( p, dt )		{ dt = ( ( ( ( *( p + 0 ) ) << 0 ) & 0xff ) + ( ( ( *( p + 1 ) ) << 8 ) & 0xff00 ) );p += sizeof( uint16_t ); }
#define		RBLE_DATA_GET32( p, dt )		{ dt = ( ( ( ( uint32_t )( *( p + 0 ) ) << 0 ) & 0xff ) + ( ( ( uint32_t )( *( p + 1 ) ) << 8 ) & 0xff00 ) + ( ( ( uint32_t )( *( p + 2 ) ) << 16 ) & 0xff0000 ) + ( ( ( uint32_t )( *( p + 3 ) ) << 24 ) & 0xff000000 ) );p += sizeof( uint32_t ); }

/**************************** Structure Declaration ***********************************/
typedef struct RBLE_GAP_INFO_t {
	uint16_t				conhdl;			/* Connection handle */
	RBLE_GAP_EVENT_HANDLER	Handler;		/* Event Handler */
	void					*Temp_Malloc_p;	/* Temporary Memory(ke_malloc) */
} RBLE_GAP_INFO;

typedef struct RBLE_SM_INFO_t {
	uint16_t				conhdl;		/* Connection Handle */
	RBLE_SM_EVENT_HANDLER	Handler;	/* Event Handler */
} RBLE_SM_INFO;

typedef struct RBLE_GATT_INFO_t {
	RBLE_GATT_EVENT_HANDLER	Handler;			/**< GATT event handler */
} RBLE_GATT_INFO;

typedef struct RBLE_FMPT_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_FMPT_EVENT_HANDLER		Fmt_Handler;	/* Find Me Target Event Handler */
} RBLE_FMPT_INFO;

typedef struct RBLE_FMPL_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_FMPL_EVENT_HANDLER		Fml_Handler;	/* Find Me Locator Event Handler */
} RBLE_FMPL_INFO;

typedef struct RBLE_PXPR_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_PXPR_EVENT_HANDLER		Pmr_Handler;	/* Proximity Reporter Event Handler */
} RBLE_PXPR_INFO;

typedef struct RBLE_PXPM_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_PXPM_EVENT_HANDLER		Pmm_Handler;	/* Proximity Monitor Event Handler */
} RBLE_PXPM_INFO;

typedef struct RBLE_HTPT_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_HTPT_EVENT_HANDLER		Htt_Handler;	/* Health Thermometer Thermometer Event Handler */
} RBLE_HTPT_INFO;

typedef struct RBLE_HTPC_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_HTPC_EVENT_HANDLER		Htc_Handler;	/* Health Thermometer Collector Event Handler */
} RBLE_HTPC_INFO;

typedef struct RBLE_BLPS_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_BLPS_EVENT_HANDLER		Bls_Handler;	/* Blood Pressure Sensor Event Handler */
} RBLE_BLPS_INFO;

typedef struct RBLE_BLPC_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_BLPC_EVENT_HANDLER		Blc_Handler;	/* Blood Pressure Collector Event Handler */
} RBLE_BLPC_INFO;

typedef struct RBLE_HGHD_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_HGHD_EVENT_HANDLER		Hghd_Handler;	/* HID over GATT HID Device Event Handler */
} RBLE_HGHD_INFO;

typedef struct RBLE_HGBH_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_HGBH_EVENT_HANDLER		Hgbh_Handler;	/* HID over GATT Boot Host Event Handler */
    
	RBLE_HIDS_CONTENT			*Hids_Temp_Memory_Top;		/* Handle attribute information of HID Service */
    RBLE_BAS_CONTENT			*Bas_Temp_Memory_Top;		/* Handle attribute information of Battery Service */
} RBLE_HGBH_INFO;

typedef struct RBLE_HGRH_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_HGRH_EVENT_HANDLER		Hgrh_Handler;	/* HID over GATT Report Host Event Handler */
    
	RBLE_HIDS_CONTENT			*Hids_Temp_Memory_Top;		/* Handle attribute information of HID Service */
    RBLE_BAS_CONTENT			*Bas_Temp_Memory_Top;		/* Handle attribute information of Battery Service */
} RBLE_HGRH_INFO;

typedef struct RBLE_SPPS_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_SPPS_EVENT_HANDLER		Scans_Handler;	/* Scan Parameters Server Event Handler */
} RBLE_SPPS_INFO;

typedef struct RBLE_SPPC_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_SPPC_EVENT_HANDLER		Scanc_Handler;	/* Scan Parameters Client Event Handler */
} RBLE_SPPC_INFO;

typedef struct RBLE_HRPC_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_HRPC_EVENT_HANDLER		Hrc_Handler;	/* Heart Rate Collector Event Handler */
} RBLE_HRPC_INFO;

typedef struct RBLE_HRPS_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_HRPS_EVENT_HANDLER		Hrs_Handler;	/* Heart Rate Sensor Event Handler */
} RBLE_HRPS_INFO;

typedef struct RBLE_CSCPC_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_CSCPC_EVENT_HANDLER	Cscc_Handler;	/* Cycling Speed and Cadence Collector Event Handler */
} RBLE_CSCPC_INFO;

typedef struct RBLE_CSCPS_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_CSCPS_EVENT_HANDLER	Cscs_Handler;	/* Cycling Speed and Cadence Sensor Event Handler */
} RBLE_CSCPS_INFO;

typedef struct RBLE_CPPC_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_CPPC_EVENT_HANDLER		Cpc_Handler;	/* Cycling Power Collector Event Handler */
} RBLE_CPPC_INFO;

typedef struct RBLE_CPPS_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_CPPS_EVENT_HANDLER		Cps_Handler;	/* Cycling Power Sensor Event Handler */
} RBLE_CPPS_INFO;

typedef struct RBLE_GLPS_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_GLPS_EVENT_HANDLER		Gls_Handler;	/* Glucose Sensor Event Handler */
} RBLE_GLPS_INFO;

typedef struct RBLE_GLPC_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_GLPC_EVENT_HANDLER		Glc_Handler;	/* Glucose Collector Event Handler */
} RBLE_GLPC_INFO;

typedef struct RBLE_TIPC_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_TIPC_EVENT_HANDLER		Tic_Handler;	/* Time Client Event Handler */
} RBLE_TIPC_INFO;

typedef struct RBLE_TIPS_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_TIPS_EVENT_HANDLER		Tis_Handler;	/* Time Server Event Handler */
} RBLE_TIPS_INFO;

typedef struct RBLE_ANPC_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_ANPC_EVENT_HANDLER		Anc_Handler;	/* Alert Notification Client Event Handler */
} RBLE_ANPC_INFO;

typedef struct RBLE_ANPS_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_ANPS_EVENT_HANDLER		Ans_Handler;	/* Alert Notification Server Event Handler */
} RBLE_ANPS_INFO;

typedef struct RBLE_LNPC_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_LNPC_EVENT_HANDLER		Lnc_Handler;	/* Location and Navigation Collector Event Handler */
} RBLE_LNPC_INFO;

typedef struct RBLE_LNPS_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_LNPS_EVENT_HANDLER		Lns_Handler;	/* Location and Navigation Sensor Event Handler */
} RBLE_LNPS_INFO;

typedef struct RBLE_PASPC_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_PASPC_EVENT_HANDLER	Pasc_Handler;	/* Phone Alert Status Client Event Handler */
} RBLE_PASPC_INFO;

typedef struct RBLE_PASPS_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_PASPS_EVENT_HANDLER	Pass_Handler;	/* Phone Alert Status Server Event Handler */
} RBLE_PASPS_INFO;

typedef struct RBLE_RSCPC_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_RSCPC_EVENT_HANDLER	Rscc_Handler;	/* Running Speed and Cadence Collector Event Handler */
} RBLE_RSCPC_INFO;

typedef struct RBLE_RSCPS_INFO_t {
	uint16_t					conhdl;			/* Connection Handle */
	RBLE_RSCPS_EVENT_HANDLER	Rscs_Handler;	/* Running Speed and Cadence Sensor Event Handler */
} RBLE_RSCPS_INFO;

typedef struct RBLE_VS_INFO_t {
	uint16_t				conhdl;		/* Connection Handle */
	RBLE_VS_EVENT_HANDLER	Handler;	/* Event Handler */
} RBLE_VS_INFO;


typedef struct RBLE_OVER_PACKET_INFO_t {
	RBLE_SUBHEADER_t		*w_subhead_pnt;
	uint8_t					*w_data_pnt;			/* Payload data pinter */
	RBLE_HEADER_t			*r_head_pnt;
	uint16_t				r_data_cnt;
} RBLE_OVER_PACKET_INFO;

/**************************** Extern Function *****************************************/
extern uint8_t *RBLE_Get_CMD_Buffer(void);

extern RBLE_STATUS RBLE_Event_Data_Analisys(uint8_t *data_pnt);
#ifdef RBLE_DEBUG
extern void rBLE_Data_Disp(RBLE_HEADER_t *pnt);
#endif

extern void RBLE_Data_Set_Start( RBLE_HEADER_t *h_pnt );
extern void RBLE_Data_Set( uint8_t type_size, uint32_t dt );
extern void RBLE_Data_Set_End( void );

/**************************** Extern Global Memory *****************************************/
extern RBLE_GAP_INFO	RBLE_GAP_Info;
extern RBLE_SM_INFO		RBLE_SM_Info;
extern RBLE_GATT_INFO	RBLE_GATT_Info;
extern RBLE_FMPL_INFO	RBLE_FMPL_Info;
extern RBLE_FMPT_INFO	RBLE_FMPT_Info;
extern RBLE_HTPC_INFO	RBLE_HTPC_Info;
extern RBLE_HTPT_INFO	RBLE_HTPT_Info;
extern RBLE_PXPM_INFO	RBLE_PXPM_Info;
extern RBLE_PXPR_INFO	RBLE_PXPR_Info;
extern RBLE_BLPC_INFO	RBLE_BLPC_Info;
extern RBLE_BLPS_INFO	RBLE_BLPS_Info;
extern RBLE_HGHD_INFO	RBLE_HGHD_Info;
extern RBLE_HGBH_INFO	RBLE_HGBH_Info;
extern RBLE_HGRH_INFO	RBLE_HGRH_Info;
extern RBLE_SPPC_INFO	RBLE_SPPC_Info;
extern RBLE_SPPS_INFO	RBLE_SPPS_Info;
extern RBLE_HRPC_INFO	RBLE_HRPC_Info;
extern RBLE_HRPS_INFO	RBLE_HRPS_Info;
extern RBLE_CSCPC_INFO	RBLE_CSCPC_Info;
extern RBLE_CSCPS_INFO	RBLE_CSCPS_Info;
extern RBLE_CPPC_INFO	RBLE_CPPC_Info;
extern RBLE_CPPS_INFO	RBLE_CPPS_Info;
extern RBLE_GLPC_INFO	RBLE_GLPC_Info;
extern RBLE_GLPS_INFO	RBLE_GLPS_Info;
extern RBLE_TIPC_INFO	RBLE_TIPC_Info;
extern RBLE_TIPS_INFO	RBLE_TIPS_Info;
extern RBLE_ANPC_INFO	RBLE_ANPC_Info;
extern RBLE_ANPS_INFO	RBLE_ANPS_Info;
extern RBLE_LNPC_INFO	RBLE_LNPC_Info;
extern RBLE_LNPS_INFO	RBLE_LNPS_Info;
extern RBLE_PASPC_INFO	RBLE_PASPC_Info;
extern RBLE_PASPS_INFO	RBLE_PASPS_Info;
extern RBLE_RSCPC_INFO	RBLE_RSCPC_Info;
extern RBLE_RSCPS_INFO	RBLE_RSCPS_Info;
extern RBLE_VS_INFO		RBLE_VS_Info;
extern RBLE_MODE		rble_mode;										/* rBLE Internal Mode */

extern RBLE_OVER_PACKET_INFO	RBLE_OVER_PACKET_info;
/**************************** PROTOTYPE Declaration ***********************************/
#ifdef __cplusplus
extern "C" {
#endif
void rBLE_Exit( void );
void rBLE_Run(void);
void rBLE_Profile_Init( void );
bool rBLE_Is_Idle( void );
#ifdef __cplusplus
}
#endif

#endif // __RBLE_HOST_INC__
