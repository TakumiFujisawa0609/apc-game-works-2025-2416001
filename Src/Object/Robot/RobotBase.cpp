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

    MV1SetPosition(trans_.modelId, trans_.pos);
    MV1SetPosition(weponModel.modelId, weponModel.pos);

    MV1SetRotationMatrix(trans_.modelId,
        MatrixUtility::Multiplication(trans_.localRot, trans_.rot));
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

void RobotBase::InitTransformPost(void)
{
    // 大きさをモデルに反映
    MV1SetScale(trans_.modelId, trans_.scl);
    MV1SetScale(weponModel.modelId, weponModel.scl);
    // 角度から方向に変換する
    trans_.moveDir = { sinf(trans_.rot.y), 0.0f, cosf(trans_.rot.y) };
    weponModel.moveDir = { sinf(weponModel.rot.y), 0.0f, cosf(weponModel.rot.y) };
    // 行列の合成(子, 親と指定すると親⇒子の順に適用される)
    // 回転行列をモデルに反映
    MV1SetRotationMatrix(trans_.modelId,
        MatrixUtility::Multiplication(trans_.localRot, trans_.rot));
    MV1SetRotationMatrix(weponModel.modelId,
        MatrixUtility::Multiplication(weponModel.localRot, weponModel.rot));
    // 座標をモデルに反映
    MV1SetPosition(trans_.modelId, trans_.pos);
    MV1SetPosition(weponModel.modelId, weponModel.pos);
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

