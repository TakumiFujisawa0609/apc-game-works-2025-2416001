#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
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
	for (auto& colParam : colParam_)
	{
		//所持している全コライダの削除
		colParam.collider_->Kill();
	}
}

void ObjectBase::OnHit(const std::weak_ptr<Collider> hitCol)
{
}

void ObjectBase::MakeCollider(const std::set<Collider::TAG> _tag, std::unique_ptr<Geometry> _geometry, const std::set<Collider::TAG> _notHitTags)
{
	//当たり判定情報
	ColParam colParam;

	//形状情報の挿入
	colParam.geometry_ = std::move(_geometry);

	//情報を使ってコライダの作成
	colParam.collider_ = std::make_shared<Collider>(*this, _tag, *colParam.geometry_, _notHitTags, trans_.pos, trans_.Radius_);

	//コライダを管理マネージャーに追加
	CollisionManager::GetInstance().AddCollider(colParam.collider_);

	//配列にセット
	colParam_.push_back(std::move(colParam));
}
