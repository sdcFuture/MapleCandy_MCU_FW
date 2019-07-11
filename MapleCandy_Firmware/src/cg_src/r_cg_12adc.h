/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2016, 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_12adc.h
* Version      : Code Generator for RL78/L1A V1.01.02.03 [24 May 2018]
* Device(s)    : R5F11MMF
* Tool-Chain   : CCRL
* Description  : This file implements device driver for ADC module.
* Creation Date: 4/1/2019
***********************************************************************************************************************/
#ifndef ADC_H
#define ADC_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    A/D Control Register (ADCSR) 
*/
/* A/D conversion operation control (ADST) */
#define _0000_AD_CONVERSION_DISABLE                     (0x0000U) /* stops A/D conversion process */
#define _8000_AD_CONVERSION_ENABLE                      (0x8000U) /* starts A/D conversion process */
/* Scan mode select bit (ADCS) */
#define _0000_AD_SCAN_MODE_SINGLE                       (0x0000U) /* single scan mode */
#define _4000_AD_SCAN_MODE_CONTINUOUS                   (0x4000U) /* continuous scan mode */
/* Scan end interrupt enable bit (ADIE) */
#define _0000_AD_INTAD_DISABLE                          (0x0000U) /* disables INTAD generation upon scan completion */
#define _1000_AD_INTAD_ENABLE                           (0x1000U) /* enables INTAD  generation upon scan completion */
/* A/D conversion select bit (ADHSC) */
#define _0000_AD_CONVERSION_HIGH_SPEED                  (0x0000U) /* high-speed conversion */
#define _0400_AD_CONVERSION_NORMAL                      (0x0400U) /* normal conversion */
/* Trigger start enable bit (TRGE) */
#define _0000_AD_TRIGGER_CONVERSION_DISABLE             (0x0000U) /* disables A/D conversion to be started by trigger */
#define _0200_AD_TRIGGER_CONVERSION_ENABLE              (0x0200U) /* enables A/D conversion to be started by trigger */
/* Trigger select bit (EXTRG) */
#define _0000_AD_SYNCHRONOUS_TRIGGER                    (0x0000U) /* A/D conversion is started by synchronous trigger */
#define _0100_AD_ASYNCHRONOUS_TRIGGER                   (0x0100U) /* A/D conversion is started by asynchronous trigger */

