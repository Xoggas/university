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

FILE* outputFile;

void Server_initialize();

void Server_writeDataToFile();

void Server_start()
{
    socket = IPX_OpenSocket(IPX_SocketType_LongLiving, SERVER_SOCKET);

    Server_initialize();

    printf("Listening for requests!\n");

    while (1)
    {
        IPX_ListenForPacket(&receiveEcb);

        Server_writeDataToFile();

        IPX_SendPacket(&sendEcb);
    }
}

void Server_writeDataToFile()
{
    if (not outputFile)
    {
        char* imagePath;

        printf("Enter input image path: ");

        scanf("%128s", imagePath);

        outputFile = fopen(imagePath, "wb");

        printf("Opened a file for writing!");
    }

    printf("[Receiving] offset: %ld, size: %ld, total: %ld\n", receivePacket.chunkOffset, receivePacket.chunkSize,
           receivePacket.totalSize);

    fwrite(receivePacket.chunk, sizeof(uint8_t), receivePacket.chunkSize, outputFile);

    if (receivePacket.chunkOffset == receivePacket.totalSize)
    {
        fclose(outputFile);

        outputFile = NULL;

        printf("Finished receiving file!\n");
    }
}

void Server_initialize()
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
    sendPacket.header.destSocket = CLIENT_SOCKET;

    memset(&receiveEcb, 0, sizeof(ECB));
    memset(receiveEcb.immAddress, 0xFF, 6);
    receiveEcb.esrAddress = NULL;
    receiveEcb.socket = socket;
    receiveEcb.fragmentCount = 1;
    receiveEcb.fragments[0].address = (uint16_t far*)&receivePacket;
    receiveEcb.fragments[0].size = sizeof(Packet);
}
