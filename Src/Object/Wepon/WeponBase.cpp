#include "../../Manager/ResourceManager.h"
#include "../../Utility/MatrixUtility.h"
#include "../Common/Transform.h"
#include "WeponBase.h"

WeponBase::WeponBase(WEPON_TYPE type)
{
	type_ = type;
}

WeponBase::~WeponBase(void)
{
}

void WeponBase::Init(VECTOR pos, VECTOR dir)
{
	ObjectBase::Init();

	playPos_ = pos;
	trans_.pos = VAdd(playPos_, trans_.localPos);
	trans_.moveDir = VNorm(dir);
	isAlive_ = true;
}

void WeponBase::Init(VECTOR pos, VECTOR dir, VECTOR targetPos)
{
	ObjectBase::Init();

	playPos_ = pos;
	trans_.pos = VAdd(playPos_, trans_.localPos);
	trans_.moveDir = VNorm(dir);
	targetPos_ = targetPos;
	isAlive_ = true;

}

void WeponBase::Update(void)
{
	if (!isAlive_)
	{
		return;
	}

	// ˆÚ“®ˆ—
	Move();
}

WeponBase::WEPON_TYPE WeponBase::GetType(void)
{
	return type_;
}

void WeponBase::Move(void)
{
	// ˆÚ“®ˆ—(ˆê•û•ûŒü)
	trans_.pos = VAdd(trans_.pos, VScale(trans_.moveDir, speed_));
}