/*
    A/D conversion channel select bit (ADANSA0) 
*/
/* A/D conversion channel select bit 00 (ANSA00) */
#define _0000_AD_ANI00_NOT_SUBJECT                      (0x0000U) /* ANI00 is not subjected to conversion */
#define _0001_AD_ANI00_SUBJECT                          (0x0001U) /* ANI00 is subjected to conversion */
/* A/D conversion channel select bit 01 (ANSA01) */
#define _0000_AD_ANI01_NOT_SUBJECT                      (0x0000U) /* ANI01 is not subjected to conversion */
#define _0002_AD_ANI01_SUBJECT                          (0x0002U) /* ANI01 is subjected to conversion */
/* A/D conversion channel select bit 02 (ANSA02) */
#define _0000_AD_ANI02_NOT_SUBJECT                      (0x0000U) /* ANI02 is not subjected to conversion */
#define _0004_AD_ANI02_SUBJECT                          (0x0004U) /* ANI02 is subjected to conversion */
/* A/D conversion channel select bit 03 (ANSA03) */
#define _0000_AD_ANI03_NOT_SUBJECT                      (0x0000U) /* ANI03 is not subjected to conversion */
#define _0008_AD_ANI03_SUBJECT                          (0x0008U) /* ANI03 is subjected to conversion */
/* A/D conversion channel select bit 04 (ANSA04) */
#define _0000_AD_ANI04_NOT_SUBJECT                      (0x0000U) /* ANI04 is not subjected to conversion */
#define _0010_AD_ANI04_SUBJECT                          (0x0010U) /* ANI04 is subjected to conversion */
/* A/D conversion channel select bit 05 (ANSA05) */
#define _0000_AD_ANI05_NOT_SUBJECT                      (0x0000U) /* ANI05 is not subjected to conversion */
#define _0020_AD_ANI05_SUBJECT                          (0x0020U) /* ANI05 is subjected to conversion */
/* A/D conversion channel select bit 06 (ANSA06) */
#define _0000_AD_ANI06_NOT_SUBJECT                      (0x0000U) /* ANI06 is not subjected to conversion */
#define _0040_AD_ANI06_SUBJECT                          (0x0040U) /* ANI06 is subjected to conversion */
/* A/D conversion channel select bit 07 (ANSA07) */
#define _0000_AD_ANI07_NOT_SUBJECT                      (0x0000U) /* ANI07 is not subjected to conversion */
#define _0080_AD_ANI07_SUBJECT                          (0x0080U) /* ANI07 is subjected to conversion */
/* A/D conversion channel select bit 08 (ANSA08) */
#define _0000_AD_ANI08_NOT_SUBJECT                      (0x0000U) /* ANI08 is not subjected to conversion */
#define _0100_AD_ANI08_SUBJECT                          (0x0100U) /* ANI08 is subjected to conversion */
/* A/D conversion channel select bit 09 (ANSA09) */
#define _0000_AD_ANI09_NOT_SUBJECT                      (0x0000U) /* ANI09 is not subjected to conversion */
#define _0200_AD_ANI09_SUBJECT                          (0x0200U) /* ANI09 is subjected to conversion */
/* A/D conversion channel select bit 10 (ANSA010) */
#define _0000_AD_ANI10_NOT_SUBJECT                      (0x0000U) /* ANI10 is not subjected to conversion */
#define _0400_AD_ANI10_SUBJECT                          (0x0400U) /* ANI10 is subjected to conversion */
/* A/D conversion channel select bit 11 (ANSA011) */
#define _0000_AD_ANI11_NOT_SUBJECT                      (0x0000U) /* ANI11 is not subjected to conversion */
#define _0800_AD_ANI11_SUBJECT                          (0x0800U) /* ANI11 is subjected to conversion */
/* A/D conversion channel select bit 12 (ANSA012) */
#define _0000_AD_ANI12_NOT_SUBJECT                      (0x0000U) /* ANI12 is not subjected to conversion */
#define _1000_AD_ANI12_SUBJECT                          (0x1000U) /* ANI12 is subjected to conversion */
/* A/D conversion channel select bit 13 (ANSA013) */
#define _0000_AD_ANI13_NOT_SUBJECT                      (0x0000U) /* ANI13 is not subjected to conversion */
#define _2000_AD_ANI13_SUBJECT                          (0x2000U) /* ANI13 is subjected to conversion */
/* A/D conversion channel select bit 14 (ANSA014) */
#define _0000_AD_ANI14_NOT_SUBJECT                      (0x0000U) /* ANI14 is not subjected to conversion */
#define _4000_AD_ANI14_SUBJECT                          (0x4000U) /* ANI14 is subjected to conversion */

