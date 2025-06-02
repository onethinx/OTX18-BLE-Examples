/********************************************************************************
 *    ___             _   _     _			
 *   / _ \ _ __   ___| |_| |__ (_)_ __ __  __
 *  | | | | '_ \ / _ \ __| '_ \| | '_ \\ \/ /
 *  | |_| | | | |  __/ |_| | | | | | | |>  < 
 *   \___/|_| |_|\___|\__|_| |_|_|_| |_/_/\_\
 *
 ********************************************************************************
 *
 * Copyright (c) 2019-2022 Onethinx BV <info@onethinx.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 ********************************************************************************
 *
 * Created by: Rolf Nooteboom on 2024-04-09
 *
 * Sample project to demostrate the BLE capabilities with the OTX-18 module
 * 
 * For a description please see:
 * https://github.com/onethinx/BLE-example
 *
 ********************************************************************************/

#include "project.h"
#include "ble.h"

#include "OnethinxCore01.h"
#include "LoRaWAN_keys.h"

/* Go to ../OnethinxCore/LoRaWAN_keys.h and fill in the fields of the TTN_OTAAkeys structure */

coreConfiguration_t	coreConfig = {
	.Join =
	{
		.KeysPtr = 			&TTN_OTAAkeys,
		.DataRate =			DR_AUTO,
		.Power =			PWR_MAX,
		.MAXTries = 		4,
		.SubBand_1st =     	EU_SUB_BANDS_DEFAULT,
		.SubBand_2nd =     	EU_SUB_BANDS_DEFAULT
	},
	.TX =
	{
		.Confirmed = 		false,
		.DataRate = 		DR_0,
		.Power = 			PWR_MAX,
		.FPort = 			1
	},
	.RX =
	{
		.Boost = 			true
	},
	.System =
	{
		.Idle =
		{
			.Mode = 		M0_Active,	// Must be Active for BLE operation
			.BleEcoON =		true,		// Must be ON for BLE operation
			.DebugON =		true,
		}
	}
};

sleepConfig_t sleepConfig =
{
	.sleepMode = modeDeepSleep,
	.BleEcoON = false,
	.DebugON = true,
	.sleepCores = coresBoth,
	.wakeUpPin = wakeUpPinHigh(true),
	.wakeUpTime = wakeUpDelay(0, 0, 0, 30), // day, hour, minute, second
};

typedef enum
{
	lorawan_idle = 0,
	lorawan_join = 0x10,
    lorawan_send = 0x11,
} LoRaStatus_e;

/* OnethinxCore uses the following structures and variables, which can be defined globally */
coreStatus_t 	coreStatus;
coreInfo_t 		coreInfo;
LoRaStatus_e	LoRaStatus;

uint8_t RXbuffer[64];
uint8_t TXbuffer[64];

/*******************************************************************************
* Function Name: DeviceSleep
********************************************************************************
*
* Summary:
*   Turns off used H/W and GPIOs and Puts the device in sleep mode
*
*******************************************************************************/
void DeviceSleep(sleepConfig_t * sleepConfig)
{
    uint32_t RX_HSIOM = Cy_GPIO_GetHSIOM(RX_IN_PORT, RX_IN_NUM);
    uint32_t TX_HSIOM = Cy_GPIO_GetHSIOM(TX_OUT_PORT, TX_OUT_NUM);
    Cy_GPIO_Pin_FastInit(RX_IN_PORT, RX_IN_NUM, CY_GPIO_DM_HIGHZ, 0, HSIOM_SEL_GPIO);
	Cy_GPIO_Pin_FastInit(TX_OUT_PORT, TX_OUT_NUM, CY_GPIO_DM_HIGHZ, 0, HSIOM_SEL_GPIO);
    Cy_GPIO_Write(LED_B_PORT, LED_B_NUM, 0);
    Cy_GPIO_Write(LED_R_PORT, LED_R_NUM, 0);
    Cy_SCB_UART_Disable(UART_HW, &UART_context);
    LoRaWAN_Sleep(sleepConfig);
    UART_Start();
    Cy_GPIO_Pin_FastInit(RX_IN_PORT, RX_IN_NUM, CY_GPIO_DM_HIGHZ, 1, RX_HSIOM);
	Cy_GPIO_Pin_FastInit(TX_OUT_PORT, TX_OUT_NUM, CY_GPIO_DM_STRONG_IN_OFF, 1, TX_HSIOM);
}

