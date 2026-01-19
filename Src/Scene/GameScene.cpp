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
#include "../Object/ObjectBase.h"
#include "../Object/Robot/Player/Player.h"
#include "../Object/Robot/Enemy/EnemyBase.h"
#include "../Object/Common/Collider/ColliderBase.h"
#include "../Object/Stage/SkyDome.h"
#include "../Object/Stage/Stage.h"
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

	//ステージ初期化処理
	stage_ = std::make_shared<Stage>();
	skydome_ = std::make_shared<SkyDome>();
	stage_->Init();
	skydome_->Init();

	//プレイヤー初期化処理
	player_ = std::make_shared<Player>();
	camera->SetFollow(&player_.get()->GetTransform());
	player_->Init();
	player_->SetCamera(SceneManager::GetInstance().GetCamera());

	//エネミー初期化処理
	enemys_ = std::make_shared<EnemyManager>();
	enemy_ = nullptr;
	enemys_->Init();

	//グリッド初期化処理
	grid_ = std::make_unique<Grid>();

	camera->ChangeMode(Camera::MODE::FOLLOW);

	// Stageクラスのコライダーを各クラスに登録
	const ColliderBase* stageCollider =
		stage_->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::MODEL));
	player_->AddHitCollider(stageCollider);
	enemys_->AddHitCollider(stageCollider);
	camera->AddHitCollider(stageCollider);

	// Cameraクラスのコライダーを各クラスに登録
	const ColliderBase* cameraCollider =
		camera->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::SPHERE));
	stage_->AddHitCollider(cameraCollider);
}

void GameScene::Update(void)
{
	stage_->Update();
	skydome_->Update();

	//ロボット更新処理
	player_->Update();
	enemys_->Update();

	const ColliderBase* stageCollider =
		stage_->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::MODEL));
	enemys_->AddHitCollider(stageCollider);

	// Weponクラスのコライダーを各クラスに登録
	//プレイヤーの武器情報を取得
	const auto& playerWepons = player_->GetUseWepons()->GetWepons();
	const auto& enemys = enemys_->GetEnemys();
	for(auto& enemy: enemys)
	{ 
		if (enemy == nullptr)continue;

		//エネミーの武器情報を取得
		const auto& enemyWepons = enemy->GetUseWepons()->GetWepons();
		for (auto& enemyWepon : enemyWepons)
		{
			if (enemyWepon == nullptr) continue;

			const ColliderBase* enemyWeponCollider =
				enemyWepon->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::SPHERE));
			player_->AddHitCollider(enemyWeponCollider);
		}

		for (auto& playerWepon : playerWepons)
		{
			if (playerWepon == nullptr)continue;

			const ColliderBase* playerWeponCollider =
				playerWepon->GetOwnCollider(static_cast<int>(ColliderBase::SHAPE::SPHERE));
			enemy->AddHitCollider(playerWeponCollider);
		}
	}

	//ロック対象更新
	UpdateAutoLockOn();
}

void GameScene::Draw(void)
{
	skydome_->Draw();
	stage_->Draw();

	//ロボット描画処理
	player_->Draw();

	enemys_->Draw();

#ifdef _DEBUG
	////グリッド描画処理
	//grid_->Draw();

	DrawFormatString(
		0, 20, GetColor(255, 255, 255),
		"GameScene");

	DrawFormatString(0, 40, 0xFFFFFF,
		"Player(%.1f, %.1f, %.1f)",
		player_->GetTransform().pos.x,
		player_->GetTransform().pos.y,
		player_->GetTransform().pos.z);

	for(auto enemy : enemys_->GetEnemys())
	{
		DrawFormatString(0, 60, 0xFFFFFF,
			"Enemy(%.1f, %.1f, %.1f)",
			enemy->GetTransform().pos.x,
			enemy->GetTransform().pos.y,
			enemy->GetTransform().pos.z);
	}

#endif
}

void GameScene::Release(void)
{
	stage_->Release();
	skydome_->Release();

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

	if (enemy_ != nullptr && (!enemy_->GetIsAlive() || enemy_->GetHp() <= 0)) {
		enemy_ = nullptr;
		camera->ChangeMode(Camera::MODE::FOLLOW);
	}

	// 現在のモードと違う場合のみ変更する
	if (player_->IsTargetLockFlage()) {
		if (camera->GetCameraMode() != Camera::MODE::TARGET_ROCKE) {
			camera->ChangeMode(Camera::MODE::TARGET_ROCKE);
		}
	}
	else {
		if (camera->GetCameraMode() != Camera::MODE::FOLLOW) {
			camera->ChangeMode(Camera::MODE::FOLLOW);
		}
	}

	bool needNewTarget;

	// キー入力によるカメラの回転
	auto& ins = InputManager::GetInstance();
	if (GetJoypadNum() == 0)
	{
		needNewTarget = (enemy_ == nullptr) || inp.IsTrgDown(KEY_INPUT_L);
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
		float min = 5000.0f;

		for (auto& enemy : enemys) {

			if (!enemy->GetIsAlive() || enemy->GetHp() <= 0) continue;
			if (needNewTarget && enemy == enemy_) continue; 
			float diff = VSize(VSub(enemy->GetTransform().pos, player_->GetTransform().pos));

			if (diff < min) {
				min = diff;
				newTarget = enemy;
			}
		}
		enemy_ = newTarget;
		if (enemy_ == nullptr) {
			camera->ChangeMode(Camera::MODE::FOLLOW);
		}
	}

	//対象の情報を格納
	if (enemy_ != nullptr) {
		camera->SetTargetFollow(&enemy_->GetTransform());
		player_->SetLockOnPos(enemy_->GetTransform().pos);
	}
}