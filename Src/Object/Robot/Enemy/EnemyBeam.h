#pragma once
#include <vector>
#include <memory>
#include <DxLib.h>
#include "EnemyBase.h"
class EnemyBeam :
    public EnemyBase
{
public:
	// 点滅時の拡散光色
	static constexpr COLOR_F COLOR_DIF_BLINK = { 1.0f, 0.2f, 0.2f, 1.0f };
	//初期サイズ
	static constexpr VECTOR ROBOT_DEF_SCL = { 0.35f,0.35f,0.35f };
	//初期相対角度
	static constexpr VECTOR LOCAL_DEF_ROT = { 0.0f,0.0f, 0.0f };
	//初期相対座標
	static constexpr VECTOR LOCAL_DEF_POS = { 0.0f,0.0f, 0.0f };
	//初期座標
	static constexpr VECTOR  DEFALUT_POS = { 0.0f,0.0f,1000.0f };
	//出現範囲
	static constexpr float SPAWN_RANGE = 1.2f;
	//衝突座標
	static constexpr VECTOR  COLLIDER_POS = { 0.0f,130.0f,0.0f };

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

	//エネミーの衝突半径
	static constexpr float DEFALUT_RADIUS = 70.0f;

	//HP
	static constexpr int DEFALUT_HP = 15;

	// 死亡時間
	static constexpr int CNT_DEAD_REACT = 40;

	EnemyBeam(void);
	~EnemyBeam(void)override;
	void Update(void)override;

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

private:

};

