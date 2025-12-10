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

	//初期サイズ
	static constexpr VECTOR ROBOT_DEF_SCL = { 0.35f,0.35f,0.35f };
	//初期相対角度
	static constexpr VECTOR LOCAL_DEF_ROT = { 0.0f, 180.0f * DX_PI_F / 180.0f, 0.0f };
	//初期座標
	static constexpr VECTOR DEFALUT_POS = { 0.0f,0.0f,1000.0f };
	// 衝突判定用線分開始
	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 80.0f, 0.0f };
	// 衝突判定用線分終了
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };


	//出現範囲
	static constexpr float SPAWN_RANGE = 1.2f;
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
	//アニメーション再生速度
	static constexpr float DEFAULT_ANIMATION = 30.0f;

	// 弾発射後の硬直時間
	static constexpr float SHOT_DELAY = 7.0f;

	//ビーム出現数
	static constexpr int BEAM_CNT = 1;

	//HP
	static constexpr int DEFALUT_HP = 15;
	// 死亡時間
	static constexpr int CNT_DEAD_REACT = 40;

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
	{ 0.0f, 160.0f, 0.0f };
	// 衝突判定用カプセル下部球体(ジャンプ時)
	static constexpr VECTOR COL_CAPSULE_DOWN_JUMP_LOCAL_POS =
	{ 0.0f, 80.0f, 0.0f };

	// 衝突判定用線分開始(ジャンプ時)
	static constexpr VECTOR COL_LINE_JUMP_START_LOCAL_POS = { 0.0f, 130.0f, 0.0f };
	// 衝突判定用線分終了(ジャンプ時)
	static constexpr VECTOR COL_LINE_JUMP_END_LOCAL_POS = { 0.0f, 50.0f, 0.0f };
	// 衝突判定用カプセル上部球体
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 110.0f, 0.0f };
	// 衝突判定用カプセル下部球体
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 30.0f, 0.0f };
	// 衝突判定用カプセル球体半径
	static constexpr float COL_CAPSULE_RADIUS = 20.0f;

};

