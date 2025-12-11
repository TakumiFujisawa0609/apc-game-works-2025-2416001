#include "../../Application.h"
#include "../Common/HpBer.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Camera.h"
#include "../../Manager/InputManager.h"
#include "../../Utility/MatrixUtility.h"
#include "../Common/AnimationController.h"
#include "../Common/Collider/ColliderLine.h"
#include "../Common/Collider/ColliderModel.h"
#include "../Common/Collider/ColliderCapsule.h"
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

    useWepon_ = std::make_shared<WeponManager>();
    useWepon_->Init();

    hpBer_ = new HpBer(maxHp_, hpTextOffset_, hpScl_, hpCol_, hpBackCol_, -1);
}

void RobotBase::Update(void)
{
    if (!isAlive_)
    {
        return;
    }

    // 移動前座標を更新
    prevPos_ = trans_.pos;

    // 各キャラクターごとの更新処理
    UpdateProcess();

    //遅延回転処理
    DelayRotate();

    // 重力による移動量
    CalcGravityPow();

    // 衝突判定前準備
    CollisionReserve();

    // 衝突判定
    Collision();

    // モデル制御更新
    trans_.Update();

    hpBer_->SetHp(hp_);
    useWepon_->Update();
    anim_->Update();

    // 各キャラクターごとの更新後処理
    UpdateProcessPost();
}

void RobotBase::Draw(void)
{
    if (!isAlive_)
    {
        return;
    }

    ObjectBase::Draw();

    Debug();

    useWepon_->Draw();
}

void RobotBase::Release(void)
{
    ObjectBase::Release();

    useWepon_->Release();
    delete hpBer_;

}

void RobotBase::DelayRotate(void)
{
    // 移動方向から回転に変換する
    Quaternion goalRot = Quaternion::LookRotation(trans_.moveDir);
    // 回転の補間
    trans_.quaRot =
        Quaternion::Slerp(trans_.quaRot, goalRot, 0.2f);
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
        jumpPow_.x = 0;
        jumpPow_.y = MAX_FALL_SPEED;
        jumpPow_.z = 0;
    }
}

void RobotBase::Collision(void)
{
    // 移動処理
    trans_.pos = VAdd(trans_.pos, movePow_);

    // 衝突(カプセル)
    CollisionCapsule();

    // ジャンプ量を加算
    trans_.pos = VAdd(trans_.pos, jumpPow_);

    // 衝突(重力)
    CollisionGravity();
}

void RobotBase::CollisionGravity(void)
{
    // 落下中しか判定しない
    if (!(VDot(AsoUtility::DIR_D, jumpPow_) > 0.9f))
    {
        return;
    }

    // 線分コライダ
    int lineType = static_cast<int>(COLLIDER_TYPE::LINE);

    // 線分コライダが無ければ処理を抜ける
    if (ownColliders_.count(lineType) == 0) return;

    // 線分コライダ情報
    ColliderLine* colliderLine_ =
        dynamic_cast<ColliderLine*>(ownColliders_.at(lineType));

    if (colliderLine_ == nullptr) return;

    // 登録されている衝突物を全てチェック
    for (const auto& hitCol : hitColliders_)
    {
        // ステージ以外は処理を飛ばす
        if (hitCol->GetTag() != ColliderBase::TAG::STAGE) continue;

        // 派生クラスへキャスト
        const ColliderModel* colliderModel =
            dynamic_cast<const ColliderModel*>(hitCol);

        if (colliderModel == nullptr) continue;

        bool isHit = colliderLine_->PushBackUp(colliderModel, trans_, 2.0f, true, false);

        if (isHit)
        {
            isJump_ = false;
        }
    }
    if (!isJump_)
    {
        // ジャンプリセット
        jumpPow_ = AsoUtility::VECTOR_ZERO;

        // ジャンプの入力受付時間をリセット
        stepJump_ = 0.0f;
    }
}

void RobotBase::CollisionCapsule(void)
{
    // カプセルコライダ  
    int capsuleType = static_cast<int>(COLLIDER_TYPE::CAPSULE);
    // カプセルコライダが無ければ処理を抜ける  
    if (ownColliders_.count(capsuleType) == 0) return;
    // カプセルコライダ情報  
    ColliderCapsule* colliderCapsule =
        dynamic_cast<ColliderCapsule*>(ownColliders_.at(capsuleType));

    if (colliderCapsule == nullptr) return;
    // 登録されている衝突物を全てチェック  
    for (const auto& hitCol : hitColliders_)
    {
        // モデル以外は処理を飛ばす  
        if (hitCol->GetShape() != ColliderBase::SHAPE::MODEL) continue;

        const ColliderModel* colliderModel =
            dynamic_cast<const ColliderModel*>(hitCol);

        if (colliderModel == nullptr) continue;

        colliderCapsule->PushBackAlongNormal(
            colliderModel,
            trans_,
            CNT_TRY_COLLISION,
            COLLISION_BACK_DIS,
            true,
            false
        );
    }
}
