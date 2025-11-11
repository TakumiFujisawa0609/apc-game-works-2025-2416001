#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/Camera.h"
#include "../Object/Common/EffectController.h"
#include "../Object/Common/Transform.h"
#include "../Object/Common/Grid.h"
#include "../Object/Manager/EnemyManager.h"
#include "../Object/Manager/CollisinManager.h"
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
	//当たり判定インスタンス生成
	CollisinManager::CreateInstance();

	//ロボット初期化処理
	//カメラ追尾対象初期設定
	Camera* camera = SceneManager::GetInstance().GetCamera();
	camera->ChangeMode(Camera::MODE::FIXED_POINT);

	//プレイヤー初期化処理
	player_ = std::make_shared<Player>();
	player_->Init();
	player_->SetCamera(SceneManager::GetInstance().GetCamera());

	//エネミー初期化処理
	enemys_ = std::make_shared<EnemyManager>();
	enemy_ = nullptr;
	enemys_->Init();

	//グリッド初期化処理
	grid_ = std::make_unique<Grid>();

	//カメラの注視点をプレイヤーに設定
	camera->SetPlayer(player_.get());

}

void GameScene::Update(void)
{
	if (!player_->IsAlive() || enemys_->IsClear == true)
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::RESULT);
	}

	//ロボット更新処理
	player_->Update();
	enemys_->Update();

	//各当たり判定更新
	CollisinUpdate();
	CollisinManager::GetInstance().Update();
	HitCollisinUpdate();

	//ロック対象更新
	UpdateAutoLockOn();
}

