#pragma once
#include <d3d12.h>
#include "Vector2.h"
#include "Vector3.h"

class Vertex
{
public:
	Vector3 pos;
	Vector3 normal;
	Vector2 uv;

	/// <summary>
	/// �@���x�N�g�����v�Z
	/// </summary>
	/// <param name="list">�v�Z���钸�_�Q</param>
	/// <param name="indexlist">�C���f�b�N�X���X�g�i�O�p�`�A�S�Ď��v���ł��邱�Ɓj</param>
	/// <param name="indexcount">�C���f�b�N�X�̐�</param>
	static void CalcNormalVec(Vertex list[], unsigned int indexlist[], unsigned int indexcount);
};

