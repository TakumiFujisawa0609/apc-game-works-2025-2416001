#include "../../Utility/MatrixUtility.h"
#include "../../Utility/AsoUtility.h"
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

	// ビームの終点を方向ベクトルを使って計算
	statePos_ = VAdd(trans_.pos, VScale(trans_.moveDir, bemelong_));

	DrawCapsule3D(
		trans_.pos,      // 開始点
		statePos_,          // 終点（方向を考慮）
		5.0f,
		30,
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

void WeponBeam::InitTransform(void)
{
	trans_.localPos = LOCAL_POS;
	trans_.Radius_ = DEFALUT_RADIUS;
}

void WeponBeam::InitCollider(void)
{
}

void WeponBeam::InitPost(void)
{
	speed_ = DEFAULT_SPEED;
	bemelong_ = 0.0f;
	damage_ = DAMAGE;
}

void WeponBeam::Move(void)
{
	VECTOR  pos = VSub(statePos_, playPos_);
	float diff = VSize(pos);
	if (diff >= MAX_BEAM_DIFF)
	{
		bemelong_ -= speed_;
		if (bemelong_ >= 0) {
			isAlive_ = false;
		}
	}

	if (bemelong_ > MAX_BEAM_LENGTH)
	{
		bemelong_ = MAX_BEAM_LENGTH;

		trans_.pos = VAdd(trans_.pos, VScale(trans_.moveDir, speed_));
	}
	else
	{
		bemelong_ += speed_;
	}
}
