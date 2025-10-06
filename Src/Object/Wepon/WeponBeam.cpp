#include "../../Utility/MatrixUtility.h"
#include "../../Utility/AsoUtility.h"
#include "WeponBeam.h"

WeponBeam::WeponBeam(void)
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
	VECTOR endPos = VAdd(trans_.pos, VScale(trans_.moveDir, bemelong_));

	DrawCapsule3D(
		trans_.pos,      // 開始点
		endPos,          // 終点（方向を考慮）
		5.0f,
		8,
		GetColor(0, 255, 0),
		GetColor(255, 255, 255),
		FALSE);
}

void WeponBeam::Release(void)
{
}

void WeponBeam::Use(VECTOR pos, VECTOR dir)
{
	trans_.pos = VAdd(pos, trans_.localPos);
	trans_.moveDir = VNorm(dir);
	isAlive_ = true;
	bemelong_ = 0.0f;
	
}

void WeponBeam::Load(void)
{
}

void WeponBeam::SetParam(void)
{
	trans_.localPos = LOCAL_POS;
	speed_ = DEFAULT_SPEED;
}

void WeponBeam::Move(void)
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
}
