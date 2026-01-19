#pragma once
#include<vector>
#include<memory>
#include<map>
#include"Common/Transform.h"
#include "Common/Collider/ColliderBase.h"

class ResourceManager;
class SceneManager;
class InputManager;
class Sphere;
class Collider;

class ObjectBase
{
public:

	// コンストラクタ
	ObjectBase(void);

	// デストラクタ
	virtual ~ObjectBase(void);

	virtual void Init(void);
	virtual void Update(void) = 0;
	virtual void Draw(void);
	virtual void Release(void);

	inline const Transform& GetTransform(void) const { return trans_; }
	Transform& GetTransform(void){ return trans_; }

	// 自身の衝突情報取得
	const std::map<int, ColliderBase*>& GetOwnColliders(void) const
	{
		return ownColliders_;
	}

	// 特定の自身の衝突情報取得
	const ColliderBase* GetOwnCollider(int key) const;

	// 衝突対象となるコライダを登録
	void AddHitCollider(const ColliderBase* hitCollider);

	// 衝突対象となるコライダをクリア
	void ClearHitCollider(void);

	//生存フラグ取得
	const bool GetIsAlive(void)const { return isAlive_; }
	void SetAlive(bool alive) { isAlive_ = alive; }

protected:

	// シングルトン参照
	ResourceManager& resMng_;
	SceneManager& scnMng_;
	InputManager& inpMng_;

	// モデル制御の基本情報
	Transform trans_;

	//生存フラグ
	bool isAlive_;

	// 自身の衝突情報
	std::map<int, ColliderBase*> ownColliders_;

	// 衝突相手の情報
	std::vector<const ColliderBase*> hitColliders_;

	// リソースロード
	virtual void InitLoad(void) = 0;

	// 大きさ、回転、座標の初期化
	virtual void InitTransform(void) = 0;

	// 衝突判定の初期化
	virtual void InitCollider(void) = 0;

	// アニメーションの初期化
	virtual void InitAnimation(void) = 0;

	// 初期化後の個別処理
	virtual void InitPost(void) = 0;

};

