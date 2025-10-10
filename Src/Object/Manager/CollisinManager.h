#pragma once
#include <DxLib.h>

class Transform;

class CollisinManager
{
public:
	// コンストラクタ
	CollisinManager(void);
	// デストラクタ
	~CollisinManager(void);

	void Update(void);
	void Release(void);
private:

	void SetWeponIsCollision(const Transform& trans1, const Transform& trans2);
};

