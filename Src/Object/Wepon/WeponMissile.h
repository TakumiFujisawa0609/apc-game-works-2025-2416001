#pragma once
#include <DxLib.h>
#include "WeponBase.h"
class WeponMissile :
    public WeponBase
{
public:

	//移動スピード
	static constexpr float DEFAULT_SPEED = 15.0f;
	//衝突判定用半径
	static constexpr float DEFAULT_RADIUS = 60.0f;
	//モデルの大きさ
	static constexpr VECTOR DEFAULT_SIZE = { 0.5f, 0.5f, 0.5f };
	//使用時の相対座標
	static constexpr VECTOR DEFAULT_POS = { 0.0f, 90.0f, 0.0f };
	// 使用時のジャンプ力
	static constexpr float JUMP_POW = 30.0f;
	// 重力
	static constexpr float GRAVITY = 2.0f;

	// コンストラクタ
	WeponMissile(void);
	// デストラクタ
	~WeponMissile(void);

	void Draw(void) override;
	void Release(void) override;

	// 武器を使用する
	void Use(VECTOR pos, VECTOR rot, VECTOR dir) override;

protected:

	// 画像やモデルなどのロード
	void Load(void) override;
	// パラメータ設定
	void SetParam(void) override;

private:
	// ジャンプ力
	float jumpPow_;

	// 移動処理
	void Move(void) override;
};

