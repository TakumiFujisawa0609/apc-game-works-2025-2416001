#include "../../../Application.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/Camera.h"
#include "../../../Manager/InputManager.h"
#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/MatrixUtility.h"
#include "../../Common/AnimationController.h"
#include "./../../Common/Transform.h"
#include "../../Wepon/WeponBeam.h"
#include "../../Wepon/WeponMissile.h"

#include "Player.h"

Player::Player(void)
{
}

Player::~Player(void)
{
}

void Player::Update(void)
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

    MV1SetPosition(trans_.modelId, trans_.pos);
    MV1SetRotationMatrix(trans_.modelId,
        MatrixUtility::Multiplication(trans_.localRot, trans_.rot));

    switch (state_)
    {
    case Player::STATE::STANDBY:
        UpdateStandby();
        break;
    case Player::STATE::KNOCKBACK:
        UpdateKnockback();
        break;
    case Player::STATE::ATTACK:
        UpdateAttack();
        break;
    case Player::STATE::DEAD:
        UpdateDead();
        break;
    case Player::STATE::END:
        UpdateEnd();
        break;
    case Player::STATE::VICTORY:
        UpdateVictory();
        break;
    default:
        break;
    }

    weponbeam_->Update();
    for (int i = 0; i < weponMissile_.size(); i++)
    {
        weponMissile_[i]->UpdateTarget(debugSpherePos_);
        weponMissile_[i]->Update();
    }

    //状態遷移初期設定
    ChangeState(STATE::STANDBY);

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
}

void Player::Draw(void)
{
    MV1DrawModel(trans_.modelId);

    weponbeam_->Draw();
    for (int i = 0; i < weponMissile_.size(); i++)
    {
        weponMissile_[i]->Draw();
    }

    switch (state_)
    {
    case Player::STATE::STANDBY:
        DrawStandby();
        break;
    case Player::STATE::KNOCKBACK:
        DrawKnockback();
        break;
    case Player::STATE::ATTACK:
        DrawAttack();
        break;
    case Player::STATE::DEAD:
        DrawDead();
        break;
    case Player::STATE::END:
        DrawEnd();
        break;
    case Player::STATE::VICTORY:
        DrawVictory();
        break;
    default:
        break;
    }

#ifdef _DEBUG
    DrawFormatString(0, 20, GetColor(255, 255, 255), "加速度：%.1f", movePow_);

    DrawSphere3D(debugSpherePos_, 100.0f, 16, 0xFFFF00, 0xAAAA00, false);

    DrawFormatString(
        0, 40, GetColor(255, 255, 255),
        "座標：(%.1f,%.1f,%.1f)",
        debugSpherePos_.x,
        debugSpherePos_.y,
        debugSpherePos_.z
    );
#endif
}

void Player::Release(void)
{
    MV1DeleteModel(trans_.modelId);

    weponbeam_->Release();
    for (int i = 0; i < weponMissile_.size(); i++)
    {
        weponMissile_[i]->Release();
    }
}

void Player::SetCamera(Camera* camera)
{
    camera_ = camera;
}

void Player::InitLoad(void)
{
    trans_.modelId = resMng_.LoadModelDuplicate(ResourceManager::SRC::ROBOT);
}

void Player::InitTransform(void)
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
}

void Player::InitAnimation(void)
{
    anim_ = new AnimationController(trans_.modelId);

    anim_->Add(
        static_cast<int>(ANIM_TYPE::WIKE),
        Application::PATH_MODEL + "Walk.mv1",
        30.0f
    );
}

void Player::InitPost(void)
{
    //移動量
    movePow_ = 0;
    //上昇量
    rise_ = RISE_SPEED;
    //回転量
    rotPow_ = ROT_POW;

    lockcnt = 0;
}

void Player::ChangeState(STATE state)
{
    state_ = state;

    switch (state_)
    {
    case Player::STATE::STANDBY:
        ChangeStandby();
        break;
    case Player::STATE::KNOCKBACK:
        ChangeKnockback();
        break;
    case Player::STATE::ATTACK:
        ChangeAttack();
        break;
    case Player::STATE::DEAD:
        ChangeDead();
        break;
    case Player::STATE::END:
        ChangeEnd();
        break;
    case Player::STATE::VICTORY:
        ChangeVictory();
        break;
    default:
        break;
    }
}

void Player::ProcessMove(void)
{
    //移動方向
    VECTOR dir = { 0.0f,0.0f,0.0f };
    //移動量を常に減少
    movePow_ *= FRICTION;

    // 入力状態をチェック
    bool isBustPressed = inpMng_.IsTrgDown(KEY_INPUT_B);
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

void Player::ProcessRise(void)
{
    bool isRisePressed = inpMng_.IsNew(KEY_INPUT_G);

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

    if (inpMng_.IsTrgDown(KEY_INPUT_R)
        && !weponbeam_->IsAlive())
    {
        weponbeam_->Use(trans_.pos, targetDir);
    }

    if (inpMng_.IsTrgDown(KEY_INPUT_F))
    {
        for (int i = 0; i < weponMissile_.size(); i++)
        {
            if (!weponMissile_[i]->IsAlive())
            {
                // ランダムな角度でばらけさせる
                int randAnglePow = 10000;
                int randAnglehraf = randAnglePow / 2;
                float randomAngleY = targetDir.y + ((rand() % randAnglePow - randAnglehraf) / 100.0f);
                float randomAngleX = targetDir.x + ((rand() % randAnglePow - randAnglehraf) / 100.0f);
                float randomAngleZ = targetDir.z + ((rand() % randAnglePow - randAnglehraf) / 100.0f);

                VECTOR spreadDir = VNorm(VGet(randomAngleX, randomAngleY, randomAngleZ));

                weponMissile_[i]->Use(trans_.pos, spreadDir);
            }
        }
    }
}

void Player::ProcessTargetLock(void)
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

void Player::ChangeStandby(void)
{
}

void Player::ChangeKnockback(void)
{
}

void Player::ChangeAttack(void)
{
}

void Player::ChangeDead(void)
{
}

void Player::ChangeVictory(void)
{
}

void Player::ChangeEnd(void)
{
}

void Player::UpdateStandby(void)
{
}

void Player::UpdateKnockback(void)
{
}

void Player::UpdateAttack(void)
{
}

void Player::UpdateDead(void)
{
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