/*******************************************************************************
* Function Name: UpdateLedState
********************************************************************************
*
* Summary:
*   Handles indications on Advertising and Disconnection LEDs.
*
* NOTE: To set breakpoints, please disable compiler inline optimizations
* using -fno-inline or -O0
*******************************************************************************/
void UpdateLedState(void)
{
    if(Cy_BLE_GetAdvertisementState() == CY_BLE_ADV_STATE_ADVERTISING)
    {
        /* In advertising state, turn off disconnect indication LED */
        Cy_GPIO_Write(LED_R_PORT, LED_R_NUM, 0);

        /* Blink advertising indication LED */
        Cy_GPIO_Inv(LED_B_PORT, LED_B_NUM);
    }
    else if(Cy_BLE_GetNumOfActiveConn() == 0u)
    {
        /* If in disconnected state, turn on disconnect indication LED and turn
        * off Advertising LED.
        */
        Cy_GPIO_Write(LED_R_PORT, LED_R_NUM, 1);
        Cy_GPIO_Write(LED_B_PORT, LED_B_NUM, 0);
    }
    else 
    {
        /* In connected state, turn off disconnect indication and turn on 
        * blue LEDs. 
        */
        Cy_GPIO_Write(LED_R_PORT, LED_R_NUM, 0);
        Cy_GPIO_Write(LED_B_PORT, LED_B_NUM, 1);
    }
}

/*******************************************************************************
* Function Name: _write
********************************************************************************
* Summary: 
* NewLib C library is used to retarget printf to _write. printf is redirected to 
* this function when GCC compiler is used to print data to terminal using UART. 
*
* \param file
* This variable is not used.
* \param *ptr
* Pointer to the data which will be transfered through UART.
* \param len
* Length of the data to be transfered through UART.
*
* \return
* returns the number of characters transferred using UART.
* \ref int
*******************************************************************************/
int _write(int file __attribute__((unused)), char *ptr, int len)
{
    return Cy_SCB_UART_PutArray(UART_HW, ptr, len);
}

/*******************************************************************************
* Function Name: HostMain
********************************************************************************
*
* Summary:
*   Main function for the project.
*
* Theory:
*   The function starts BLE.
*   This function processes all BLE events and also implements the low power 
*   functionality.
*
*******************************************************************************/

uint32_t btcnt = 0;
uint32_t RXtimeout = 0;
uint32_t LEDdelay = 0;
uint8_t RXcnt = 0;

BLEbuffer buffer = {{'O', 'T', 'X'}, 3};

