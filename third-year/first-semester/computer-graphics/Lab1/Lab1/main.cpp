#include <Windows.h>
#include <iostream>

#include "Drawer.h"
#include <windowsx.h>

const wchar_t CLASS_NAME[] = L"window_for_drawing";
const UINT_PTR TIMER_ID = 1;
const int TIMER_INTERVAL_MS = 33;

Frame frame;

int windowWidth = 640;
int windowHeight = 480;
int frameIndex = 0;
int zoom = 1;

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
			
		case WM_TIMER:
		{
			if (wParam == TIMER_ID) 
			{
				frameIndex++;
				Draw(&frame, frameIndex);
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

			Draw(&frame, frameIndex);
			StretchDIBits(hdc, 0, 0, windowWidth, windowHeight, 0, 0, frame.width, frame.height, frame.matrix, &bmi, DIB_RGB_COLORS, SRCCOPY);
			EndPaint(hwnd, &ps);
		}

		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main()
{
	Frame_init(&frame, windowWidth, windowHeight);

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