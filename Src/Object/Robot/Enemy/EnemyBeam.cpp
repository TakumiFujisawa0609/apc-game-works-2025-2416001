#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/Camera.h"
#include "../../../Manager/InputManager.h"

#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/MatrixUtility.h"

#include "../../Common/AnimationController.h"
#include "./../../Common/Transform.h"

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

void EnemyBeam::Update(void)
{
    MV1SetPosition(trans_.modelId, trans_.pos);
    MV1SetRotationMatrix(trans_.modelId,
        MatrixUtility::Multiplication(trans_.localRot, trans_.rot));

    anim_->Update();
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

    lockcnt = 0;
}

void EnemyBeam::ProcessMove(void)
{
}

void EnemyBeam::ProcessRise(void)
{
}

void EnemyBeam::ProcessAttack(void)
{
}

void EnemyBeam::ProcessTargetLock(void)
{
}

void EnemyBeam::ChangeStandby(void)
{
}

void EnemyBeam::ChangeKnockback(void)
{
}

void EnemyBeam::ChangeAttack(void)
{
}

void EnemyBeam::ChangeDead(void)
{
}

void EnemyBeam::ChangeVictory(void)
{
}

void EnemyBeam::ChangeEnd(void)
{
}

void EnemyBeam::UpdateStandby(void)
{
}

void EnemyBeam::UpdateKnockback(void)
{
}

void EnemyBeam::UpdateAttack(void)
{
}

void EnemyBeam::UpdateDead(void)
{
}

void EnemyBeam::UpdateVictory(void)
{
}

void EnemyBeam::UpdateEnd(void)
{
}

void EnemyBeam::DrawStandby(void)
{
}

void EnemyBeam::DrawKnockback(void)
{
}

void EnemyBeam::DrawAttack(void)
{
}

void EnemyBeam::DrawDead(void)
{
}

void EnemyBeam::DrawVictory(void)
{
}

void EnemyBeam::DrawEnd(void)
{
}
