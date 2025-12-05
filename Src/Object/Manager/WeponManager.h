#pragma once
#include <vector>
#include <memory>
#include <DxLib.h>
#include "../../Utility/AsoUtility.h"
#include "../Wepon/WeponBase.h"

class WeponBase;
class Collider;

class WeponManager
{
public:

	// コンストラクタ
	WeponManager(void);
	// デストラクタ
	~WeponManager(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	//武器の情報を取得
	const std::vector<std::shared_ptr<WeponBase>>& GetWepons(void) const { return wepons_; }

	//武器の切り替え処理
	void ChangeWepon(WeponBase::WEPON_TYPE type, Collider::TAG tag, VECTOR pos, VECTOR dir, int weponCnt, VECTOR targetPos = AsoUtility::VECTOR_ZERO);

private:

	//武器情報
	std::vector<std::shared_ptr<WeponBase>> wepons_;

	//武器種類
	WeponBase::WEPON_TYPE type_;

	//有効な武器を取得する
	std::shared_ptr<WeponBase>GetValidWepon(WeponBase::WEPON_TYPE type);
};

