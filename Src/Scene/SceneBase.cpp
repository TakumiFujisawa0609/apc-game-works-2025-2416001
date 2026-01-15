#include "../Manager/ResourceManager.h"
#include "../Manager/FontManager.h"
#include "SceneBase.h"

SceneBase::SceneBase(void):
	resMng_(ResourceManager::GetInstance()),
	fontMng_(FontManager::GetInstance())
{
}

SceneBase::~SceneBase(void)
{
}
