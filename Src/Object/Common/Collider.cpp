#include "../ObjectBase.h"
#include "Geometry/Geometry.h"
#include "Collider.h"


Collider::Collider(ObjectBase& parent, const std::set<TAG> tags, Geometry& geometry, const std::set<TAG> notHitTags):
	parent_(parent),
	tags_(tags),
	geometry_(geometry),
	notHitTags_(notHitTags)
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
	parent_.OnHit(_collider);
}