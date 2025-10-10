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
#include "../Object/Robot/Player/Player.h"
#include "../Object/Robot/Enemy/EnemyBase.h"
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
	camera->ChangeMode(Camera::MODE::FIXED_POINT);

	//プレイヤー初期化処理
	player_ = std::make_unique<Player>();
	player_->Init();
	player_->SetCamera(SceneManager::GetInstance().GetCamera());

	//エネミー初期化処理
	enemys_ = std::make_unique<EnemyBase>();
	enemys_->Init();

	//グリッド初期化処理
	grid_ = std::make_unique<Grid>();

	camera->SetPlayer(player_.get());
	camera->SetEnemy(enemys_.get());
}

void GameScene::Update(void)
{
	//シーン遷移処理
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_SPACE))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::RESULT);
	}

	//ロボット更新処理
	player_->SetLockOnPos(enemys_->GetTransform().pos);
	player_->Update();

	//エネミー更新処理
	enemys_->Update();

	Camera* camera = SceneManager::GetInstance().GetCamera();
	if (player_->IsTargetLockFlage())
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
	player_->Draw();
	//エネミー描画処理
	enemys_->Draw();
	//グリッド描画処理
	grid_->Draw();

#ifdef _DEBUG

	VECTOR playerClliderPos = VAdd(Player::COLLIDER_POS, player_->GetTransform().pos);
	VECTOR enemyClliderPos = VAdd(EnemyBase::COLLIDER_POS, enemys_->GetTransform().pos);

	DrawSphere3D(
		playerClliderPos,
		Player::DEFALUT_RADIUS,	
		16,
		GetColor(200, 200, 200),
		GetColor(200, 200, 200),
		false);

	DrawSphere3D(
		enemyClliderPos,
		EnemyBase::DEFALUT_RADIUS,
		16,
		GetColor(200, 200, 200),
		GetColor(200, 200, 200),
		false);

#endif
}

void GameScene::Release(void)
{
	//ロボット解放処理
	player_->Release();
	//エネミー解放処理
	enemys_->Release();
}
