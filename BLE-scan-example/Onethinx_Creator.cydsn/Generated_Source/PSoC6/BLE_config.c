/***************************************************************************//**
* \file CY_BLE.c
* \version 2.20
* 
* \brief
*  This file contains the source code of initialization of the config structure
*  for BLE.
* 
********************************************************************************
* \copyright
* Copyright 2017-2019, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "BLE_config.h"

#if (CY_BLE_HOST_CONTR_CORE)    
#include "flash/cy_flash.h"
#include "ble/cy_ble_event_handler.h"
#include "cyfitter_sysint_cfg.h"

#if (CY_BLE_MODE_PROFILE)
/***************************************
* Global Variables
***************************************/
/* Initializes the cy_stc_ble_gapp_disc_mode_info_t cy_ble_discoveryModeInfo  structure */
#if (CY_BLE_GAP_ROLE_PERIPHERAL || CY_BLE_GAP_ROLE_BROADCASTER)
static cy_stc_ble_gapp_adv_params_t cy_ble_gappAdvConfig[0x01u] = {

    /* Peripheral configuration 0 */
    {
        .fastAdvIntervalMin                 = 0x00A0u, 
        .fastAdvIntervalMax                 = 0x0140u, 
        .fastAdvTimeOut                     = 0x00B4u, 
        .slowAdvEnable                      = 0x00u, 
        .slowAdvIntervalMin                 = 0x0640u, 
        .slowAdvIntervalMax                 = 0x0FA0u, 
        .slowAdvTimeOut                     = 0x0096u, 
    },
};


cy_stc_ble_gapp_disc_param_t cy_ble_discoveryParam[0x01u] = {

    /* Peripheral configuration 0 */
    {
        0x00A0u, /* uint16_t advertising_interval_min */ 
        0x0140u, /* uint16_t advertising_interval_max */ 
        CY_BLE_GAPP_CONNECTABLE_UNDIRECTED_ADV, /* uint8_t advertising_type */ 
        0x00u, /* uint8_t own_addr_type */ 
        0x00u, /* uint8_t direct_addr_type */ 
        {0x01u, 0x00u, 0x00u, 0x50u, 0xA0u, 0x00u}, /* uint8_t* direct_addr */ 
        0x07u, /* uint8_t advertising_channel_map */ 
        0x00u, /* uint8_t advertising_filter_policy */ 
    },
};


cy_stc_ble_gapp_disc_data_t cy_ble_discoveryData[0x01u] = {

    /* Peripheral configuration 0 */
    {
        { 0x02u, 0x01u, 0x05u, 0x10u, 0x09u, 0x4Fu, 0x54u,
        0x58u, 0x20u, 0x42u, 0x4Cu, 0x45u, 0x20u, 0x45u,
        0x78u, 0x61u, 0x6Du, 0x70u, 0x6Cu, 0x65u, 0x03u,
        0x03u, 0x0Au, 0x18u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u }, /* uint8_t advertising_data[CY_BLE_MAX_ADV_DATA_LEN] */ 
        0x18u, /* uint8_t adv_data_length */ 
    },
};


cy_stc_ble_gapp_scan_rsp_data_t cy_ble_scanRspData[0x01u] = {

    /* Peripheral configuration 0 */
    {
        { 0x0Au, 0x08u, 0x4Fu, 0x54u, 0x58u, 0x20u, 0x42u,
        0x4Cu, 0x45u, 0x20u, 0x45u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u }, /* uint8_t scan_rsp_data[CY_BLE_MAX_SCAN_RSP_DATA_LEN] */ 
        0x0Bu, /* uint8_t scan_rsp_data_length */ 
    },
};


/* This array of type cy_stc_ble_gapp_disc_mode_info_t is present only when the 
   BLE component is configured for Peripheral or Broadcaster GAP roles. 
   It contains the Advertisement settings and also the Advertisement and 
   Scan response data parameters entered in the customizer. This variable
   can be used by advanced users to change Advertisement settings,
   Advertisement or Scan response data in runtime. 
*/
cy_stc_ble_gapp_disc_mode_info_t cy_ble_discoveryModeInfo[0x01u] = {

    /* Peripheral configuration 0 */
    {
        0x01u, /* uint8_t discMode */ 
        &cy_ble_discoveryParam[0], 
        &cy_ble_discoveryData[0], 
        &cy_ble_scanRspData[0], 
        0x00B4u, /* uint16_t advTo */ 
    },
};


