#pragma once
#include<vector>
#include <string>
#include"ColliderBase.h"

class Model : public ColliderBase
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pos">追従する親の座標</param>
	/// <param name="_rot">追従する親の回転</param>
	/// <param name="_modelId">追従する親のモデルID</param>
	Model(TAG tag, const Transform* follow);
	//デストラクタ
	~Model(void)override;

	// 指定された文字を含むフレームを衝突判定から除外
	void AddExcludeFrameIds(const std::string& name);

	// 衝突判定から除外するフレームをクリアする
	void ClearExcludeFrame(void);

	// 除外フレーム判定
	bool IsExcludeFrame(int frameIdx) const;

private:

	// 衝突判定から除外するフレーム番号
	std::vector<int> excludeFrameIds_;

	// デバッグ用描画
	void DrawDebug(int color) override {};
};
