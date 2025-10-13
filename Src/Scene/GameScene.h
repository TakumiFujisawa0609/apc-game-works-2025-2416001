#pragma once
#include <memory>
#include<functional>
#include "SceneBase.h"

class Player;
class EnemyManager;
class EnemyBase;
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
	std::unique_ptr<Player> player_;

	//エネミー
	std::shared_ptr<EnemyManager> enemys_;

	//グリッド
	std::unique_ptr<Grid> grid_;


	//衝突判定関数
	void UpdateCollider(void);

};