#endif /* CY_BLE_GAP_ROLE_PERIPHERAL || CY_BLE_GAP_ROLE_BROADCASTER */

/* Initializes the cy_stc_ble_gapc_disc_info_t  cy_ble_discoveryInfo  structure */
#if (CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_OBSERVER)
static cy_stc_ble_gapc_scan_params_t cy_ble_gapcScanConfig[0x01u] = {

    /* Central configuration 0 */
    {
        .fastScanInterval                   = 0x0050u, 
        .fastScanWindow                     = 0x0050u, 
        .fastScanTimeOut                    = 0x001Eu, 
        .slowScanEnabled                    = 0x01u, 
        .slowScanInterval                   = 0x0800u, 
        .slowScanWindow                     = 0x0708u, 
        .slowScanTimeOut                    = 0x0096u, 
        .gapcConnectionIntervalMin          = 0x0006u, 
        .gapcConnectionIntervalMax          = 0x0028u, 
        .gapcConnectionSlaveLatency         = 0x0000u, 
        .gapcConnectionTimeOut              = 0x03E8u, 
    },
};


/* This array of type cy_stc_ble_gapc_disc_info_t is present only when  
   the BLE component is configured for Central or Observer GAP roles.  
   It contains the Scan settings entered in the customizer.  
   This variable can be used by advanced users to change Scan  
   settings in runtime.
*/
cy_stc_ble_gapc_disc_info_t cy_ble_discoveryInfo[0x01u] = {
    {
        0x02u, /* discProcedure */ 
        0x01u, /* scanType */ 
        0x0050u, /* scanInterval */ 
        0x0050u, /* scanWindow */ 
        0x00u, /* ownAddrType */ 
        0x00u, /* scanFilterPolicy */ 
        0x001Eu, /* scanTO */ 
        0x00u, /* filterDuplicates */ 
    },
};

#endif /* CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_OBSERVER */

#if ((CY_BLE_MODE_PROFILE) && (CY_BLE_BONDING_REQUIREMENT == CY_BLE_BONDING_YES))

#if(CY_BLE_MODE_PROFILE)
    CY_SECTION(".cy_em_eeprom") CY_ALIGN(CY_FLASH_SIZEOF_ROW) static const cy_stc_ble_flash_storage_t cy_ble_flashStorage =
    {
        { 0x00u }, 
        {{
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        }}, 
        0x04u, /* CY_BLE_GATT_DB_CCCD_COUNT */ 
        0x06u, 
    };
#endif /* (CY_BLE_MODE_PROFILE) */

#endif  /* (CY_BLE_MODE_PROFILE) && (CY_BLE_BONDING_REQUIREMENT == CY_BLE_BONDING_YES) */

