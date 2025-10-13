#pragma once
#include <vector>
#include <memory>
#include <DxLib.h>
#include "../RobotBase.h"

class EnemyBase
	:public RobotBase
{
public:

	//エネミー種類
	enum class ENEMY_TYPE
	{
		BEME,
		MISSILE
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

	EnemyBase(void);
	~EnemyBase(void)override;
	virtual void Update(void)override = 0;

protected:

	// リソースロード
	virtual void InitLoad(void)override = 0;
	// 大きさ、回転、座標の初期化
	virtual void InitTransform(void)override = 0;
	// アニメーションの初期化
	virtual void InitAnimation(void)override = 0;
	// 初期化後の個別処理
	virtual void InitPost(void)override = 0;
	//移動処理
	virtual void ProcessMove(void)override = 0;
	//上昇処理
	virtual void ProcessRise(void)override = 0;
	//攻撃処理
	virtual void ProcessAttack(void)override = 0;
	//対象ロック処理
	virtual void ProcessTargetLock(void)override = 0;

	// 状態遷移
	virtual void ChangeStandby(void) = 0;
	virtual void ChangeKnockback(void) = 0;
	virtual void ChangeAttack(void) = 0;
	virtual void ChangeDead(void) = 0;
	virtual void ChangeVictory(void) = 0;
	virtual void ChangeEnd(void) = 0;
	// 状態別更新
	virtual void UpdateStandby(void) = 0;
	virtual void UpdateKnockback(void) = 0;
	virtual void UpdateAttack(void) = 0;
	virtual void UpdateDead(void) = 0;
	virtual void UpdateVictory(void) = 0;
	virtual void UpdateEnd(void) = 0;
	// 状態別描画
	virtual void DrawStandby(void) = 0;
	virtual void DrawKnockback(void) = 0;
	virtual void DrawAttack(void) = 0;
	virtual void DrawDead(void) = 0;
	virtual void DrawVictory(void) = 0;
	virtual void DrawEnd(void) = 0;

private:

};

