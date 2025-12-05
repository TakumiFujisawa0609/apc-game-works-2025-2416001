#include "../ObjectBase.h"
#include "Geometry/Geometry.h"
#include "Collider.h"

Collider::Collider(ObjectBase& _parent, const std::set<TAG> _tags, Geometry& _geometry, const std::set<TAG> _notHitTags) :
	parent_(_parent),
	tags_(_tags),
	geometry_(_geometry),
	notHitTags_(_notHitTags)
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
