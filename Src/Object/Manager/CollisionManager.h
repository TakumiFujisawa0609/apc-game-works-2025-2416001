#pragma once
#include<vector>
#include<memory>
#include<map>
#include<functional>
#include"../Common/Collider.h"

class Geometry;

class CollisionManager
{
public:
		//当たり判定をする範囲
	static constexpr float HIT_RANGE_NORMAL = 1000.0f;	//通常の当たり判定距離

	//更新用
	static constexpr int COL_UPDATE_FRAME = 0;		//更新フレーム
	
	//インスタンス生成
	static void CreateInstance(void);

	//インスタンスの取得
	static CollisionManager& GetInstance(void) { return *instance_; }

	//コライダの追加
	void AddCollider(const std::shared_ptr<Collider> _collider);

	//必要なくなったコライダの削除(更新の最後に置く)
	void Sweep(void);

	//更新
	void Update(void);

	//削除
	void Destroy(void);

	/// <summary>
	/// そのタグがプレイヤーかどうか調べる
	/// </summary>
	/// <param name="_tag">調べるタグ</param>
	/// <returns>true:プレイヤーだった</returns>
	const bool IsPlayer(const Collider::TAG tag)const;

	/// <summary>
	/// そのタグがプレイヤ武器かどうか調べる
	/// </summary>
	/// <param name="_tag">調べるタグ</param>
	/// <returns>true:プレイヤー武器だった</returns>
	const bool IsPlayerWepon(const Collider::TAG tag)const;

	/// <summary>
	/// そのタグがエネミーかどうか調べる
	/// </summary>
	/// <param name="_tag">調べるタグ</param>
	/// <returns>true:エネミーだった</returns>
	const bool IsEnemy(const Collider::TAG tag)const;

	/// <summary>
	/// そのタグがエネミー武器かどうか調べる
	/// </summary>
	/// <param name="_tag">調べるタグ</param>
	/// <returns>true:エネミー武器だった</returns>
	const bool IsEnemyWepon(const Collider::TAG tag)const;

	/// <summary>
	/// そのタグステージかどうか調べる
	/// </summary>
	/// <param name="_tag">調べるタグ</param>
	/// <returns>true:ステージだった</returns>
	const bool IsStage(const Collider::TAG tag)const;

private:

	//静的インスタンス
	static CollisionManager* instance_;

	//当たり判定格納
	std::vector<std::shared_ptr<Collider>>colliders_;

	//当たり判定距離の二乗
	std::map<Collider::TAG, float> hitRange_;

	//当たり判定するフレーム
	int updateFrame_;

	//コンストラクタ
	CollisionManager(void);

	//コピーの禁止
	CollisionManager(const CollisionManager& _copy) = delete;
	CollisionManager& operator= (const CollisionManager& _copy) = delete;

	//デストラクタ
	~CollisionManager(void);

	/// <summary>
	/// 当たり判定距離内にいるか
	/// </summary>
	/// <param name="_col1">1つ目のコライダ</param>
	/// <param name="_col2">2つ目のコライダ</param>
	/// <returns>true:範囲内</returns>
	const bool IsWithInHitRange(const std::weak_ptr<Collider> _col1, const std::weak_ptr<Collider> _col2)const;

	/// <summary>
	/// 当たり判定をするか(全部当てはまったらtrue)
	/// </summary>
	/// <param name="_col1">1つ目のコライダ番号</param>
	/// <param name="_col2">2つ目のコライダ番号</param>
	/// <returns>true:当たり判定をする</returns>
	const bool JudgeIsCollision(const int _col1Num, const int _col2Num)const;

	/// <summary>
	/// タグごとでの当たり判定するかどうか
	/// </summary>
	/// <param name="_tag1">1つ目のタグ</param>
	/// <param name="_tag2">2つ目のタグ</param>
	/// <returns>true:当たり判定をする</returns>
	const bool JudgeIsColTag(const Collider::TAG _tag1, const Collider::TAG _tag2)const;

	/// <summary>
	/// 当たり判定
	/// </summary>
	/// <param name="_col1">1つ目のコライダ</param>
	/// <param name="_col2">2つ目のコライダ</param>
	/// <returns>true:当たった</returns>
	bool IsCollision(const std::weak_ptr<Collider> _col1, const std::weak_ptr<Collider> _col2);

};

