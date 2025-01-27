#include <string.h>
#include "project.h"
#include "ble.h"
#include "stdio.h"

/* Reference to buffer defined on main.c */
extern BLEbuffer buffer;

#ifdef DEBUG
#define DEBUG_BLE(...) printf(__VA_ARGS__)
#else
#define DEBUG_BLE(...)
#endif

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

void Ble_Init(uint8_t * devAddress)
{
    cy_stc_ble_stack_lib_version_t stackVersion;
    
    /* Start Host of BLE Component and register generic event handler */
    apiResult = Cy_BLE_Start(StackEventHandler);
    
    if(apiResult != CY_BLE_SUCCESS)
    {
        Ble_Terminated = true;
        DEBUG_BLE("Cy_BLE_Start API Error: 0x%x\n", apiResult);
    }
    else
    {
        Ble_Terminated = false;
        DEBUG_BLE("BLE Stack Initialized...\n");
    }

    *cy_ble_config.deviceAddress = (cy_stc_ble_gap_bd_addr_t) {{devAddress[5], devAddress[4], devAddress[3], devAddress[2], devAddress[1], devAddress[0]}, 0x00u };
    
    apiResult = Cy_BLE_GetStackLibraryVersion(&stackVersion);
    
    if(apiResult != CY_BLE_SUCCESS)
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
void StackEventHandler(uint32 event, void* eventParam)
{
    switch(event)
    {
        /* This event is received when the BLE component is Started */
        case CY_BLE_EVT_STACK_ON:
        {
            DEBUG_BLE("R: BLE Stack on\n   Start Advertisement\n");    
            /* Enter into discoverable mode so that remote device can search it */
            Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
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
            
            if(Cy_BLE_GetAdvertisementState() == CY_BLE_ADV_STATE_STOPPED)
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
            uint16 negotiatedMtu = (((cy_stc_ble_gatt_xchg_mtu_param_t *)eventParam)->mtu < CY_BLE_GATT_MTU) ?
                            ((cy_stc_ble_gatt_xchg_mtu_param_t *)eventParam)->mtu : CY_BLE_GATT_MTU;
            DEBUG_BLE("R: GATT MTU Exchange Request, negotiated = %d\n", negotiatedMtu);
            
            break;
        }

        /* This event is triggered when there is a write request from the Client device. */
        case CY_BLE_EVT_GATTS_WRITE_REQ:
        {
            cy_stc_ble_gatt_write_param_t *writeReqParam = (cy_stc_ble_gatt_write_param_t *)eventParam;
            DEBUG_BLE("R: GATT Write Request [bdHandle 0x%02X, attrHandle 0x%02X]\n", writeReqParam->connHandle.bdHandle, writeReqParam->handleValPair.attrHandle);

			if(writeReqParam->handleValPair.attrHandle == (CY_BLE_DATA_INOUT_DATA_OUT_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE))
            {
                /* Received a Notification (write) request for DataOut */
                Cy_BLE_GATTS_WriteAttributeValuePeer(&writeReqParam->connHandle, &writeReqParam->handleValPair);
                
                /* Send the response to the request received */
                if(Cy_BLE_GATTS_WriteRsp(writeReqParam->connHandle) != CY_BLE_SUCCESS)
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
            if( (((cy_stc_ble_gatts_write_cmd_req_param_t *)eventParam)->handleValPair.attrHandle) ==  CY_BLE_DATA_INOUT_DATA_IN_CHAR_HANDLE) 
            {
                uint8_t cnt;
                uint8_t * data = ((cy_stc_ble_gatts_write_cmd_req_param_t *)eventParam)->handleValPair.value.val;
                uint8_t length = ((cy_stc_ble_gatts_write_cmd_req_param_t *)eventParam)->handleValPair.value.len;
                DEBUG_BLE("R: GATT WriteWithoutResponse: '%.*s' (", length, data);
                for (cnt = 0; cnt < length - 1; cnt ++) DEBUG_BLE("%2X-", data[cnt]);
                DEBUG_BLE("%2X)\n", buffer.data[cnt]);
                /* Copy received data to buffer */
                for (cnt = 0; cnt < length; cnt++) buffer.data[cnt] = data[cnt];
                buffer.length = length;
            }
            break;
        }

        /**********************************************************
        *                       Other Events
        ***********************************************************/
        default:
            DEBUG_BLE("R: Other event: %lx\n", (unsigned long) event);
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
        for (cnt = 0; cnt < buffer.length - 1; cnt ++) DEBUG_BLE("%2X-", buffer.data[cnt]);
         DEBUG_BLE("%2X)\n", buffer.data[cnt]);
    }
    else
        DEBUG_BLE("GATTS Send Notification API Error: 0x%2.2x, Attrhandle: 0x%4X\n", apiResult, notificationPacket.handleValPair.attrHandle);
}

/*******************************************************************************
* Function Name: DisUpdateFirmWareRevision
********************************************************************************
*
* Summary:
*   Updates the Firmware Revision characteristic with BLE Stack version.
*
*******************************************************************************/
static void DisUpdateFirmWareRevision(void)
{
    cy_stc_ble_stack_lib_version_t stackVersion;
    uint8_t fwRev[9u] = "0.0.0.000";
    
    if(Cy_BLE_GetStackLibraryVersion(&stackVersion) == CY_BLE_SUCCESS)
    {
        /* Transform numbers to ASCII string */
        fwRev[0u] = stackVersion.majorVersion + '0'; 
        fwRev[2u] = stackVersion.minorVersion + '0';
        fwRev[4u] = stackVersion.patch + '0';
        fwRev[6u] = (stackVersion.buildNumber / 100u) + '0';
        stackVersion.buildNumber %= 100u; 
        fwRev[7u] = (stackVersion.buildNumber / 10u) + '0';
        fwRev[8u] = (stackVersion.buildNumber % 10u) + '0';
    }
    
    Cy_BLE_DISS_SetCharacteristicValue(CY_BLE_DIS_FIRMWARE_REV, sizeof(fwRev), fwRev);
}

/* [] END OF FILE */
