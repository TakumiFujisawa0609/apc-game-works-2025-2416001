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


protected:

	//敵の出現範囲
	float spawnRange_;

	// 更新系
	void UpdateProcess(void)override;
	void UpdateProcessPost(void)override;

	//移動処理
	void ProcessMove(void)override;
	//上昇処理
	virtual void ProcessRise(void) {}
	//攻撃処理
	virtual void ProcessAttack(void) {}
	//対象ロック処理
	void ProcessTargetLock(void)override;

private:

};

