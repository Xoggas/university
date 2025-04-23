#pragma once
#define uint32 unsigned int
#define uint8 unsigned char

typedef struct Color
{
	uint8 r;
	uint8 g;
	uint8 b;
	uint8 a;
} Color;

typedef struct Frame
{
	Color** matrix;
	uint32 width;
	uint32 height;
} Frame;

void Frame_init(Frame* frame, uint32 width, uint32 height)
{

}

void Frame_setSize(Frame* frame, uint32 newWidth, uint32 newHeight)
{
	
}

void Frame_setPixel(Frame* frame, int x, int y, Color color)
{
	frame->matrix[y][x] = color;
}

Color Frame_getPixel(Frame* frame, int x, int y)
{
	return frame->matrix[y][x];
}