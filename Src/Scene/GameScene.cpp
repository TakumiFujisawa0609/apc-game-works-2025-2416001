#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/Camera.h"
#include "../Object/Common/EffectController.h"
#include "../Object/Common/Transform.h"
#include "../Object/Common/Grid.h"
#include "../Object/Manager/EnemyManager.h"
#include "../Object/Manager/CollisionManager.h"
#include "../Object/Manager/WeponManager.h"
#include "../Object/ObjectBase.h"
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


	InitCollision();
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

	//ロック対象更新
	UpdateAutoLockOn();
}

void GameScene::Draw(void)
{
	//グリッド描画処理
	grid_->Draw();

	//ロボット描画処理
	player_->Draw();
	if(player_->IsAlive()){
		player_->DrawHp();
	}

	enemys_->Draw();
	Camera* camera = SceneManager::GetInstance().GetCamera();
	if (enemy_ != nullptr && camera->GetCameraMode() == Camera::MODE::TARGET_ROCKE) {
		enemy_->DrawHp();
	}

#ifdef _DEBUG
	DrawFormatString(
		0, 20, GetColor(255, 255, 255),
		"GameScene");
#endif
}

void GameScene::Release(void)
{
	//ロボット解放処理
	player_->Release();
	//エネミー解放処理
	enemys_->Release();
}

void GameScene::InitCollision(void)
{
	std::vector<const ColliderBase*> colliders;

	colliders.emplace_back(
		player_->GetOwnCollider(static_cast<int>(ObjectBase::COLLIDER_TYPE::MODEL)));

	auto& enemys = enemys_->GetEnemys();
	for (auto& enemy : enemys)
	{
		colliders.emplace_back(
			enemy->GetOwnCollider(static_cast<int>(ObjectBase::COLLIDER_TYPE::MODEL)));
	}

	player_->AddHitCollider(colliders);
	enemy_->AddHitCollider(colliders);
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