#pragma once
#include<vector>
#include<memory>
#include"Common/Transform.h"
#include "Common/Collider.h"

class ResourceManager;
class SceneManager;
class InputManager;
class Sphere;
class Collider;

class ObjectBase
{
public:

	//当たり判定情報
	struct ColParam
	{
		std::unique_ptr<Geometry> geometry_;	//形状情報
		std::shared_ptr<Collider> collider_;	//全体の当たり判定情報
	};

	// コンストラクタ
	ObjectBase(void);

	// デストラクタ
	virtual ~ObjectBase(void);

	virtual void Init(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;
	virtual void Release(void) = 0;

	const bool IsDead(void)const { return isDead_; }
	void Kill(void) { isDead_ = true; }

	inline const Transform& GetTransform(void) const { return trans_; }
	Transform& GetTransform(void){ return trans_; }

	//ヒット処理
	virtual void OnHit(const std::weak_ptr<Collider> hitCol);

	void ColliderClear(void);

protected:

	// シングルトン参照
	ResourceManager& resMng_;
	SceneManager& scnMng_;
	InputManager& inpMng_;

	// モデル制御の基本情報
	Transform trans_;

	bool isDead_;

	//当たり判定関係
	std::vector<ColParam> colParam_;

	/// <summary>
	/// 当たり判定作成(形状情報作成後)
	/// </summary>
	/// <param name="_tag">自身の当たり判定タグ</param>
	/// <param name="_Geometry">自身の形状情報</param>
	/// <param name="_notHitTags">衝突させないタグ</param>
	void MakeCollider(const std::set<Collider::TAG> _tag, std::unique_ptr<Geometry> _geometry, const std::set<Collider::TAG> _notHitTags = {});

};

