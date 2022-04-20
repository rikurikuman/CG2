#pragma once
#include <dinput.h>

void InitInput();
void UpdateInput();

bool GetKey(unsigned char key);
bool GetKeyUp(unsigned char key);
bool GetKeyDown(unsigned char key);