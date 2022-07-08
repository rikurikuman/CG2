#pragma once
#include <string>
#include <vector>

namespace Util {
	extern bool debugBool;
	constexpr float PI = 3.1415926535897932384626f;

	//DegreeからRadianへ変換する
	float AngleToRadian(float angle);

	//RadianからDegreeへ変換する
	float RadianToAngle(float radian);

    /*
    printfみたいに
    フォーマット指定子を含む文字列(X = %dみたいなの)と
    引数を好きなだけ入れると
    その結果の文字列を返す
    例:
    StringFormat("X = %d, Y = %d", 100, 500); -> "X = 100, Y = 500"
*/
    template<typename ... Args>
    std::string StringFormat(const std::string& format, Args&& ... args) {
        /* フォーマット後の文字数を算出 */
        int str_len = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args) ...);

        /* フォーマット失敗 */
        if (str_len < 0)
        {
            runtime_error("String Formatting Error");
        }

        /* バッファサイズを算出(文字列長 + null文字サイズ) */
        size_t buffer_size = str_len + sizeof(char);

        /* バッファサイズ分メモリ確保 */
        std::unique_ptr<char[]> buffer(new char[buffer_size]);

        /* 文字列のフォーマット */
        std::snprintf(buffer.get(), buffer_size, format.c_str(), args ...);

        /* 文字列をstd::string型に変換して出力 */
        return std::string(buffer.get(), buffer.get() + str_len);
    }

    /*
        Javaでいうinstanceofとほぼ一緒！
        instanceof<int>(a)みたいな書き方でどうぞ
    */
    template <typename check, typename NANIYATU>
    inline bool instanceof(const NANIYATU& a)
    {
        return dynamic_cast<const check*>(&a) != 0;
    }

    /*
        std::stringにsplitがないので
        自前でsplitできるやつ
    */
    std::vector<std::string> StringSplit(std::string s, std::string separator);

    //指定したintがminからmaxの間に収まっていればそのまま、
    //収まっていなければ収めて返す
    int Clamp(int i, int min, int max);

    //指定したfloatがminからmaxの間に収まっていればそのまま、
    //収まっていなければ収めて返す
    float Clamp(float f, float min, float max);

    //指定したdoubleがminからmaxの間に収まっていればそのまま、
    //収まっていなければ収めて返す
    double Clamp(double d, double min, double max);

    //std::vectorにsearchが含まれているか調べる
    template <class T>
    bool Contains(std::vector<T> vector, T search) {
        for (auto itr = vector.begin(); itr != vector.end(); itr++) {
            if (*itr == search) {
                return true;
            }
        }
        return false;
    }

    //std::vectorにsearchがいるならそのインデックスを得る
    //ないなら-1
    template <class T>
    int IndexOf(std::vector<T> vector, T search) {
        auto itr = std::find(vector.begin(), vector.end(), search);
        if (itr == vector.end()) {
            return -1;
        }
        return std::distance(vector.begin(), itr);
    }

    //stringをwstringに変換する（MultiByteToWideChar)
    std::wstring ConvertStringToWString(std::string str);
}
