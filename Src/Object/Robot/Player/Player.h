#pragma once
#include <vector>
#include <memory>
#include <DxLib.h>
#include "../RobotBase.h"

class Camera;

class Player
	:public RobotBase
{
public:

	//初期位置
	static constexpr VECTOR ROBOT_DEF_POS = { 0.0f,0.0f,0.0f };
	//初期サイズ
	static constexpr VECTOR ROBOT_DEF_SCL = { 0.1f,0.1f,0.1f };
	//初期相対角度
	static constexpr VECTOR LOCAL_DEF_ROT = { 0.0f,180.0f * DX_PI_F / 180.0f, 0.0f };
	//初期相対座標
	static constexpr VECTOR  LOCAL_DEF_POS = { 0.0f,0.0f,500.0f };


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

	//ロボットのカメラ半径
	static constexpr float MAX_ROBOT_ANGLES = 40.0f * DX_PI_F / 180.0f;

	Player(void);
	~Player(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void SetCamera(Camera* camera);

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

