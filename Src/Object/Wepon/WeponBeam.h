#pragma once
#include <DxLib.h>
#include "WeponBase.h"

class WeponBeam
	:public WeponBase
{
public:

	//ビーム最大長さ
	static constexpr float MAX_BEAM_LENGTH = 1000.0f;
	//ビーム生存距離
	static constexpr float MAX_BEAM_DIFF = 50000.0f;
	//ビームの速度
	static constexpr float DEFAULT_SPEED = 100.0f;

	// コンストラクタ
	WeponBeam(WEPON_TYPE type);
	// デストラクタ
	~WeponBeam(void);

	void Draw(void) override;

protected:

	// 大きさ、回転、座標の初期化
	void InitTransform(void)override;
	// 衝突判定の初期化
	void InitCollider(void)override;
	// 初期化後の個別処理
	void InitPost(void)override;
	//移動処理
	void Move(void) override;

private:

	// 衝突判定用球体半径
	static constexpr float COL_CAPSULE_SPHERE = 30.0f;

	//ビームの長さ
	float bemelong_;
};

