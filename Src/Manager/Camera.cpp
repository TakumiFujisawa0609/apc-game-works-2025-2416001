#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Utility/AsoUtility.h"
#include "../Utility/MatrixUtility.h"
#include "../Object/Common/Transform.h"
#include "../Object/Common/Collider/ColliderModel.h"
#include "../Object/Common/Collider/ColliderSphere.h"
#include "InputManager.h"
#include "Camera.h"

Camera::Camera(void)
	:
	followTransform_(nullptr),
	mode_(MODE::NONE),
	angles_(AsoUtility::VECTOR_ZERO),
	rotY_(Quaternion::Identity()),
	targetPos_(AsoUtility::VECTOR_ZERO)
{
}

Camera::~Camera(void)
{
}

void Camera::Update(void)
{
}

void Camera::SetBeforeDraw(void)
{
	// クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(VIEW_NEAR, VIEW_FAR);

	// 更新前情報
	prePos_ = trans_.pos;

	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		SetBeforeDrawFixedPoint();
		break;
	case Camera::MODE::FREE:
		SetBeforeDrawFree();
		break;
	case Camera::MODE::FOLLOW:
		SetBeforeDrawFollow();
		break;
	case Camera::MODE::TARGET_ROCKE:
		SetBeforeDrawTargetLockeOn();
		break;
	}

	// カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec(
		trans_.pos,
		targetPos_,
		trans_.quaRot.GetUp()
	);

	// DXライブラリのカメラとEffekseerのカメラを同期する。
	Effekseer_Sync3DSetting();

}

void Camera::DrawDebug(void)
{
#ifdef _DEBUG

	if (mode_ == MODE::FIXED_POINT) {
		DrawString(0, 40, "CameraMode:(Defaluto)", GetColor(255, 255, 255));
	}
	else if (mode_ == MODE::TARGET_ROCKE) {
		DrawString(0, 40, "CameraMode:(TagetLocke)", GetColor(255, 255, 255));
	}

#endif
}

void Camera::Release(void)
{
}

void Camera::SetFollow(const Transform* follow)
{
	followTransform_ = follow;
}

void Camera::SetTargetFollow(const Transform* follow)
{
	targetTransform_ = follow;
}

void Camera::AddHitCollider(const ColliderBase* hitCollider)
{
	for (const auto& c : hitColliders_)
	{
		if (c == hitCollider)
		{
			return;
		}
	}
	hitColliders_.emplace_back(hitCollider);
}

void Camera::InitCollider(void)
{
	// 主に地面との衝突で使用する球体コライダ
	ColliderSphere* colliderSphere = new ColliderSphere(
		ColliderBase::TAG::CAMERA,
		&trans_,
		AsoUtility::VECTOR_ZERO,
		COL_CAPSULE_SPHERE
	);
	ownColliders_.emplace(
		static_cast<int>(ColliderBase::SHAPE::SPHERE), colliderSphere);

}

void Camera::InitPost(void)
{
	ChangeMode(MODE::FIXED_POINT);
}

const VECTOR& Camera::GetPos(void) const
{
	return  trans_.pos;
}

const VECTOR& Camera::GetAngles(void) const
{
	return angles_;
}

const VECTOR& Camera::GetTargetPos(void) const
{
	return targetPos_;
}

const Quaternion& Camera::GetQuaRot(void) const
{
	return trans_.quaRot;
}

const Quaternion& Camera::GetQuaRotY(void) const
{
	return rotY_;
}

VECTOR Camera::GetForward(void) const
{
	return VNorm(VSub(targetPos_, trans_.pos));
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
		break;
	case Camera::MODE::FREE:
		break;
	case Camera::MODE::FOLLOW:
		break;
	case Camera::MODE::TARGET_ROCKE:
		break;
	}

}

void Camera::SetDefault(void)
{

	// カメラの初期設定
	trans_.pos = DERFAULT_POS;

	// カメラ角
	angles_ = DERFAULT_ANGLES;
	trans_.quaRot = Quaternion::Identity();

	// 注視点
	targetPos_ = AsoUtility::VECTOR_ZERO;
}

