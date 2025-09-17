#pragma once
#include <DxLib.h>
#include <memory>
#include "../ObjectBase.h"

class WeponBase;
class WeponBeam;

class RobotBase
	:public ObjectBase
{
public:

	// 状態
	enum class STATE
	{
		NONE,
		STANDBY,
		KNOCKBACK,
		ATTACK,
		DEAD,
		END,
		VICTORY,
	};

	//初期位置
	static constexpr VECTOR ROBOT_DEF_POS = { 0.0f,0.0f,0.0f };
	//初期サイズ
	static constexpr VECTOR ROBOT_DEF_SCL = { 0.1f,0.1f,0.1f };
	//初期角度
	static constexpr VECTOR ROBOT_DEF_ROT = { 0.0f,180.0f * DX_PI_F / 180.0f, 0.0f };

	//最大回転量
	static constexpr float MAX_MOVE_ROT = 15.0f * DX_PI_F / 180.0f;

	//移動量
	static constexpr float POS_POW = 10.0f;
	//回転量
	static constexpr float ROT_POW = 2.5f * DX_PI_F / 180.0f;

	//ロボットのカメラ半径
	static constexpr float ROBOT_CAMERA_RAG = 300.0f;
	static constexpr float MAX_ROBOT_ANGLES = 40.0f * DX_PI_F / 180.0f;

	// コンストラクタ
	RobotBase(void);

	// デストラクタ
	~RobotBase(void);

	//初期化処理
	void Init(void)override;
	//更新処理
	void Update(void)override;
	//描画処理
	void Draw(void)override;
	//解放処理
	void Release(void)override;

	// 状態遷移
	void ChangeState(STATE state);

private:

	// 武器
	std::unique_ptr<WeponBeam> weponbeam_;

	//状態
	STATE state_;

	//移動量
	float posPow_;
	//回転量
	float rotPow_;


	//移動処理
	void ProcessMove(void);
	//ジャンプ処理
	void ProcessJump(void);

	// 状態遷移
	void ChangeStandby(void);
	void ChangeKnockback(void);
	void ChangeAttack(void);
	void ChangeDead(void);
	void ChangeVictory(void);
	void ChangeEnd(void);
	// 状態別更新
	void UpdateStandby(void);
	void UpdateKnockback(void);
	void UpdateAttack(void);
	void UpdateDead(void);
	void UpdateVictory(void);
	void UpdateEnd(void);
	// 状態別描画
	void DrawStandby(void);
	void DrawKnockback(void);
	void DrawAttack(void);
	void DrawDead(void);
	void DrawVictory(void);
	void DrawEnd(void);
};

