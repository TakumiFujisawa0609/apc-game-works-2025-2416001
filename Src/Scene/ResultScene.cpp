#include <DxLib.h>
#include"../Application.h"
#include"../Utility/AsoUtility.h"
#include "../Manager/Camera.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Databank.h"
#include "ResultScene.h"

ResultScene::ResultScene(void)
{
}

ResultScene::~ResultScene(void)
{
}

void ResultScene::Init(void)
{
	Camera* camera = SceneManager::GetInstance().GetCamera();
	camera->ChangeMode(Camera::MODE::FREE);
}

void ResultScene::Update(void)
{
	auto& ins = InputManager::GetInstance();
	bool isKey;

	if (GetJoypadNum() == 0)
	{
		isKey = InputManager::GetInstance().IsTrgDown(KEY_INPUT_SPACE);
	}
	else
	{
		isKey = ins.IsPadBtnTrgDown(
			InputManager::JOYPAD_NO::PAD1,
			InputManager::JOYPAD_BTN::RIGHT
		);
	}

	if (isKey)
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}
}

void ResultScene::Draw(void)
{
	DrawFormatString(
		0, 20, GetColor(255, 255, 255),
		"ResultScene");
}

void ResultScene::Release(void)
{
}
