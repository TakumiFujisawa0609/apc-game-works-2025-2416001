#include<cassert>
#include"../../../Common/Quaternion.h"
#include"../../../Utility/AsoUtility.h"
#include"Model.h"
#include"Sphere.h"
#include"Capsule.h"
#include"Line.h"
#include"Cube.h"

//***************************************************
//箱
//***************************************************

Cube::Cube(const VECTOR& _pos, const Quaternion& _rot, const VECTOR _min, const VECTOR _max) : Geometry(_pos, _rot)
{
	obb_.vMin = _min;
	obb_.vMax = _max;

	UpdateObbAxis();
}

Cube::Cube(const VECTOR& _pos, const Quaternion& _rot, const VECTOR _halfSize) : Geometry(_pos, _rot)
{
	obb_.vMin = VScale(_halfSize, -1.0f);
	obb_.vMax = _halfSize;

	UpdateObbAxis();
}

Cube::Cube(const Cube& _copyBase, const VECTOR& _pos, const Quaternion& _rot) : Geometry(_pos, _rot),obb_(_copyBase.GetObb())
{
	UpdateObbAxis();
}

Cube::~Cube(void)
{

}

void Cube::Draw(void)
{
	VECTOR vertices[8];
	CalculateVertices(vertices);

	// 12本のエッジのインデックス
	static const int edges[12][2] = {
		{0,1},{0,2},{0,4}, {1,3},{1,5},
		{2,3},{2,6}, {3,7},
		{4,5},{4,6}, {5,7},{6,7}
	};

	for (int i = 0; i < 12; ++i)
	{
		DrawLine3D(vertices[edges[i][0]], vertices[edges[i][1]], NORMAL_COLOR);
	}
}

const bool Cube::IsHit(Geometry& _geometry)
{
	//回転の更新
	UpdateObbAxis();

	bool ret = _geometry.IsHit(*this);

	return ret;
}

const bool Cube::IsHit(Model& _model)
{
	return false;
}

const bool Cube::IsHit(Cube& _cube)
{
	// 各OBBの中心座標（ワールド空間）
	VECTOR centerA = VAdd(pos_, VScale(VAdd(obb_.vMin, obb_.vMax), 0.5f));
	VECTOR centerB = VAdd(_cube.GetColPos(), VScale(VAdd(_cube.GetObb().vMin, _cube.GetObb().vMax), 0.5f));

	// 2つの中心の差
	VECTOR t = VSub(centerB, centerA);

	// OBBの軸長（半サイズ）
	VECTOR halfA = VScale(VSub(obb_.vMax, obb_.vMin), 0.5f);
	VECTOR halfB = VScale(VSub(_cube.GetObb().vMax, _cube.GetObb().vMin), 0.5f);

	// 各軸を順にチェック（15軸）
	for (int i = 0; i < 3; ++i) {
		const VECTOR& axisA = obb_.axis[i];

		// 軸Aの投影量
		float ra = halfA.x * fabs(VDot(axisA, obb_.axis[0])) +
			halfA.y * fabs(VDot(axisA, obb_.axis[1])) +
			halfA.z * fabs(VDot(axisA, obb_.axis[2]));

		float rb = halfB.x * fabs(VDot(axisA, _cube.GetObb().axis[0])) +
			halfB.y * fabs(VDot(axisA, _cube.GetObb().axis[1])) +
			halfB.z * fabs(VDot(axisA, _cube.GetObb().axis[2]));

		if (fabs(VDot(t, axisA)) > ra + rb) return false;
	}

	for (int i = 0; i < 3; ++i) {
		const VECTOR& axisB = _cube.GetObb().axis[i];

		float ra = halfA.x * fabs(VDot(axisB, obb_.axis[0])) +
			halfA.y * fabs(VDot(axisB, obb_.axis[1])) +
			halfA.z * fabs(VDot(axisB, obb_.axis[2]));

		float rb = halfB.x * fabs(VDot(axisB, _cube.GetObb().axis[0])) +
			halfB.y * fabs(VDot(axisB, _cube.GetObb().axis[1])) +
			halfB.z * fabs(VDot(axisB, _cube.GetObb().axis[2]));

		if (fabs(VDot(t, axisB)) > ra + rb) return false;
	}

	// 外積軸
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			VECTOR axis = VCross(obb_.axis[i], _cube.GetObb().axis[j]);

			// 軸が0に近い（平行またはゼロベクトル） → 無視
			if (VSize(axis) < 0.0001f) continue;
			axis = VNorm(axis);

			float ra = halfA.x * fabs(VDot(axis, obb_.axis[0])) +
				halfA.y * fabs(VDot(axis, obb_.axis[1])) +
				halfA.z * fabs(VDot(axis, obb_.axis[2]));

			float rb = halfB.x * fabs(VDot(axis, _cube.GetObb().axis[0])) +
				halfB.y * fabs(VDot(axis, _cube.GetObb().axis[1])) +
				halfB.z * fabs(VDot(axis, _cube.GetObb().axis[2]));

			if (fabs(VDot(t, axis)) > ra + rb) return false;
		}
	}

	// すべての軸で重なっている → 衝突
	return true;
}

