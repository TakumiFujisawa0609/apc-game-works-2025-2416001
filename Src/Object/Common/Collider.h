#pragma once
#include<set>
#include"Geometry/Geometry.h"

class ObjectBase;
class Transform;

class Collider
{

public :
	
	enum class TAG
	{
		//モデル
		PLAYER,
		ENEMY,

		//武器タグ
		PLAYER_WEPON,
		ENEMY_WEPON,

		//ステージ
		STAGE,
	};

	struct HIT_OBJECT
	{
		//親
		ObjectBase& parent;

		// 衝突用タグ
		std::set<TAG> tags;

		//当たり判定の形状
		Geometry& geometry;

		// 衝突しないタグ
		std::set<TAG> notHitTags;

		VECTOR& pos;

		float& radus;
	};

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_tags">自身の衝突用タグ</param>
	/// <param name="_geometry">当たり判定の形状</param>
	/// <param name="_notHitTags">衝突させないタグ</param>
	Collider(ObjectBase& parent, const std::set<TAG> tags, Geometry& geometry, const std::set<TAG> notHitTags, VECTOR& pos, float& radus);

	// デストラクタ
	~Collider(void);

	inline const HIT_OBJECT GetHitObject(void)const { return hitObj_; }

	//衝突用タグの取得
	inline const std::set<TAG> GetTags(void)const { return  hitObj_.tags; }

	//衝突用ベクトルを取得
	inline VECTOR& GetHitMovePos(void)const { return  hitObj_.pos; }

	//衝突用半径を取得
	inline float& GetHitRadius(void)const { return hitObj_.radus; }

	//当たり判定の形状を取得
	inline Geometry& GetGeometry(void)const { return  hitObj_.geometry; }

	//衝突させないタグの取得
	inline const std::set<TAG> GetNotHitTags(void)const { return  hitObj_.notHitTags; }

	//親を取得
	inline const ObjectBase& GetParent(void)const { return  hitObj_.parent; }

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

	HIT_OBJECT hitObj_;

	//当たったかの判定
	bool isHit_;

	//終了判定
	bool isDead_;
};
