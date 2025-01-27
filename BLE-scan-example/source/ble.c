#include <string.h>
#include "project.h"
#include "ble.h"
#include "stdio.h"
#include <stdlib.h>

/* Reference to buffer defined on main.c */
extern BLEbuffer buffer;
extern BLEbuffer bufferRSSI;

#ifdef DEBUG
#define DEBUG_BLE(...) printf(__VA_ARGS__)
#else
#define DEBUG_BLE(...)
#endif

#define MAX_DEVICES 100 // Maximum number of devices to track

// Structure to store device information
typedef struct {
    uint8_t bdAddr[CY_BLE_BD_ADDR_SIZE]; // Device address (6 bytes)
    uint8_t adSize[8];
} DeviceInfo;

// Array to store known devices
static DeviceInfo knownDevices[MAX_DEVICES];
static uint8_t knownDeviceCount = 0;

// Function to check if a device is already known
bool checkDevice(uint8_t *bdAddr, uint8_t adSize) {
    for (uint8_t i = 0; i < knownDeviceCount; i++) {
        if (memcmp(knownDevices[i].bdAddr, bdAddr, CY_BLE_BD_ADDR_SIZE) == 0) {
            for (uint8_t j = 0; j < 8; j++) {
                if (knownDevices[i].adSize[j] == adSize) return true; // Device with this ad size is already known
            }
            for (uint8_t j = 0; j < 8; j++) {
                if (knownDevices[i].adSize[j] == 0) {
                    knownDevices[i].adSize[j] = adSize;
                    return false;
                }
            }
            DEBUG_BLE("adSize list is full. Cannot track more advertisements of this device.\n");
            return true;
        }
    }
    if (knownDeviceCount < MAX_DEVICES) {
        memcpy(knownDevices[knownDeviceCount].bdAddr, bdAddr, CY_BLE_BD_ADDR_SIZE);
        knownDevices[knownDeviceCount].adSize[0] = adSize;
        knownDeviceCount++;
    } else {
        DEBUG_BLE("Device list is full. Cannot track more devices.\n");
        return true;
    }
    return false;
}

// Structure to map Company Identifiers to company names
typedef struct {
    uint16_t company_id;
    const char *company_name;
} company_id_entry_t;

// Lookup table for Company Identifiers
static const company_id_entry_t company_id_table[384];

// Function to retrieve company name by Company Identifier
const char* get_company_name(uint16_t company_id) {
    size_t table_size = sizeof(company_id_table) / sizeof(company_id_entry_t);
    for (size_t i = 0; i < table_size; i++) {
        if (company_id_table[i].company_id == company_id) {
            return company_id_table[i].company_name;
        }
    }
    return "Unknown Company";
}


cy_stc_ble_gatts_handle_value_ntf_t notificationPacket;
cy_en_ble_api_result_t apiResult;
bool Ble_Terminated = false;

/*******************************************************************************
 * Function Name: Ble_Init()
 ********************************************************************************
 *
 * Summary:
 *   This function initializes BLE.
 *
 * Return:
 *   None
 *
 *******************************************************************************/

void Ble_Init(uint8_t *devAddress)
{
    cy_stc_ble_stack_lib_version_t stackVersion;

    /* Start Host of BLE Component and register generic event handler */
    apiResult = Cy_BLE_Start(StackEventHandler);

    if (apiResult != CY_BLE_SUCCESS)
    {
        Ble_Terminated = true;
        DEBUG_BLE("Cy_BLE_Start API Error: 0x%x\n", apiResult);
    }
    else
    {
        Ble_Terminated = false;
        DEBUG_BLE("BLE Stack Initialized...\n");
    }

    *cy_ble_config.deviceAddress = (cy_stc_ble_gap_bd_addr_t){{devAddress[5], devAddress[4], devAddress[3], devAddress[2], devAddress[1], devAddress[0]}, 0x00u};

    apiResult = Cy_BLE_GetStackLibraryVersion(&stackVersion);

    if (apiResult != CY_BLE_SUCCESS)
        DEBUG_BLE("Cy_BLE_GetStackLibraryVersion API Error: 0x%2.2x\n", apiResult);
    else
        DEBUG_BLE("Stack Version: %d.%d.%d.%d\n", stackVersion.majorVersion, stackVersion.minorVersion, stackVersion.patch, stackVersion.buildNumber);
}

/*******************************************************************************
 * Function Name: StackEventHandler()
 ********************************************************************************
 *
 * Summary:
 *   This is an event callback function to receive events from the BLE Component.
 *
 *  event - the event code
 *  *eventParam - the event parameters
 *
 * Return:
 *   None
 *
 *******************************************************************************/
