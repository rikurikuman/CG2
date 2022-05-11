#pragma once
#include <DirectXMath.h>
#include <d3d12.h>

class Vertex
{
public:
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;

	/// <summary>
	/// �@���x�N�g�����v�Z
	/// </summary>
	/// <param name="list">�v�Z���钸�_�Q</param>
	/// <param name="indexlist">�C���f�b�N�X���X�g�i�O�p�`�A�S�Ď��v���ł��邱�Ɓj</param>
	/// <param name="indexcount">�C���f�b�N�X�̐�</param>
	static void CalcNormalVec(Vertex list[], uint16_t indexlist[], uint16_t indexcount);
};

