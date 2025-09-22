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
    // 角度から方向に変換する
    trans_.moveDir = { sinf(trans_.rot.y), 0.0f, cosf(trans_.rot.y) };

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
        /*trans_.rot.y = camAngles.y;*/
    }

    DelayRotate();
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
    VECTOR dir = { 0.0f,0.0f,0.0f };
    // 入力状態をチェック
    bool isDownPressed = inpMng_.IsNew(KEY_INPUT_S);
    bool isUpPressed = inpMng_.IsNew(KEY_INPUT_W);
    bool isRightPressed = inpMng_.IsNew(KEY_INPUT_D);
    bool isLeftPressed = inpMng_.IsNew(KEY_INPUT_A);

    // 上下移動とX軸回転の処理
    if (isDownPressed) {
        dir = { 0.0f, 0.0f, -1.0f };
        if (trans_.rot.x > -MAX_MOVE_ROT) {
            trans_.rot.x -= rotPow_;
        }
    }
    if (isUpPressed) {
        dir = { 0.0f, 0.0f, 1.0f };
        if (trans_.rot.x < MAX_MOVE_ROT) {
            trans_.rot.x += rotPow_;
        }
    }

    // 上下キーが押されていない時、X軸回転を0に戻す
    if (!isDownPressed && !isUpPressed) {
        if (trans_.rot.x > rotPow_) {
            trans_.rot.x -= rotPow_;
        }
        if (trans_.rot.x < -rotPow_) {
            trans_.rot.x += rotPow_;
        }
        if (trans_.rot.x >= -rotPow_ && trans_.rot.x <= rotPow_) {
            trans_.rot.x = 0.0f;
        }
    }

    // 左右移動とZ軸回転の処理
    if (isRightPressed) {
        dir = { 1.0f, 0.0f, 0.0f };
        if (trans_.rot.z > -MAX_MOVE_ROT) {
            trans_.rot.z -= rotPow_;
        }
    }
    if (isLeftPressed) {
        dir = { -1.0f, 0.0f, 0.0f };
        if (trans_.rot.z < MAX_MOVE_ROT) {
            trans_.rot.z += rotPow_;
        }
    }

    // 左右キーが押されていない時、Z軸回転を0に戻す
    if (!isRightPressed && !isLeftPressed) {
        if (trans_.rot.z > rotPow_) {
            trans_.rot.z -= rotPow_;
        }
        if (trans_.rot.z < -rotPow_) {
            trans_.rot.z += rotPow_;
        }
        if (trans_.rot.z >= -rotPow_ && trans_.rot.z <= rotPow_) {
            trans_.rot.z = 0.0f;
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
        // 移動方向から角度に変換する
        //angles_.y = atan2f(moveDir_.x, moveDir_.z);
        // 方向×スピードで移動量を作って、座標に足して移動
        trans_.pos = VAdd(trans_.pos, VScale(trans_.moveDir, posPow_));

    }
}

void RobotBase::DelayRotate(void)
{
    // 移動方向から角度に変換する
    float goal = atan2f(trans_.moveDir.x, trans_.moveDir.z);
    // 常に最短経路で補間
    trans_.rot.y = AsoUtility::LerpAngle(trans_.rot.y, goal, 0.2f);
}

void RobotBase::ProcessJump(void)
{
}

void RobotBase::ProcessAttack(void)
{
    if (inpMng_.IsTrgDown(KEY_INPUT_R)
        && !weponbeam_->IsAlive())
    {

        weponbeam_->Use(trans_.pos,trans_.rot, trans_.moveDir);
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

