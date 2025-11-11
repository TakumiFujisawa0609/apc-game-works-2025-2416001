#include "../Robot/Enemy/EnemyBase.h"
#include "../Robot/Enemy/EnemyBeam.h"
#include "../Robot/Enemy/EnemyMissile.h"
#include "EnemyManager.h"

EnemyManager::EnemyManager(void)
{
}

EnemyManager::~EnemyManager(void)
{
}

void EnemyManager::Init(void)
{
	// 最初から出現させる
	cntSpawn_ = SPAWN_INTERVAL_1 - 10;
	cntEnemy_ = 0;
}

void EnemyManager::Update(void)
{
	// すべての敵の生存状態をチェック
	if (cntEnemy_ >= SPAWN_ENEMY) {
		bool allDead = true;
		for (std::shared_ptr<EnemyBase> enemy : enemys_) {
			if (enemy->IsAlive()) {
				allDead = false;
				break;
			}
		}
		// すべての敵が死んでいたらクリア
		if (allDead) {
			IsClear = true;
		}
	}

	// 出現カウンタ
	cntSpawn_++;

	//敵の数をカウント
	if (cntEnemy_ < SPAWN_ENEMY)
	{

		// 一定間隔でエネミーを出現させる
		if (cntSpawn_ % SPAWN_INTERVAL_1 == 0)
		{
			std::shared_ptr<EnemyBase> enemy = std::make_shared<EnemyBeam>();
			enemy->Init();
			enemys_.emplace_back(enemy);
			++cntEnemy_;
		}

		// 一定間隔でエネミーを出現させる
		if (cntSpawn_ % SPAWN_INTERVAL_2 == 0)
		{
			std::shared_ptr<EnemyBase> enemy = std::make_shared<EnemyMissile>();
			enemy->Init();
			enemys_.emplace_back(enemy);
			++cntEnemy_;
		}
	}
	//エネミー更新処理
	for (std::shared_ptr<EnemyBase> enemy : enemys_)
	{
		enemy->Update();
	}
}

void EnemyManager::Draw(void)
{
	//エネミー描画処理
	for (std::shared_ptr<EnemyBase> enemy : enemys_)
	{
		enemy->Draw();
	}
}

void EnemyManager::DrawHp(void)
{
	//エネミーHP描画処理
	for (std::shared_ptr<EnemyBase> enemy : enemys_)
	{
		enemy->DrawHp();
	}
}

void EnemyManager::Release(void)
{
	//エネミー解放処理
	for (std::shared_ptr<EnemyBase> enemy : enemys_)
	{
		enemy->Release();
	}
}

const std::vector<std::shared_ptr<EnemyBase>>& EnemyManager::GetEnemys(void) const
{
	return enemys_;
}