int main(void)
{
	/* enable global interrupts */
	__enable_irq();

    /* Start UART (115200 baud, RX on P9_0, TX on P9_1) */
	UART_Start();

    /* initialize radio with parameters in coreConfig */
	coreStatus = LoRaWAN_Init(&coreConfig);

	/* Check Onethinx Core info */
	coreStatus = LoRaWAN_GetInfo(&coreInfo);

    uint8_t devAddress[] = {0x00, 0xA0u, 0x50, 0x00, coreInfo.devEUI[6], coreInfo.devEUI[7]};
	/* Initialize BLE */
    Ble_Init((uint8_t *) &devAddress);
	LoRaStatus = lorawan_idle;
   
    for( ; ; ) {
        /* Allow BLE stack to process pending events */
        if (Ble_ProcessEvents()) break;

		coreStatus = LoRaWAN_GetStatus();

        /* Send notification at button press */
		if (Cy_GPIO_Read(BUTTON_PORT, BUTTON_NUM))
		{
			if (++btcnt == 10) {
				buffer.data[0] = 40;			/// <param name="tone">Semitone offset from C4 (0⇒C4, 12⇒C5, etc.).</param>
				buffer.data[1] = 50;			/// <param name="time">Duration in 10 ms units (e.g., 30 ⇒ 300 ms).</param>
				buffer.length = 2;
				buffer.command = host_beep;
				Ble_SendNotification();
				buffer.command = cmd_idle;
			}
		}
		else btcnt = 0;

        /* Check for data in UART RX buffer */
        while (Cy_SCB_UART_GetNumInRxFifo(UART_HW))
        {
            if (RXcnt < 240) buffer.data[RXcnt++] = Cy_SCB_UART_Get(UART_HW);
            RXtimeout = 1000;
        }
        /* Send notification with UART data after a small timeout */
        if (RXtimeout && RXtimeout-- == 1)
        {
            buffer.length = RXcnt;
			buffer.command = host_message;
            Ble_SendNotification();
			buffer.command = cmd_idle;
            RXcnt = 0;
        }

		if (buffer.command != cmd_idle)
		{
			switch (buffer.command)
			{
				case dev_uart_send:
				{
					uint8_t cnt = 0;
					DEBUG_BLE("'%.*s' (", buffer.length, buffer.data);
					for (; cnt < buffer.length - 1; cnt ++) DEBUG_BLE("%2X-", buffer.data[cnt]);
					DEBUG_BLE("%2X)\n", buffer.data[cnt]);
				}
				break;
				case dev_lorawan_join:
					if (coreStatus.system.isBusy)
					{
						buffer.command = host_error;
						buffer.length = sprintf((char *) &buffer.data, "Error: LoRa radio busy!");
					}
					else
					{
						coreStatus = LoRaWAN_Join(M4_NoWait);
						LoRaStatus = lorawan_join;
						buffer.command = host_message;
						buffer.length = sprintf((char *) &buffer.data, "Joining...");
					}
					Ble_SendNotification();
				break;
				case dev_lorawan_send:
					if (coreStatus.system.isBusy)
					{
						buffer.command = host_error;
						buffer.length = sprintf((char *) &buffer.data, "Error: LoRa radio busy!");
					}
					else if (!coreStatus.mac.isJoined)
					{
						buffer.command = host_error;
						buffer.length = sprintf((char *) &buffer.data, "Error: Not Joined!");
					}
					else
					{
						coreStatus = LoRaWAN_Send((uint8_t *) buffer.data, buffer.length, M4_NoWait);
						LoRaStatus = lorawan_send;
						buffer.command = host_message;
						buffer.length = sprintf((char *) &buffer.data, "Sending...");
					}
					Ble_SendNotification();
				break;
			}
			buffer.command = cmd_idle;
		}


		if (LoRaStatus != lorawan_idle && !coreStatus.system.isBusy)
		{
			uint32_t loraError = LoRaWAN_GetError().errorValue;
			if (loraError == errorStatus_NoError)
			{
				buffer.command = host_message;
				buffer.length = sprintf((char *) &buffer.data, "Finished OK!");
				Ble_SendNotification();
			}
			else
			{
				buffer.command = host_error;
				buffer.length = sprintf((char *) &buffer.data, "Error: %8X", loraError);
				Ble_SendNotification();
			}
			buffer.command = cmd_idle;
			LoRaStatus = lorawan_idle;
		}

        /* Update Leds once in a while */
        if ((++LEDdelay & 0x1FFF) == 1) 
		{
			if (coreStatus.system.isBusy) {
				Cy_GPIO_Inv(LED_B_PORT, LED_B_NUM);
				Cy_GPIO_Write(LED_R_PORT, LED_R_NUM, Cy_GPIO_ReadOut(LED_B_PORT, LED_B_NUM) == 0);
			}
			else
				UpdateLedState();
		}	
    }

	/* send join using parameters in coreConfig, blocks until either success or MAXtries */	coreStatus = LoRaWAN_Join(M4_NoWait);
    while ((coreStatus = LoRaWAN_GetStatus()).system.isBusy) {
        Cy_GPIO_Write(LED_R_PORT, LED_R_NUM, 1);
        Cy_GPIO_Write(LED_B_PORT, LED_B_NUM, 0);
        CyDelay(500);
        Cy_GPIO_Write(LED_R_PORT, LED_R_NUM, 0);
        Cy_GPIO_Write(LED_B_PORT, LED_B_NUM, 1);
        CyDelay(500);
    }

	/* check for successful join, flash Red LED if not joined */
	if (!coreStatus.mac.isJoined) {
		while(1) {
			Cy_GPIO_Write(LED_B_PORT, LED_B_NUM, 0);
			CyDelay(100);
			Cy_GPIO_Write(LED_B_PORT, LED_B_NUM, 1);
			CyDelay(100);
		}
	}

	/* main loop */
	for(;;)
	{
		/* Blue LED on while sending*/
		Cy_GPIO_Write(LED_B_PORT, LED_B_NUM, 1);

		/* Compose a message to send */
        uint8_t j=0;
        TXbuffer[j++] = 0x48; /* H */
		TXbuffer[j++] = 0x45; /* E */
		TXbuffer[j++] = 0x4c; /* L */
		TXbuffer[j++] = 0x4c; /* L */
		TXbuffer[j++] = 0x4f; /* O */
		TXbuffer[j++] = 0x20; /*   */
		TXbuffer[j++] = 0x57; /* W */
		TXbuffer[j++] = 0x4f; /* O */
		TXbuffer[j++] = 0x52; /* R */
		TXbuffer[j++] = 0x4c; /* L */
		TXbuffer[j++] = 0x44; /* D */

		/* Send message over LoRaWAN */
        coreStatus = LoRaWAN_Send(TXbuffer, j, M4_WaitDeepSleep);

		/* Turn led off before sleep */
		Cy_GPIO_Write(LED_B_PORT, LED_B_NUM, 0);

		/* Sleep before sending next message, wake up with a button as well */
		DeviceSleep(&sleepConfig);
	}
}

