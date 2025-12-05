#include "../../Application.h"
#include "../Common/HpBer.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Camera.h"
#include "../../Manager/InputManager.h"
#include "../../Utility/MatrixUtility.h"
#include "../Common/AnimationController.h"
#include "../Common/Geometry/ColliderLine.h"
#include "../Common/Geometry/ColliderCapsule.h"
#include "../Common/Geometry/ColliderModel.h"
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

    // 衝突判定
    Collision();

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

#ifdef _DEBUG
    // 所有しているコライダの描画
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
    int lineType = static_cast<int>(ColliderBase::SHAPE::LINE);

    // 線分コライダが無ければ処理を抜ける
    if (ownColliders_.count(lineType) == 0) return;

    // 線分コライダ情報
    ColliderLine* colliderLine_ =
        dynamic_cast<ColliderLine*>(ownColliders_.at(lineType));

    if (colliderLine_ == nullptr) return;

    // 線分の始点と終点を取得
    VECTOR s = colliderLine_->GetPosStart();
    VECTOR e = colliderLine_->GetPosEnd();

    // 登録されている衝突物を全てチェック
    for (const auto& hitCol : hitColliders_)
    {
        // ステージ以外は処理を飛ばす
        if (hitCol->GetTag() != ColliderBase::TAG::STAGE) continue;

        // 派生クラスへキャスト
        const ColliderModel* colliderModel =
            dynamic_cast<const ColliderModel*>(hitCol);

        if (colliderModel == nullptr) continue;

        // ステージモデル(地面)との衝突
        auto hits = MV1CollCheck_LineDim(
            colliderModel->GetFollow()->modelId, -1, s, e);

        for (int i = 0; i < hits.HitNum; i++)
        {
            auto hit = hits.Dim[i];

            // 除外フレームは無視する
            if (colliderModel->IsExcludeFrame(hit.FrameIndex))
            {
                continue;
            }

            // 衝突地点から、少し上に移動
            if (trans_.pos.y < hit.HitPosition.y)
            {
                // 衝突物より、下側にいる場合のみ、位置を修正する
                trans_.pos =
                    VAdd(hit.HitPosition, VScale(AsoUtility::DIR_U, 2.0f));
            }
            // ジャンプ判定
            isJump_ = false;
        }
        // 検出した地面ポリゴン情報の後始末
        MV1CollResultPolyDimTerminate(hits);
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
    int capsuleType = static_cast<int>(ColliderBase::SHAPE::CAPSULE);
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
        // 派生クラスへキャスト
        const ColliderModel* colliderModel =
            dynamic_cast<const ColliderModel*>(hitCol);
        if (colliderModel == nullptr) continue;
        auto hits = MV1CollCheck_Capsule(
            colliderModel->GetFollow()->modelId, -1,
            colliderCapsule->GetPosTop(), colliderCapsule->GetPosDown(),
            colliderCapsule->GetRadius());

        // 衝突した複数のポリゴンと衝突回避するまで、
        // プレイヤーの位置を移動させる
        for (int i = 0; i < hits.HitNum; i++)
        {
            auto hit = hits.Dim[i];

            // 除外フレームは無視する
            if (colliderModel->IsExcludeFrame(hit.FrameIndex))
            {
                continue;
            }

            // 地面と異なり、衝突回避位置が不明なため、何度か移動させる
            // この時、移動させる方向は、移動前座標に向いた方向であったり、
            // 衝突したポリゴンの法線方向だったりする
            for (int tryCnt = 0; tryCnt < CNT_TRY_COLLISION; tryCnt++)
            {
                // 再度、モデル全体と衝突検出するには、効率が悪過ぎるので、
                // 最初の衝突判定で検出した衝突ポリゴン1枚と衝突判定を取る
                int pHit = HitCheck_Capsule_Triangle(
                    colliderCapsule->GetPosTop(), colliderCapsule->GetPosDown(),
                    colliderCapsule->GetRadius(),
                    hit.Position[0], hit.Position[1], hit.Position[2]);
                if (pHit)
                {
                    // 法線の方向にちょっとだけ移動させる
                    trans_.pos =
                        VAdd(trans_.pos,
                            VScale(hit.Normal, COLLISION_BACK_DIS));
                    continue;
                }
                break;
            }
        }
        // 検出した地面ポリゴン情報の後始末
        MV1CollResultPolyDimTerminate(hits);
    }

}
