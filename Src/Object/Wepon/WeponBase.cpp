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

void WeponBase::Init(void){}

void WeponBase::Init(VECTOR pos, VECTOR dir)
{
	// 画像やモデルなどのロード
	Load();

	// パラメータ設定
	SetParam();

	trans_.pos = VAdd(pos, trans_.localPos);
	trans_.moveDir = VNorm(dir);
	isAlive_ = true;

	//// 大きさの設定
	//MV1SetScale(modelId_, scl_);
	//// 回転の設定
	//MV1SetRotationXYZ(modelId_, rot_);
	//// 位置の設定
	//MV1SetPosition(modelId_, pos_);
}

void WeponBase::Init(VECTOR pos, VECTOR dir, VECTOR trgPos)
{
	// 画像やモデルなどのロード
	Load();

	// パラメータ設定
	SetParam();

	trans_.pos = VAdd(pos, trans_.localPos);
	trans_.moveDir = VNorm(dir);
	targetPos_ = trgPos;
	isAlive_ = true;

	//// 大きさの設定
	//MV1SetScale(modelId_, scl_);
	//// 回転の設定
	//MV1SetRotationXYZ(modelId_, rot_);
	//// 位置の設定
	//MV1SetPosition(modelId_, pos_);
}

void WeponBase::Update(void)
{
	if (!isAlive_)
	{
		return;
	}

	// 移動処理
	Move();
}

bool WeponBase::IsAlive(void)
{
	return false;
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
