#pragma once
namespace Util {
	extern bool debugBool;
	extern const float PI;

	//DegreeからRadianへ変換する
	float AngleToRadian(float angle);

	//RadianからDegreeへ変換する
	float RadianToAngle(float radian);
}
