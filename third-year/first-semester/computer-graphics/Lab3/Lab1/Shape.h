#pragma once

#include "Drawer.h"
#include "Shaders.h"
#include "Frame.h"

#define M_PI 3.14159265358979323846
#define DEG_TO_RAD(deg) deg * (M_PI / 180.0f)
#define DCOS(deg) cos(DEG_TO_RAD((deg)))
#define DSIN(deg) sin(DEG_TO_RAD((deg)))

class Shape
{
public:
	Shape(Frame* _frame) : frame(_frame)
	{
	}

	virtual void draw(int frameIndex) = 0;

protected:
	Frame* frame;
};