void Camera::SyncFollow(void)
{

	// 同期先の位置
	VECTOR pos = followTransform_->pos;

	// Y軸
	rotY_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);

	// Y軸 + X軸
	trans_.quaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	VECTOR localPos;

	// 注視点
	localPos = trans_.quaRot.PosAxis(FOLLOW_TARGET_LOCAL_POS);
	targetPos_ = VAdd(pos, localPos);

	// カメラ位置
	localPos = trans_.quaRot.PosAxis(FOLLOW_CAMERA_LOCAL_POS);
	trans_.pos = VAdd(pos, localPos);

}

void Camera::ProcessRot(bool isLimit)
{

	if (GetJoypadNum() == 0)
	{
		// 方向回転によるXYZの移動(キーボード)
		RotKeyboard(isLimit);
	}
	else
	{
		// 方向回転によるXYZの移動(ゲームパッド)
		RotGamePad(isLimit);
	}

}

void Camera::ProcessMove(void)
{

	auto& ins = InputManager::GetInstance();

	VECTOR moveDir = AsoUtility::VECTOR_ZERO;

	if (GetJoypadNum() == 0)
	{
		if (ins.IsNew(KEY_INPUT_UP)) { moveDir = AsoUtility::DIR_F; }
		if (ins.IsNew(KEY_INPUT_DOWN)) { moveDir = AsoUtility::DIR_B; }
		if (ins.IsNew(KEY_INPUT_LEFT)) { moveDir = AsoUtility::DIR_L; }
		if (ins.IsNew(KEY_INPUT_RIGHT)) { moveDir = AsoUtility::DIR_R; }
	}
	else
	{

		InputManager::JOYPAD_IN_STATE padState =
			ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

		// 左スティックの傾き
		moveDir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

	}

	// 移動処理
	if (!AsoUtility::EqualsVZero(moveDir))
	{

		// 移動させたい方向(ベクトル)に変換

		// 現在の向きからの進行方向を取得
		VECTOR direction = VNorm(trans_.quaRot.PosAxis(moveDir));

		// 移動させたい方向に移動量をかける(=移動量)
		VECTOR movePow = VScale(direction, SPEED);

		// カメラ位置も注視点も移動させる
		trans_.pos = VAdd(trans_.pos, movePow);
		targetPos_ = VAdd(targetPos_, movePow);

	}

}

void Camera::SetBeforeDrawFixedPoint(void)
{
	// 何もしない
}

void Camera::SetBeforeDrawFree(void)
{

	// カメラ操作(回転)
	ProcessRot(false);

	// カメラ操作(移動)
	ProcessMove();

	// Y軸
	rotY_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);

	// Y軸 + X軸
	trans_.quaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	// 注視点更新
	targetPos_ = VAdd(trans_.pos, trans_.quaRot.PosAxis(FOLLOW_TARGET_LOCAL_POS));
}

void Camera::SetBeforeDrawFollow(void)
{

	// カメラ操作(回転)
	ProcessRot(true);

	// 追従対象との相対位置を同期
	SyncFollow();

	// 衝突判定
	Collision();

	// カメラ位置の補間
	if (isCameraLope_) {
		trans_.pos =
			AsoUtility::Lerp(prePos_, trans_.pos, LERP_RATE_MOVE);
	}

}

