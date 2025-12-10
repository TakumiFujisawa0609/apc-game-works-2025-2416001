#pragma once
#include "SceneBase.h"
#include "../Common/Vector2.h"
#include <DxLib.h>

class Player;
class EnemyManager;
class EnemyBase;
class EnemyBeam;
class Grid;
class CollisinManager;
class Stage;
class SkyDome;

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
	std::shared_ptr<EnemyBase> enemy_;
	//グリッド
	std::unique_ptr<Grid> grid_;

	//ステージ
	std::shared_ptr<Stage> stage_;
	std::shared_ptr<SkyDome> skydome_;

	//自動ロックオン対象選別
	void UpdateAutoLockOn(void);
};

