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
	.KeyType = OTAA_10x_key,
	.PublicNetwork = true,
	.OTAA_10x.DevEui = {thisDevEUI},
	.OTAA_10x.AppEui = {{0x01, 0x23, 0xCA, 0xFE, 0x01, 0x23, 0xCA, 0xFE}},
	.OTAA_10x.AppKey = {{0xE6, 0xDB, 0x34, 0x07, 0xA1, 0xBF, 0x94, 0xCB, 0xE2, 0x39, 0x7A, 0x91, 0x12, 0x0E, 0xD8, 0x0F}}};

/* [] END OF FILE */