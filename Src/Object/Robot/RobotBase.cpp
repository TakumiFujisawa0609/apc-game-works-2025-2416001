#include "../../Application.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Camera.h"
#include "../../Manager/InputManager.h"
#include "../../Utility/AsoUtility.h"
#include "../../Utility/MatrixUtility.h"
#include "../Common/AnimationController.h"
#include "./../Common/Transform.h"
#include "../Wepon/WeponBeam.h"
#include "../Wepon/WeponMissile.h"
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

    //武器の初期化
    weponbeam_ = std::make_unique<WeponBeam>();
    weponbeam_->Init();

    for (int i = 0; i < 10; i++)
    {
        weponMissile_.push_back(std::make_unique<WeponMissile>());
    }

    for(int i = 0; i < weponMissile_.size(); i++)
    {
        weponMissile_[i]->Init();
    }

    //状態遷移初期設定
    ChangeState(STATE::STANDBY);

#ifdef _DEBUG
    debugSpherePos_ = DEBUG_SPHERE_POS;

    deBugLeft = false;
    deBugRight = true;
#endif

}

void RobotBase::Update(void)
{
    //遅延回転処理
    DelayRotate();

    //移動処理
    ProcessMove();

    //上昇処理
    ProcessRise();

    ProcessTargetLock();

    //攻撃処理
    ProcessAttack();

    MV1SetPosition(trans_.modelId, trans_.pos);
    MV1SetRotationMatrix(trans_.modelId,
        MatrixUtility::Multiplication(trans_.localRot, trans_.rot));

#ifdef _DEBUG

    //デバッグ用円の移動処理
    float moveSpeed = 15.0f;
    float maxMove = 1000.0f;

    if (debugSpherePos_.x > maxMove) {
        deBugLeft = true;
        deBugRight = false;
    }
    if (debugSpherePos_.x < -maxMove) {
        deBugLeft = false;
        deBugRight = true;
    }

    if (deBugLeft) {
        debugSpherePos_.x -= moveSpeed;
    }
    if (deBugRight) {
        debugSpherePos_.x += moveSpeed;
    }

#endif

    weponbeam_->Update();
    for (int i = 0; i < weponMissile_.size(); i++)
    {
        weponMissile_[i]->UpdateTarget(debugSpherePos_);
        weponMissile_[i]->Update();
    }
}

void RobotBase::Draw(void)
{
	MV1DrawModel(trans_.modelId);

    weponbeam_->Draw();
    for (int i = 0; i < weponMissile_.size(); i++)
    {
        weponMissile_[i]->Draw();
    }

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

    DrawSphere3D(debugSpherePos_, 100.0f, 16, 0xFFFF00, 0xAAAA00, false);

    DrawFormatString(
        0,40,GetColor(255, 255, 255),
        "座標：(%.1f,%.1f,%.1f)", 
        debugSpherePos_.x,
        debugSpherePos_.y,
        debugSpherePos_.z
    );

#endif
}

void RobotBase::Release(void)
{
	MV1DeleteModel(trans_.modelId);
    weponbeam_->Release();
    for (int i = 0; i < weponMissile_.size(); i++)
    {
        weponMissile_[i]->Release();
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

void RobotBase::InitTransformPost(void)
{
    // 大きさをモデルに反映
    MV1SetScale(trans_.modelId, trans_.scl);
    // 角度から方向に変換する
    trans_.moveDir = { sinf(trans_.rot.y), 0.0f, cosf(trans_.rot.y) };
    /*preInputDir_ = moveDir_;*/
    // 行列の合成(子, 親と指定すると親⇒子の順に適用される)
    MATRIX mat = MatrixUtility::Multiplication(trans_.localRot, trans_.rot);
    // 回転行列をモデルに反映
    MV1SetRotationMatrix(trans_.modelId, mat);
    // 座標をモデルに反映
    MV1SetPosition(trans_.modelId, trans_.pos);
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

const VECTOR* RobotBase::GetDebugSpherePos(void)
{
    return &debugSpherePos_;
}

