#pragma once
#include <memory>
#include<functional>
#include "SceneBase.h"

class Robot;

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

	std::unique_ptr<Robot> robot_;

};

