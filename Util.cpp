#include "Util.h"

using namespace std;

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

vector<string> Util::StringSplit(string s, string separator)
{
	vector<string> result;

	//��؂蕶���̒�����0�Ȃ��؂�킯���Ȃ��̂ł��̂܂ܕԂ�
	if (separator.length() == 0) {
		result.push_back(s);
		return result;
	}

	auto offset = string::size_type(0);
	while (1) {
		//��؂蕶�����o������ʒu���擾
		auto find = s.find(separator, offset);

		//������̏I�_�܂ōs������
		if (find == string::npos) {
			//�c��S�������Ă����܂�
			result.push_back(s.substr(offset));
			break;
		}

		//�����łȂ��Ȃ��؂��ĉ�����
		result.push_back(s.substr(offset, find - offset));

		//�����ς݂̏��܂ŃC�e���[�^��i�߂�
		offset = find + separator.length();
	}

	return result;
}

int Util::Clamp(int i, int min, int max) {
	if (i < min) { return min; }
	if (i > max) { return max; }
	return i;
}

float Util::Clamp(float f, float min, float max) {
	if (f < min) { return min; }
	if (f > max) { return max; }
	return f;
}

double Util::Clamp(double d, double min, double max) {
	if (d < min) { return min; }
	if (d > max) { return max; }
	return d;
}