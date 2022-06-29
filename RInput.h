#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl.h>
#include "Vector3.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class RInput {
public:
	//入力情報管理機能初期化
	static void Init();
	static RInput* GetInstance();

	//入力情報の更新
	static void Update();

	//キーが押されているか
	static bool GetKey(unsigned char key);
	//キーが離れた瞬間か
	static bool GetKeyUp(unsigned char key);
	//キーが押された瞬間か
	static bool GetKeyDown(unsigned char key);

	//マウスの状態を得る
	static inline DIMOUSESTATE2 GetMouseState() {
		return GetInstance()->mouseState;
	}
	//前回のマウスの状態を得る
	static inline DIMOUSESTATE2 GetOldMouseState() {
		return GetInstance()->oldMouseState;
	}

	//マウスクリックされているか
	static bool GetMouseClick(int buttonNum);
	//マウスクリックが離れた瞬間か
	static bool GetMouseClickUp(int buttonNum);
	//マウスクリックがされた瞬間か
	static bool GetMouseClickDown(int buttonNum);

	//マウスの移動量を取得する
	static Vector3 GetMouseMove();

private:
	RInput() {};
	~RInput() = default;
	RInput(const RInput& a) {};
	RInput& operator=(const RInput&) { return *this; }

	Microsoft::WRL::ComPtr<IDirectInput8> directInput = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse = nullptr;
	BYTE keyState[256] = {};
	BYTE oldKeyState[256] = {};
	DIMOUSESTATE2 mouseState = {};
	DIMOUSESTATE2 oldMouseState = {};

	void InitInternal();
};