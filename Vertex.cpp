#include "Vertex.h"
#include <cassert>

using namespace DirectX;

void Vertex::CalcNormalVec(Vertex list[], uint16_t indexlist[], uint16_t indexcount)
{
	assert(indexcount % 3 == 0);
	for (int i = 0; i < indexcount / 3; i++) {
		uint16_t index0 = indexlist[i * 3 + 0];
		uint16_t index1 = indexlist[i * 3 + 1];
		uint16_t index2 = indexlist[i * 3 + 2];

		XMVECTOR p0 = XMLoadFloat3(&list[index0].pos);
		XMVECTOR p1 = XMLoadFloat3(&list[index1].pos);
		XMVECTOR p2 = XMLoadFloat3(&list[index2].pos);

		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);

		XMVECTOR normal = XMVector3Cross(v1, v2);

		normal = XMVector3Normalize(normal);

		XMStoreFloat3(&list[index0].normal, normal);
		XMStoreFloat3(&list[index1].normal, normal);
		XMStoreFloat3(&list[index2].normal, normal);
	}
}
