#pragma once
#include <vector>
#include <memory>
#include <DxLib.h>
#include "../../../Application.h"
#include "EnemyBase.h"
class EnemyBeam :
    public EnemyBase
{
public:

	// 状態
	enum class STATE
	{
		NONE,
		THINK,
		IDLE,
		WANDER,
		END
	};

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
	static constexpr VECTOR ROBOT_DEF_SCL = { 0.35f,0.35f,0.35f };
	//初期相対角度
	static constexpr VECTOR LOCAL_DEF_ROT = { 0.0f, 180.0f * DX_PI_F / 180.0f, 0.0f };

	// 衝突判定用線分開始
	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 80.0f, 0.0f };
	// 衝突判定用線分終了
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };

	//出現範囲
	static constexpr float SPAWN_RANGE = 1.2f;

	//アニメーション再生速度
	static constexpr float DEFAULT_ANIMATION = 30.0f;
	// 弾発射後の硬直時間
	static constexpr float SHOT_DELAY = 7.0f;

	//ビーム出現数
	static constexpr int BEAM_CNT = 1;
	//HP
	static constexpr int DEFALUT_HP = 15;

	// テキスト調整値
	Vector2 HP_TEXT_OFFSET = { -200, -40 };

	// HPバー同士の間隔
	Vector2 HPBER_POS = { Application::SCREEN_SIZE_X / 2 ,  50 };
	// HPバーのサイズ
	Vector2 HPBER_SIZE = { 400, 15 };
	// P1のHPバーの色
	static constexpr unsigned int HPBER_COLOR = 0xFFFFFF;
	// P1のHPバー背景色
	static constexpr unsigned int HPBER_COLOR_BACK = 0xAAAAAA;

	EnemyBeam(void);
	~EnemyBeam(void)override;

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

	//攻撃処理
	void ProcessAttack(void)override;

	// 衝突判定
	void CollisionReserve(void) override;

private:

	// 衝突判定用カプセル上部球体(ジャンプ時)
	static constexpr VECTOR COL_CAPSULE_TOP_JUMP_LOCAL_POS =
	{ 0.0f, 240.0f, 0.0f };
	// 衝突判定用カプセル下部球体(ジャンプ時)
	static constexpr VECTOR COL_CAPSULE_DOWN_JUMP_LOCAL_POS =
	{ 0.0f, 80.0f, 0.0f };

	// 衝突判定用線分開始(ジャンプ時)
	static constexpr VECTOR COL_LINE_JUMP_START_LOCAL_POS = { 0.0f, 130.0f, 0.0f };
	// 衝突判定用線分終了(ジャンプ時)
	static constexpr VECTOR COL_LINE_JUMP_END_LOCAL_POS = { 0.0f, 50.0f, 0.0f };
	// 衝突判定用カプセル上部球体
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 180.0f, 0.0f };
	// 衝突判定用カプセル下部球体
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 30.0f, 0.0f };
	// 衝突判定用カプセル球体半径
	static constexpr float COL_CAPSULE_RADIUS = 35.0f;

	// 状態
	STATE state_;

	// 更新ステップ
	float step_;
	//状態変更カウント
	float stepCnt_;

	// 状態遷移
	void ChangeState(STATE state);
	void ChangeStateNone(void);
	void ChangeStateThink(void);
	void ChangeStateIdle(void);
	void ChangeStateWander(void);
	void ChangeStateEnd(void);

	// 更新系
	void UpdateNone(void);
	void UpdateThink(void);
	void UpdateIdle(void);
	void UpdateWander(void);
	void UpdateEnd(void);
};

