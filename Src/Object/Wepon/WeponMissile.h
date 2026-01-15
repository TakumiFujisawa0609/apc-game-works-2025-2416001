#pragma once
#include <DxLib.h>
#include "WeponBase.h"

class WeponMissile :
    public WeponBase
{
public:

	//相対座標
	static constexpr VECTOR LOCAL_POS = { 0.0f,180.0f,0.0f };
	//ビームの長さ
	static constexpr float MAX_BEAM_LENGTH = 10.0f;
	//移動スピード
	static constexpr float DEFAULT_SPEED = 50.0f;
	//モデルの大きさ
	static constexpr VECTOR DEFAULT_SIZE = { 0.5f, 0.5f, 0.5f };
	//使用時の相対座標
	static constexpr VECTOR DEFAULT_POS = { 0.0f, 90.0f, 0.0f };
	// 使用時のジャンプ力
	static constexpr float JUMP_POW = 30.0f;
	// 重力
	static constexpr float GRAVITY = 0.1f;
	//追尾開始カウント
	static constexpr float HOMINGSTATE_CNT = 40.0f;

	// コンストラクタ
	WeponMissile(WEPON_TYPE type);
	// デストラクタ
	~WeponMissile(void);

	void Draw(void) override;

protected:

	// 大きさ、回転、座標の初期化
	void InitTransform(void)override;
	// 衝突判定の初期化
	void InitCollider(void)override;
	// 初期化後の個別処理
	void InitPost(void)override;
	// 移動処理
	void Move(void) override;

private:

	// 衝突判定用球体半径
	static constexpr float COL_CAPSULE_SPHERE = 50.0f;

	// ジャンプ力
	float jumpPow_;

	//ビームの長さ
	float long_;

	//追尾カウント
	float homingCnt_;
};