/*
    A/D-converted value addition/average channel select (ADADS0) 
*/
/* A/D-converted value addition/average channel 00 select (ADS00) */
#define _0000_AD_ANI00_NOT_ADD_AVERAGE                  (0x0000U) /* A/D-converted value addition/average mode for ANI00 is not selected */
#define _0001_AD_ANI00_ADD_AVERAGE                      (0x0001U) /* A/D-converted value addition/average mode for ANI00 is selected */
/* A/D-converted value addition/average channel 01 select (ADS01) */
#define _0000_AD_ANI01_NOT_ADD_AVERAGE                  (0x0000U) /* A/D-converted value addition/average mode for ANI01 is not selected */
#define _0002_AD_ANI01_ADD_AVERAGE                      (0x0002U) /* A/D-converted value addition/average mode for ANI01 is selected */
/* A/D-converted value addition/average channel 02 select (ADS02) */
#define _0000_AD_ANI02_NOT_ADD_AVERAGE                  (0x0000U) /* A/D-converted value addition/average mode for ANI02 is not selected */
#define _0004_AD_ANI02_ADD_AVERAGE                      (0x0004U) /* A/D-converted value addition/average mode for ANI02 is selected */
/* A/D-converted value addition/average channel 03 select (ADS03) */
#define _0000_AD_ANI03_NOT_ADD_AVERAGE                  (0x0000U) /* A/D-converted value addition/average mode for ANI03 is not selected */
#define _0008_AD_ANI03_ADD_AVERAGE                      (0x0008U) /* A/D-converted value addition/average mode for ANI03 is selected */
/* A/D-converted value addition/average channel 04 select (ADS04) */
#define _0000_AD_ANI04_NOT_ADD_AVERAGE                  (0x0000U) /* A/D-converted value addition/average mode for ANI04 is not selected */
#define _0010_AD_ANI04_ADD_AVERAGE                      (0x0010U) /* A/D-converted value addition/average mode for ANI04 is selected */
/* A/D-converted value addition/average channel 05 select (ADS05) */
#define _0000_AD_ANI05_NOT_ADD_AVERAGE                  (0x0000U) /* A/D-converted value addition/average mode for ANI05 is not selected */
#define _0020_AD_ANI05_ADD_AVERAGE                      (0x0020U) /* A/D-converted value addition/average mode for ANI05 is selected */
/* A/D-converted value addition/average channel 06 select (ADS06) */
#define _0000_AD_ANI06_NOT_ADD_AVERAGE                  (0x0000U) /* A/D-converted value addition/average mode for ANI06 is not selected */
#define _0040_AD_ANI06_ADD_AVERAGE                      (0x0040U) /* A/D-converted value addition/average mode for ANI06 is selected */
/* A/D-converted value addition/average channel 07 select (ADS07) */
#define _0000_AD_ANI07_NOT_ADD_AVERAGE                  (0x0000U) /* A/D-converted value addition/average mode for ANI07 is not selected */
#define _0080_AD_ANI07_ADD_AVERAGE                      (0x0080U) /* A/D-converted value addition/average mode for ANI07 is selected */
/* A/D-converted value addition/average channel 08 select (ADS08) */
#define _0000_AD_ANI08_NOT_ADD_AVERAGE                  (0x0000U) /* A/D-converted value addition/average mode for ANI08 is not selected */
#define _0100_AD_ANI08_ADD_AVERAGE                      (0x0100U) /* A/D-converted value addition/average mode for ANI08 is selected */
/* A/D-converted value addition/average channel 09 select (ADS09) */
#define _0000_AD_ANI09_NOT_ADD_AVERAGE                  (0x0000U) /* A/D-converted value addition/average mode for ANI09 is not selected */
#define _0200_AD_ANI09_ADD_AVERAGE                      (0x0200U) /* A/D-converted value addition/average mode for ANI09 is selected */
/* A/D-converted value addition/average channel 10 select (ADS010) */
#define _0000_AD_ANI10_NOT_ADD_AVERAGE                  (0x0000U) /* A/D-converted value addition/average mode for ANI10 is not selected */
#define _0400_AD_ANI10_ADD_AVERAGE                      (0x0400U) /* A/D-converted value addition/average mode for ANI10 is selected */
/* A/D-converted value addition/average channel 11 select (ADS011) */
#define _0000_AD_ANI11_NOT_ADD_AVERAGE                  (0x0000U) /* A/D-converted value addition/average mode for ANI11 is not selected */
#define _0800_AD_ANI11_ADD_AVERAGE                      (0x0800U) /* A/D-converted value addition/average mode for ANI11 is selected */
/* A/D-converted value addition/average channel 12 select (ADS012) */
#define _0000_AD_ANI12_NOT_ADD_AVERAGE                  (0x0000U) /* A/D-converted value addition/average mode for ANI12 is not selected */
#define _1000_AD_ANI12_ADD_AVERAGE                      (0x1000U) /* A/D-converted value addition/average mode for ANI12 is selected */
/* A/D-converted value addition/average channel 13 select (ADS013) */
#define _0000_AD_ANI13_NOT_ADD_AVERAGE                  (0x0000U) /* A/D-converted value addition/average mode for ANI13 is not selected */
#define _2000_AD_ANI13_ADD_AVERAGE                      (0x2000U) /* A/D-converted value addition/average mode for ANI13 is selected */
/* A/D-converted value addition/average channel 14 select (ADS014) */
#define _0000_AD_ANI14_NOT_ADD_AVERAGE                  (0x0000U) /* A/D-converted value addition/average mode for ANI14 is not selected */
#define _4000_AD_ANI14_ADD_AVERAGE                      (0x4000U) /* A/D-converted value addition/average mode for ANI14 is selected */

