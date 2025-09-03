#pragma once

#include <math.h>

#include "Shape.h"

class Circle : public Shape
{
public:
	int cx, cy, radius;

	Circle(Frame* _frame, Color _c1, Color _c2, Color _c3) : Shape(_frame, _c1, _c2, _c3)
	{
	}

	bool isPointInside(int x, int y) override
	{
		int dx = cx - x;
		int dy = cy - y;
		int distance = sqrt(dx * dx + dy * dy);
		return distance <= radius;
	}

	void setFillShader() override
	{
		if (shader != nullptr)
		{
			delete shader;
		}

		shader = new FillShader(c1, isSelected);
	}

	void setBarycentricShader() override
	{
		if (shader != nullptr)
		{
			delete shader;
		}

		int x0 = cx + DCOS(270) * radius;
		int y0 = cy + DSIN(270) * radius;

		int x1 = cx + DCOS(270 + 120) * radius;
		int y1 = cy + DSIN(270 + 120) * radius;

		int x2 = cx + DCOS(270 + 120 + 120) * radius;
		int y2 = cy + DSIN(270 + 120 + 120) * radius;

		shader = new BarycentricInterpolator(x0, y0, x1, y1, x2, y2, c1, c2, c3, isSelected);
	}

	void setRadialShader() override
	{
		if (shader != nullptr)
		{
			delete shader;
		}

		int x0 = cx - DCOS(270 + 45) * radius;
		int y0 = cx - DSIN(270 + 45) * radius;

		int x1 = cx + DCOS(270 - 180 + 45) * radius;
		int y1 = cx + DSIN(270 - 180 + 45) * radius;

		shader = new RadialBrush(x0, y0, x1, y1, c1, c2, 0, isSelected);
	}

	void setSectorShader() override
	{
		if (shader != nullptr)
		{
			delete shader;
		}

		shader = new SectorShader(cx, cy, radius, isSelected);
	}

	void draw(int frameIndex) override
	{
		for (int y = -radius; y <= radius; y++)
		{
			int dx = (int)sqrt(radius * radius - y * y);
			
			for (int x = -dx; x <= dx; x++)
			{
				DrawPixel(frame, cx + x, cy + y, shader);
			}
		}
	}
};

class Triangle : public Shape
{
public:
	int x0, y0, x1, y1, x2, y2;

	Triangle(Frame* _frame, Color _c1, Color _c2, Color _c3) : Shape(_frame, _c1, _c2, _c3)
	{
	}

	int sign(int a)
	{
		if (a < 0)
		{
			return -1;
		}

		if (a > 0)
		{
			return 1;
		}

		return 0;
	}

	bool isPointInside(int x, int y) override
	{
		float a = sign((x0 - x) * (y1 - y0) - (x1 - x0) * (y0 - y));
		float b = sign((x1 - x) * (y2 - y1) - (x2 - x1) * (y1 - y));
		float c = sign((x2 - x) * (y0 - y2) - (x0 - x2) * (y2 - y));
		return a == b && b == c && a == c || (a == 0 || b == 0 || c == 0);
	}

	void setCoordinates(Point p1, Point p2, Point p3)
	{
		x0 = p1.x;
		y0 = p1.y;
		x1 = p2.x;
		y1 = p2.y;
		x2 = p3.x;
		y2 = p3.y;
	}

	void setFillShader() override
	{
		if (shader != nullptr)
		{
			delete shader;
		}

		shader = new FillShader(c1, isSelected);
	}

	void setBarycentricShader() override
	{
		if (shader != nullptr)
		{
			delete shader;
		}

		shader = new BarycentricInterpolator(x0, y0, x1, y1, x2, y2, c1, c2, c3, isSelected);
	}

	void setRadialShader() override
	{
		if (shader != nullptr)
		{
			delete shader;
		}

		Point center = Frame_getCenter(frame);

		shader = new RadialBrush(center.x, center.y, frame->width, frame->height, c1, c2, 0, isSelected);
	}

	float distance(float x1, float y1, float x2, float y2) 
	{
		return sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
	}

	float circumradius(float ax, float ay, float bx, float by, float cx, float cy) {
		float a = distance(bx, by, cx, cy);
		float b = distance(ax, ay, cx, cy);
		float c = distance(ax, ay, bx, by);
		float s = (a + b + c) / 2.0f;
		float area = sqrtf(s * (s - a) * (s - b) * (s - c));

		if (area == 0.0f)
		{
			return 0.0f;
		}

		return (a * b * c) / (4.0f * area);
	}

