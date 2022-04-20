#pragma once
#include <Windows.h>
#include <string>
#include <memory>

LRESULT _DefWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class RWindow
{
private:
	LRESULT(*WindowProc)(HWND, UINT, WPARAM, LPARAM) = _DefWindowProc; //ウィンドウプロシージャへのポインタ

	int windowWidth = 1280; //ウィンドウの幅
	int windowHeight = 720; //ウィンドウの高さ
	std::wstring windowName = L"DirectXGame";

	WNDCLASSEX wndClassEx{};
	HWND wndHandle;
	MSG msg{};

public:
	WNDCLASSEX GetWindowClassEx() {
		return wndClassEx;
	}

	HWND GetWindowHandle() {
		return wndHandle;
	}

	MSG GetMessageStructure() {
		return msg;
	}

	//WindowsAPIの初期化処理
	void Init();

	//メッセージ処理
	void ProcessMessage();
};

//RWindowを取得する
RWindow* GetRWindow();

//RWindowを初期化する
void InitRWindow();