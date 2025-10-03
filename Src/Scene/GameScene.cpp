#include <DxLib.h>

#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"

#include "../Manager/Camera.h"
#include "../Manager/DataBank.h"

#include "../Object/Common/EffectController.h"
#include "../Object/Common/Transform.h"
#include "../Object/Common/Grid.h"
#include "../Object/Robot/RobotBase.h"

#include "../Application.h"

#include "GameScene.h"

GameScene::GameScene(void)
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	//ロボット初期化処理
	//カメラ追尾対象初期設定
	Camera* camera = SceneManager::GetInstance().GetCamera();
	camera->ChangeMode(Camera::MODE::FREE);

	robot_ = std::make_unique<RobotBase>();
	robot_->Init();
	robot_->SetCamera(SceneManager::GetInstance().GetCamera());

	//グリッド初期化処理
	grid_ = std::make_unique<Grid>();

	camera->SetRobot(robot_.get());
}

void GameScene::Update(void)
{
	//シーン遷移処理
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_SPACE))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::RESULT);
	}

	//ロボット更新処理
	robot_->Update();

	Camera* camera = SceneManager::GetInstance().GetCamera();
	if (robot_->IsTargetLockFlage())
	{
		camera->ChangeMode(Camera::MODE::TARGET_ROCKE);
	}
	else
	{
		camera->ChangeMode(Camera::MODE::FIXED_POINT);
	}
}

void GameScene::Draw(void)
{
	//ロボット描画処理
	robot_->Draw();
	//グリッド描画処理
	grid_->Draw();
}

void GameScene::Release(void)
{
	//ロボット解放処理
	robot_->Release();
}
