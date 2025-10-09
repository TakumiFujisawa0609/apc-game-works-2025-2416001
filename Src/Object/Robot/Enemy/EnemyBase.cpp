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
#include "EnemyBase.h"

EnemyBase::EnemyBase(void)
{
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Update(void)
{
    MV1SetPosition(trans_.modelId, trans_.pos);
    MV1SetRotationMatrix(trans_.modelId,
        MatrixUtility::Multiplication(trans_.localRot, trans_.rot));

    anim_->Update();
}

void EnemyBase::InitLoad(void)
{
	trans_.modelId = resMng_.LoadModelDuplicate(ResourceManager::SRC::ENEMY_GEORGE);
}

void EnemyBase::InitTransform(void)
{
    trans_.rot = AsoUtility::VECTOR_ZERO;
    trans_.pos = DEFALUT_POS;
    trans_.scl = ROBOT_DEF_SCL;
    trans_.localRot = LOCAL_DEF_ROT;
}

void EnemyBase::InitAnimation(void)
{
    anim_ = std::make_unique<AnimationController>(trans_.modelId);

	for (int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++)
	{
		anim_->Add(i, DEFAULT_ANIMATION, trans_.modelId);
	}

	// 初期アニメーション再生
    anim_->Play(static_cast<int>(ANIM_TYPE::IDLE));
}

void EnemyBase::InitPost(void)
{
    //移動量
    movePow_ = 0;
    //上昇量
    rise_ = RISE_SPEED;
    //回転量
    rotPow_ = ROT_POW;

    lockcnt = 0;
}

void EnemyBase::ProcessMove(void)
{
}

void EnemyBase::ProcessRise(void)
{
}

void EnemyBase::ProcessAttack(void)
{
}

void EnemyBase::ProcessTargetLock(void)
{
}

void EnemyBase::ChangeStandby(void)
{
}

void EnemyBase::ChangeKnockback(void)
{
}

void EnemyBase::ChangeAttack(void)
{
}

void EnemyBase::ChangeDead(void)
{
}

void EnemyBase::ChangeVictory(void)
{
}

void EnemyBase::ChangeEnd(void)
{
}

void EnemyBase::UpdateStandby(void)
{
}

void EnemyBase::UpdateKnockback(void)
{
}

void EnemyBase::UpdateAttack(void)
{
}

void EnemyBase::UpdateDead(void)
{
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
