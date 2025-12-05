#include"../../../Utility/AsoUtility.h"
#include "../../../Common/Quaternion.h"
#include"Model.h"
#include"Cube.h"
#include"Sphere.h"
#include"Line.h"
#include"Capsule.h"

//***************************************************
//カプセル
//***************************************************

Capsule::Capsule(const VECTOR& _pos, const Quaternion& _rot, const VECTOR _localPosTop, const VECTOR _localPosDown, const float _radius) : Geometry(_pos,_rot),
	localPosTop_(_localPosTop),
	localPosDown_(_localPosDown),
	radius_(_radius)
{
	std::memset(&hitInfo_, 0, sizeof(hitInfo_));
}

Capsule::Capsule(const Capsule& _copyBase, const VECTOR& _pos, const Quaternion& _rot) : Geometry(_pos,_rot)
{
	radius_ = _copyBase.GetRadius();
	localPosTop_ = _copyBase.GetLocalPosTop();
	localPosDown_ = _copyBase.GetLocalPosDown();
	std::memset(&hitInfo_, 0, sizeof(hitInfo_));
}

Capsule::~Capsule(void)
{
}

void Capsule::Draw(void)
{
	// 上の球体
	VECTOR pos1 = GetPosTop();
	DrawSphere3D(pos1, radius_, 5, NORMAL_COLOR, NORMAL_COLOR, false);

	// 下の球体
	VECTOR pos2 = GetPosDown();
	DrawSphere3D(pos2, radius_, 5, NORMAL_COLOR, NORMAL_COLOR, false);

	VECTOR dir;
	VECTOR s;
	VECTOR e;

	// 球体を繋ぐ線(X+)
	dir = quaRot_.PosAxis(AsoUtility::DIR_R);
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, NORMAL_COLOR);

	// 球体を繋ぐ線(X-)
	dir = quaRot_.PosAxis(AsoUtility::DIR_L);
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, NORMAL_COLOR);

	// 球体を繋ぐ線(Z+)
	dir = quaRot_.PosAxis(AsoUtility::DIR_F);
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, NORMAL_COLOR);

	// 球体を繋ぐ線(Z-)
	dir = quaRot_.PosAxis(AsoUtility::DIR_B);
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, NORMAL_COLOR);

	// カプセルの中心
	DrawSphere3D(GetCenter(), 5.0f, 10, NORMAL_COLOR, NORMAL_COLOR, true);
}

const bool Capsule::IsHit(Geometry& _geometry)
{
	return _geometry.IsHit(*this);
}

const bool Capsule::IsHit(Model& _model)
{
	return _model.IsHit(*this);
}

const bool Capsule::IsHit(Cube& _cube)
{
	return _cube.IsHit(*this);
}

const bool Capsule::IsHit(Sphere& _sphere)
{
	return _sphere.IsHit(*this);
}

const bool Capsule::IsHit(Capsule& _capsule)
{
	VECTOR d1 = VSub(GetPosDown(), GetPosTop());					// 線分1の方向ベクトル
	VECTOR d2 = VSub(_capsule.GetPosDown(), _capsule.GetPosTop());	// 線分2の方向ベクトル
	VECTOR r = VSub(GetPosTop(), _capsule.GetPosTop());

	float a = VDot(d1, d1); // d1・d1
	float e = VDot(d2, d2); // d2・d2
	float f = VDot(d2, r);

	float s, t;

	float c = VDot(d1, r);
	float b = VDot(d1, d2);
	float denom = a * e - b * b;

	if (denom != 0.0f)
	{
		s = (b * f - c * e) / denom;
		s = AsoUtility::ClampFloat(s, 0.0f, 1.0f);
	}
	else
	{
		s = 0.0f;
	}

	t = (b * s + f) / e;
	if (t < 0.0f)
	{
		t = 0.0f;
		s = AsoUtility::ClampFloat(-c / a, 0.0f, 1.0f);
	}
	else if (t > 1.0f)
	{
		t = 1.0f;
		s = AsoUtility::ClampFloat((b - c) / a, 0.0f, 1.0f);
	}

	VECTOR c1 = VAdd(GetPosTop(), VScale(d1, s));
	VECTOR c2 = VAdd(_capsule.GetPosTop(), VScale(d2, t));
	float distance = VSize(VSub(c1, c2));

	//衝突したか
	return distance <= (GetRadius() + _capsule.GetRadius());
}

const bool Capsule::IsHit(Line& _line)
{
	VECTOR u = VSub(_line.GetPosPoint2(), _line.GetPosPoint1());
	VECTOR v = VSub(GetPosDown(), GetPosTop());
	VECTOR w = VSub(_line.GetPosPoint1(), GetPosTop());

	float a = VDot(u, u);
	float b = VDot(u, v);
	float c = VDot(v, v);
	float d = VDot(u, w);
	float e = VDot(v, w);

	float denom = a * c - b * b;
	float s = 0.0f, t = 0.0f;

	if (denom != 0.0f) {
		s = AsoUtility::ClampFloat((b * e - c * d) / denom, 0.0f, 1.0f);
	}

	t = (b * s + e) / c;
	t = AsoUtility::ClampFloat(t, 0.0f, 1.0f);

	s = (b * t - d) / a;
	s = AsoUtility::ClampFloat(s, 0.0f, 1.0f);

	VECTOR closest1 = VAdd(_line.GetPosPoint1(), VScale(u, s));
	VECTOR closest2 = VAdd(GetPosTop(), VScale(v, t));
	VECTOR diff = VSub(closest1, closest2);

	float distance = sqrt(VDot(diff, diff));

	return distance <= GetRadius();
}

void Capsule::HitAfter(void)
{
	if (hitInfo_.HitNum > 0 && hitInfo_.Dim != nullptr)
	{
		//当たり判定情報の解放
		MV1CollResultPolyDimTerminate(hitInfo_);

		//再初期化
		std::memset(&hitInfo_, 0, sizeof(hitInfo_));
	}
}

const VECTOR Capsule::GetCenter(void) const
{
	VECTOR top = GetPosTop();
	VECTOR down = GetPosDown();

	VECTOR diff = VSub(top, down);
	return VAdd(down, VScale(diff, 0.5f));
}