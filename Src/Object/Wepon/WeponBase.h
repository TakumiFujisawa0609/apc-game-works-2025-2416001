#pragma once
#include <DxLib.h>
#include "../ObjectBase.h"
class WeponBase
	:public ObjectBase
{
public:
	//攻撃種類
	enum class WAPON_TYPE
	{
		NONE,
		BEAM,
		MISSILE,
		SWORD,
	};

	// コンストラクタ
	WeponBase(void);

	// デストラクタ
	~WeponBase(void);

	void Init(void)override;
	void Update(void)override;
	virtual void Draw(void) = 0;
	virtual void Release(void) = 0;
	// 武器を使用する
	virtual void Use(VECTOR pos,VECTOR dir) = 0;
	// 生存判定
	bool IsAlive(void);
	// 武器種別の取得
	WAPON_TYPE GetType(void);
protected:
	// 武器種別
	WAPON_TYPE type_;
	// 移動スピード
	float speed_;
	// 生存判定
	bool isAlive_;
	// 使用時の位置調整(Y)
	VECTOR localPos_;
	// 画像やモデルなどのロード(純粋仮想関数)
	virtual void Load(void) = 0;
	// パラメータ設定(純粋仮想関数)
	virtual void SetParam(void) = 0;
	// 移動処理
	virtual void Move(void);
};

