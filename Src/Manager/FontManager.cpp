#include <DxLib.h>
#include <cassert>
#include "../Application.h"
#include "FontManager.h"

FontManager* FontManager::instance_ = nullptr;

void FontManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new FontManager();
	}
	instance_->Init();
}

FontManager& FontManager::GetInstance(void)
{
	return *instance_;
}

void FontManager::Init(void)
{
}

void FontManager::Destroy(void)
{
	if (instance_)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

int FontManager::CreateMyFont(const std::wstring& fontName, const int size, const int thickness)
{
	FontKey key{ fontName, size, thickness };  // キー

	auto it = fontHandleMap_.find(key);        // 既存検索
	if (it != fontHandleMap_.end())
	{
		return it->second;  // 既存を返す
	}

	// 生成
	std::string fontNameA(fontName.begin(), fontName.end());
	int handle = CreateFontToHandle(
		fontNameA.c_str(),  // フォント名
		size,               // サイズ
		thickness);         // 太さ

	assert(handle != -1 && L"フォント生成に失敗しました");

	fontHandleMap_[key] = handle;                    // 登録
	return handle;                                   // 生成結果
}

void FontManager::Release()
{
	SceneChangeRelease();
}

void FontManager::SceneChangeRelease()
{
	//生成したフォントを解放
	for (const auto& font : fontHandleMap_)
	{
		DeleteFontToHandle(font.second);
	}

	fontHandleMap_.clear();
}