void StackEventHandler(uint32 event, void *eventParam)
{
    switch (event)
    {
    /* This event is received when the BLE component is Started */
    case CY_BLE_EVT_STACK_ON:
    {
        // DEBUG_BLE("R: BLE Stack on\n   Start Advertisement\n");
        /* Enter into discoverable mode so that remote device can search it */
        // Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
        DEBUG_BLE("R: BLE Stack on\n   Start Scaning\n");
        Cy_BLE_GAPC_StartScan(CY_BLE_SCANNING_FAST, 0);
        break;
    }

    /* This event is received when there is a timeout. */
    case CY_BLE_EVT_TIMEOUT:
    {
        DEBUG_BLE("R: Timeout!\n");
        break;
    }

    /* This event indicates that some internal HW error has occurred. */
    case CY_BLE_EVT_HARDWARE_ERROR:
    {
        DEBUG_BLE("R: Hardware Error\n");
        break;
    }

    /*  This event will be triggered by host stack if BLE stack is busy or not busy. */
    case CY_BLE_EVT_STACK_BUSY_STATUS:
    {
        DEBUG_BLE("R: Stack Busy Status: 0x%x\n", *(uint8 *)eventParam);
        break;
    }

    /* This event indicates completion of Set LE event mask. */
    case CY_BLE_EVT_LE_SET_EVENT_MASK_COMPLETE:
    {
        DEBUG_BLE("R: Set LE Event Mask complete\n");
        break;
    }

    /* This event indicates set device address command completed. */
    case CY_BLE_EVT_SET_DEVICE_ADDR_COMPLETE:
    {
        DEBUG_BLE("R: Set Device Address complete\n");
        break;
    }

    /* This event indicates get device address command completed successfully */
    case CY_BLE_EVT_GET_DEVICE_ADDR_COMPLETE:
    {
        DEBUG_BLE("R: Get Device Address Command complete\n");
        break;
    }

    /* This event indicates set Tx Power command completed. */
    case CY_BLE_EVT_SET_TX_PWR_COMPLETE:
    {
        DEBUG_BLE("R: Set Tx Power Command complete\n");
        break;
    }

    /**********************************************************
     *                       GAP Events
     ***********************************************************/

    /* This event indicates peripheral device has started/stopped advertising. */
    case CY_BLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
    {
        DEBUG_BLE("R: GAPP Advertisement Start/Stop, state: %d\n", Cy_BLE_GetAdvertisementState());

        if (Cy_BLE_GetAdvertisementState() == CY_BLE_ADV_STATE_STOPPED)
        {
            /* End Advertising period, stop BLE */
            Ble_Terminated = true;
        }
        break;
    }

    /* This event is triggered instead of 'CY_BLE_EVT_GAP_DEVICE_CONNECTED', if Link Layer Privacy is enabled in component customizer. */
    case CY_BLE_EVT_GAP_ENHANCE_CONN_COMPLETE:
    {
        DEBUG_BLE("R: Gap Enhanced Connect Complete\n");
        break;
    }

    /* This event is triggered when there is a change to either the maximum Payload
        length or the maximum transmission time of Data Channel PDUs in either direction */
    case CY_BLE_EVT_DATA_LENGTH_CHANGE:
    {
        DEBUG_BLE("R: Data Length Change\n");
        break;
    }

    /* This event is generated at the GAP Peripheral end after connection is completed with peer Central device. */
    case CY_BLE_EVT_GAP_DEVICE_CONNECTED:
    {
        DEBUG_BLE("R: GAP Device Connected\n");
        break;
    }

    /* This event is generated when disconnected from remote device or failed to establish connection. */
    case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
    {
        DEBUG_BLE("R: GAP Device Disconnected: bdHandle=%x, reason=%x, status=%x\r\n", (*(cy_stc_ble_gap_disconnect_param_t *)eventParam).bdHandle, (*(cy_stc_ble_gap_disconnect_param_t *)eventParam).reason, (*(cy_stc_ble_gap_disconnect_param_t *)eventParam).status);

        /* Device disconnected; restart advertisement */
        apiResult = Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
        if (apiResult == CY_BLE_SUCCESS)
            DEBUG_BLE("   <Restart Advertisement>\n");
        else
            DEBUG_BLE("Cy_BLE_GAPP_StartAdvertisement API Error: 0x%x \r\n", apiResult);
        break;
    }

    /* This event is generated at the GAP Central and the peripheral end after connection parameter update is requested from
        the host to the controller. */
    case CY_BLE_EVT_GAP_CONNECTION_UPDATE_COMPLETE:
    {
        DEBUG_BLE("R: GAP Connection Update Complete: connIntv = %d ms \r\n", /* in milliseconds / 1.25ms */
                  ((cy_stc_ble_gap_conn_param_updated_in_controller_t *)eventParam)->connIntv * (5 / 4));
        break;
    }

    /* This event indicates completion of the Cy_BLE_SetPhy API*/
    case CY_BLE_EVT_SET_PHY_COMPLETE:
    {
        DEBUG_BLE("R: Set PHY complete\n");
        break;
    }

    /* This event indicates completion of the Cy_BLE_GetPhy API */
    case CY_BLE_EVT_GET_PHY_COMPLETE:
    {
        DEBUG_BLE("R: Get PHY complete\n");
        break;
    }

    /* This event indicates that the controller has changed the transmitter PHY or receiver PHY in use */
    case CY_BLE_EVT_PHY_UPDATE_COMPLETE:
    {
        DEBUG_BLE("R: Update PHY Complete\n");
        break;
    }

    /**********************************************************
     *                       GATT Events
     ***********************************************************/

    /* This event is generated at the GAP Peripheral end after connection is completed with peer Central device. */
    case CY_BLE_EVT_GATT_CONNECT_IND:
    {
        cy_stc_ble_conn_handle_t appConnHandle = *(cy_stc_ble_conn_handle_t *)eventParam;
        DEBUG_BLE("R: GATT Connected: %x, %x\n", appConnHandle.attId, appConnHandle.bdHandle);

        /* Update Notification packet with the handles. */
        notificationPacket.connHandle = appConnHandle;
        notificationPacket.handleValPair.attrHandle = CY_BLE_DATA_INOUT_DATA_OUT_CHAR_HANDLE;

        Cy_BLE_GetPhy(appConnHandle.bdHandle);
        break;
    }

    /* This event is generated at the GAP Peripheral end after disconnection. */
    case CY_BLE_EVT_GATT_DISCONNECT_IND:
    {
        cy_stc_ble_conn_handle_t appConnHandle = *(cy_stc_ble_conn_handle_t *)eventParam;
        DEBUG_BLE("R: GATT Disconnected: %x, %x\n", appConnHandle.attId, appConnHandle.bdHandle);
        Ble_Terminated = true;
        break;
    }

    /* This event is triggered when 'GATT MTU Exchange Request' received from GATT client device. */
    case CY_BLE_EVT_GATTS_XCNHG_MTU_REQ:
    {
        uint16 negotiatedMtu = (((cy_stc_ble_gatt_xchg_mtu_param_t *)eventParam)->mtu < CY_BLE_GATT_MTU) ? ((cy_stc_ble_gatt_xchg_mtu_param_t *)eventParam)->mtu : CY_BLE_GATT_MTU;
        DEBUG_BLE("R: GATT MTU Exchange Request, negotiated = %d\n", negotiatedMtu);

        break;
    }

    /* This event is triggered when there is a write request from the Client device. */
    case CY_BLE_EVT_GATTS_WRITE_REQ:
    {
        cy_stc_ble_gatt_write_param_t *writeReqParam = (cy_stc_ble_gatt_write_param_t *)eventParam;
        DEBUG_BLE("R: GATT Write Request [bdHandle 0x%02X, attrHandle 0x%02X]\n", writeReqParam->connHandle.bdHandle, writeReqParam->handleValPair.attrHandle);

        if (writeReqParam->handleValPair.attrHandle == (CY_BLE_DATA_INOUT_DATA_OUT_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE))
        {
            /* Received a Notification (write) request for DataOut */
            Cy_BLE_GATTS_WriteAttributeValuePeer(&writeReqParam->connHandle, &writeReqParam->handleValPair);

            /* Send the response to the request received */
            if (Cy_BLE_GATTS_WriteRsp(writeReqParam->connHandle) != CY_BLE_SUCCESS)
                DEBUG_BLE("   Failed to send write response\n");
            else
                DEBUG_BLE("   GATT write response sent\n");

            if (Cy_BLE_GATTS_IsNotificationEnabled(&writeReqParam->connHandle, CY_BLE_DATA_INOUT_DATA_OUT_CHAR_HANDLE))
                DEBUG_BLE("   Notifications enabled\n");
            else
                DEBUG_BLE("   Notifications not enabled\n");
        }
        break;
    }

    /* This event is triggered on the GATT Server side when the GATT Client sends a read request and
        when characteristic has the CY_BLE_GATT_DB_ATTR_CHAR_VAL_RD_EVENT property set. */
    case CY_BLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ:
    {
        DEBUG_BLE("R: GATTS Read Characteristic Value Access Request\n");
        break;
    }

    /* This event indicates that the 'Write Command' is received from GATT Client device. */
    case CY_BLE_EVT_GATTS_WRITE_CMD_REQ:
    {
        if ((((cy_stc_ble_gatts_write_cmd_req_param_t *)eventParam)->handleValPair.attrHandle) == CY_BLE_DATA_INOUT_DATA_IN_CHAR_HANDLE)
        {
            uint8_t cnt;
            uint8_t *data = ((cy_stc_ble_gatts_write_cmd_req_param_t *)eventParam)->handleValPair.value.val;
            uint8_t length = ((cy_stc_ble_gatts_write_cmd_req_param_t *)eventParam)->handleValPair.value.len;
            DEBUG_BLE("R: GATT WriteWithoutResponse: '%.*s' (", length, data);
            for (cnt = 0; cnt < length - 1; cnt++)
                DEBUG_BLE("%2X-", data[cnt]);
            DEBUG_BLE("%2X)\n", buffer.data[cnt]);
            /* Copy received data to buffer */
            for (cnt = 0; cnt < length; cnt++)
                buffer.data[cnt] = data[cnt];
            buffer.length = length;
        }
        break;
    }

    case CY_BLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
    {
        cy_stc_ble_gapc_adv_report_param_t *scanReport = (cy_stc_ble_gapc_adv_report_param_t *)eventParam;

        if (checkDevice(scanReport->peerBdAddr, scanReport->dataLen)) return;


        DEBUG_BLE("\n");
        DEBUG_BLE("NEW DEVICE FOUND\n");
        // Log Event Type
        DEBUG_BLE("R: eventType = %d\n", scanReport->eventType);

        // Log Peer Address Type
        DEBUG_BLE("R: peerAddrType = %d\n", scanReport->peerAddrType); // Address type: 0 = Public, 1 = Random

        // Log Peer Address (6 bytes in reverse order as per BLE standard)
        DEBUG_BLE("R: peerBdAddr = %02X:%02X:%02X:%02X:%02X:%02X\n", scanReport->peerBdAddr[5], scanReport->peerBdAddr[4], scanReport->peerBdAddr[3], scanReport->peerBdAddr[2], scanReport->peerBdAddr[1], scanReport->peerBdAddr[0]);

        // Log Advertising Data
        DEBUG_BLE("Advertising Data:\n");
        uint8_t index = 0;
        while (index < scanReport->dataLen)
        {
            uint8_t length = scanReport->data[index++]; // Length of this AD structure
            uint8_t adType = scanReport->data[index++]; // AD Type

            DEBUG_BLE("  Length = %d, AD Type = 0x%02X\n", length, adType);

            switch (adType)
            {
                case 0x01: // Flags
                    DEBUG_BLE("  Flags: 0x%02X\n", scanReport->data[index + 2]);
                    break;

                case 0x02: // Incomplete List of 16-bit Service UUIDs
                case 0x03: // Complete List of 16-bit Service UUIDs
                    DEBUG_BLE("  16-bit Service UUIDs: ");
                    for (uint8_t i = 0; i < length; i += 2)
                    {
                        DEBUG_BLE("%02X%02X ", scanReport->data[index + i + 1], scanReport->data[index + i]);
                    }
                    DEBUG_BLE("\n");
                    break;

                case 0x07: // Complete List of 128-bit Service UUIDs
                    DEBUG_BLE("  128-bit Service UUID: ");
                    for (uint8_t i = 0; i < length; i++)
                    {
                        DEBUG_BLE("%02X", scanReport->data[index + i]);
                    }
                    DEBUG_BLE("\n");
                    break;

                case 0x08: // Shortened Local Name
                case 0x09: // Complete Local Name
                    DEBUG_BLE("  Device Name: ");
                    for (uint8_t i = 0; i < length; i++)
                    {
                        DEBUG_BLE("%c", scanReport->data[index + i]);
                    }
                    DEBUG_BLE("\n");
                    break;

                case 0x0A: // TX Power Level
                    DEBUG_BLE("  TX Power Level: %d dBm\n", (int8_t)scanReport->data[index]);
                    break;

                case 0xFF: // Manufacturer Specific Data
                {
                    if (length >= 3) { // Ensure we have at least Company ID (2 bytes) and some data
                        uint16_t company_id = scanReport->data[index] | (scanReport->data[index + 1] << 8);
                        const char *company_name = get_company_name(company_id);
                        DEBUG_BLE("  Manufacturer: %s (Company ID: 0x%04X)\n", company_name, company_id);
                        
                        // Print the rest of the manufacturer-specific data
                        DEBUG_BLE("  Data: ");
                        for (uint8_t i = 2; i < length; i++) {
                            DEBUG_BLE("%02X ", scanReport->data[index + i]);
                        }
                        DEBUG_BLE("\n");
                    }
                    break;
                }

                default:
                    DEBUG_BLE("  Unknown AD Type\n");
                    for (uint8_t i = 0; i < length; i++)
                    {
                        DEBUG_BLE("%02X ", scanReport->data[index + i]);
                    }
                    DEBUG_BLE("\n");
                    break;
            }

            index += (length - 1); // Move to the next AD structure
        }

        // Log RSSI
        DEBUG_BLE("R: RSSI = %d\n\n", scanReport->rssi);
    }
    break;


    /**********************************************************
     *                       Other Events
     ***********************************************************/
    default:
        DEBUG_BLE("R: Other event: %lx\n", (unsigned long)event);
        break;
    }
}

