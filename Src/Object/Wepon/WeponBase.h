#pragma once
#include <DxLib.h>
#include "../ObjectBase.h"
class WeponBase
	:public ObjectBase
{
public:
	//攻撃種類
	enum class WEPON_TYPE
	{
		NONE,
		BEAM,
		MISSILE,
		SWORD,
	};

	// コンストラクタ
	WeponBase(WEPON_TYPE type);

	// デストラクタ
	~WeponBase(void);
	void Init(void)override;
	void Init(VECTOR pos, VECTOR dir);
	void Init(VECTOR pos, VECTOR dir, VECTOR targetPos);
	void Update(void)override;
	virtual void Draw(void) = 0;
	virtual void Release(void) = 0;

	// 生存判定
	bool IsAlive(void);
	// 武器種別の取得
	WEPON_TYPE GetType(void);
	//ダメージの取得
	float GetDamage(void) { return damage_; }

protected:
	// 武器種別
	WEPON_TYPE type_;
	//標敵の座標
	VECTOR targetPos_;
	// 移動スピード
	float speed_;
	// 生存判定
	bool isAlive_;
	//ダメージ
	float damage_;
	// 画像やモデルなどのロード(純粋仮想関数)
	virtual void Load(void) = 0;
	// パラメータ設定(純粋仮想関数)
	virtual void SetParam(void) = 0;
	// 移動処理
	virtual void Move(void);
};