/*
    A/D-converted Value Addion/Average Count Select Register (ADADC) 
*/
/* Average mode enable bit (AVEE) */
#define _00_AD_MODE_ADDITION                            (0x00U) /* addition mode is selected */
#define _80_AD_MODE_AVERAGE                             (0x80U) /* average mode is selected */
/* Addition count select bit (ADC) */
#define _00_AD_CONVERSION_1TIME                         (0x00U) /* 1-time conversion (no addition; same as normal conversion) */
#define _01_AD_CONVERSION_2TIME                         (0x01U) /* 2-time conversion (addition once) */
#define _02_AD_CONVERSION_3TIME                         (0x02U) /* 3-time conversion (addition twice) */
#define _03_AD_CONVERSION_4TIME                         (0x03U) /* 4-time conversion (addition three times) */
#define _05_AD_CONVERSION_16TIME                        (0x05U) /* 16-time conversion (addition 15 times) */

/*
    A/D Control Extended Register (ADCER) 
*/
/* A/D data register format select bit (ADRFMT) */
#define _0000_AD_DATA_FLUSH_RIGHT                       (0x0000U) /* flush-right is selected for the A/D data register format */
#define _8000_AD_DATA_FLUSH_LEFT                        (0x8000U) /* flush-left is selected for the A/D data register format */
/* Self-diagnosis enable bit (DIAGM) */
#define _0000_AD_SELF_DIAGNOSIS_DISABLE                 (0x0000U) /* disables self-diagnosis of 12-bit A/D converter. */
#define _0800_AD_SELF_DIAGNOSIS_ENABLE                  (0x0800U) /* enables self-diagnosis of 12-bit A/D converter. */
/* Self-diagnosis mode select bit (DIAGLD) */
#define _0000_AD_SELF_DIAGNOSIS_ROTATION                (0x0000U) /* rotation mode for self-diagnosis voltage */
#define _0400_AD_SELF_DIAGNOSIS_FIXED                   (0x0400U) /* fixed mode for self-diagnosis voltage */
/* Self-diagnosis conversion voltage select bit (DIAGVAL) */
#define _0100_AD_SELF_DIAGNOSIS_0_V                     (0x0100U) /* uses the voltage of 0 V for self-diagnosis */
#define _0200_AD_SELF_DIAGNOSIS_1_2_V                   (0x0200U) /* uses the voltage of reference power supply × 1/2 for self-diagnosis */
#define _0300_AD_SELF_DIAGNOSIS_REF_V                   (0x0300U) /* uses the voltage of reference power supply for self-diagnosis. */
/* A/D data register automatic clearing enable bit (ACE) */
#define _0000_AD_AUTO_CLEAR_DISABLE                     (0x0000U) /* disables automatic clearing */
#define _0020_AD_AUTO_CLEAR_ENABLE                      (0x0020U) /* enables automatic clearing */

/*
    A/D Conversion Start Trigger Select Register (ADSTRGR) 
*/
/* A/D conversion start trigger select bit (TRSA5,TRSA4,TRSA3,TRSA2,TRSA1,TRSA0) */
#define _0000_AD_TRIGGER_ADTRG                          (0x0000U) /* external trigger input pin (ADTRG) */
#define _3000_AD_TRIGGER_ELCTRG0                        (0x3000U) /* event output signal from event link controller (ELCTRG0) */
#define _3F00_AD_TRIGGER_SOURCE                         (0x3F00U) /* trigger source deselection */

/*
    A/D Conversion Extended Input Control Register (ADEXICR) 
*/
/* Internal reference voltage A/D conversion select bit (OCSA) */
#define _0000_AD_REF_VOLTAGE_DISABLE                    (0x0000U) /* A/D conversion of internal reference voltage is not performed */
#define _0200_AD_REF_VOLTAGE_ENABLE                     (0x0200U) /* A/D conversion of internal reference voltage is performed */
/* Temperature sensor output A/D conversion select bit (TSSA) */
#define _0000_AD_TEMPERATURE_DISABLE                    (0x0000U) /* A/D conversion of temperature sensor output is not performed */
#define _0100_AD_TEMPERATURE_ENABLE                     (0x0100U) /* A/D conversion of temperature sensor output is performed */
/* Internal reference voltage A/D-converted value addition/average mode select bit (OCSAD) */
#define _0000_AD_REF_ADD_AVERAGE_DISABLE                (0x0000U) /* internal reference voltage A/D-converted value addition/average mode is not selected */
#define _0002_AD_REF_ADD_AVERAGE_ENABLE                 (0x0002U) /* internal reference voltage A/D-converted value addition/average mode is selected */
/* Temperature sensor output A/D-converted value addition/average mode select (TSSAD) */
#define _0000_AD_TEMPERATURE_ADD_AVERAGE_DISABLE        (0x0000U) /* temperature sensor output A/D-converted value addition/average mode is not selected. */
#define _0001_AD_TEMPERATURE_ADD_AVERAGE_ENABLE         (0x0001U) /* temperature sensor output A/D-converted value addition/average mode is selected. */

