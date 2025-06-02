#pragma once

#include <string.h>
#include "stdio.h"

/***************************************
*       Function Prototypes
***************************************/

void Ble_Init(uint8_t * devAddress);
void Ble_SendNotification(void);
bool Ble_ProcessEvents(void);
void StackEventHandler(uint32 event, void* eventParam);

#ifdef DEBUG
#define DEBUG_BLE(...) printf(__VA_ARGS__)
#else
#define DEBUG_BLE(...)
#endif

typedef enum
{
    cmd_idle,
    dev_uart_send = 0x01,
    dev_lorawan_join = 0x10,
    dev_lorawan_send = 0x11,

    host_message  = 0x81,
    host_error  = 0x82,
    host_beep = 0x83
} BLEcommand;

typedef struct 
{
    union
    {
        uint8_t rawbytes[243];
        struct
        {
            uint8_t command;
            uint8_t data[242];
        };
    };
    uint8_t length;
} BLEbuffer;