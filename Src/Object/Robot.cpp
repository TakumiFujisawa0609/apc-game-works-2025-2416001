#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "./Common/Transform.h"
#include "Robot.h"

Robot::Robot(void)
{
}

Robot::~Robot(void)
{
}

void Robot::Init(void)
{
	trans_.modelId = resMng_.LoadModelDuplicate(ResourceManager::SRC::ROBOT);
	trans_.rot = ROBOT_DEF_ROT;
	trans_.pos = ROBOT_DEF_POS;
	trans_.scl = ROBOT_DEF_SCL;

    MV1SetPosition(trans_.modelId, trans_.pos);
	MV1SetScale(trans_.modelId, trans_.scl);
	MV1SetRotationXYZ(trans_.modelId, trans_.rot);

    //移動量
    posPow_ = POS_POW;
    //回転量
    rotPow_ = ROT_POW;
}

void Robot::Update(void)
{
    PressedMove();

    MV1SetPosition(trans_.modelId, trans_.pos);
	MV1SetRotationXYZ(trans_.modelId, trans_.rot);
}

void Robot::Draw(void)
{
	MV1DrawModel(trans_.modelId);
}

void Robot::Release(void)
{
	MV1DeleteModel(trans_.modelId);
}

void Robot::PressedMove(void)
{
    // 入力状態をチェック
    bool isDownPressed = inpMng_.IsNew(KEY_INPUT_S);
    bool isUpPressed = inpMng_.IsNew(KEY_INPUT_W);
    bool isRightPressed = inpMng_.IsNew(KEY_INPUT_D);
    bool isLeftPressed = inpMng_.IsNew(KEY_INPUT_A);

    // 上下移動とX軸回転の処理
    if (isDownPressed) {
        trans_.pos.z -= posPow_;
        if (trans_.rot.x < MAX_MOVE_ROT) {
            trans_.rot.x += rotPow_;
        }
    }
    if (isUpPressed) {
        trans_.pos.z += posPow_;
        if (trans_.rot.x > -MAX_MOVE_ROT) {
            trans_.rot.x -= rotPow_;
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
        trans_.pos.x += posPow_;
        if (trans_.rot.z > -MAX_MOVE_ROT) {
            trans_.rot.z -= rotPow_;
        }
    }
    if (isLeftPressed) {
        trans_.pos.x -= posPow_;
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

    // 座標の設定
    MV1SetPosition(trans_.modelId, trans_.pos);
    // 角度の設定
    MV1SetRotationXYZ(trans_.modelId, trans_.rot);
}
