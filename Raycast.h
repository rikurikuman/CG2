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
	/// 他Collisionとの当たり判定を取る
	/// 他Collisionの型は問わないが、
	/// その型との当たり判定が未定義なら例外がスローされる
	/// </summary>
	/// <param name="col">当たり判定を行う相手のCollisionの参照</param>
	/// <returns>当たっているかどうか</returns>
	virtual bool Collide(Collision& col) const;

	/// <summary>
	/// 球との当たり判定を取る
	/// </summary>
	/// <param name="col">当たり判定を行う相手のSphereCollision</param>
	/// <returns>当たっているかどうか</returns>
	bool Collide(SphereCollision& col) const;
};

