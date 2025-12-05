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
	for (auto& colParam : colParam_)
	{
		//所持している全コライダの削除
		colParam.collider_->Kill();
	}
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

void ObjectBase::OnHit(const std::weak_ptr<Collider> _hitCol)
{
}

void ObjectBase::MakeCollider(const std::set<Collider::TAG> _tag, std::unique_ptr<Geometry> _geometry, const std::set<Collider::TAG> _notHitTags)
{
	//当たり判定情報
	ColParam colParam;

	//形状情報の挿入
	colParam.geometry_ = std::move(_geometry);

	//情報を使ってコライダの作成
	colParam.collider_ = std::make_shared<Collider>(*this, _tag, *colParam.geometry_, _notHitTags);

	//コライダを管理マネージャーに追加
	CollisionManager::GetInstance().AddCollider(colParam.collider_);

	//配列にセット
	colParam_.push_back(std::move(colParam));
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
