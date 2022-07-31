#pragma once
class Collision
{
public:
	virtual ~Collision() {};

	/// <summary>
	/// 他Collisionとの当たり判定を取る
	/// 他Collisionの型は問わないが、
	/// その型との当たり判定が未定義なら例外がスローされる
	/// </summary>
	/// <param name="col">当たり判定を行う相手のCollision</param>
	/// <returns>当たっているかどうか</returns>
	virtual bool Collide(Collision& col) const = 0;
};