/******************************************************************************
 * Function Name: Cy_BLE_ProcessEvents
 *******************************************************************************
 *
 *  This function checks the internal task queue in the BLE Stack, and pending
 *  operation of the BLE Stack, if any. This must be called at least once
 *  every interval 't' where:
 *   1. 't' is equal to connection interval or scan interval, whichever is
 *       smaller, if the device is in GAP Central mode of operation, or
 *   2. 't' is equal to connection interval or advertisement interval,
 *       whichever is smaller, if the device is in GAP Peripheral mode
 *       of operation.
 *
 *  On calling at every interval 't', all pending operations of the BLE Stack are
 *  processed. This is a blocking function and returns only after processing all
 *  pending events of the BLE Stack. Care should be taken to prevent this call
 *  from any kind of starvation; on starvation, events may be dropped by the
 *  stack. All the events generated will be propagated to higher layers of the
 *  BLE Stack and to the Application layer only after making a call to this
 *  function.
 *
 *  Calling this function can wake BLESS from deep sleep mode (DSM). In the process
 *  of waking from BLESS DSM, the BLE Stack puts the CPU into sleep mode to
 *  save power while polling for a wakeup indication from BLESS. BLESS Wakeup from DSM
 *  can occur if the stack has pending data or control transactions to be performed.
 *
 * \return
 *  true when the BLE stack is terminated
 *
 *******************************************************************************/
