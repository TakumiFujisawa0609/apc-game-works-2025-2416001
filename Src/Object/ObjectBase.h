#pragma once
#include "./Common/Transform.h"

class ResourceManager;
class SceneManager;

class ObjectBase
{
	//仮

public:

	// コンストラクタ
	ObjectBase(void);

	// デストラクタ
	virtual ~ObjectBase(void);

	virtual void Load(void) = 0;
	virtual void Init(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;

	inline const Transform& GetTransform(void) const { return trans_; }

protected:

	// シングルトン参照
	ResourceManager& resMng_;
	SceneManager& scnMng_;

	// モデル制御の基本情報
	Transform trans_;

};

