#pragma once
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

	//初期サイズ
	static constexpr VECTOR DEFALUT_SCL = { 0.35f,0.35f,0.35f };
	//初期相対角度
	static constexpr VECTOR LOCAL_DEF_ROT = { 0.0f,180.0f * DX_PI_F / 180.0f, 0.0f };
	//初期座標
	static constexpr VECTOR  DEFALUT_POS = { 0.0f,0.0f,0.0f };
	//衝突座標
	static constexpr VECTOR  COLLIDER_POS = { 0.0f,120.0f,0.0f };


	//最大回転量
	static constexpr float MAX_MOVE_ROT = 15.0f * DX_PI_F / 180.0f;
	//ブースト時最大回転量
	static constexpr float MAX_BOOST_ROT = 40.0f * DX_PI_F / 180.0f;
	//回転量
	static constexpr float ROT_POW = 2.5f * DX_PI_F / 180.0f;
	//加速量
	static constexpr float MOVE_SPEED = 0.95f;
	//上昇量
	static constexpr float RISE_SPEED = 10.5f;
	//ブースト量
	static constexpr float BUST_SPEED = 10.0f;
	//最大加速量
	static constexpr float MAX_MOVE_SPEED = 20.0f;
	//最大ブースト量
	static constexpr float MAX_BUST_SPEED = 30.0f;
	//摩擦係数
	static constexpr float FRICTION = 0.96f;
	//停止判定の値
	static constexpr float STOP_THRESHOLD = 0.1f;
	// 重力
	static constexpr float GRAVITY = 5.5f;
	//アニメーションの再生速度
	static constexpr float DEFAULT_ANIMATION = 30.0f;
	//プレイヤーのカメラ半径
	static constexpr float MAX_ROBOT_ANGLES = 40.0f * DX_PI_F / 180.0f;
	//プレイヤーの衝突半径
	static constexpr float DEFALUT_RADIUS = 60.0f;
	// 弾発射後の硬直時間
	static constexpr float SHOT_DELAY = 1.0f;
	//HP
	static constexpr int DEFALUT_HP = 50;


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

	void SetCamera(Camera* camera);

	const float& GetDegreep(void) const;

	void Damage(CollisinManager::HitObject hitObject)override;

	//HP描画
	void DrawHp(void)override;

protected:

	// リソースロード
	void InitLoad(void)override;
	// 大きさ、回転、座標の初期化
	void InitTransform(void)override;
	// アニメーションの初期化
	void InitAnimation(void)override;
	// 初期化後の個別処理
	void InitPost(void)override;
	//移動処理
	void ProcessMove(void)override;
	//上昇処理
	void ProcessRise(void)override;
	//攻撃処理
	void ProcessAttack(void)override;
	//対象ロック処理
	void ProcessTargetLock(void)override;


	// 状態遷移
	void ChangeStandby(void)override;
	void ChangeKnockback(void)override;
	void ChangeAttack(void)override;
	void ChangeDead(void)override;
	void ChangeVictory(void)override;
	void ChangeEnd(void)override;

	// 状態別更新
	void UpdateStandby(void)override;
	void UpdateKnockback(void)override;
	void UpdateAttack(void)override;
	void UpdateDead(void)override;
	void UpdateVictory(void)override;
	void UpdateEnd(void)override;

	// 状態別描画
	void DrawStandby(void)override;
	void DrawKnockback(void)override;
	void DrawAttack(void)override;
	void DrawDead(void)override;
	void DrawVictory(void)override;
	void DrawEnd(void)override;

private:

	//カメラの角度
	Camera* camera_;

};

