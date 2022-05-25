#pragma once
#include <dinput.h>
#include "Vector3.h"
#define DIRECTINPUT_VERSION 0x0800

void InitInput();
void UpdateInput();

bool GetKey(unsigned char key);
bool GetKeyUp(unsigned char key);
bool GetKeyDown(unsigned char key);

bool GetMouseClick(int buttonNum);
bool GetMouseClickUp(int buttonNum);
bool GetMouseClickDown(int buttonNum);

Vector3 GetMouseMove();