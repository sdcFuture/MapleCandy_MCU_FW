/**
 ****************************************************************************************
 *
 * @file		prf_sel.h
 *
 * @brief	Setting of Profile Selection
 *
 * Copyright(C) 2013-2014 Renesas Electronics Corporation
 *
 * $Rev: 9242 $
 *
 ****************************************************************************************
 */

#ifndef _PRF_SEL_H_
#define _PRF_SEL_H_

/**
 * INCLUDE FILES
 ****************************************************************************************
 */

/**
 * TYPE DEFINES
 ****************************************************************************************
 */

/**
 * DEFINES
 ****************************************************************************************
 */
#ifdef USE_SAMPLE_PROFILE
//#define	USE_CUSTOM_DEMO
#endif

/* for Profile Selection (Set to 0 if the profile is not selected.) */
#ifndef USE_SAMPLE_PROFILE
#ifdef USE_MODULE_RY7011
//for module
#define PRF_SEL_PXPM    1   /* Proximity Profile Monitor role */
#define PRF_SEL_PXPR    1   /* Proximity Profile Reporter role */
#define PRF_SEL_FMPL    1   /* Find Me Profile Locator role */
#define PRF_SEL_FMPT    1   /* Find Me Profile Target role */
#define PRF_SEL_HTPC    1   /* Health Thermometer Profile Collector role */
#define PRF_SEL_HTPT    1   /* Health Thermometer Profile Thermometer role */
#define PRF_SEL_BLPC    1   /* Blood Pressure Profile Collector role */
#define PRF_SEL_BLPS    1   /* Blood Pressure Profile Sensor role */
#define PRF_SEL_HGHD    0   /* HID over GATT Profile HID Device role */
#define PRF_SEL_HGBH    0   /* HID over GATT Profile Boot Host role */
#define PRF_SEL_HGRH    0   /* HID over GATT Profile Report Host role */
#define PRF_SEL_SPPC    0   /* Scan Parameters Profile Scan Client role */
#define PRF_SEL_SPPS    0   /* Scan Parameters Profile Scan Server role */
#define PRF_SEL_HRPC    1   /* Heart Rate Profile Collector role */
#define PRF_SEL_HRPS    1   /* Heart Rate Profile Sensor role */
#define PRF_SEL_CSCC    0   /* Cycling Speed and Cadence Profile Collector role */
#define PRF_SEL_CSCS    0   /* Cycling Speed and Cadence Profile Sensor role */
#define PRF_SEL_GLPC    1   /* Glucose Profile Collector role */
#define PRF_SEL_GLPS    1   /* Glucose Profile Sensor role */
#define PRF_SEL_CPPC    0   /* Cycling Power Profile Collector role */
#define PRF_SEL_CPPS    0   /* Cycling Power Profile Sensor role */
#define PRF_SEL_TIPC    1   /* Time Profile Client role */
#define PRF_SEL_TIPS    1   /* Time Profile Server role */
#define PRF_SEL_ANPC    1   /* Alert Notification Profile Client role */
#define PRF_SEL_ANPS    1   /* Alert Notification Profile Server role */
#define PRF_SEL_LNPS    0   /* Location and Navigation Profile Sensor role */
#define PRF_SEL_LNPC    0   /* Location and Navigation Profile Collector role */
#define PRF_SEL_PASC    1   /* Phone Alert Status Profile Client role */
#define PRF_SEL_PASS    1   /* Phone Alert Status Profile Server role */
#define PRF_SEL_RSCC    1   /* Running Speed and Cadence Profile Collector role */
#define PRF_SEL_RSCS    1   /* Running Speed and Cadence Profile Sensor role */
#else /* USE_MODULE_RY7011 */
//for ble soft for default with vuart
#define PRF_SEL_PXPM    1   /* Proximity Profile Monitor role */
#define PRF_SEL_PXPR    1   /* Proximity Profile Reporter role */
#define PRF_SEL_FMPL    1   /* Find Me Profile Locator role */
#define PRF_SEL_FMPT    1   /* Find Me Profile Target role */
#define PRF_SEL_HTPC    0   /* Health Thermometer Profile Collector role */
#define PRF_SEL_HTPT    0   /* Health Thermometer Profile Thermometer role */
#define PRF_SEL_BLPC    0   /* Blood Pressure Profile Collector role */
#define PRF_SEL_BLPS    0   /* Blood Pressure Profile Sensor role */
#define PRF_SEL_HGHD    0   /* HID over GATT Profile HID Device role */
#define PRF_SEL_HGBH    0   /* HID over GATT Profile Boot Host role */
#define PRF_SEL_HGRH    0   /* HID over GATT Profile Report Host role */
#define PRF_SEL_SPPC    0   /* Scan Parameters Profile Scan Client role */
#define PRF_SEL_SPPS    0   /* Scan Parameters Profile Scan Server role */
#define PRF_SEL_HRPC    0   /* Heart Rate Profile Collector role */
#define PRF_SEL_HRPS    0   /* Heart Rate Profile Sensor role */
#define PRF_SEL_CSCC    0   /* Cycling Speed and Cadence Profile Collector role */
#define PRF_SEL_CSCS    0   /* Cycling Speed and Cadence Profile Sensor role */
#define PRF_SEL_GLPC    0   /* Glucose Profile Collector role */
#define PRF_SEL_GLPS    0   /* Glucose Profile Sensor role */
#define PRF_SEL_CPPC    0   /* Cycling Power Profile Collector role */
#define PRF_SEL_CPPS    0   /* Cycling Power Profile Sensor role */
#define PRF_SEL_TIPC    0   /* Time Profile Client role */
#define PRF_SEL_TIPS    0   /* Time Profile Server role */
#define PRF_SEL_ANPC    1   /* Alert Notification Profile Client role */
#define PRF_SEL_ANPS    1   /* Alert Notification Profile Server role */
#define PRF_SEL_LNPS    0   /* Location and Navigation Profile Sensor role */
#define PRF_SEL_LNPC    0   /* Location and Navigation Profile Collector role */
#define PRF_SEL_PASC    0   /* Phone Alert Status Profile Client role */
#define PRF_SEL_PASS    0   /* Phone Alert Status Profile Server role */
#define PRF_SEL_RSCC    0   /* Running Speed and Cadence Profile Collector role */
#define PRF_SEL_RSCS    0   /* Running Speed and Cadence Profile Sensor role */
#endif /* USE_MODULE_RY7011 */
#else /* USE_SAMPLE_PROFILE */
#define PRF_SEL_PXPM    0   /* Proximity Profile Monitor role */
#define PRF_SEL_PXPR    0   /* Proximity Profile Reporter role */
#define PRF_SEL_FMPL    0   /* Find Me Profile Locator role */
#define PRF_SEL_FMPT    0   /* Find Me Profile Target role */
#define PRF_SEL_HTPC    0   /* Health Thermometer Profile Collector role */
#define PRF_SEL_HTPT    0   /* Health Thermometer Profile Thermometer role */
#define PRF_SEL_BLPC    0   /* Blood Pressure Profile Collector role */
#define PRF_SEL_BLPS    0   /* Blood Pressure Profile Sensor role */
#define PRF_SEL_HGHD    0   /* HID over GATT Profile HID Device role */
#define PRF_SEL_HGBH    0   /* HID over GATT Profile Boot Host role */
#define PRF_SEL_HGRH    0   /* HID over GATT Profile Report Host role */
#define PRF_SEL_SPPS    0   /* Scan Parameters Profile Scan Server role */
#define PRF_SEL_SPPC    0   /* Scan Parameters Profile Scan Client role */
#define PRF_SEL_HRPC    0   /* Heart Rate Profile Collector role */
#define PRF_SEL_HRPS    0   /* Heart Rate Profile Sensor role */
#define PRF_SEL_CSCC    0   /* Cycling Speed and Cadence Profile Collector role */
#define PRF_SEL_CSCS    0   /* Cycling Speed and Cadence Profile Sensor role */
#define PRF_SEL_GLPC    0   /* Glucose Profile Collector role */
#define PRF_SEL_GLPS    0   /* Glucose Profile Sensor role */
#define PRF_SEL_CPPS    0   /* Cycling Power Profile Sensor role */
#define PRF_SEL_CPPC    0   /* Cycling Power Profile Collector role */
#define PRF_SEL_TIPS    0   /* Time Profile Server role */
#define PRF_SEL_TIPC    0   /* Time Profile Client role */
#define PRF_SEL_ANPS    0   /* Alert Notification Profile Server role */
#define PRF_SEL_ANPC    0   /* Alert Notification Profile Client role */
#define PRF_SEL_LNPS    0   /* Location and Navigation Profile Sensor role */
#define PRF_SEL_LNPC    0   /* Location and Navigation Profile Collector role */
#define PRF_SEL_PASS    0   /* Phone Alert Status Profile Client role */
#define PRF_SEL_PASC    0   /* Phone Alert Status Profile Server role */
#define PRF_SEL_RSCS    0   /* Running Speed and Cadence Profile Collector role */
#define PRF_SEL_RSCC    0   /* Running Speed and Cadence Profile Sensor role */
#endif

