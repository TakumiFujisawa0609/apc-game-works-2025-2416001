#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Camera.h"
#include "../../Manager/InputManager.h"
#include "../../Utility/AsoUtility.h"
#include "../../Utility/MatrixUtility.h"
#include "./../Common/Transform.h"
#include "../Wepon/WeponBeam.h"
#include "RobotBase.h"

RobotBase::RobotBase(void)
{
}

RobotBase::~RobotBase(void)
{
}

void RobotBase::Init(void)
{
	trans_.modelId = resMng_.LoadModelDuplicate(ResourceManager::SRC::ROBOT);
	trans_.rot = AsoUtility::VECTOR_ZERO;
    trans_.localRot = LOCAL_DEF_ROT;
	trans_.pos = AsoUtility::VECTOR_ZERO;
	trans_.scl = ROBOT_DEF_SCL;

    MV1SetRotationMatrix(trans_.modelId,
        MatrixUtility::Multiplication(trans_.localRot, trans_.rot));

    MV1SetPosition(trans_.modelId, trans_.pos);
	MV1SetScale(trans_.modelId, trans_.scl);

    //武器の初期化
    weponbeam_ = std::make_unique<WeponBeam>();
    weponbeam_->Init();

    //移動量
    posPow_ = POS_POW;
    //回転量
    rotPow_ = ROT_POW;

    //状態遷移初期設定
    ChangeState(STATE::STANDBY);
}

void RobotBase::Update(void)
{

    // カメラの向き（Y軸回転）をロボットに反映
    if (camera_ != nullptr) {
        VECTOR camAngles = camera_->GetAngles();
        // カメラのY軸回転のみコピー
        trans_.rot.y = camAngles.y;
    }

    ProcessMove();

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

    weponbeam_->Update();
}

void RobotBase::Draw(void)
{
	MV1DrawModel(trans_.modelId);

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
    weponbeam_->Draw();
}

