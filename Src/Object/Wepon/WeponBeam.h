#pragma once
#include <DxLib.h>
#include "WeponBase.h"
class WeponBeam
	:public WeponBase
{
public:

	//ビームの長さ
	static constexpr float BEAM_LENGTH = 100.0f;

	// コンストラクタ
	WeponBeam(void);
	// デストラクタ
	~WeponBeam(void);

	void Draw(void) override;
	void Release(void) override;

	// 武器を使用する
	void Use(VECTOR pos) override;

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

