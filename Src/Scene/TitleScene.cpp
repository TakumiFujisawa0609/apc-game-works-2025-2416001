#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"

#include "../Application.h"
#include "TitleScene.h"
#include <cmath>

TitleScene::TitleScene(void)
{
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
}

void TitleScene::Update(void)
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
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}
}

void TitleScene::Draw(void)
{
	DrawFormatString(
		0, 20, GetColor(255, 255, 255),
		"TitleScene");
}

void TitleScene::Release(void)
{
}
