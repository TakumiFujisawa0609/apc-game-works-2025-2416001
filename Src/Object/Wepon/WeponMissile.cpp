#include "../Common/Collider/ColliderSphere.h"
#include "../Common/Collider/ColliderBase.h"
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

	DrawCapsule3D(
		trans_.pos,
		endPos_,
		5.0f,
		8,
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

void WeponMissile::InitTransform(void)
{
}

void WeponMissile::InitCollider(void)
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

void WeponMissile::InitPost(void)
{
	speed_ = DEFAULT_SPEED;
	long_ = 0.0f;
	endPos_ = trans_.pos;
	jumpPow_ = JUMP_POW;
	homingCnt_ = 0.0f;
}

void WeponMissile::Move(void)
{
    if (homingCnt_ < HOMINGSTATE_CNT)
    {
        homingCnt_++;
    }
    else
    {
        VECTOR toTarget = VSub(targetPos_, trans_.pos);
        float distance = VSize(toTarget);

        if (distance >= 0.01f)
        {
            VECTOR targetDir = VNorm(toTarget);
            float power = 0.3f;
            trans_.moveDir = VNorm(
				VAdd(VScale(trans_.moveDir, 1.0f - power),VScale(targetDir, power)));
        }
    }

    trans_.pos = VAdd(trans_.pos, VScale(trans_.moveDir, speed_));
    if (long_ > MAX_BEAM_LENGTH)
    {
        endPos_ = VAdd(endPos_, VScale(trans_.moveDir, speed_));
    }
    else
    {
        long_ += speed_;
    }

    if (trans_.pos.y < 0)
    {
        isAlive_ = false;
    }
}