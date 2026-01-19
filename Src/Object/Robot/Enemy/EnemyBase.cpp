#include <memory>
#include "../../../Application.h"
#include "../../../Utility/MatrixUtility.h"
#include "../../../Utility/AsoUtility.h"
#include "../../Common/AnimationController.h"
#include "../../Common/EffectController.h"
#include "../../Manager/WeponManager.h"
#include "../../Common/HpBer.h"
#include "EnemyBase.h"

EnemyBase::EnemyBase(void)
{
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Init(void)
{
    ObjectBase::Init();

    useWepon_ = std::make_shared<WeponManager>();
    useWepon_->Init();

    hpBer_ = new HpBer(maxHp_, hpTextOffset_, hpScl_, hpCol_, hpBackCol_, -1);

    // 弾発射の硬直時間
    stepShotDelay_ = 0.0f;

    eff_ = std::make_unique<EffectController>();
    eff_->Add(1, Application::PATH_EFFECT + "ToonHit.efkefc");
    eff_->Play(1);
}

void EnemyBase::SetSpawnPostiton(void)
{
    //出現半径
    float radius = SPAWN_RANIUS;
    //出現中心座標
    VECTOR spawnPos = { 0.0f, 100.0f, 0.0f };
    //長さを設定
    radius *= spawnRange_;

    //ランダムな数値を取得する(0～200)
    int degree = GetRand(360);

    float radian =
        static_cast<float>(degree) * DX_PI_F / 180.0f;

    //角度から方向を求める
    float dirX = sinf(radian);
    float dirZ = cosf(radian);
    VECTOR dir = { dirX, 0.0f, dirZ };

    //方向X長さ（長さが半径分のランダムなベクトル）
    VECTOR vec = VScale(dir, radius);

    //ステージの中心座標にベクトルを加算して
    //円状の座標を計算する
    VECTOR pos = VAdd(spawnPos, vec);

    trans_.pos = pos;
}

void EnemyBase::ProcessMove(void)
{
    //移動量を常に減少
    moveSpeed_ = 10.0f;

    movePow_ = VScale(trans_.targetDir, moveSpeed_);
}

void EnemyBase::ProcessTargetLock(void)
{
    // デバッグ球体へのベクトルを計算
    VECTOR toTarget = VSub(lockOnPos_, trans_.pos);

    // ターゲットまでの距離をチェック（ゼロ除算回避）
    float distance = VSize(toTarget);
    if (distance < 0.01f) {
        return;
    }

    // ターゲット方向の正規化
    trans_.targetDir = VNorm(toTarget);

    // ターゲット方向からY軸回転角度を計算（水平方向）
    float targetAngleY = atan2f(trans_.targetDir.x, trans_.targetDir.z);

    // ターゲット方向のX軸回転角度を計算（上下の角度）
    float horizontalDist = sqrtf(
        trans_.targetDir.x * trans_.targetDir.x 
        + trans_.targetDir.z * trans_.targetDir.z);

    // 滑らかに回転させる
    trans_.rot.y = AsoUtility::LerpAngle(trans_.rot.y, targetAngleY, 0.5f);

    // 移動方向もターゲット方向に更新
    trans_.moveDir = trans_.targetDir;
}

void EnemyBase::ChangeState(int state)
{
    stateBase_ = state;
    // 各状態遷移の初期処理
    stateChanges_[stateBase_]();
}

//bool EnemyBase::InMovableRange(void) const
//{
//    bool ret = false;
//    // 初期位置からの距離
//    float dis = static_cast<float>(
//        AsoUtility::SqrMagnitude(defaultPos_, transform_.pos));
//    // 指定距離判定
//    if (dis < moveRadius_ * moveRadius_)
//    {
//        return true;
//    }
//    return ret;
//}

