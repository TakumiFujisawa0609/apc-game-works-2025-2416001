#include "../../Common/Vector2.h"
#include <string>

class HpBer
{
public:

	/// <summary>
	/// デフォルトコンストラクタ
	/// </summary>
	HpBer(void);

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="maxHP">最大HP</param>
	/// <param name="pos">HPバー位置</param>
	/// <param name="size">HPバーサイズ</param>
	/// <param name="color">HPバー色</param>
	/// <param name="backColor">HPバー背景色</param>
	/// <param name="handle">画像ハンドル</param>
	HpBer(int maxHP, const Vector2& pos, const Vector2& size,
		  unsigned int color, unsigned int backColor,
		  int handle = -1);


	/// <summary>
	/// デストラクタ
	/// </summary>
	~HpBer(void) = default;

	// HP情報取得(テキストあり)
	void SetHpBerBack(const Vector2& pos, const Vector2& size, unsigned int color,
					  const char* text, const Vector2& textPos, unsigned int textColor);

	/// <summary>
	/// HP描画処理
	/// </summary>
	void Draw(void);


	/// <summary>
	/// HPバー位置 割り当て
	/// </summary>
	void SetPos(const Vector2& pos);

	/// <summary>
	/// HP情報 割り当て
	/// </summary>
	void SetHp(int HP);

	/// <summary>
	/// HPバー位置取得
	/// </summary>
	const Vector2& GetPos(void)const;

	/// <summary>
	/// HP値取得
	/// </summary>
	int GetCurHp(void)const;

	/// <summary>
	/// HPバーサイズ取得
	/// </summary>
	const Vector2& GetSize(void)const;


private:

	struct HP
	{
		// 画像ハンドル
		int handle;

		// HPバーサイズ
		Vector2 size;

		// 位置
		Vector2 pos;

		// 色
		unsigned int color;

		// 背景色
		unsigned int backColor;

		// 現在HP
		unsigned int curHp;

		// 最大HP
		unsigned int maxHp;
	};
	
	struct BACK_BER
	{
		// サイズ
		Vector2 size;

		// 位置
		Vector2 pos;

		// 色
		unsigned int color;

		// テキスト
		std::string text;

		// テキスト位置
		Vector2 posText;

		// テキスト色
		unsigned int colorText;
	};

	// HPバー
	HP hpBer_;

	// HPバーの背景
	BACK_BER backBer_;
};