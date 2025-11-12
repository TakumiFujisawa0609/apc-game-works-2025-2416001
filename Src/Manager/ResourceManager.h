#pragma once
#include <map>
#include <string>
#include "Resource.h"
#include<memory>

class ResourceManager
{

public:
	//インスタンス生成
	// インスタンスを明示的に生成
	static void CreateInstance(void);

	// インスタンスの取得
	static ResourceManager& GetInstance(void);

	/// <summary>
	/// リソース名
	/// </summary>
	enum class SRC
	{
		PLAYR,
		ENEMY_GEORGE,
		ENEMY_LEELA,
		ENEMY_MIKE,
		ENEMY_STAN, 

		//武器モデル
	    WEPON
	};

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name=""></param>
	void Init(void);

	/// <summary>
	/// 解放(シーン切替時に一旦解放)
	/// </summary>
	/// <param name=""></param>
	void SceneChangeRelease(void);

	/// <summary>
	/// リソースの完全解放
	/// </summary>
	/// <param name=""></param>
	void Release(void);

	/// <summary>
	/// リソースのロード
	/// </summary>
	/// <param name="src">読み込むリソース</param>
	/// <returns>リソース</returns>
	const Resource& Load(SRC src);

	/// <summary>
	/// リソースの複製ロード(モデル用)
	/// </summary>
	/// <param name="src">複製したいリソース</param>
	/// <returns>複製したリソース</returns>
	int LoadModelDuplicate(SRC src);

private:
	//ナンバー画像分割数X
	static constexpr int NUMBERS_DIV_X = 5;
	//ナンバー画像分割数Y
	static constexpr int NUMBERS_DIV_Y = 2;
	//ナンバーの各画像サイズ
	static constexpr int NUMBERS_SIZE = 128;

	// デフォルトコンストラクタをprivateにして、
// 外部から生成できない様にする
	ResourceManager(void);
	ResourceManager(const ResourceManager& manager) = default;
	~ResourceManager(void) = default;

	// リソース管理の対象
	std::map<SRC, std::unique_ptr<Resource>> resourcesMap_;

	// 読み込み済みリソース
	std::map<SRC, Resource&> loadedMap_;

	Resource dummy_;

	// シングルトン用インスタンス
	static ResourceManager* instance_;

	// 内部ロード
	Resource& _Load(SRC src);

};
