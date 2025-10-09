#pragma once
#include <vector>
#include <memory>
#include <DxLib.h>

class WeponBase;

class WeponManager
{
public:

	// コンストラクタ
	WeponManager(void);
	// デストラクタ
	~WeponManager(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

private:



};

