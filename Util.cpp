#include "Util.h"

bool Util::debugBool = false;
const float Util::PI = 3.1415926535897932384626f;

float Util::AngleToRadian(float angle)
{
	return angle * (PI / 180);
}

float Util::RadianToAngle(float radian)
{
	return radian * (180 / PI);
}