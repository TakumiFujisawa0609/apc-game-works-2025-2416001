#include "WeponMissile.h"

WeponMissile::WeponMissile(void)
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
	VECTOR endPos = VAdd(trans_.pos, VScale(trans_.moveDir, bemelong_));



	DrawCapsule3D(
		trans_.pos,      // 開始点
		endPos,          // 終点（方向を考慮）
		2.5f,
		8,
		GetColor(0, 255, 0),
		GetColor(255, 255, 255),
		FALSE);
}

void WeponMissile::Release(void)
{
}

void WeponMissile::Use(VECTOR pos, VECTOR dir)
{
	trans_.pos = VAdd(pos, trans_.localPos);
	trans_.moveDir = VNorm(dir);
	isAlive_ = true;
	bemelong_ = 0.0f;
	jumpPow_ = JUMP_POW;
}

void WeponMissile::Load(void)
{
}

void WeponMissile::SetParam(void)
{
	trans_.localPos = LOCAL_POS;
	speed_ = DEFAULT_SPEED;
}

void WeponMissile::Move(void)
{
	if (bemelong_ > MAX_BEAM_LENGTH)
	{
		bemelong_ = MAX_BEAM_LENGTH;


		trans_.pos = VAdd(trans_.pos, VScale(trans_.moveDir, speed_));
	}
	else
	{
		bemelong_ += speed_;
	}

	// 重力(加速度を速度に加算していく)
	jumpPow_ -= GRAVITY;
	trans_.pos.y += jumpPow_;

	if (trans_.pos.y < 0)
	{
		isAlive_ = false;
	}

}
