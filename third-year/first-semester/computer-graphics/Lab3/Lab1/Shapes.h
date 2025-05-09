#pragma once

#include <math.h>

#include "Shape.h"
#include "Matrix.h"

class ReuleauxTriangle : public Shape
{
public:
	float cx;
	float cy;
	float scale;
	float rotation;
	RTriangleShader shader;

	ReuleauxTriangle(Frame* _frame) : Shape(_frame), shader(COLOR_RED(), COLOR_BLUE())
	{
	}

	void fillArc(Vector center, Vector from, Vector to, float radius, int segments = 100)
	{
		float startAngle = atan2(from.y - center.y, from.x - center.x);
		float endAngle = atan2(to.y - center.y, to.x - center.x);
		float deltaAngle = endAngle - startAngle;

		if (deltaAngle > M_PI)
		{
			deltaAngle -= 2 * M_PI;
		}
		else if (deltaAngle < -M_PI)
		{
			deltaAngle += 2 * M_PI;
		}

		float step = deltaAngle / segments;

		for (int i = 0; i < segments; ++i)
		{
			float theta1 = startAngle + i * step;
			float theta2 = startAngle + (i + 1) * step;

			Vector p1 = center + Vector(radius * cos(theta1), radius * sin(theta1));
			Vector p2 = center + Vector(radius * cos(theta2), radius * sin(theta2));

			float radius1 = center.length(p1);
			float radius2 = center.length(p2);

			shader.radius = (radius1 + radius2) / 2.f;

			DrawTriangle_P(frame, center, p1, p2, &shader);
		}
	}

	void draw(int frameIndex) override
	{
		float W = frame->width, H = frame->height;
		float minSize = Frame_getMinSize(frame);
		Matrix WS = Matrix::WorldToScreen(W / 2 - minSize / 2, H / 2 - minSize / 2, W / 2 + minSize / 2, H / 2 + minSize / 2, -1, -1, 1, 1);
		Matrix translation = Matrix::Translate(cx, cy);
		Matrix scaling = Matrix::Scale(scale, scale);
		Matrix rotating = Matrix::Rotation(rotation);

		float angleA = 0.0f;
		float angleB = 120.0f;
		float angleC = 240.0f;

		Vector a = Vector(DCOS(angleA), DSIN(angleA));
		Vector b = Vector(DCOS(angleB), DSIN(angleB));
		Vector c = Vector(DCOS(angleC), DSIN(angleC));

		a = a * translation * scaling * rotating * WS;
		b = b * translation * scaling * rotating * WS;
		c = c * translation * scaling * rotating * WS;

		Vector center(0, 0);
		Vector size(1, 1);
		center = center * translation * scaling * rotating * WS;
		size = size * scaling * WS;

		float arcRadius = a.length(b);

		shader.center = center;
		shader.radius = size.x / 2.5f;

		fillArc(a, b, c, arcRadius);
		fillArc(b, c, a, arcRadius);
		fillArc(c, a, b, arcRadius);
	}
};