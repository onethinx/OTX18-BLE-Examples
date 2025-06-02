/* ==========================================================
 *    ___             _   _     _			
 *   / _ \ _ __   ___| |_| |__ (_)_ __ __  __
 *  | | | | '_ \ / _ \ __| '_ \| | '_ \\ \/ /https://eu1.cloud.thethings.network/console/organizations
 *  | |_| | | | |  __/ |_| | | | | | | |>  < 
 *   \___/|_| |_|\___|\__|_| |_|_|_| |_/_/\_\
 *									   
 * Copyright Onethinx, 2018
 * All Rights Reserved
 *
 * UNPUBLISHED, LICENSED SOFTWARE.
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Onethinx BV
 *
 * ==========================================================
*/

#pragma once

#include "OnethinxCore01.h"

LoRaWAN_keys_t TTN_OTAAkeys = {
// OTX_Extension_Provisioning_Start
    .KeyType                    = OTAA_10x_key,
    .PublicNetwork              = true,
    .OTAA_10x = {
        .DevEui				    = { thisDevEUI },
        .AppEui				    = {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }},
        .AppKey				    = {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }}
    }
// OTX_Extension_Provisioning_End
};

/* [] END OF FILE */