#include "../Wepon/WeponBase.h"
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
	WeponBase::WEPON_TYPE type, VECTOR pos, VECTOR dir, VECTOR targetPos)
{
	type_ = type;

	switch (type_)
	{
	case WeponBase::WEPON_TYPE::NONE:
		break;
	case WeponBase::WEPON_TYPE::BEAM:
	{
		std::shared_ptr<WeponBase> wepons =
			std::make_shared<WeponBeam>(WeponBase::WEPON_TYPE::BEAM);
		wepons->Init(pos, dir);
		wepons_.emplace_back(wepons);
	}
		break;
	case WeponBase::WEPON_TYPE::MISSILE:
	{
		std::shared_ptr<WeponBase> wepons =
			std::make_shared<WeponMissile>(WeponBase::WEPON_TYPE::MISSILE);
		wepons->Init(pos, dir,targetPos);
		wepons_.emplace_back(wepons);
	}
		break;
	case WeponBase::WEPON_TYPE::SWORD:
		break;
	default:
		break;
	}
}
