#include "../Wepon/WeponBeam.h"
#include "../Wepon/WeponMissile.h"
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
	WeponBase::WEPON_TYPE type, VECTOR pos, VECTOR dir, int weponCnt, VECTOR targetPos) {

	type_ = type;
	std::shared_ptr<WeponBase> wepons = nullptr;

	for(int i = 0; i < weponCnt; i++)
	{
		switch (type_)
		{
		case WeponBase::WEPON_TYPE::NONE:
			break;
		case WeponBase::WEPON_TYPE::BEAM:
		{
			wepons = std::make_shared<WeponBeam>(WeponBase::WEPON_TYPE::BEAM);
			wepons->Init(pos, dir);
			wepons_.emplace_back(wepons);
		}
		break;
		case WeponBase::WEPON_TYPE::MISSILE: {
			wepons = std::make_shared<WeponMissile>(WeponBase::WEPON_TYPE::MISSILE);

			// ƒ‰ƒ“ƒ_ƒ€‚ÈŠp“x‚Å‚Î‚ç‚¯‚³‚¹‚é
			int randAnglePow = 10000;
			float randomAngleY = dir.y + ((rand() % randAnglePow - randAnglePow / 2) / 100.0f);
			float randomAngleX = dir.x + ((rand() % randAnglePow - randAnglePow / 2) / 100.0f);
			float randomAngleZ = dir.z + ((rand() % randAnglePow - randAnglePow / 2) / 100.0f);
			VECTOR moveDir = VNorm(VGet(randomAngleX, randomAngleY, randomAngleZ));

			wepons->Init(pos, moveDir, targetPos);
			wepons_.emplace_back(wepons);
		}
										   break;
		case WeponBase::WEPON_TYPE::SWORD:
			break;
		default:
			break;
		}
	}
}
