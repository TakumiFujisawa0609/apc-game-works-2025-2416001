#pragma once
#include<vector>
#include <string>
#include "../ObjectBase.h"
#include "../Common/Transform.h"
class Stage :
    public ObjectBase
{
public:

	// 衝突判定種別
	enum class COLLIDER_TYPE
	{
		MODEL = 0,
		MAX,
	};

	// コンストラクタ
	Stage(void);

	// デストラクタ
	~Stage(void);

	// 更新
	void Update(void)override;

	//描画処理
	void Draw(void)override;

protected:

	// リソースロード
	void InitLoad(void)override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void)override;

	// 衝突判定の初期化
	void InitCollider(void)override;

	// アニメーションの初期化
	void InitAnimation(void)override;

	// 初期化後の個別処理
	void InitPost(void)override;

private:

	// 除外フレーム名称
	const std::vector<std::string> EXCLUDE_FRAME_NAMES = { "Mush", "Grass", "Tree", "Stump", "Rock"};

	// 対象フレーム
	const std::vector<std::string> TARGET_FRAME_NAMES = { "Ground", };

	std::vector<int> frameOpacityRate_;
	std::vector<int> DRAW_FRAME = { 1,2,3,4,5,6,7,8,27,28,30 };

	// 衝突判定
	void Collision(void);

	void RateFrameIds(const std::string& name);

	bool IsRateFrame(int frameIdx) const;
};

