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
	state_ = STATE::GAMEOVER;
	mode_ = MODE::TRYAGAIN;
	if(state_ == STATE::CLERA)
	{
		img_ = resMng_.Load(ResourceManager::SRC::RESULT_CLEAR).handleId_;
	}
	else {
		img_ = resMng_.Load(ResourceManager::SRC::RESULT_GAMEOVER).handleId_;
	}
}

void ResultScene::Update(void)
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

	if (isKey && mode_ == MODE::TITLEBACK)
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}
	else if (isKey && mode_ == MODE::TRYAGAIN)
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}
}

void ResultScene::Draw(void)
{
	if(state_ == STATE::CLERA)
	{
		DrawRotaGraph(
			Application::SCREEN_SIZE_X / 2,
			Application::SCREEN_SIZE_Y / 2,
			1.0f, 0.0f, img_, true);
	}
	else {
		DrawRotaGraph(
			Application::SCREEN_SIZE_X / 2,
			Application::SCREEN_SIZE_Y / 2,
			1.0f, 0.0f, img_, true);
	}

	VECTOR dir = AsoUtility::VECTOR_ZERO;
	auto& ins = InputManager::GetInstance();

	if (GetJoypadNum() == 0) {
		if (ins.IsTrgDown(KEY_INPUT_W)) { mode_ = MODE::TRYAGAIN;}
		if (ins.IsTrgDown(KEY_INPUT_S)) { mode_ = MODE::TITLEBACK;}
	}
	else {
		// 接続されているゲームパッド１の情報を取得
		InputManager::JOYPAD_IN_STATE padState =
			ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
		// アナログキーの入力値から方向を取得
		dir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

		// 上方向入力で TRY AGAIN を選択
		if (dir.z < -0.5f) {
			mode_ = MODE::TRYAGAIN;
		}
		// 下方向入力で TITLE BACK を選択
		else if (dir.z > 0.5f) {
			mode_ = MODE::TITLEBACK;
		}
	}

	bool shouldBlink = (loopCounter / 20) % 2 == 0;
	if (mode_ != MODE::TRYAGAIN || !shouldBlink)
	{
		DrawString((Application::SCREEN_SIZE_X - GetDrawStringWidth("TRY AGAIN", 16)) / 2
			, Application::SCREEN_SIZE_Y - 150
			, "TRY AGAIN"
			, 0xFFFFFF);
	}

	if (mode_ != MODE::TITLEBACK|| !shouldBlink)
	{
		DrawString((Application::SCREEN_SIZE_X - GetDrawStringWidth("TITLE BACK", 16)) / 2
			, Application::SCREEN_SIZE_Y - 100
			, "TITLE BACK"
			, 0xFFFFFF);
	}

#ifdef _DEBUG
	DrawFormatString(
		0, 20, GetColor(255, 255, 255),
		"ResultScene");
#endif
}

void ResultScene::Release(void)
{
}
