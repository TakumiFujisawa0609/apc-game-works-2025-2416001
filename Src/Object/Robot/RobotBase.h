#pragma once
#include <DxLib.h>
#include <memory>
#include "../ObjectBase.h"

class WeponBase;
class WeponBeam;
class WeponMissile;
class AnimationController;
class Camera;

class RobotBase
	:public ObjectBase
{
public:

	//アニメーション
	enum class ANIM_TYPE
	{
		WIKE
	};


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
	//初期相対角度
	static constexpr VECTOR LOCAL_DEF_ROT = { 0.0f,180.0f * DX_PI_F / 180.0f, 0.0f };
	//初期相対座標
	static constexpr VECTOR  LOCAL_DEF_POS = { 0.0f,0.0f,500.0f };

	//デバック用敵円座標
	static constexpr VECTOR DEBUG_SPHERE_POS = { 0.0f,50.0f,1000.0f };

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

	void SetCamera(Camera* camera);

	//ロック機能入力フラグ
	bool IsTargetLockFlage(void);

	//デバッグ円座標取得関数
	const VECTOR GetDebugSpherePos(void);

private:

	// 武器
	std::unique_ptr<WeponBeam> weponbeam_;

	std::unique_ptr<WeponMissile> weponMissile_;

	//アニメション
	AnimationController* anim_;

	//状態
	STATE state_;

	//カメラの角度
	Camera* camera_;

	//デバッグ用円座標
	VECTOR debugSpherePos_;

	//上昇量
	float rise_;
	//移動量
	float movePow_;
	//回転量
	float rotPow_;
	
	//デバッグ用フラグ
	bool deBugLeft = false;
	bool deBugRight = true;

	//ロック機能入力カウント
	int lockcnt;

	//移動処理
	void ProcessMove(void);
	// プレイヤーの遅延回転処理
	void DelayRotate(void);
	//上昇処理
	void ProcessRise(void);
	//攻撃処理
	void ProcessAttack(void);
	//対象ロック処理
	void ProcessTargetLock(void);

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

