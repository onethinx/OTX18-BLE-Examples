#pragma once

/***************************************
*       Function Prototypes
***************************************/

void Ble_Init(uint8_t * devAddress);
void Ble_SendNotification(void);
bool Ble_ProcessEvents(void);
void StackEventHandler(uint32 event, void* eventParam);

typedef struct 
{
    uint8_t data[243];
    uint8_t length;
} BLEbuffer;