#include "../Common/Collider/ColliderSphere.h"
#include "WeponMissile.h"

WeponMissile::WeponMissile(WEPON_TYPE type):
	WeponBase(type)
{
}

WeponMissile::~WeponMissile(void)
{
}

void WeponMissile::Draw(void)
{
	if (!isAlive_)
	{
		return;
	}

	// ビームの終点を方向ベクトルを使って計算
	statePos_ = VAdd(trans_.pos, VScale(trans_.moveDir, missileLong_));

		DrawCapsule3D(
			trans_.pos,
			statePos_,
			5.0f,
			8,
			GetColor(0, 255, 0),
			GetColor(255, 255, 255),
			FALSE);

#ifdef _DEBUG

	/*DrawFormatString(
		0, 60, GetColor(255, 255, 255),
		"座標：(%.1f,%.1f,%.1f)",
		trans_.pos.x,
		trans_.pos.y,
		trans_.pos.z
	);*/

#endif
}

void WeponMissile::InitTransform(void)
{
	trans_.localPos = LOCAL_POS;
	trans_.Radius_ = DEFALUT_RADIUS;
}

void WeponMissile::InitCollider(void)
{
	// 主に地面との衝突で使用する球体コライダ
	ColliderSphere* colliderSphere = new ColliderSphere(
		ColliderBase::TAG::CAMERA,
		&trans_,
		AsoUtility::VECTOR_ZERO,
		COL_CAPSULE_SPHERE
	);
	ownColliders_.emplace(
		static_cast<int>(COLLIDER_TYPE::SPHERE), colliderSphere);
}

void WeponMissile::InitPost(void)
{
	speed_ = DEFAULT_SPEED;
	missileSpeed_ = BEAM_LENGTH_SPEED;
	missileLong_ = 0.0f;
	jumpPow_ = JUMP_POW;
	damage_ = DAMAGE;
	homingCnt_ = 0.0f;
}

void WeponMissile::Move(void)
{

	if (missileLong_ > MAX_BEAM_LENGTH)
	{
		missileLong_ = MAX_BEAM_LENGTH;
		trans_.pos = VAdd(trans_.pos, VScale(trans_.moveDir, speed_));
	}
	else
	{
		homingCnt_++;
		missileLong_ += missileSpeed_;
	}

	// 重力(加速度を速度に加算していく)
	jumpPow_ -= GRAVITY;
	trans_.pos.y += jumpPow_;
	if (trans_.pos.y < 0)
	{
		isAlive_ = false;
	}

	if (homingCnt_ <= HOMINGSTATE_CNT)
	{
		return;
	}

	homingCnt_ = HOMINGSTATE_CNT;

	VECTOR toTarget = VSub(targetPos_, trans_.pos);
	float distance = VSize(toTarget);

	if (distance >= 0.01f)
	{
		VECTOR targetDir = VNorm(toTarget);
		trans_.moveDir = VNorm(VAdd(trans_.moveDir,targetDir));
	}
}