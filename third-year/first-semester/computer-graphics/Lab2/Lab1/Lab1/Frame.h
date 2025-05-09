#pragma once

#include <malloc.h>
#include <stdexcept>

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
	uint32_t* matrix;
	int width;
	int height;
} Frame;

const Color COLOR_BLACK()
{
	return { 0, 0, 0, 255 };
}

const Color COLOR_RED()
{
	return { 255, 0, 0, 255 };
}

const Color COLOR_BLUE()
{
	return { 0, 0, 255, 255 };
}

const Color COLOR_GREEN()
{
	return { 0, 255, 0, 255 };
}

void Frame_init(Frame* frame, int width, int height)
{
	frame->matrix = (uint32_t*)malloc(sizeof(uint32_t) * width * height);
	frame->width = width;
	frame->height = height;
}

void Frame_dispose(Frame* frame)
{
	free(frame->matrix);
	frame->width = 0;
	frame->height = 0;
}

void Frame_setSize(Frame* frame, int newWidth, int newHeight, float zoom)
{
	if (frame->matrix == NULL)
	{
		throw std::overflow_error("Tried to reallocate NULL ptr");
	}

	if (zoom <= 0)
	{
		return;
	}

	newWidth /= zoom;
	newHeight /= zoom;

	if (newWidth <= 0 || newHeight <= 0)
	{
		return;
	}

	frame->matrix = (uint32_t*)realloc(frame->matrix, sizeof(uint32_t) * newWidth * newHeight);

	if (frame->matrix == NULL)
	{
		throw std::overflow_error("Couldn't reallocate the memory");
	}

	frame->width = newWidth;
	frame->height = newHeight;
}

void Frame_setPixel(Frame* frame, int x, int y, Color color)
{
	if (x < 0 || x >= frame->width || y < 0 || y >= frame->height)
	{
		return;
	}

	int index = y * frame->width + x;

	frame->matrix[index] = (color.b) | (color.g << 8) | (color.r << 16) | (color.a << 24);
}