#if (CY_BLE_GATT_ROLE_SERVER)
static const cy_stc_ble_gatts_t cy_ble_gatts =
{
    0x0008u,    /* Handle of the GATT service */
    0x000Au,    /* Handle of the Service Changed characteristic */
    0x000Bu,    /* Handle of the Client Characteristic Configuration descriptor */
};
static const cy_stc_ble_gaps_t cy_ble_gaps =
{
    0x0001u,    /* Handle of the GAP service */
    0x0003u,    /* Handle of the Device Name characteristic */
    0x0005u,    /* Handle of the Appearance characteristic */
    0x0007u,    /* Handle of the Peripheral Preferred Connection Parameters characteristic */
    CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Central Address Resolution characteristic */
    CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Resolvable Private Address Only characteristic */
};
static uint8_t cy_ble_attValues[0x0273u] = {
    /* Device Name */
    (uint8_t)'O', (uint8_t)'T', (uint8_t)'X', (uint8_t)' ', (uint8_t)'B', (uint8_t)'L', (uint8_t)'E', (uint8_t)' ',
(uint8_t)'E', (uint8_t)'x', (uint8_t)'a', (uint8_t)'m', (uint8_t)'p', (uint8_t)'l', (uint8_t)'e', 

    /* Appearance */
    0x00u, 0x00u, 

    /* Peripheral Preferred Connection Parameters */
    0x06u, 0x00u, 0x80u, 0x0Cu, 0x00u, 0x00u, 0xE8u, 0x03u, 

    /* Service Changed */
    0x00u, 0x00u, 0x00u, 0x00u, 

    /* Manufacturer Name String */
    (uint8_t)'C', (uint8_t)'y', (uint8_t)'p', (uint8_t)'r', (uint8_t)'e', (uint8_t)'s', (uint8_t)'s', (uint8_t)' ',
(uint8_t)'S', (uint8_t)'e', (uint8_t)'m', (uint8_t)'i', (uint8_t)'c', (uint8_t)'o', (uint8_t)'n', (uint8_t)'d',
(uint8_t)'u', (uint8_t)'c', (uint8_t)'t', (uint8_t)'o', (uint8_t)'r', 

    /* Model Number String */
    (uint8_t)'D', (uint8_t)'I', (uint8_t)'S', (uint8_t)' ', (uint8_t)'v', (uint8_t)'1', (uint8_t)'.', (uint8_t)'0', 

    /* Serial Number String */
    (uint8_t)'\0', (uint8_t)'\0', (uint8_t)'\0', (uint8_t)'\0', (uint8_t)'\0', (uint8_t)'\0', (uint8_t)'\0', (uint8_t)'\0',
(uint8_t)'\0', (uint8_t)'\0', 

    /* Hardware Revision String */
    (uint8_t)'C', (uint8_t)'Y', (uint8_t)'8', (uint8_t)'C', (uint8_t)'K', (uint8_t)'I', (uint8_t)'T', (uint8_t)'-',
(uint8_t)'0', (uint8_t)'6', (uint8_t)'2', (uint8_t)' ', (uint8_t)'P', (uint8_t)'S', (uint8_t)'o', (uint8_t)'C',
(uint8_t)' ', (uint8_t)'6', (uint8_t)' ', (uint8_t)'B', (uint8_t)'L', (uint8_t)'E', (uint8_t)' ', (uint8_t)'P',
(uint8_t)'i', (uint8_t)'o', (uint8_t)'n', (uint8_t)'e', (uint8_t)'e', (uint8_t)'r', (uint8_t)' ', (uint8_t)'K',
(uint8_t)'i', (uint8_t)'t', 

    /* Firmware Revision String */
    (uint8_t)'\0', (uint8_t)'\0', (uint8_t)'\0', (uint8_t)'\0', (uint8_t)'\0', (uint8_t)'\0', (uint8_t)'\0', (uint8_t)'\0',
(uint8_t)'\0', 

    /* Software Revision String */
    (uint8_t)'C', (uint8_t)'y', (uint8_t)'S', (uint8_t)'m', (uint8_t)'a', (uint8_t)'r', (uint8_t)'t', (uint8_t)' ',
(uint8_t)'t', (uint8_t)'o', (uint8_t)'o', (uint8_t)'l', 

    /* System ID */
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 

    /* IEEE 11073-20601 Regulatory Certification Data List */
    0x00u, 

    /* PnP ID */
    0x01u, 0xB4u, 0x04u, 0x01u, 0x00u, 0x01u, 0x00u, 

    /* Data Out */
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 

    /* Data In */
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 

};
#if(CY_BLE_GATT_DB_CCCD_COUNT != 0u)
static uint8_t cy_ble_attValuesCCCD[CY_BLE_GATT_DB_CCCD_COUNT];
#endif /* CY_BLE_GATT_DB_CCCD_COUNT != 0u */

static const uint8_t cy_ble_attUuid128[][16u] = {
    /* Data InOut */
    { 0x97u, 0xF3u, 0x8Eu, 0x85u, 0x9Fu, 0x87u, 0x4Cu, 0x8Bu, 0x86u, 0x4Fu, 0x08u, 0x8Cu, 0x6Du, 0x2Fu, 0x7Au, 0xCBu },
    /* Data Out */
    { 0x9Du, 0xB3u, 0x07u, 0x35u, 0x1Cu, 0xFDu, 0xA6u, 0xBFu, 0x80u, 0x48u, 0xF5u, 0xC1u, 0xD9u, 0xE7u, 0x0Fu, 0x31u },
    /* Data In */
    { 0x27u, 0xC0u, 0x5Fu, 0xD5u, 0xC0u, 0xA3u, 0x45u, 0x9Bu, 0xC4u, 0x44u, 0x42u, 0x75u, 0x31u, 0xF4u, 0xA6u, 0x70u },
};

