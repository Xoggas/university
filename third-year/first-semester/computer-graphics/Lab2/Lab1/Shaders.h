#pragma once

#include "Frame.h"

#define M_PI 3.14159265358979323846

class Shader
{
	bool isInverted = false;

public:
	uint8 alpha = 255;

	Shader(bool _isInverted) : isInverted(_isInverted)
	{

	}

	void setIsInverted(bool isInverted)
	{
		this->isInverted = isInverted;
	}

	bool getIsInverted() const
	{
		return isInverted;
	}

	virtual Color getColor(float x, float y) = 0;
};

class FillShader : public  Shader
{
	Color color;

public:
	FillShader(Color c, bool isInverted) : Shader(isInverted), color(c) {}

	Color getColor(float x, float y) override
	{
		Color newColor = color.applyAlpha(alpha);

		if (getIsInverted())
		{
			return newColor.getInverted();
		}

		return newColor;
	}
};

class BarycentricInterpolator : public  Shader
{
	float x0, y0, x1, y1, x2, y2, S;
	Color C0, C1, C2;

public:
	BarycentricInterpolator(float _x0, float _y0, float _x1, float _y1, float
		_x2, float _y2, Color A0, Color A1, Color A2, bool isInverted) :
		Shader(isInverted), x0(_x0), y0(_y0), x1(_x1), y1(_y1), x2(_x2), y2(_y2),
		S((_y1 - _y2)* (_x0 - _x2) + (_x2 - _x1) * (_y0 - _y2)), C0(A0), C1(A1),
		C2(A2)
	{
	}

	void setPoints(Point p1, Point p2, Point p3)
	{
		x0 = p1.x;
		y0 = p1.y;
		x1 = p2.x;
		y1 = p2.y;
		x2 = p3.x;
		y2 = p3.y;
		S = ((y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2));
	}

	Color getColor(float x, float y) override
	{
		float h0 = ((y1 - y2) * (x - x2) + (x2 - x1) * (y - y2)) / S;
		float h1 = ((y2 - y0) * (x - x2) + (x0 - x2) * (y - y2)) / S;
		float h2 = 1 - h0 - h1;
		float r = h0 * C0.r + h1 * C1.r + h2 * C2.r;
		float g = h0 * C0.g + h1 * C1.g + h2 * C2.g;
		float b = h0 * C0.b + h1 * C1.b + h2 * C2.b;
		float a = h0 * C0.a + h1 * C1.a + h2 * C2.a;

		Color c = { (uint8)r, (uint8)g, (uint8)b, (uint8)a };

		c = c.applyAlpha(alpha);

		if (getIsInverted())
		{
			return c.getInverted();
		}

		return c;
	}
};

class RadialBrush : public Shader
{
	float cx, cy;
	Color C0, C1;
	float angle;

public:
	RadialBrush(float _x0, float _y0, float _x1, float _y1, Color A0, Color A1, float
		_angle, bool isInverted) : Shader(isInverted),
		cx((_x0 + _x1) / 2.0f), cy((_y0 + _y1) / 2.0f),
		C0(A0), C1(A1), angle(_angle)
	{
	}

	Color getColor(float x, float y) override
	{
		double dx = (double)x - cx, dy = (double)y - cy;
		double radius = sqrt(dx * dx + dy * dy);
		float h0 = (sin(radius + angle) + 1.0f) / 2;
		float h1 = 1 - h0;
		float r = h0 * C0.r + h1 * C1.r;
		float g = h0 * C0.g + h1 * C1.g;
		float b = h0 * C0.b + h1 * C1.b;
		float a = h0 * C0.a + h1 * C1.a;

		Color c = { (uint8)r, (uint8)g, (uint8)b, (uint8)a };

		c = c.applyAlpha(alpha);

		if (getIsInverted())
		{
			return c.getInverted();
		}

		return c;
	}
};

class SectorShader : public Shader
{
	float cx, cy, radius;

public:
	SectorShader(float _cx, float _cy, float _radius, bool isInverted) : Shader(isInverted),
		cx(_cx), cy(_cy), radius(_radius)
	{
	}

	float GetAngle(float cx, float cy, float px, float py)
	{
		float dx = px - cx;
		float dy = py - cy;
		float angle = atan2f(dy, dx) * (180.0f / M_PI);

		if (angle < 0)
		{
			angle += 360.0f;
		}

		return angle;
	}

	Color HSVtoRGB(float h, float s, float v, uint8 alpha)
	{
		float c = v * s;
		float x = c * (1 - fabs(fmodf(h / 60.0f, 2) - 1));
		float m = v - c;

		float r1, g1, b1;

		if (h < 60) 
		{
			r1 = c; g1 = x; b1 = 0;
		}
		else if (h < 120) 
		{
			r1 = x; g1 = c; b1 = 0;
		}
		else if (h < 180) 
		{
			r1 = 0; g1 = c; b1 = x;
		}
		else if (h < 240) 
		{
			r1 = 0; g1 = x; b1 = c;
		}
		else if (h < 300) 
		{
			r1 = x; g1 = 0; b1 = c;
		}
		else 
		{
			r1 = c; g1 = 0; b1 = x;
		}

		Color result;
		result.r = (uint8)((r1 + m) * 255);
		result.g = (uint8)((g1 + m) * 255);
		result.b = (uint8)((b1 + m) * 255);
		result.a = alpha;

		return result;
	}

	Color getColor(float x, float y) override
	{
		double hue = GetAngle(cx, cy, x, y);
		double dx = (double)x - cx, dy = (double)y - cy;
		double distanceFromCenter = sqrt(dx * dx + dy * dy);
		double saturation = distanceFromCenter / radius;
		
		Color c = HSVtoRGB(hue, saturation, 1, alpha);

		if (getIsInverted())
		{
			return c.getInverted();
		}

		return c;
	}
};