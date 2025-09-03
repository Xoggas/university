#pragma once

#include <math.h>

typedef struct Point
{
	int x;
	int y;

public:
	int length(Point& b) const {
		double dx = x - b.x;
		double dy = y - b.y;
		return sqrt(dx * dx + dy * dy);
	}
} Point;