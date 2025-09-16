#include "InputManager.h"
#include "DataBank.h"

DataBank* DataBank::instance_ = nullptr;

void DataBank::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new DataBank();
	}
	instance_->Init();
}

DataBank& DataBank::GetInstance(void)
{
    return *instance_;
}

void DataBank::Init(void)
{
}

void DataBank::Destroy(void)
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
}


DataBank::DataBank(void)
{
}

DataBank::~DataBank(void)
{
}
