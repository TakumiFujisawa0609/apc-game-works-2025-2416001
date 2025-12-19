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
    bool  isLock;

    if (GetJoypadNum() == 0)
    {
        isLock = inpMng_.IsTrgDown(KEY_INPUT_L);
    }
    else
    {
        isLock = inpMng_.IsPadBtnTrgDown(
            InputManager::JOYPAD_NO::PAD1,
            InputManager::JOYPAD_BTN::L_BTN);
    }

    //対象ロック処理
    if (isLock)
    {
        lockcnt++;
    }

    if (camera_->GetCameraMode() == Camera::MODE::FIXED_POINT)
    {
        return;
    }

    // デバッグ球体へのベクトルを計算
    VECTOR toTarget = VSub(lockOnPos_, trans_.pos);

    // ターゲットまでの距離をチェック（ゼロ除算回避）
    float distance = VSize(toTarget);
    if (distance < 0.01f) {
        return;
    }

    // ターゲット方向の正規化
    VECTOR targetDir = VNorm(toTarget);

    // ターゲット方向からY軸回転角度を計算（水平方向）
    float targetAngleY = atan2f(targetDir.x, targetDir.z);

    // ターゲット方向のX軸回転角度を計算（上下の角度）
    float horizontalDist = sqrtf(targetDir.x * targetDir.x + targetDir.z * targetDir.z);
    float targetAngleX = atan2f(-targetDir.y, horizontalDist);

    // 滑らかに回転させる
    trans_.rot.y = AsoUtility::LerpAngle(trans_.rot.y, targetAngleY, 0.5f);
    trans_.rot.x = AsoUtility::LerpAngle(trans_.rot.x, targetAngleX, 0.5f);

    // 移動方向もターゲット方向に更新
    trans_.targetDir = targetDir;

    // ローカル回転とグローバル回転を合成してモデルに適用
    MV1SetRotationMatrix(trans_.modelId,
        MatrixUtility::Multiplication(trans_.localRot, trans_.rot));
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