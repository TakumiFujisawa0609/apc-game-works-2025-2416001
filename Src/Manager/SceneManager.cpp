#include <EffekseerForDXLib.h>
#include <DxLib.h>

#include <string>
#include <fstream>

#include "../Application.h"
#include "../Common/Fader.h"

#include "./ResourceManager.h"

#include "../Scene/SceneBase.h"
#include "../Scene/TitleScene.h"
#include "../Scene/GameScene.h"
#include "../Scene/ResultScene.h"
#include"DataBank.h"

#include "Camera.h"

#include "SceneManager.h"

SceneManager* SceneManager::instance_ = nullptr;

SceneManager::SceneManager(void)
{

	sceneId_ = SCENE_ID::NONE;
	waitSceneId_ = SCENE_ID::NONE;

	

	isSceneChanging_ = false;

	camera_ = nullptr;


}

SceneManager::~SceneManager(void)
{
	delete instance_;
}

void SceneManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new SceneManager();
	}
	instance_->Init();
}

SceneManager& SceneManager::GetInstance(void)
{
	return *instance_;
}

void SceneManager::Init(void)
{

	sceneId_ = SCENE_ID::TITLE;
	waitSceneId_ = SCENE_ID::NONE;

	fader_ = std::make_unique<Fader>();
	fader_->Init();

	// カメラ
	camera_ = std::make_shared<Camera>();
	camera_->Init();

	isSceneChanging_ = false;

	totalGameTime_ = 0.0f;

	//データバンクの作成
	DataBank::CreateInstance();

	// 3D用の設定
	Init3D();

	// 初期シーンの設定
	DoChangeScene(SCENE_ID::GAME);

}
void SceneManager::Init3D(void)
{

	// 背景色設定
	SetBackgroundColor(0, 139, 139);

	// Zバッファを有効にする
	SetUseZBuffer3D(true);

	// Zバッファへの書き込みを有効にする
	SetWriteZBuffer3D(true);

	// バックカリングを有効にする
	SetUseBackCulling(true);

	// ライトの設定
	SetUseLighting(true);

	// ライトの設定
	ChangeLightTypeDir({ 0.0f, 0.2f, 0.8f });
	//ChangeLightTypePoint({ 0.0f, -100.0f, 0.0f },20000.0f, 0.0f, 0.0006f, 0.0f);


	// フォグ設定
	SetFogEnable(true);
	SetFogColor(5, 5, 5);
	SetFogStartEnd(10000.0f, 20000.0f);

	mainScreen_ = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y);
}

void SceneManager::Update(void)
{

	// デルタタイム
	auto nowTime = std::chrono::system_clock::now();
	deltaTime_ = static_cast<float>(
		std::chrono::duration_cast<std::chrono::nanoseconds>(nowTime - preTime_).count() / 1000000000.0);
	preTime_ = nowTime;

	// フェード更新
	fader_->Update();
	if (isSceneChanging_)
	{
		Fade();
	}
	else
	{
		// 更新
		scene_->Update();
	}

	// カメラ更新
	camera_->Update();
}

void SceneManager::Draw(void)
{

	// 描画先グラフィック領域の指定
	// (３Ｄ描画で使用するカメラの設定などがリセットされる)
	SetDrawScreen(mainScreen_);

	// 画面を初期化
	ClearDrawScreen();

	// カメラ設定
	camera_->SetBeforeDraw();

	// Effekseerにより再生中のエフェクトを更新する。
	UpdateEffekseer3D();

	// 描画
	scene_->Draw();

	camera_->Draw();

	// Effekseerにより再生中のエフェクトを描画する。
	DrawEffekseer3D();

	// 最後
	fader_->Draw();

	SetDrawScreen(DX_SCREEN_BACK);
	DrawGraph(0, 0, mainScreen_, true);

}

void SceneManager::Destroy(void)
{
	DeleteGraph(mainScreen_);
	DataBank::GetInstance().Destroy();
	if (capturedScreenGraph_ != -1)
	{
		DeleteGraph(capturedScreenGraph_);
		capturedScreenGraph_ = -1;
	}
}

void SceneManager::ChangeScene(SCENE_ID nextId)
{

	// フェード処理が終わってからシーンを変える場合もあるため、
	// 遷移先シーンをメンバ変数に保持
	waitSceneId_ = nextId;

	// フェードアウト(暗転)を開始する
	fader_->SetFade(Fader::NET_STATE::FADE_OUT);
	isSceneChanging_ = true;

}

SceneManager::SCENE_ID SceneManager::GetSceneID(void)
{
	return sceneId_;
}

float SceneManager::GetDeltaTime(void) const
{
	//return 1.0f / DEFAULT_FPS;
	return deltaTime_;
}


Camera* SceneManager::GetCamera(void) const
{
	return camera_.get();
}

const int& SceneManager::GetMainScreen(void)
{
	return mainScreen_;
}

void SceneManager::DoChangeScene(SCENE_ID sceneId)
{
	// リソースの解放
	ResourceManager::GetInstance().SceneChangeRelease();

	// シーンを変更する
	sceneId_ = sceneId;

	// 現在のシーンを解放
	if (scene_ != nullptr)
	{
		scene_->Release();
		scene_.reset();
	}

	switch (sceneId_)
	{
	case SCENE_ID::TITLE:
		scene_ = std::make_unique<TitleScene>();
		break;
	case SCENE_ID::GAME:
		scene_ = std::make_unique<GameScene>();
		break;
	case SCENE_ID::RESULT:
		scene_ = std::make_unique<ResultScene>();
		break;
	}

	scene_->Init();

	//ResetDeltaTime();

	waitSceneId_ = SCENE_ID::NONE;


	// フォントサイズを元に戻す
	SetFontSize(16);

}

void SceneManager::Fade(void)
{

	Fader::NET_STATE fState = fader_->GetState();
	switch (fState)
	{
	case Fader::NET_STATE::FADE_OUT:
		if (fader_->IsEnd())
		{
			// 暗転後、シーン変更
			DoChangeScene(waitSceneId_);
			fader_->SetFade(Fader::NET_STATE::FADE_IN);
		}
		break;
	case Fader::NET_STATE::FADE_IN:
		if (fader_->IsEnd())
		{
			// 明転後、シーン遷移終了
			fader_->SetFade(Fader::NET_STATE::NONE);
			isSceneChanging_ = false;
		}
		break;
	}

}
