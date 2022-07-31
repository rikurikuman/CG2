#pragma once
#include "Collision.h"
#include "Vector3.h"

class SphereCollision;

class Raycast : public Collision
{
public:
	Vector3 pos;
	Vector3 direction;
	float distance;

	Raycast(Vector3 pos, Vector3 direction, float distance = -1) : pos(pos), direction(direction), distance(distance) {}

	/// <summary>
	/// ��Collision�Ƃ̓����蔻������
	/// ��Collision�̌^�͖��Ȃ����A
	/// ���̌^�Ƃ̓����蔻�肪����`�Ȃ��O���X���[�����
	/// </summary>
	/// <param name="col">�����蔻����s�������Collision�̎Q��</param>
	/// <returns>�������Ă��邩�ǂ���</returns>
	virtual bool Collide(Collision& col) const;

	/// <summary>
	/// ���Ƃ̓����蔻������
	/// </summary>
	/// <param name="col">�����蔻����s�������SphereCollision</param>
	/// <returns>�������Ă��邩�ǂ���</returns>
	bool Collide(SphereCollision& col) const;
};