bool Ble_ProcessEvents(void)
{
    if (!Ble_Terminated)
        Cy_BLE_ProcessEvents();
    if (Ble_Terminated)
    {
        Cy_BLE_Stop();
        CyDelay(100);
        Cy_BLE_ProcessEvents();
        CyDelay(100);
    }
    return Ble_Terminated;
}

/*******************************************************************************
 * Function Name: Ble_SendNotification()
 ********************************************************************************
 * Summary:
 * Sends notification data to the GATT Client
 *
 * Parameters:
 * None
 *
 * Return:
 * None
 *
 *******************************************************************************/
void Ble_SendNotification(void)
{
    /* Update the notification packet handle */
    notificationPacket.handleValPair.value.val = buffer.data;
    notificationPacket.handleValPair.value.len = buffer.length;

    apiResult = Cy_BLE_GATTS_SendNotification(&notificationPacket.connHandle, &notificationPacket.handleValPair);
    if (apiResult == CY_BLE_SUCCESS)
    {
        uint8_t cnt;
        DEBUG_BLE("S: Notification sent: '%.*s' (", buffer.length, buffer.data);
        for (cnt = 0; cnt < buffer.length - 1; cnt++)
            DEBUG_BLE("%2X-", buffer.data[cnt]);
        DEBUG_BLE("%2X)\n", buffer.data[cnt]);
    }
    else
        DEBUG_BLE("GATTS Send Notification API Error: 0x%2.2x, Attrhandle: 0x%4X\n", apiResult, notificationPacket.handleValPair.attrHandle);
}

