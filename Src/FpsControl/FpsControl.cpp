#include <DxLib.h>
#include "FpsControl.h"
#include "../Application.h"

FpsControl::FpsControl()
{
	currentTime_ = 0;
	prevFrameTime_ = 0;
	frameCnt_ = 0;
	updateFrameRateTime_ = 0;
	frameCnt_ = 0.0f;
}

FpsControl::~FpsControl()
{
}

void FpsControl::Init()
{
	//初期化
	currentTime_ = 0;
	prevFrameTime_ = 0;
	frameCnt_ = 0;
	updateFrameRateTime_ = 0;
	frameCnt_ = 0.0f;
}

bool FpsControl::UpdateFrameRate()
{
	Sleep(1);	//システムに処理を返す

	//現在の時刻を取得
	currentTime_ = GetNowCount();

	//現在の時刻が、前回のフレーム実行時より
	//1/60秒経過していたら処理を実行する
	if (currentTime_ - prevFrameTime_ >= FRAME_RATE)
	{
		//フレーム実行時の時間を計算
		prevFrameTime_ = currentTime_;

		//フレーム数をカウント
		frameCnt_++;

		//1/60経過した
		return true;
	}

	return false;
}

void FpsControl::CalcFrameRate()
{
	//前回のフレームレート更新からの経過時間を求める
	int difTime = currentTime_ - updateFrameRateTime_;

	//前回のフレームレートを更新から
	//1秒以上経過していたらフレームレートを更新する
	if (difTime > 1000)
	{
		//フレーム回数をミリ秒に合わせる
		//少数まで出したのでfloatにキャスト
		float castFrameCnt = (float)(frameCnt_ * 1000);

		//フレームレートを求める
		//理想通りなら 60000 / 1000 で60となる
		frameRate_ = castFrameCnt / difTime;

		//フレームレート更新時間を更新
		updateFrameRateTime_ = currentTime_;
	}
}

void FpsControl::DrawFrameRate()
{
	//デバッグ用表示

	//スクリーンの右端に出るように表示
	DrawFormatString(
		Application::SCREEN_SIZE_X - 90,
		0,
		0xff0000,
		"FPS[%.2f]",
		frameRate_);
}