const bool Cube::IsHit(Sphere& _sphere)
{
	return _sphere.IsHit(*this);
}

const bool Cube::IsHit(Capsule& _capsule)
{
	// OBB のローカル中心
	VECTOR localCenter = VScale(VAdd(obb_.vMin, obb_.vMax), 0.5f);

	// OBB のワールド中心
	VECTOR worldCenter = VAdd(
		VAdd(
			VAdd(
				VScale(obb_.axis[0], localCenter.x),
				VScale(obb_.axis[1], localCenter.y)
			),
			VScale(obb_.axis[2], localCenter.z)
		),
		pos_
	);

	// カプセル線分をOBBのローカル空間に変換
	VECTOR rel1 = VSub(_capsule.GetPosTop(), worldCenter);
	VECTOR rel2 = VSub(_capsule.GetPosDown(), worldCenter);

	VECTOR local1 = {
		VDot(rel1, obb_.axis[0]),
		VDot(rel1, obb_.axis[1]),
		VDot(rel1, obb_.axis[2])
	};

	VECTOR local2 = {
		VDot(rel2, obb_.axis[0]),
		VDot(rel2, obb_.axis[1]),
		VDot(rel2, obb_.axis[2])
	};

	// スラブ法で最近接点を見つける
	// AABBとして処理する（OBBローカル空間内で）

	float distSq = ClosestSegmentAABB(local1, local2, obb_.vMin, obb_.vMax);

	return distSq <= (_capsule.GetRadius() * _capsule.GetRadius());
}

