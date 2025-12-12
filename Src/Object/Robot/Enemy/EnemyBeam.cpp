#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/Camera.h"
#include "../../../Manager/InputManager.h"
#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/MatrixUtility.h"
#include "../../Common/AnimationController.h"
#include "./../../Common/Transform.h"
#include "./../../Common/Collider/ColliderCapsule.h"
#include "./../../Common/Collider/ColliderLine.h"
#include "../../Manager/WeponManager.h"
#include "../../Wepon/WeponBeam.h"
#include "../../Wepon/WeponMissile.h"
#include "../../../Application.h"
#include "EnemyBeam.h"

EnemyBeam::EnemyBeam(void)
{
}

EnemyBeam::~EnemyBeam(void)
{
}

void EnemyBeam::InitLoad(void)
{
    trans_.modelId = resMng_.LoadModelDuplicate(ResourceManager::SRC::ENEMY_STAN);
}

void EnemyBeam::InitTransform(void)
{
    trans_.scl = ROBOT_DEF_SCL;
    trans_.quaRotLocal = Quaternion::Euler(LOCAL_DEF_ROT);
    trans_.Update();
}

void EnemyBeam::InitCollider(void)
{
    // 主に地面との衝突で仕様する線分コライダ
    ColliderLine* colLine = new ColliderLine(
        ColliderBase::TAG::ENEMY, &trans_,
        COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

    // 主に壁や木などの衝突で仕様するカプセルコライダ
    ColliderCapsule* colCapsule = new ColliderCapsule(
        ColliderBase::TAG::ENEMY, &trans_,
        COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS,
        COL_CAPSULE_RADIUS);
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);
}

void EnemyBeam::InitAnimation(void)
{
    anim_ = std::make_unique<AnimationController>(trans_.modelId);

    for (int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++)
    {
        anim_->Add(i, DEFAULT_ANIMATION, trans_.modelId);
    }

    // 初期アニメーション再生
    anim_->Play(static_cast<int>(ANIM_TYPE::IDLE));
}

void EnemyBeam::InitPost(void)
{
    //ロックカウント
    lockcnt = 0;
    //HP
    hp_ = DEFALUT_HP;
    //出現範囲
    spawnRange_ = SPAWN_RANGE;
    //ビーム出現数
    beamCnt_ = BEAM_CNT;

    maxHp_ = DEFALUT_HP;
    hpScl_ = HPBER_SIZE;
    hpTextOffset_ = { (HPBER_POS.x - hpScl_.x / 2), HPBER_POS.y};
    hpCol_ = HPBER_COLOR;
    hpBackCol_ = HPBER_COLOR_BACK;
}

void EnemyBeam::ProcessAttack(void)
{
    if (stepShotDelay_ <= 0.0f) {
        useWepon_->ChangeWepon(
            WeponBase::WEPON_TYPE::BEAM,
            trans_.pos,
            trans_.targetDir,
            beamCnt_);

        // 弾発射後の硬直時間セット
        stepShotDelay_ = SHOT_DELAY;
    }

    // 弾発射後の硬直時間を減らしていく
    if (stepShotDelay_ > 0.0f)
    {
        stepShotDelay_ -= SceneManager::GetInstance().GetDeltaTime();
    }
}

void EnemyBeam::CollisionReserve(void)
{
    // 通常時の線分に戻す
    if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::LINE)) != 0)
    {
        ColliderLine* colLine = dynamic_cast<ColliderLine*>(
            ownColliders_.at(static_cast<int>(COLLIDER_TYPE::LINE)));
        colLine->SetLocalPosStart(COL_LINE_START_LOCAL_POS);
        colLine->SetLocalPosEnd(COL_LINE_END_LOCAL_POS);
    }
    // 通常時のカプセルに戻す
    if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::CAPSULE)) != 0)
    {
        ColliderCapsule* colCapsule = dynamic_cast<ColliderCapsule*>(
            ownColliders_.at(static_cast<int>(COLLIDER_TYPE::CAPSULE)));
        colCapsule->SetLocalPosTop(COL_CAPSULE_TOP_LOCAL_POS);
        colCapsule->SetLocalPosDown(COL_CAPSULE_DOWN_LOCAL_POS);
    }
}
