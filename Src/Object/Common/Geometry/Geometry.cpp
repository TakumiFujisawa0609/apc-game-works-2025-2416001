#include "../../../Common/Quaternion.h"
#include "Geometry.h"

//***************************************************
//Šî–{
//***************************************************

Geometry::~Geometry(void)
{
}

Geometry::Geometry(const VECTOR& _pos, const Quaternion& _rot) : pos_(_pos), quaRot_(_rot)
{
}

const VECTOR Geometry::GetRotPos(const VECTOR& _localPos) const
{
	VECTOR localRotPos = quaRot_.PosAxis(_localPos);
	return VAdd(pos_, localRotPos);
}

void Geometry::HitAfter(void)
{
}
