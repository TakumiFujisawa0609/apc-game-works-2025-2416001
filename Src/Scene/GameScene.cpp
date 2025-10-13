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
#include "../Object/Manager/EnemyManager.h"
#include "../Object/Robot/Player/Player.h"
#include "../Object/Robot/Enemy/EnemyBase.h"
#include "../Object/Robot/Enemy/EnemyBeam.h"
#include "../Object/Robot/Enemy/EnemyMissile.h"
#include "../Utility/AsoUtility.h"
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
	enemys_ = std::make_shared<EnemyManager>();
	enemys_->Init();

	//グリッド初期化処理
	grid_ = std::make_unique<Grid>();

	camera->SetPlayer(player_.get());
	std::vector<std::shared_ptr<EnemyBase>> enemy_ = enemys_->GetEnemys();
	for (std::shared_ptr<EnemyBase> enemy : enemy_)
	{
		camera->SetEnemy(enemy.get());
	}
}

void GameScene::Update(void)
{
	//シーン遷移処理
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_SPACE))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::RESULT);
	}

	//ロボット更新処理
	std::vector<std::shared_ptr<EnemyBase>> enemy_ = enemys_->GetEnemys();
	for (std::shared_ptr<EnemyBase> enemy : enemy_)
	{
		player_->SetLockOnPos(enemy->GetTransform().pos);
	}
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

	UpdateCollider();
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
	
	DrawSphere3D(
		playerClliderPos,
		Player::DEFALUT_RADIUS,	
		16,
		GetColor(200, 200, 200),
		GetColor(200, 200, 200),
		false);

	std::vector<std::shared_ptr<EnemyBase>> enemy_ = enemys_->GetEnemys();
    for (std::shared_ptr<EnemyBase> enemy : enemy_)
    {
        if (enemy != nullptr)  // nullチェック追加
        {
            VECTOR enemyClliderPos = VAdd(EnemyBeam::COLLIDER_POS, enemy->GetTransform().pos);
            DrawSphere3D(
                enemyClliderPos,
                EnemyBeam::DEFALUT_RADIUS,
                16,
				GetColor(200, 200, 200),
				GetColor(200, 200, 200),
                false);
        }
    }

#endif
}

void GameScene::Release(void)
{
	//ロボット解放処理
	player_->Release();
	//エネミー解放処理
	enemys_->Release();
}

void GameScene::UpdateCollider(void)
{
	VECTOR playerClliderPos = VAdd(Player::COLLIDER_POS, player_->GetTransform().pos);
	std::vector<std::shared_ptr<EnemyBase>> enemy_ = enemys_->GetEnemys();
	for (std::shared_ptr<EnemyBase> enemy : enemy_)
	{
		VECTOR enemyClliderPos = VAdd(EnemyBeam::COLLIDER_POS, enemy->GetTransform().pos);
	}

	//if(AsoUtility::IsHitSpheres())
}