/*
    A/D High-potential/Low-potential Reference Voltage Control Register (ADHVREFCNT) 
*/
/* Sleep bit (ADSLP) */
#define _00_AD_SLEEP_NORMAL                             (0x00U) /* normal operation */
#define _80_AD_SLEEP_STANDBY                            (0x80U) /* standby state */
/* Low-potential reference voltage select bit (LVSEL) */
#define _00_AD_LOW_REF_AVSS                             (0x00U) /* AVSS is selected as the low-potential reference voltage */
#define _10_AD_LOW_REF_AVREFM                           (0x10U) /* AVREFM is selected as the low-potential reference voltage */
/* High-potential reference voltage select bit (HVSEL1,HVSEL0) */
#define _00_AD_HIGH_REF_AVDD                            (0x00U) /* AVDD is selected as the high-potential reference voltage */
#define _01_AD_HIGH_REF_AVREFP_VREFOUT                  (0x01U) /* AVREFP or VREFOUT is selected as the high-potential reference voltage */
#define _02_AD_HIGH_REF_VBGR                            (0x02U) /* VBGR is selected as the high-potential reference voltage. */
#define _03_AD_HIGH_REF_DISCHARGE                       (0x03U) /* discharges the internal reference voltage (the high-potential reference voltage is not selected) */

/*
    A/D Conversion Clock Control Register (ADCKS) 
*/
/* A/D conversion clock select bit (ADCKS1,ADCKS0) */
#define _00_AD_CLK_NOT_DIVIDED                          (0x00U) /* system clock not divided (f1) */
#define _01_AD_CLK_DIVIDED_2                            (0x01U) /* system clock divided by 2 (f2) */
#define _02_AD_CLK_DIVIDED_4                            (0x02U) /* system clock divided by 4 (f4) */
#define _03_AD_CLK_DIVIDED_8                            (0x03U) /* system clock divided by 8 (f8) */


/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define _24_AD_SAMPLE_STATE0_VALUE                      (0x24U) /* A/D sampling state register 0 */
#define _24_AD_SAMPLE_STATE4_VALUE                      (0x24U) /* A/D sampling state register 4 */
#define _24_AD_SAMPLE_STATE7_VALUE                      (0x24U) /* A/D sampling state register 7 */
#define _24_AD_SAMPLE_STATE8_VALUE                      (0x24U) /* A/D sampling state register 8 */
#define _24_AD_SAMPLE_STATE9_VALUE                      (0x24U) /* A/D sampling state register 9 */
#define _0391_AD_ADANSA0_GUI_VALUE                      (0x0391U)
#define _0000_AD_ADEXICR_GUI_VALUE                      (0x0000U)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef enum
{
    ADCHANNEL0,
    ADCHANNEL3,
    ADCHANNEL4,
    ADCHANNEL5,
    ADCHANNEL6,
    ADCHANNEL7,
    ADCHANNEL8,
    ADCHANNEL9,
    ADCHANNEL12,
    ADCHANNEL13,
    ADCHANNEL14,
    ADINTERREFVOLT,
    ADTEMPERSENSOR0
} ad_channel_t;

typedef enum
{
    SEL_ADANSA0,
    SEL_ADEXICR
} ad_sel_regester_t;

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_12ADC_Create(void);
void R_12ADC_Start(void);
void R_12ADC_Stop(void);
MD_STATUS R_12ADC_Get_ValueResult(ad_channel_t channel, uint16_t * const buffer);
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
