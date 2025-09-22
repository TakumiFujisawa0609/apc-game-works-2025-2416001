#pragma once
#include <DxLib.h>
#include "WeponBase.h"
class WeponBeam
	:public WeponBase
{
public:

	//ビームの長さ
	static constexpr float MAX_BEAM_LENGTH = 1000.0f;

	// コンストラクタ
	WeponBeam(void);
	// デストラクタ
	~WeponBeam(void);

	void Draw(void) override;
	void Release(void) override;

	// 武器を使用する
	void Use(VECTOR pos, VECTOR rot, VECTOR dir) override;

protected:

	// 画像やモデルなどのロード
	void Load(void) override;
	// パラメータ設定
	void SetParam(void) override;
	void Move(void) override;

private:

	VECTOR playerRot_;
	float bemelong_;
};

