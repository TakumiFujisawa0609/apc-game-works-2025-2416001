#pragma once
#include "SceneBase.h"
#include<functional>
class ResultScene :
    public SceneBase
{

public:

	// コンストラクタ
	ResultScene(void);

	// デストラクタ
	~ResultScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:
};

