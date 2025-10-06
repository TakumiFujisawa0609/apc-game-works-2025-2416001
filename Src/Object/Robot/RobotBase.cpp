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

    for (int i = 0; i < 10; i++)
    {
        weponMissile_.push_back(std::make_unique<WeponMissile>());
    }

    for(const auto& missile_ : weponMissile_)
    {
        missile_->Init();
    }

    //移動量
    movePow_ = 0;
    //上昇量
    rise_ = RISE_SPEED;
    //回転量
    rotPow_ = ROT_POW;

    lockcnt = 0;

    anim_ = new AnimationController(trans_.modelId);

    anim_->Add(
        static_cast<int>(ANIM_TYPE::WIKE),
        Application::PATH_MODEL + "Walk.mv1",
        30.0f
    );

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
    // カメラの向き（Y軸回転）をロボットに反映
    if (camera_ != nullptr) {
        VECTOR camAngles = camera_->GetAngles();
        // カメラのY軸回転のみコピー
        /*trans_.rot.y = camAngles.y;*/
    }

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
    weponMissile_->UpdateTarget(debugSpherePos_);
    weponMissile_->Update();

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
    weponMissile_->Draw();

#ifdef _DEBUG

    DrawFormatString(0, 20, GetColor(255, 255, 255), "加速度：%.1f",movePow_);

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
    weponMissile_->Release();
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
    //移動方向
    VECTOR dir = { 0.0f,0.0f,0.0f };
    //移動量を常に減少
    movePow_ *= FRICTION;

    // 入力状態をチェック
    bool isBustPressed = inpMng_.IsTrgDown(KEY_INPUT_B);
    bool isDownPressed =inpMng_.IsNew(KEY_INPUT_S);
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

    //ブースト時の移動量と最大値を設定
    float maxMoveSpeed = MAX_MOVE_SPEED;
    if (isBustPressed)
    {
        movePow_ += BUST_SPEED;
        maxMoveSpeed = MAX_BUST_SPEED;
        if (trans_.rot.x < MAX_BOOST_ROT) {
            trans_.rot.x += rotPow_;
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
                trans_.rot.x += rotPow_;
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
}

void RobotBase::DelayRotate(void)
{
    // 移動方向から角度に変換する
    float goal = atan2f(trans_.moveDir.x, trans_.moveDir.z);
    // 常に最短経路で補間
    trans_.rot.y = AsoUtility::LerpAngle(trans_.rot.y, goal, 0.2f);
}

void RobotBase::ProcessRise(void)
{
    bool isRisePressed = inpMng_.IsNew(KEY_INPUT_G);

    if (isRisePressed)
    {
        trans_.pos.y += rise_;
    }

    if(trans_.pos.y > 0)
    {
        trans_.pos.y -= GRAVITY;
    }
    MV1SetPosition(trans_.modelId, trans_.pos);
}

void RobotBase::ProcessAttack(void)
{
    VECTOR targetDir = trans_.moveDir;
    if (IsTargetLockFlage())
    {
        targetDir = trans_.targetDir;
    }

    if (inpMng_.IsTrgDown(KEY_INPUT_R)
        && !weponbeam_->IsAlive())
    {
        weponbeam_->Use(trans_.pos, targetDir);
    }

    if (inpMng_.IsTrgDown(KEY_INPUT_F)
        && !weponMissile_->IsAlive())
    {
        weponMissile_->Use(trans_.pos, targetDir);
    }
}

void RobotBase::ProcessTargetLock(void)
{
    bool  isLock = inpMng_.IsTrgDown(KEY_INPUT_L);

    //対象ロック処理
    if (isLock)
    {
        lockcnt++;
    }


    if (!IsTargetLockFlage())
    {
        return;
    }

    // デバッグ球体へのベクトルを計算
    VECTOR toTarget = VSub(debugSpherePos_, trans_.pos);

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

    //// 即座にターゲット方向を向く（補間なし）
    //trans_.rot.y = targetAngleY;
    //trans_.rot.x = targetAngleX;

    // 滑らかに回転させる（補間）
    trans_.rot.y = AsoUtility::LerpAngle(trans_.rot.y, targetAngleY, 0.5f);
    trans_.rot.x = AsoUtility::LerpAngle(trans_.rot.x, targetAngleX, 0.5f);

    // 移動方向もターゲット方向に更新
    trans_.targetDir = targetDir;

    // ローカル回転とグローバル回転を合成してモデルに適用
    MV1SetRotationMatrix(trans_.modelId,
        MatrixUtility::Multiplication(trans_.localRot, trans_.rot));
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

