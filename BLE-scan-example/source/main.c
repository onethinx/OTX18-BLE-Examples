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

coreConfiguration_t coreConfig = {
	.Join =
		{
			.KeysPtr = &TTN_OTAAkeys,
			.DataRate = DR_AUTO,
			.Power = PWR_MAX,
			.MAXTries = 100,
			.SubBand_1st = EU_SUB_BANDS_DEFAULT,
			.SubBand_2nd = EU_SUB_BANDS_DEFAULT},
	.TX =
		{
			.Confirmed = false,
			.DataRate = DR_0,
			.Power = PWR_MAX,
			.FPort = 1},
	.RX =
		{
			.Boost = true},
	.System =
		{
			.Idle =
				{
					.Mode = M0_DeepSleep,
					.BleEcoON = false,
					.DebugON = true,
				}}};

sleepConfig_t sleepConfig =
	{
		.sleepMode = modeDeepSleep,
		.BleEcoON = false,
		.DebugON = true,
		.sleepCores = coresBoth,
		.wakeUpPin = wakeUpPinHigh(true),
		.wakeUpTime = wakeUpDelay(0, 0, 0, 30), // day, hour, minute, second
};

/* OnethinxCore uses the following structures and variables, which can be defined globally */
coreStatus_t coreStatus;
coreInfo_t coreInfo;

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
void DeviceSleep(sleepConfig_t *sleepConfig)
{
	uint32_t RX_HSIOM = Cy_GPIO_GetHSIOM(UART_rx_PORT, UART_rx_NUM);
	uint32_t TX_HSIOM = Cy_GPIO_GetHSIOM(UART_tx_PORT, UART_tx_NUM);
	Cy_GPIO_Pin_FastInit(UART_rx_PORT, UART_rx_NUM, CY_GPIO_DM_HIGHZ, 0, HSIOM_SEL_GPIO);
	Cy_GPIO_Pin_FastInit(UART_tx_PORT, UART_tx_NUM, CY_GPIO_DM_HIGHZ, 0, HSIOM_SEL_GPIO);
	Cy_GPIO_Write(LED_B_PORT, LED_B_NUM, 0);
	Cy_GPIO_Write(LED_R_PORT, LED_R_NUM, 0);
	Cy_SCB_UART_Disable(UART_HW, &UART_context);
	LoRaWAN_Sleep(sleepConfig);
	UART_Start();
	Cy_GPIO_Pin_FastInit(UART_rx_PORT, UART_rx_NUM, CY_GPIO_DM_HIGHZ, 1, RX_HSIOM);
	Cy_GPIO_Pin_FastInit(UART_tx_PORT, UART_tx_NUM, CY_GPIO_DM_STRONG_IN_OFF, 1, TX_HSIOM);
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
	if (Cy_BLE_GetAdvertisementState() == CY_BLE_ADV_STATE_ADVERTISING)
	{
		/* In advertising state, turn off disconnect indication LED */
		Cy_GPIO_Write(LED_R_PORT, LED_R_NUM, 0);

		/* Blink advertising indication LED */
		Cy_GPIO_Inv(LED_B_PORT, LED_B_NUM);
	}
	else if (Cy_BLE_GetNumOfActiveConn() == 0u)
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
	 Cy_SCB_UART_PutArrayBlocking(UART_HW, ptr, len);
	 return 0;
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
BLEbuffer bufferRSSI = {{127}, 1};

int main(void)
{
	/* enable global interrupts */
	__enable_irq();

	/* Start UART (115200 baud, RX on P9_0, TX on P9_1) */
	UART_Start();

	/* initialize radio with parameters in coreConfig */
	coreStatus = LoRaWAN_Init(&coreConfig);

	/* Check Onethinx Core info */
	LoRaWAN_GetInfo(&coreInfo);

	uint8_t devAddress[] = {0x00, 0xA0u, 0x50, 0x00, 0x01, 0x5A};
	/* Initialize BLE */
	Ble_Init((uint8_t *)&devAddress);

	for (;;)
	{
		/* Allow BLE stack to process pending events */
		if (Ble_ProcessEvents())
			break;

		/* Send notification at button press */
		if (Cy_GPIO_Read(BUTTON_PORT, BUTTON_NUM))
		{
			if (++btcnt == 10)
			{
				// Ble_SendNotification();
				Cy_BLE_Stop();
				CyDelay(100);
				Cy_BLE_ProcessEvents();
				CyDelay(100);
				break;
			}
		}
		else
			btcnt = 0;

		/* Check for data in UART RX buffer */
		while (Cy_SCB_UART_GetNumInRxFifo(UART_HW))
		{
			if (RXcnt < 240)
				buffer.data[RXcnt++] = Cy_SCB_UART_Get(UART_HW);
			RXtimeout = 1000;
		}
		/* Send notification with UART data after a small timeout */
		if (RXtimeout && RXtimeout-- == 1)
		{
			buffer.length = RXcnt;
			Ble_SendNotification();
			RXcnt = 0;
		}

		/* Update Leds once in a while */
		if ((++LEDdelay & 16383) == 1)
			UpdateLedState();
	}

	/* send join using parameters in coreConfig, blocks until either success or MAXtries */ coreStatus = LoRaWAN_Join(M4_NoWait);
	while ((coreStatus = LoRaWAN_GetStatus()).system.isBusy)
	{
		Cy_GPIO_Write(LED_R_PORT, LED_R_NUM, 1);
		Cy_GPIO_Write(LED_B_PORT, LED_B_NUM, 0);
		CyDelay(500);
		Cy_GPIO_Write(LED_R_PORT, LED_R_NUM, 0);
		Cy_GPIO_Write(LED_B_PORT, LED_B_NUM, 1);
		CyDelay(500);
	}

	/* check for successful join, flash Red LED if not joined */
	if (!coreStatus.mac.isJoined)
	{
		while (1)
		{
			Cy_GPIO_Write(LED_B_PORT, LED_B_NUM, 0);
			CyDelay(100);
			Cy_GPIO_Write(LED_B_PORT, LED_B_NUM, 1);
			CyDelay(100);
		}
	}

	/* main loop */
	for (;;)
	{
		/* Blue LED on while sending*/
		Cy_GPIO_Write(LED_B_PORT, LED_B_NUM, 1);

		/* Compose a message to send */
		/* Lowest RSSI scanned */
		uint8_t j = 0;
		TXbuffer[j++] = bufferRSSI.data[0];

		/* Send message over LoRaWAN */
		coreStatus = LoRaWAN_Send(TXbuffer, j, M4_WaitDeepSleep);

		/* Turn led off before sleep */
		Cy_GPIO_Write(LED_B_PORT, LED_B_NUM, 0);

		/* Sleep before sending next message, wake up with a button as well */
		DeviceSleep(&sleepConfig);
	}
}
