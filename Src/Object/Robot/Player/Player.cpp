

#include<memory>
#include "../../../Application.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/Camera.h"
#include "../../../Manager/InputManager.h"
#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/MatrixUtility.h"
#include "../../Common/AnimationController.h"
#include "./../../Common/Transform.h"
#include "../../Common/Collider/ColliderBase.h"
#include "../../Common/Collider/ColliderLine.h"
#include "../../Common/Collider/ColliderCapsule.h"
#include "../../../Common/Quaternion.h"
#include "./../../Common/HpBer.h"
#include "../../Wepon/WeponBase.h"
#include "../../Manager/WeponManager.h"
#include "Player.h"

Player::Player(void)
{
}

Player::~Player(void)
{
}

void Player::Debug(void)
{
}

void Player::SetCamera(Camera* camera)
{
    camera_ = camera;
}

void Player::InitLoad(void)
{
    trans_.modelId = resMng_.LoadModelDuplicate(ResourceManager::SRC::ENEMY_GEORGE);
    weponModel = 0;
}

void Player::InitTransform(void)
{
    trans_.scl = DEFALUT_SCL;
    trans_.quaRot = Quaternion::Identity();
    trans_.quaRotLocal = Quaternion::Identity();
    trans_.quaRotLocal =
        Quaternion::Mult(trans_.quaRotLocal,
            Quaternion::AngleAxis(AsoUtility::Deg2RadF(180.0f), AsoUtility::AXIS_Y));
    trans_.pos = DEFALUT_POS;
    trans_.Update();
}

void Player::InitCollider(void)
{
    // 主に地面との衝突で仕様する線分コライダ
    ColliderLine* colLine = new ColliderLine(
        ColliderBase::TAG::PLAYER, &trans_,
        COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
    ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::LINE), colLine);

    // 主に壁や木などの衝突で仕様するカプセルコライダ
    ColliderCapsule* colCapsule = new ColliderCapsule(
        ColliderBase::TAG::PLAYER, &trans_,
        COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS,
        COL_CAPSULE_RADIUS);
    ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::CAPSULE), colCapsule);
}

void Player::InitAnimation(void)
{
    anim_ = std::make_unique<AnimationController>(trans_.modelId);

    for (int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++)
    {
        anim_->Add(i, DEFAULT_ANIMATION, trans_.modelId);
    }

    // 初期アニメーション再生
    anim_->Play(static_cast<int>(ANIM_TYPE::IDLE));
}

void Player::InitPost(void)
{
    //ロック処理判定カウント
    lockcnt = 0;
    // 弾発射の硬直時間
    stepShotDelay_ = 0.0f;
    //ビーム出現数
    beamCnt_ = BEAM_CNT;
    //ミサイル出現数
    missileCnt_ = MISSILE_CNT;
    //HP
    hp_ = DEFALUT_HP;

    // 移動方向
    trans_.moveDir = AsoUtility::DIR_F;
    // 移動スピード
    moveSpeed_ = 0.0f;
    // 移動量
    movePow_ = AsoUtility::VECTOR_ZERO;

    maxHp_ = DEFALUT_HP;
    hpTextOffset_ = HPBER_POS;
    hpScl_ = HPBER_SIZE;
    hpCol_ = HPBER_COLOR;
    hpBackCol_ = HPBER_COLOR_BACK;

    //状態
    state_ = STATE::IDLE;
}

void Player::UpdateProcess(void)
{
    //対象ロック処理
    ProcessTargetLock();

    //移動処理
    ProcessMove();

    //上昇処理
    ProcessRise();

    //攻撃処理
    ProcessAttack();
}

void Player::UpdateProcessPost(void)
{
}