void RobotBase::Release(void)
{
	MV1DeleteModel(trans_.modelId);
    weponbeam_->Release();
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

void RobotBase::SetCamera(Camera* camera)
{
    camera_ = camera;
}

void RobotBase::ProcessMove(void)
{
    //// 入力状態をチェック
    //bool isDownPressed = inpMng_.IsNew(KEY_INPUT_S);
    //bool isUpPressed = inpMng_.IsNew(KEY_INPUT_W);
    //bool isRightPressed = inpMng_.IsNew(KEY_INPUT_D);
    //bool isLeftPressed = inpMng_.IsNew(KEY_INPUT_A);

    //trans_.dir = { 0.0f,0.0f,0.0f };

    //// 上下移動とX軸回転の処理
    //if (isDownPressed) {
    //    trans_.dir = { 0.0f, 0.0f, -1.0f };
    //    if (trans_.rot.x > -MAX_MOVE_ROT) {
    //        trans_.rot.x -= rotPow_;
    //    }
    //}
    //if (isUpPressed) {
    //    trans_.dir = { 0.0f, 0.0f, 1.0f };
    //    if (trans_.rot.x < MAX_MOVE_ROT) {
    //        trans_.rot.x += rotPow_;
    //    }
    //}

    //// 上下キーが押されていない時、X軸回転を0に戻す
    //if (!isDownPressed && !isUpPressed) {
    //    if (trans_.rot.x > rotPow_) {
    //        trans_.rot.x -= rotPow_;
    //    }
    //    if (trans_.rot.x < -rotPow_) {
    //        trans_.rot.x += rotPow_;
    //    }
    //    if (trans_.rot.x >= -rotPow_ && trans_.rot.x <= rotPow_) {
    //        trans_.rot.x = 0.0f;
    //    }
    //}

    //// 左右移動とZ軸回転の処理
    //if (isRightPressed) {
    //    trans_.dir = { 1.0f, 0.0f, 0.0f };
    //    if (trans_.rot.z > -MAX_MOVE_ROT) {
    //        trans_.rot.z -= rotPow_;
    //    }
    //}
    //if (isLeftPressed) {
    //    trans_.dir = { -1.0f, 0.0f, 0.0f };
    //    if (trans_.rot.z < MAX_MOVE_ROT) {
    //        trans_.rot.z += rotPow_;
    //    }
    //}

    //// 左右キーが押されていない時、Z軸回転を0に戻す
    //if (!isRightPressed && !isLeftPressed) {
    //    if (trans_.rot.z > rotPow_) {
    //        trans_.rot.z -= rotPow_;
    //    }
    //    if (trans_.rot.z < -rotPow_) {
    //        trans_.rot.z += rotPow_;
    //    }
    //    if (trans_.rot.z >= -rotPow_ && trans_.rot.z <= rotPow_) {
    //        trans_.rot.z = 0.0f;
    //    }
    //}

    //if (!AsoUtility::EqualsVZero(trans_.dir))
    //{
    //    // XYZの回転行列
    //    // XZ平面移動にする場合は、XZの回転を考慮しないようにする
    //    MATRIX mat = MGetIdent();
    //    mat = MMult(mat, MGetRotY(trans_.rot.y));

    //    // 回転行列を使用して、ベクトルを回転させ
    //    VECTOR moveDir = VTransform(trans_.dir, mat);
    //    // 方向×スピードで移動量を作って、座標に足して移動
    //    trans_.pos = VAdd(trans_.pos, VScale(moveDir, posPow_));
    //}

    // 入力状態をチェック
    VECTOR dir = AsoUtility::VECTOR_ZERO;
    bool isDownPressed = inpMng_.IsNew(KEY_INPUT_S);
    bool isUpPressed = inpMng_.IsNew(KEY_INPUT_W);
    bool isRightPressed = inpMng_.IsNew(KEY_INPUT_D);
    bool isLeftPressed = inpMng_.IsNew(KEY_INPUT_A);

    // 移動方向と目標傾き角を初期化
    dir = { 0.0f, 0.0f, 0.0f };
    float targetPitch = 0.0f;  // X軸回転（前後の傾き）
    float targetRoll = 0.0f;   // Z軸回転（左右の傾き）

    // 上下移動とピッチ（前後傾き）の処理
    if (isDownPressed) {
        dir = { 0.0f, 0.0f, -1.0f };
        targetPitch = -MAX_MOVE_ROT; // 後退時は機体を少し上向きに
    }
    if (isUpPressed) {
        dir = { 0.0f, 0.0f, 1.0f };
        targetPitch = MAX_MOVE_ROT; // 前進時は機体を少し下向きに
    }

    // 左右移動とロール（バンキング）の処理
    if (isRightPressed) {
        // 右移動時の処理
        if (dir.z == 0.0f) {
            // 純粋な横移動
            dir = { 1.0f, 0.0f, 0.0f };
            targetRoll = -MAX_MOVE_ROT; // 右移動時は左にバンク
        }
        else {
            // 斜め移動（前後＋右）
            dir.x = 1.0f;
            dir = VNorm(dir); // 正規化して速度を一定に
            targetRoll = -MAX_MOVE_ROT; // 斜め移動時は傾きを少し抑える
        }
    }
    if (isLeftPressed) {
        // 左移動時の処理
        if (dir.z == 0.0f) {
            // 純粋な横移動
            dir = { -1.0f, 0.0f, 0.0f };
            targetRoll = MAX_MOVE_ROT; // 左移動時は右にバンク
        }
        else {
            // 斜め移動（前後＋左）
            dir.x = -1.0f;
            dir = VNorm(dir); // 正規化
            targetRoll = MAX_MOVE_ROT;
        }
    }

    // ピッチ（X軸回転）のスムーズな適用
    if (isDownPressed || isUpPressed) {
        // 目標角度に向かって徐々に回転
        if (trans_.rot.x < targetPitch) {
            trans_.rot.x += rotPow_;
            trans_.rot.x = min(trans_.rot.x, targetPitch);
        }
        else if (trans_.rot.x > targetPitch) {
            trans_.rot.x -= rotPow_;
            trans_.rot.x = max(trans_.rot.x, targetPitch);
        }
    }
    else {
        // 上下キーが押されていない時、X軸回転を0に戻す
        if (trans_.rot.x > rotPow_) {
            trans_.rot.x -= rotPow_;
        }
        else if (trans_.rot.x < -rotPow_) {
            trans_.rot.x += rotPow_;
        }
        else {
            trans_.rot.x = 0.0f;
        }
    }

    // ロール（Z軸回転）のスムーズな適用
    if (isRightPressed || isLeftPressed) {
        // 目標角度に向かって徐々に回転
        if (trans_.rot.z < targetRoll) {
            trans_.rot.z += rotPow_;
            trans_.rot.z = min(trans_.rot.z, targetRoll);
        }
        else if (trans_.rot.z > targetRoll) {
            trans_.rot.z -= rotPow_;
            trans_.rot.z = max(trans_.rot.z, targetRoll);
        }
    }
    else {
        // 左右キーが押されていない時、Z軸回転を0に戻す
        if (trans_.rot.z > rotPow_) {
            trans_.rot.z -= rotPow_;
        }
        else if (trans_.rot.z < -rotPow_) {
            trans_.rot.z += rotPow_;
        }
        else {
            trans_.rot.z = 0.0f;
        }
    }

    // 移動実行
    if (!AsoUtility::EqualsVZero(dir)) {
        // Y軸回転のみを移動方向に適用（機体の向きに関係なく移動）
        MATRIX mat = MGetIdent();
        mat = MMult(mat, MGetRotY(trans_.rot.y));

        // 回転行列を使用して、ベクトルを回転させ
        trans_.moveDir = VTransform(dir, mat);

        // 方向×スピードで移動量を作って、座標に足して移動
        trans_.pos = VAdd(trans_.pos, VScale(trans_.moveDir, posPow_));
    }
}

void RobotBase::ProcessJump(void)
{
}

void RobotBase::ProcessAttack(void)
{
    if (inpMng_.IsTrgDown(KEY_INPUT_R)
        && !weponbeam_->IsAlive())
    {

        weponbeam_->Use(trans_.pos, trans_.moveDir);
    }
}

void RobotBase::ChangeStandby(void)
{
}

void RobotBase::ChangeKnockback(void)
{
}

void RobotBase::ChangeAttack(void)
{
}

void RobotBase::ChangeDead(void)
{
}

void RobotBase::ChangeVictory(void)
{
}

void RobotBase::ChangeEnd(void)
{
}

void RobotBase::UpdateStandby(void)
{
}

void RobotBase::UpdateKnockback(void)
{
}

void RobotBase::UpdateAttack(void)
{
}

void RobotBase::UpdateDead(void)
{
}

void RobotBase::UpdateVictory(void)
{
}

void RobotBase::UpdateEnd(void)
{
}

void RobotBase::DrawStandby(void)
{
}

void RobotBase::DrawKnockback(void)
{
}

void RobotBase::DrawAttack(void)
{
}

void RobotBase::DrawDead(void)
{
}

void RobotBase::DrawVictory(void)
{
}

void RobotBase::DrawEnd(void)
{
}

