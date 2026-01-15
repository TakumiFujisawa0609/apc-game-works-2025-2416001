#pragma once
#include "SceneBase.h"

class ResultScene :
    public SceneBase
{

public:

	enum class STATE
	{
		CLERA,
		GAMEOVER
	};

	// コンストラクタ
	ResultScene(void);

	// デストラクタ
	~ResultScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

	int img_;
	int loopCounter;

	STATE state_;
};

