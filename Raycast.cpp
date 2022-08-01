#include "Raycast.h"
#include "SphereCollision.h"
#include "Util.h"

bool Raycast::Collide(Collision& col) const
{
    if (Util::instanceof<SphereCollision>(col)) {
        SphereCollision _col = dynamic_cast<SphereCollision&>(col);
        return Collide(_col);
    }
    throw std::logic_error("未定義の相手との当たり判定を行おうとしました");
    return false;
}

bool Raycast::Collide(SphereCollision& col) const
{
    if (this->distance == 0) return false;

    Vector3 direction = this->direction.GetNormalize();
    Vector3 toSphere = col.pos - this->pos;

    //球から、レイから球へ向かうベクトルに垂線を降ろす
    float posInVec = toSphere.Dot(direction);

    //垂線を降ろした位置がレイより手前なら当たってない
    if (posInVec < 0) return false;

    //レイが有限長で、垂線を降ろした位置がそれより奥なら当たってない
    if (this->distance > 0 && this->distance < posInVec) return false;

    Vector3 hitPos = this->pos + direction * posInVec;
    float hitDistance = (col.pos - hitPos).LengthSq();

    return hitDistance <= powf(col.r, 2);
}