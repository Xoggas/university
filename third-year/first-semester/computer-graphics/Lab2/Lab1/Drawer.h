#pragma once

#include <math.h>
#include <vector>

#include "Point.h"
#include "Frame.h"
#include "Shape.h"
#include "Shaders.h"

void DrawPixel(Frame* frame, int x, int y, Shader* shader)
{
	Frame_setPixel(frame, x, y, shader->getColor(x, y));
}

void DrawTriangle(Frame* frame, int x0, int y0, int x1, int y1, int x2, int y2, Shader* shader)
{
	if (y1 < y0)
	{
		std::swap(y1, y0);
		std::swap(x1, x0);
	}
	if (y2 < y1)
	{
		std::swap(y2, y1);
		std::swap(x2, x1);
	}
	if (y1 < y0)
	{
		std::swap(y1, y0);
		std::swap(x1, x0);
	}

	int totalHeight = y2 - y0;

	for (int i = 0; i < totalHeight; i++)
	{
		bool secondHalf = i > y1 - y0 || y1 == y0;
		int segmentHeight = secondHalf ? y2 - y1 : y1 - y0;
		float alpha = (float)i / totalHeight;
		float beta = (float)(i - (secondHalf ? y1 - y0 : 0)) / segmentHeight;

		Point A = {
			x0 + (x2 - x0) * alpha,
			y0 + (y2 - y0) * alpha
		};

		Point B = {
			secondHalf ? x1 + (x2 - x1) * beta : x0 + (x1 - x0) * beta,
			secondHalf ? y1 + (y2 - y1) * beta : y0 + (y1 - y0) * beta
		};

		if (A.x > B.x)
		{
			std::swap(A, B);
		}

		for (int j = A.x; j <= B.x; j++)
		{
			int x = j;
			int y = y0 + i;

			DrawPixel(frame, x, y, shader);
		}
	}
}

void DrawTriangle_P(Frame* frame, Point p0, Point p1, Point p2, Shader* shader)
{
	DrawTriangle(frame, p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, shader);
}

void DrawLineEx(Frame* frame, int x0, int y0, int x1, int y1, Shader* shader)
{
	bool steep = abs(y1 - y0) > abs(x1 - x0);

	if (steep)
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
	}

	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = abs(y1 - y0);
	int error = dx / 2;
	int ystep = (y0 < y1) ? 1 : -1;
	int y = y0;

	for (int x = x0; x <= x1; ++x)
	{
		if (steep)
		{
			DrawPixel(frame, y, x, shader);
		}
		else
		{
			DrawPixel(frame, x, y, shader);
		}

		error -= dy;

		if (error < 0)
		{
			y += ystep;
			error += dx;
		}
	}
}

void DrawCheckerboard(Frame* frame)
{
	const Color c1 = { 128, 128, 128, 255 };
	const Color c2 = { 120, 160, 140, 255 };

	for (int y = 0; y < frame->height; y++)
	{
		for (int x = 0; x < frame->width; x++)
		{
			Frame_setPixel(frame, x, y, (x + y) % 2 == 0 ? c1 : c2);
		}
	}
}

void DrawLine(Frame* frame, Point a, Point b, Shader* shader)
{
	DrawLineEx(frame, a.x, a.y, b.x, b.y, shader);
}

void Draw(Frame* frame, std::vector<Shape*>& shapes, int frameIndex)
{
	DrawCheckerboard(frame);

	for (Shape* shape : shapes)
	{
		shape->draw(frameIndex);
	}
}