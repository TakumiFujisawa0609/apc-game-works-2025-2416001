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
#include "EnemyMissile.h"

EnemyMissile::EnemyMissile(void)
{
}

EnemyMissile::~EnemyMissile(void)
{
}

void EnemyMissile::Update(void)
{
    if (!IsAlive())
    {
        return;
    }

    MV1SetPosition(trans_.modelId, trans_.pos);
    MV1SetRotationMatrix(trans_.modelId,
        MatrixUtility::Multiplication(trans_.localRot, trans_.rot));

    anim_->Update();
}

void EnemyMissile::InitLoad(void)
{
    trans_.modelId = resMng_.LoadModelDuplicate(ResourceManager::SRC::ENEMY_LEELA);
}

void EnemyMissile::InitTransform(void)
{
    trans_.rot = AsoUtility::VECTOR_ZERO;
    trans_.pos = DEFALUT_POS;
    trans_.scl = ROBOT_DEF_SCL;
    trans_.localRot = LOCAL_DEF_ROT;
    trans_.localPos = LOCAL_DEF_POS;
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
}

void EnemyMissile::ProcessMove(void)
{
}

void EnemyMissile::ProcessRise(void)
{
}

void EnemyMissile::ProcessAttack(void)
{
}

void EnemyMissile::ProcessTargetLock(void)
{
}