void Player::ProcessMove(void)
{
    if (state_ != STATE::IDLE) {
        return;
    }

    VECTOR dir = AsoUtility::VECTOR_ZERO;

    auto& ins = InputManager::GetInstance();
    if (GetJoypadNum() == 0) {

        if (ins.IsNew(KEY_INPUT_W)) { dir = AsoUtility::DIR_F; }
        if (ins.IsNew(KEY_INPUT_A)) { dir = AsoUtility::DIR_L; }
        if (ins.IsNew(KEY_INPUT_S)) { dir = AsoUtility::DIR_B; }
        if (ins.IsNew(KEY_INPUT_D)) { dir = AsoUtility::DIR_R; }
    }
    else {
        // 接続されているゲームパッド１の情報を取得
        InputManager::JOYPAD_IN_STATE padState =
            ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
        // アナログキーの入力値から方向を取得
        dir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);
    }

    if (!AsoUtility::EqualsVZero(dir))
    {
        if (state_ == STATE::IDLE) {
            bool isD;
            auto& ins = InputManager::GetInstance();
            if (GetJoypadNum() == 0)
            {
                isD = ins.IsNew(KEY_INPUT_B);
            }
            else {
                isD = ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1,
                    InputManager::JOYPAD_BTN::R_TRIGGER);
            }

            float targetSpeed = 0.0f;
            float acceleration = 15.0f;
            //ジャンプ中はアニメーションを変えない
            if (!isJump_)
            {
                if (isD) {
                    targetSpeed = SPEED_DASH;
                    anim_->Play(static_cast<int>(ANIM_TYPE::RUN));
                }
                else {
                    targetSpeed = SPEED_MOVE;
                    anim_->Play(static_cast<int>(ANIM_TYPE::WALK));
                }

                // 加速処理
                if (moveSpeed_ < targetSpeed) {
                    moveSpeed_ += acceleration * scnMng_.GetDeltaTime();
                    if (moveSpeed_ > targetSpeed) {
                        moveSpeed_ = targetSpeed; // 最高速度を超えないようにする
                    }
                }

            }
        }

        Quaternion cameraRot = scnMng_.GetCamera()->GetQuaRotY();
        trans_.moveDir = Quaternion::PosAxis(cameraRot, dir);
        movePow_ = VScale(trans_.moveDir, moveSpeed_);
    }
    else {
        // 減速処理
        float deceleration = 17.0f;
        if (moveSpeed_ > 0.0f) {
            moveSpeed_ -= deceleration * scnMng_.GetDeltaTime();
            if (moveSpeed_ < 0.0f) {
                moveSpeed_ = 0.0f;
            }
        }

        movePow_ = VScale(trans_.moveDir, moveSpeed_);

        // ジャンプ中はアニメーションを変えない
        if (!isJump_)
        {
            if (state_ == STATE::IDLE) {
                // 歩くアニメーションを再生すること！(ループ再生有り)
                // アニメーションの再生
                anim_->Play(static_cast<int>(ANIM_TYPE::IDLE));
            }
        }
    }
}

