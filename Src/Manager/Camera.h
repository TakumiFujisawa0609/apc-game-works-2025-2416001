#pragma once
#include <DxLib.h>
#include "../Common/Quaternion.h"

class RobotBase;
class Transform;

class Camera
{

public:

	// カメラの初期座標
	static constexpr VECTOR DEFAULT_CAMERA_POS = { 0.0f, 100.0f, -500.0f };

	//カメラの初期角度
	static constexpr VECTOR DEFAULT_CAMERA_ANGLE = { 30.0f, 0.0f, 0.0f };

	// 追従位置からカメラ位置までの相対座標
	static constexpr VECTOR LOCAL_F2C_POS = { 0.0f, 0.0f, -600.0f };

	// 追従位置から注視点までの相対座標
	static constexpr VECTOR LOCAL_F2T_POS = { 0.0f, 0.0f, 500.0f };


	// カメラのX回転上限度角
	static constexpr float LIMIT_X_UP_RAD = 40.0f * (DX_PI_F / 180.0f);
	static constexpr float LIMIT_X_DW_RAD = 15.0f * (DX_PI_F / 180.0f);

	//カメラアングル加速度
	static constexpr float CAMERA_ANGLE_SPEED = 5.0f * (DX_PI_F / 180.0f);

	// カメラクリップ：NEAR
	static constexpr float CAMERA_NEAR = 10.0f;

	// カメラクリップ：NEAR
	static constexpr float CAMERA_FAR = 30000.0f;


	// カメラモード
	enum class MODE
	{
		NONE,
		FIXED_POINT,
		TARGET_ROCKE,
	};

	Camera(void);
	~Camera(void);

	void Init(void);
	void Update(void);
	void SetBeforeDraw(void);
	void Draw(void);

	// カメラ位置
	VECTOR GetPos(void) const;
	// カメラの操作角度
	VECTOR GetAngles(void) const;
	// カメラの注視点
	VECTOR GetTargetPos(void) const;

	// カメラ角度
	Quaternion GetQuaRot(void) const;
	// X回転を抜いたカメラ角度
	Quaternion GetQuaRotOutX(void) const;
	// カメラの前方方向
	VECTOR GetForward(void) const;

	// カメラモードの変更
	void ChangeMode(MODE mode);
	//ロボット機能取得関数
	void SetRobot(RobotBase* robot);
	//ターゲット座標取得関数
    void SetTargetTransForom(const Transform* transform);

private:

	//ターゲットTransform
	const Transform* transform_;

	// ロボット
	RobotBase* robot_;

	// カメラモード
	MODE mode_;

	// カメラの位置
	VECTOR pos_;

	// カメラ角度(rad)
	VECTOR angles_;

	// X軸回転が無い角度
	Quaternion rotOutX_;

	// カメラ角度
	Quaternion rot_;

	// 注視点
	VECTOR targetPos_;

	// カメラの上方向
	VECTOR cameraUp_;

	//追尾対象方向設定
	VECTOR targetDir_;

	// カメラを初期位置に戻す
	void SetDefault(void);

	// 追従対象との位置同期を取る
	void SyncFollow(void);

	// カメラ操作
	void ProcessRot(void);

	// モード別更新ステップ
	void SetBeforeDrawFixedPoint(void);

	//ロックオン機能
	void TargetLockeOn(void);

};

