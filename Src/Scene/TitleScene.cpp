#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/FontManager.h"
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
	img_ = resMng_.Load(ResourceManager::SRC::TITLE_IMG).handleId_;
}

void TitleScene::Update(void)
{
	loopCounter++;
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
	DrawRotaGraph(
		Application::SCREEN_SIZE_X / 2,
		Application::SCREEN_SIZE_Y / 3,
		1.0f,0.0f,img_,true);

	if ((loopCounter / 20) % 3 == 0)
	{
		//0ÇÃéûÇÕÅAè¡Ç∑
	}
	else
	{
		//1 Ç∆ 2 ÇÃéûÇÕÅAì_ÇØÇÈ
		DrawString((Application::SCREEN_SIZE_X - GetDrawStringWidth("HIT SPACE KEY !!", 16)) / 2
			, Application::SCREEN_SIZE_Y - 100
			, "HIT SPACE KEY !!"
			, 0xFFFFFF);
	}


#ifdef _DEBUG
	DrawFormatString(
		0, 20, GetColor(255, 255, 255),
		"TitleScene");
#endif
}

void TitleScene::Release(void)
{
	DeleteGraph(img_);
}
