#include "HpBer.h"
#include <string>
#include <DxLib.h>

HpBer::HpBer(void)
{
	/*　デフォルトコンストラクタ　*/

	// HP初期化
	hpBer_.handle = -1;
	hpBer_.size = {};
	hpBer_.pos  = {-999, -999};
	hpBer_.color = hpBer_.backColor = 0;
	hpBer_.curHp = hpBer_.maxHp = 0;

	// HPバー初期化
	backBer_.size  = {};
	backBer_.pos   = {-999, -999};
	backBer_.color = 0;

	// テキスト初期化
	backBer_.text = "";
	backBer_.posText = {};
	backBer_.colorText = 0;
}

HpBer::HpBer(int maxHP, const Vector2& pos, const Vector2& size,
			 unsigned int color, unsigned int backColor, int handle)
{
	// HP初期化
	hpBer_.curHp = hpBer_.maxHp = maxHP;
	hpBer_.pos  = pos;
	hpBer_.size = size;
	hpBer_.color = color;
	hpBer_.backColor = backColor;
	hpBer_.handle = handle;
}


void HpBer::SetHpBerBack(const Vector2& pos, const Vector2& size, unsigned int color, const char* text, const Vector2& textPos, unsigned int textColor)
{
	/*　HPバー情報初期化処理　*/

	// 位置初期化
	backBer_.pos = pos;

	// サイズ初期化
	backBer_.size = size;

	// 背景色
	backBer_.color = color;

	// テキスト内容
	backBer_.text = text;

	// テキスト横位置
	backBer_.posText = textPos;

	// テキスト色
	backBer_.colorText = textColor;
}


/*　現在HP描画処理　*/
void HpBer::Draw(void)
{
	// 現在のHP割合
	float cur = static_cast<float>(hpBer_.curHp);
	float max = static_cast<float>(hpBer_.maxHp);
	float rateHP = (cur / max);	

	// 現在HP 横サイズ
	rateHP *= static_cast<float>(hpBer_.size.x);
	int hpSizeX = static_cast<int>(rateHP);

	if (hpBer_.handle != -1)
	{
		/*　HPバー(現在HP含め)描画処理　*/

		if (backBer_.pos.x != -999.9f && backBer_.pos.y != -999.9f)
		{
			// HPバー 描画
			DrawBox(backBer_.pos.x, backBer_.pos.y,
					backBer_.pos.x + backBer_.size.x,
					backBer_.pos.y + backBer_.size.y,
					backBer_.color, true);


			// 名前描画 
			DrawString(backBer_.posText.x, backBer_.posText.y,
					   backBer_.text.c_str(), backBer_.colorText);
		}

		// 最大HP描画
		DrawRectGraph(hpBer_.pos.x, hpBer_.pos.y, 0, 0,
					  hpBer_.size.x, hpBer_.size.y,
					  hpBer_.handle, true);
		
		// 現在HP描画
		DrawRectGraph(hpBer_.pos.x, hpBer_.pos.y, 0, 0,
					  hpSizeX, hpBer_.size.y,
					  hpBer_.handle, true);
	}

	else
	{
		if (backBer_.color != 0)
		{
			/*
			// 背景バー 描画
			DrawBox(backBer_.pos.x, backBer_.pos.y,
					backBer_.pos.x + backBer_.size.x,
					backBer_.pos.y + backBer_.size.y,
					backBer_.color, true);


			// 名前描画 
			DrawString(backBer_.posText.x, backBer_.posText.y,
					   backBer_.text.c_str(), backBer_.colorText);*/
		}

		// 最大HP 描画
		DrawBox(hpBer_.pos.x, hpBer_.pos.y,
				(hpBer_.pos.x + hpBer_.size.x),
				(hpBer_.pos.y + hpBer_.size.y),
				hpBer_.backColor, true);

		// 現在HP 描画
		DrawBox(hpBer_.pos.x, hpBer_.pos.y,
				(hpBer_.pos.x + hpSizeX),
				(hpBer_.pos.y + hpBer_.size.y),
				hpBer_.color, true);
	}
}

void HpBer::SetPos(const Vector2& pos)
{
	/*　位置情報 割り当て　*/
	hpBer_.pos = pos;
}

void HpBer::SetHp(int hp)
{
	/*　HP情報 割り当て　*/
	hpBer_.curHp = hp;
}

const Vector2& HpBer::GetPos(void)const
{
	return hpBer_.pos;
}

int HpBer::GetCurHp(void)const
{
	return hpBer_.curHp;
}

const Vector2& HpBer::GetSize(void)const
{
	return hpBer_.size;
}
