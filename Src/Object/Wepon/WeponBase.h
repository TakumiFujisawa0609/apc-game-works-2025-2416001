#pragma once
#include <DxLib.h>
#include "../ObjectBase.h"

class ColliderBase;


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

	//相対座標
	static constexpr VECTOR LOCAL_POS = { 0.0f,180.0f,0.0f };

	// コンストラクタ
	WeponBase(WEPON_TYPE type);

	// デストラクタ
	~WeponBase(void);
	void Init(VECTOR pos, VECTOR dir, ColliderBase::TAG tag);
	void Init(VECTOR pos, VECTOR dir, VECTOR targetPos, ColliderBase::TAG tag);
	void Update(void)override;
	virtual void Draw(void) = 0;

	// 武器種別の取得
	WEPON_TYPE GetType(void);

protected:

	//武器タグ
	ColliderBase::TAG weponTag_;

	// 移動前の座標
	VECTOR prevPos_;
	//カプセル先頭座標
	VECTOR endPos_;
	// 武器種別
	WEPON_TYPE type_;
	//標敵の座標
	VECTOR targetPos_;
	//使用者の座標
	VECTOR playPos_;
	// 移動スピード
	float speed_;

	// リソースロード
	void InitLoad(void)override{}
	// アニメーションの初期化
	void InitAnimation(void)override{}

	// 移動処理
	virtual void Move(void);
};

