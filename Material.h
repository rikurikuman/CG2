#pragma once
#include "Color.h"
#include "Vector3.h"
#include <string>
#include "Texture.h"

struct MaterialBuffer {
	Color color;
	Vector3 ambient;
	Vector3 diffuse;
	Vector3 specular;
};

class Material
{
public:
	std::string name; //おなまえドットコム
	TextureHandle texture; //テクスチャ
	Color color = {1, 1, 1, 1}; //色(RGBA)
	Vector3 ambient = {1, 1, 1}; //環境光
	Vector3 diffuse = {0, 0, 0}; //拡散反射光
	Vector3 specular = {0, 0, 0}; //鏡面反射光

	/// <summary>
	/// 定数バッファへ転送
	/// </summary>
	/// <param name="target">対象のバッファへのポインタ</param>
	void Transfer(MaterialBuffer* target);

	//読み込んでくる
	static Material Load(std::string filepath, std::string filename);
};