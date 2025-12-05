#pragma once
#include <DxLib.h>
#include "ColliderBase.h"
class Transform;
class ColliderCapsule : public ColliderBase
{
public:
	// コンストラクタ
	ColliderCapsule(
		TAG tag, const Transform* follow,
		const VECTOR& localPosTop, const VECTOR& localPosDown, float radius);
	// デストラクタ
	~ColliderCapsule(void);
	// 親Transformからの相対位置を取得
	const VECTOR& GetLocalPosTop(void) const { return localPosTop_; }
	const VECTOR& GetLocalPosDown(void) const { return localPosDown_; }
	// 親Transformからの相対位置をセット
	void SetLocalPosTop(const VECTOR& pos) { localPosTop_ = pos; }
	void SetLocalPosDown(const VECTOR& pos) { localPosDown_ = pos; }
	// ワールド座標を取得
	VECTOR GetPosTop(void) const { return GetRotPos(localPosTop_); }
	VECTOR GetPosDown(void) const { return GetRotPos(localPosDown_); }
	// 半径
	float GetRadius(void) const { return radius_; }
	void SetRadius(float radius) { radius_ = radius; }
	// 高さ
	float GetHeight(void) const { return localPosTop_.y; }
	// カプセルの中心座標
	VECTOR GetCenter(void) const;
protected:
	// デバッグ用描画
	void DrawDebug(int color) override;
private:
	// 親Transformからの相対位置(上側)
	VECTOR localPosTop_;
	// 親Transformからの相対位置(下側)
	VECTOR localPosDown_;
	// 半径
	float radius_;
};
