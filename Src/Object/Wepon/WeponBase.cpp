#include "../../Manager/ResourceManager.h"
#include "../../Utility/MatrixUtility.h"
#include "../Common/Transform.h"
#include "../Common/Collider/ColliderBase.h"
#include "WeponBase.h"

WeponBase::WeponBase(WEPON_TYPE type)
{
	type_ = type;
}

WeponBase::~WeponBase(void)
{
}

void WeponBase::Init(VECTOR pos, VECTOR dir, ColliderBase::TAG tag)
{
	playPos_ = pos;
	trans_.localPos = LOCAL_POS;
	trans_.pos = VAdd(playPos_, trans_.localPos);
	trans_.moveDir = VNorm(dir);
	weponTag_ = tag;

	ObjectBase::Init();
}

void WeponBase::Init(VECTOR pos, VECTOR dir, VECTOR targetPos, ColliderBase::TAG tag)
{
	playPos_ = pos;
	trans_.localPos = LOCAL_POS;
	trans_.pos = VAdd(playPos_, trans_.localPos);
	trans_.moveDir = VNorm(dir);
	targetPos_ = targetPos;
	weponTag_ = tag;

	ObjectBase::Init();
}

void WeponBase::Update(void)
{
	if (!isAlive_)
	{
		return;
	}

	// 移動前座標を更新
	prevPos_ = trans_.pos;

	// 移動処理
	Move();
}

WeponBase::WEPON_TYPE WeponBase::GetType(void)
{
	return type_;
}

void WeponBase::Move(void)
{
	// 移動処理(一方方向)
	trans_.pos = VAdd(trans_.pos, VScale(trans_.moveDir, speed_));
}
