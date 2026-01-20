#pragma once
#include <functional>
#include <vector>
#include <memory>
#include <DxLib.h>
#include "../../../Application.h"
#include "../RobotBase.h"

class Camera;

class Player
	:public RobotBase
{
public:

	//アニメーション
	enum class ANIM_TYPE
	{
		GESUTYE = 0,
		DEATH = 1,
		HIT_REACT = 3,
		IDLE = 5,
		JUMP = 6,
		RUN = 11,
		ATTACK = 14,
		WALK = 16,
		MAX,
	};

	enum class STATE
	{
		NONE,
		IDLE,
		DAMAGE,
		DEAD,
		END
	};

	//初期サイズ
	static constexpr VECTOR DEFALUT_SCL = { 0.35f,0.35f,0.35f };
	//初期相対角度
	static constexpr VECTOR LOCAL_DEF_ROT = { 0.0f,180.0f * DX_PI_F / 180.0f, 0.0f };
	//初期座標
	static constexpr VECTOR  DEFALUT_POS = { 0.0f,100.0f,0.0f };

	// 衝突判定用線分開始
	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 80.0f, 0.0f };
	// 衝突判定用線分終了
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };


	// 移動速度(通常)
	static constexpr float SPEED_MOVE = 20.0f;
	// 移動速度(ダッシュ)
	static constexpr float SPEED_DASH = 30.0f;
	// ジャンプ力
	static constexpr float POW_JUMP_INIT = 5500.0f;
	// 持続ジャンプ力
	static constexpr float POW_JUMP_KEEP = 400.0f;
	// ジャンプ受付時間
	static constexpr float TIME_JUMP_INPUT = 0.5f;

	//アニメーションの再生速度
	static constexpr float DEFAULT_ANIMATION = 30.0f;
	// 弾発射後の硬直時間
	static constexpr float SHOT_DELAY = 1.0f;
	//HP
	static constexpr int DEFALUT_HP = 1000;


	// テキスト調整値
	Vector2 HP_TEXT_OFFSET = { -200, -40 };

	// HPバー同士の間隔
	Vector2 HPBER_POS = { 50,  Application::SCREEN_SIZE_Y - 150 };

	// HPバーのサイズ
	Vector2 HPBER_SIZE = { 400, 25 };

	// P1のHPバーの色
	static constexpr unsigned int HPBER_COLOR = 0xFFFFFF;

	// P1のHPバー背景色
	static constexpr unsigned int HPBER_COLOR_BACK = 0xAAAAAA;


	//ビーム出現数
	static constexpr int BEAM_CNT = 1;
	//ミサイル出現数
	static constexpr int MISSILE_CNT = 10;


	Player(void);
	~Player(void)override;

	void Debug(void)override;

	void SetCamera(Camera* camera);

protected:

	// リソースロード
	void InitLoad(void)override;
	// 大きさ、回転、座標の初期化
	void InitTransform(void)override;
	// 衝突判定の初期化
	void InitCollider(void)override;
	// アニメーションの初期化
	void InitAnimation(void)override;
	// 初期化後の個別処理
	void InitPost(void)override;

	// 更新系
	void UpdateProcess(void)override;
	void UpdateProcessPost(void)override;

	//移動処理
	void ProcessMove(void)override;
	//上昇処理
	void ProcessRise(void)override;
	//攻撃処理
	void ProcessAttack(void)override;
	//対象ロック処理
	void ProcessTargetLock(void)override;

	// 衝突判定
	void CollisionReserve(void) override;

	// ダメージ処理用
	void TakeDamage(float damage)override;
	bool IsInvincible(void) const;

private:

	// 衝突判定用カプセル上部球体(ジャンプ時)
	static constexpr VECTOR COL_CAPSULE_TOP_JUMP_LOCAL_POS =
	{ 0.0f, 210.0f, 0.0f };
	// 衝突判定用カプセル下部球体(ジャンプ時)
	static constexpr VECTOR COL_CAPSULE_DOWN_JUMP_LOCAL_POS =
	{ 0.0f, 80.0f, 0.0f };

	// 衝突判定用線分開始(ジャンプ時)
	static constexpr VECTOR COL_LINE_JUMP_START_LOCAL_POS = { 0.0f, 130.0f, 0.0f };
	// 衝突判定用線分終了(ジャンプ時)
	static constexpr VECTOR COL_LINE_JUMP_END_LOCAL_POS = { 0.0f, 50.0f, 0.0f };
	// 衝突判定用カプセル上部球体
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 150.0f, 0.0f };
	// 衝突判定用カプセル下部球体
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 30.0f, 0.0f };
	// 衝突判定用カプセル球体半径
	static constexpr float COL_CAPSULE_RADIUS = 35.0f;

	//状態
	STATE state_;

	// 状態管理(状態遷移時初期処理)
	std::map<int, std::function<void(void)>> stateChanges_;

	// 状態管理(更新ステップ)
	std::function<void(void)> stateUpdate_;

	// 更新ステップ
	float step_;

	// 状態遷移
	void ChangeState(STATE state);
	void ChangeNone(void);
	void ChangeIdle(void);
	void ChangeDamage(void);
	void ChangeDead(void);
	void ChangeEnd(void);

	// 更新系
	void UpdateNone(void);
	void UpdateIdle(void);
	void UpdateDamage(void);
	void UpdateDead(void);
	void UpdateEnd(void);


	//カメラの角度
	Camera* camera_;

	bool debug_;

	static constexpr float INVINCIBLE_DURATION = 1.0f;  // 無敵時間（秒）
	static constexpr float KNOCKBACK_DURATION = 0.3f;   // ノックバック時間（秒）
	static constexpr float KNOCKBACK_STRENGTH = 500.0f; // ノックバック初速


};

