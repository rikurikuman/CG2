#pragma once
#include <Windows.h>
#include <string>
#include <memory>

LRESULT _DefWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class RWindow
{
private:
	LRESULT(*WindowProc)(HWND, UINT, WPARAM, LPARAM) = _DefWindowProc; //�E�B���h�E�v���V�[�W���ւ̃|�C���^

	int windowWidth = 1280; //�E�B���h�E�̕�
	int windowHeight = 720; //�E�B���h�E�̍���
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

	//WindowsAPI�̏���������
	void Init();

	//���b�Z�[�W����
	void ProcessMessage();
};

//RWindow���擾����
RWindow* GetRWindow();

//RWindow������������
void InitRWindow();