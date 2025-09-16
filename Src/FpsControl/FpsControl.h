#pragma once

//フレームレート
static constexpr float FRAME_RATE(1000 / 60);

class FpsControl
{
public:

	FpsControl();	// コンストラクタ
	~FpsControl();	// デストラクタ

	//初期化
	void Init();

	//フレームレート更新
	bool UpdateFrameRate();

	//フレームレート計算
	void CalcFrameRate();

	//フレームレート表示(デバッグ表示)
	void DrawFrameRate();

private:

	//現在の時間
	int currentTime_;

	//前回のフレーム実行時の時間
	int prevFrameTime_;

	//フレームカウント用
	int frameCnt_;

	//フレームレートを更新した時間
	int updateFrameRateTime_;

	//フレームレート(表示用)
	float frameRate_;

};

