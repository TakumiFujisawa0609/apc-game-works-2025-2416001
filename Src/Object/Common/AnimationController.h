#pragma once
#include <string>
#include <map>
class SceneManager;

class AnimationController
{
	
public :

	// アニメーションデータ
	struct Animation
	{
		int model = -1;
		int attachNo = -1;
		int animIndex = 0;
		float speed = 0.0f;
		float totalTime = 0.0f;
		float step = 0.0f;
	};


	// コンストラクタ
	AnimationController(int modelId);
	// デストラクタ
	~AnimationController(void);

	// アニメーション追加
	void Add(int type, const std::string& path, float speed);

	void Add(int type, const float speed,int modelId = -1);

	/// <summary>
	/// アニメーションプレイ
	/// </summary>
	/// <param name="type">アニメーション番号</param>
	/// <param name="isLoop">ループするか</param>
	/// <param name="startStep">再生開始フレーム</param>
	/// <param name="endStep">再生終了フレーム</param>
	/// <param name="isStop">アニメーションを止める</param>
	/// <param name="isForce">同じアニメーションを再生したい場合はtrue</param>
	void Play(int type, bool isLoop = true, 
		float startStep = 0.0f, float endStep = -1.0f, bool isStop = false, bool isForce = false);

	void Update(void);

	// アニメーション終了後に繰り返すループステップ
	void SetEndLoop(float startStep, float endStep, float speed);

	// 再生中のアニメーション
	int GetPlayType(void) const;

	//アニメーションステップゲッタ
	const float GetAnimStep(void)const;

	// 再生終了
	bool IsEnd(void) const;

private :

	// モデルのハンドルID
	int modelId_;

	// 種類別のアニメーションデータ
	std::map<int, Animation> animations_;

	int playType_;
	Animation playAnim_;

	// アニメーションをループするかしないか
	bool isLoop_;

	// アニメーションを止めたままにする
	bool isStop_;

	// アニメーション終了後に繰り返すループステップ
	float stepEndLoopStart_;
	float stepEndLoopEnd_;
	float endLoopSpeed_;

	// 逆再生
	float switchLoopReverse_;

};

