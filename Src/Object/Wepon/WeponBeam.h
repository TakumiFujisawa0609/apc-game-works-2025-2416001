#pragma once
#include <DxLib.h>
#include "WeponBase.h"

class WeponBeam
	:public WeponBase
{
public:

	//相対座標
	static constexpr VECTOR LOCAL_POS = { 0.0f,180.0f,0.0f };

	//ビーム最大長さ
	static constexpr float MAX_BEAM_LENGTH = 5000.0f;

	//ビーム生存距離
	static constexpr float MAX_BEAM_DIFF = 50000.0f;

	//ビームの速度
	static constexpr float DEFAULT_SPEED = 400.0f;

	//衝突半径
	static constexpr float DEFALUT_RADIUS = 15.0f;

	static constexpr float DAMAGE = 3.0f;

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

	//ビームの長さ
	float bemelong_;
};

