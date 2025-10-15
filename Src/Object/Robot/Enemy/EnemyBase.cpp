#include "../../../Utility/MatrixUtility.h"
#include "../../../Utility/AsoUtility.h"
#include "../../Manager/WeponManager.h"
#include "EnemyBase.h"

EnemyBase::EnemyBase(void)
{
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Init(void)
{
    // リソースロード
    InitLoad();
    // Transform初期化
    InitTransform();
    // 大きさ、回転、座標のモデル設定
    InitTransformPost();
    // アニメーションの初期化
    InitAnimation();
    // 初期化後の個別処理
    InitPost();

    SetSpawnPostiton();

    MV1SetPosition(trans_.modelId, trans_.pos);
    MV1SetRotationMatrix(trans_.modelId,
        MatrixUtility::Multiplication(trans_.localRot, trans_.rot));

    for (int i = 0; i < 10; i++)
    {
        useWepon_.push_back(std::make_shared<WeponManager>());
    }
    for (int i = 0; i < useWepon_.size(); i++)
    {
        useWepon_[i]->Init();
    }

    //状態遷移初期設定
    ChangeState(STATE::STANDBY);
}

void EnemyBase::SetSpawnPostiton(void)
{
    //出現半径
    float radius = SPAWN_RANIUS;
    //出現中心座標
    VECTOR spawnPos = AsoUtility::VECTOR_ZERO;
    //長さを設定
    radius *= spawnRange_;

    //ランダムな数値を取得する(0～200)
    int degree = GetRand(360);

    float radian =
        static_cast<float>(degree) * DX_PI_F / 180.0f;

    //角度から方向を求める
    float dirX = sinf(radian);
    float dirZ = cosf(radian);
    VECTOR dir = { dirX, 0.0f, dirZ };

    //方向X長さ（長さが半径分のランダムなベクトル）
    VECTOR vec = VScale(dir, radius);

    //ステージの中心座標にベクトルを加算して
    //円状の座標を計算する
    VECTOR pos = VAdd(spawnPos, vec);

    trans_.pos = pos;
}
