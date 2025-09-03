#pragma once

#include <math.h>

class Matrix
{
	float M[3][3];

public:
	Matrix() : M{ { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } }
	{
	}

	// Конструктор, который инициализирует матрицу M поэлементно значениями аргументов
	Matrix(float A00, float A01, float A02,
		float A10, float A11, float A12,
		float A20, float A21, float A22) : M{
			{ A00, A01, A02 },
			{ A10, A11, A12 },
			{ A20, A21, A22 }
		}
	{
	}

	// Умножение матриц
	Matrix operator * (const Matrix& A) const
	{
		Matrix R;

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				R.M[i][j] = M[i][0] * A.M[0][j] + M[i][1] * A.M[1][j] + M[i][2] * A.M[2][j];
			}
		}

		return R;
	}

	static Matrix Translate(float x, float y)
	{
		return Matrix(
			1, 0, 0,
			0, 1, 0,
			x, y, 1
		);
	}

	static Matrix Scale(float x, float y)
	{
		return Matrix(
			x, 0, 0, 
			0, y, 0, 
			0, 0, 1
		);
	}

	static Matrix Rotation(float angle)
	{
		const float PI = 3.14159274;
		angle = angle / 180 * PI;
		float cosA = cos(angle);
		float sinA = sin(angle);
		return Matrix(cosA, sinA, 0, -sinA, cosA, 0, 0, 0, 1);
	}

	// Матрица для преобразования мировых координат в экранные координаты области (порта) вывода
	// (X1, Y1) - экранные координаты левого верхнего угла области (порта) вывода
	// (X2, Y2) - экранные координаты правого нижнего угла области (порта) вывода
	// (x1, y1) - мировые координаты левого нижнего угла области вывода
	// (x2, y2) - мировые координаты правого верхнего угла области вывода
	static Matrix WorldToScreen(float X1, float Y1, float X2, float Y2, float x1, float y1, float x2, float y2)
	{
		float px = (X2 - X1) / (x2 - x1), py = (Y2 - Y1) / (y2 - y1);
		
		return Matrix(
			px, 0, 0,
			0, -py, 0,
			X1 - x1 * px, Y2 + y1 * py, 1
		);
	}

	friend class Vector;
};

class Vector
{
public:

	float x, y;
	
	Vector() : x(0), y(0)
	{
	}

	Vector(float _x, float _y) : x(_x), y(_y)
	{
	}
	
	Vector operator * (const Matrix& A)
	{
		Vector E;
		E.x = x * A.M[0][0] + y * A.M[1][0] + A.M[2][0];
		E.y = x * A.M[0][1] + y * A.M[1][1] + A.M[2][1];
		float h = x * A.M[0][2] + y * A.M[1][2] + A.M[2][2];
		E.x /= h;
		E.y /= h;
		return E;
	}

	Vector operator + (const Vector& B)
	{
		return Vector(x + B.x, y + B.y);
	}

	Vector operator / (const float B)
	{
		return Vector(x / B, y / B);
	}

	float length(const Vector& b) const
	{
		double dx = x - b.x;
		double dy = y - b.y;
		return sqrt(dx * dx + dy * dy);
	}
};