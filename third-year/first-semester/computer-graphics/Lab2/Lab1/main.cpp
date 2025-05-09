#include <Windows.h>
#include <iostream>
#include <windowsx.h>
#include <vector>

#include "Drawer.h"
#include "Shapes.h"

const wchar_t CLASS_NAME[] = L"window_for_drawing";
const UINT_PTR TIMER_ID = 1;
const int TIMER_INTERVAL_MS = 33;

Frame frame;
Circle outerCircle = Circle(&frame, COLOR_RED(), COLOR_GREEN(), COLOR_BLUE());
Triangle triangle = Triangle(&frame, COLOR_GREEN(), COLOR_RED(), COLOR_BLUE());
Circle innerCircle = Circle(&frame, COLOR_BLUE(), COLOR_BLUE(), COLOR_RED());
Star star = Star(&frame, COLOR_GREEN(), COLOR_RED(), COLOR_GREEN());

std::vector<Shape*> shapes = {
	&outerCircle,
	&triangle,
	&innerCircle,
	&star
};

int windowWidth = 640;
int windowHeight = 480;
int frameIndex = 0;
int zoom = 2;

Shape* selectedShape;
int mouseX = 0;
int mouseY = 0;

void InitShapes()
{
	outerCircle.setFillShader();
	triangle.setFillShader();
	innerCircle.setFillShader();
	star.setFillShader();
}

void Update()
{
	int size = Frame_getMinSize(&frame);
	int outerRadius = size / 2;
	Point center = Frame_getCenter(&frame);

	outerCircle.radius = outerRadius;
	outerCircle.cx = center.x;
	outerCircle.cy = center.y;

	Point t1 = center;
	t1.x += DCOS(270) * outerRadius;
	t1.y += DSIN(270) * outerRadius;

	Point t2 = center;
	t2.x += DCOS(270 + 120) * outerRadius;
	t2.y += DSIN(270 + 120) * outerRadius;

	Point t3 = center;
	t3.x += DCOS(270 + 120 + 120) * outerRadius;
	t3.y += DSIN(270 + 120 + 120) * outerRadius;

	triangle.x0 = t1.x;
	triangle.y0 = t1.y;
	triangle.x1 = t2.x;
	triangle.y1 = t2.y;
	triangle.x2 = t3.x;
	triangle.y2 = t3.y;

	int innerRadius = (t1.length(t2) * sqrt(3)) / 6.0;

	innerCircle.radius = innerRadius;
	innerCircle.cx = center.x;
	innerCircle.cy = center.y;

	star.radius = innerRadius;
	star.cx = center.x;
	star.cy = center.y;
}

void UpdateSelectedShape()
{
	Shape* overlappedShape = nullptr;

	for (int i = shapes.size() - 1; i >= 0; i--)
	{
		if (shapes[i]->isPointInside(mouseX, mouseY))
		{
			overlappedShape = shapes[i];
			break;
		}
	}

	if (overlappedShape == selectedShape)
	{
		return;
	}

	if (selectedShape != nullptr)
	{
		selectedShape->setSelected(false);
	}

	selectedShape = overlappedShape;

	if (selectedShape != nullptr)
	{
		selectedShape->setSelected(true);
	}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		SetTimer(hwnd, TIMER_ID, TIMER_INTERVAL_MS, nullptr);
		return 0;
	}

	case WM_DESTROY:
	{
		KillTimer(hwnd, TIMER_ID);
		PostQuitMessage(0);
		return 0;
	}

	case WM_MOUSEMOVE:
	{
		mouseX = GET_X_LPARAM(lParam) / zoom;
		mouseY = GET_Y_LPARAM(lParam) / zoom;
		UpdateSelectedShape();
		return 0;
	}

	case WM_MOUSEWHEEL:
	{
		short delta = GET_WHEEL_DELTA_WPARAM(wParam) / 120;

		zoom += delta;

		if (zoom < 1)
		{
			zoom = 1;
		}

		Frame_setSize(&frame, windowWidth, windowHeight, zoom);

		return 0;
	}

	case WM_KEYDOWN:
	{
		if (selectedShape == nullptr)
		{
			return 0;
		}

		switch (wParam)
		{
		case VK_UP:
			selectedShape->changeAlpha(5);
			break;
		case VK_DOWN:
			selectedShape->changeAlpha(-5);
			break;
		case VK_LEFT:
			selectedShape->setPreviousShader();
			break;
		case VK_RIGHT:
			selectedShape->setNextShader();
			break;
		}

		return 0;
	}

	case WM_TIMER:
	{
		if (wParam == TIMER_ID)
		{
			frameIndex++;
			Update();
			Draw(&frame, shapes, frameIndex);
			InvalidateRect(hwnd, nullptr, FALSE);
		}

		return 0;
	}

	case WM_SIZE:
	{
		windowWidth = LOWORD(lParam);
		windowHeight = HIWORD(lParam);
		Frame_setSize(&frame, windowWidth, windowHeight, zoom);
		return 0;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		BITMAPINFO bmi = {};
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = frame.width;
		bmi.bmiHeader.biHeight = -frame.height;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;

		Update();
		Draw(&frame, shapes, frameIndex);
		StretchDIBits(hdc, 0, 0, windowWidth, windowHeight, 0, 0, frame.width, frame.height, frame.matrix, &bmi, DIB_RGB_COLORS, SRCCOPY);
		EndPaint(hwnd, &ps);
	}

	return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

typedef struct AnotherLargeStruct
{
	int array[512];
} AnotherLargeStruct;

typedef struct LargeStruct
{
	std::vector<AnotherLargeStruct> array;

	LargeStruct() : array(512)
	{
	}
} LargeStruct;

static void someFunc(LargeStruct a)
{
	for (auto& i : a.array)
	{
	}
}

int main()
{
	Frame_init(&frame, windowWidth, windowHeight);

	InitShapes();

	HINSTANCE instance = GetModuleHandle(NULL);

	WNDCLASS wc = { };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = instance;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	RegisterClass(&wc);

	HWND window = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Лабораторная работа №1",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		frame.width,
		frame.height,
		NULL,
		NULL,
		instance,
		NULL
	);

	if (window == NULL)
	{
		return 0;
	}

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);

	MSG msg = {};

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Frame_dispose(&frame);
}