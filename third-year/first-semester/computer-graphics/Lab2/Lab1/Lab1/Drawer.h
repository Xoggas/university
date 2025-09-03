#pragma once

#include <math.h>

#include "Frame.h"

typedef struct Point
{
	int x;
	int y;
} Point;

int Length(Point a, Point b)
{
	double dx = a.x - b.x;
	double dy = a.y - b.y;
	return sqrt(dx * dx + dy * dy);
}

#define M_PI 3.14159265358979323846
#define DEG_TO_RAD(deg) deg * (M_PI / 180.0f)
#define DCOS(deg) cos(DEG_TO_RAD((deg)))
#define DSIN(deg) sin(DEG_TO_RAD((deg)))

void DrawCheckerboard(Frame* frame);

void DrawPixel(Frame* frame, int x, int y, Color color);

void DrawLine(Frame* frame, Point a, Point b, Color color);

void DrawLine(Frame* frame, int x0, int y0, int x1, int y1, Color color);

void DrawCircle(Frame* frame, Point p, int radius, Color color);

void DrawCircle(Frame* frame, int cx, int cy, int radius, Color color);

int GetSize(Frame* frame)
{
	return 7.0f / 8.0f * min(frame->width, frame->height);
}

Point GetCenterPoint(Frame* frame)
{
	return { frame->width / 2 , frame->height / 2 };
}

void Draw(Frame* frame, int frameIndex)
{
	DrawCheckerboard(frame);

	int size = GetSize(frame);
	int radius = size / 2;
	int topAngle = 270;
	Point center = GetCenterPoint(frame);

	DrawCircle(frame, center, radius, COLOR_BLUE());

	// Рисуем треугольник

	Point t1 = center;
	t1.x += DCOS(topAngle) * radius;
	t1.y += DSIN(topAngle) * radius;

	Point t2 = center;
	t2.x += DCOS(topAngle + 120) * radius;
	t2.y += DSIN(topAngle + 120) * radius;

	Point t3 = center;
	t3.x += DCOS(topAngle + 120 + 120) * radius;
	t3.y += DSIN(topAngle + 120 + 120) * radius;

	DrawLine(frame, t1, t2, COLOR_RED());
	DrawLine(frame, t2, t3, COLOR_RED());
	DrawLine(frame, t1, t3, COLOR_RED());

	// Рисуем вписанную окружность

	int innerRadius = (Length(t1, t2) * sqrt(3)) / 6.0;

	DrawCircle(frame, center, innerRadius, COLOR_BLUE());

	// Рисуем звезду

	int angle = topAngle - frameIndex * 2;
	int smallRadius = innerRadius * 0.3;

	for (int i = 1; i <= 8; i++)
	{
		if (i % 2 == 0)
		{
			Point s1 = center;
			s1.x += DCOS(angle + 45 * (i - 1)) * innerRadius;
			s1.y += DSIN(angle + 45 * (i - 1)) * innerRadius;

			Point s2 = center;
			s2.x += DCOS(angle + 45 * i) * smallRadius;
			s2.y += DSIN(angle + 45 * i) * smallRadius;

			DrawLine(frame, s1, s2, COLOR_GREEN());
		}
		else
		{
			Point s1 = center;
			s1.x += DCOS(angle + 45 * (i - 1)) * smallRadius;
			s1.y += DSIN(angle + 45 * (i - 1)) * smallRadius;

			Point s2 = center;
			s2.x += DCOS(angle + 45 * i) * innerRadius;
			s2.y += DSIN(angle + 45 * i) * innerRadius;

			DrawLine(frame, s1, s2, COLOR_GREEN());
		}
	}
}

void DrawPixel(Frame* frame, int x, int y, Color color)
{
	Frame_setPixel(frame, x, y, color);
}

void DrawLine(Frame* frame, Point a, Point b, Color color)
{
	DrawLine(frame, a.x, a.y, b.x, b.y, color);
}

void DrawLine(Frame* frame, int x0, int y0, int x1, int y1, Color color)
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
			DrawPixel(frame, y, x, color);
		}
		else
		{
			DrawPixel(frame, x, y, color);
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

void DrawCircle(Frame* frame, Point p, int radius, Color color)
{
	DrawCircle(frame, p.x, p.y, radius, color);
}

void DrawCircle(Frame* frame, int cx, int cy, int radius, Color color)
{
	int x = 0;
	int y = -radius;
	int p = -radius;

	while (x < -y)
	{
		if (p > 0)
		{
			y += 1;
			p += 2 * (x + y) + 1;
		}
		else
		{
			p += 2 * x + 1;
		}

		DrawPixel(frame, cx + x, cy + y, color);
		DrawPixel(frame, cx - x, cy + y, color);
		DrawPixel(frame, cx + x, cy - y, color);
		DrawPixel(frame, cx - x, cy - y, color);
		DrawPixel(frame, cx + y, cy + x, color);
		DrawPixel(frame, cx - y, cy + x, color);
		DrawPixel(frame, cx + y, cy - x, color);
		DrawPixel(frame, cx - y, cy - x, color);

		x += 1;
	}
}
