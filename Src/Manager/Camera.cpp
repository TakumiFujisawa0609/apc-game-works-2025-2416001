#include <math.h>
#include <DxLib.h>
#include <EffekseerForDXLib.h>

#include "../Utility/AsoUtility.h"
#include "../Object/Common/Transform.h"
#include "../Object/Robot.h"
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

	// キー入力によるカメラの回転
	auto& ins = InputManager::GetInstance();
	if (angles_.x < Robot::MAX_ROBOT_ANGLES)
	{
		if (ins.IsNew(KEY_INPUT_UP)) {
			angles_.x += 0.05f;
		}
	}
	if (angles_.x > -Robot::MAX_ROBOT_ANGLES)
	{
		if (ins.IsNew(KEY_INPUT_DOWN)) {
			angles_.x -= 0.05f;
		}
	}
	if (ins.IsNew(KEY_INPUT_LEFT)) {
		angles_.y += 0.05f;
	}
	if (ins.IsNew(KEY_INPUT_RIGHT)) {
		angles_.y -= 0.05f;
	}

	// カメラ位置：ロボットクラスのカメラ半径で回転させる
	pos_.x = targetPos_.x + cosf(angles_.y) * Robot::ROBOT_CAMERA_RAG;
	pos_.y = targetPos_.y + sinf(angles_.x) * Robot::ROBOT_CAMERA_RAG;
	pos_.z = targetPos_.z + sinf(angles_.y) * Robot::ROBOT_CAMERA_RAG;
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

void Camera::SetRobot(Robot* robot)
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

	angles_.x = AsoUtility::Deg2RadF(30.0f);
	angles_.y = 0.0f;
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
	//演習① カメラの要件に沿って、カメラ操作を実装してください
	//	・ Y軸に、360度回転すること
	//	・ X軸に、上は40度、下は15度回転すること
	//	・ カメラ操作は矢印キーを用いること

	auto& ins = InputManager::GetInstance();


	//回転軸と量決め
	//const float ROT_POW = 1.0f;
	const float ROT_POW = AsoUtility::Deg2RadF(1.0f);
	VECTOR axisDeg = AsoUtility::VECTOR_ZERO;


	if (angles_.x <= LIMIT_X_UP_RAD)
	{
		if (ins.IsNew(KEY_INPUT_DOWN)) { axisDeg.x = ROT_POW; }
	}

	if (angles_.x >= LIMIT_X_DW_RAD)
	{
		if (ins.IsNew(KEY_INPUT_UP)) { axisDeg.x = -ROT_POW; }
	}


	if (ins.IsNew(KEY_INPUT_RIGHT)) { axisDeg.y = ROT_POW; }
	if (ins.IsNew(KEY_INPUT_LEFT)) { axisDeg.y = -ROT_POW; }


	if (!AsoUtility::EqualsVZero(axisDeg))
	{
		//今回回転させたい回転量をクォータニオンで作る
		Quaternion rotPow = Quaternion();

		/*rotPow = rotPow.Mult(
			Quaternion::AngleAxis(
				AsoUtility::Deg2RadF(axisDeg.z), AsoUtility::AXIS_Z
			));*/
		rotPow = rotPow.Mult(
			Quaternion::AngleAxis(
				AsoUtility::Deg2RadF(axisDeg.x), AsoUtility::AXIS_X
			));
		rotPow = rotPow.Mult(
			Quaternion::AngleAxis(
				AsoUtility::Deg2RadF(axisDeg.y), AsoUtility::AXIS_Y
			));

		// 回転諒を加える(合成)
		angles_ = VAdd(angles_, axisDeg);

	}

}

void Camera::SetBeforeDrawFixedPoint(void)
{
	if (robot_) {
		targetPos_ = robot_->GetTransform().pos;
	}
	targetPos_.y = 100.0f;
}