	void setSectorShader() override
	{
		if (shader != nullptr)
		{
			delete shader;
		}

		auto radius = circumradius(x0, y0, x1, y1, x2, y2);

		shader = new SectorShader(frame->width / 2, frame->height / 2, radius, isSelected);
	}

	void draw(int frameIndex) override
	{
		DrawTriangle(frame, x0, y0, x1, y1, x2, y2, shader);
	}
};

class Star : public Shape
{
	Point s1, s2, s3, s4, s5, s6, s7, s8;
	Triangle spike1, spike2, spike3, spike4, inner1, inner2;

public:
	int cx, cy, radius;

	Star(Frame* _frame, Color _c1, Color _c2, Color _c3) : Shape(_frame, _c1, _c2, _c3), 
		spike1(_frame, _c1, _c2, _c3),
		spike2(_frame, _c1, _c2, _c3), 
		spike3(_frame, _c1, _c2, _c3),
		spike4(_frame, _c1, _c2, _c3), 
		inner1(_frame, _c1, _c2, _c3), 
		inner2(_frame, _c1, _c2, _c3)
	{
	}

	bool isPointInside(int x, int y) override
	{
		return spike1.isPointInside(x, y) ||
			spike2.isPointInside(x, y) ||
			spike3.isPointInside(x, y) ||
			spike4.isPointInside(x, y) ||
			inner1.isPointInside(x, y) ||
			inner2.isPointInside(x, y);
	}

	void setShader(Shader* shader)
	{
		spike1.setShader(shader);
		spike2.setShader(shader);
		spike3.setShader(shader);
		spike4.setShader(shader);
		inner1.setShader(shader);
		inner2.setShader(shader);
	}

	void setFillShader() override
	{
		if (shader != nullptr)
		{
			delete shader;
		}

		shader = new FillShader(c1, isSelected);
		
		setShader(shader);
	}

	void setBarycentricShader() override
	{
		if (shader != nullptr)
		{
			delete shader;
		}

		shader = new BarycentricInterpolator(s1.x, s1.y, s7.x, s7.y, s4.x, s4.y, c1, c2, c3, isSelected);

		setShader(shader);
	}

	void setRadialShader() override
	{
		if (shader != nullptr)
		{
			delete shader;
		}

		shader = new RadialBrush(cx, cy, 0, 0, c1, c2, 0, isSelected);

		setShader(shader);
	}

	void setSectorShader() override
	{
		if (shader != nullptr)
		{
			delete shader;
		}

		shader = new SectorShader(cx, cy, radius, isSelected);

		setShader(shader);
	}

	void draw(int frameIndex) override
	{
		Point center = { cx, cy };
		int topAngle = 270;
		int angle = topAngle - frameIndex * 2;
		int smallRadius = radius * 0.3;

		s1 = center;
		s1.x += DCOS(angle) * radius;
		s1.y += DSIN(angle) * radius;

		s2 = center;
		s2.x += DCOS(angle + 45) * smallRadius;
		s2.y += DSIN(angle + 45) * smallRadius;

		s3 = center;
		s3.x += DCOS(angle + 45 * 2) * radius;
		s3.y += DSIN(angle + 45 * 2) * radius;

		s4 = center;
		s4.x += DCOS(angle + 45 * 3) * smallRadius;
		s4.y += DSIN(angle + 45 * 3) * smallRadius;

		s5 = center;
		s5.x += DCOS(angle + 45 * 4) * radius;
		s5.y += DSIN(angle + 45 * 4) * radius;

		s6 = center;
		s6.x += DCOS(angle + 45 * 5) * smallRadius;
		s6.y += DSIN(angle + 45 * 5) * smallRadius;

		s7 = center;
		s7.x += DCOS(angle + 45 * 6) * radius;
		s7.y += DSIN(angle + 45 * 6) * radius;

		s8 = center;
		s8.x += DCOS(angle + 45 * 7) * smallRadius;
		s8.y += DSIN(angle + 45 * 7) * smallRadius;

		if (typeid(shader) == typeid(BarycentricInterpolator))
		{
			auto bi = (BarycentricInterpolator*)shader;
			bi->setPoints(s1, s7, s4);
		}

		spike1.setCoordinates(s1, s8, s2);
		spike2.setCoordinates(s2, s3, s4);
		spike3.setCoordinates(s4, s5, s6);
		spike4.setCoordinates(s6, s7, s8);
		inner1.setCoordinates(s8, s6, s2);
		inner2.setCoordinates(s6, s2, s4);

		spike1.draw(frameIndex);
		spike2.draw(frameIndex);
		spike3.draw(frameIndex);
		spike4.draw(frameIndex);
		inner1.draw(frameIndex);
		inner2.draw(frameIndex);
	}
};