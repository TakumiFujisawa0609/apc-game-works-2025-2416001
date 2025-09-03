#include <DxLib.h>
#include <wingdi.h>
#include <EffekseerForDXLib.h>
#include <cassert>
#include "Resource.h"

Resource::Resource(void)
{
	type_ = TYPE::NONE;
	path_ = "";

	numX_ = -1;
	numY_ = -1;
	sizeX_ = -1;
	sizeY_ = -1;

	handleId_ = -1;
	handleIds_ = nullptr;
}

Resource::Resource(TYPE type, const std::string& path)
{
	type_ = type;
	path_ = path;

	numX_ = -1;
	numY_ = -1;
	sizeX_ = -1;
	sizeY_ = -1;

	handleId_ = -1;
	handleIds_ = nullptr;
}

Resource::Resource(TYPE type, const std::string& path, int numX, int numY, int sizeX, int sizeY)
{
	type_ = type;
	path_ = path;
	numX_ = numX;
	numY_ = numY;
	sizeX_ = sizeX;
	sizeY_ = sizeY;

	handleId_ = -1;
	handleIds_ = nullptr;
}

void Resource::Load(void)
{

	switch (type_)
	{
	case Resource::TYPE::IMG:
		// 画像
		handleId_ = LoadGraph(path_.c_str());
		break;

	case Resource::TYPE::IMGS:
		// 複数画像
		handleIds_ = new int[numX_ * numY_];
		LoadDivGraph(
			path_.c_str(),
			numX_ * numY_,
			numX_, numY_,
			sizeX_, sizeY_,
			&handleIds_[0]);
		break;

	case Resource::TYPE::MODEL:
		// モデル
		handleId_ = MV1LoadModel(path_.c_str());
		break;

	case Resource::TYPE::EFFEKSEER:
		//エフェクト
		handleId_ = LoadEffekseerEffect(path_.c_str());
		break;

	case Resource::TYPE::FONT:
		//フォント
		handleId_ = AddFontResourceEx(path_.c_str(), FR_PRIVATE, NULL);
		break;

	case Resource::TYPE::VERTEX_SHADER:
		//頂点シェーダー
		handleId_ = LoadVertexShader(path_.c_str());
		break;

	case Resource::TYPE::PIXEL_SHADER:
		//ピクセルシェーダー
		handleId_ = LoadPixelShader(path_.c_str());
		break;

	case Resource::TYPE::SOUND:
		//音声
		handleId_ = LoadSoundMem(path_.c_str());
		break;
	}

	//リソースが読み込めたか確認
	if (type_ != Resource::TYPE::IMGS) {
		assert(handleId_ != -1);
	}
	else {
		for (int i = 0; i < numX_ * numY_; i++) {
			assert(handleIds_[i] != -1); // 各スプライトの読み込みが成功してるか確認
		}
	}
	

}

void Resource::Release(void)
{

	switch (type_)
	{
	case Resource::TYPE::IMG:
		DeleteGraph(handleId_);
		break;

	case Resource::TYPE::IMGS:
	{
		int num = numX_ * numY_;
		for (int i = 0; i < num; i++)
		{
			DeleteGraph(handleIds_[i]);
		}
		delete[] handleIds_;
	}
		break;

	case Resource::TYPE::MODEL:
	{
		MV1DeleteModel(handleId_);
		auto ids = duplicateModelIds_;
		for (auto id : ids)
		{
			MV1DeleteModel(id);
		}
	}
		break;

	case Resource::TYPE::EFFEKSEER:
		DeleteEffekseerEffect(handleId_);
		break;

	case Resource::TYPE::FONT:
		RemoveFontResourceEx(path_.c_str(), FR_PRIVATE, NULL);
		break;

	case Resource::TYPE::VERTEX_SHADER:
		DeleteShader(handleId_);
		break;

	case Resource::TYPE::PIXEL_SHADER:
		DeleteShader(handleId_);
		break;

	case Resource::TYPE::SOUND:
		DeleteSoundMem(handleId_);
		break;
	}

}

void Resource::CopyHandle(int* imgs)
{

	if (handleIds_ == nullptr)
	{
		return;
	}

	int num = numX_ * numY_;
	for (int i = 0; i < num; i++)
	{
		imgs[i] = handleIds_[i];
	}

}
