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
	bool preventShaderDisposal = false;

public:
	Shape(Frame* _frame, Color _c1, Color _c2, Color _c3) : frame(_frame), c1(_c1), c2(_c2), c3(_c3) 
	{
		
	}

	~Shape() 
	{
		if (preventShaderDisposal)
		{
			return;
		}

		if (shader != nullptr)
		{
			delete shader;
		}
	}

	void setShader(Shader* shader)
	{
		this->shader = shader;
		
		preventShaderDisposal = true;
	}

	void changeAlpha(char addedValue)
	{
		if (shader == nullptr)
		{
			return;
		}

		int alpha = (int)shader->alpha + addedValue;

		if (alpha > 255)
		{
			alpha = 255;
		}

		if (alpha < 0)
		{
			alpha = 0;
		}

		shader->alpha = (uint8)alpha;
	}

	void setShaderByIndex()
	{
		switch (selectedShader)
		{
		case 0:
			setFillShader();
			break;
		case 1:
			setBarycentricShader();
			break;
		case 2:
			setRadialShader();
			break;
		case 3:
			setSectorShader();
			break;
		}
	}

	void setNextShader()
	{
		selectedShader++;

		if (selectedShader > 3)
		{
			selectedShader = 0;
		}

		setShaderByIndex();
	}

	void setPreviousShader()
	{
		selectedShader--;

		if (selectedShader < 0)
		{
			selectedShader = 3;
		}

		setShaderByIndex();
	}

	void setSelected(bool isSelected)
	{
		this->isSelected = isSelected;
		
		if (shader != nullptr)
		{
			shader->setIsInverted(isSelected);
		}
	}

	virtual void setFillShader() = 0;
	virtual void setBarycentricShader() = 0;
	virtual void setRadialShader() = 0;
	virtual void setSectorShader() = 0;
	virtual void draw(int frameIndex) = 0;
	virtual bool isPointInside(int x, int y) = 0;

protected:
	bool isSelected = false;
	int selectedShader = 0;
	Color c1, c2, c3;
	Frame* frame;
	Shader* shader;
};

