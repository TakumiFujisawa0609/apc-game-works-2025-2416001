#pragma once
#include<vector>
#include<memory>
#include"Common/Transform.h"
#include "Common/Collider.h"

class ResourceManager;
class SceneManager;
class InputManager;
class Sphere;
class Collider;

class ObjectBase
{
public:

	// コンストラクタ
	ObjectBase(void);

	// デストラクタ
	virtual ~ObjectBase(void);

	virtual void Init(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;
	virtual void Release(void) = 0;

	inline const Transform& GetTransform(void) const { return trans_; }
	Transform& GetTransform(void){ return trans_; }

protected:

	// シングルトン参照
	ResourceManager& resMng_;
	SceneManager& scnMng_;
	InputManager& inpMng_;

	// モデル制御の基本情報
	Transform trans_;
};

