#pragma once
#include <vector>
#include <memory>
#include <DxLib.h>
#include "../../Common/Vector2.h"
#include "../ObjectBase.h"
#include "../Wepon/WeponBase.h"
#include "../../Utility/AsoUtility.h"
#include "../Manager/CollisinManager.h"

class WeponBase;
class WeponManager;
class AnimationController;
class HpBer;

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
	//HP描画
	virtual void DrawHp(void) = 0;
	//解放処理
	void Release(void)override;

	//ロック機能入力フラグ
	bool IsTargetLockFlage(void);

	//ロックオン座標の設定
	void SetLockOnPos(VECTOR lockOnPos = AsoUtility::VECTOR_ZERO);

	// ダメージを与える
	virtual void Damage(CollisinManager::HIT_TYPE type) = 0;

	float GetHp(void) { return hp_; }

	const std::shared_ptr<WeponManager>& GetUseWepons(void) const { return useWepon_; }

	// 衝突判定が有効な状態
	bool IsCollisionState(void);

	//敵の生存判定
	bool IsAlive(void);

	const STATE& GetState(void) const { return state_; }

	//当たり判定半径取得
	const float& GetCillisionRadius(void) { return trans_.Radius_; }

	//当たり判定座標取得
	const VECTOR& GetCillisionPos(void)const {return VAdd(trans_.cillisionPos, trans_.pos); }

protected:

	//// 武器
	//std::unique_ptr<WeponBase> useWepon_;
	//std::unique_ptr<WeponBeam> weponbeam_;
	//std::vector<std::unique_ptr<WeponMissile>> weponMissile_;
	std::shared_ptr<WeponManager> useWepon_;

	//アニメション
	std::unique_ptr<AnimationController> anim_;

	HpBer* hpBer_;

	//状態
	STATE state_;
	//ロックオン座標
	VECTOR lockOnPos_;

	//上昇量
	float rise_;
	//移動量
	float movePow_;
	//回転量
	float rotPow_;

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

	// 状態遷移
	virtual void ChangeState(STATE state);

	// リソースロード
	virtual void InitLoad(void) = 0;
	// 大きさ、回転、座標の初期化
	virtual void InitTransform(void) = 0;
	// 大きさ、回転、座標のモデル設定
	void InitTransformPost(void);
	// アニメーションの初期化
	virtual void InitAnimation(void) = 0;
	// 初期化後の個別処理
	virtual void InitPost(void) = 0;

	//移動処理
	virtual void ProcessMove(void) = 0;
	//キャラの遅延回転処理
	void DelayRotate(void);
	//上昇処理
	virtual void ProcessRise(void) = 0;
	//攻撃処理
	virtual void ProcessAttack(void) = 0;
	//対象ロック処理
	virtual void ProcessTargetLock(void) = 0;

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
};

