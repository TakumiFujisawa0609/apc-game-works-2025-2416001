#include "WeponBeam.h"

WeponBeam::WeponBeam(void)
{
}

WeponBeam::~WeponBeam(void)
{
}

void WeponBeam::Draw(void)
{
	DrawCapsule3D(
		VGet(trans_.pos.x, trans_.pos.y + 150.0f, trans_.pos.z),
		VGet(trans_.pos.x, trans_.pos.y + 150.0f, trans_.pos.z + 300.0f),
		2.5f,
		8,
		GetColor(0, 255, 0),
		GetColor(255, 255, 255),
		FALSE);
}

void WeponBeam::Release(void)
{
}

void WeponBeam::Use(VECTOR pos)
{
	trans_.pos = pos;
}

void WeponBeam::Load(void)
{
}

void WeponBeam::SetParam(void)
{
}

void WeponBeam::Move(void)
{
}
