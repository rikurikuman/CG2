#pragma once
#include "ViewProjection.h"
#include "Vector2.h"
#include "RWindow.h"

class DebugCamera
{
private:
	Vector2 angle = {90, 0};

public:
	ViewProjection viewProjection;
	float moveSpeed = 0.2f;
	float sensitivity = 10.0f;

	DebugCamera(Vector3 pos, float moveSpeed = 0.2f, float sensitivity = 10.0f) : moveSpeed(moveSpeed), sensitivity(sensitivity) {
		viewProjection.eye = pos;
		viewProjection.aspect = (float)RWindow::GetWidth() / RWindow::GetHeight();
	}

	void Update();
};

