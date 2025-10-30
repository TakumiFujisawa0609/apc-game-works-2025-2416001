#include "Collider.h"

Collider* Collider::instance_ = nullptr;

//インスタンスの生成
void Collider::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new Collider();
	}
}

//インスタンスの取得
Collider& Collider::GetInstance(void)
{
	return *instance_;
}

void Collider::Init(void)
{

}

bool Collider::IsHitSpheres(const VECTOR& pos1, float radius1, const VECTOR& pos2, float radius2)
{
    // 球体同士の衝突判定
    bool ret = false;

    // お互いの半径の合計
    float radius = radius1 + radius2;

    // 座標の差からお互いの距離を取る
    VECTOR diff = VSub(pos2, pos1);

    // 三平方の定理で比較(SqrMagnitudeと同じ)
    float dis = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);
    if (dis < (radius * radius))
    {
        ret = true;
    }

    return ret;
}

bool Collider::IsHitSphereCapsule(const VECTOR& sphPos, float sphRadius, const VECTOR& capPos1, const VECTOR& capPos2, float capRadius)
{
    bool ret = false;

    // カプセル球体の中心を繋ぐベクトル
    VECTOR cap1to2 = VSub(capPos2, capPos1);

    // ベクトルを正規化
    VECTOR cap1to2ENor = VNorm(cap1to2);

    // カプセル繋ぎの単位ベクトルと、
    // そのベクトル元から球体へのベクトルの内積を取る
    float dot = VDot(cap1to2ENor, VSub(sphPos, capPos1));

    // 内積で求めた射影距離を使って、カプセル繋ぎ上の座標を取る
    VECTOR capRidePos = VAdd(capPos1, VScale(cap1to2ENor, dot));

    // カプセル繋ぎのベクトルの長さを取る
    float len = VSize(cap1to2);

    // 球体がカプセル繋ぎ上にいるか判別するため、比率を取る
    float rate = dot / len;

    VECTOR centerPos;

    // 球体の位置が３エリアに分割されたカプセル形状のどこにいるか判別
    if (rate > 0.0f && rate <= 1.0f)
    {
        // ①球体がカプセル繋ぎ上にいる
        centerPos = VAdd(capPos1, VScale(cap1to2ENor, dot));
    }
    else if (rate > 1.0f)
    {
        // ②球体がカプセルの終点側にいる
        centerPos = capPos2;
    }
    else if (rate < 0.0f)
    {
        // ③球体がカプセルの始点側にいる
        centerPos = capPos1;
    }
    else
    {
        // ここにきてはいけない
    }

    // 球体同士の当たり判定
    if (IsHitSpheres(centerPos, capRadius, sphPos, sphRadius))
    {
        ret = true;
    }
    else
    {
        ret = false;
    }

    return ret;
}

bool Collider::IsHitMeshSphere(int modelId, const VECTOR& sphPos, float sphRadius, VECTOR* hitPos, VECTOR* hitNor)
{
    // 衝突判定
    MV1_COLL_RESULT_POLY_DIM hitResult = MV1CollCheck_Sphere(modelId, -1, sphPos, sphRadius);


    if (hitResult.HitNum > 0)
    {
        // 最も近い衝突地点を探す
        float minDist = FLT_MAX;

        int nearIndex = 0;

        for (int i = 0; i < hitResult.HitNum; i++)
        {
            VECTOR hitPos = hitResult.Dim[i].HitPosition;

            float dist = static_cast<float>(VSize(VSub(hitPos, sphPos)));

            if (dist < minDist)
            {
                minDist = dist;

                nearIndex = i;
            }
        }

        if (hitPos)
        {
            *hitPos = hitResult.Dim[nearIndex].HitPosition;
        }

        if (hitNor)
        {
            VECTOR v1 = hitResult.Dim[nearIndex].Position[0];

            VECTOR v2 = hitResult.Dim[nearIndex].Position[1];

            VECTOR v3 = hitResult.Dim[nearIndex].Position[2];

            VECTOR edge1 = VSub(v2, v1);

            VECTOR edge2 = VSub(v3, v1);

            *hitNor = VNorm(VCross(edge1, edge2));
        }

        MV1CollResultPolyDimTerminate(hitResult);

        return true;
    }
	return false;
}