static cy_stc_ble_gatts_att_gen_val_len_t cy_ble_attValuesLen[0x14u] = {
    { 0x000Fu, (void *)&cy_ble_attValues[0] }, /* Device Name */
    { 0x0002u, (void *)&cy_ble_attValues[15] }, /* Appearance */
    { 0x0008u, (void *)&cy_ble_attValues[17] }, /* Peripheral Preferred Connection Parameters */
    { 0x0004u, (void *)&cy_ble_attValues[25] }, /* Service Changed */
    { 0x0002u, (void *)&cy_ble_attValuesCCCD[0] }, /* Client Characteristic Configuration */
    { 0x0015u, (void *)&cy_ble_attValues[29] }, /* Manufacturer Name String */
    { 0x0008u, (void *)&cy_ble_attValues[50] }, /* Model Number String */
    { 0x000Au, (void *)&cy_ble_attValues[58] }, /* Serial Number String */
    { 0x0022u, (void *)&cy_ble_attValues[68] }, /* Hardware Revision String */
    { 0x0009u, (void *)&cy_ble_attValues[102] }, /* Firmware Revision String */
    { 0x000Cu, (void *)&cy_ble_attValues[111] }, /* Software Revision String */
    { 0x0008u, (void *)&cy_ble_attValues[123] }, /* System ID */
    { 0x0001u, (void *)&cy_ble_attValues[131] }, /* IEEE 11073-20601 Regulatory Certification Data List */
    { 0x0007u, (void *)&cy_ble_attValues[132] }, /* PnP ID */
    { 0x0010u, (void *)&cy_ble_attUuid128[0] }, /* Data InOut UUID */
    { 0x0010u, (void *)&cy_ble_attUuid128[1] }, /* Data Out UUID */
    { 0x00F4u, (void *)&cy_ble_attValues[139] }, /* Data Out */
    { 0x0002u, (void *)&cy_ble_attValuesCCCD[2] }, /* Client Characteristic Configuration */
    { 0x0010u, (void *)&cy_ble_attUuid128[2] }, /* Data In UUID */
    { 0x00F4u, (void *)&cy_ble_attValues[383] }, /* Data In */
};

