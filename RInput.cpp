#include "RInput.h"

#define DIRECTINPUT_VERSION 0x0800
#include <memory>
#include <cassert>
#include <wrl.h>
#include "RWindow.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

using namespace std;
using namespace Microsoft::WRL;

ComPtr<IDirectInput8> directInput = nullptr;
ComPtr<IDirectInputDevice8> keyboard = nullptr;
BYTE keyState[256] = {};
BYTE oldKeyState[256] = {};

void InitInput()
{
	HRESULT result;
	result = DirectInput8Create(
		GetRWindow()->GetWindowClassEx().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr
	);
	assert(SUCCEEDED(result));

	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//���̓f�[�^�`���̃Z�b�g
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	//�r�����䃌�x���̃Z�b�g
	result = keyboard->SetCooperativeLevel(
		GetRWindow()->GetWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY //������A�A�N�e�B�u�E�B���h�E,��L���Ȃ�,Win�L�[����
	);
	assert(SUCCEEDED(result));
}

void UpdateInput()
{
	//���̎擾�J�n
	keyboard->Acquire();

	//�S�L�[�̓��͏�Ԃ��擾����
	for (int i = 0; i < 256; i++) {
		oldKeyState[i] = keyState[i];
	}
	keyboard->GetDeviceState(sizeof(keyState), keyState);
}

bool GetKey(unsigned char key)
{
	return keyState[key];
}

bool GetKeyUp(unsigned char key)
{
	return !keyState[key] && oldKeyState[key];
}

bool GetKeyDown(unsigned char key)
{
	return keyState[key] && !oldKeyState[key];
}