const bool Cube::IsHit(Line& _line)
{
	// ワールド空間の線分座標
	VECTOR p1 = _line.GetPosPoint1();
	VECTOR p2 = _line.GetPosPoint2();

	// OBB のローカル中心（Min/Maxの中点）
	VECTOR localCenter = VScale(VAdd(obb_.vMin, obb_.vMax), 0.5f);

	// OBB のワールド中心を計算：axisで回転して pos_ を加算
	VECTOR worldCenter = VAdd(
		VAdd(
			VAdd(
				VScale(obb_.axis[0], localCenter.x),
				VScale(obb_.axis[1], localCenter.y)
			),
			VScale(obb_.axis[2], localCenter.z)
		),
		pos_
	);

	// 線分をOBB空間に変換（ワールド中心 → ローカル）
	VECTOR rel1 = VSub(p1, worldCenter);
	VECTOR rel2 = VSub(p2, worldCenter);

	VECTOR local1 = {
		VDot(rel1, obb_.axis[0]),
		VDot(rel1, obb_.axis[1]),
		VDot(rel1, obb_.axis[2])
	};

	VECTOR local2 = {
		VDot(rel2, obb_.axis[0]),
		VDot(rel2, obb_.axis[1]),
		VDot(rel2, obb_.axis[2])
	};

	// スラブ法：線分が AABB（Min/Max）と交差するか
	VECTOR dir = VSub(local2, local1);
	float tmin = 0.0f, tmax = 1.0f;

	// 各軸 (x, y, z)
	for (int axis = 0; axis < 3; ++axis)
	{
		float start, delta, minB, maxB;

		if (axis == 0) {
			start = local1.x; delta = dir.x;
			minB = obb_.vMin.x; maxB = obb_.vMax.x;
		}
		else if (axis == 1) {
			start = local1.y; delta = dir.y;
			minB = obb_.vMin.y; maxB = obb_.vMax.y;
		}
		else {
			start = local1.z; delta = dir.z;
			minB = obb_.vMin.z; maxB = obb_.vMax.z;
		}

		if (fabs(delta) < 1e-6)
		{
			// 線が平行で AABB のこの面を貫通しない
			if (start < minB || start > maxB) return false;
		}
		else
		{
			float ood = 1.0f / delta;
			float t1 = (minB - start) * ood;
			float t2 = (maxB - start) * ood;

			if (t1 > t2) std::swap(t1, t2);

			if (t1 > tmin) tmin = t1;
			if (t2 < tmax) tmax = t2;

			if (tmin > tmax) return false;
		}
	}

	return true;
}

void Cube::SetHalfSize(const VECTOR& _halfSize)
{
	obb_.vMin = VScale(_halfSize, -1.0f);
	obb_.vMax = _halfSize;
}

void Cube::UpdateObbAxis(void) 
{
	MATRIX rotMat;
	rotMat = quaRot_.ToMatrix();

	obb_.axis[0] = VTransform(VGet(1, 0, 0), rotMat); // Right
	obb_.axis[1] = VTransform(VGet(0, 1, 0), rotMat); // Up
	obb_.axis[2] = VTransform(VGet(0, 0, 1), rotMat); // Forward
}

void Cube::CalculateVertices(VECTOR outVertices[8]) const
{
	MATRIX rotMat;
	rotMat = quaRot_.ToMatrix();

	int idx = 0;
	for (int x = 0; x <= 1; ++x)
	{
		for (int y = 0; y <= 1; ++y)
		{
			for (int z = 0; z <= 1; ++z)
			{
				VECTOR local;
				local.x = (x == 0) ? obb_.vMin.x : obb_.vMax.x;
				local.y = (y == 0) ? obb_.vMin.y : obb_.vMax.y;
				local.z = (z == 0) ? obb_.vMin.z : obb_.vMax.z;

				VECTOR world = VTransform(local, rotMat);
				world = VAdd(world, pos_);

				outVertices[idx++] = world;
			}
		}
	}
}

float Cube::ClosestSegmentAABB(const VECTOR& segA, const VECTOR& segB, const VECTOR& aabbMin, const VECTOR& aabbMax)
{
	// 線分とAABBの最短距離²を求める
	// → 各軸でクランプを行う

	float t = 0.0f;
	float minDistSq = FLT_MAX;

	// 線分上の点 P(t) = A + t*(B - A), 0 <= t <= 1
	const int steps = 10;
	for (int i = 0; i <= steps; ++i)
	{
		float ft = static_cast<float>(i) / steps;
		VECTOR point = VAdd(segA, VScale(VSub(segB, segA), ft));

		// AABB内の最近接点
		VECTOR clamped = {
			std::max(aabbMin.x, std::min(point.x, aabbMax.x)),
			std::max(aabbMin.y, std::min(point.y, aabbMax.y)),
			std::max(aabbMin.z, std::min(point.z, aabbMax.z))
		};

		float distSq = AsoUtility::SqrMagnitudeF(VSub(point, clamped));
		if (distSq < minDistSq)
		{
			minDistSq = distSq;
			t = ft;
		}
	}

	return minDistSq;
}
