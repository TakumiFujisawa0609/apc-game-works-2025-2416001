#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/Camera.h"
#include "../../../Manager/InputManager.h"

#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/MatrixUtility.h"

#include "../../Common/AnimationController.h"
#include "./../../Common/Transform.h"

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
    trans_.rot = AsoUtility::VECTOR_ZERO;
    trans_.pos = DEFALUT_POS;
    trans_.scl = ROBOT_DEF_SCL;
    trans_.localRot = LOCAL_DEF_ROT;
    trans_.Radius_ = DEFALUT_RADIUS;
    //衝突座標
    trans_.cillisionPos = COLLIDER_POS;

    //ビーム出現数
    beamCnt_ = BEAM_CNT;
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
    //移動量
    movePow_ = 0;
    //上昇量
    rise_ = RISE_SPEED;
    //回転量
    rotPow_ = ROT_POW;
    //ロックカウント
    lockcnt = 0;
    //HP
    hp_ = DEFALUT_HP;
    //衝突半径
    trans_.Radius_ = DEFALUT_RADIUS;
    //出現範囲
    spawnRange_ = SPAWN_RANGE;

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
            Collider::TAG::ENEMY_WEPON,
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