void Player::ProcessRise(void)
{
    auto& ins = InputManager::GetInstance();
    // 持続ジャンプ処理
    bool isHitKeyNew = ins.IsNew(KEY_INPUT_BACKSLASH)
        || ins.IsPadBtnNew(
            InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

    if (isHitKeyNew)
    {
        // ジャンプの入力受付時間を減少
        stepJump_ += scnMng_.GetDeltaTime();
        if (stepJump_ < TIME_JUMP_INPUT)
        {
            // ジャンプ量の計算
            float jumpSpeed = POW_JUMP_KEEP * scnMng_.GetDeltaTime();
            jumpPow_ = VAdd(jumpPow_, VScale(AsoUtility::DIR_U, jumpSpeed));
        }
    }
    else
    {
        // ボタンを離したらジャンプ力に加算しない
        stepJump_ = 0.0f;
    }

    // 初期ジャンプ処理
    bool isHitKey = ins.IsTrgDown(KEY_INPUT_BACKSLASH)
        || ins.IsPadBtnTrgDown(
            InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

    // ジャンプ
    if (isHitKey && !isJump_)
    {
        // ジャンプ量の計算
        float jumpSpeed = POW_JUMP_INIT * scnMng_.GetDeltaTime();
        jumpPow_ = VScale(AsoUtility::DIR_U, jumpSpeed);
        isJump_ = true;
        // アニメーション再生
        anim_->Play(
            static_cast<int>(ANIM_TYPE::JUMP), false);
    }
}

void Player::ProcessAttack(void)
{
    VECTOR targetDir = trans_.moveDir;

    if (IsTargetLockFlage())
    {
        targetDir = trans_.targetDir;
    }

    bool IsBeam;
    bool IsMissile;

    if (GetJoypadNum() == 0) {
        IsBeam = inpMng_.IsTrgDown(KEY_INPUT_R);
        IsMissile = inpMng_.IsTrgDown(KEY_INPUT_F);
    }
    else {
        IsBeam = inpMng_.IsPadBtnTrgDown(
            InputManager::JOYPAD_NO::PAD1,
            InputManager::JOYPAD_BTN::L_TRIGGER);
        IsMissile = inpMng_.IsPadBtnTrgDown(
            InputManager::JOYPAD_NO::PAD1,
            InputManager::JOYPAD_BTN::R_TRIGGER);
    }

    if (IsBeam && stepShotDelay_ <= 0.0f) {
        useWepon_->ChangeWepon(
            WeponBase::WEPON_TYPE::BEAM,
            ColliderBase::TAG::PLAYER_WEPON,
            trans_.pos,
            targetDir,
            beamCnt_);

        // 弾発射後の硬直時間セット
        stepShotDelay_ = SHOT_DELAY;
    }

    if (IsMissile && stepShotDelay_ <= 0.0f) {
        useWepon_->ChangeWepon(
            WeponBase::WEPON_TYPE::MISSILE,
            ColliderBase::TAG::PLAYER_WEPON,
            trans_.pos,
            targetDir, 
            missileCnt_,
            lockOnPos_
        );

        // 弾発射後の硬直時間セット
        stepShotDelay_ = SHOT_DELAY;
    }

    // 弾発射後の硬直時間を減らしていく
    if (stepShotDelay_ > 0.0f)
    {
        stepShotDelay_ -= SceneManager::GetInstance().GetDeltaTime();
    }
}

void Player::ProcessTargetLock(void)
{
    bool isLock;
    if (GetJoypadNum() == 0) {
        isLock = inpMng_.IsTrgDown(KEY_INPUT_L);
    }
    else {
        isLock = inpMng_.IsPadBtnTrgDown(
            InputManager::JOYPAD_NO::PAD1,
            InputManager::JOYPAD_BTN::L_BTN);
    }

    if (isLock) { lockcnt++; }

    if (camera_->GetCameraMode() == Camera::MODE::FIXED_POINT) {
        return;
    }

    // 1. ターゲットへのベクトル計算
    VECTOR toTarget = VSub(lockOnPos_, trans_.pos);
    float distance = VSize(toTarget);
    if (distance < 0.01f) return;

    // 2. ターゲット方向を向くクォータニオンを作成
    // LookAt のように、現在の位置からターゲット方向への回転を求める
    // (注: AsoUtility や Quaternion クラスに LookRotation や LookAt 相当の関数があればそれを使用してください)
    // ここでは基本的なラジアンからクォータニオンを作成する方法で実装します。
    float targetAngleY = atan2f(toTarget.x, toTarget.z);
    float horizontalDist = sqrtf(toTarget.x * toTarget.x + toTarget.z * toTarget.z);
    float targetAngleX = atan2f(-toTarget.y, horizontalDist);

    Quaternion targetQua = Quaternion::AngleAxis(targetAngleY, AsoUtility::AXIS_Y)
        .Mult(Quaternion::AngleAxis(targetAngleX, AsoUtility::AXIS_X));

    // 3. 球面線形補間 (Slerp) で滑らかに回転させる
    // 0.1f ～ 0.2f 程度にすると「ゆっくり振り向く」感じになります。0.5fはかなり速めです。
    trans_.quaRot = Quaternion::Slerp(trans_.quaRot, targetQua, 0.2f);

    // 4. 移動方向の更新
    trans_.targetDir = VNorm(toTarget);

    // 5. モデルへの適用 (クォータニオンをマトリックスに変換)
    // ローカル回転がある場合は先に適用
    MATRIX rotMat = trans_.quaRot.ToMatrix();
    MV1SetRotationMatrix(trans_.modelId, MMult(trans_.matRot, rotMat));
}

void Player::CollisionReserve(void)
{
    // アニメーションごとの線分調整
    if (anim_->GetPlayType() == static_cast<int>(ANIM_TYPE::JUMP))
    {
        // ジャンプ中は線分を伸ばす
        if (ownColliders_.count(static_cast<int>(ColliderBase::SHAPE::LINE)) != 0)
        {
            ColliderLine* colLine = dynamic_cast<ColliderLine*>(
                ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::LINE)));
            colLine->SetLocalPosStart(COL_LINE_JUMP_START_LOCAL_POS);
            colLine->SetLocalPosEnd(COL_LINE_JUMP_END_LOCAL_POS);
        }
        // ジャンプ中はカプセルを伸ばす
        if (ownColliders_.count(static_cast<int>(ColliderBase::SHAPE::CAPSULE)) != 0)
        {
            ColliderCapsule* colCapsule = dynamic_cast<ColliderCapsule*>(
                ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::CAPSULE)));
            colCapsule->SetLocalPosTop(COL_CAPSULE_TOP_JUMP_LOCAL_POS);
            colCapsule->SetLocalPosDown(COL_CAPSULE_DOWN_JUMP_LOCAL_POS);
        }
    }
    else
    {
        // 通常時の線分に戻す
        if (ownColliders_.count(static_cast<int>(ColliderBase::SHAPE::LINE)) != 0)
        {
            ColliderLine* colLine = dynamic_cast<ColliderLine*>(
                ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::LINE)));
            colLine->SetLocalPosStart(COL_LINE_START_LOCAL_POS);
            colLine->SetLocalPosEnd(COL_LINE_END_LOCAL_POS);
        }
        // 通常時のカプセルに戻す
        if (ownColliders_.count(static_cast<int>(ColliderBase::SHAPE::CAPSULE)) != 0)
        {
            ColliderCapsule* colCapsule = dynamic_cast<ColliderCapsule*>(
                ownColliders_.at(static_cast<int>(ColliderBase::SHAPE::CAPSULE)));
            colCapsule->SetLocalPosTop(COL_CAPSULE_TOP_LOCAL_POS);
            colCapsule->SetLocalPosDown(COL_CAPSULE_DOWN_LOCAL_POS);
        }
    }
}