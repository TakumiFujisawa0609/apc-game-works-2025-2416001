#pragma once

#include <string>
#include <vector>
class DataBank
{
public:	
	/// <summary>
	/// 明示的にインステンスを生成する
	/// </summary>
	/// <param name=""></param>
	static void CreateInstance(void);

	/// <summary>
	/// 静的インスタンスの取得
	/// </summary>
	/// <param name=""></param>
	/// <returns>インスタンスを返す</returns>
	static DataBank& GetInstance(void);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name=""></param>
	void Init(void);

	/// <summary>
	/// 解放処理
	/// </summary>
	/// <param name=""></param>
	void Destroy(void);

private:

	static DataBank* instance_;
	DataBank(void);
	DataBank(const DataBank& instance_) = default;
	~DataBank(void);

};