void GameScene::Draw(void)
{
	//グリッド描画処理
	grid_->Draw();

	//ロボット描画処理
	player_->Draw();
	player_->DrawHp();

	enemys_->Draw();
	Camera* camera = SceneManager::GetInstance().GetCamera();
	if (enemy_ != nullptr && camera->GetCameraMode() == Camera::MODE::TARGET_ROCKE) {
		enemy_->DrawHp();
	}

#ifdef _DEBUG

	DrawSphere3D(
		player_->GetCillisionPos(),
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
		if ((enemy->IsAlive()))
		{
			DrawSphere3D(
				enemy->GetCillisionPos(),
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
	
	/*float vAngle = 5000.0f;

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
	DrawLine3D(pos0, pos3, 0x000000);*/

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

	auto& inp = InputManager::GetInstance();
	float diff = 0.0f;
	float min = 5000.0f;

	//エネミーのオートロックオン処理
	for (std::shared_ptr<EnemyBase> enemy : enemys) {
		enemy->SetLockOnPos(player_->GetTransform().pos);
	}

	if (enemy_ != nullptr && ( !enemy_->IsAlive() || enemy_->GetHp() <= 0)) {
		enemy_ = nullptr;
		camera->ChangeMode(Camera::MODE::FIXED_POINT);
	}

	//プレイヤーのオートロックオン処理
	if (player_->IsTargetLockFlage()) {
		camera->ChangeMode(Camera::MODE::TARGET_ROCKE);
	}
	else {
		camera->ChangeMode(Camera::MODE::FIXED_POINT);
		return;
	};

	bool needNewTarget;

	// キー入力によるカメラの回転
	auto& ins = InputManager::GetInstance();
	if (GetJoypadNum() == 0)
	{
		needNewTarget = (enemy_ == nullptr) || inp.IsTrgDown(KEY_INPUT_RETURN);
	}
	else
	{
		auto& ins = InputManager::GetInstance();
		needNewTarget = ins.IsPadBtnTrgDown(
			InputManager::JOYPAD_NO::PAD1,
			InputManager::JOYPAD_BTN::R_BTN
		);
	}


	if (needNewTarget) {
		std::shared_ptr<EnemyBase> newTarget = nullptr;

		// 現在のターゲットが有効かチェック
		bool currentTargetValid = (enemy_ != nullptr &&
			enemy_->IsAlive() &&
			enemy_->GetHp() > 0);

		// 現在のターゲットが有効な場合は、距離チェックのみ行う
		if (currentTargetValid) {
			VECTOR Pos = VSub(
				enemy_->GetTransform().pos,
				player_->GetTransform().pos);
			float currentDiff = VSize(Pos);

			// 現在のターゲットが範囲外の場合のみ新しいターゲットを探す
			if (currentDiff >= 5000.0f) {
				currentTargetValid = false;
			}
		}

		// 現在のターゲットが無効な場合のみ、新しいターゲットを探す
		if (!currentTargetValid) {
			float min = FLT_MAX;

			for (auto& enemy : enemys) {
				// 生存している敵のみを対象にする
				if (!enemy->IsAlive() || enemy->GetHp() <= 0) {
					continue;
				}
				//プレイヤーからエネミーの長さ
				VECTOR Pos = VSub(
					enemy->GetTransform().pos,
					player_->GetTransform().pos);
				float diff = VSize(Pos);
				//範囲外はスキップ
				if (diff >= 5000.0f) {
					continue;
				}
				//一番近い敵を探す
				if (diff < min) {
					min = diff;
					newTarget = enemy;
				}
			}

			// 新しいターゲットが見つかった場合のみ更新
			if (newTarget != nullptr) {
				enemy_ = newTarget;
			}
			else {
				// 見つからなかった場合はターゲットをクリア
				enemy_ = nullptr;
			}
		}
	}

	// ターゲットがある場合のみカメラとプレイヤーに設定
	if (enemy_ != nullptr) {
		camera->SetEnemy(enemy_.get());
		player_->SetLockOnPos(enemy_->GetTransform().pos);
	}
}

void GameScene::CollisinUpdate(void)
{
	auto& collisin_ = CollisinManager::GetInstance();
	collisin_.Clear();

	//プレイヤー機能当たり判定
	collisin_.RegisterSphere(
		player_,
		player_->GetCillisionPos(),
		player_->GetTransform().Radius_,
		CollisinManager::TAG_TYPE::PLAYER,
		true
	);


	auto& playerWepon = player_->GetUseWepons()->GetWepons();
	for (auto& wepon : playerWepon) {

		if (!wepon->isAlive_) {
			continue;
		}

		collisin_.RegisterCapsule(
			wepon,
			wepon->GetStatePos(),
			wepon->GetTransform().pos,
			wepon->GetColliderRadius(),
			CollisinManager::TAG_TYPE::PLAYER_WEPON,
			false
		);
	}


	//エネミー機能当たり判定
	auto& enemys = enemys_->GetEnemys();
	for (auto& enemy : enemys) {

		if (!enemy->IsAlive()) {
			continue;
		}

		collisin_.RegisterSphere(
			enemy,
			enemy->GetCillisionPos(),
			enemy->GetTransform().Radius_,
			CollisinManager::TAG_TYPE::ENEMY,
			true
		);

		auto& enemyWepon = enemy->GetUseWepons()->GetWepons();
		for (auto& wepon : enemyWepon) {

			if (!wepon->isAlive_) {
				continue;
			}

			collisin_.RegisterCapsule(
				wepon,
				wepon->GetStatePos(), 
				wepon->GetTransform().pos,
				wepon->GetColliderRadius(),
				CollisinManager::TAG_TYPE::ENEMY_WEPON,
				false
			);
		}
	}
}

void GameScene::HitCollisinUpdate(void)
{
	auto& collisin_ = CollisinManager::GetInstance();

	if(player_->GetState() != Player::STATE::KNOCKBACK
		|| player_->GetState() != Player::STATE::DEAD
		|| player_->GetState() != Player::STATE::END)
	{
		auto playerHitObj = collisin_.GetCollisionObject(player_);
		if (playerHitObj.hitType == CollisinManager::HIT_TYPE::PLAYER_ENEMY_HIT
			|| playerHitObj.hitType == CollisinManager::HIT_TYPE::ENEMY_WEPON_HIT) {
			player_->Damage(playerHitObj);
		}
	}

	auto& enemys = enemys_->GetEnemys();
	for (auto& enemy : enemys) {

		if (!enemy->IsAlive()) {
			continue;
		}

		auto enemyHitObj = collisin_.GetCollisionObject(enemy);
		if (enemyHitObj.hitType == CollisinManager::HIT_TYPE::PLAYER_ENEMY_HIT
			|| enemyHitObj.hitType == CollisinManager::HIT_TYPE::PLAYER_WEPON_HIT
			|| enemyHitObj.hitType == CollisinManager::HIT_TYPE::ENEMYS_HIT) {
			enemy->Damage(enemyHitObj);
		}
	}
}
