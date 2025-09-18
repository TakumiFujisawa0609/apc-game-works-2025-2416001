#include "WeponBeam.h"

WeponBeam::WeponBeam(void)
{
}

WeponBeam::~WeponBeam(void)
{
}

void WeponBeam::Update(void)
{
	if (!isAlive_)
	{
		return;
	}

	// ˆÚ“®ˆ—
	Move();
}

void WeponBeam::Draw(void)
{
	if (!isAlive_)
	{
		return;
	}

	DrawCapsule3D(
		trans_.pos,
		VGet(trans_.pos.x, trans_.pos.y, trans_.pos.z + bemelong_),
		2.5f,
		8,
		GetColor(0, 255, 0),
		GetColor(255, 255, 255),
		FALSE);
}

void WeponBeam::Release(void)
{
}

void WeponBeam::Use(VECTOR pos, VECTOR dir)
{
	trans_.pos = pos;
	trans_.pos.y += 150.0f;
	moveDir_ = dir;
	isAlive_ = true;
	bemelong_ = 0.0f;
}

void WeponBeam::Load(void)
{
}

void WeponBeam::SetParam(void)
{
	speed_ = 200.0f;
}

void WeponBeam::Move(void)
{
	if (bemelong_ > MAX_BEAM_LENGTH)
	{
		bemelong_ = MAX_BEAM_LENGTH;
		// ˆÚ“®ˆ—(ˆê•û•ûŒü)
		trans_.pos = VAdd(trans_.pos, VScale(moveDir_, speed_));
	}
	else
	{
		bemelong_ += speed_;
	}
}
