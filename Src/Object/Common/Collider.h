#pragma once
#include<set>
#include"Geometry/Geometry.h"

class ObjetBase;

class Collider
{

public :
	
	enum class TAG
	{
		//モデル
		PLAYER,
		ENEMY,

		//武器タグ
		PLAYER_BEAM,
		PLAYER_MISSILE,
		ENEMY_BEAM,
		ENEMY_MISSILE,

		//ステージ
		STAGE,
	};

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_tags">自身の衝突用タグ</param>
	/// <param name="_geometry">当たり判定の形状</param>
	/// <param name="_notHitTags">衝突させないタグ</param>
	Collider(ObjectBase& parent, const std::set<TAG> tags, Geometry& geometry, const std::set<TAG> notHitTags);

	// デストラクタ
	~Collider(void);

	//衝突用タグの取得
	inline const std::set<TAG> GetTags(void)const { return tags_; }

	//当たり判定の形状を取得
	inline Geometry& GetGeometry(void)const { return geometry_; }

	//衝突させないタグの取得
	inline const std::set<TAG> GetNotHitTags(void)const { return notHitTags_; }

	//親を取得
	inline const ObjectBase& GetParent(void)const { return parent_; }

	//当たったかの判定の取得
	inline const bool IsHit(void)const { return isHit_; }

	//当たっていない
	inline void NotHit(void) { isHit_ = false; }

	//終了判定の取得
	inline const bool IsDead(void)const { return isDead_; }

	//終了処理(所持者の解放時に置く)
	inline void Kill(void) { isDead_ = true; }

	/// <summary>
	/// 当たった時の処理
	/// </summary>
	/// <param name="_collider">相手のコライダ</param>
	void OnHit(const std::weak_ptr<Collider> _collider);

private:

	//親
	ObjectBase& parent_;

	// 衝突用タグ
	std::set<TAG> tags_;

	// 衝突しないタグ
	std::set<TAG> notHitTags_;

	//当たり判定の形状
	Geometry& geometry_;

	//当たったかの判定
	bool isHit_;

	//終了判定
	bool isDead_;
};
