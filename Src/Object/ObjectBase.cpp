#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Utility/MatrixUtility.h"
#include "ObjectBase.h"

ObjectBase::ObjectBase(void)
	: resMng_(ResourceManager::GetInstance()),
	scnMng_(SceneManager::GetInstance()),
	inpMng_(InputManager::GetInstance())
{
	isAlive_ = true;
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

	// 衝突判定の初期化
	InitCollider();

	// アニメーションの初期化
	InitAnimation();

	// 初期化後の個別処理
	InitPost();
}

void ObjectBase::Draw(void)
{
	if (!isAlive_)
	{
		return;
	}

#ifdef _DEBUG
	// 所有しているコライダの描画
	for (const auto& own : ownColliders_)
	{
		own.second->Draw();
	}
#endif // _DEBUG

	if (trans_.modelId != -1)
	{
		MV1DrawModel(trans_.modelId);
	}
}

void ObjectBase::Release(void)
{
	// 自身のコライダ解放
	for (auto& own : ownColliders_)
	{
		delete own.second;
	}

	if (trans_.modelId != -1)
	{
		MV1DeleteModel(trans_.modelId);
	}
}

const ColliderBase* ObjectBase::GetOwnCollider(int key) const
{
	if (ownColliders_.count(key) == 0)
	{
		return nullptr;
	}
	return ownColliders_.at(key);
}

void ObjectBase::AddHitCollider(const ColliderBase* hitCollider)
{
	for (const auto& c : hitColliders_)
	{
		if (c == hitCollider)
		{
			return;
		}
	}
	hitColliders_.emplace_back(hitCollider);

}

void ObjectBase::ClearHitCollider(void)
{
	hitColliders_.clear();
}
