#pragma once

#include"Geometry.h"

class Capsule : public Geometry
{

public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pos">追従する親の座標</param>
	/// <param name="_rot">追従する親の回転</param>
	/// <param name="_localPosTop">上側の相対座標</param>
	/// <param name="_localPosDown">下側の相対座標</param>
	/// <param name="_radius">半径</param>
	Capsule(const VECTOR& _pos, const Quaternion& _rot, const VECTOR _localPosTop, const VECTOR _localPosDown, const float _radius);

	/// <summary>
	/// コピーコンストラクタ
	/// </summary>
	/// <param name="_copyBase">コピー元</param>
	/// <param name="_pos">追従する親の座標</param>
	/// <param name="_rot">追従する親の回転</param>
	Capsule(const Capsule& _copyBase, const VECTOR& _pos, const Quaternion& _rot);

	// デストラクタ
	~Capsule(void)override;

	// 描画
	void Draw(void)override;

	//各種当たり判定
	const bool IsHit(Geometry& _geometry) override;
	const bool IsHit(Model& _model) override;
	const bool IsHit(Cube& _cube) override;
	const bool IsHit(Sphere& _sphere) override;
	const bool IsHit(Capsule& _capsule) override;
	const bool IsHit(Line& _line) override;

	//ヒット後の処理
	void HitAfter(void)override;

	// 親Transformからの相対位置を取得
	inline const VECTOR GetLocalPosTop(void) const { return localPosTop_; }
	inline const VECTOR GetLocalPosDown(void) const { return localPosDown_; }

	// 親Transformからの相対位置をセット
	inline void SetLocalPosTop(const VECTOR& _pos) { localPosTop_ = _pos; }
	inline void SetLocalPosDown(const VECTOR& _pos) { localPosDown_ = _pos; }

	// ワールド座標を取得
	inline const VECTOR GetPosTop(void) const { return GetRotPos(localPosTop_); }
	inline const VECTOR GetPosDown(void) const { return GetRotPos(localPosDown_); }

	// 半径
	inline const float GetRadius(void) const { return radius_; }
	inline void SetRadius(float _radius) { radius_ = _radius; }

	// 高さ
	inline const float GetHeight(void) const { return localPosTop_.y; }

	// カプセルの中心座標
	inline const VECTOR GetCenter(void) const;

	//当たった時の情報取得
	inline const MV1_COLL_RESULT_POLY_DIM& GetHitInfo(void)const { return hitInfo_; }

	//当たった時の情報設定
	inline void SetHitInfo(MV1_COLL_RESULT_POLY_DIM _hitInfo) { std::swap(hitInfo_, _hitInfo); }

private:

	VECTOR localPosTop_;	// 親Transformからの相対位置(上側)
	VECTOR localPosDown_;	// 親Transformからの相対位置(下側)
	float radius_;			// 半径

	MV1_COLL_RESULT_POLY_DIM hitInfo_;	//当たった時の情報(球、カプセル)
};
