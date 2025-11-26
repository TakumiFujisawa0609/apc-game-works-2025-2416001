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
#include "./../../Common/HpBer.h"
#include "../../Wepon/WeponBase.h"
#include "../../Manager/WeponManager.h"
#include "../../Common/Geometry/Sphere.h"
#include "../../Manager/CollisionManager.h"
#include "../../Common/Collider.h"
#include "Player.h"

Player::Player(void)
{
}

Player::~Player(void)
{
}

void Player::SetCamera(Camera* camera)
{
    camera_ = camera;
}

const float& Player::GetDegreep(void) const
{
    return atan2f(trans_.rot.x, trans_.rot.z);
}

void Player::Damage(void)
{
    if (hp_ <= 0) {
        ChangeState(STATE::DEAD);
    }
}

void Player::InitLoad(void)
{
    trans_.modelId = resMng_.LoadModelDuplicate(ResourceManager::SRC::ENEMY_GEORGE);
    weponModel = 0;
}

void Player::InitTransform(void)
{
    trans_.rot = AsoUtility::VECTOR_ZERO;
    trans_.pos = DEFALUT_POS;
    trans_.scl = DEFALUT_SCL;
    trans_.localRot = LOCAL_DEF_ROT;
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
    //移動量
    movePow_ = 0;
    //上昇量
    rise_ = RISE_SPEED;
    //回転量
    rotPow_ = ROT_POW;
    //ロック処理判定カウント
    lockcnt = 0;
    // 弾発射の硬直時間
    stepShotDelay_ = 0.0f;
    //衝突半径
    trans_.Radius_ = DEFALUT_RADIUS;
    //ビーム出現数
    beamCnt_ = BEAM_CNT;
    //ミサイル出現数
    missileCnt_ = MISSILE_CNT;
    //衝突座標
    trans_.cillisionPos = COLLIDER_POS;
    //HP
    hp_ = DEFALUT_HP;

    maxHp_ = DEFALUT_HP;
    hpTextOffset_ = HPBER_POS;
    hpScl_ = HPBER_SIZE;
    hpCol_ = HPBER_COLOR;
    hpBackCol_ = HPBER_COLOR_BACK;
}

void Player::ProcessMove(void)
{

    MATRIX matRot = MGetIdent();
    //移動方向
    VECTOR dir = { 0.0f,0.0f,0.0f };
    //移動量を常に減少
    movePow_ *= FRICTION;
    // 入力状態をチェック
    bool isBustPressed;

    if (GetJoypadNum() == 0)
    {
        // 入力状態をチェック
        isBustPressed = inpMng_.IsTrgDown(KEY_INPUT_B);
        bool isDownPressed = inpMng_.IsNew(KEY_INPUT_S);
        bool isUpPressed = inpMng_.IsNew(KEY_INPUT_W);
        bool isRightPressed = inpMng_.IsNew(KEY_INPUT_D);
        bool isLeftPressed = inpMng_.IsNew(KEY_INPUT_A);

        // 上下移動とX軸回転の処理
        if (isDownPressed) {
            dir = { 0.0f, 0.0f, -1.0f };
            if (trans_.rot.x > -MAX_MOVE_ROT) {
               /* trans_.rot.x -= rotPow_;*/
                matRot = MMult(matRot, MGetRotX(trans_.rot.x -= rotPow_));
            }
        }

        if (isUpPressed) {
            dir = { 0.0f, 0.0f, 1.0f };
            if (trans_.rot.x < MAX_MOVE_ROT) {
               /* trans_.rot.x += rotPow_;*/
                matRot = MMult(matRot, MGetRotX(trans_.rot.x += rotPow_));
            }
        }

        // 上下キーが押されていない時、X軸回転を0に戻す
        if (!isDownPressed && !isUpPressed) {
            if (trans_.rot.x > rotPow_) {
                /*trans_.rot.x -= rotPow_;*/
                matRot = MMult(matRot, MGetRotX(trans_.rot.x -= rotPow_));
            }
            if (trans_.rot.x < -rotPow_) {
                /*trans_.rot.x += rotPow_;*/
                matRot = MMult(matRot, MGetRotX(trans_.rot.x += rotPow_));
            }
            if (trans_.rot.x >= -rotPow_ && trans_.rot.x <= rotPow_) {
                matRot = MMult(matRot, MGetRotX(0.0f));
            }
        }

        // 左右移動とZ軸回転の処理
        if (isRightPressed) {
            dir = { 1.0f, 0.0f, 0.0f };
            if (trans_.rot.z > -MAX_MOVE_ROT) {
               /* trans_.rot.z -= rotPow_;*/
                matRot = MMult(matRot, MGetRotZ(trans_.rot.z -= rotPow_));

            }
        }
        if (isLeftPressed) {
            dir = { -1.0f, 0.0f, 0.0f };
            if (trans_.rot.z < MAX_MOVE_ROT) {
               /* trans_.rot.z += rotPow_;*/
                matRot = MMult(matRot, MGetRotZ(trans_.rot.z += rotPow_));

            }
        }

        // 左右キーが押されていない時、Z軸回転を0に戻す
        if (!isRightPressed && !isLeftPressed) {
            if (trans_.rot.z > rotPow_) {
                /*trans_.rot.z -= rotPow_;*/
                matRot = MMult(matRot, MGetRotZ(trans_.rot.z -= rotPow_));
            }
            if (trans_.rot.z < -rotPow_) {
                /*trans_.rot.z += rotPow_;*/
                matRot = MMult(matRot, MGetRotZ(trans_.rot.z += rotPow_));
            }
            if (trans_.rot.z >= -rotPow_ && trans_.rot.z <= rotPow_) {
                matRot = MMult(matRot, MGetRotZ(0.0f));
            }
        }
    }
    else
    {

        isBustPressed = inpMng_.IsPadBtnTrgDown(
            InputManager::JOYPAD_NO::PAD1,
            InputManager::JOYPAD_BTN::RIGHT
        );

        // 接続されているゲームパッド１の情報を取得
        InputManager::JOYPAD_IN_STATE padState =
            inpMng_.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
        // アナログキーの入力値から方向を取得
        dir = inpMng_.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

    }

    //ブースト時の移動量と最大値を設定
    float maxMoveSpeed = MAX_MOVE_SPEED;
    if (isBustPressed)
    {
        movePow_ += BUST_SPEED;
        maxMoveSpeed = MAX_BUST_SPEED;
        if (trans_.rot.x < MAX_BOOST_ROT) {
           /* trans_.rot.x += rotPow_;*/
            matRot = MMult(matRot, MGetRotZ(trans_.rot.x += rotPow_));
        }
    }

    if (!AsoUtility::EqualsVZero(dir))
    {
        // XYZの回転行列
        // XZ平面移動にする場合は、XZの回転を考慮しないようにする
        MATRIX mat = MGetIdent();
        /*mat = MMult(mat, MGetRotX(rot_.x));*/
        mat = MMult(mat, MGetRotY(camera_->GetAngles().y));
        /*mat = MMult(mat, MGetRotZ(angles_.z));*/

        // 回転行列を使用して、ベクトルを回転させる
        trans_.moveDir = VTransform(dir, mat);


        //入力中のブースト時の移動量と最大値を設定
        if (isBustPressed)
        {
            movePow_ += BUST_SPEED;
            maxMoveSpeed = MAX_BUST_SPEED;
            if (trans_.rot.x < MAX_BOOST_ROT) {
               /* trans_.rot.x += rotPow_;*/
                matRot = MMult(matRot, MGetRotZ(trans_.rot.x += rotPow_));
            }
        }
        else
        {
            //加速度処理
            movePow_ += MOVE_SPEED;
            maxMoveSpeed = MAX_MOVE_SPEED;
        }

        // 最高速度制限
        if (movePow_ > maxMoveSpeed) {
            movePow_ -= (movePow_ - maxMoveSpeed) * 0.1f;
        }
    }

    // 停止閾値以下なら完全停止
    if (movePow_ < STOP_THRESHOLD) { movePow_ = 0.0f; }

    // 方向×スピードで移動量を作って、座標に足して移動
    trans_.pos = VAdd(trans_.pos, VScale(trans_.moveDir, movePow_));

    MV1SetRotationMatrix(trans_.modelId,
        MatrixUtility::Multiplication(trans_.localRot, trans_.rot));
}

