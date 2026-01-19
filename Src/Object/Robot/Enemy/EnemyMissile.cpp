#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/Camera.h"
#include "../../../Manager/InputManager.h"
#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/MatrixUtility.h"
#include "../../Common/AnimationController.h"
#include "./../../Common/Collider/ColliderBase.h"
#include "./../../Common/Collider/ColliderCapsule.h"
#include "./../../Common/Collider/ColliderLine.h"
#include "./../../Common/Transform.h"
#include "../../Wepon/WeponBeam.h"
#include "../../Manager/WeponManager.h"
#include "../../Wepon/WeponMissile.h"
#include "../../../Application.h"
#include "EnemyMissile.h"

EnemyMissile::EnemyMissile(void)
{
}

EnemyMissile::~EnemyMissile(void)
{
}

void EnemyMissile::InitLoad(void)
{
    trans_.modelId = resMng_.LoadModelDuplicate(ResourceManager::SRC::ENEMY_LEELA);
}

void EnemyMissile::InitTransform(void)
{
    trans_.scl = ROBOT_DEF_SCL;
    trans_.quaRotLocal = Quaternion::Euler(LOCAL_DEF_ROT);
    trans_.Update();
}

void EnemyMissile::InitCollider(void)
{
    // 主に地面との衝突で仕様する線分コライダ
    ColliderLine* colLine = new ColliderLine(
        ColliderBase::TAG::ENEMY, &trans_,
        COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
    ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::LINE), colLine);

    // 主に壁や木などの衝突で仕様するカプセルコライダ
    ColliderCapsule* colCapsule = new ColliderCapsule(
        ColliderBase::TAG::ENEMY, &trans_,
        COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS,
        COL_CAPSULE_RADIUS);
    ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::CAPSULE), colCapsule);
}

void EnemyMissile::InitAnimation(void)
{
    anim_ = std::make_unique<AnimationController>(trans_.modelId);

    for (int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++)
    {
        anim_->Add(i, DEFAULT_ANIMATION, trans_.modelId);
    }

    // 初期アニメーション再生
    anim_->Play(static_cast<int>(ANIM_TYPE::IDLE));
}

void EnemyMissile::InitPost(void)
{
    //ロックカウント
    lockcnt = 0;
    //HP
    hp_ = DEFALUT_HP;
    //出現範囲
    spawnRange_ = SPAWN_RANGE;
    //ミサイル出現数
    missileCnt_ = MISSILE_CNT;

    maxHp_ = DEFALUT_HP;
    hpScl_ = HPBER_SIZE;
    hpTextOffset_ = { (HPBER_POS.x - hpScl_.x / 2), HPBER_POS.y };
    hpCol_ = HPBER_COLOR;
    hpBackCol_ = HPBER_COLOR_BACK;

    //ランダムな出現座標
    SetSpawnPostiton();

    // 状態遷移初期処理登録
    stateChanges_.emplace(static_cast<int>(STATE::NONE),
        std::bind(&EnemyMissile::ChangeStateNone, this));
    stateChanges_.emplace(static_cast<int>(STATE::THINK),
        std::bind(&EnemyMissile::ChangeStateThink, this));
    stateChanges_.emplace(static_cast<int>(STATE::IDLE),
        std::bind(&EnemyMissile::ChangeStateIdle, this));
    stateChanges_.emplace(static_cast<int>(STATE::WANDER),
        std::bind(&EnemyMissile::ChangeStateWander, this));
    stateChanges_.emplace(static_cast<int>(STATE::END),
        std::bind(&EnemyMissile::ChangeStateEnd, this));

    // 初期状態設定
    ChangeState(STATE::THINK);
}