#if PRF_SEL_HGHD
#define HID_MULTI_INSTANCE
#endif

/* Flag indicating if use Link Loss Service */
#if (PRF_SEL_PXPR)
    #define USE_LINK_LOSS_SERVICE           1
#else
    /* If use LLS without PXP, set to 1. */
    #define USE_LINK_LOSS_SERVICE           0
#endif

/* Flag indicating if use Tx Power Service */
#if (PRF_SEL_PXPR)
    #define USE_TX_POWER_SERVICE            1
#else
    /* If use TPS without PXP, set to 1. */
    #define USE_TX_POWER_SERVICE            0
#endif

/* Flag indicating if use Immediate Alert Service */
#if (PRF_SEL_PXPR || PRF_SEL_FMPT)
    #define USE_IMMEDIATE_ALERT_SERVICE     1
#else
    /* If use IAS without PXP and FMP, set to 1. */
    #define USE_IMMEDIATE_ALERT_SERVICE     0
#endif

/* Flag indicating if use Device Information Service */
#if (PRF_SEL_HTPT || PRF_SEL_BLPS || PRF_SEL_HRPS || PRF_SEL_CSCS || PRF_SEL_GLPS || PRF_SEL_CPPS || PRF_SEL_LNPS || PRF_SEL_RSCS)
    #define USE_DEV_INFO_SERVICE            1
#else
    /* If use DIS without HTP, BLP and so on, set to 1. */
    #define USE_DEV_INFO_SERVICE            0
#endif

/* Flag indicating if use Device Information Service (PnP characteristic) */
#if (PRF_SEL_HGHD)
    #define USE_DEV_INFO_SERVICE_PNP        1
#else
    /* If use DIS(PnP) without HOGP, set to 1. */
    #define USE_DEV_INFO_SERVICE_PNP        0
#endif

/* Flag indicating if use Battery Service */
#if (PRF_SEL_HGHD || PRF_SEL_CPPS || PRF_SEL_LNPS)
    #define USE_BATTERY_SERVICE             1
#else
    /* If use BAS without HOGP, CPP, LNP, set to 1. */
    #define USE_BATTERY_SERVICE             0
#endif

/* Flag indicating if use Scan Parameters Service */
#if (PRF_SEL_SPPS)
    #define USE_SCAN_PARAMETER_SERVICE      1
#else
    /* If use ScPS without ScPP, set to 1. */
    #define USE_SCAN_PARAMETER_SERVICE      0
#endif


/**
 * STRUCTURE DECLARATIONS
 ****************************************************************************************
 */

/**
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

#endif /* _PRF_SEL_H_ */
