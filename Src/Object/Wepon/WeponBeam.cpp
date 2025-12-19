#include "../../Utility/MatrixUtility.h"
#include "../../Utility/AsoUtility.h"
#include "../Common/Collider/ColliderSphere.h"
#include "../Common/Collider/ColliderBase.h"
#include "WeponBeam.h"

WeponBeam::WeponBeam(WEPON_TYPE type):
	WeponBase(type)
{
}

WeponBeam::~WeponBeam(void)
{
}

void WeponBeam::Draw(void)
{
	if (!isAlive_)
	{
		return;
	}

	DrawCapsule3D(
		trans_.pos,
		endPos_,
		5.0f,
		30,
		GetColor(0, 255, 0),
		GetColor(255, 255, 255),
		true);

#ifdef _DEBUG

	// 所有しているコライダの描画
	for (const auto& own : ownColliders_)
	{
		own.second->Draw();
	}

#endif
}

void WeponBeam::InitTransform(void)
{
}

void WeponBeam::InitCollider(void)
{
	// 主に地面との衝突で使用する球体コライダ
	ColliderSphere* colliderSphere = new ColliderSphere(
		weponTag_,
		&trans_,
		AsoUtility::VECTOR_ZERO,
		COL_CAPSULE_SPHERE
	);
	ownColliders_.emplace(
		static_cast<int>(ColliderBase::SHAPE::SPHERE), colliderSphere);
}

void WeponBeam::InitPost(void)
{
	speed_ = DEFAULT_SPEED;
	bemelong_ = 0.0f;
	endPos_ = trans_.pos;
}

void WeponBeam::Move(void)
{ 
	VECTOR  pos = VSub(trans_.pos, playPos_);
	float diff = VSize(pos);
	if (diff >= MAX_BEAM_DIFF)
	{
		isAlive_ = false;
	}

	trans_.pos = VAdd(trans_.pos, VScale(trans_.moveDir, speed_));
	if (bemelong_ > MAX_BEAM_LENGTH)
	{
		endPos_ = VAdd(endPos_, VScale(trans_.moveDir, speed_));
	}
	else
	{
		bemelong_+= speed_;
	}

}
