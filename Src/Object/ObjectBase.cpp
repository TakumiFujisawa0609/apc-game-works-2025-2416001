#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Utility/MatrixUtility.h"
#include "Manager/CollisionManager.h"
#include "ObjectBase.h"

ObjectBase::ObjectBase(void)
	: resMng_(ResourceManager::GetInstance()),
	scnMng_(SceneManager::GetInstance()),
	inpMng_(InputManager::GetInstance())
{
}

ObjectBase::~ObjectBase(void)
{
}

void ObjectBase::Init(void)
{
	// リソースロード
	InitLoad();
	// Transform初期化
	InitTransform();
	// 大きさ、回転、座標のモデル設定
	InitTransformPost();
	// アニメーションの初期化
	InitAnimation();
	// 初期化後の個別処理
	InitPost();

	MV1SetPosition(trans_.modelId, trans_.pos);
	MV1SetRotationMatrix(trans_.modelId,
		MatrixUtility::Multiplication(trans_.localRot, trans_.rot));
}

const ColliderBase* ObjectBase::GetOwnCollider(int key) const
{
	if (ownColliders_.count(key) == 0)
	{
		return nullptr;
	}
	return ownColliders_.at(key);
}

void ObjectBase::AddHitCollider(const std::vector<const ColliderBase*> hitColliders)
{
	for (const auto& c : hitColliders_)
	{
		for (const auto& hitCollider : hitColliders)
		{
			if (c == hitCollider)
			{
				return;
			}
		}
	}
	hitColliders_ = hitColliders;
}

void ObjectBase::ClearHitCollider(void)
{
	hitColliders_.clear(); 
}

void ObjectBase::InitTransformPost(void)
{
	// 大きさをモデルに反映
	MV1SetScale(trans_.modelId, trans_.scl);
	// 角度から方向に変換する
	trans_.moveDir = { sinf(trans_.rot.y), 0.0f, cosf(trans_.rot.y) };
	// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
	// 回転行列をモデルに反映
	MV1SetRotationMatrix(trans_.modelId,
		MatrixUtility::Multiplication(trans_.localRot, trans_.rot));
	// 座標をモデルに反映
	MV1SetPosition(trans_.modelId, trans_.pos);
}
