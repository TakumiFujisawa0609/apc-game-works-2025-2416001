#pragma once
#include <vector>
#include <memory>
#include <DxLib.h>

class EnemyBase;

class EnemyManager
{
public:

	// 敵の出現間隔
	static constexpr int SPAWN_INTERVAL_1 = 250;
	static constexpr int SPAWN_INTERVAL_2 = 300;
	static constexpr int SPAWN_ENEMY = 1;

	// コンストラクタ
	EnemyManager(void);
	// デストラクタ
	~EnemyManager(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	// エネミーの取得
	const std::vector<std::shared_ptr<EnemyBase>>& GetEnemys(void) const;
private:

	// エネミー
	std::vector<std::shared_ptr<EnemyBase>> enemys_;

	// 出現間隔
	int cntSpawn_;
	int cntEnemy_;

};

