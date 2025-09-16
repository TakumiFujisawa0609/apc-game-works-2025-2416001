#include <DxLib.h>

#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"

#include "../Manager/Camera.h"
#include "../Manager/DataBank.h"

#include "../Object/Common/EffectController.h"
#include "../Object/Common/Transform.h"
#include "../Object/Robot.h"

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
  robot_ = std::make_unique<Robot>();  
  robot_->Init();  

  Camera* camera = SceneManager::GetInstance().GetCamera();
  camera->SetRobot(robot_.get());
}

void GameScene::Update(void)
{
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_SPACE))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::RESULT);
	}
	robot_->Update();
}

void GameScene::Draw(void)
{
	robot_->Draw();
}

void GameScene::Release(void)
{
	robot_->Release();
}