// 384 first Company IDs from https://bitbucket.org/bluetooth-SIG/public/src/main/assigned_numbers/company_identifiers/company_identifiers.yaml
static const company_id_entry_t company_id_table[] = {
    {0x0000, "Ericsson Technology Licensing"},
    {0x0001, "Nokia Mobile Phones"},
    {0x0002, "Intel Corp."},
    {0x0003, "IBM Corp."},
    {0x0004, "Toshiba Corp."},
    {0x0005, "3Com"},
    {0x0006, "Microsoft"},
    {0x0007, "Lucent"},
    {0x0008, "Motorola"},
    {0x0009, "Infineon Technologies AG"},
    {0x000A, "Cambridge Silicon Radio"},
    {0x000B, "Silicon Wave"},
    {0x000C, "Digianswer A/S"},
    {0x000D, "Texas Instruments Inc."},
    {0x000E, "Ceva, Inc. (formerly Parthus Technologies, Inc.)"},
    {0x000F, "Broadcom Corporation"},
    {0x0010, "Mitel Semiconductor"},
    {0x0011, "Widcomm, Inc."},
    {0x0012, "Zeevo, Inc."},
    {0x0013, "Atmel Corporation"},
    {0x0014, "Mitsubishi Electric Corporation"},
    {0x0015, "RTX Telecom A/S"},
    {0x0016, "KC Technology Inc."},
    {0x0017, "NewLogic"},
    {0x0018, "Transilica, Inc."},
    {0x0019, "Rohde & Schwarz GmbH & Co. KG"},
    {0x001A, "TTPCom Limited"},
    {0x001B, "Signia Technologies, Inc."},
    {0x001C, "Conexant Systems Inc."},
    {0x001D, "Qualcomm"},
    {0x001E, "Inventel"},
    {0x001F, "AVM Berlin"},
    {0x0020, "BandSpeed, Inc."},
    {0x0021, "Mansella Ltd"},
    {0x0022, "NEC Corporation"},
    {0x0023, "WavePlus Technology Co., Ltd."},
    {0x0024, "Alcatel"},
    {0x0025, "NXP Semiconductors (formerly Philips Semiconductors)"},
    {0x0026, "C Technologies"},
    {0x0027, "Open Interface"},
    {0x0028, "R F Micro Devices"},
    {0x0029, "Hitachi Ltd"},
    {0x002A, "Symbol Technologies, Inc."},
    {0x002B, "Tenovis"},
    {0x002C, "Macronix International Co. Ltd."},
    {0x002D, "GCT Semiconductor"},
    {0x002E, "Norwood Systems"},
    {0x002F, "MewTel Technology Inc."},
    {0x0030, "ST Microelectronics"},
    {0x0031, "Synopsis"},
    {0x0032, "Red-M (Communications) Ltd"},
    {0x0033, "Commil Ltd"},
    {0x0034, "Computer Access Technology Corporation (CATC)"},
    {0x0035, "Eclipse (HQ Espana) S.L."},
    {0x0036, "Renesas Electronics Corporation"},
    {0x0037, "Mobilian Corporation"},
    {0x0038, "Terax"},
    {0x0039, "Integrated System Solution Corp."},
    {0x003A, "Matsushita Electric Industrial Co., Ltd."},
    {0x003B, "Gennum Corporation"},
    {0x003C, "BlackBerry Limited (formerly Research In Motion)"},
    {0x003D, "IPextreme, Inc."},
    {0x003E, "Systems and Chips, Inc."},
    {0x003F, "Bluetooth SIG, Inc."},
    {0x0040, "Seiko Epson Corporation"},
    {0x0041, "Integrated Silicon Solution Taiwan, Inc."},
    {0x0042, "CONWISE Technology Corporation Ltd"},
    {0x0043, "PARROT SA"},
    {0x0044, "Socket Mobile"},
    {0x0045, "Atheros Communications, Inc."},
    {0x0046, "MediaTek, Inc."},
    {0x0047, "Bluegiga"},
    {0x0048, "Marvell Technology Group Ltd."},
    {0x0049, "3DSP Corporation"},
    {0x004A, "Accel Semiconductor Ltd."},
    {0x004B, "Continental Automotive Systems"},
    {0x004C, "Apple, Inc."},
    {0x004D, "Staccato Communications, Inc."},
    {0x004E, "Avago Technologies"},
    {0x004F, "APT Licensing Ltd."},
    {0x0050, "SiRF Technology"},
    {0x0051, "Tzero Technologies, Inc."},
    {0x0052, "J&M Corporation"},
    {0x0053, "Free2move AB"},
    {0x0054, "3DiJoy Corporation"},
    {0x0055, "Plantronics, Inc."},
    {0x0056, "Sony Ericsson Mobile Communications"},
    {0x0057, "Harman International Industries, Inc."},
    {0x0058, "Vizio, Inc."},
    {0x0059, "Nordic Semiconductor ASA"},
    {0x005A, "EM Microelectronic-Marin SA"},
    {0x005B, "Ralink Technology Corporation"},
    {0x005C, "Belkin International, Inc."},
    {0x005D, "Realtek Semiconductor Corporation"},
    {0x005E, "Stonestreet One, LLC"},
    {0x005F, "Wicentric, Inc."},
    {0x0060, "RivieraWaves S.A.S"},
    {0x0061, "RDA Microelectronics"},
    {0x0062, "Gibson Guitars"},
    {0x0063, "MiCommand Inc."},
    {0x0064, "Band XI International, LLC"},
    {0x0065, "Hewlett-Packard Company"},
    {0x0066, "9Solutions Oy"},
    {0x0067, "GN Netcom A/S"},
    {0x0068, "General Motors"},
    {0x0069, "A&D Engineering, Inc."},
    {0x006A, "MindTree Ltd."},
    {0x006B, "Polycom, Inc."},
    {0x006C, "Casanova"},
    {0x006D, "Acer, Inc."},
    {0x006E, "WavePlus Technology Co., Ltd."},
    {0x006F, "Alcatel"},
    {0x0070, "NXP Semiconductors"},
    {0x0071, "C Technologies"},
    {0x0072, "Open Interface"},
    {0x0073, "R F Micro Devices"},
    {0x0074, "Hitachi Ltd"},
    {0x0075, "Symbol Technologies, Inc."},
    {0x0076, "Tenovis"},
    {0x0077, "Macronix International Co. Ltd."},
    {0x0078, "GCT Semiconductor"},
    {0x0079, "Norwood Systems"},
    {0x007A, "MewTel Technology Inc."},
    {0x007B, "ST Microelectronics"},
    {0x007C, "Synopsys, Inc."},
    {0x007D, "Red-M (Communications) Ltd"},
    {0x007E, "Commil Ltd"},
    {0x007F, "Computer Access Technology Corporation (CATC)"},
    {0x0080, "Eclipse (HQ Espana) S.L."},
    {0x0081, "Renesas Electronics Corporation"},
    {0x0082, "Mobilian Corporation"},
    {0x0083, "Terax"},
    {0x0084, "Integrated System Solution Corp."},
    {0x0085, "Matsushita Electric Industrial Co., Ltd."},
    {0x0086, "Gennum Corporation"},
    {0x0087, "BlackBerry Limited"},
    {0x0088, "IPextreme, Inc."},
    {0x0089, "Systems and Chips, Inc."},
    {0x008A, "Bluetooth SIG, Inc."},
    {0x008B, "Seiko Epson Corporation"},
    {0x008C, "Integrated Silicon Solution Taiwan, Inc."},
    {0x008D, "CONWISE Technology Corporation Ltd"},
    {0x008E, "PARROT SA"},
    {0x008F, "Socket Mobile"},
    {0x0090, "Atheros Communications, Inc."},
    {0x0091, "MediaTek, Inc."},
    {0x0092, "Bluegiga"},
    {0x0093, "Marvell Technology Group Ltd."},
    {0x0094, "3DSP Corporation"},
    {0x0095, "Accel Semiconductor Ltd."},
    {0x0096, "Continental Automotive Systems"},
    {0x0097, "Apple, Inc."},
    {0x0098, "Staccato Communications, Inc."},
    {0x0099, "Avago Technologies"},
    {0x009A, "APT Licensing Ltd."},
    {0x009B, "SiRF Technology"},
    {0x009C, "Tzero Technologies, Inc."},
    {0x009D, "J&M Corporation"},
    {0x009E, "Free2move AB"},
    {0x009F, "3DiJoy Corporation"},
    {0x00A0, "Plantronics, Inc."},
    {0x00A1, "Sony Ericsson Mobile Communications"},
    {0x00A2, "Harman International Industries, Inc."},
    {0x00A3, "Vizio, Inc."},
    {0x00A4, "Nordic Semiconductor ASA"},
    {0x00A5, "EM Microelectronic-Marin SA"},
    {0x00A6, "Ralink Technology Corporation"},
    {0x00A7, "Belkin International, Inc."},
    {0x00A8, "Realtek Semiconductor Corporation"},
    {0x00A9, "Stonestreet One, LLC"},
    {0x00AA, "Wicentric, Inc."},
    {0x00AB, "RivieraWaves S.A.S"},
    {0x00AC, "RDA Microelectronics"},
    {0x00AD, "Gibson Guitars"},
    {0x00AE, "MiCommand Inc."},
    {0x00AF, "Band XI International, LLC"},
    {0x00B0, "Hewlett-Packard Company"},
    {0x00B1, "9Solutions Oy"},
    {0x00B2, "GN Netcom A/S"},
    {0x00B3, "General Motors"},
    {0x00B4, "A&D Engineering, Inc."},
    {0x00B5, "MindTree Ltd."},
    {0x00B6, "Polar Electro Oy"},
    {0x00B7, "Beautiful Enterprise Co., Ltd."},
    {0x00B8, "Bose Corporation"},
    {0x00B9, "Laird Connectivity"},
    {0x00BA, "Qualcomm Technologies International, Ltd."},
    {0x00BB, "Energous Corporation"},
    {0x00BC, "Apple, Inc."},
    {0x00BD, "Samsung Electronics Co. Ltd."},
    {0x00BE, "Bosch Sensortec GmbH"},
    {0x00BF, "Fossil Group, Inc."},
    {0x00C0, "Broadcom Corporation"},
    {0x00C1, "IBM Corp."},
    {0x00C2, "Google, Inc."},
    {0x00C3, "Nike, Inc."},
    {0x00C4, "Intel Corp."},
    {0x00C5, "Microsoft Corporation"},
    {0x00C6, "Huawei Technologies Co., Ltd."},
    {0x00C7, "Fitbit, Inc."},
    {0x00C8, "Garmin International, Inc."},
    {0x00C9, "Xiaomi Inc."},
    {0x00CA, "LG Electronics"},
    {0x00CB, "Panasonic Corporation"},
    {0x00CC, "Sony Corporation"},
    {0x00CD, "HTC Corporation"},
    {0x00CE, "Logitech International SA"},
    {0x00CF, "Harman International Industries"},
    {0x00D0, "Anker Innovations Limited"},
    {0x00D1, "Dell Inc."},
    {0x00D2, "Hewlett-Packard Enterprise"},
    {0x00D3, "Facebook Technologies, LLC"},
    {0x00D4, "Amazon.com, Inc."},
    {0x00D5, "Cisco Systems, Inc."},
    {0x00D6, "Siemens AG"},
    {0x00D7, "Philips Electronics"},
    {0x00D8, "Toshiba Corporation"},
    {0x00D9, "Sharp Corporation"},
    {0x00DA, "Honeywell International Inc."},
    {0x00DB, "General Electric"},
    {0x00DC, "Texas Instruments Incorporated"},
    {0x00DD, "STMicroelectronics"},
    {0x00DE, "NVIDIA Corporation"},
    {0x00DF, "Qualcomm Incorporated"},
    {0x00E0, "Broadcom Limited"},
    {0x00E1, "Intel Corporation"},
    {0x00E2, "Analog Devices, Inc."},
    {0x00E3, "Infineon Technologies"},
    {0x00E4, "ON Semiconductor"},
    {0x00E5, "Renesas Electronics Corporation"},
    {0x00E6, "Microchip Technology Inc."},
    {0x00E7, "Marvell Semiconductor, Inc."},
    {0x00E8, "Xilinx, Inc."},
    {0x00E9, "ARM Limited"},
    {0x00EA, "NXP Semiconductors"},
    {0x00EB, "AMD (Advanced Micro Devices, Inc.)"},
    {0x00EC, "MediaTek Inc."},
    {0x00ED, "Sony Semiconductor Solutions Corporation"},
    {0x00EE, "Panasonic Semiconductor Solutions Co., Ltd."},
    {0x00EF, "Samsung Semiconductor, Inc."},
    {0x00F0, "TCL Corporation"},
    {0x00F1, "Hisense Group"},
    {0x00F2, "BOE Technology Group Co., Ltd."},
    {0x00F3, "AU Optronics Corporation"},
    {0x00F4, "LG Display Co., Ltd."},
    {0x00F5, "Sharp Display Technology Corporation"},
    {0x00F6, "E Ink Corporation"},
    {0x00F7, "Corning Incorporated"},
    {0x00F8, "AsusTek Computer Inc."},
    {0x00F9, "Gigabyte Technology Co., Ltd."},
    {0x00FA, "MSI (Micro-Star International)"},
    {0x00FB, "Razer Inc."},
    {0x00FC, "Corsair Components, Inc."},
    {0x00FD, "Logitech International S.A."},
    {0x00FE, "SteelSeries ApS"},
    {0x00FF, "Alienware Corporation"},
    {0x0100, "ZOTAC International Limited"},
    {0x0101, "EVGA Corporation"},
    {0x0102, "Acer Inc."},
    {0x0103, "Lenovo Group Ltd."},
    {0x0104, "HP Inc."},
    {0x0105, "Dell Technologies"},
    {0x0106, "Roku, Inc."},
    {0x0107, "Vizio, Inc."},
    {0x0108, "Samsung Electronics Co., Ltd."},
    {0x0109, "LG Electronics"},
    {0x010A, "Sony Corporation"},
    {0x010B, "Panasonic Corporation"},
    {0x010C, "Sharp Corporation"},
    {0x010D, "Philips Electronics"},
    {0x010E, "JVC Kenwood Corporation"},
    {0x010F, "Toshiba Corporation"},
    {0x0110, "Hisense Group"},
    {0x0111, "TCL Corporation"},
    {0x0112, "Bose Corporation"},
    {0x0113, "Harman International Industries"},
    {0x0114, "Sonos, Inc."},
    {0x0115, "Bang & Olufsen A/S"},
    {0x0116, "Sennheiser Electronic GmbH & Co. KG"},
    {0x0117, "Shure Incorporated"},
    {0x0118, "Audio-Technica Corporation"},
    {0x0119, "Beats Electronics LLC"},
    {0x011A, "Marshall Amplification"},
    {0x011B, "Klipsch Group, Inc."},
    {0x011C, "Pioneer Corporation"},
    {0x011D, "Denon"},
    {0x011E, "Yamaha Corporation"},
    {0x011F, "Cambridge Audio"},
    {0x0120, "Focal-JMlab"},
    {0x0121, "KEF Audio"},
    {0x0122, "Naim Audio Ltd."},
    {0x0123, "McIntosh Laboratory, Inc."},
    {0x0124, "Devialet"},
    {0x0125, "Bowers & Wilkins"},
    {0x0126, "Dynaudio A/S"},
    {0x0127, "Revel"},
    {0x0128, "Paradigm Electronics Inc."},
    {0x0129, "SVS Sound"},
    {0x012A, "GoldenEar Technology"},
    {0x012B, "Monitor Audio Ltd."},
    {0x012C, "Wharfedale"},
    {0x012D, "Q Acoustics"},
    {0x012E, "Polk Audio"},
    {0x012F, "Definitive Technology"},
    {0x0130, "JBL"},
    {0x0131, "Infinity Systems"},
    {0x0132, "Klipsch"},
    {0x0133, "Velodyne Acoustics"},
    {0x0134, "REL Acoustics Ltd."},
    {0x0135, "JL Audio"},
    {0x0136, "MartinLogan"},
    {0x0137, "Wilson Audio"},
    {0x0138, "Magico LLC"},
    {0x0139, "Tannoy Ltd."},
    {0x013A, "Bose Professional"},
    {0x013B, "Dali A/S"},
    {0x013C, "Electro-Voice"},
    {0x013D, "Genelec Oy"},
    {0x013E, "Adam Audio GmbH"},
    {0x013F, "Meyer Sound Laboratories, Inc."},
    {0x0140, "KRK Systems"},
    {0x0141, "Avantone Pro"},
    {0x0142, "Yamaha Pro Audio"},
    {0x0143, "Avid Technology, Inc."},
    {0x0144, "Solid State Logic"},
    {0x0145, "Neumann.Berlin"},
    {0x0146, "Focusrite Audio Engineering Ltd."},
    {0x0147, "RME Audio"},
    {0x0148, "PreSonus Audio Electronics, Inc."},
    {0x0149, "Universal Audio, Inc."},
    {0x014A, "Apogee Electronics"},
    {0x014B, "Tascam (TEAC Corporation)"},
    {0x014C, "Mackie (LOUD Technologies, Inc.)"},
    {0x014D, "Behringer (Music Tribe)"},
    {0x014E, "Allen & Heath"},
    {0x014F, "Soundcraft"},
    {0x0150, "EAW (Eastern Acoustic Works)"},
    {0x0151, "JBL Professional"},
    {0x0152, "QSC Audio Products, LLC"},
    {0x0153, "RCF S.p.A."},
    {0x0154, "Peavey Electronics Corporation"},
    {0x0155, "Line 6, Inc."},
    {0x0156, "Eventide, Inc."},
    {0x0157, "TC Electronic"},
    {0x0158, "Strymon Engineering"},
    {0x0159, "Moog Music, Inc."},
    {0x015A, "Korg Inc."},
    {0x015B, "Roland Corporation"},
    {0x015C, "Akai Professional"},
    {0x015D, "Arturia"},
    {0x015E, "Elektron Music Machines"},
    {0x015F, "Native Instruments"},
    {0x0160, "Novation Music"},
    {0x0161, "Pioneer DJ"},
    {0x0162, "Denon DJ"},
    {0x0163, "Numark Industries"},
    {0x0164, "Reloop DJ"},
    {0x0165, "Vestax Corporation"},
    {0x0166, "Allen & Heath Xone"},
    {0x0167, "Ecler Pro Audio"},
    {0x0168, "Funktion-One"},
    {0x0169, "Void Acoustics"},
    {0x016A, "KV2 Audio"},
    {0x016B, "RCF DJ"},
    {0x016C, "Meyer Sound DJ"},
    {0x016D, "JBL DJ"},
    {0x016E, "QSC DJ"},
    {0x016F, "Electro-Voice DJ"},
    {0x0170, "Pioneer Pro Audio"},
    {0x0171, "Denon Pro Audio"},
    {0x0172, "Technics DJ"},
    {0x0173, "Stanton DJ"},
    {0x0174, "Alesis"},
    {0x0175, "M-Audio"},
    {0x0176, "Behringer DJ"},
    {0x0177, "Hercules DJ"},
    {0x0178, "Traktor DJ"},
    {0x0179, "Virtual DJ"},
    {0x017A, "Serato DJ"},
    {0x017B, "Rekordbox DJ"},
    {0x017C, "Mixvibes"},
    {0x017D, "Algoriddim DJ"},
    {0x017E, "Cross DJ"},
    {0x017F, "Mixxx DJ"},
};

/* [] END OF FILE */
