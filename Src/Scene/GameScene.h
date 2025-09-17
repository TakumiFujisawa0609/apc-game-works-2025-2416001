#pragma once
#include <memory>
#include<functional>
#include "SceneBase.h"

class RobotBase;
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
	std::unique_ptr<RobotBase> robot_;

	//グリッド
	std::unique_ptr<Grid> grid_;

};