static const cy_stc_ble_gatts_db_t cy_ble_gattDB[0x24u] = {
    { 0x0001u, 0x2800u /* Primary service                     */, 0x00000001u /*       */, 0x0007u, {{0x1800u, NULL}}                           },
    { 0x0002u, 0x2803u /* Characteristic                      */, 0x00020001u /* rd    */, 0x0003u, {{0x2A00u, NULL}}                           },
    { 0x0003u, 0x2A00u /* Device Name                         */, 0x01020001u /* rd    */, 0x0003u, {{0x000Fu, (void *)&cy_ble_attValuesLen[0]}} },
    { 0x0004u, 0x2803u /* Characteristic                      */, 0x00020001u /* rd    */, 0x0005u, {{0x2A01u, NULL}}                           },
    { 0x0005u, 0x2A01u /* Appearance                          */, 0x01020001u /* rd    */, 0x0005u, {{0x0002u, (void *)&cy_ble_attValuesLen[1]}} },
    { 0x0006u, 0x2803u /* Characteristic                      */, 0x00020001u /* rd    */, 0x0007u, {{0x2A04u, NULL}}                           },
    { 0x0007u, 0x2A04u /* Peripheral Preferred Connection Par */, 0x01020001u /* rd    */, 0x0007u, {{0x0008u, (void *)&cy_ble_attValuesLen[2]}} },
    { 0x0008u, 0x2800u /* Primary service                     */, 0x00000001u /*       */, 0x000Bu, {{0x1801u, NULL}}                           },
    { 0x0009u, 0x2803u /* Characteristic                      */, 0x00200001u /* ind   */, 0x000Bu, {{0x2A05u, NULL}}                           },
    { 0x000Au, 0x2A05u /* Service Changed                     */, 0x01200000u /* ind   */, 0x000Bu, {{0x0004u, (void *)&cy_ble_attValuesLen[3]}} },
    { 0x000Bu, 0x2902u /* Client Characteristic Configuration */, 0x030A0101u /* rd,wr */, 0x000Bu, {{0x0002u, (void *)&cy_ble_attValuesLen[4]}} },
    { 0x000Cu, 0x2800u /* Primary service                     */, 0x00000001u /*       */, 0x001Eu, {{0x180Au, NULL}}                           },
    { 0x000Du, 0x2803u /* Characteristic                      */, 0x00020001u /* rd    */, 0x000Eu, {{0x2A29u, NULL}}                           },
    { 0x000Eu, 0x2A29u /* Manufacturer Name String            */, 0x01020001u /* rd    */, 0x000Eu, {{0x0015u, (void *)&cy_ble_attValuesLen[5]}} },
    { 0x000Fu, 0x2803u /* Characteristic                      */, 0x00020001u /* rd    */, 0x0010u, {{0x2A24u, NULL}}                           },
    { 0x0010u, 0x2A24u /* Model Number String                 */, 0x01020001u /* rd    */, 0x0010u, {{0x0008u, (void *)&cy_ble_attValuesLen[6]}} },
    { 0x0011u, 0x2803u /* Characteristic                      */, 0x00020001u /* rd    */, 0x0012u, {{0x2A25u, NULL}}                           },
    { 0x0012u, 0x2A25u /* Serial Number String                */, 0x01020001u /* rd    */, 0x0012u, {{0x000Au, (void *)&cy_ble_attValuesLen[7]}} },
    { 0x0013u, 0x2803u /* Characteristic                      */, 0x00020001u /* rd    */, 0x0014u, {{0x2A27u, NULL}}                           },
    { 0x0014u, 0x2A27u /* Hardware Revision String            */, 0x01020001u /* rd    */, 0x0014u, {{0x0022u, (void *)&cy_ble_attValuesLen[8]}} },
    { 0x0015u, 0x2803u /* Characteristic                      */, 0x00020001u /* rd    */, 0x0016u, {{0x2A26u, NULL}}                           },
    { 0x0016u, 0x2A26u /* Firmware Revision String            */, 0x01020001u /* rd    */, 0x0016u, {{0x0009u, (void *)&cy_ble_attValuesLen[9]}} },
    { 0x0017u, 0x2803u /* Characteristic                      */, 0x00020001u /* rd    */, 0x0018u, {{0x2A28u, NULL}}                           },
    { 0x0018u, 0x2A28u /* Software Revision String            */, 0x01020001u /* rd    */, 0x0018u, {{0x000Cu, (void *)&cy_ble_attValuesLen[10]}} },
    { 0x0019u, 0x2803u /* Characteristic                      */, 0x00020001u /* rd    */, 0x001Au, {{0x2A23u, NULL}}                           },
    { 0x001Au, 0x2A23u /* System ID                           */, 0x01020001u /* rd    */, 0x001Au, {{0x0008u, (void *)&cy_ble_attValuesLen[11]}} },
    { 0x001Bu, 0x2803u /* Characteristic                      */, 0x00020001u /* rd    */, 0x001Cu, {{0x2A2Au, NULL}}                           },
    { 0x001Cu, 0x2A2Au /* IEEE 11073-20601 Regulatory Certifi */, 0x01020001u /* rd    */, 0x001Cu, {{0x0001u, (void *)&cy_ble_attValuesLen[12]}} },
    { 0x001Du, 0x2803u /* Characteristic                      */, 0x00020001u /* rd    */, 0x001Eu, {{0x2A50u, NULL}}                           },
    { 0x001Eu, 0x2A50u /* PnP ID                              */, 0x01020001u /* rd    */, 0x001Eu, {{0x0007u, (void *)&cy_ble_attValuesLen[13]}} },
    { 0x001Fu, 0x2800u /* Primary service                     */, 0x08000001u /*       */, 0x0024u, {{0x0010u, (void *)&cy_ble_attValuesLen[14]}} },
    { 0x0020u, 0x2803u /* Characteristic                      */, 0x00100001u /* ntf   */, 0x0022u, {{0x0010u, (void *)&cy_ble_attValuesLen[15]}} },
    { 0x0021u, 0xE7D9u /* Data Out                            */, 0x09100000u /* ntf   */, 0x0022u, {{0x00F4u, (void *)&cy_ble_attValuesLen[16]}} },
    { 0x0022u, 0x2902u /* Client Characteristic Configuration */, 0x030A0101u /* rd,wr */, 0x0022u, {{0x0002u, (void *)&cy_ble_attValuesLen[17]}} },
    { 0x0023u, 0x2803u /* Characteristic                      */, 0x00040001u /* wwr   */, 0x0024u, {{0x0010u, (void *)&cy_ble_attValuesLen[18]}} },
    { 0x0024u, 0xF431u /* Data In                             */, 0x09040100u /* wwr   */, 0x0024u, {{0x00F4u, (void *)&cy_ble_attValuesLen[19]}} },
};

#endif /* (CY_BLE_GATT_ROLE_SERVER) */

