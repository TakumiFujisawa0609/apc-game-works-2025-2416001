#include<algorithm>
#include "../../../Common/Quaternion.h"
#include "../../../Utility/AsoUtility.h"
#include"Model.h"
#include"Cube.h"
#include"Capsule.h"
#include"Line.h"
#include"Sphere.h"

//***************************************************
//球
//***************************************************

Sphere::Sphere(const VECTOR& _pos, const float _radius) : Geometry(_pos,Quaternion()),
	radius_(_radius)
{
	std::memset(&hitInfo_, 0, sizeof(hitInfo_));
}

Sphere::Sphere(const Sphere& _copyBase, const VECTOR& _pos) : Geometry(_pos, Quaternion())
{
	radius_ = _copyBase.GetRadius();
	std::memset(&hitInfo_, 0, sizeof(hitInfo_));
}

Sphere::~Sphere(void)
{
}

void Sphere::Draw(void)
{
	DrawSphere3D(pos_, radius_, 10, NORMAL_COLOR, NORMAL_COLOR, false);
}

const bool Sphere::IsHit(Geometry& _geometry)
{
	return _geometry.IsHit(*this);
}

const bool Sphere::IsHit(Model& _model)
{
	//球体とモデルの当たり判定
	auto col = MV1CollCheck_Sphere(_model.GetParentModel(), -1, GetColPos(), GetRadius());

	return col.HitNum >= 1;
}

const bool Sphere::IsHit(Cube& _cube)
{
	// OBBの中心（ローカルmin/maxの中点 + ワールド位置）
	VECTOR obbCenter = VAdd(_cube.GetColPos(), VScale(VAdd(_cube.GetObb().vMin, _cube.GetObb().vMax), 0.5f));

	// 球の中心との相対ベクトル
	VECTOR d = VSub(pos_, obbCenter);

	// OBBの半サイズ（各軸方向の長さの半分）
	VECTOR halfSize = VScale(VSub(_cube.GetObb().vMax, _cube.GetObb().vMin), 0.5f);

	// 最近接点をOBBの中心からスタート
	VECTOR closest = obbCenter;

	// X軸方向
	{
		float dist = VDot(d, _cube.GetObb().axis[0]);
		dist = std::max(-halfSize.x, std::min(dist, halfSize.x));
		closest = VAdd(closest, VScale(_cube.GetObb().axis[0], dist));
	}

	// Y軸方向
	{
		float dist = VDot(d, _cube.GetObb().axis[1]);
		dist = std::max(-halfSize.y, std::min(dist, halfSize.y));
		closest = VAdd(closest, VScale(_cube.GetObb().axis[1], dist));
	}

	// Z軸方向
	{
		float dist = VDot(d, _cube.GetObb().axis[2]);
		dist = std::max(-halfSize.z, std::min(dist, halfSize.z));
		closest = VAdd(closest, VScale(_cube.GetObb().axis[2], dist));
	}

	// 球と最近接点の距離²を計算
	float distSq = AsoUtility::SqrMagnitudeF(VSub(closest, pos_));

	//半径と比較
	return distSq <= (radius_ * radius_);
}

const bool Sphere::IsHit(Sphere& _sphere)
{
	// 球体同士の衝突判定
	bool ret = false;

	// お互いの半径の合計
	float radius = GetRadius() + _sphere.GetRadius();

	// 座標の差からお互いの距離を取る
	VECTOR diff = VSub(_sphere.GetColPos(), GetColPos());

	// 三平方の定理で比較(SqrMagnitudeと同じ)
	float dis = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);
	if (dis < (radius * radius))
	{
		ret = true;
	}

	return ret;
}

const bool Sphere::IsHit(Capsule& _capsule)
{
	//球体とカプセルの当たり判定
	bool ret = false;

	// カプセル球体の中心を繋ぐベクトル
	VECTOR cap1to2 = VSub(_capsule.GetPosDown(), _capsule.GetPosTop());

	// ベクトルを正規化
	VECTOR cap1to2ENor = VNorm(cap1to2);

	// カプセル繋ぎの単位ベクトルと、
	// そのベクトル元から球体へのベクトルの内積を取る
	float dot = VDot(cap1to2ENor, VSub(GetColPos(), _capsule.GetPosTop()));

	// 内積で求めた射影距離を使って、カプセル繋ぎ上の座標を取る
	VECTOR capRidePos = VAdd(_capsule.GetPosTop(), VScale(cap1to2ENor, dot));

	// カプセル繋ぎのベクトルの長さを取る
	float len = sqrt((cap1to2.x * cap1to2.x) + (cap1to2.y * cap1to2.y) + (cap1to2.z * cap1to2.z));

	// 球体がカプセル繋ぎ上にいるか判別するため、比率を取る
	float rate = dot / len;

	VECTOR centerPos = { 0.0f,0.0f,0.0f };

	// 球体の位置が３エリアに分割されたカプセル形状のどこにいるか判別
	if (rate > 0.0f && rate <= 1.0f)
	{
		// ①球体がカプセル繋ぎ上にいる
		centerPos = VAdd(_capsule.GetPosTop(), VScale(cap1to2ENor, dot));
	}
	else if (rate > 1.0f)
	{
		// ②球体がカプセルの終点側にいる
		centerPos = _capsule.GetPosDown();
	}
	else if (rate < 0.0f)
	{
		// ③球体がカプセルの始点側にいる
		centerPos = _capsule.GetPosTop();
	}
	else
	{
		// ここにきてはいけない
		return false;
	}

	// お互いの半径の合計
	float radius = GetRadius() + _capsule.GetRadius();

	// 座標の差からお互いの距離を取る
	VECTOR diff = VSub(centerPos, GetColPos());

	// 三平方の定理で比較(SqrMagnitudeと同じ)
	float dis = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);
	if (dis < (radius * radius))
	{
		ret = true;
	}

	return ret;
}

const bool Sphere::IsHit(Line& _line)
{
	//線のベクトル
	VECTOR d = VSub(_line.GetPosPoint2(), _line.GetPosPoint1());

	//線の先端から球体の中心まで
	VECTOR m = VSub(GetColPos(), _line.GetPosPoint1());

	float t = VDot(m, d) / VSquareSize(d);
	t = std::max<float>(0.0f, std::min<float>(1.0f, t));  // 線分内に制限

	VECTOR closestPoint = VAdd(_line.GetPosPoint1(), VScale(d, t));
	VECTOR diff = VSub(closestPoint, GetColPos());

	return VSquareSize(diff) <= std::pow(GetRadius(), 2.0);
}

void Sphere::HitAfter(void)
{
	if (hitInfo_.HitNum > 0 && hitInfo_.Dim != nullptr)
	{
		//当たり判定情報の解放
		MV1CollResultPolyDimTerminate(hitInfo_);

		//再初期化
		std::memset(&hitInfo_, 0, sizeof(hitInfo_));
	}
}
