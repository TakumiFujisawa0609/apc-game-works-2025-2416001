#pragma once
#include <DxLib.h>
#include "ObjectBase.h"
class Robot
	:public ObjectBase
{
public:

	//初期位置
	static constexpr VECTOR ROBOT_DEF_POS = { 0.0f,0.0f,0.0f };
	//初期サイズ
	static constexpr VECTOR ROBOT_DEF_SCL = { 0.1f,0.1f,0.1f };
	//初期角度
	static constexpr VECTOR ROBOT_DEF_ROT = { 0.0f,180.0f * DX_PI_F / 180.0f, 0.0f };

	//最大回転量
	static constexpr float MAX_MOVE_ROT = 15.0f * DX_PI_F / 180.0f;

	//移動量
	static constexpr float POS_POW = 3.0f;
	//回転量
	static constexpr float ROT_POW = 2.5f * DX_PI_F / 180.0f;

	//ロボットのカメラ半径
	static constexpr float ROBOT_CAMERA_RAG = 300.0f;
	static constexpr float MAX_ROBOT_ANGLES = 40.0f * DX_PI_F / 180.0f;

	// コンストラクタ
	Robot(void);

	// デストラクタ
	~Robot(void);

	//初期化処理
	void Init(void)override;
	//更新処理
	void Update(void)override;
	//描画処理
	void Draw(void)override;
	//解放処理
	void Release(void)override;

private:

	//移動処理
	void PressedMove(void);

	//移動量
	float posPow_;
	//回転量
	float rotPow_;

};

