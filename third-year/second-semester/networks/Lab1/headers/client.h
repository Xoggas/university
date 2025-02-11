#pragma once

#include <mem.h>
#include <stdio.h>
#include <stdlib.h>

#include "ecb.h"
#include "file_reader.h"
#include "ipx.h"
#include "packet.h"

#define SERVER_SOCKET 0x4000
#define CLIENT_SOCKET 0x4001

uint16_t socket;
Packet sendPacket;
ECB sendEcb;
Packet receivePacket;
ECB receiveEcb;

char* imagePath;
Chunk chunk;

void Client_initialize();

void Client_start()
{
    socket = IPX_OpenSocket(IPX_SocketType_LongLiving, CLIENT_SOCKET);

    Client_initialize();

    while (1)
    {
        printf("Enter input image path: ");

        scanf("%128s", imagePath);

        File_open(imagePath);

        sendPacket.totalSize = File_getSize();

        while (File_getNextChunk(&chunk))
        {
            sendPacket.chunkOffset = chunk.offset;

            sendPacket.chunkSize = chunk.size;

            memcpy(sendPacket.chunk, chunk.buffer, chunk.size);

            IPX_SendPacket(&sendEcb);

            IPX_ListenForPacket(&receiveEcb);
        }
    }
}

void Client_initialize()
{
    memset(&sendEcb, 0, sizeof(ECB));
    memset(sendEcb.immAddress, 0xFF, 6);
    sendEcb.esrAddress = NULL;
    sendEcb.socket = socket;
    sendEcb.fragmentCount = 1;
    sendEcb.fragments[0].address = (uint16_t far*)&sendPacket;
    sendEcb.fragments[0].size = sizeof(Packet);

    sendPacket.header.packetType = 0x04;
    memset(sendPacket.header.destNode, 0xFF, 6);
    sendPacket.header.destSocket = SERVER_SOCKET;

    memset(&receiveEcb, 0, sizeof(ECB));
    memset(receiveEcb.immAddress, 0xFF, 6);
    receiveEcb.esrAddress = NULL;
    receiveEcb.socket = socket;
    receiveEcb.fragmentCount = 1;
    receiveEcb.fragments[0].address = (uint16_t far*)&receivePacket;
    receiveEcb.fragments[0].size = sizeof(Packet);
}