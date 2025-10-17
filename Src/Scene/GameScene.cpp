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
#include "../Object/Manager/WeponManager.h"
#include "../Object/Wepon/WeponBeam.h"
#include "../Object/Wepon/WeponMissile.h"
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
	std::vector<std::shared_ptr<EnemyBase>> enemy_ = enemys_->GetEnemys();
	enemys_->Init();


	//グリッド初期化処理
	grid_ = std::make_unique<Grid>();

	camera->SetPlayer(player_.get());
}

void GameScene::Update(void)
{
	std::vector<std::shared_ptr<EnemyBase>> enemy_ = enemys_->GetEnemys();
	/*for (std::shared_ptr<EnemyBase> enemy : enemy_)
	{
		if(!enemy->IsAlive())
		{
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::RESULT);
		}
	}*/

	//ロボット更新処理
	player_->Update();
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

	for (std::shared_ptr<EnemyBase> enemy : enemy_)
	{
		enemy->SetLockOnPos(player_->GetTransform().pos);
	}

	UpdateCollider();
	UpdateAutoLockOn();
}

void GameScene::Draw(void)
{
	//ロボット描画処理
	player_->Draw();
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

	for (auto& weapon : player_->GetUseWepons())
	{
		for (const auto& useWeapon : weapon->GetWepons())
		{
			if(useWeapon->isAlive_ == true)
			{
				DrawSphere3D(
					useWeapon->GetStatePos(),
					useWeapon->GetColliderRadius(),
					16,
					GetColor(200, 200, 200),
					GetColor(200, 200, 200),
					false);
			}
		}
	}

	std::vector<std::shared_ptr<EnemyBase>> enemy_ = enemys_->GetEnemys();
	for (std::shared_ptr<EnemyBase> enemy : enemy_)
	{
		VECTOR enemyClliderPos = VAdd(EnemyBeam::COLLIDER_POS, enemy->GetTransform().pos);

		if((enemy->IsAlive()))
		{
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

	DrawFormatString(
		0, 20, GetColor(255, 255, 255),
		"GameScene");

	int y = 40;
	for (std::shared_ptr<EnemyBase> enemy : enemy_)
	{
		int cnt = enemy_.size();
		float sub = VSize(VSub(player_->GetTransform().pos, enemy->GetTransform().pos));
		DrawFormatString(
			0, y, GetColor(255, 255, 255),
			"HP：(%.1f), diff(%.3f)",
			enemy->GetHp(), sub);
		y += 16;
	}
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

		if (AsoUtility::IsHitSpheres(
			playerClliderPos, Player::DEFALUT_RADIUS,
			enemyClliderPos, EnemyBeam::DEFALUT_RADIUS))
		{

		}

		for (auto& weapon : player_->GetUseWepons())
		{
			for (const auto& useWeapon : weapon->GetWepons())
			{
				if (AsoUtility::IsHitSpheres(
					useWeapon->GetStatePos(), useWeapon->GetColliderRadius(),
					enemyClliderPos, EnemyBeam::DEFALUT_RADIUS))
				{
					if (!enemy->IsCollisionState())
					{
						continue;
					}

					if (useWeapon->isAlive_ == true)
					{
						enemy->Damage(useWeapon->GetDamage());
					}

					useWeapon->isAlive_ = false;
				}
			}
		}
	}
}

void GameScene::UpdateAutoLockOn(void)
{
	Camera* camera = SceneManager::GetInstance().GetCamera();
	auto& enemys = enemys_->GetEnemys();
	auto& inp = InputManager::GetInstance();
	float diff = 0.0f;
	float min = 5000.0f;
	std::shared_ptr<EnemyBase> enemy_;

	for (auto& enemy : enemys)
	{
		VECTOR Pos = VSub(
			enemy->GetTransform().pos,
			player_->GetTransform().pos);
		diff = VSize(Pos);
		if (diff >= 5000 || !enemy->IsAlive()) {
			continue;
		}

		if (diff < min)
		{
			min = diff;
			enemy_ = enemy;
		}

		camera->SetEnemy(enemy_.get());
		player_->SetLockOnPos(enemy_->GetTransform().pos);
	}

	/*EnemyBubbleSort(enemys);*/
}

//void GameScene::EnemyBubbleSort(std::vector<std::shared_ptr<EnemyBase>> arr)
//{
//	int n = arr.size();
//	for (int i = 0; i < n - 1; i++){
//		for (int j = 0; j < n - i - 1; j++) {
//
//			VECTOR Pos1 = VSub(arr[j]->GetTransform().pos,
//				player_->GetTransform().pos);
//			float diff1 = VSize(Pos1);
//
//			VECTOR Pos2 = VSub(arr[j + 1]->GetTransform().pos,
//				player_->GetTransform().pos);
//			float diff2 = VSize(Pos2);
//
//			if (diff1 > diff2)
//			{
//				std::shared_ptr<EnemyBase> temp = arr[j];
//				arr[j] = arr[j + 1];
//				arr[j + 1] = temp;
//			}
//		}
//	}
//}
