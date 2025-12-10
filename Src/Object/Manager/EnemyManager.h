#pragma once
#include <vector>
#include <memory>
#include <DxLib.h>

class EnemyBase;
class ColliderBase;

class EnemyManager
{
public:

	// 敵の出現間隔
	static constexpr int SPAWN_INTERVAL_1 = 250;
	static constexpr int SPAWN_INTERVAL_2 = 300;
	static constexpr int SPAWN_ENEMY = 5;

	// コンストラクタ
	EnemyManager(void);
	// デストラクタ
	~EnemyManager(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void DrawHp(void);
	void Release(void);

	// 衝突対象となるコライダを登録
	void AddHitCollider(const ColliderBase* hitCollider);

	// エネミーの取得
	const std::vector<std::shared_ptr<EnemyBase>>& GetEnemys(void) const;

	//クリアに切り替え
	bool IsClear;

private:

	// エネミー
	std::vector<std::shared_ptr<EnemyBase>> enemys_;

	// 出現間隔
	int cntSpawn_;
	int cntEnemy_;

};

