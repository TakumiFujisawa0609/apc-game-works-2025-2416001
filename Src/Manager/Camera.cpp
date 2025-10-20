#include <math.h>
#include <DxLib.h>
#include <EffekseerForDXLib.h>

#include "../Utility/AsoUtility.h"
#include "../Utility/MatrixUtility.h"
#include "../Object/Common/Transform.h"
#include "../Object/Robot/Player/Player.h"
#include "../Object/Robot/Enemy/EnemyBase.h"
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
	player_ = nullptr;
	enemys_ = nullptr;
}

Camera::~Camera(void)
{
}

void Camera::Init(void)
{
	angles_.x = AsoUtility::Deg2RadF(DEFAULT_CAMERA_ANGLE.x);
	angles_.y = AsoUtility::Deg2RadF(DEFAULT_CAMERA_ANGLE.y);
	angles_.z = 0.0f;
}

void Camera::Update(void)
{
}

void Camera::SetBeforeDraw(void)
{
	// クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);

	switch (mode_)
	{
	case Camera::MODE::FREE:
		SetBeforeDrawFixedPoint();
		break;
	case Camera::MODE::FIXED_POINT:
		SetBeforePlayerDraw();
		break;
	case Camera::MODE::TARGET_ROCKE:
		TargetLockeOn();
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
#ifdef _DEBUG

	if(mode_ == MODE::FIXED_POINT){
		DrawString(0, 40, "CameraMode:(Defaluto)", GetColor(255, 255, 255));
	}
	else if (mode_ == MODE::TARGET_ROCKE){
		DrawString(0, 40, "CameraMode:(TagetLocke)", GetColor(255, 255, 255));
	}

#endif
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

	if (enemys_ == nullptr) {
		mode_ = Camera::MODE::FIXED_POINT;
	}

	// 変更時の初期化処理
	switch (mode_)
	{
	case Camera::MODE::FREE:
		break;
	case Camera::MODE::FIXED_POINT:
		break;
	case Camera::MODE::TARGET_ROCKE:
		break;
	}

}

void Camera::SetPlayer(Player* player)
{
	player_ = player;
}

void Camera::SetEnemy(EnemyBase* enemys)
{
	enemys_ = enemys;
}

void Camera::SetDefault(void)
{
	// カメラの初期設定
	pos_ = DEFAULT_CAMERA_POS;
	// 注視点
	targetPos_ = AsoUtility::VECTOR_ZERO;
	// カメラの上方向
	cameraUp_ = AsoUtility::DIR_U;

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
	if (GetJoypadNum() == 0)
	{
		if (angles_.x > -Player::MAX_ROBOT_ANGLES) {
			if (ins.IsNew(KEY_INPUT_UP))
			{
				angles_.x -= CAMERA_ANGLE_SPEED;
			}
		}
		if (angles_.x < Player::MAX_ROBOT_ANGLES) {
			if (ins.IsNew(KEY_INPUT_DOWN))
			{
				angles_.x += CAMERA_ANGLE_SPEED;
			}
		}

		if (ins.IsNew(KEY_INPUT_LEFT))
		{
			angles_.y -= CAMERA_ANGLE_SPEED;
		}

		if (ins.IsNew(KEY_INPUT_RIGHT))
		{
			angles_.y += CAMERA_ANGLE_SPEED;
		}
	}
	else
	{
		auto& ins = InputManager::GetInstance();
		// 矢印キーでカメラの角度を変える
		float rotPow = 1.0f * DX_PI_F / 180.0f;

		// 接続されているゲームパッド１の情報を取得
		InputManager::JOYPAD_IN_STATE padState =
			ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
		// アナログキーの入力値から方向を取得
		VECTOR dir = ins.GetDirectionXZAKey(padState.AKeyRX, padState.AKeyRY);

		//右スティックが上下の傾き
		angles_.x += dir.z * rotPow * 3.0f;
		angles_.y += dir.x * rotPow * 3.0f;

		if (angles_.x > 50.0f * DX_PI_F / 180.0f)
		{
			angles_.x = 50.0f * DX_PI_F / 180.0f;
		}
		if (angles_.x < -(20.0f * DX_PI_F / 180.0f))
		{
			angles_.x = -(20.0f * DX_PI_F / 180.0f);
		}
	}

	//回転行列を使ったカメラ操作処理
	VECTOR PlayerlocalPos = { 0.0f,0.0f,500.0f };
	VECTOR localPos = { PlayerlocalPos.x, PlayerlocalPos.y, -PlayerlocalPos.z };
	// 回転マトリックス生成（Y軸→X軸の順で回転）
	MATRIX mat = MGetIdent();
	mat = MMult(mat, MGetRotX(angles_.x));
	mat = MMult(mat, MGetRotY(angles_.y));
	// 回転を適用
	VECTOR rotatedPos = VTransform(localPos, mat);
	// ワールド座標に変換
	pos_ = VAdd(targetPos_, rotatedPos);

	VECTOR up = { 0.0f, 1.0f, 0.0f };

	// カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec(
		pos_,
		targetPos_,
		up
	);
}

void Camera::SetBeforeDrawFixedPoint(void)
{
	// カメラの設定(位置と角度による制御)
	SetCameraPositionAndAngle(
		pos_,
		angles_.x,
		angles_.y,
		angles_.z
	);
}

void Camera::SetBeforePlayerDraw(void)
{
	targetPos_ = player_->GetTransform().pos;
	targetPos_.y += 200.0f;

	ProcessRot();
}

void Camera::TargetLockeOn(void)
{
	// デバッグ球体へのベクトルを計算
	VECTOR toTarget = VSub(enemys_->GetTransform().pos, player_->GetTransform().pos);

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

	// 滑らかに回転させる（補間）
	angles_.y = AsoUtility::LerpAngle(angles_.y, targetAngleY, 0.1f);
	angles_.x = AsoUtility::LerpAngle(angles_.x, targetAngleX, 0.1f);

	// ロボットが向いている方向を取得
	VECTOR robotForward = player_->GetTransform().targetDir;

	// ロボットの向き（Y軸回転角度）を前方ベクトルから計算
	float robotAngleY = atan2f(robotForward.x, robotForward.z);

	// ターゲット方向のX軸回転角度を計算（上下の角度）
	float robotAngleX = atan2f(robotForward.z, robotForward.y);

	// カメラのローカルオフセット（ロボットから見た相対位置）
	VECTOR localOffset = VGet(100.0f, 250.0f, -300.0f);

	// ロボットの向きに応じてオフセットを回転
	MATRIX rotMat;
	rotMat = MGetRotX(robotAngleX);
	rotMat = MGetRotY(robotAngleY);
	VECTOR worldOffset = VTransform(localOffset, rotMat);

	// カメラの位置を計算
	pos_ = VAdd(player_->GetTransform().pos, worldOffset);
	targetPos_ = enemys_->GetTransform().pos;
}


