#include "../../Application.h"
#include "../Common/HpBer.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Camera.h"
#include "../../Manager/InputManager.h"
#include "../../Utility/MatrixUtility.h"
#include "../Common/AnimationController.h"
#include "./../Common/Transform.h"
#include "../Wepon/WeponBase.h"
#include "../Manager/WeponManager.h"
#include "RobotBase.h"

RobotBase::RobotBase(void)
{
}

RobotBase::~RobotBase(void)
{
}

void RobotBase::Init(void)
{
    ObjectBase::Init();

    MV1SetPosition(weponModel.modelId, weponModel.pos);
    MV1SetRotationMatrix(weponModel.modelId,
        MatrixUtility::Multiplication(weponModel.localRot, weponModel.rot));

    useWepon_ = std::make_shared<WeponManager>();
    useWepon_->Init();

    hpBer_ = new HpBer(maxHp_, hpTextOffset_, hpScl_, hpCol_, hpBackCol_, -1);

    //èÛë‘ëJà⁄èâä˙ê›íË
    ChangeState(STATE::STANDBY);
}

void RobotBase::Update(void)
{

    if (!IsAlive())
    {
        return;
    }

    switch (state_)
    {
    case RobotBase::STATE::STANDBY:
        UpdateStandby();
        break;
    case RobotBase::STATE::KNOCKBACK:
        UpdateKnockback();
        break;
    case RobotBase::STATE::ATTACK:
        UpdateAttack();
        break;
    case RobotBase::STATE::DEAD:
        UpdateDead();
        break;
    case RobotBase::STATE::END:
        UpdateEnd();
        break;
    case RobotBase::STATE::VICTORY:
        UpdateVictory();
        break;
    default:
        break;
    }

    //íxâÑâÒì]èàóù
    DelayRotate();
    trans_.cillisionPos = {
        trans_.pos.x,
        trans_.pos.y + trans_.cillisionPos.y,
    trans_.pos.z };
    MV1SetPosition(trans_.modelId, trans_.pos);
    MV1SetPosition(weponModel.modelId, weponModel.pos);

    MV1SetRotationMatrix(trans_.modelId,
        MatrixUtility::Multiplication(trans_.localRot, trans_.rot));
    MV1SetRotationMatrix(weponModel.modelId,
        MatrixUtility::Multiplication(weponModel.localRot, weponModel.rot));

    hpBer_->SetHp(hp_);

    useWepon_->Update();
    anim_->Update();
}

void RobotBase::Draw(void)
{
    if (!IsAlive())
    {
        return;
    }

#ifdef _DEBUG
    // èäóLÇµÇƒÇ¢ÇÈÉRÉâÉCÉ_ÇÃï`âÊ
    for (const auto& own : ownColliders_)
    {
        own.second->Draw();
    }
#endif // _DEBUG

    MV1DrawModel(trans_.modelId);
    MV1DrawModel(weponModel.modelId);

    useWepon_->Draw();

    switch (state_)
    {
    case RobotBase::STATE::STANDBY:
        DrawStandby();
        break;
    case RobotBase::STATE::KNOCKBACK:
        DrawKnockback();
        break;
    case RobotBase::STATE::ATTACK:
        DrawAttack();
        break;
    case RobotBase::STATE::DEAD:
        DrawDead();
        break;
    case RobotBase::STATE::END:
        DrawEnd();
        break;
    case RobotBase::STATE::VICTORY:
        DrawVictory();
        break;
    default:
        break;
    }
}

void RobotBase::Release(void)
{

    MV1DeleteModel(trans_.modelId);
    MV1DeleteModel(weponModel.modelId);

    useWepon_->Release();
    delete hpBer_;

    // é©êgÇÃÉRÉâÉCÉ_âï˙
    for (auto& own : ownColliders_)
    {
        delete own.second;
    }
}

void RobotBase::ChangeState(STATE state)
{
    state_ = state;

    switch (state_)
    {
    case RobotBase::STATE::STANDBY:
        ChangeStandby();
        break;
    case RobotBase::STATE::KNOCKBACK:
        ChangeKnockback();
        break;
    case RobotBase::STATE::ATTACK:
        ChangeAttack();
        break;
    case RobotBase::STATE::DEAD:
        ChangeDead();
        break;
    case RobotBase::STATE::END:
        ChangeEnd();
        break;
    case RobotBase::STATE::VICTORY:
        ChangeVictory();
        break;
    default:
        break;
    }
}

void RobotBase::DelayRotate(void)
{
    // à⁄ìÆï˚å¸Ç©ÇÁäpìxÇ…ïœä∑Ç∑ÇÈ
    float goal = atan2f(trans_.moveDir.x, trans_.moveDir.z);
    // èÌÇ…ç≈íZåoòHÇ≈ï‚ä‘
    trans_.rot.y = AsoUtility::LerpAngle(trans_.rot.y, goal, 0.2f);
}

bool RobotBase::IsTargetLockFlage(void)
{
    if (lockcnt % 2 == 0)
    {
        return false;
    }
    return true;
}

void RobotBase::SetLockOnPos(VECTOR lockOnPos)
{
    lockOnPos_ = lockOnPos;
}

bool RobotBase::IsCollisionState(void)
{
    return state_ == STATE::STANDBY;
}

bool RobotBase::IsAlive(void)
{
    return state_ != STATE::END;
}

