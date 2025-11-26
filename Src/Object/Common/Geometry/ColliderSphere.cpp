#include "../../Common/Transform.h"
#include "ColliderSphere.h"

ColliderSphere::ColliderSphere(TAG tag, const Transform* follow, const VECTOR& localPos, float radius)
	:
	ColliderBase(SHAPE::SPHERE, tag, follow),
	localPos_(localPos),
	radius_(radius)
{
}

ColliderSphere::~ColliderSphere(void)
{
}

void ColliderSphere::DrawDebug(int color)
{
	//円デバッグ描画
	DrawSphere3D(GetPos(), GetRadius(), DIV_NUM, color, color, true);
}
