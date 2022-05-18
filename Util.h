#pragma once
#include <string>
#include <vector>

namespace Util {
	extern bool debugBool;
	extern const float PI;

	//Degree����Radian�֕ϊ�����
	float AngleToRadian(float angle);

	//Radian����Degree�֕ϊ�����
	float RadianToAngle(float radian);

    /*
    printf�݂�����
    �t�H�[�}�b�g�w��q���܂ޕ�����(X = %d�݂����Ȃ�)��
    �������D���Ȃ���������
    ���̌��ʂ̕������Ԃ�
    ��:
    StringFormat("X = %d, Y = %d", 100, 500); -> "X = 100, Y = 500"
*/
    template<typename ... Args>
    std::string StringFormat(const std::string& format, Args&& ... args) {
        /* �t�H�[�}�b�g��̕��������Z�o */
        int str_len = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args) ...);

        /* �t�H�[�}�b�g���s */
        if (str_len < 0)
        {
            runtime_error("String Formatting Error");
        }

        /* �o�b�t�@�T�C�Y���Z�o(������ + null�����T�C�Y) */
        size_t buffer_size = str_len + sizeof(char);

        /* �o�b�t�@�T�C�Y���������m�� */
        std::unique_ptr<char[]> buffer(new char[buffer_size]);

        /* ������̃t�H�[�}�b�g */
        std::snprintf(buffer.get(), buffer_size, format.c_str(), args ...);

        /* �������std::string�^�ɕϊ����ďo�� */
        return std::string(buffer.get(), buffer.get() + str_len);
    }

    /*
        Java�ł���instanceof�Ƃقڈꏏ�I
        instanceof<int>(a)�݂����ȏ������łǂ���
    */
    template <typename check, typename NANIYATU>
    inline bool instanceof(const NANIYATU& a)
    {
        return dynamic_cast<const check*>(&a) != 0;
    }

    /*
        std::string��split���Ȃ��̂�
        ���O��split�ł�����
    */
    std::vector<std::string> StringSplit(std::string s, std::string separator);

    //�w�肵��int��min����max�̊ԂɎ��܂��Ă���΂��̂܂܁A
    //���܂��Ă��Ȃ���Ύ��߂ĕԂ�
    int Clamp(int i, int min, int max);

    //�w�肵��float��min����max�̊ԂɎ��܂��Ă���΂��̂܂܁A
    //���܂��Ă��Ȃ���Ύ��߂ĕԂ�
    float Clamp(float f, float min, float max);

    //�w�肵��double��min����max�̊ԂɎ��܂��Ă���΂��̂܂܁A
    //���܂��Ă��Ȃ���Ύ��߂ĕԂ�
    double Clamp(double d, double min, double max);
}
