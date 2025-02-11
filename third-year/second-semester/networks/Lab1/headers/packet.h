#pragma once

#include "types.h"

typedef struct Header
{
    uint16_t checksum;
    uint16_t length;
    uint8_t transportControl;
    uint8_t packetType;
    uint8_t destNetwork[4];
    uint8_t destNode[6];
    uint16_t destSocket;
    uint8_t sourceNetwork[4];
    uint8_t sourceNode[6];
    uint16_t sourceSocket;
} Header;

typedef struct Packet
{
    Header header;
    uint8_t chunk[512];
    uint32_t chunkOffset;
    uint32_t chunkSize;
    uint32_t totalSize;
} Packet;
