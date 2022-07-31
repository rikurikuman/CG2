#pragma once
#include "Collision.h"
#include "Vector3.h"

class Raycast;

class SphereCollision : public Collision
{
public:
	Vector3 pos;
	float r;

	SphereCollision(Vector3 pos, float r) : pos(pos), r(r) {}

	/// <summary>
	/// 他Collisionとの当たり判定を取る
	/// 他Collisionの型は問わないが、
	/// その型との当たり判定が未定義なら例外がスローされる
	/// </summary>
	/// <param name="col">当たり判定を行う相手のCollisionの参照</param>
	/// <returns>当たっているかどうか</returns>
	virtual bool Collide(Collision& col) const override;

	/// <summary>
	/// 球との当たり判定を取る
	/// </summary>
	/// <param name="col">当たり判定を行う相手のSphereCollision</param>
	/// <returns>当たっているかどうか</returns>
	bool Collide(const SphereCollision& col) const;

	/// <summary>
	/// Rayとの当たり判定を取る
	/// </summary>
	/// <param name="col">当たり判定を行う相手のRay</param>
	/// <returns>当たっているかどうか</returns>
	bool Collide(const Raycast& col) const;
};

