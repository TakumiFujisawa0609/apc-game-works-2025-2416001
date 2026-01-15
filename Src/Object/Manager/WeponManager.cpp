#include "../Wepon/WeponBeam.h"
#include "../Wepon/WeponMissile.h"
#include "../Common/Collider/ColliderBase.h"
#include "WeponManager.h"

WeponManager::WeponManager(void)
{
}

WeponManager::~WeponManager(void)
{
}

void WeponManager::Init(void)
{
}

void WeponManager::Update(void)
{
	//•Ší•`‰æˆ—
	for (std::shared_ptr<WeponBase> wepons : wepons_)
	{
		wepons->Update();
	}
}

void WeponManager::Draw(void)
{
	//•Ší•`‰æˆ—
	for (std::shared_ptr<WeponBase> wepons : wepons_)
	{
		wepons->Draw();
	}
}

void WeponManager::Release(void)
{
	//•Ší‰ğ•úˆ—
	for (std::shared_ptr<WeponBase> wepons : wepons_)
	{
		wepons->Release();
	}
}

void WeponManager::ChangeWepon(
	WeponBase::WEPON_TYPE type, ColliderBase::TAG tag, VECTOR pos, VECTOR dir, int weponCnt, VECTOR targetPos) {

	type_ = type;

	std::shared_ptr<WeponBase> wepon = nullptr;

	for(int i = 0; i < weponCnt; i++)
	{
		switch (type_)
		{
		case WeponBase::WEPON_TYPE::NONE:
			break;
		case WeponBase::WEPON_TYPE::BEAM:
		{
			wepon = GetValidWepon(type_);
			wepon->Init(pos, dir, tag);
		}
		break;
		case WeponBase::WEPON_TYPE::MISSILE: {
			wepon = GetValidWepon(type_);

			// ƒ‰ƒ“ƒ_ƒ€‚ÈŠp“x‚Å‚Î‚ç‚¯‚³‚¹‚é
			int randAnglePow = 10000;
			float randomAngleY = dir.y + ((rand() % randAnglePow - randAnglePow / 2) / 100.0f);
			float randomAngleX = dir.x + ((rand() % randAnglePow - randAnglePow / 2) / 100.0f);
			float randomAngleZ = dir.z + ((rand() % randAnglePow - randAnglePow / 2) / 100.0f);
			VECTOR moveDir = VNorm(VGet(randomAngleX, randomAngleY, randomAngleZ));

			wepon->Init(pos, moveDir, targetPos, tag);
		}
		break;
		case WeponBase::WEPON_TYPE::SWORD:
			break;
		default:
			break;
		}
	}
}

void WeponManager::SetTargetPos(const VECTOR& targetPos)
{
	for (std::shared_ptr<WeponBase> wepons : wepons_)
	{
		wepons->SetTarget(targetPos);
	}
}

std::shared_ptr<WeponBase> WeponManager::GetValidWepon(WeponBase::WEPON_TYPE type)
{
	//size_t size = wepons_.size();
	//for (int i = 0; i < size; i++){
	//	// –¢g—p
	//	if (!wepons_[i]->isAlive_)
	//	{
	//		return wepons_[i];
	//	}
	//}

	std::shared_ptr<WeponBase> wepon = nullptr;
	if(type == WeponBase::WEPON_TYPE::BEAM){
		wepon = std::make_shared<WeponBeam>(type);
	}
	if (type == WeponBase::WEPON_TYPE::MISSILE) {
		wepon = std::make_shared<WeponMissile>(type);
	}

	wepons_.emplace_back(wepon);
	return wepon;
}