void EnemyMissile::ProcessAttack(void)
{
    useWepon_->SetTargetPos(lockOnPos_);

    if (stepShotDelay_ <= 0.0f) {
        useWepon_->ChangeWepon(
            WeponBase::WEPON_TYPE::MISSILE,
            ColliderBase::TAG::ENEMY_WEPON,
            trans_.pos,
            trans_.targetDir,
            missileCnt_,
            lockOnPos_
        );

        // 弾発射後の硬直時間セット
        stepShotDelay_ = SHOT_DELAY;
    }

    // 弾発射後の硬直時間を減らしていく
    if (stepShotDelay_ > 0.0f)
    {
        stepShotDelay_ -= SceneManager::GetInstance().GetDeltaTime();
    }
}

void EnemyMissile::CollisionReserve(void)
{
    // 通常時の線分に戻す
    if (ownColliders_.count(static_cast<int>(ColliderBase::SHAPE::LINE)) != 0)
    {
        ColliderLine* colLine = dynamic_cast<ColliderLine*>(
            ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::LINE)));
        colLine->SetLocalPosStart(COL_LINE_START_LOCAL_POS);
        colLine->SetLocalPosEnd(COL_LINE_END_LOCAL_POS);
    }
    // 通常時のカプセルに戻す
    if (ownColliders_.count(static_cast<int>(ColliderBase::SHAPE::CAPSULE)) != 0)
    {
        ColliderCapsule* colCapsule = dynamic_cast<ColliderCapsule*>(
            ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::CAPSULE)));
        colCapsule->SetLocalPosTop(COL_CAPSULE_TOP_LOCAL_POS);
        colCapsule->SetLocalPosDown(COL_CAPSULE_DOWN_LOCAL_POS);
    }
}

void EnemyMissile::UpdateProcess(void)
{
    if (hp_ <= 0 && state_ != STATE::DEAD)
    {
        // 死亡状態に遷移
        state_ = STATE::DEAD;
        moveSpeed_ = 0.0f;
        movePow_ = AsoUtility::VECTOR_ZERO;
        anim_->Play(static_cast<int>(ANIM_TYPE::DEATH), false);
    }

    if (state_ == STATE::DEAD)
    {
        if (anim_->IsEnd())
        {
            isAlive_ = false;
        }
        return;
    }

    // 状態別更新
    stateUpdate_();

    //対象ロック処理
    ProcessTargetLock();

    //移動処理
    ProcessMove();

    //上昇処理
    ProcessRise();

    //攻撃処理
    ProcessAttack();
}

void EnemyMissile::UpdateProcessPost(void)
{
}

void EnemyMissile::ChangeState(STATE state)
{
    state_ = state;

    // 各状態遷移の初期処理
    EnemyBase::ChangeState(static_cast<int>(state_));
}

void EnemyMissile::ChangeStateNone(void)
{
    stateUpdate_ = std::bind(&EnemyMissile::UpdateNone, this);
}

void EnemyMissile::ChangeStateThink(void)
{
    stateUpdate_ = std::bind(&EnemyMissile::UpdateThink, this);

    stepCnt_ = 0.0f;

    //// 思考
    //// ランダムに次の行動を決定
    //// 30%で待機、70%で徘徊
    //int rand = GetRand(100);
    //if (rand < 30)
    //{
    //    ChangeState(STATE::IDLE);
    //}
    //else
    //{
    //    ChangeState(STATE::WANDER);
    //}
}

void EnemyMissile::ChangeStateIdle(void)
{
    stateUpdate_ = std::bind(&EnemyMissile::UpdateIdle, this);
}

void EnemyMissile::ChangeStateWander(void)
{
    stateUpdate_ = std::bind(&EnemyMissile::UpdateWander, this);
}

void EnemyMissile::ChangeStateEnd(void)
{
    stateUpdate_ = std::bind(&EnemyMissile::UpdateEnd, this);
}

void EnemyMissile::UpdateNone(void)
{
}

void EnemyMissile::UpdateThink(void)
{
}

void EnemyMissile::UpdateIdle(void)
{
}

void EnemyMissile::UpdateWander(void)
{
}

void EnemyMissile::UpdateEnd(void)
{
}

