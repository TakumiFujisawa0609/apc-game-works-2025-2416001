#pragma once
#include <vector>
#include <memory>
#include <DxLib.h>
#include <functional>
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

	//出現半径
	static constexpr float SPAWN_RANIUS = 500.0f;

	EnemyBase(void);
	~EnemyBase(void)override;

	//初期化処理
	virtual void Init(void)override;

	//出現処理
	void SetSpawnPostiton(void);


protected:

	// 状態管理
	int stateBase_;
	// 状態管理(状態遷移時初期処理)
	std::map<int, std::function<void(void)>> stateChanges_;
	// 状態管理(更新ステップ)
	std::function<void(void)> stateUpdate_;

	//敵の出現範囲
	float spawnRange_;
	//移動可能範囲
	float moveRadius_;

	//移動処理
	void ProcessMove(void)override;
	//上昇処理
	virtual void ProcessRise(void) {}
	//攻撃処理
	virtual void ProcessAttack(void) {}
	//対象ロック処理
	void ProcessTargetLock(void)override;

	// 状態遷移
	void ChangeState(int state);

	// 更新系
	virtual void UpdateProcessPost(void) override {}

	// 移動可能範囲判定
	/*bool InMovableRange(void) const;*/
};

