#include <math.h>
#include <DxLib.h>
#include <EffekseerForDXLib.h>

#include "../Utility/AsoUtility.h"
#include "../Object/Common/Transform.h"
#include "../Object/Robot/RobotBase.h"
#include "InputManager.h"

#include "../Application.h"

#include "Camera.h"

Camera::Camera(void)
{
	angles_ = VECTOR();
	cameraUp_ = VECTOR();
	mode_ = MODE::NONE;
	pos_ = AsoUtility::VECTOR_ZERO;
	targetPos_ = AsoUtility::VECTOR_ZERO;
}

Camera::~Camera(void)
{
}

void Camera::Init(void)
{
	ChangeMode(MODE::FIXED_POINT);
}

void Camera::Update(void)
{ 
	ProcessRot();
}

void Camera::SetBeforeDraw(void)
{

	// クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);

	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		SetBeforeDrawFixedPoint();
		break;
	}
	// カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec(
		pos_, 
		targetPos_, 
		cameraUp_
	);

	// DXライブラリのカメラとEffekseerのカメラを同期する。
	Effekseer_Sync3DSetting();

}

void Camera::Draw(void)
{
}

VECTOR Camera::GetPos(void) const
{
	return pos_;
}

VECTOR Camera::GetAngles(void) const
{
	return angles_;
}

VECTOR Camera::GetTargetPos(void) const
{
	return targetPos_;
}

Quaternion Camera::GetQuaRot(void) const
{
	return rot_;
}

Quaternion Camera::GetQuaRotOutX(void) const
{
	return rotOutX_;
}

VECTOR Camera::GetForward(void) const
{
	return VNorm(VSub(targetPos_, pos_));
}

void Camera::ChangeMode(MODE mode)
{

	// カメラの初期設定
	SetDefault();

	// カメラモードの変更
	mode_ = mode;

	// 変更時の初期化処理
	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		// カメラ位置：原点から少し離れたところに置く（例：斜め上から）
		pos_ = VGet(targetPos_.x, targetPos_.y + 300.0f, targetPos_.z - 400.0f);

		targetPos_ = AsoUtility::VECTOR_ZERO;
		// 上方向（Y軸）
		cameraUp_ = AsoUtility::DIR_U;

		SetMouseDispFlag(true);
		break;
	}

}

void Camera::SetRobot(RobotBase* robot)
{
	robot_ = robot;
}

void Camera::SetDefault(void)
{

	// カメラの初期設定
	pos_ = DEFAULT_CAMERA_POS;

	// 注視点
	targetPos_ = AsoUtility::VECTOR_ZERO;

	// カメラの上方向
	cameraUp_ = AsoUtility::DIR_U;

	angles_.x = AsoUtility::Deg2RadF(DEFAULT_CAMERA_ANGLE.x);
	angles_.y = AsoUtility::Deg2RadF(DEFAULT_CAMERA_ANGLE.y);
	angles_.z = 0.0f;

	rot_ = Quaternion();

}

void Camera::SyncFollow(void)
{

	//auto& gIns = GravityManager::GetInstance();

	// 同期先の位置
	VECTOR pos = AsoUtility::VECTOR_ZERO;

	// 重力の方向制御に従う
	//Quaternion gRot = gIns.GetTransform().quaRot;
	Quaternion gRot = Quaternion();

	// 正面から設定されたY軸分、回転させる
	rotOutX_ = gRot.Mult(Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y));

	// 正面から設定されたX軸分、回転させる
	rot_ = rotOutX_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	VECTOR localPos;

	// 注視点(通常重力でいうところのY値を追従対象と同じにする)
	localPos = rotOutX_.PosAxis(LOCAL_F2T_POS);
	targetPos_ = VAdd(pos, localPos);

	// カメラ位置
	localPos = rot_.PosAxis(LOCAL_F2C_POS);
	pos_ = VAdd(pos, localPos);

	// カメラの上方向
	cameraUp_ = gRot.GetUp();

}

void Camera::ProcessRot(void)
{
	// キー入力によるカメラの回転
	auto& ins = InputManager::GetInstance();
	if (angles_.x > -RobotBase::MAX_ROBOT_ANGLES){
		if (ins.IsNew(KEY_INPUT_UP)) { angles_.x -= CAMERA_ANGLE_SPEED; }
	}
	if (angles_.x < RobotBase::MAX_ROBOT_ANGLES){
		if (ins.IsNew(KEY_INPUT_DOWN)) { angles_.x += CAMERA_ANGLE_SPEED; }
	}
	if (ins.IsNew(KEY_INPUT_LEFT)) { angles_.y -= CAMERA_ANGLE_SPEED; }
	if (ins.IsNew(KEY_INPUT_RIGHT)) { angles_.y += CAMERA_ANGLE_SPEED;; }

	//回転行列を使ったカメラ操作処理
	VECTOR localPos = { 0.0f, 0.0f, -RobotBase::ROBOT_CAMERA_RAG };
	// 回転マトリックス生成（Y軸→X軸の順で回転）
	MATRIX mat = MGetIdent();
	mat = MMult(mat, MGetRotX(angles_.x));
	mat = MMult(mat, MGetRotY(angles_.y));
	// 回転を適用
	VECTOR rotatedPos = VTransform(localPos, mat);
	// ワールド座標に変換
	pos_ = VAdd(targetPos_, rotatedPos);
}

void Camera::SetBeforeDrawFixedPoint(void)
{
	if (robot_) {
		targetPos_ = robot_->GetTransform().pos;
	}
	targetPos_.y = 100.0f;
}


