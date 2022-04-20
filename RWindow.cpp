#include "RWindow.h"

using namespace std;

unique_ptr<RWindow> window;

void InitRWindow() {
	window = unique_ptr<RWindow>(new RWindow());
	window->Init();
}

RWindow* GetRWindow() {
	return window.get();
}

void RWindow::Init()
{
	wndClassEx.cbSize = sizeof(WNDCLASSEX);
	wndClassEx.lpfnWndProc = (WNDPROC)WindowProc;
	wndClassEx.lpszClassName = windowName.c_str();
	wndClassEx.hInstance = GetModuleHandle(nullptr);
	wndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&wndClassEx);
	RECT wrc = { 0, 0, windowWidth, windowHeight };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	wndHandle = CreateWindow(wndClassEx.lpszClassName,
		wndClassEx.lpszClassName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr, //�e�E�B���h�E�n���h��
		nullptr, //���j���[�n���h��
		wndClassEx.hInstance, //�Ăяo���A�v���P�[�V�����n���h��
		nullptr); //�I�v�V����

	ShowWindow(wndHandle, SW_SHOW);
}

void RWindow::ProcessMessage()
{
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg); //�L�[���̓��b�Z�[�W����
		DispatchMessage(&msg);
	}
}

LRESULT _DefWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}