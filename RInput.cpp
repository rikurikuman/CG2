#include "RInput.h"
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
ComPtr<IDirectInputDevice8> mouse = nullptr;
BYTE keyState[256] = {};
BYTE oldKeyState[256] = {};
DIMOUSESTATE2 mouseState;
DIMOUSESTATE2 oldMouseState;

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
	result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	assert(SUCCEEDED(result));

	//���̓f�[�^�`���̃Z�b�g
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	result = mouse->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(result));

	//�r�����䃌�x���̃Z�b�g
	result = keyboard->SetCooperativeLevel(
		GetRWindow()->GetWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY //������A�A�N�e�B�u�E�B���h�E,��L���Ȃ�,Win�L�[����
	);
	assert(SUCCEEDED(result));
	result = mouse->SetCooperativeLevel(
		GetRWindow()->GetWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE //������A�A�N�e�B�u�E�B���h�E,��L���Ȃ�
	);
	assert(SUCCEEDED(result));
}

void UpdateInput()
{
	//���̎擾�J�n
	keyboard->Acquire();
	mouse->Acquire();

	//�S�L�[�̓��͏�Ԃ��擾����
	for (int i = 0; i < 256; i++) {
		oldKeyState[i] = keyState[i];
	}
	oldMouseState = mouseState;
	keyboard->GetDeviceState(sizeof(keyState), keyState);
	mouse->GetDeviceState(sizeof(mouseState), &mouseState);
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

bool GetMouseClick(int buttonNum)
{
	return (mouseState.rgbButtons[buttonNum] & 0x80) != 0;
}

bool GetMouseClickUp(int buttonNum)
{
	return (mouseState.rgbButtons[buttonNum] & 0x80) == 0 && (oldMouseState.rgbButtons[buttonNum] & 0x80) != 0;
}

bool GetMouseClickDown(int buttonNum)
{
	return (mouseState.rgbButtons[buttonNum] & 0x80) != 0 && (oldMouseState.rgbButtons[buttonNum] & 0x80) == 0;
}

Vector3 GetMouseMove() {
	return Vector3(mouseState.lX, mouseState.lY, mouseState.lZ);
}