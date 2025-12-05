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

	//出現半径
	static constexpr float SPAWN_RANIUS = 5000.0f;

	// 重力
	static constexpr float GRAVITY = 5.5f;

	EnemyBase(void);
	~EnemyBase(void)override;

	//初期化処理
	virtual void Init(void)override;

	//出現処理
	void SetSpawnPostiton(void);

	void Damage(void)override;

	//HP描画
	void DrawHp(void)override;

protected:

	//敵の出現範囲
	float spawnRange_;
	//移動処理
	virtual void ProcessMove(void)override;
	//上昇処理
	virtual void ProcessRise(void)override;
	//対象ロック処理
	virtual void ProcessTargetLock(void)override;

    void UpdateWepon(void)override;

	// 状態遷移
	virtual void ChangeStandby(void);
	virtual void ChangeKnockback(void);
	virtual void ChangeAttack(void);
	virtual void ChangeDead(void);
	virtual void ChangeVictory(void);
	virtual void ChangeEnd(void);
	// 状態別更新
	virtual void UpdateStandby(void);
	virtual void UpdateKnockback(void);
	virtual void UpdateAttack(void);
	virtual void UpdateDead(void);
	virtual void UpdateVictory(void);
	virtual void UpdateEnd(void);
	// 状態別描画
	virtual void DrawStandby(void);
	virtual void DrawKnockback(void);
	virtual void DrawAttack(void);
	virtual void DrawDead(void);
	virtual void DrawVictory(void);
	virtual void DrawEnd(void);

private:

};

