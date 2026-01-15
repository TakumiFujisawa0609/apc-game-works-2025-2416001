#pragma once
#include <string>
#include <map>

class FontManager
{
public:
	// 明示的にインステンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static FontManager& GetInstance(void);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name=""></param>
	void Init(void);

	/// <summary>
	/// 解放処理
	/// </summary>
	/// <param name=""></param>
	void Destroy(void);

	/// <summary>
	/// オリジナルフォントの生成
	/// </summary>
	/// <param name="fontName">フォント名前</param>
	/// <param name="size">サイズ</param>
	/// <param name="thickness">太さ</param>
	/// <returns>生成できた場合true,失敗した場合false</returns>
	int CreateMyFont(const std::wstring& fontName, const int size, const int thickness);

	/// <summary>
	/// 解放処理
	/// </summary>
	void Release();

	/// <summary>
	/// 生成したフォントを解放する
	/// </summary>
	void SceneChangeRelease();

private:

	// フォント設定情報をまとめた構造体
	struct FontKey
	{
		std::wstring fontName;
		int size;
		int thickness;

		// map用の比較演算子
		bool operator<(const FontKey& other) const noexcept
		{
			if (fontName != other.fontName) return fontName < other.fontName;
			if (size != other.size) return size < other.size;
			return thickness < other.thickness;
		}
	};

	//生成したフォントを管理するマップ
	std::map<FontKey, int> fontHandleMap_;

	// 静的インスタンス
	static FontManager* instance_;

	/// <summary>
	/// コンストラクタ
	/// </summary>
	FontManager(void) = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~FontManager(void) = default;
};

