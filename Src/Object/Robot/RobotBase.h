#pragma once
#include <vector>
#include <memory>
#include <DxLib.h>
#include "../ObjectBase.h"

class WeponBase;
class WeponBeam;
class WeponMissile;
class AnimationController;

class RobotBase
	:public ObjectBase
{
public:

	//デバック用敵円座標
	static constexpr VECTOR DEBUG_SPHERE_POS = { 0.0f,50.0f,1000.0f };

	// コンストラクタ
	RobotBase(void);
	// デストラクタ
	virtual ~RobotBase(void);
	//初期化処理
	void Init(void)override;
	//更新処理
	virtual void Update(void)override;
	//描画処理
	virtual void Draw(void)override;
	//解放処理
	virtual void Release(void)override;

	//ロック機能入力フラグ
	bool IsTargetLockFlage(void);
	//デバッグ円座標取得関数
	const VECTOR* GetDebugSpherePos(void);

protected:

	// 武器
	std::unique_ptr<WeponBeam> weponbeam_;
	std::vector<std::unique_ptr<WeponMissile>> weponMissile_;

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
};

