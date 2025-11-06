#pragma once
#include <DxLib.h>
#include <vector>
#include <memory>
#include <algorithm>

class Transform;

class CollisinManager
{
public:
	// 当たり判定タイプ
	enum class COLLISION_TYPE
	{
		SPHERE,      //球体
		BOX,         //直方体
		MESH,        //メッシュ
		SPHERE_PUSH, //球体(押し出し)
		BOX_PUSH,    //直方体(押し出し)
		MESH_PUSH,   //メッシュ(押し出し
	};

	// タグタイプ
	enum class TAG_TYPE
	{
		NONE,        //無し
		PLAYER,      //プレイヤー
		ENEMY,       //エネミー
		WALL,        //壁
		PLAYER_WEPON,//プレイヤー武器
		ENEMY_WEPON,//エネミー武器
		CAMERA,      //カメラ
		GROUND,      //地面
	};

	//ヒットタイプ
	enum class HIT_TYPE
	{
		OBJECT_HIT,
		STAGE_HIT,
		PLAYER_WEPON_HIT,
		ENEMY_WEPON_HIT
	};

	// 当たり判定情報
	struct CollisionObject
	{
		std::shared_ptr<void> owner;    //所持者
		VECTOR posPtr;   //座標
		float radius = 0.0f;            //半径(球体)
		VECTOR min = VGet(0, 0, 0);     //最小座標(直方体)
		VECTOR max = VGet(0, 0, 0);     //最大座標(直方体)
		int modelId = -1;               //モデルID(メッシュ)
		COLLISION_TYPE type;            //当たり判定タイプ
		bool pushEnabled = false;       //押し出し判定
		TAG_TYPE tag = TAG_TYPE::NONE;  // タグ
		VECTOR center = VGet(0, 0, 0);  //中心座標(メッシュ)
		float radiusBound = 0.0f;       //境界半径(メッシュ)
	};

	// インスタンスの生成
	static void CreateInstance(void);

	// シングルトン取得
	static CollisinManager& GetInstance(void);

	// インスタンスの破棄
	static void Destroy(void);

	// 初期化
	void Init(void);

	// 球の登録
	void RegisterSphere(std::shared_ptr<void> owner, VECTOR pos, float radius, TAG_TYPE tag, bool push = false);

	// BOXの登録
	void RegisterBox(std::shared_ptr<void> owner, VECTOR pos, VECTOR min, VECTOR max, TAG_TYPE tag, bool push = false);

	// メッシュの登録
	void RegisterMesh(std::shared_ptr<void> owner, int modelId, TAG_TYPE tag, bool push = false);

	// 地面用のメッシュ登録
	void RegisterMeshTile(std::shared_ptr<void> owner, int modelId, VECTOR pos, float tileSize);

	// 全削除
	void Clear(void);

	// 全オブジェクトの当たり判定・押し出し処理
	void Update(void);

	// タグ同士で判定するかをチェック
	bool CanCollide(TAG_TYPE tagA, TAG_TYPE tagB) const;

	const std::shared_ptr<HIT_TYPE> GetHitType(void) const { return hitType_; }

private:

	// コンストラクタ
	CollisinManager(void) = default;

	// デストラクタ
	~CollisinManager(void) = default;

	//シングルトンインスタンスのコピー禁止
	static CollisinManager* instance_;

	//全ての当たり判定オブジェクト
	std::vector<std::shared_ptr<CollisionObject>> objects_;

	//ヒット種類
	std::shared_ptr<HIT_TYPE> hitType_;

};

