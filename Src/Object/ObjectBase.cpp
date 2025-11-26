#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "Manager/CollisionManager.h"
#include "ObjectBase.h"

ObjectBase::ObjectBase(void)
	: resMng_(ResourceManager::GetInstance()),
	scnMng_(SceneManager::GetInstance()),
	inpMng_(InputManager::GetInstance())
{
}

ObjectBase::~ObjectBase(void)
{
}