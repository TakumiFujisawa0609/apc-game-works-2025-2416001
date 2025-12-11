#pragma once
#include <vector>
#include <memory>
#include <DxLib.h>
#include "../../Common/Vector2.h"
#include "../ObjectBase.h"
#include "../Wepon/WeponBase.h"
#include "../../Utility/AsoUtility.h"

class WeponBase;
class WeponManager;
class AnimationController;
class HpBer;

class RobotBase
	:public ObjectBase
{
public:

	// 衝突判定種別
	enum class COLLIDER_TYPE
	{
		LINE,
		CAPSULE,
		MAX,
	};

	// コンストラクタ
	RobotBase(void);
	// デストラクタ
	virtual ~RobotBase(void);
	//初期化処理
	virtual void Init(void)override;
	//更新処理
	void Update(void)override;
	//描画処理
	void Draw(void)override;
	//解放処理
	void Release(void)override;

	virtual void Debug(void) {}

	//ロック機能入力フラグ
	bool IsTargetLockFlage(void);

	//ロックオン座標の設定
	void SetLockOnPos(VECTOR lockOnPos = AsoUtility::VECTOR_ZERO);

	float GetHp(void) { return hp_; }

	const std::shared_ptr<WeponManager>& GetUseWepons(void) const { return useWepon_; }

protected:

	// 最大落下速度
	static constexpr float MAX_FALL_SPEED = -30.0f;
	// 衝突時の押し戻し試行回数
	static constexpr int CNT_TRY_COLLISION = 20;
	// 衝突時の押し戻し量
	static constexpr float COLLISION_BACK_DIS = 1.0f;

	// 武器
	std::shared_ptr<WeponManager> useWepon_;
	Transform weponModel;

	//アニメション
	std::unique_ptr<AnimationController> anim_;

	HpBer* hpBer_;

	//ロックオン座標
	VECTOR lockOnPos_;

	// 移動量
	VECTOR movePow_;
	// 移動前の座標
	VECTOR prevPos_;
	// ジャンプ量
	VECTOR jumpPow_;
	// 移動スピード
	float moveSpeed_;
	// ジャンプの入力受付時間
	float stepJump_;
	// ジャンプ判定
	bool isJump_;

	//ロック機能入力カウント
	int lockcnt;
	// HP
	int hp_;
	//ビーム出現数
	int beamCnt_;
	//ミサイル出現数
	int missileCnt_;
	//被ダメージ用一定間隔カウンタ
	int cntHitReact_;
	//撃破用一定間隔カウンタ
	int cntDeadReact_;

	int maxHp_;
	Vector2 hpTextOffset_;
	Vector2 hpScl_;
	unsigned int hpCol_;
	unsigned int hpBackCol_;

	// 弾発射後の硬直時間計算用
	float stepShotDelay_;

	// 更新系
	virtual void UpdateProcess(void) = 0;
	virtual void UpdateProcessPost(void) = 0;

	//移動処理
	virtual void ProcessMove(void) = 0;
	//上昇処理
	virtual void ProcessRise(void) {}
	//攻撃処理
	virtual void ProcessAttack(void) = 0;
	//対象ロック処理
	virtual void ProcessTargetLock(void) = 0;

	//キャラの遅延回転処理
	void DelayRotate(void);
	// 重力計算
	void CalcGravityPow(void);

	// 衝突判定
	virtual void CollisionReserve(void) {}
	void Collision(void);
	void CollisionGravity(void);
	void CollisionCapsule(void);
};

