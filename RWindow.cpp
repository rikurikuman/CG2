#include "RWindow.h"

using namespace std;

void RWindow::Init() {
	GetInstance()->InitInternal();
}

RWindow* RWindow::GetInstance() {
	static RWindow instance;
	return &instance;
}

void RWindow::InitInternal()
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
		nullptr, //親ウィンドウハンドル
		nullptr, //メニューハンドル
		wndClassEx.hInstance, //呼び出しアプリケーションハンドル
		nullptr); //オプション

	ShowWindow(wndHandle, SW_SHOW);
}

void RWindow::ProcessMessage()
{
	RWindow* instance = GetInstance();
	if (PeekMessage(&instance->msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&instance->msg); //キー入力メッセージ処理
		DispatchMessage(&instance->msg);
	}
}

void RWindow::SetMousePos(int posX, int posY)
{
	int xPos_absolute, yPos_absolute;

	WINDOWINFO windowInfo;

	//ウィンドウの位置を取得
	windowInfo.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(RWindow::GetWindowHandle(), &windowInfo);

	//マウスの移動先の絶対座標（モニター左上からの座標）
	xPos_absolute = posX + windowInfo.rcWindow.left;
	yPos_absolute = posY + windowInfo.rcWindow.top + 35; //ウィンドウのタイトルバーの分（35px）をプラス

	SetCursorPos(xPos_absolute, yPos_absolute);
}

void RWindow::SetAbsMousePos(int posX, int posY)
{
	SetCursorPos(posX, posY);
}

void RWindow::SetMouseHideFlag(bool hide)
{
	if (hide) {
		while (ShowCursor(0) >= 0);
	}
	else {
		while (ShowCursor(1) < 0);
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