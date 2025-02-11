#pragma once

#include <mem.h>
#include <stdio.h>
#include <stdlib.h>

#include "types.h"

FILE* file;
uint16_t totalSize;

typedef struct Chunk
{
    uint8_t buffer[512];
    uint32_t offset;
    uint32_t size;
} Chunk;

void File_open(char* path)
{
    file = fopen(path, "rb");

    if (not file)
    {
        printf("Couldn't open the file!\n");
    }
}

uint32_t File_getSize()
{
    fseek(file, 0, SEEK_END);

    {
        uint32_t size = ftell(file);

        fseek(file, 0, SEEK_SET);

        return size;
    }
}

uint8_t File_getNextChunk(Chunk* chunk)
{
    uint32_t readBytes = fread(chunk->buffer, 1, 512, file);

    if (readBytes == 0)
    {
        fclose(file);

        return 0;
    }

    chunk->offset = ftell(file);

    chunk->size = readBytes;

    return 1;
}
