#pragma once

#include <math.h>

#include "Frame.h"
#include "Matrix.h"

#define M_PI 3.14159265358979323846

class Shader
{
public:
	Shader()
	{
	}

	virtual Color getColor(float x, float y) = 0;
};

class FillShader : public  Shader
{
	Color color;

public:
	FillShader(Color c) : color(c) {}

	Color getColor(float x, float y) override
	{
		return color;
	}
};

class RTriangleShader : public Shader
{
	Color edgeColor;
	Color centerColor;

public:
	Vector center;
	float radius;

	RTriangleShader(Color edge, Color centerCol) : edgeColor(edge), centerColor(centerCol)
	{
	}

	Color getColor(float x, float y) override
	{
		Vector p = { x, y };

		float t = p.length(center) / radius;

		Color result;
		result.r = edgeColor.r * (1 - t) + centerColor.r * t;
		result.g = edgeColor.g * (1 - t) + centerColor.g * t;
		result.b = edgeColor.b * (1 - t) + centerColor.b * t;
		result.a = 255;
		return result;
	}
};