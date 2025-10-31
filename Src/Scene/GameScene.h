#pragma once
#include <memory>
#include "SceneBase.h"

class Player;
class EnemyManager;
class EnemyBase;
class EnemyBeam;
class Grid;

class GameScene :
    public SceneBase
{

public:

	// コンストラクタ
	GameScene(void);

	// デストラクタ
	~GameScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

	//ロボット
	std::shared_ptr<Player> player_;
	//エネミー
	std::shared_ptr<EnemyManager> enemys_;
	//グリッド
	std::unique_ptr<Grid> grid_;
	//自動ロックオン対象選別
	void UpdateAutoLockOn(void);

	//衝突判定更新
	void CollisinUpdate(void);
};