void Camera::Collision(void)
{
	// プレイヤーのルートフレーム
	VECTOR start = MV1GetFramePosition(followTransform_->modelId, 1);

	for (const auto& hitCol : hitColliders_)
	{
		// モデル以外は処理を飛ばす
		if (hitCol->GetShape() != ColliderBase::SHAPE::MODEL) continue;

		// 派生クラスへキャスト
		const ColliderModel* colliderModel =
			dynamic_cast<const ColliderModel*>(hitCol);

		if (colliderModel == nullptr) continue;

		// 線分で衝突判定
		auto hits = MV1CollCheck_LineDim(
			colliderModel->GetFollow()->modelId,
			-1,
			trans_.pos,
			start
		);

		// 追従対象に一番近い衝突点を探す
		bool isCollision_ = false;
		isCameraLope_ = false;
		MV1_COLL_RESULT_POLY hitPoly;
		double minDist = DBL_MAX;
		for (int i = 0; i < hits.HitNum; i++)
		{
			const auto& hit = hits.Dim[i];

			//// 除外フレームは無視する
			//if (colliderModel->IsExcludeFrame(hit.FrameIndex))
			//{
			//	continue;
			//}

			// 対象フレーム以外は無視する
			if (!colliderModel->IsTargetFrame(hit.FrameIndex))
			{
				continue;
			}

			// 衝突判定
			isCollision_ = true;
			isCameraLope_ = true;

			// 距離判定
			double dist = AsoUtility::Distance(start, hit.HitPosition);
			if (minDist > dist)
			{
				// 追従対象に一番近い衝突点を優先
				minDist = dist;
				hitPoly = hit;
			}
		}

		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);

		if (!isCollision_)
		{
			// 衝突していなければ次のコライダへ
			continue;
		}

		// カメラ位置から注視点への方向
		VECTOR dirToTarget = VNorm(VSub(targetPos_, trans_.pos));

		// 衝突点の少し手前にカメラを置く
		trans_.pos =
			VAdd(hitPoly.HitPosition, VScale(dirToTarget, COLLISION_BACK_DIS));

		// カメラ位置の球体コライダ
		int typeSphere = static_cast<int>(ColliderBase::SHAPE::SPHERE);

		// 球体コライダが無ければ処理を抜ける
		if (ownColliders_.count(typeSphere) == 0) continue;

		// 指定された回数と距離で三角形の法線方向に押し戻す
		trans_.pos =
			ownColliders_.at(typeSphere)->GetPosPushBackAlongNormal(
				hitPoly, CNT_TRY_COLLISION_CAMERA, COLLISION_BACK_DIS);
	}
}

void Camera::RotKeyboard(bool isLimit)
{

	const auto& ins = InputManager::GetInstance();

	// カメラ回転
	if (ins.IsNew(KEY_INPUT_RIGHT))
	{
		// 右回転
		angles_.y += ROT_POW_RAD;
	}
	if (ins.IsNew(KEY_INPUT_LEFT))
	{
		// 左回転
		angles_.y -= ROT_POW_RAD;
	}

	// 上回転
	if (ins.IsNew(KEY_INPUT_UP))
	{
		angles_.x += ROT_POW_RAD;
		if (isLimit && angles_.x > LIMIT_X_UP_RAD)
		{
			angles_.x = LIMIT_X_UP_RAD;
		}
	}

	// 下回転
	if (ins.IsNew(KEY_INPUT_DOWN))
	{
		angles_.x -= ROT_POW_RAD;
		if (isLimit && angles_.x < -LIMIT_X_DW_RAD)
		{
			angles_.x = -LIMIT_X_DW_RAD;
		}
	}

}

void Camera::RotGamePad(bool isLimit)
{

	auto& ins = InputManager::GetInstance();

	// 接続されているゲームパッド１の情報を取得
	InputManager::JOYPAD_IN_STATE padState =
		ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

	// 右スティックの傾き
	VECTOR dir = ins.GetDirectionXZAKey(padState.AKeyRX, padState.AKeyRY);

	// 右スティック左右の傾き
	angles_.y += dir.x * ROT_POW_RAD;

	// 右スティック上下の傾き
	angles_.x += dir.z * ROT_POW_RAD;

	// 角度制限
	if (isLimit && angles_.x < -LIMIT_X_DW_RAD)
	{
		angles_.x = -LIMIT_X_DW_RAD;
	}
	if (isLimit && angles_.x > LIMIT_X_UP_RAD)
	{
		angles_.x = LIMIT_X_UP_RAD;
	}

}


void Camera::SetBeforeDrawTargetLockeOn(void)
{
	// デバッグ球体へのベクトルを計算
	VECTOR toTarget = VSub(targetTransform_->pos, followTransform_->pos);

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
	VECTOR robotForward = followTransform_->targetDir;

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
	trans_.pos = VAdd(followTransform_->targetDir, worldOffset);
	targetPos_ = targetTransform_->pos;
}


