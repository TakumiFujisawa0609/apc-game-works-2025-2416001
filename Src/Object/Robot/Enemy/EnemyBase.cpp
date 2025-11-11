#include "../../../Utility/MatrixUtility.h"
#include "../../../Utility/AsoUtility.h"
#include "../../Common/AnimationController.h"
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
    // リソースロード
    InitLoad();
    // Transform初期化
    InitTransform();
    // 大きさ、回転、座標のモデル設定
    InitTransformPost();
    // アニメーションの初期化
    InitAnimation();
    // 初期化後の個別処理
    InitPost();
    //ランダムな出現座標
    SetSpawnPostiton();

    trans_.pos = VAdd(trans_.localPos, trans_.pos);
    MV1SetPosition(trans_.modelId, trans_.pos);
    MV1SetRotationMatrix(trans_.modelId,
        MatrixUtility::Multiplication(trans_.localRot, trans_.rot));

    useWepon_ = std::make_shared<WeponManager>();
    useWepon_->Init();

    hpBer_ = new HpBer(maxHp_, hpTextOffset_, hpScl_, hpCol_, hpBackCol_, -1);

    // 弾発射の硬直時間
    stepShotDelay_ = 0.0f;

    //状態遷移初期設定
    ChangeState(STATE::STANDBY);
}

void EnemyBase::SetSpawnPostiton(void)
{
    //出現半径
    float radius = SPAWN_RANIUS;
    //出現中心座標
    VECTOR spawnPos = AsoUtility::VECTOR_ZERO;
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
    ////移動量を常に減少
    movePow_ = 11.0f;

    // 方向×スピードで移動量を作って、座標に足して移動
    trans_.pos = VAdd(trans_.pos, VScale(trans_.targetDir, movePow_));
}

void EnemyBase::ProcessRise(void)
{
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
    VECTOR targetDir = VNorm(toTarget);

    // ターゲット方向からY軸回転角度を計算（水平方向）
    float targetAngleY = atan2f(targetDir.x, targetDir.z);

    // ターゲット方向のX軸回転角度を計算（上下の角度）
    float horizontalDist = sqrtf(targetDir.x * targetDir.x + targetDir.z * targetDir.z);
    float targetAngleX = atan2f(-targetDir.y, horizontalDist);

    // 滑らかに回転させる
    trans_.rot.y = AsoUtility::LerpAngle(trans_.rot.y, targetAngleY, 0.5f);
    trans_.rot.x = AsoUtility::LerpAngle(trans_.rot.x, targetAngleX, 0.5f);

    // 移動方向もターゲット方向に更新
    trans_.targetDir = targetDir;

    // ローカル回転とグローバル回転を合成してモデルに適用
    MV1SetRotationMatrix(trans_.modelId,
        MatrixUtility::Multiplication(trans_.localRot, trans_.rot));
}

void EnemyBase::ChangeStandby(void)
{
    // 初期アニメーション再生
    anim_->Play(static_cast<int>(ANIM_TYPE::IDLE));
}

void EnemyBase::ChangeKnockback(void)
{
    anim_->Play(static_cast<int>(ANIM_TYPE::HIT_REACT), false);
}

void EnemyBase::ChangeAttack(void)
{
}

void EnemyBase::ChangeDead(void)
{
    anim_->Play(static_cast<int>(ANIM_TYPE::DEATH), false);
}

void EnemyBase::ChangeVictory(void)
{
}

void EnemyBase::ChangeEnd(void)
{
}

void EnemyBase::UpdateStandby(void)
{

    ProcessTargetLock();

    //移動処理
    ProcessMove();

    ////上昇処理
    //ProcessRise();

    //攻撃処理
    ProcessAttack();
}

void EnemyBase::UpdateKnockback(void)
{
    if (anim_->IsEnd())
    {
        ChangeState(STATE::STANDBY);
    }
}

void EnemyBase::UpdateAttack(void)
{
}

void EnemyBase::UpdateDead(void)
{
    if (anim_->IsEnd())
    {
        ChangeState(STATE::END);
    }
}

void EnemyBase::UpdateVictory(void)
{
}

void EnemyBase::UpdateEnd(void)
{
}

void EnemyBase::DrawStandby(void)
{
}

void EnemyBase::DrawKnockback(void)
{
}

void EnemyBase::DrawAttack(void)
{
}

void EnemyBase::DrawDead(void)
{
}

void EnemyBase::DrawVictory(void)
{
}

void EnemyBase::DrawEnd(void)
{
}

void EnemyBase::DrawHp(void)
{
    hpBer_->Draw();
}

void EnemyBase::Damage(CollisinManager::HitObject hitObject)
{

    if (hp_ <= 0) {
        ChangeState(STATE::DEAD);
        return;
    }

    CollisinManager::HitObject hitObject_ = hitObject;

    if (hitObject_.hitType == CollisinManager::HIT_TYPE::PLAYER_ENEMY_HIT) {
        hp_ -= 1;
        trans_.pos = VAdd(trans_.pos, VScale(AsoUtility::DIR_F, 50.0f));
        ChangeState(STATE::KNOCKBACK);
    }
    if (hitObject_.hitType == CollisinManager::HIT_TYPE::PLAYER_WEPON_HIT) {
        hp_ -= 3;
        ChangeState(STATE::KNOCKBACK);
    }
    if (hitObject_.hitType == CollisinManager::HIT_TYPE::ENEMYS_HIT) {
        trans_.pos = VAdd(trans_.pos, VScale(AsoUtility::DIR_F, 10.0f));
    }

    MV1SetPosition(trans_.modelId, trans_.pos);
}

