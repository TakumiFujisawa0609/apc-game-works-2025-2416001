#include "../../Manager/ResourceManager.h"
#include "../../Utility/MatrixUtility.h"
#include "../Common/Transform.h"
#include "WeponBase.h"

WeponBase::WeponBase(void)
{
}

WeponBase::~WeponBase(void)
{
}

void WeponBase::Init(void)
{
	// 初期は生存していない
	isAlive_ = false;
	// 画像やモデルなどのロード
	Load();
	// パラメータ設定
	SetParam();
	// モデルの大きさ設定
	MV1SetScale(trans_.modelId, trans_.scl);
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

WeponBase::WAPON_TYPE WeponBase::GetType(void)
{
	return type_;
}

void WeponBase::Move(void)
{
	// 移動処理(一方方向)
	trans_.pos = VAdd(trans_.pos, VScale(trans_.moveDir, speed_));
}
