#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include "address.h"
#include "ecb.h"
#include "types.h"

#define IPX_OpenSocket_Success 0x00
#define IPX_OpenSocket_AlreadyOpened 0xFF
#define IPX_OpenSocket_SocketTableOverflow 0xFE

#define IPX_SocketType_ShortLiving 0x00
#define IPX_SocketType_LongLiving 0xFF
#define IPX_SocketNumber_DynamicSocketNumber 0x0000

bool IPX_IsDriverOk()
{
    uint8_t result = 0;

    __asm
    {
        mov ax, 7A00h
        int 2Fh
        mov result, al
    }

    return result == 0xFF;
}

uint16_t IPX_OpenSocket(uint8_t socketType, uint16_t socketNumber)
{
    uint8_t outputCode = -1;
    uint16_t claimedSocketNumber = -1;

    __asm
    {
        mov bx, 00h
        mov al, socketType
        mov dx, socketNumber
        int 7Ah
        mov outputCode, al
        mov claimedSocketNumber, dx
    }

    if (outputCode == IPX_OpenSocket_Success)
    {
        printf("Successfully opened socket with number %d\n", claimedSocketNumber);
        return claimedSocketNumber;
    }
    else if (outputCode == IPX_OpenSocket_AlreadyOpened)
    {
        printf("Socket with number %d was already opened\n", claimedSocketNumber);
        return -1;
    }
    else if (outputCode == IPX_OpenSocket_SocketTableOverflow)
    {
        printf("Socket table overflow\n");
        return -1;
    }
    else
    {
        printf("Unknown error\n");
        return -1;
    }
}

void IPX_CloseSocket(uint16_t socketNumber)
{
    __asm
    {
        mov bx, 01h
        mov dx, socketNumber
        int 7Ah
    }

    printf("Closed socket with number %d\n", socketNumber);
}

void IPX_GetLocalTarget(Address* address)
{
    __asm
    {
        mov bx, 09h
        mov ax, ds
        mov es, ax
        mov si, [address]
        int 7Ah
    }
}

void IPX_ListenForPacket(ECB* ecb)
{
    ecb->inUse = 0;

    _asm
    {
        mov bx, 04h
        mov ax, ds
        mov es, ax
        mov si, [ecb]
        int 7Ah
    }

    while (ecb->inUse != 0)
    {
        _asm
        {
            mov bx, 0Ah
            int 7Ah
        }
    }
}

void IPX_SendPacket(ECB* ecb)
{
    ecb->inUse = 0;

    _asm
    {
        mov bx, 03h
        mov ax, ds
        mov es, ax
        mov si, [ecb]
        int 7Ah
    }

    while (ecb->inUse != 0)
    {
        _asm
        {
            mov bx, 0Ah
            int 7Ah
        }
    }
}
