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

	LRESULT(*WindowProc)(HWND, UINT, WPARAM, LPARAM) = _DefWindowProc; //�E�B���h�E�v���V�[�W���ւ̃|�C���^

	int windowWidth = 1280; //�E�B���h�E�̕�
	int windowHeight = 720; //�E�B���h�E�̍���
	std::wstring windowName = L"DirectXGame";

	WNDCLASSEX wndClassEx{};
	HWND wndHandle;
	MSG msg{};

	void InitInternal();

public:
	//WindowsAPI�̏���������
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

	//���b�Z�[�W����
	static void ProcessMessage();
};