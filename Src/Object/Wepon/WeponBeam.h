#pragma once
#include <DxLib.h>
#include "WeponBase.h"
class WeponBeam
	:public WeponBase
{
public:

	//相対座標
	static constexpr VECTOR LOCAL_POS = { 0.0f,150.0f,0.0f };

	//ビーム最大長さ
	static constexpr float MAX_BEAM_LENGTH = 1000.0f;

	//ビームの速度
	static constexpr float DEFAULT_SPEED = 200.0f;

	// コンストラクタ
	WeponBeam(void);
	// デストラクタ
	~WeponBeam(void);

	void Draw(void) override;
	void Release(void) override;

	// 武器を使用する
	void Use(VECTOR pos, VECTOR dir) override;

protected:

	// 画像やモデルなどのロード
	void Load(void) override;
	// パラメータ設定
	void SetParam(void) override;
	void Move(void) override;

private:

	//ビームの長さ
	float bemelong_;
};