void Player::ProcessRise(void)
{
    bool isRisePressed;
    if (GetJoypadNum() == 0)
    {
        isRisePressed = inpMng_.IsNew(KEY_INPUT_G);
    }
    else
    {
        isRisePressed = inpMng_.IsPadBtnNew(
            InputManager::JOYPAD_NO::PAD1,
            InputManager::JOYPAD_BTN::DOWN
        );
    }

    if (isRisePressed)
    {
        trans_.pos.y += rise_;
    }

    if (trans_.pos.y > 0)
    {
        trans_.pos.y -= GRAVITY;
    }
    MV1SetPosition(trans_.modelId, trans_.pos);
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
            Collider::TAG::PLAYER_WEPON,
            trans_.pos,
            targetDir,
            beamCnt_);

        // 弾発射後の硬直時間セット
        stepShotDelay_ = SHOT_DELAY;
    }

    if (IsMissile && stepShotDelay_ <= 0.0f) {
        useWepon_->ChangeWepon(
            WeponBase::WEPON_TYPE::MISSILE,
            Collider::TAG::PLAYER_WEPON,
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

void Player::UpdateWepon(void)
{
}

void Player::DrawHp(void)
{
    hpBer_->Draw();
}

void Player::ChangeStandby(void)
{
}

void Player::ChangeKnockback(void)
{
    anim_->Play(static_cast<int>(ANIM_TYPE::HIT_REACT), false);
}

void Player::ChangeAttack(void)
{
}

void Player::ChangeDead(void)
{
    anim_->Play(static_cast<int>(ANIM_TYPE::DEATH), false);
}

void Player::ChangeVictory(void)
{
}

void Player::ChangeEnd(void)
{
}

void Player::UpdateStandby(void)
{

    UpdateWepon();

    //移動処理
    ProcessMove();

    //上昇処理
    ProcessRise();

    ProcessTargetLock();

    //攻撃処理
    ProcessAttack();
}

void Player::UpdateKnockback(void)
{
    if (anim_->IsEnd())
    {
        ChangeState(STATE::STANDBY);
    }
}

void Player::UpdateAttack(void)
{
}

void Player::UpdateDead(void)
{
    if (anim_->IsEnd())
    {
        ChangeState(STATE::END);
    }
}

void Player::UpdateVictory(void)
{
}

void Player::UpdateEnd(void)
{
}

void Player::DrawStandby(void)
{
}

void Player::DrawKnockback(void)
{
}

void Player::DrawAttack(void)
{
}

void Player::DrawDead(void)
{
}

void Player::DrawVictory(void)
{
}

void Player::DrawEnd(void)
{
}
