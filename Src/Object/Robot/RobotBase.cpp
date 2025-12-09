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

    //状態遷移初期設定
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

    //遅延回転処理
    DelayRotate();

    // 重力による移動量
    CalcGravityPow();

    // 移動処理
    trans_.pos = VAdd(trans_.pos, movePow_);

    //// ジャンプ量を加算
    //trans_.pos = VAdd(trans_.pos, jumpPow_);

    MV1SetPosition(trans_.modelId, trans_.pos);
    MV1SetRotationMatrix(trans_.modelId,
        MatrixUtility::Multiplication(trans_.localRot, trans_.rot));

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

#ifdef _DEBUG
    // 所有しているコライダの描画
    for (const auto& own : ownColliders_)
    {
        own.second->Draw();
    }
#endif // _DEBUG
}

void RobotBase::Release(void)
{

    MV1DeleteModel(trans_.modelId);
    MV1DeleteModel(weponModel.modelId);

    useWepon_->Release();
    delete hpBer_;

    // 自身のコライダ解放
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
    // 移動方向から角度に変換する
    float goal = atan2f(trans_.moveDir.x, trans_.moveDir.z);
    // 常に最短経路で補間
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

void RobotBase::CalcGravityPow(void)
{
    // 重力方向
    VECTOR dirGravity = AsoUtility::DIR_D;
    // 重力の強さ
    float gravityPow = Application::GetInstance().GetGravityPow() *
        scnMng_.GetDeltaTime();
    // 重力
    VECTOR gravity = VScale(dirGravity, gravityPow);
    jumpPow_ = VAdd(jumpPow_, gravity);

    // 重力速度の制限
    if (jumpPow_.y < MAX_FALL_SPEED)
    {
        jumpPow_.y = MAX_FALL_SPEED;
    }
}