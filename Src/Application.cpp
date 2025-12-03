#include <DxLib.h>
#include <EffekseerForDXLib.h>

#include "Manager/SceneManager.h"
#include "Manager/InputManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/SoundManager.h"

#include "./FpsControl/FpsController.h"
#include "Application.h"

Application* Application::instance_ = nullptr;

const std::string Application::PATH_IMAGE = "Data/Image/";
const std::string Application::PATH_MODEL = "Data/Model/";
const std::string Application::PATH_EFFECT = "Data/Effect/";
const std::string Application::PATH_SOUND = "Data/Sound/";
const std::string Application::PATH_OBJECT = "Data/Image/Object/";

void Application::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new Application();
	}
	instance_->Init();
}

Application& Application::GetInstance(void)
{
	return *instance_;
}

void Application::Init(void)
{

	// アプリケーションの初期設定
	SetWindowText("ロボット大戦");

	// ウィンドウサイズ
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 32);
	ChangeWindowMode(true);

	// FPS制御初期化
	fps_ = std::make_unique<FpsController>(FRAME_RATE);

	// DxLibの初期化
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	isInitFail_ = false;
	if (DxLib_Init() == -1)
	{
		isInitFail_ = true;
		return;
	}

	// Effekseerの初期化
	InitEffekseer();

	// キー制御初期化
	SetUseDirectInputFlag(true);

	// リソース管理初期化
	ResourceManager::CreateInstance();
	ResourceManager::GetInstance().Init();

	SoundManager::CreateInstance();
	SoundManager::GetInstance().Init();

	SceneManager::CreateInstance();
	InputManager::CreateInstance();

}

void Application::Run(void)
{

	// ゲームループ
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
	{

		InputManager::GetInstance().Update();

		SceneManager::GetInstance().Update();
		SceneManager::GetInstance().Draw();

#ifdef _DEBUG
		// 平均FPS描画
		fps_->Draw();
#endif
		ScreenFlip();

		// 理想FPS経過待ち
		fps_->Wait();
	}
}

void Application::Destroy(void)
{

	// Effekseerを終了する。
	Effkseer_End();

	// DxLib終了
	if (DxLib_End() == -1)
	{
		isReleaseFail_ = true;
	}

	SceneManager::GetInstance().Destroy();
	InputManager::GetInstance().Destroy();
	ResourceManager::GetInstance().Release();
	SoundManager::GetInstance().Destroy();

	delete instance_;

}

bool Application::IsInitFail(void) const
{
	return isInitFail_;
}

bool Application::IsReleaseFail(void) const
{
	return isReleaseFail_;
}

Application::Application(void)
{
	isInitFail_ = false;
	isReleaseFail_ = false;
	fps_ = nullptr;
}

void Application::InitEffekseer(void)
{
	if (Effekseer_Init(8000) == -1)
	{
		DxLib_End();
	}

	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
}
