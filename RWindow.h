#pragma once
#include <Windows.h>
#include <string>
#include <memory>

LRESULT _DefWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class RWindow
{
private:
	RWindow() {};
	~RWindow() = default;
	RWindow(const RWindow& a) {};
	RWindow& operator=(const RWindow&) { return *this; }

	LRESULT(*WindowProc)(HWND, UINT, WPARAM, LPARAM) = _DefWindowProc; //ウィンドウプロシージャへのポインタ

	int windowWidth = 1280; //ウィンドウの幅
	int windowHeight = 720; //ウィンドウの高さ
	std::wstring windowName = L"DirectXGame";

	WNDCLASSEX wndClassEx{};
	HWND wndHandle;
	MSG msg{};

	void InitInternal();

public:
	//WindowsAPIの初期化処理
	static void Init();
	static RWindow* GetInstance();

	static int GetWidth() {
		return GetInstance()->windowWidth;
	}

	static int GetHeight() {
		return GetInstance()->windowHeight;
	}

	static WNDCLASSEX GetWindowClassEx() {
		return GetInstance()->wndClassEx;
	}

	static HWND GetWindowHandle() {
		return GetInstance()->wndHandle;
	}

	static MSG GetMessageStructure() {
		return GetInstance()->msg;
	}

	//メッセージ処理
	static void ProcessMessage();

	/// <summary>
	/// マウスカーソル移動
	/// ウィンドウの左上を0, 0として指定する
	/// </summary>
	/// <param name="posX">移動先X座標</param>
	/// <param name="posY">移動先Y座標</param>
	static void SetMousePos(int posX, int posY);

	/// <summary>
	/// マウスカーソル移動
	/// モニターの左上を0, 0として指定する
	/// </summary>
	/// <param name="posX">移動先X座標</param>
	/// <param name="posY">移動先Y座標</param>
	static void SetAbsMousePos(int posX, int posY);

	static void SetMouseHideFlag(bool hide);
};