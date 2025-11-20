#include "../ObjectBase.h"
#include "Geometry/Geometry.h"
#include "../Common/Transform.h"
#include "Collider.h"


Collider::Collider(ObjectBase& parent, const std::set<TAG> tags, Geometry& geometry, const std::set<TAG> notHitTags, VECTOR& pos, float& radus):
	hitObj_{ parent, tags, geometry, notHitTags, pos, radus}
{
	isHit_ = false;
	isDead_ = false;
}

Collider::~Collider(void)
{
}

void Collider::OnHit(const std::weak_ptr<Collider> _collider)
{
	//この当たり判定が当たった
	isHit_ = true;

	//親に相手のコライダを渡す
	hitObj_.parent.OnHit(_collider);
}

void Collider::Clear(void)
{
	hitObj_.parent.ColliderClear();
}
