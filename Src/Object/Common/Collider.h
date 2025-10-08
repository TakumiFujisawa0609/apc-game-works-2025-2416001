#pragma once
#include <DxLib.h>

class Transform;

class Collider
{

public :

	// コンストラクタ
	Collider(void);
	// デストラクタ
	~Collider(void);

	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

private:

	void SetWeponIsCollision(const Transform& trans1, const Transform& trans2);

};