/* Default device security */
#if (CY_BLE_MODE_PROFILE) 
    cy_stc_ble_gap_auth_info_t cy_ble_authInfo[0x01u] = {

    /* Security configuration 0 */
    {
        .security = (CY_BLE_GAP_SEC_MODE_1 | CY_BLE_GAP_SEC_LEVEL_1 ), 
        .bonding = CY_BLE_GAP_BONDING_NONE, 
        .ekeySize = 0x10u, 
        .authErr = CY_BLE_GAP_AUTH_ERROR_NONE, 
        .pairingProperties = 0x00u, 
    },
};

#endif /* CY_BLE_MODE_PROFILE */

/** Initialize BLE configuration parameters structure */
static const cy_stc_ble_params_t cy_ble_params =
{
        .txPowerLevelAdv                    = CY_BLE_LL_PWR_LVL_0_DBM,
        .txPowerLevelConn                   = CY_BLE_LL_PWR_LVL_0_DBM,

        .securityIoCapability               = CY_BLE_GAP_IOCAP_NOINPUT_NOOUTPUT,
        .securityPairingMethod              = 0x00u,
    
        .siliconDeviceAddressEnabled        = 0x00u,
    
        .gattDbIndexCount                   = 0x0024u,
};

cy_stc_ble_gap_bd_addr_t cy_ble_deviceAddress = {{0x01u, 0x00u, 0x00u, 0x50u, 0xA0u, 0x00u}, 0x00u };

/**
* \addtogroup group_globals
* @{
*/
#endif /* CY_BLE_MODE_PROFILE */

/** The configuration structure for BLE */
cy_stc_ble_config_t cy_ble_config =
{
#if (CY_BLE_MODE_PROFILE)
    /* Initialize the GAPP structures */
    /* Initialize the cy_stc_ble_gapp_disc_mode_info_t cy_ble_discoveryModeInfo structure */
    #if (CY_BLE_GAP_ROLE_PERIPHERAL || CY_BLE_GAP_ROLE_BROADCASTER)
        .discoveryModeInfo = cy_ble_discoveryModeInfo,
        .gappAdvParams = cy_ble_gappAdvConfig,
    #else
        .discoveryModeInfo = NULL,
    #endif /* CY_BLE_GAP_ROLE_PERIPHERAL || CY_BLE_GAP_ROLE_BROADCASTER */

    /* Initialize the cy_stc_ble_gapc_disc_info_t  cy_ble_discoveryInfo  structure */
    #if (CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_OBSERVER)
        .discoveryInfo = cy_ble_discoveryInfo,
        .gapcScanParams = cy_ble_gapcScanConfig,
    #else
        .discoveryInfo = NULL,
    #endif /* CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_OBSERVER */

    /* Initialize the GATT structures */
    #if ((CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL) && (CY_BLE_BONDING_REQUIREMENT == CY_BLE_BONDING_YES))
        .flashStorage = &cy_ble_flashStorage,
    #else
        .flashStorage = NULL,
    #endif /* CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL) && (CY_BLE_BONDING_REQUIREMENT == CY_BLE_BONDING_YES */

    #if (CY_BLE_GATT_ROLE_SERVER)
        .gatts            = &cy_ble_gatts,
        .gaps             = &cy_ble_gaps,
        .attValuesCCCD    = cy_ble_attValuesCCCD,
        .gattDB           = cy_ble_gattDB,
    #else
        .gatts            = NULL,
        .attValuesCCCD    = NULL,
        .gattDB           = NULL,
    #endif /* CY_BLE_GATT_ROLE_SERVER */

    #if (CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL)
        /* Initialize the device security structure */
        .authInfo = cy_ble_authInfo,
    #else
        .authInfo = NULL,
    #endif /* (CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL */

    /* Initialize the BLE configuration parameters structure */
    .params   = &cy_ble_params,

    /* An application layer event callback function to receive service events from the BLE Component. */
    .callbackFunc   = NULL,
    
    .deviceAddress  = &cy_ble_deviceAddress,
#endif /* CY_BLE_MODE_PROFILE */

#if (CY_BLE_CONFIG_STACK_CONTR_CORE)
    /* The BLESS interrupt configuration */
    .blessIsrConfig = &BLE_bless_isr_cfg,
#endif /* CY_BLE_CONFIG_STACK_CONTR_CORE */
};

#endif /* CY_BLE_HOST_CONTR_CORE */ 

/** @} group_globals */

/* [] END OF FILE */
