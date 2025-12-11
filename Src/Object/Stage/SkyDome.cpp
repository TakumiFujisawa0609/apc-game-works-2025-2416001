#include "../../Manager/ResourceManager.h"
#include "../Common/Transform.h"
#include "../../Utility/AsoUtility.h"
#include "SkyDome.h"

SkyDome::SkyDome(void)
{
}

SkyDome::~SkyDome(void)
{
}

void SkyDome::Update(void)
{
	trans_.quaRot = Quaternion::Mult(trans_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(-0.01f), AsoUtility::AXIS_Y));

	trans_.Update();
}

void SkyDome::Draw(void)
{
	SetUseLighting(FALSE);
	MV1DrawModel(trans_.modelId);
	SetUseLighting(TRUE);
}

void SkyDome::InitLoad(void)
{
	trans_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::SKY_DOME));
}

void SkyDome::InitTransform(void)
{
	trans_.scl = { 150.0f, 150.0f, 150.0f };
	trans_.quaRot = Quaternion::Identity();
	trans_.quaRotLocal = Quaternion::Identity();
	trans_.quaRotLocal =
		Quaternion::Mult(trans_.quaRotLocal,
			Quaternion::AngleAxis(AsoUtility::Deg2RadF(180.0f), AsoUtility::AXIS_Y));
	trans_.pos = { 0.0f, 0.0f, 0.0f };
	trans_.Update();
}

void SkyDome::InitCollider(void)
{
}

void SkyDome::InitAnimation(void)
{
}

void SkyDome::InitPost(void)
{
	// Zバッファ無効(突き抜け対策)
	MV1SetUseZBuffer(trans_.modelId, false);
	MV1SetWriteZBuffer(trans_.modelId, false);
}
