#pragma once

#include <dos.h>

#include "types.h"

#define ECB_InUse_UsedToTransferData 0xFF
#define ECB_InUse_UsedToReceiveData 0xFE
#define ECB_InUse_UsedByAES 0xFD
#define ECB_InUse_CompletedAndQueued 0xFB

#define ECB_CompletionCode_Receive_Success 0x00
#define ECB_CompletionCode_Receive_SocketWasntOpened 0xFF
#define ECB_CompletionCode_Receive_PacketOverflow 0xFD
#define ECB_CompletionCode_Receive_WasCancelled 0xFC

#define ECB_CompletionCode_Send_Success 0x00
#define ECB_CompletionCode_Send_CouldSentPacket 0xFF
#define ECB_CompletionCode_Send_StationDoesntExist 0xFE
#define ECB_CompletionCode_Send_IncorrectPacket 0xFD
#define ECB_CompletionCode_Send_WasCancelled 0xFC

#define MAX_FRAGMENT_COUNT 1

typedef struct ECB
{
    uint16_t far* link;
    void(far* esrAddress)();
    uint8_t inUse;
    uint8_t completionCode;
    uint16_t socket;
    uint8_t ipxWorkspace[4];
    uint8_t driverWorkspace[12];
    uint8_t immAddress[6];
    uint16_t fragmentCount;
    struct
    {
        uint16_t far* address;
        uint16_t size;
    } fragments[MAX_FRAGMENT_COUNT];
} ECB;
