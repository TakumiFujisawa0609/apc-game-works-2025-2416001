#include "../../Application.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Camera.h"
#include "../../Manager/InputManager.h"
#include "../../Utility/AsoUtility.h"
#include "../../Utility/MatrixUtility.h"
#include "../Common/AnimationController.h"
#include "./../Common/Transform.h"
#include "../Wepon/WeponBase.h"
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

    MV1SetPosition(trans_.modelId, trans_.pos);
    MV1SetRotationMatrix(trans_.modelId,
        MatrixUtility::Multiplication(trans_.localRot, trans_.rot));

    //状態遷移初期設定
    ChangeState(STATE::STANDBY);
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

    MV1SetPosition(trans_.modelId, trans_.pos);
    MV1SetRotationMatrix(trans_.modelId,
        MatrixUtility::Multiplication(trans_.localRot, trans_.rot));

    size_t size = useWepon_.size();
    for (int i = 0; i < size; i++)
    {
        useWepon_[i]->Update();
    }
}

void RobotBase::Draw(void)
{
	MV1DrawModel(trans_.modelId);

    size_t size = useWepon_.size();
    for (int i = 0; i < size; i++)
    {
        useWepon_[i]->Draw();
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
}

void RobotBase::Release(void)
{
	MV1DeleteModel(trans_.modelId);

    size_t size = useWepon_.size();
    for (int i = 0; i < size; i++)
    {
        useWepon_[i]->Release();
    }
    useWepon_.clear();
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

WeponBase* RobotBase::GetValidWepon(WeponBase::WEPON_TYPE type)
{
    size_t size = useWepon_.size();

    for (int i = 0; i < size; i++)
    {
        // 未使用で、かつ、武器の種別が同じ
        if (!useWepon_[i]->IsAlive() && useWepon_[i]->GetType() == type)
        {
            return useWepon_[i].get();
        }
    }

    //新しい武器のインスタンスを生成する
    std::shared_ptr<WeponBase> wepon;
    switch (type)
    {
    case WeponBase::WEPON_TYPE::NONE:
        break;
    case WeponBase::WEPON_TYPE::BEAM:
        wepon = std::make_unique<WeponBeam>(type);
        break;
    case WeponBase::WEPON_TYPE::MISSILE:
        wepon = std::make_unique<WeponMissile>(type);
        break;
    case WeponBase::WEPON_TYPE::SWORD:
        break;
    default:
        break;
    }

    if (!wepon) {
        return nullptr;
    }

    // 可変長配列に追加
    useWepon_.push_back(wepon);

    return wepon.get();
}

void RobotBase::InitTransformPost(void)
{
    // 大きさをモデルに反映
    MV1SetScale(trans_.modelId, trans_.scl);
    // 角度から方向に変換する
    trans_.moveDir = { sinf(trans_.rot.y), 0.0f, cosf(trans_.rot.y) };
    // 行列の合成(子, 親と指定すると親⇒子の順に適用される)
    // 回転行列をモデルに反映
    MV1SetRotationMatrix(trans_.modelId,
        MatrixUtility::Multiplication(trans_.localRot, trans_.rot));
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

void RobotBase::SetLockOnPos(VECTOR lockOnPos)
{
    lockOnPos_ = lockOnPos;
}

