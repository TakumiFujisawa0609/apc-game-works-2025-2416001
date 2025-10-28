#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/Camera.h"
#include "../Object/Common/EffectController.h"
#include "../Object/Common/Transform.h"
#include "../Object/Common/Grid.h"
#include "../Object/Manager/EnemyManager.h"
#include "../Object/Manager/WeponManager.h"
#include "../Object/Robot/Player/Player.h"
#include "../Object/Robot/Enemy/EnemyBase.h"
#include "../Utility/AsoUtility.h"
#include "../Utility/MatrixUtility.h"
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
	//ロボット更新処理
	player_->Update();
	enemys_->Update();

	Camera* camera = SceneManager::GetInstance().GetCamera();

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

	VECTOR playerClliderPos = VAdd(player_->GetCillisionPos(), player_->GetTransform().pos);

	DrawSphere3D(
		playerClliderPos,
		Player::DEFALUT_RADIUS,
		16,
		GetColor(200, 200, 200),
		GetColor(200, 200, 200),
		false);

	for (const auto& useWeapon : player_->GetUseWepons()->GetWepons())
	{
		if (useWeapon->isAlive_ == true)
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

	auto& enemys = enemys_->GetEnemys();
	for (auto& enemy : enemys)
	{
		VECTOR enemyClliderPos = VAdd(enemy->GetCillisionPos(), enemy->GetTransform().pos);

		if ((enemy->IsAlive()))
		{
			DrawSphere3D(
				enemyClliderPos,
				enemy->GetCillisionRadius(),
				16,
				GetColor(200, 200, 200),
				GetColor(200, 200, 200),
				false);
		}
	}

	DrawFormatString(
		0, 20, GetColor(255, 255, 255),
		"GameScene");
	
	float vAngle = 5000.0f;

	MATRIX mat = MGetIdent();
	mat = MatrixUtility::GetMatrixRotateXYZ(player_->GetTransform().rot);

	VECTOR forward = VTransform(AsoUtility::DIR_F, mat);

	MATRIX rightMat = MMult(mat, MGetRotY(AsoUtility::Deg2RadF(vAngle)));
	VECTOR right = VTransform(AsoUtility::DIR_F, rightMat);

	MATRIX leftMat = MMult(mat, MGetRotY(AsoUtility::Deg2RadF(-vAngle)));
	VECTOR left = VTransform(AsoUtility::DIR_F, leftMat);

	VECTOR pos0 = player_->GetTransform().pos;
	VECTOR pos1 = VAdd(pos0, VScale(forward, vAngle));
	VECTOR pos2 = VAdd(pos0, VScale(right, vAngle));
	VECTOR pos3 = VAdd(pos0, VScale(left, vAngle));

	pos0.y = pos1.y = pos2.y = pos3.y = 10.0f;
	DrawTriangle3D(pos0, pos2, pos1, 0xdd77dd, true);
	DrawTriangle3D(pos0, pos1, pos3, 0xdd77dd, true);

	DrawLine3D(pos0, pos1, 0x000000);
	DrawLine3D(pos0, pos2, 0x000000);
	DrawLine3D(pos0, pos3, 0x000000);

#endif
}

void GameScene::Release(void)
{
	//ロボット解放処理
	player_->Release();
	//エネミー解放処理
	enemys_->Release();
}

void GameScene::UpdateAutoLockOn(void)
{
	Camera* camera = SceneManager::GetInstance().GetCamera();
	auto& enemys = enemys_->GetEnemys();
	std::shared_ptr<EnemyBase> enemy_ = nullptr;

	auto& inp = InputManager::GetInstance();
	float diff = 0.0f;
	float min = 5000.0f;

	//エネミーのオートロックオン処理
	for (std::shared_ptr<EnemyBase> enemy : enemys) {
		enemy->SetLockOnPos(player_->GetTransform().pos);
	}

	//プレイヤーのオートロックオン処理
	if (player_->IsTargetLockFlage()) {
		camera->ChangeMode(Camera::MODE::TARGET_ROCKE);
	}
	else {
		camera->ChangeMode(Camera::MODE::FIXED_POINT);
	}

	for (auto& enemy : enemys){

		//プレイヤーからエネミーの長さ
		VECTOR Pos = VSub(
			enemy->GetTransform().pos,
			player_->GetTransform().pos);
		diff = VSize(Pos);

		//プレイヤーからエネミーの長さが条件より大きい
		// または、エネミーが生存していないの時処理をスキップ
		if (diff >= 5000 || !enemy->IsAlive()) {
			continue;
		}

		if (inp.IsTrgDown(KEY_INPUT_LEFT)) {
			//プレイヤーからエネミーの長さが一番小さい長さを格納
			if (diff < min) {
				min = diff;
				enemy_ = enemy;
			}
		}

		if (inp.IsTrgDown(KEY_INPUT_RIGHT)) {
			//プレイヤーからエネミーの長さが一番小さい長さを格納
			if (diff < min) {
				min = diff;
				enemy_ = enemy;
			}
		}

		if (inp.IsTrgDown(KEY_INPUT_UP)) {
			//プレイヤーからエネミーの長さが一番小さい長さを格納
			if (diff < min) {
				min = diff;
				enemy_ = enemy;
			}
		}

		if (inp.IsTrgDown(KEY_INPUT_DOWN)) {
			//プレイヤーからエネミーの長さが一番小さい長さを格納
			if (diff < min) {
				min = diff;
				enemy_ = enemy;
			}
		}
	}

	if (enemy_ == nullptr){
		return;
	}

	//長さが一番小さいエネミー情報を渡す
	camera->SetEnemy(enemy_.get());
	player_->SetLockOnPos(enemy_->GetTransform().pos);
}
