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
	for (std::shared_ptr<EnemyBase> enemy : enemy_)
	{
		camera->SetEnemy(enemy.get());
	}
}

void GameScene::Update(void)
{
	std::vector<std::shared_ptr<EnemyBase>> enemy_ = enemys_->GetEnemys();
	for (std::shared_ptr<EnemyBase> enemy : enemy_)
	{
		if(!enemy->IsAlive())
		{
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::RESULT);
		}
	}

	//ロボット更新処理
	player_->Update();
	enemys_->Update();
	
	float degreemum = DX_PI_F * 2;

	for (std::shared_ptr<EnemyBase> enemy : enemy_)
	{
		VECTOR enemyPos = enemy->GetTransform().pos;
		VECTOR playerPos = player_->GetTransform().pos;

		//計算して出た暫定的に一番小さい角度を記憶する変数です

		//プレイヤーからエネミーの距離が、一定外だったら処理をスキップする
		if (AsoUtility::IsLenge(enemyPos, playerPos, degreemum))
		{
			continue;
		}

		//プレイヤーからエネミーの距離を取り、Y座標を0にし、正規化する
		VECTOR vectorPos = VSub(enemyPos, playerPos);
		vectorPos.y = 0.0f;
		VNorm(vectorPos);

		float degree = atan2f(vectorPos.x, vectorPos.z);
		float degreep = atan2f(player_->GetTransform().rot.x,
			player_->GetTransform().rot.z);
	
		if (DX_PI <= (degreep - degree))
		{
			degree = degreep - degree - degreemum;
		}
		else if (-DX_PI >= (degreep - degree))
		{
			degree = degreep - degree + degreemum;
		}
		else
		{
			degree = degreep - degree;
		}

		//求めた角度にプレイヤーとエネミーの距離に応じて補正をかける(距離が長いほど補正は大きい)
		float lerpPos = AsoUtility::IsLenge(enemyPos, playerPos, 500);
		degree = degree + degree * lerpPos * 0.3f;

		if (AsoUtility::MyFabs(degreemum) >= AsoUtility::MyFabs(degree))
		{
			degreemum = degree;
		}
	}
	if (AsoUtility::MyFabs(degreemum) <= DX_PI_F / 3)
	{

	}
	else
	{
		
	}


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

		DrawSphere3D(
			enemyClliderPos,
			EnemyBeam::DEFALUT_RADIUS,
			16,
			GetColor(200, 200, 200),
			GetColor(200, 200, 200),
			false);
	}
#endif

	DrawFormatString(
		0, 20, GetColor(255, 255, 255),
		"GameScene");

	for (std::shared_ptr<EnemyBase> enemy : enemy_)
	{
		DrawFormatString(
			0, 40, GetColor(255, 255, 255),
			"HP：(%.1f)",
			enemy->GetHp());
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
