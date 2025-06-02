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
        .DevEui                 = { thisDevEUI },
        .AppEui                 = {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }}, // 0000000000000000
        .AppKey                 = {{ 0xB2, 0x91, 0x28, 0x93, 0xED, 0x7B, 0xB6, 0x4A, 0x9B, 0xAE, 0xD9, 0x2B, 0xFE, 0x36, 0x36, 0x5E }}  // B2912893ED7BB64A9BAED92BFE36365E
    }
// OTX_Extension_Provisioning_End
};

LoRaWAN_keys_t TTN_OTAAkeys2 = {
	.KeyType 						= OTAA_10x_key,
	.PublicNetwork					= true,
	.OTAA_10x.DevEui				= { thisDevEUI },
	.OTAA_10x.AppEui				= {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }},
	.OTAA_10x.AppKey				= {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }}
};

/* [] END OF FILE */